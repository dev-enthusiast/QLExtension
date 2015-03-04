#define GSCVSID "$Header: /home/cvs/src/secdb/src/dtsec.c,v 1.47 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	DTSEC.C		- Security DataType
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: dtsec.c,v $
**	Revision 1.47  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.46  2011/11/15 22:11:25  khodod
**	Make sure to minimize the impact of the new test.
**	AWR: #177526
**
**	Revision 1.45  2011/11/02 18:09:13  c07426
**	Check b->DataType for NULL in DtFuncSecurity while reporting unsupported operations, to avoid crashes.
**	AWR: #177526
**
**	Revision 1.44  2004/01/30 23:38:18  khodod
**	Fixed the comparators for large pointer values.
**
**	Revision 1.43  2001/11/27 23:23:33  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.41  2001/10/22 12:19:35  goodfj
**	Trap NULL pointers passed into SecDbHashInStreams
**	
**	Revision 1.40  2001/08/22 20:30:20  vengrd
**	Got rid of an extra little class that arrowkludge eliminates the need for.
**	
**	Revision 1.39  2001/08/20 16:29:46  vengrd
**	Avoid infinite recursion when hashing security cycles.
**	
**	Revision 1.38  2001/07/25 20:48:30  nauntm
**	added a compare operator for securities
**	
**	Revision 1.37  2001/07/19 18:57:39  vengrd
**	Has all instreams of a security by default.  Set SECDB_HASH_SECURITIES_BY_NAME for old behavior.
**	
**	Revision 1.36  2001/06/01 00:24:46  vengrd
**	Make HASH_PORTABLE work for Security Lists and Securities.
**	
**	Revision 1.35  2001/05/31 23:29:21  vengrd
**	Hash the instreams of a security to do a portable hash.  Protected by env var.  Default is old behaviour.
**	
**	Revision 1.34  2001/03/01 23:14:39  nauntm
**	fixed recursive streaming bug
**	fixed null ptr streaming bug
**	better error msgs
**	
**	Revision 1.33  2001/02/21 01:40:30  nauntm
**	streaming now allows instream addition even on inline objects
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include    <bstream.h>
#include    <datatype.h>
#include	<secdb.h>
#include    <secobj.h>
#include	<secdt.h>
#include	<secerror.h>
#include	<dtformat.h>
#include    <hash.h>
#include	<sdb_int.h>
#include    <sectrace.h>
#include	<set>

/*
**	Variables
*/

int		SecDbStreamByNameDefault = FALSE;


/****************************************************************
** Class	   : SecDbObjectBeingHashed 
** Description : A simple class to maintain a static set of
**				 objects being hashed, so we don't go into
**				 and infinite loop hashing secs with instream
**				 cycles.  Constructor adds the obj to the set
**               being hashed, dtor takes it out.  On return
**               of the outermost SecDbHashInstreams, the set
**               should again be empty.
****************************************************************/

struct sdb_object_less {
	bool operator () ( SDB_OBJECT const * const p, SDB_OBJECT const * const q ) const 
	{
		int r;
		if( ( r = stricmp( p->SecData.Name, q->SecData.Name ) ) ) 
			return r < 0;
		
		SDB_DB::db_less less;
		return less( p->Db, q->Db ); // OK just fix the forward declaration
	}
};

typedef CC_STL_SET_WITH_COMP( const SDB_OBJECT *, sdb_object_less ) SdbObjectSet;

class SecDbObjectBeingHashed
{
public:

	SecDbObjectBeingHashed( const SDB_OBJECT * SdbObject ) 
	:
		m_SdbObject( SdbObject )
	{ 
		s_AllObjectsBeingHashed.insert( SdbObject ); 
	} 

	~SecDbObjectBeingHashed()
	{
		s_AllObjectsBeingHashed.erase( m_SdbObject );
	}

	static bool BeingHashed( const SDB_OBJECT * SdbObject )
	{
		return ( s_AllObjectsBeingHashed.find( SdbObject ) != s_AllObjectsBeingHashed.end() );
	}

private:
	const SDB_OBJECT
			*m_SdbObject;
			
	static SdbObjectSet
		s_AllObjectsBeingHashed;
};

SdbObjectSet
		SecDbObjectBeingHashed::s_AllObjectsBeingHashed;



