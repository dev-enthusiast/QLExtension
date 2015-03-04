#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_of.c,v 1.75 2001/04/20 16:06:45 goodfj Exp $"
/****************************************************************
**
**	SDB_OF.C	- Security database security object functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_of.c,v $
**	Revision 1.75  2001/04/20 16:06:45  goodfj
**	Insist that class IDs in sectypes.dat increase monotonically
**
**	Revision 1.74  2001/04/20 15:37:24  goodfj
**	Use ARGCHECK_FUNCTION_NAME
**	
**	Revision 1.73  2001/03/23 23:26:39  singhki
**	Make ValueTypeMap just an STL map.
**	
**	Revision 1.72  2001/02/22 12:32:21  shahia
**	changed ValueTypeMap implementation to use STL map
**	
**	Revision 1.71  2001/01/19 19:15:54  shahia
**	Got rid of use of SDB_NODE_CONTEXTs Clear method, OK to PRODVER
**	
**	Revision 1.70  2000/12/28 21:27:45  singhki
**	Use an STL map for ImplementedInterfaces. Nicer than a HASH
**	
**	Revision 1.69  2000/12/19 23:36:27  simpsk
**	More cleanups.  Also added loop for autochildren for the dbchanged case.
**	
**	Revision 1.68  2000/12/19 17:20:44  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.67  2000/06/05 14:56:17  singhki
**	Allow comment to start anywhere
**	
**	Revision 1.66  2000/04/24 11:04:24  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.65  2000/04/10 20:33:10  fradkv
**	SecDb Base Interface is now automatically inherited by any class.
**	The interface mirrows SecDb Base Class
**	
**	Revision 1.64  2000/03/30 22:38:41  fradkv
**	Interfaces and abstract classes are introduced
**	
**	Revision 1.63  2000/02/26 00:15:07  singhki
**	Add SecDbClassMarkAsNotStarted for OS/2
**	
**	Revision 1.62  2000/02/08 04:19:12  singhki
**	unmask real error by putting ERR_OFF/ON around bogus errors from MarkAsNotStarted
**	
**	Revision 1.61  2000/02/01 22:54:18  singhki
**	Replace ValueTypeMap in SecPtr with the SDB_CLASS_INFO for the class
**	from which we should get VTIs. Class still points to the original
**	SDB_CLASS_INFO to be used for the ObjectFunc
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<errno.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<malloc.h>
#include	<ctype.h>
#include	<kool_ade.h>
#include	<mempool.h>
#include	<hash.h>
#include	<dynalink.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secerror.h>
#include	<secindex.h>
#include	<sectrace.h>
#include	<sdb_int.h>
#include	<secnode.h>
#include	<secnodei.h>


/*
**	How many security types are there?  Just a guess to initialize
**	the Hash tables
*/

#define SDB_SEC_TYPE_COUNT	1000

#define	CLASS_MESSAGE( Class_, Msg_, Data_)	( SecDbTraceFunc ? ClassMessageTrace( (Class_), (Msg_), (Data_) ) : (*(Class_)->FuncPtr)( NULL, (Msg_), (Data_) ))


/*
**	Private variables
*/

static int
		DefaultMaxUnparentedNodes = 150,
		DefaultGCBlockSize = 50;



/****************************************************************
**	Routine: ClassMessageTrace
**	Returns: TRUE or FALSE
**	Action : Calls SecDbMessageTrace with dummy SecPtr pointing to
**			 given class; for message like SDB_MSG_START that do
**			 not have a SecPtr
****************************************************************/

static int ClassMessageTrace(
	SDB_CLASS_INFO	*Class,
	int				Msg,
	SDB_M_DATA		*MsgData
)
{
	SDB_OBJECT
			Sec;

	Sec.Class = Class;
	strcpy( Sec.SecData.Name, Class->Name );
	return( SecDbMessageTrace( &Sec, Msg, MsgData ));
}



