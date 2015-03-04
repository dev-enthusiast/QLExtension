#define GSCVSID "$Header: /home/cvs/src/secdb/src/dtseclst.c,v 1.28 2001/11/27 23:23:33 procmon Exp $"
/****************************************************************
**
**	DTSECLST.C	- Security List datatype
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: dtseclst.c,v $
**	Revision 1.28  2001/11/27 23:23:33  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.24  2001/09/10 15:32:22  nauntm
**	fixed uninited var bug
**	
**	Revision 1.23  2001/06/01 00:24:47  vengrd
**	Make HASH_PORTABLE work for Security Lists and Securities.
**	
**	Revision 1.22  1999/09/01 15:29:26  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.21  1999/04/16 12:51:25  nauntm
**	memory leak fix
**	
**	Revision 1.20  1999/04/06 22:07:51  nauntm
**	Added streaming
**	
**	Revision 1.19  1999/02/02 22:55:18  brownb
**	eliminate GCC signed/unsigned compare warning
**	
**	Revision 1.18  1998/11/16 22:59:54  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.17.4.1  1998/11/06 17:43:53  singhki
**	fix useless extra alloc
**	
**	Revision 1.17  1998/08/27 19:24:30  singhki
**	Use DT_FLAG_VALUE_TYPE & DT_FLAG_SDB_OBJECT to control which datatypes
**	get casts generated for them
**	
**	Revision 1.16  1998/01/02 21:32:43  rubenb
**	added GSCVSID
**	
**	Revision 1.15  1997/10/07 20:50:43  lundek
**	Plug reference leak in DT_MSG_SUBSCRIPT_REPLACE
**	
**	Revision 1.14  1997/10/07 20:16:19  simonde
**	Plug memory and reference leak in DT_MSG_SUBSCRIPT_REPLACE.
**	
**	Revision 1.13  1997/05/01 19:44:48  lundek
**	Put DtEach back in
**
**	Revision 1.12  1997/05/01 19:44:08  lundek
**	Back out DtEach temporarily
**
**	Revision 1.11  1997/05/01 18:43:53  schlae
**	Heap functions become GSHeap functions
**
**	Revision 1.10  1997/04/30 01:53:05  lundek
**	Each( SecList ) and Array( SecList )
**
**	Revision 1.9  1997/03/06 01:44:36  lundek
**	UFO enhancements and clean up
**
**	Revision 1.8  1997/02/25 17:01:11  gribbg
**	Move SecDb datatypes to secdt.h
**
**	Revision 1.7  1997/02/21 23:58:16  lundek
**	Take out SecList[ String ]
**
**	Revision 1.6  1996/12/27 20:24:50  rubenb
**	Allow subscript to be a string (just like DtArray) as well as a double,
**	to make secview editor happy.
**
**	Revision 1.5  1996/11/19 22:52:17  lundek
**	PurifyFix: stop accessing b->Data.Pointer unless datatype is known to be DtArray
**
**	Revision 1.4  1996/10/11 01:10:38  lundek
**	BugFix: Security List to Structure conversion
**
**	Revision 1.3  1996/09/11 13:39:22  gribbg
**	New Trees Merge
**
**	Revision 1.1.2.4  1996/08/08 01:02:40  lundek
**	Support Security List( Array ) and Security List( Structure )
**
**	Revision 1.1.2.3  1996/08/02 13:08:10  schlae
**	Casts for NT
**
**	Revision 1.1.2.2  1996/08/01 22:16:55  lundek
**	ChildListItemRef->PassErrors; ~vt( self [, PassErrors ])
**
**	Revision 1.1.2.1  1996/05/10 11:54:29  gribbg
**	NewTrees: First snapshot
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<limits.h>
#include	<math.h>
#include	<err.h>
#include	<heapinfo.h>
#include	<datatype.h>
#include	<dterr.h>
#include	<secdb.h>
#include	<secdt.h>


/*
**	Define constants
*/

#define	BIG_BLOCK_OF_MEMORY		32000		// FIX - Should be smarter
#define	SAFETY_ZONE				1000



/****************************************************************
**	Routine: DtFuncSecurityList
**	Returns: TRUE	- Operation worked
**			 FALSE	- Operation failed
**	Action : Handle operations for security list data types
****************************************************************/