/****************************************************************
**	Routine: SecDbHashInStreams
**	Returns: A hash value for all in-stream values
**	Action : Hash all the instreams of a sec together.
**			 There are misc functions in gob that sort of
**			 do this, but they are geared towards value
**			 function use.  It's scary to touch them, as
**			 the sky could fall rapidly.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbHashInStreams"

int SecDbHashInStreams(
	SDB_OBJECT		*SecPtr,
	DT_HASH_CODE	*Hash
)
{
	if( !SecPtr )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": SecPtr is NULL" );
	if( !Hash )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Hash is NULL" );

	// Protect from cycles by making sure the object is not
	// currently being hashed.

	if( !SecDbObjectBeingHashed::BeingHashed( SecPtr ) )
	{
		SecDbObjectBeingHashed
				ObjectBeingHashed( SecPtr );

		SDB_VALUE_TYPE_INFO
				*Vti,
				*ValueTable;

		ValueTable = SecPtr->Class->ValueTable;

		for( Vti = ValueTable; Vti->Name; Vti++ )
			if( ( Vti->ValueFlags & SDB_IN_STREAM ) && !( Vti->ValueFlags & GOB_MUSH_IGNORE_NEW ) )
			{
				DT_VALUE
						*Value = SecDbGetValue( SecPtr, Vti->ValueType );

				if( !Value )
					return ErrMore( ARGCHECK_FUNCTION_NAME ": Unable to get instream value '%s' to hash.", Vti->Name );

				if( !DtHashPortable( Value, Hash ) )
					return ErrMore( ARGCHECK_FUNCTION_NAME ": Unable to hash instream value '%s'.", Vti->Name );
			}
	}

	return TRUE;
}



/****************************************************************
**	Routine: DtFuncSecurity
**	Returns: TRUE	- Operation worked
**			 FALSE	- Operation failed
**	Action : Handle operations for security data type
****************************************************************/