/****************************************************************
**	Routine: SecDbLoadObjTable
**	Returns: TRUE	- Object table loaded without error
**			 FALSE	- Error loading security object table
**	Action : Load a security object table from an ASCII file
**			 into a hash table.  The format for the ASCII file is:
**
**				SecurityType	- Numeric identifier for the type
**				DllPath			- Path and filename for the dll
**				FunctionName	- Name of the function in the dll
**				Name			- Name of the security type
**				Argument		- Optional argument used by the
**								  security class
**
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbLoadObjTable"

int SecDbLoadObjTable(
	const char	*Path,
	const char	*TableName
)
{
	char	Buffer[ 256 ],
			FileName[ FILENAME_MAX + 1 ],
			*Str;

	FILE	*fpTable;

	SDB_CLASS_INFO
			*SecClassPtr = NULL;

	int		Status = TRUE,
			LineNo = 0,
			ClassId,
			PrevClassId = 0,
			i;


/*
**	Open the ASCII file containing the table
*/

	if( !KASearchPath( Path, TableName, FileName, sizeof( FileName )))
		return FALSE;

	if( !(fpTable = fopen( FileName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ) %s", FileName, strerror( errno ));

	DefaultMaxUnparentedNodes = atoi( SecDbConfigurationGet( "SECDB_MAX_UNPARENTED_NODES", NULL, NULL, "150" ));
	DefaultGCBlockSize = atoi( SecDbConfigurationGet( "SECDB_NODE_GC_BLOCK_SIZE", NULL, NULL, "50" ));


/*
**	If the hash table hasn't been created, create it...
*/

	if( !SecDbSecTypeHash )
	{
		// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
		SecDbSecTypeHash = HashCreate( "Security Classes by Type", (HASH_FUNC) HashIntHash, NULL, SDB_SEC_TYPE_COUNT, NULL );
		SecDbSecDescHash = HashCreate( "Security Classes by Desc", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, SDB_SEC_TYPE_COUNT, NULL );
	}


/*
**	Read and parse the file
*/
	while( fgets( Buffer, sizeof( Buffer ), fpTable ))
	{
		++LineNo;

		// Security type number
		if( !(Str = strtok( Buffer, " \t\n" )))
			continue;		// allow blank lines

		if( Str[0] == '/' && Str[1] == '/' )
			continue;

		if( !( SecClassPtr = (SDB_CLASS_INFO *) calloc( 1, sizeof( SDB_CLASS_INFO ))))
			return Err( ERR_MEMORY, "@" );

		ClassId = atoi( Str );

		if( ClassId <= PrevClassId )
			return Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME ": Class IDs must increase monotonically in %s (found %d after %d", FileName, ClassId, PrevClassId );

		PrevClassId = ClassId;

		SecClassPtr->Type = (SDB_SEC_TYPE) ClassId;
		SecClassPtr->MaxUnparentedNodes = DefaultMaxUnparentedNodes;
		SecClassPtr->GCBlockSize = DefaultGCBlockSize;

		// Dll path
		if( !(Str = strtok( NULL, " \t\n" )))
		{
			Status = Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME ": incomplete line number %d in %s", LineNo, FileName );
			break;
		}
		strncpy( SecClassPtr->DllPath, Str, SDB_PATH_NAME_SIZE );
		SecClassPtr->DllPath[ SDB_PATH_NAME_SIZE-1 ] = '\0';

		// Function name
		if( !(Str = strtok( NULL, " \t\n\"" )))
		{
			Status = Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME ": incomplete line number %d in %s", LineNo, FileName );
			break;
		}
		strncpy( SecClassPtr->FuncName, Str, SDB_FUNC_NAME_SIZE );
		SecClassPtr->FuncName[ SDB_FUNC_NAME_SIZE-1 ] = '\0';

		// Name (optionally quoted region)
		if( !(Str = strtok( NULL, "\n\r" )))
		{
			Status = Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME ": incomplete line number %d in %s", LineNo, FileName );
			break;
		}
		while( ISSPACE( *Str ))
			++Str;
		if( *Str == '\0' )
		{
			Status = Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME ": incomplete line number %d in %s", LineNo, FileName );
			break;
		}
		if( *Str == '\"' )
		{
			++Str;
			for (i = 0; Str[ i ] != '\"' && Str[ i ] != '\0'; ++i);
			if (Str[ i ] == '\"')
				Str[ i++ ] = '\0';
			strncpy( SecClassPtr->Name, Str, SDB_CLASS_NAME_SIZE );
			Str += i;
		}
		else
		{
			for (i = 0; !isspace( Str[ i ] ) && Str[ i ] != '\0'; ++i );
			if (isspace( Str[ i ] ))
				Str[ i++ ] = '\0';
			strncpy( SecClassPtr->Name, Str, SDB_CLASS_NAME_SIZE );
			SecClassPtr->Name[ SDB_CLASS_NAME_SIZE-1 ] = '\0';
		}

		// Optional arg (optionally quoted region)
		while( ISSPACE( *Str ))
			++Str;
		if( *Str == '\"' )
		{
			++Str;
			for (i = 0; Str[ i ] != '\"' && Str[ i ] != '\0'; ++i);
			if (Str[ i ] == '\"')
				Str[ i++ ] = '\0';
			strncpy( SecClassPtr->Argument, Str, SDB_CLASS_ARG_SIZE );
			SecClassPtr->Argument[ SDB_CLASS_ARG_SIZE-1 ] = '\0';
		}
		else if (*Str != '\0')
		{
			strncpy( SecClassPtr->Argument, Str, SDB_CLASS_ARG_SIZE );
			SecClassPtr->Argument[ SDB_CLASS_ARG_SIZE-1 ] = '\0';
		}

		// Default MaxStreamSize
		SecClassPtr->MaxStreamSize = SDB_MAX_MEM_SIZE;

		// Add the class
		if( !HashInsert( SecDbSecTypeHash, (HASH_KEY) (long) SecClassPtr->Type, SecClassPtr ))
			free( SecClassPtr );
		else if( !HashInsert( SecDbSecDescHash, SecClassPtr->Name, SecClassPtr ))
			free( SecClassPtr );

		SecClassPtr = NULL;
	}

	if( SecClassPtr )
		free( SecClassPtr );