int DtFuncSecurityList(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
#define	MAX_ARRAY_SIZE		(INT_MAX / (int) sizeof( SDB_OBJECT * ))

	DT_INFO	*DataTypeInfo;

	DT_SECURITY_LIST
			*Array,
			*ArrayB;

	int		ElementNumber,
			TmpElementNumber,
			Ret;

	DT_VALUE 
			Temp;

	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size	= sizeof( a->Data.Pointer );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_SUBSCRIPTS | DT_FLAG_SDB_OBJECT;
			return TRUE;

		case DT_MSG_ALLOCATE:
			return ERR_CALLOC( r->Data.Pointer, DT_SECURITY_LIST, 1, sizeof( DT_SECURITY_LIST ));

		case DT_MSG_FREE:
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			for( ElementNumber = 0; ElementNumber < Array->Size; ElementNumber++ )
				SecDbFree( Array->SecPtrs[ ElementNumber ] );
			free( Array->SecPtrs );
			free( Array );
			return TRUE;

		case DT_MSG_ASSIGN:
			r->Data.Pointer = Array = (DT_SECURITY_LIST *) malloc( sizeof( DT_SECURITY_LIST ));
			Array->Size = ((DT_SECURITY_LIST *) a->Data.Pointer)->Size;
			if( Array->Size )
			{
				Array->SecPtrs = (SDB_OBJECT **) malloc( Array->Size * sizeof( SDB_OBJECT * ));
				for( ElementNumber = 0; ElementNumber < Array->Size; ElementNumber++ )
					Array->SecPtrs[ ElementNumber ] = SecDbIncrementReference(( (DT_SECURITY_LIST *) a->Data.Pointer)->SecPtrs[ ElementNumber ] );
			}
			else
				Array->SecPtrs = NULL;
			break;

		case DT_MSG_ASSIGN_AND:
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			if( b->DataType != DtSecurity )
				return( Err( ERR_INVALID_ARGUMENTS, "@ - an element in a %s must be a %s, not a %s", a->DataType->Name, DtSecurity->Name, b->DataType->Name ));
			if( Array->Size + 1 < MAX_ARRAY_SIZE )
			{
				Array->Size++;
				Array->SecPtrs = (SDB_OBJECT **) realloc( Array->SecPtrs, Array->Size * sizeof( SDB_OBJECT * ));
				Array->SecPtrs[ Array->Size - 1 ] = SecDbIncrementReference( (SDB_OBJECT *) b->Data.Pointer );
				return TRUE;
			}
			else
				return Err( ERR_OVERFLOW, "Array - @ maximum size allowed is %d", MAX_ARRAY_SIZE );

		case DT_MSG_SUBSCRIPT_REPLACE:	// a[ b ] = r and free old a[ b ]
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;

			if( r->DataType != DtSecurity )
				return( Err( ERR_INVALID_ARGUMENTS, "@ - an element in a %s must be a %s, not a %s", a->DataType->Name, DtSecurity->Name, r->DataType->Name ));

			// Get element number from b
			if( b->DataType != DtDouble )
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript must be a Double, not a %s", b->DataType->Name );
			ElementNumber = (int) b->Data.Number;
			if( ElementNumber < 0 || ElementNumber >= MAX_ARRAY_SIZE )
				return Err( DT_ERR_SUBSCRIPT_OUT_OF_RANGE, "@" );

			// Resize array if necessary
			if( ElementNumber >= Array->Size )
			{
				TmpElementNumber = Array->Size;
				Array->Size = ElementNumber + 1;
				Array->SecPtrs = (SDB_OBJECT **) realloc( Array->SecPtrs, Array->Size * sizeof( SDB_OBJECT * ));

				for( ; TmpElementNumber < Array->Size; TmpElementNumber++ )
					Array->SecPtrs[ TmpElementNumber ] = NULL;
			}

			SecDbFree( Array->SecPtrs[ ElementNumber ] );
			Array->SecPtrs[ ElementNumber ] = (SDB_OBJECT *) r->Data.Pointer;
			return( TRUE );

		case DT_MSG_SUBSCRIPT_VALUE:	// r = a[ b ]
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			if( b->DataType == DtDouble )
				ElementNumber = (int) b->Data.Number;
			else
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript must be a Double" );
			if( ElementNumber < 0 || ElementNumber >= Array->Size )
				return Err( DT_ERR_SUBSCRIPT_OUT_OF_RANGE, "@" );
			r->DataType = DtSecurity;
			r->Data.Pointer = SecDbIncrementReference( Array->SecPtrs[ ElementNumber ] );
			return( TRUE );

		case DT_MSG_SUBSCRIPT_DESTROY:
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			if( b->DataType != DtDouble )
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript must be a Double" );
			ElementNumber = (int) b->Data.Number;
			if( ElementNumber < 0 || ElementNumber >= Array->Size )
				return Err( DT_ERR_SUBSCRIPT_OUT_OF_RANGE, "@" );
			SecDbFree( Array->SecPtrs[ ElementNumber ] );
			Array->SecPtrs[ ElementNumber ] = NULL;
			return( TRUE );

		case DT_MSG_SUBSCRIPT_LAST:
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			if( Array->Size > 0 )
			{
				r->DataType = DtDouble;
				r->Data.Number = Array->Size - 1;
			}
			else
			{
				DTM_INIT( r );
				return( FALSE );
			}
			return( TRUE );

		case DT_MSG_SUBSCRIPT_NEXT:
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			if( r->DataType == DtDouble )
				r->Data.Number += 1;
			else
			{
				r->DataType = DtDouble;
				r->Data.Number = 0;
			}
			if( (int) r->Data.Number >= Array->Size )
			{
				DTM_INIT( r );
				return( FALSE );
			}
			return( TRUE );

		case DT_MSG_EQUAL:
		case DT_MSG_NOT_EQUAL:
			r->DataType = DtDouble;
			if( a->DataType != b->DataType )
				r->Data.Number = 0.0;
			else
			{
				Array	= (DT_SECURITY_LIST *) a->Data.Pointer;
				ArrayB	= (DT_SECURITY_LIST *) b->Data.Pointer;

				if( Array == ArrayB )
					r->Data.Number = 1.0;
				else if( !Array || !ArrayB || Array->Size != ArrayB->Size )
					r->Data.Number = 0.0;
				else
				{
					r->Data.Number = 1.0;
					for( ElementNumber = 0; ElementNumber < Array->Size; ElementNumber++ )
						if( Array->SecPtrs[ ElementNumber ] != ArrayB->SecPtrs[ ElementNumber ] )
						{
							r->Data.Number = 0.0;
							break;
						}
				}
			}
			if( DT_MSG_NOT_EQUAL == Msg )
				r->Data.Number = (r->Data.Number ? 0.0 : 1.0);
			return TRUE;

		case DT_MSG_TO:
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			if( r->DataType == a->DataType )
				return( DTM_ASSIGN( r, a ));
			if( r->DataType == DtString )
			{
				char	*OldStr,
						*Str;


				OldStr = Str = (char *) malloc( BIG_BLOCK_OF_MEMORY );
				*Str = '\0';
				for( ElementNumber = 0; ElementNumber < Array->Size; ElementNumber++ )
				{
					Str += sprintf( Str, "[%4d] = %s\n", ElementNumber, Array->SecPtrs[ ElementNumber ] ? Array->SecPtrs[ ElementNumber ]->SecData.Name : "Undefined" );
					if( (Str - OldStr) > (BIG_BLOCK_OF_MEMORY - SAFETY_ZONE))
					{
						Str += sprintf( Str, "Security List too long to format...\n" );
						break;
					}
				}

				r->Data.Pointer = realloc( OldStr, (Str - OldStr) + 1 );
			}
			else if( r->DataType == DtArray || r->DataType == DtEach )
			{
				DT_DATA_TYPE
						DtTmp = r->DataType;

				DTM_ALLOC( r, DtTmp );
				for( ElementNumber = 0; ElementNumber < Array->Size; ElementNumber++ )
					if( Array->SecPtrs[ ElementNumber ] != NULL )
						DtSubscriptSetPointer( r, ElementNumber, DtSecurity, Array->SecPtrs[ ElementNumber ] );
					else
						DtSubscriptSetNumber( r, ElementNumber, DtNull, 0 );
			}
			else if( r->DataType == DtStructure )
			{
				DTM_ALLOC( r, DtStructure );
				for( ElementNumber = 0; ElementNumber < Array->Size; ElementNumber++ )
					if( Array->SecPtrs[ ElementNumber ] != NULL )
						DtComponentSetPointer( r, Array->SecPtrs[ ElementNumber ]->SecData.Name, DtSecurity, Array->SecPtrs[ ElementNumber ] );
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "Security List, @" );
			return TRUE;

		case DT_MSG_FROM:
			if( b->DataType->Flags & ( DT_FLAG_SUBSCRIPTS | DT_FLAG_COMPONENTS ))
			{
				DT_VALUE
						Enum,
						ElemVal,
						TmpVal,
						*NameVal;


				Array = (DT_SECURITY_LIST *) (a->Data.Pointer = calloc( 1, sizeof( DT_SECURITY_LIST )));
				Array->Size = DtSize( b );
				Array->SecPtrs = (SDB_OBJECT **) calloc( Array->Size, sizeof( *Array->SecPtrs ));
				ElementNumber = 0;
				DTM_FOR( &Enum, b )
				{
					if( Enum.DataType != DtString )
					{
						if( !DtSubscriptGetCopy( b, &Enum, &ElemVal ))
							return( FALSE );
						NameVal = &ElemVal;
					}
					else
					{
						DTM_INIT( &ElemVal );
						NameVal = &Enum;
					}
					TmpVal.DataType = DtSecurity;
					if( DTM_TO( &TmpVal, NameVal ))
						Array->SecPtrs[ ElementNumber++ ] = (SDB_OBJECT *) TmpVal.Data.Pointer;
					else
						Array->SecPtrs[ ElementNumber++ ] = NULL;
					DTM_FREE( &ElemVal );
				}
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "Security List, @" );
			return TRUE;

		case DT_MSG_TRUTH:
			return TRUE;

		case DT_MSG_SIZE:
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			r->DataType 	= DtDouble;
			r->Data.Number	= Array->Size;
			return TRUE;

		case DT_MSG_MEMSIZE:
			Array = (DT_SECURITY_LIST *) a->Data.Pointer;
			r->DataType 	= DtDouble;
			r->Data.Number	= GSHeapMemSize( Array ) + GSHeapMemSize( Array->SecPtrs );
			return TRUE;

		case DT_MSG_INFO:
			DataTypeInfo = (DT_INFO *) calloc( 1, sizeof( DT_INFO ));
			DataTypeInfo->Flags = DT_FLAG_POINTER | DT_FLAG_SUBSCRIPTS;
			r->Data.Pointer = DataTypeInfo;
			return TRUE;

		case DT_MSG_TO_STREAM:
			Temp.DataType = DtArray;
			if ( DTM_TO( &Temp, a ))
			{
				Ret = DT_OP( DT_MSG_TO_STREAM, r, &Temp, NULL );
				Ret = DTM_FREE( &Temp ) && Ret;
				return Ret;
			}
			return FALSE;

		case DT_MSG_FROM_STREAM:
			Temp.DataType = DtArray;
			Ret = DT_OP( DT_MSG_FROM_STREAM, r, &Temp, NULL );
			if (Ret)
			{
				Ret = DTM_TO( a, &Temp );
				DTM_FREE( &Temp );
				return TRUE;
			}
			return FALSE;

		case DT_MSG_HASH_PORTABLE:
			{
				Ret = TRUE;
				DT_HASH_CODE
						Hash = *(DT_HASH_CODE *)DT_VALUE_TO_POINTER( r );

				Array = (DT_SECURITY_LIST *)DT_VALUE_TO_POINTER( a );

				for( ElementNumber = 0; ElementNumber < Array->Size; ElementNumber++ )
					if( !SecDbHashInStreams( Array->SecPtrs[ ElementNumber ], &Hash ))
					{
						Ret = ErrMore( "Could not hash array element #%d", ElementNumber );
						break;
					}
				if( Ret )
					*(DT_HASH_CODE *)DT_VALUE_TO_POINTER( r ) = Hash;
			}
			return Ret;

		default:
			return Err( ERR_UNSUPPORTED_OPERATION, "Security List, @" );
	}

	r->DataType = a->DataType;
	return TRUE;

#undef MAX_ARRAY_SIZE
}