int DtFuncSecurity(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	DT_INFO	*DataTypeInfo;

	SDB_OBJECT
			*SecPtr;

	static HASH
			*SecRefHash=NULL;

	static int
			RefNumber = 1;

	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size	= sizeof( a->Data.Pointer );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_REFERENCE | DT_FLAG_SDB_OBJECT;
			return TRUE;

		case DT_MSG_ALLOCATE:
			r->Data.Pointer = NULL;
			return TRUE;

		case DT_MSG_FREE:
			if( a->Data.Pointer )
			{
				SecDbFree( (SDB_OBJECT *) a->Data.Pointer );
				a->Data.Pointer = NULL;
			}
			return TRUE;

		case DT_MSG_ASSIGN:
			SecPtr = (SDB_OBJECT *) a->Data.Pointer;
			if( SecPtr )
				SecDbIncrementReference( SecPtr );
			*r = *a;
			return TRUE;

		case DT_MSG_TO:
			SecPtr = (SDB_OBJECT *) a->Data.Pointer;
			if( r->DataType == a->DataType )
				r->Data.Pointer = SecDbIncrementReference( (SDB_OBJECT *) a->Data.Pointer );
			else if( r->DataType == DtString )
			{
				if( SecPtr )
					r->Data.Pointer = strdup( SecPtr->SecData.Name );
				else
					r->Data.Pointer = strdup( "Undefined" );
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", a->DataType->Name, r->DataType->Name );
			return TRUE;

		case DT_MSG_FORMAT:
		{
			DT_VALUE
					TmpValue;

			TmpValue.DataType = DtString;
			TmpValue.Data.Pointer = a->Data.Pointer ? ((SDB_OBJECT *) a->Data.Pointer)->SecData.Name : "Undefined";
			return DT_OP( Msg, r, &TmpValue, b );
		}

		case DT_MSG_FROM:
			if( b->DataType == a->DataType )
				a->Data.Pointer = SecDbIncrementReference( (SDB_OBJECT *) b->Data.Pointer );
			else if( b->DataType == DtString )
			{
				if( !( a->Data.Pointer = SecDbGetByName( (char *) b->Data.Pointer, SecDbRootDb, 0 ) ) )
  					return ErrMore( "Failed to load %s", (char *) DT_VALUE_TO_POINTER( b ) );
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", b->DataType->Name, a->DataType->Name );
			return TRUE;
			
		case DT_MSG_COMPONENT_VALUE:	// r = a.b  Only supported for .ReferenceCount and .Pointer!
		case DT_MSG_SUBSCRIPT_VALUE:	// r = a[ b ]
			if( b->DataType != DtString )
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript/Component must be a String, not a %s", b->DataType->Name );

			SecPtr = (SDB_OBJECT *) a->Data.Pointer;

			if( !stricmp( (char *) b->Data.Pointer, "ReferenceCount" ))
				r->Data.Number = SecPtr->ReferenceCount;
			else if( !stricmp( (char *) b->Data.Pointer, "Pointer" ))
				r->Data.Number = (double)((long)SecPtr);
			else
				return Err( ERR_INVALID_ARGUMENTS, "@ - only ReferenceCount and Pointer components allowed" );

			r->DataType = DtDouble;
			return TRUE;

		case DT_MSG_HASH_PORTABLE:
		{
			static bool
					HashAllInstreams = ( NULL == SecDbConfigurationGet( "SECDB_HASH_SECURITIES_BY_NAME", NULL, NULL, NULL ) );

			if( HashAllInstreams )
			{
				// Hash all the instreams.
				
				DT_HASH_CODE
						Hash = *(DT_HASH_CODE *)DT_VALUE_TO_POINTER( r );

				if( !SecDbHashInStreams( (SDB_OBJECT *)DT_VALUE_TO_POINTER( a ), &Hash ) )
					return ErrMore( "DT_MSG_HASH_PORTABLE failed for Security" );

				*(DT_HASH_CODE *)DT_VALUE_TO_POINTER( r ) = Hash;

				return TRUE;
			}
			else
			{
				DT_VALUE	StrValue;

				SecPtr = (SDB_OBJECT *) a->Data.Pointer;
				DTM_INIT( &StrValue );
				StrValue.DataType = DtString;
				StrValue.Data.Pointer = SecPtr->SecData.Name;
				return DT_OP( DT_MSG_HASH_PORTABLE, r, &StrValue, NULL );
			} 
		}

		case DT_MSG_HASH_QUICK:
		{
			unsigned long
					Hash1 = (unsigned long) a->Data.Pointer,
					Hash2 = (unsigned long) DtSecurity,
					Hash3 = *(DT_HASH_CODE *) r->Data.Pointer;

			DT_HASH_MIX( Hash1, Hash2, Hash3 );
			*(DT_HASH_CODE *) r->Data.Pointer = Hash3;
			return TRUE;
		}

		case DT_MSG_EQUAL:
		case DT_MSG_NOT_EQUAL:
			r->Data.Number = ( a->DataType == b->DataType && a->Data.Pointer == b->Data.Pointer );
			r->DataType = DtDouble;
			if( Msg == DT_MSG_NOT_EQUAL )
				r->Data.Number = !r->Data.Number;
			return TRUE;

		case DT_MSG_TRUTH:
			return a->Data.Pointer ? TRUE : FALSE;

		case DT_MSG_SIZE:
			r->DataType 	= DtDouble;
			r->Data.Number	= 1.0;
			return TRUE;

		case DT_MSG_MEMSIZE:
			r->DataType 	= DtDouble;
			r->Data.Number	= 0.0;
			return TRUE;

		case DT_MSG_INFO:
			DataTypeInfo = (DT_INFO *) calloc( 1, sizeof( DT_INFO ));
			DataTypeInfo->Flags = DT_FLAG_POINTER;
			r->Data.Pointer = DataTypeInfo;
			return TRUE;

		case DT_MSG_COMPARE			:
			if ( a->DataType != b->DataType )
				return( Err( ERR_UNSUPPORTED_OPERATION, "@ - %s between %s and %s", 
							 DtMsgToString( Msg ), a->DataType->Name, 
							 b->DataType->Name ));
			if( !a->Data.Pointer || !b->Data.Pointer )
			{
				r->Data.Number = ( !a->Data.Pointer - !b->Data.Pointer );
				r->DataType = DtDouble;
				return TRUE;
			}
			r->DataType = DtDouble;
			r->Data.Number = (double) stricmp( 
				((SDB_OBJECT*)a->Data.Pointer)->SecData.Name,
				((SDB_OBJECT*)b->Data.Pointer)->SecData.Name );
			return TRUE;

		case DT_MSG_TO_STREAM:
			SecPtr = (SDB_OBJECT *) a->Data.Pointer;

			{
				BSTREAM *BStream = (BSTREAM*) r->Data.Pointer;
				if ( SecPtr == NULL )
				{
					BStreamPutByte( BStream, '0' );
					return TRUE;
				}

				static SDB_VALUE_TYPE 
						VTStreamByName = NULL;
				DT_VALUE 
						*ByName;
				int		StreamByName;

				if ( NULL == VTStreamByName )
					VTStreamByName = SecDbValueTypeFromName( "Stream By Name", DtDouble );
				
				ByName = SecDbGetValue( SecPtr, VTStreamByName );
				StreamByName = ByName ? (int) ByName->Data.Number : SecDbStreamByNameDefault ;

				if ( StreamByName )
				{
					int 	Ret;
					HASH    *Save    = SecRefHash;
					int     RefSave = RefNumber;
					
					SecRefHash = NULL;
					RefNumber  = 1;

		            SecDbIncrementReference( SecPtr );
					Ret = SecDbGetByInference( &SecPtr, SDB_GET_CREATE );

					if ( !Ret )
						BStreamInvalidate( BStream );
					else 
					{
						BStreamPutByte( BStream, 'N' );
						BStreamPutString( BStream, SecPtr->SecData.Name );
					}
					SecDbFree( SecPtr );

					SecRefHash = Save;
					RefNumber  = RefSave;

					return( Ret );
				}
				else
				{
					int     CreateHash = SecRefHash==NULL;
					HASH_KEY    
							Key = SecPtr;
					unsigned long
							Value;

					if ( CreateHash )
						SecRefHash = HashCreate( "SecRefHash", NULL, NULL, 0, NULL );

					Value = (unsigned long) HashLookup( SecRefHash, Key );

					if ( Value!=0 )
					{
						BStreamPutByte( BStream, 'R' );
						BStreamPutInt32( BStream, (INT32) Value );
					}
					else
					{
						SDB_M_DATA
								SdbMsgData,
								TempMsg;

						SDB_VALUE_TYPE_INFO
								*ValueTable,
								*Value;
						SDB_DISK_VERSION
								*pVersion;

						BSTREAM 
								SubObjectBStream;
						int
								SubObjectSize;

						HashInsert(	SecRefHash, (HASH_KEY) SecPtr, (HASH_VALUE) RefNumber );
						RefNumber++;
						BStreamPutByte( BStream, 'S' );
						BStreamPutInt16( BStream, (INT16) SecPtr->Class->Type );

						BStreamAutoGrowInit( &SubObjectBStream );
						SubObjectBStream.FreeData		= TRUE;

						memset( &SdbMsgData, 0, sizeof( SdbMsgData ));
						M_SETUP_VERSION( &SdbMsgData.Add.Version, SecPtr, SecPtr->Db );
						SdbMsgData.Add.Stream = &SubObjectBStream;

						pVersion = &SdbMsgData.Add.Version;

						ValueTable = SecPtr->Class->ValueTable;
						for( Value = ValueTable; Value->Name; Value++ )
							if( !SDB_MESSAGE_VT_TRACE( SecPtr, SDB_MSG_ADD, &SdbMsgData, Value ))
								return FALSE;

						TempMsg.Mem.Stream	= &SubObjectBStream;
						TempMsg.Mem.Version	= *pVersion;
						if( !SDB_MESSAGE( SecPtr, SDB_MSG_MEM, &TempMsg ))
						{
							BStreamInvalidate( BStream );
							return FALSE;
						}
						*pVersion = TempMsg.Mem.Version;

                        SubObjectSize = BStreamDataUsed( &SubObjectBStream );
                        BStreamRewind( &SubObjectBStream );
						BStreamPutInt32(  BStream, SubObjectSize );
						BStreamPutMemory( BStream, BStreamTell( &SubObjectBStream ), SubObjectSize );
						BStreamDestroy( &SubObjectBStream );
					}
					if ( CreateHash )
					{
						HashDestroy( SecRefHash );
						SecRefHash = NULL;
						RefNumber = 1;
					}
					return TRUE;
				}
			}
			return FALSE;

		case DT_MSG_FROM_STREAM:
		{
			BSTREAM 
					*BStream = (BSTREAM*) r->Data.Pointer;
			int     Type = BStreamGetByte( BStream );

			SDB_DB	*Db;

			if( !( Db = (SDB_DB*) BStream->UserData ))
				return Err( SDB_ERR_OBJECT_INVALID, "@ - Incorrect db???" );
			
			switch( Type )
			{
				case '0':
					a->Data.Pointer = NULL;
					a->DataType = DtSecurity;
					return TRUE;
				case 'N':
				{
					char 	SecName[SDB_SEC_NAME_SIZE];
					HASH    *Save    = SecRefHash;
					int     RefSave = RefNumber;
					
					SecRefHash = NULL;
					RefNumber  = 1;

					BStreamGetString( BStream, SecName, SDB_SEC_NAME_SIZE );
					a->Data.Pointer = SecDbGetByName( SecName, Db, 0 );
					a->DataType = DtSecurity;
					
					if ( NULL == a->Data.Pointer )
						return FALSE;

					SecRefHash = Save;
					RefNumber  = RefSave;

					return TRUE;
				}
				case 'R':
				{
					int     RefNum = (int) BStreamGetInt32( BStream );
					SDB_OBJECT 
							*SecPtr = (SDB_OBJECT*) HashLookup( SecRefHash, (HASH_KEY)RefNum );

					if ( SecPtr==NULL )
						return Err( SDB_ERR_OBJECT_INVALID, "@ - sec stream ref error (%d)", RefNum );

					SecDbIncrementReference( SecPtr );
					a->Data.Pointer = SecPtr;
					a->DataType = DtSecurity;
					return TRUE;
				}
				case 'C':
				{
					SDB_M_DATA
							MsgData;
					SDB_SEC_TYPE 
							Type;
					SDB_OBJECT 
							*SecPtr;

					int     CreateHash = SecRefHash==NULL,
							Ret = TRUE;

					if ( CreateHash )
						SecRefHash = HashCreate( "SecRefHash", NULL, NULL, 0, NULL );
					
					Type = BStreamGetInt16( BStream );
					SecPtr = SecDbNew( NULL, Type, Db );

					if ( NULL == SecPtr )
						return FALSE;
					HashInsert(	SecRefHash, (HASH_KEY) RefNumber, (HASH_VALUE) SecPtr );
					RefNumber++;

					MsgData.Load.Reload			= FALSE;
					MsgData.Load.Changed		= TRUE;
					MsgData.Load.Stream			= BStream;
					MsgData.Load.CurrentVersion	= SecPtr->Class->ObjectVersion;
					SecDbDiskInfoGetVersion( &SecPtr->SecData, &MsgData.Load.Version );

					if( MsgData.Load.Version.Object != SecPtr->Class->ObjectVersion )
						Ret = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD_VERSION, &MsgData );
					else
						Ret = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD, &MsgData );

					if ( CreateHash )
					{
						HashDestroy( SecRefHash );
						SecRefHash = NULL;
						RefNumber = 1;
					}


					a->Data.Pointer = SecPtr;
					a->DataType = DtSecurity;

					return Ret;
				}
				case 'S':
				{
					SDB_M_DATA
							MsgData;
					SDB_SEC_TYPE 
							Type;
					SDB_OBJECT 
							*SecPtr;
					BSTREAM 
							SubObjectBStream;
					int
							SubObjectSize;
					unsigned char 
							*SubObjectMem;


					int     CreateHash = SecRefHash==NULL,
							Ret = TRUE;

					if ( CreateHash )
						SecRefHash = HashCreate( "SecRefHash", NULL, NULL, 0, NULL );
					
					Type = BStreamGetInt16( BStream );
					SecPtr = SecDbNew( NULL, Type, Db );

					if ( NULL == SecPtr )
						return FALSE;
					HashInsert(	SecRefHash, (HASH_KEY) RefNumber, (HASH_VALUE) SecPtr );
					RefNumber++;

					SubObjectSize = BStreamGetInt32(  BStream );
					SubObjectMem = (unsigned char*) malloc( SubObjectSize );
					BStreamGetMemory( BStream, SubObjectMem, SubObjectSize );
					BStreamInit( &SubObjectBStream, SubObjectMem, SubObjectSize );
					SubObjectBStream.UserData = BStream->UserData;

					MsgData.Load.Reload			= FALSE;
					MsgData.Load.Changed		= TRUE;
					MsgData.Load.Stream			= &SubObjectBStream;
					MsgData.Load.CurrentVersion	= SecPtr->Class->ObjectVersion;
					SecDbDiskInfoGetVersion( &SecPtr->SecData, &MsgData.Load.Version );

					if( MsgData.Load.Version.Object != SecPtr->Class->ObjectVersion )
						Ret = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD_VERSION, &MsgData );
					else
						Ret = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD, &MsgData );

					BStreamDestroy( &SubObjectBStream );
					free( SubObjectMem );

					if ( CreateHash )
					{
						HashDestroy( SecRefHash );
						SecRefHash = NULL;
						RefNumber = 1;
					}


					a->Data.Pointer = SecPtr;
					a->DataType = DtSecurity;

					return Ret;
				}
				default:
					return Err( SDB_ERR_OBJECT_INVALID, "@ - unknown security stream type '%c'", Type );
					
			}
		}
	}


/*
**	Unsupported operation
*/

	if( b )
		Err( ERR_UNSUPPORTED_OPERATION, "@ - %s between %s and %s", 
                DtMsgToString( Msg ), 
                a->DataType->Name, 
                ( b->DataType ? b->DataType->Name : "<null>" ) );
	else if( a )
		Err( ERR_UNSUPPORTED_OPERATION, "@ - %s on %s", DtMsgToString( Msg ), a->DataType->Name );
	else
		Err( ERR_UNSUPPORTED_OPERATION, "@ - %s", DtMsgToString( Msg ) );

	return FALSE;
}