/*
**	Close the file
*/

	fclose( fpTable );
	return Status;
}



/****************************************************************
**	Routine: SecDbClassRegister
**	Returns: TRUE or FALSE
**	Action : Adds new class to the hash table.  Succeeds for
**			 for identical subsequent registrations.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassRegister"

int SecDbClassRegister(
	SDB_SEC_TYPE	Type,
	const char		*DllPath,
	const char		*FuncName,
	const char		*Name,
	const char		*Argument
)
{
	SDB_CLASS_INFO
			*SecClassPtr;


/*
**	Check length of args
*/

	if( strlen( DllPath ) + 1 > sizeof( SecClassPtr->DllPath ))
		return Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME "( %s ), DllPath too long: %s", Name, DllPath );
	if( strlen( FuncName ) + 1 > sizeof( SecClassPtr->FuncName ))
		return Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME "( %s ), FuncName too long: %s", Name, FuncName );
	if( strlen( Name ) + 1 > sizeof( SecClassPtr->Name ))
		return Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME "( %s ), Name too long: %s", Name, Name );
	if( strlen( Argument ) + 1 > sizeof( SecClassPtr->Argument ))
		return Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME "( %s ), Argument too long: %s", Name, Argument );


/*
**	Check class does not start with UFO
*/

	if( 0 == strnicmp( Name, "UFO", 3 ) )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ), Name must not start UFO: %s", Name, Name );

/*
**	See if class is already registered.  This is ok if DllPath,
**	FuncName, and Name are the same.  Different Argument is acceptable.
*/

	SecClassPtr = (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) Type );
	if( SecClassPtr )
	{
		if(		stricmp( SecClassPtr->DllPath,	DllPath ) ||
				stricmp( SecClassPtr->FuncName,	FuncName ) ||
				stricmp( SecClassPtr->Name, 	Name ))
			return Err( SDB_ERR_CLASS_FAILURE, ARGCHECK_FUNCTION_NAME ": Class '%d - %s' already exists", Type, SecClassPtr->Name );

		strcpy( SecClassPtr->Argument,	Argument );

		// Force SDB_MSG_START by NULL-ing FuncPtr
		SecClassPtr->FuncPtr = NULL;
		return TRUE;
	}


