#define GSCVSID "$Header: /home/cvs/src/secdb/src/dtdb.c,v 1.32 2008/07/02 16:13:26 khodod Exp $"
/****************************************************************
**
**	DTDB.C		- SecDb Database DataType
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: dtdb.c,v $
**	Revision 1.32  2008/07/02 16:13:26  khodod
**	New way to copy db source id in SecurityCopy and seccopy.
**	iBug: #56731
**
**	Revision 1.31  2005/05/27 20:19:29  e45019
**	SecDbCopyByName now takes Dup argument which if true causes a duplicate security to be created, and if false causes a new security to be created.
**	iBug: #25421
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<secdb.h>
#include	<secdt.h>
#include	<secerror.h>
#include	<secexpr.h>
#include	<sdb_int.h>
#include	<dterr.h>
#include	<heapinfo.h>


EXPORT_C_SECDB int SlangXUseDatabase( SLANG_ARGS );
EXPORT_C_SECDB int SlangXListDatabases( SLANG_ARGS );
EXPORT_C_SECDB int SlangXSecurityCopy( SLANG_ARGS );


/****************************************************************
**	Routine: DtFuncDatabase
**	Returns: TRUE	- Operation worked
**			 FALSE	- Operation failed
**	Action : Handle operations for Database data type
****************************************************************/