/*
**	Else create brand new entry
*/

	if( !ERR_CALLOC( SecClassPtr, SDB_CLASS_INFO, 1, sizeof( SDB_CLASS_INFO )))
		return Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );

	SecClassPtr->Type = Type;
	strcpy( SecClassPtr->DllPath, 	DllPath );
	strcpy( SecClassPtr->FuncName, FuncName );
	strcpy( SecClassPtr->Name,		Name );
	strcpy( SecClassPtr->Argument,	Argument );
	SecClassPtr->Temporary = TRUE;		// If not already registered by sectypes.dat
	SecClassPtr->MaxStreamSize = SDB_MAX_MEM_SIZE;

	SecClassPtr->MaxUnparentedNodes = DefaultMaxUnparentedNodes;
	SecClassPtr->GCBlockSize = DefaultGCBlockSize;

	// Add the class
	if( !HashInsert( SecDbSecTypeHash, (HASH_KEY) (long) SecClassPtr->Type, SecClassPtr )
			|| !HashInsert( SecDbSecDescHash, SecClassPtr->Name, SecClassPtr ))
	{
		HashDelete( SecDbSecTypeHash, (HASH_KEY) (long) SecClassPtr->Type );
		free( SecClassPtr );
		return ErrMore( ARGCHECK_FUNCTION_NAME ": failed to insert to hash" );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SecDbClassUnregister
**	Returns: TRUE or FALSE
**	Action : Deletes class from the hash table
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassUnregister"

int SecDbClassUnregister(
	const char	*Class
)
{
	SDB_CLASS_INFO
			*ClassInfo;


	SDB_M_DATA
			MsgData;


	if( !(ClassInfo = (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) SecDbClassTypeFromName( Class ))))
		return Err( SDB_ERR_CLASS_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ) @", Class );


/*
**	Free ImplementedInterfaces
*/
	delete ClassInfo->ImplementedInterfaces;
	ClassInfo->ImplementedInterfaces = NULL;

/*
**	Make sure there are no instances hanging out in the cache
*/

	// Go out and blast any instances of this class.
	if( ClassInfo->ObjectCount > 0 )
	{
		SDB_OBJECT
				*SecOfClass = ClassInfo->FirstSec,
				*tmp;

		SDB_SEC_TYPE
				UndefinedClass;


		UndefinedClass = SecDbClassTypeFromName( "Unknown Class" );

		while( SecOfClass )
		{
			// SecDbReclassify will change the class of SecOfClass, use
			// tmp to hold next instances pointer (fix to infinite loop bug)

			tmp = SecOfClass->ClassNext;
			if( !SecDbReclassify( SecOfClass, UndefinedClass ) )
				return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ) @", Class );
			SecOfClass = tmp;
		}
	}


/*
**	Remove from hashes
*/

	HashDelete( SecDbSecTypeHash, (HASH_KEY) (long) ClassInfo->Type );
	HashDelete( SecDbSecDescHash, (HASH_KEY) ClassInfo->Name );


/*
**	Clean up all the data structures hanging off the ClassInfo
*/
	if( ClassInfo->ValueTypeMap )
	{
		delete ClassInfo->ValueTypeMap;
		ClassInfo->ValueTypeMap = 0;
	}


/*
**	Send END message to class
*/

	if( ClassInfo->FuncPtr )	// send end message only if class is loaded
	{
		MsgData.End.Class = ClassInfo;
		CLASS_MESSAGE( ClassInfo, SDB_MSG_END, &MsgData );
	}

	ClassInfo->MarkAsNotStarted();


/*
**	Free ClassInfo
*/

	if( ClassInfo->FixedData )
	{
		HashDestroy( ClassInfo->FixedData->Hash );
		free( ClassInfo->FixedData->Array );
		free( ClassInfo->FixedData );
	}

	free( ClassInfo );

	return( TRUE );
}



/****************************************************************
**	Routine: SDB_CLASS_INFO::MarkAsNotStarted
**	Returns: void
**	Action : Marks the class as not having been started so the
**			 user can correct an error and restart the class
****************************************************************/

void SDB_CLASS_INFO::MarkAsNotStarted(
)
{
	if( ClassSec )
	{
		NodeDestroyCache( ClassSec, FALSE );
		SecDbFree( ClassSec );
		ClassSec = NULL;
	}
	FuncPtr = NULL;
}



/****************************************************************
**	Routine: SecDbClassMarkAsNotStarted
**	Returns: 
**	Action : 
****************************************************************/

void SecDbClassMarkAsNotStarted(
	SDB_CLASS_INFO	*Class
)
{
	Class->MarkAsNotStarted();
}



/****************************************************************
**	Routine: SDB_CLASS_INFO::GenerateClassSec
**	Returns: true/false
**	Action : Generates the class sec to hold class static vts
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SDB_CLASS_INFO::GenerateClassSec"

bool SDB_CLASS_INFO::GenerateClassSec(
	char	*Prefix
)
{
	DT_VALUE
			TmpValue,
			TmpValue1,
			*Args;

	DT_VALUE_TYPE_INFO
			*Vti;

	SDB_NODE_CONTEXT
			Ctx;

	SDB_NODE
			*Node;

	if( Interface || Abstract )
		return true;
	
	char *SecName = StrPaste( Prefix, Name, NULL );
	if( strlen( SecName ) >= SDB_SEC_NAME_SIZE )
		SecName[ SDB_SEC_NAME_SIZE - 1 ] = '\0';

	// Construct the class security
	if( !( ClassSec = SecDbNewByClass( SecName, this, SecDbClassObjectsDb )))
	{
		free( SecName );
		ERR_OFF();
		MarkAsNotStarted();
		ERR_ON();
		ErrMore( ARGCHECK_FUNCTION_NAME ": Cannot create ~Class object for class '%s'", Name );
		return false;
	}
	free( SecName );

	DTM_INIT( &TmpValue );
	TmpValue.DataType = DtSecurity;
	TmpValue.Data.Pointer = ClassSec;
	ClassSecNode = SDB_NODE::FindLiteral( &TmpValue, SecDbRootDb );

	DTM_ALLOC( &TmpValue, DtValueTypeInfo );
	Vti = (DT_VALUE_TYPE_INFO *) TmpValue.Data.Pointer;
	Vti->Free = FALSE;
	Vti->Vti = ClassSec->SdbClass->LookupVti( SecDbValueClassValueFunc->ID );

	// Awful hack. In order to allow any VTI to calculate, we must set the terminal one, i.e. Class Value Func( ClassSec, Class Value Func )
	TmpValue1.DataType = DtValueType;
	TmpValue1.Data.Pointer = SecDbValueClassValueFunc;
	Args = &TmpValue1;
	if( !SecDbSetValueWithArgs( ClassSec, SecDbValueClassValueFunc, 1, &Args, &TmpValue, SDB_CACHE_NO_COPY | SDB_SET_DONT_LOOKUP_VTI | SDB_SET_NO_MESSAGE | SDB_CACHE_SET ))
	{
		ERR_OFF();
		MarkAsNotStarted();
		ERR_ON();
		ErrMore( ARGCHECK_FUNCTION_NAME ": Cannot set Class Value Func( ~Class, Class Value Func )" );
		return false;
	}

	Node = SDB_NODE::FindTerminalAndArgs( ClassSec, SecDbValueClassValueFunc, 1, &Args, SDB_NODE::DO_NOT_CHECK_SUPPORTED, SecDbClassObjectsDb, SDB_NODE::SHARE );
	// Build children so that we will know that this node is shareable
	if( SDB_NODE::IsNode( Node ))
		Node->BuildChildren( &Ctx );

	return true;
}



/****************************************************************
**	Routine: SecDbClassInfoFromType
**	Returns: Pointer to object function, or NULL if in error
**	Action : From the type, lookup in the hash table the function
**			 pointer.  If the function pointer is not defined,
**			 open the dll, assign the function pointer and send
**			 the object function the start message
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassInfoFromType"

SDB_CLASS_INFO *SecDbClassInfoFromType(
	SDB_SEC_TYPE	Type
)
{
	SDB_M_DATA
			MsgData;

	SDB_CLASS_INFO
			*SecClassPtr;

	if( !(SecClassPtr = (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) Type )))
	{
		Err( SDB_ERR_CLASS_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %d ) @", Type );
		return NULL;
	}

	if( SecClassPtr->FuncPtr )
		return( SecClassPtr );

	SecClassPtr->FuncPtr = (SDB_OBJ_FUNC) DynaLink( SecClassPtr->DllPath, SecClassPtr->FuncName );
	if( !SecClassPtr->FuncPtr )
	{
		ErrMore( ARGCHECK_FUNCTION_NAME "( %d ) %s", Type, SecClassPtr->Name );
		return NULL;
	}

	// Send the start message to the security class
	MsgData.Start.Class = SecClassPtr;
	if( !CLASS_MESSAGE( SecClassPtr, SDB_MSG_START, &MsgData ))
	{
		// If class load fails, the class get's unregistered, so we must try to look it up again
		if(( SecClassPtr = SecDbClassInfoLookup( Type )))
		{
			ErrMore( ARGCHECK_FUNCTION_NAME "( %d ) %s", Type, SecClassPtr->Name );
			SecClassPtr->MarkAsNotStarted();
			if( SecDbTraceFunc )
				(*SecDbTraceFunc)( SDB_TRACE_ERROR_OBJECT_MSG, NULL, SDB_MSG_START, &MsgData );
		}
		else
			ErrMore( ARGCHECK_FUNCTION_NAME "( %d ) Class failed to load, has been unregistered.", Type );
		return NULL;
	}

	return( SecClassPtr );
}



/****************************************************************
**	Routine: SecDbClassInfoLookup
**	Returns: Pointer to class info, or NULL if not found
**	Action : From the type, lookup in the hash table and return it
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassInfoLookup"

SDB_CLASS_INFO *SecDbClassInfoLookup(
	SDB_SEC_TYPE	Type
)
{
	return (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) Type );
}



/****************************************************************
**	Routine: SecDbClassNameFromType
**	Returns: Class name
**			 NULL if invalid class type
**	Summary: Retrieve the class name of security type
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassNameFromType"

const char *SecDbClassNameFromType(
	SDB_SEC_TYPE	Type		// Security type to get the name of
)
{
	SDB_CLASS_INFO
			*SecClassPtr;


	if( !(SecClassPtr = (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) Type )))
	{
		Err( SDB_ERR_CLASS_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %d ) @", Type );
		return NULL;
	}

	return( SecClassPtr->Name );
}



/****************************************************************
**	Routine: SecDbClassTypeFromName
**	Returns: Class type
**			 0 if invalid class name
**	Summary: Retrieve a security type from a security class name
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassTypeFromName"

SDB_SEC_TYPE SecDbClassTypeFromName(
	const char	*ClassName			// Name of class to determine type of
)
{
	SDB_CLASS_INFO
			*SecClassPtr;


	if( ClassName==NULL )
	{
		Err( SDB_ERR_CLASS_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( NULL ) @" );
		return 0;
	}
	if( !(SecClassPtr = (SDB_CLASS_INFO *) HashLookup( SecDbSecDescHash, ClassName )))
	{
		Err( SDB_ERR_CLASS_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ) @", ClassName );
		return 0;
	}

	return SecClassPtr->Type;
}



/****************************************************************
**	Routine: SecDbClassEnumFirst
**	Returns: Pointer to first class's information
**			 NULL - no class information loaded
**	Summary: Start the enumeration of all security classes
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassEnumFirst"

SDB_CLASS_INFO *SecDbClassEnumFirst(
	SDB_ENUM_PTR *EnumPtr		// Enumeration pointer
)
{
	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));

	(*EnumPtr)->Count = SecDbSecDescHash->KeyCount;
	(*EnumPtr)->Ptr = HashFirst( SecDbSecDescHash );
	if( HashEnd( (*EnumPtr)->Ptr ))
		return NULL;
	return (SDB_CLASS_INFO *) HashValue( (*EnumPtr)->Ptr );
}



/****************************************************************
**	Routine: SecDbClassEnumNext
**	Returns: Pointer to next class's information
**			 NULL - no more class information
**	Summary: Return the next security class's information
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassEnumNext"

SDB_CLASS_INFO *SecDbClassEnumNext(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashNext( SecDbSecDescHash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;
	return (SDB_CLASS_INFO *) HashValue( EnumPtr->Ptr );
}



/****************************************************************
**	Routine: SecDbClassEnumClose
**	Returns: Nothing
**	Summary: End enumeration of security classes
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassEnumClose"

void SecDbClassEnumClose(
	SDB_ENUM_PTR	EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
		free( EnumPtr );
}



/****************************************************************
**	Routine: SecDbVtiDelete
**	Returns: void
**	Action : Deletes a SDB_VALUE_TYPE_INFO
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbVtiDelete"

void SecDbVtiDelete(
	SDB_VALUE_TYPE_INFO	*Vti	// Vti to delete
)
{
	SDB_M_DATA
			MsgData;

	if( !Vti )
		return;

	// Inform the Vti that it is getting deleted
	SDB_MESSAGE_VT_TRACE(
		NULL,
		SDB_MSG_VTI_FREE,
		&MsgData,
		Vti
	);

	Vti->ChildList.Free();
	// FIX- BIG LEAK- Should free Vti, isn't freed now because someone may have inherited off of it, and we
	// don't know, so we can't free it
	//if( Vti->Func == SecDbValueFuncGetSecurity && !Vti->Parent )
	//	free( Vti );
}



/****************************************************************
**	Routine: SecDbVtiInherit
**	Returns: void
**	Action : inherits VTI from Parent
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbVtiInherit"

void SecDbVtiInherit(
	SDB_VALUE_TYPE_INFO	*VtiChild,
	SDB_VALUE_TYPE_INFO *VtiParent
)
{
	SDB_M_DATA
			MsgData;


	*VtiChild = *VtiParent;
	VtiChild->Parent = VtiParent;
	memset( &VtiChild->ChildList, 0, sizeof( VtiChild->ChildList ));

	// Inform the Vti that it has been inherited
	SDB_MESSAGE_VT_TRACE(
		NULL,
		SDB_MSG_VTI_INHERIT,
		&MsgData,
		VtiChild
	);
}


/****************************************************************
**	Routine: SecDbClassTypesFromNameList
**	Returns: TRUE/Err
**	Action : Parses the string for class names separated by the
**           given character delimiter. Fills the SecTypes array
**
**  See Also: SecDbClassTypeFromName()
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassTypesFromNameList"

int SecDbClassTypesFromNameList(
	char const*		NameList,
	char const      Delimitter,
	SDB_SEC_TYPE*	SecTypes,
	int*			NumSecTypes	// input - max number of sectypes, output number collected
)
{
	if( !NameList || *NameList == '\0' )
	{
		SecTypes[0]  = 0;
		*NumSecTypes = 1;
		return( TRUE );
	}

	// set up delimitter string
	char    DelimStr[2] = { Delimitter, '\0' };

	char    ClassName[ SDB_CLASS_NAME_SIZE ];
	int     CurrSecType = 0;
	int const
	        MaxSecTypes = *NumSecTypes;
	char const*
	        CurrClassPtr = NameList;

    while( *CurrClassPtr && ( CurrSecType < MaxSecTypes ))
	{
		// find length to the next delimitter (or end of string)
		unsigned const ClassLen = strcspn( CurrClassPtr, DelimStr );
		
		if( ClassLen >= SDB_CLASS_NAME_SIZE )
			return( Err( ERR_INVALID_STRING, ARGCHECK_FUNCTION_NAME ": @ - \"%s\" contains a class name greater than %d characters", NameList, SDB_CLASS_NAME_SIZE ));

		strncpy( ClassName, CurrClassPtr, ClassLen );
		ClassName[ ClassLen ] = '\0';

		// get the ClassType
		SecTypes[ CurrSecType++ ] = SecDbClassTypeFromName( ClassName );

		// move to next class
		CurrClassPtr += ClassLen;

		if( *CurrClassPtr != '\0' )
			CurrClassPtr++;
	}

	if( *CurrClassPtr && ( CurrSecType > MaxSecTypes ))
		return( Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME "() - Number of classes given is larger than Types array" ));

	*NumSecTypes = CurrSecType;

	return( TRUE );
}