int DtFuncDatabase(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	DT_DATABASE
			*Database,
			*DatabaseA,
			*DatabaseB;

	SDB_DB_INFO
			DbInfo;

	SDB_DB	*Db;

	DT_INFO	*DataTypeInfo;


	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size	= sizeof( Database );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_REFERENCE;
			return TRUE;

		case DT_MSG_ALLOCATE:
			if( !SecDbDbInfoFromDb( SecDbRootDb, &DbInfo ))
			{
				DTM_INIT( r );
				return ErrMore( "Database(): Can't find ROOT database" );
			}

			Db = SecDbAttach( DbInfo.DbName, SDB_DEFAULT_DEADPOOL_SIZE, SDB_BACK );
			if( !Db )
				return ErrMore( "Database(): Can't attach to '%s'", DbInfo.DbName );

			Database = (DT_DATABASE *) calloc( 1, sizeof( *Database ));
			Database->UseCount	= 1;
			Database->Db		= Db;
			r->Data.Pointer		= Database;
			return TRUE;

		case DT_MSG_FREE:
			Database = (DT_DATABASE *) a->Data.Pointer;
			if( Database && --Database->UseCount == 0 )
			{
				SecDbDetach( Database->Db );
				free( Database );
				a->Data.Pointer = NULL;
			}
			return TRUE;

		case DT_MSG_ASSIGN:
			Database = (DT_DATABASE *) a->Data.Pointer;
			if( Database )
				Database->UseCount++;
			*r = *a;
			return TRUE;

		case DT_MSG_TO:
			Database = (DT_DATABASE *) a->Data.Pointer;
			if( Database && r->DataType == DtString )
			{
				if( SecDbDbInfoFromDb( Database->Db, &DbInfo ))
					r->Data.Pointer = strdup( DbInfo.DbName );
				else
					r->Data.Pointer = strdup( "Unknown database ID" );
			}
			else if ( Database && r->DataType == DtDouble )
			{
				r->Data.Number = Database->Db->DbID;
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", a->DataType->Name, r->DataType->Name );
			return TRUE;

		case DT_MSG_FROM:
			Db = NULL;
			if( b->DataType == DtString )
				Db = SecDbAttach( (char *) b->Data.Pointer, SDB_DEFAULT_DEADPOOL_SIZE, SDB_BACK );
			else if( b->DataType == DtSecurity )
			{
				Db = ((SDB_OBJECT *) b->Data.Pointer )->Db;
				if( !SecDbReattach( Db ))
					return FALSE;
			}
			else if( b->DataType == DtDiddleScope )
			{
				if( b->Data.Pointer )
					Db = ((DT_DIDDLE_SCOPE *) b->Data.Pointer )->Db;
				else
					return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert Null DiddleScope to a Database" );
				if( !SecDbReattach( Db ))
					return FALSE;
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", b->DataType->Name, a->DataType->Name );

			if( !Db )
				return FALSE;

			Database = (DT_DATABASE *) calloc( 1, sizeof( *Database ));
			Database->UseCount	= 1;
			Database->Db		= Db;
			a->Data.Pointer		= Database;
			return TRUE;

		case DT_MSG_EQUAL:
			if( a->DataType != b->DataType )
				r->Data.Number = (double) FALSE;
			else
			{
				DatabaseA = (DT_DATABASE *) a->Data.Pointer;
				DatabaseB = (DT_DATABASE *) b->Data.Pointer;
				r->Data.Number = (double) ( DatabaseA->Db == DatabaseB->Db );
			}
			r->DataType = DtDouble;
			return TRUE;

		case DT_MSG_NOT_EQUAL:
			if( a->DataType != b->DataType )
				r->Data.Number = (double) TRUE;
			else
			{
				DatabaseA = (DT_DATABASE *) a->Data.Pointer;
				DatabaseB = (DT_DATABASE *) b->Data.Pointer;
				r->Data.Number = (double) ( DatabaseA->Db != DatabaseB->Db );
			}
			r->DataType = DtDouble;
			return TRUE;

		case DT_MSG_TRUTH:
			return( a->Data.Pointer ? TRUE : FALSE );

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
	}


/*
**	Unsupported operation
*/

	if( b )
		Err( ERR_UNSUPPORTED_OPERATION, "@ - %s between %s and %s", DtMsgToString( Msg ), a->DataType->Name, b->DataType->Name );
	else if( a )
		Err( ERR_UNSUPPORTED_OPERATION, "@ - %s on %s", DtMsgToString( Msg ), a->DataType->Name );
	else
		Err( ERR_UNSUPPORTED_OPERATION, "@ - %s", DtMsgToString( Msg ) );

	return FALSE;
}



/****************************************************************
**	Routine: SlangXUseDatabase
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Temporarily change the root database
****************************************************************/

int SlangXUseDatabase( SLANG_ARGS )
{
	SLANG_RET
			TmpRet1;

	DT_DATABASE
			*Database;

	SDB_DB	*OldDb;

	int		RetCode;


/*
**	Check the number of arguments (the extra arg is the block)
*/

	if( Root->Argc != 2 )
		return SlangEvalError( SLANG_ERROR_PARMS, "%s( Database )\n{\n    Block\n};", Root->StringValue );


	SLANG_EVAL_ARG(0, SLANG_EVAL_RVALUE, &TmpRet1);

	if( TmpRet1.Data.Value.DataType != DtDatabase )
	{
		RetCode = SlangEvalError( SLANG_ERROR_PARMS, "%s: Expected a database, not a %s", Root->StringValue, TmpRet1.Data.Value.DataType->Name );
		SLANG_RET_FREE( &TmpRet1 );
		return( RetCode );
	}


/*
**	Set the root database
*/

	Database = (DT_DATABASE *) TmpRet1.Data.Value.Data.Pointer;
	OldDb = SecDbSetRootDatabase( Database->Db );


/*
**	Evaluate the block
*/

	RetCode = SlangEvaluate( Root->Argv[ 1 ], EvalFlag, Ret, Scope );
	SLANG_RET_FREE( &TmpRet1 );
	SecDbSetRootDatabase( OldDb );

	return( RetCode );
}



/****************************************************************
**	Routine: SlangXListDatabases
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Return a structure with entries corresponding to
**			 each database in the table
****************************************************************/

int SlangXListDatabases( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Only Open", &DtDouble,	  "Return info only for open dbs if TRUE",	SLANG_ARG_OPTIONAL | SLANG_ARG_NULLS }
			};

	SLANG_ARG_VARS;

	SDB_ENUM_PTR
			EnumPtr;

	DT_VALUE
			*Value;

	SDB_DB_INFO
			*DbInfo;

	int		OnlyOpen = FALSE;


	SLANG_ARG_PARSE();

	if( !SLANG_ARG_OMITTED(0))
		OnlyOpen = SLANG_INT(0);

	Value = &Ret->Data.Value;
	if( !DTM_ALLOC( Value, DtStructure ))
	{
		SLANG_ARG_FREE();
		return( SlangEvalError( Root, EvalFlag, Ret, Scope, "No memory" ));
	}

	for( DbInfo = SecDbDbInfoEnumFirst( &EnumPtr );
		DbInfo;
		DbInfo = SecDbDbInfoEnumNext( EnumPtr ))
	{
		DT_VALUE
				Entry;


		if( OnlyOpen && 0 == DbInfo->OpenCount )
			continue;

		if( !SecDbDbInfoToStructure( DbInfo, &Entry ))
		{
			DTM_FREE( Value );
			SLANG_ARG_FREE();
			return( SlangEvalError( Root, EvalFlag, Ret, Scope, "No memory" ));
		}

		DtComponentSet(	Value, DbInfo->DbName, &Entry );
		DTM_FREE( &Entry );
	}
	SecDbDbInfoEnumClose( EnumPtr );
	Ret->Type = SLANG_TYPE_VALUE;

	SLANG_ARG_FREE();
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXSecurityCopy
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Just like SecCopy, only inside slang
****************************************************************/

int SlangXSecurityCopy( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "SecName",    &DtString,    "Security name"			},
				{ "SrcDb",	    &DtDatabase,  "Source database"			},
				{ "DstDb",	    &DtDatabase,  "Destination database"	},
				{ "NewName",    &DtString,    "New name (defaults to SecName)",	SLANG_ARG_OPTIONAL | SLANG_ARG_NULLS },
				{ "DupSrcDbId", &DtDouble,    "Duplicate SrcDb Id (defaults to FALSE)", SLANG_ARG_OPTIONAL | SLANG_ARG_NULLS },
			};

	SLANG_ARG_VARS;

	DT_DATABASE
			*SrcDb,
			*DstDb;

	char	
			*SecName,
			*NewName;

	int     
			AddFlags = SDB_ADD_PRESERVE_DISK_INFO;

	SLANG_ARG_PARSE();

	SecName = (char *) SLANG_PTR(0);
	SrcDb = (DT_DATABASE *) SLANG_PTR(1);
	DstDb = (DT_DATABASE *) SLANG_PTR(2);
	if( SLANG_VAL(3).DataType == DtString )
		NewName = (char *) SLANG_PTR(3);
	else
		NewName = NULL;

	if( !SLANG_ARG_OMITTED(4) && SLANG_INT(4) )
		AddFlags |= SDB_ADD_PRESERVE_SRC_DB_ID;

	Ret->Type = SLANG_TYPE_VALUE;
	Ret->Data.Value.DataType	= DtDouble;
	Ret->Data.Value.Data.Number = SecDbCopyByName( SecName, SrcDb->Db, DstDb->Db, NewName, AddFlags );

	SLANG_ARG_FREE();

	return SLANG_OK;
}
