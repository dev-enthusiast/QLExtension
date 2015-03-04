#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_indx.c,v 1.88 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	SDB_INDX.C	- Security Database Index Functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_indx.c,v $
**	Revision 1.88  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.87  2010/06/18 17:26:59  ellist
**	Additional error checks for sparse indices
**
**	Revision 1.86  2010/04/08 16:55:23  ellist
**	Support constrained (sparse) indices
**
**	Revision 1.85  2001/09/04 14:26:30  goodfj
**	Support for IndexGetByName
**
**	Revision 1.84  2001/08/20 22:24:20  simpsk
**	various cleanups: added missing #includes, removed deprecated CC_ macros.
**	
**	Revision 1.83  2001/06/29 15:22:14  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.82  2001/05/18 09:57:00  shahia
**	Bug Fix: secindex -create crashes if objects are unloadable
**	
**	Revision 1.81  2001/01/11 15:15:17  goodfj
**	Don't check TotalByteWidth in IndexCompare()
**	
**	Revision 1.80  2000/11/01 22:53:04  brownb
**	fix GCC compile error: for index variable used
**	
**	Revision 1.79  2000/10/31 14:04:17  goodfj
**	Check indexes match when opening index in union db
**	
**	Revision 1.78  2000/06/13 14:52:42  goodfj
**	Get Index->Handle from DbUpdate. Verify all classes in index have same DbUpdate (for ClassFilterUnions)
**	
**	Revision 1.77  2000/06/13 11:25:29  goodfj
**	SecDbIndexEnumFirst/Next take db arg, set ErrNum == ERR_NONE after last index, to allow us to distinguish errors
**	
**	Revision 1.76  2000/06/13 09:38:24  goodfj
**	Error checking
**	
**	Revision 1.75  2000/06/12 14:42:41  goodfj
**	DbUpdate fixes
**	
**	Revision 1.74  2000/06/08 20:28:13  brownb
**	add / delete a class from an existing index
**	
**	Revision 1.73  2000/02/08 22:24:07  singhki
**	No more references to DbID for virtual databases. All virtual
**	databases have the same DbID. This removes the limit on the number of
**	virtual databases you can create in memory.
**	
**	Revision 1.72  1999/12/06 13:50:33  goodfj
**	DbUpdate is now private. New api to access it (for the time-being)
**	
**	Revision 1.71  1999/10/01 16:44:10  singhki
**	Use BuildingIndex to determine if we can add classes to an index
**	
**	Revision 1.70  1999/09/01 15:29:30  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.69  1999/08/23 15:29:02  singhki
**	Death to SDB_DB_ID in Index API
**	
**	Revision 1.68  1999/08/10 19:58:08  singhki
**	Register None and Null as diddle scopes in order to preserve behaviour
**	
**	Revision 1.67  1999/08/07 01:40:10  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.66  1999/07/15 16:01:01  singhki
**	remove nasty linked list of open dbs, use DatabaseIDHash and OpenCount instead.
**	
**	Revision 1.65  1999/04/23 00:03:32  nauntm
**	Unused arg/uninitialized variable extermination
**	
**	Revision 1.64  1999/03/22 20:52:43  singhki
**	Use ERR_STR_DONT_CHANGE
**	
**	Revision 1.63  1999/03/18 22:05:47  rubenb
**	change to match prototype
**	
**	Revision 1.62  1999/02/02 12:29:43  vengrd
**	Grab the driver handle from DbUpdate during IndexOpen so index updates happen properly.
**	
**	Revision 1.61  1999/01/22 18:56:24  vengrd
**	Synthetic DBs have no indices.  They are handled in unions
**	by simply leaving them out of the index pos tree.
**	
**	Revision 1.60  1999/01/12 21:40:38  singhki
**	Make IndexNew() & IndexDestroy() work for Union Databases by passing them down to DbUpdate
**	
**	Revision 1.59  1998/12/09 01:28:28  singhki
**	Use ERR_OFF/ON instead of ErrPush/Pop
**	
**	Revision 1.58  1998/11/16 22:59:58  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.57  1998/11/13 02:02:48  vengrd
**	Support indices in subdbs.
**	
**	Revision 1.56  1998/11/12 00:55:05  vengrd
**	Exported SecDbIndexCreate for use in SubDbs
**	
**	Revision 1.55.2.2  1998/11/05 01:01:24  singhki
**	TempRev: hacks to support OS/2 IBM compiler in C++ mode
**	
**	Revision 1.55.2.1  1998/10/30 00:03:17  singhki
**	DiddleColors: Initial revision
**	
**	Revision 1.55  1998/10/14 18:20:52  singhki
**	New search paths
**	
**	Revision 1.54.2.1  1998/08/28 23:33:59  singhki
**	search paths, index merge working, some problems with ~casts
**	
**	Revision 1.54  1998/07/09 02:32:36  nauntm
**	casts and fixes to quiet compiler
**	
**	Revision 1.53  1998/07/08 16:43:02  nauntm
**	Fixed string on an Err() [wrong arg count]
**	
**	Revision 1.52  1998/05/13 19:57:33  brownb
**	SecDbTransMimic returns TransId
**	
**	Revision 1.51  1998/03/26 17:07:59  gribbg
**	Add SecDbIndexBuildPartCount
**	
**	Revision 1.50  1998/01/02 21:33:02  rubenb
**	added GSCVSID
**	
**	Revision 1.49  1997/12/19 01:58:06  gribbg
**	Change SecDbError to Err and SDB_ERR to ERR
**	
**	Revision 1.48  1997/10/29 20:43:29  lundek
**	casts
**	
**	Revision 1.47  1997/09/03 22:39:20  gribbg
**	BugFix: use proper DbId in IndexRepair
**	
**	Revision 1.46  1997/08/28 23:10:26  gribbg
**	BugFix: Prevent IndexName overflow
**	
**	Revision 1.45  1997/07/31 20:24:00  gribbg
**	BugFix: Use IndexFromName in IndexDestroy
**	
**	Revision 1.44  1997/03/06 01:44:42  lundek
**	UFO enhancements and clean up
**
**	Revision 1.43  1997/02/24 23:30:45  lundek
**	SDB_IGNORE_PATH
**
**	Revision 1.42  1996/12/23 03:06:20  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<mempool.h>
#include	<secdb.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<secdrv.h>
#include	<sectrans.h>
#include	<sdb_int.h>

#include    <string>
CC_USING( std::string );

/*
**	Define constants
*/

#define	SDB_INDEX_VALIDATE_BUFFER_SIZE		ERR_MAX_STR
#define	SDB_INDEX_VALIDATE_BUFFER_MARGIN	256


/*
**	Global for transaction part count when rebuilding an Index
*/

long	SecDbIndexBuildPartCount = 100;

/*
**	Prototype functions
*/

static int
		SecDbIndexBuild(	SDB_INDEX *Index, SDB_DB *Db );


/****************************************************************
**	Routine: IndexCompare
**	Returns: TRUE if identical, else Err()
**	Action : Makes sure two indexes match
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "IndexCompare"

static int IndexCompare(
	SDB_INDEX	*Index1,
	SDB_INDEX	*Index2
)
{
	int i;

	if( stricmp( Index1->Name, Index2->Name ) )
		return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": Name mismatch (%s != %s)", Index1->Name, Index2->Name );
	if( Index1->PartCount != Index2->PartCount )
		return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": PartCount mismatch (%d != %d)", Index1->PartCount, Index2->PartCount );
	if( Index1->ClassCount != Index2->ClassCount )
		return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": ClassCount mismatch (%d != %d)", Index1->ClassCount, Index2->ClassCount );
	if( Index1->Flags != Index2->Flags )
		return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": Flags mismatch (%d != %d)", Index1->Flags, Index2->Flags );
	if( Index1->ConstraintCount != Index2->ConstraintCount )
		return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": ConstraintCount mismatch (%d != %d)", Index1->ConstraintCount, Index2->ConstraintCount );
/*
**	Don't check TotalByteWidth for the moment. Some indexes have incorrect TotalByteWidth with apparently#
**	no harmful side-effects.
*/
//	if( Index1->TotalByteWidth != Index2->TotalByteWidth )
//		return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": TotalByteWidth mismatch (%d != %d)", Index1->TotalByteWidth, Index2->TotalByteWidth );

	for( i = 0; i < Index1->PartCount; ++i )
	{
		SDB_INDEX_PART
			Part1 = Index1->Parts[ i ],
			Part2 = Index2->Parts[ i ];

		if( Part1.ValueType != Part2.ValueType )
		{
			const char
				*Name1 = SecDbValueNameFromType( Part1.ValueType ),
				*Name2 = SecDbValueNameFromType( Part2.ValueType );

			if( !Name1 )
				Name1 = "<Unknown VT>";
			if( !Name2 )
				Name2 = "<Unknown VT>";
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": ValueType mismatch at Parts[ %d ] (%s != %s)", i, Name1, Name2 );
		}
		if( Part1.Flags != Part2.Flags )
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": Flags mismatch at Parts[ %d ] (%d != %d)", i, Part1.Flags, Part2.Flags );
		if( Part1.ByteWidth != Part2.ByteWidth )
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": ByteWidth mismatch at Parts[ %d ] (%d != %d)", i, Part1.ByteWidth, Part2.ByteWidth );
	}

	for( i = 0; i < Index1->ClassCount; ++i )
		if( Index1->Classes[ i ] != Index2->Classes[ i ] )
		{
			const char
				*Name1 = SecDbClassNameFromType( Index1->Classes[ i ] ),
				*Name2 = SecDbClassNameFromType( Index2->Classes[ i ] );

			if( !Name1 )
				Name1 = "<Unknown Class>";
			if( !Name2 )
				Name2 = "<Unknown Class>";
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": Mismatch at Classes[ %d ] (%s != %s)", i, Name1, Name2 );
		}

	for( i = 0; i < Index1->ConstraintCount; ++i )
	{
		SDB_INDEX_CONSTRAINT
			Con1 = Index1->Constraints[ i ],
			Con2 = Index2->Constraints[ i ];

		if( stricmp( Con1.VtName, Con2.VtName ) )
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": VtName mismatch at Constraints[ %d ] (%s != %s)", i, Con1.VtName, Con2.VtName );
		if( Con1.DataType != Con2.DataType )
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": DataType mismatch at Constraints[ %d ] (%s != %s)", i, Con1.DataType->Name, Con2.DataType->Name );
		if( Con1.ConstraintType != Con2.ConstraintType )
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": ConstraintType mismatch at Constraints[ %d ] (%d != %d)", i, Con1.ConstraintType, Con2.ConstraintType );
		if( Con1.DataType == DtDouble )
		{
			if( Con1.Value.NumericValue != Con2.Value.NumericValue )
				return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": Value mismatch at Constraints[ %d ] (%f != %f)", i, Con1.Value.NumericValue, Con2.Value.NumericValue );
		}
		else
		{
			if( stricmp( Con1.Value.StringValue, Con2.Value.StringValue ) )
				return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": Value mismatch at Constraints[ %d ] (%s != %s)", i, Con1.Value.StringValue, Con2.Value.StringValue );
		}
	}

	return TRUE;
}


/****************************************************************
**	Routine: IndexOpen
**	Returns: TRUE/Err
**	Action : Makes sure an index is open
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "IndexOpen"

static int IndexOpen(
	SDB_INDEX	*Index
)
{
	if( !Index )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Index arg missing" );

	if( !Index->OpenFlag )
	{
		SDB_INDEX
				*Index1;

		int		ClassNum,
				NeedRight;

		SDB_DB	*pDbUpdate = NULL,
				*pDbForClass = NULL;


		if( !Index->pDb->IsVirtual() )
			return Index->OpenFlag = ( *Index->pDb->pfIndexOpen )( Index );

		if( Index->pDb->DbType & SDB_NO_SECURITY_DATABASE )
			return Err( SDB_ERR_INDEX_INVALID, "Index '%s' not found because database '%s has SDB_NO_SECURITY_DATABASE'", 
						Index->Name, Index->pDb->Left->FullDbName.c_str() );
/*
**	Get the dbHandle from update db. Verify that update db is the same for all classes, and
**	return an error if index classes have different update dbs.
*/
		for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
		{
			pDbForClass = SecDbDbUpdateGet( Index->pDb, Index->Classes[ ClassNum ] );

			if( pDbForClass )
            {
				if( !pDbUpdate )
					pDbUpdate = pDbForClass;
				else if( pDbForClass != pDbUpdate )
					return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME "( %s ): Default db is %s, but %s class updates in %s", Index->Name, pDbUpdate->FullDbName.c_str(), SecDbClassNameFromType( Index->Classes[ ClassNum ] ), pDbForClass->FullDbName.c_str() );
            }
		}

		if( !pDbUpdate )
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME "( %s ): No update db found", Index->Name );

		if( !(Index1 = SecDbIndexFromName( Index->Name, pDbUpdate )))
			return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": Index '%s' not found for update database '%s'", Index->Name, pDbUpdate->FullDbName.c_str() );
		if( !IndexOpen( Index1 ))
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to open index for update db", Index->Name );
		if( !( Index->Handle = Index1->Handle ) )
			return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ): Got NULL db handle from %s", Index->Name, pDbUpdate->FullDbName.c_str() );

		if( !( Index->pDb->Left->DbType & SDB_NO_SECURITY_DATABASE ) )
		{
			if( !(Index1 = SecDbIndexFromName( Index->Name, Index->pDb->Left )))
				return Err( SDB_ERR_INDEX_INVALID, "Index '%s' not found for child database '%s'", Index->Name, Index->pDb->Left->FullDbName.c_str() );
			if( !IndexOpen( Index1 ))
				return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to open index for left db" );
			
			NeedRight = FALSE;					
		}
		else
			NeedRight = TRUE;										

		if(   !Index->pDb->Right 
			|| ( Index->pDb->Right->DbType & SDB_NO_SECURITY_DATABASE ) )
			return Index->OpenFlag = !NeedRight;
		
		if( !(Index1 = SecDbIndexFromName( Index->Name, Index->pDb->Right )))
			return Err( SDB_ERR_INDEX_INVALID, "Index '%s' not found for child database '%s'", Index->Name, Index->pDb->Right->FullDbName.c_str() );

		if( !IndexCompare( Index, Index1 ) )
			return ErrMore( "Index '%s' mismatch between %s & %s", Index->Name, Index->pDb->Right->FullDbName.c_str(), Index->pDb->Left->FullDbName.c_str() );

		Index->OpenFlag = IndexOpen( Index1 );
		
		return Index->OpenFlag;
	}

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexClassExists
**	Returns: TRUE, if successful; FALSE, otherwise
**	Action : Is ClassName a member of Index?
****************************************************************/

static int SecDbIndexClassExists(
	SDB_INDEX	*Index,
	const char	*ClassName
)
{
	int		ClassNum;


	for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
		if( !stricmp( ClassName, SecDbClassNameFromType( Index->Classes[ ClassNum ] )))
			return TRUE;

	return FALSE;
}



/****************************************************************
**	Routine: SecDbIndexAddClassToExisting
**	Returns: TRUE, if successful; FALSE, otherwise
**	Action : Add the class represented by SecurityType to an index.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexAddClassToExisting"

int SecDbIndexAddClassToExisting(
	char			*IndexName,
	SDB_DB			*Database,
	SDB_SEC_TYPE	SecurityType
)
{
	SDB_INDEX
			*Index;

	const char
			*ClassName;

	SDB_OBJECT
			*SecurityDatabase;


	Index = SecDbIndexFromName( IndexName, Database );
	if( Index == NULL )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get index %s", IndexName );

	ClassName = SecDbClassNameFromType( SecurityType );
	if( ClassName == NULL )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get class" );

	if( SecDbIndexClassExists( Index, ClassName ))
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME ": class '%s' already in index", ClassName );

	SecurityDatabase = SecDbGetByName( "Security Database", Database, SDB_IGNORE_PATH );
	if( SecurityDatabase == NULL )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get Security Database" );

	if( !SecDbIndexAddClass( Index, SecurityType, TRUE ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to add class to index" );

	if( !SecDbUpdate( SecurityDatabase ))
	{
		SecDbFree( SecurityDatabase );
		SecDbIndexDeleteClass( Index, SecurityType, TRUE );

		return ErrMore( ARGCHECK_FUNCTION_NAME ": update failed" );
	}
	SecDbFree( SecurityDatabase );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexDeleteClassFromExisting
**	Returns: TRUE, if successful; FALSE, otherwise
**	Action : Remove the class represented by SecurityType from an index.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexDeleteClassFromExisting"

int SecDbIndexDeleteClassFromExisting(
	char			*IndexName,
	SDB_DB			*Database,
	SDB_SEC_TYPE	SecurityType
)
{
	SDB_INDEX
			*Index;

	const char
			*ClassName;

	SDB_OBJECT
			*SecurityDatabase;


	Index = SecDbIndexFromName( IndexName, Database );
	if( Index == NULL )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get index %s", IndexName );

	ClassName = SecDbClassNameFromType( SecurityType );
	if( ClassName == NULL )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get class" );

	if( !SecDbIndexClassExists( Index, ClassName ))
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME ": class not in index" );

	SecurityDatabase = SecDbGetByName( "Security Database", Database, SDB_IGNORE_PATH );
	if( SecurityDatabase == NULL )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get Security Database" );

	if( !SecDbIndexDeleteClass( Index, SecurityType, TRUE ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to delete class from index" );

	if( !SecDbUpdate( SecurityDatabase ))
	{
		SecDbFree( SecurityDatabase );
		SecDbIndexAddClass( Index, SecurityType, TRUE );

		return ErrMore( ARGCHECK_FUNCTION_NAME ": update failed" );
	}
	SecDbFree( SecurityDatabase );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexNew
**	Returns: Pointer to new index
**			 NULL if index couldn't be created
**	Summary: Create a new index structure
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexNew"

SDB_INDEX *SecDbIndexNew(
	const char	*Name,		// Name of index to create
	SDB_DB		*pDb
)
{
	SDB_INDEX
			*Index;


	if( strlen( Name ) >= SDB_INDEX_NAME_SIZE )
		return (SDB_INDEX *) ErrN( ERR_INVALID_STRING, ARGCHECK_FUNCTION_NAME "( %s ), name too long", Name );
	if( !( pDb = DbToPhysicalDb( pDb )))
		return (SDB_INDEX *) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s, NULL ) @", Name );

	pDb = SecDbDefaultDbUpdateGet( pDb );

	Index = (SDB_INDEX *) calloc( 1, sizeof( SDB_INDEX ));
	if( !Index )
		Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );
	else
	{
		strcpy( Index->Name, Name );
		Index->pDb = pDb;
		Index->BuildingIndex = TRUE;
	}

	return Index;
}



/****************************************************************
**	Routine: SecDbIndexAddPart
**	Returns: TRUE	- Index part added to index without error
**			 FALSE	- Error adding new part to the index
**	Summary: Add a new part (ValueType) to an index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexAddPart"

int SecDbIndexAddPart(
	SDB_INDEX		*Index,
	SDB_VALUE_TYPE	ValueType,
	int				Flags,
	int				ByteWidth
)
{
	DT_INFO
			*DataTypeInfo;

	DT_VALUE
			TmpValue1,
			TmpValue2;


	if( Index->PartCount >= SDB_MAX_INDEX_PARTS )
		return Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME "( %s), Can't exceed %d parts", Index->Name, SDB_MAX_INDEX_PARTS );

	TmpValue2.DataType = ValueType->DataType;
	if( !DT_OP( DT_MSG_INFO, &TmpValue1, &TmpValue2, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", Index->Name );

	DataTypeInfo = (DT_INFO *) TmpValue1.Data.Pointer;
	if( DataTypeInfo->Flags & DT_FLAG_NUMERIC )
	{
		ByteWidth = sizeof( double );
		Flags |= SDB_INDEX_NUMERIC;
	}
	else
	{
		if( !ByteWidth )
			ByteWidth = SDB_SEC_NAME_SIZE;
		Flags |= SDB_INDEX_STRING;
	}
	DtFreeInfo( DataTypeInfo );

	Index->Parts[ Index->PartCount ].ValueType	= ValueType;
	Index->Parts[ Index->PartCount ].Flags		= Flags;
	Index->Parts[ Index->PartCount ].ByteWidth	= ByteWidth;

	Index->PartCount++;
	Index->TotalByteWidth += ByteWidth;

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexAddConstraint
**	Returns: TRUE	- Index constraint added to index without error
**			 FALSE	- Error adding new constraint to the index
**	Summary: Add a new value type value constraint to a sparse index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexAddConstraint"

int SecDbIndexAddConstraint(
	SDB_INDEX		*Index,
	const char		*VtName,
	short			Type,
	DT_VALUE        *Value
)
{
	int		ClassNum;

	const char
			*ClassName;

	if( Index->ConstraintCount >= SDB_MAX_INDEX_CONSTRAINTS )
		return Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME "( %s ), Can't exceed %d constraints", Index->Name, SDB_MAX_INDEX_CONSTRAINTS );

	if( strlen( VtName ) >= SDB_VALUE_NAME_SIZE )
		return Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME "( %s ), Constraint value type name '%s' too long, max %d", Index->Name, VtName, SDB_VALUE_NAME_SIZE );

	for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
	{
		ClassName = SecDbClassNameFromType( Index->Classes[ ClassNum ]);
		if( ClassName == NULL )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get class" );

		// Constrained indices not supported on Positions because Postions are subject to 
		// incremental updates. Constrained indices do not work with incremental updates
		// because new security value type values are not established at incremental
		// index update time to allow for checking whether or not constraints are met.
		if( !stricmp( ClassName, "Position" ))
			return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME ": Constrained indices not supported on %s class as it is subject to incremental updates", ClassName );
	}

	strcpy( (char *) &Index->Constraints[ Index->ConstraintCount ].VtName, VtName );
	Index->Constraints[ Index->ConstraintCount ].DataType		= Value->DataType;

	Index->Constraints[ Index->ConstraintCount ].ConstraintType	= Type;

	if( Value->DataType == DtDouble )
		Index->Constraints[ Index->ConstraintCount ].Value.NumericValue	= Value->Data.Number;
	else if( Value->DataType == DtString )
	{	
		if( strlen( (char *) Value->Data.Pointer ) >= SDB_CONSTRAINT_SIZE )
			return Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME "( %s ), Constraint value '%s' for value type '%s' too long, max %d", Index->Name, (const char *) Value->Data.Pointer, VtName, SDB_CONSTRAINT_SIZE );

		strcpy( (char *) &Index->Constraints[ Index->ConstraintCount ].Value.StringValue, (char *) Value->Data.Pointer );
	}	
	else
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ), Only Double and String supported as constraint types", Index->Name );

	Index->ConstraintCount++;

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexAdd
**	Returns: TRUE	- Index added without error
**			 FALSE	- Error adding index
**	Summary: Add an index to a database
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexAdd"

int SecDbIndexAdd(
	SDB_INDEX	*Index
)
{
	SDB_DB	*pDb;


/*
**	Lookup the database information from Index->DbID
*/

	if( !( pDb = DbToPhysicalDb( Index->pDb )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) Db=<null> @", Index->Name );

	// This cannot be done inside a transaction, because the server
	// will not have processed the create by the time the build is
	// done.
	if( pDb->TransDepth )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME "(): @, cannot add an index inside a transaction." );

/*
**	Check the hash table to see if the index name already exists
*/

	if( HashLookup( SecDbIndexHash, Index ))
		return Err( SDB_ERR_INDEX_ALREADY_EXISTS, ARGCHECK_FUNCTION_NAME "( %s ) @", Index->Name );


/*
**	Build the index
*/

	Index->pDb 	= pDb;
	Index->BuildingIndex = FALSE;

	if( !SecDbIndexCreate( Index, pDb ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to create index" );

	if( !SecDbIndexBuild( Index, pDb ))
	{
		ERR_OFF();
		SecDbIndexDestroy( Index->Name, pDb );
		ERR_ON();
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to build index" );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexRegister
**	Returns: TRUE/FALSE
**	Action : Register a DescriptorOnly index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexRegister"

int SecDbIndexRegister(
	SDB_INDEX	*Index
)
{
	SDB_DB	*pDb;


	if( !Index )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Index arg missing" );

	// First, validate the index
	if( !( pDb = DbToPhysicalDb( Index->pDb )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) Db=<null>, @", Index->Name );

	if( HashLookup( SecDbIndexHash, Index ))
		return Err( SDB_ERR_INDEX_ALREADY_EXISTS, ARGCHECK_FUNCTION_NAME "( %s ) @", Index->Name );

	Index->pDb 	= pDb;
	Index->BuildingIndex = FALSE;

	if( !Index->DescriptorOnly || 0 == Index->IndexID )
		return Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) Can only register 'DescriptorOnly' indicies", Index->Name );

	// Add the index to the global hash
	HashInsert( SecDbIndexHash, Index, Index );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexCreate
**	Returns: TRUE/FALSE
**	Action : Create an index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexCreate"

int SecDbIndexCreate(
	SDB_INDEX	*Index,
	SDB_DB		*pDb
)
{
	SDB_TRANS_PART
			Trans;

	SDB_INDEX_DESCRIPTOR
			*NewIdxDesc;

	int		PartNum;

	SDB_OBJECT
			*SecPtr;


/*
**	Get the Security Database object
*/

	if( !( SecPtr = SecDbGetByName( "Security Database", pDb, SDB_IGNORE_PATH )))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to get Security Database", Index->Name );

/*
**	Create the index
*/

	strcpy( Trans.Data.Begin.SecName, Index->Name );
	Trans.Data.Begin.SecType		= 0;
	Trans.Data.Begin.DbID			= Index->pDb->DbID;
	Trans.Data.Begin.TransType		= SDB_TRAN_INDEX_CREATE;
	Trans.Data.Begin.SourceTransID	= 0;
	if( !SecDbTransactionAdd( Index->pDb, SDB_TRAN_BEGIN, &Trans, NULL ))
	{
		SecDbFree( SecPtr );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to begin transaction", Index->Name );
	}


/*
**	Translate Index to IndexDescriptor
*/

	SecDbTransPartInit( &Trans, SDB_TRAN_INDEX_CREATE );
	NewIdxDesc = Trans.Data.IndexCreate.IndexDescriptor;
	strcpy( NewIdxDesc->Name, Index->Name );
	NewIdxDesc->TotalByteWidth 	= Index->TotalByteWidth;
	NewIdxDesc->PartCount		= Index->PartCount;
	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
	{
		strcpy( NewIdxDesc->Parts[ PartNum ].ValueTypeName,	Index->Parts[ PartNum ].ValueType->Name );
		strcpy( NewIdxDesc->Parts[ PartNum ].DataTypeName,	Index->Parts[ PartNum ].ValueType->DataType->Name );
		NewIdxDesc->Parts[ PartNum ].ByteWidth	= Index->Parts[ PartNum ].ByteWidth;
		NewIdxDesc->Parts[ PartNum ].Flags		= Index->Parts[ PartNum ].Flags;
	}


/*
**	Save the index information in the security database object
*/

	HashInsert( SecDbIndexHash, Index, Index );
	if( !SecDbUpdate( SecPtr ))
	{
		SecDbTransactionAdd( Index->pDb, SDB_TRAN_ABORT, NULL, NULL );
		HashDelete( SecDbIndexHash, Index );
		SecDbFree( SecPtr );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to update Security Database", Index->Name );
	}
	SecDbFree( SecPtr );


/*
**	Finish the transaction
*/

	if( !SecDbTransactionAdd( Index->pDb, SDB_TRAN_INDEX_CREATE, &Trans, NULL ))
	{
		SecDbTransactionAdd( Index->pDb, SDB_TRAN_ABORT, NULL, NULL );
		HashDelete( SecDbIndexHash, Index );
		SecDbFree( SecPtr );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to add INDEX_CREATE transaction", Index->Name );
	}

	if( !SecDbTransCommitForce( Index->pDb ))
	{
		HashDelete( SecDbIndexHash, Index );
		SecDbFree( SecPtr );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to commit transaction", Index->Name );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexBuild
**	Returns: TRUE	- Index built without error
**			 FALSE	- Error building index
**	Action : Build an index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexBuild"

static int SecDbIndexBuild(
	SDB_INDEX	*Index,
	SDB_DB		*pDb
)
{
	SDB_SEC_TYPE
			SecType;

	SDB_OBJECT
			*SecPtr;

	SDB_INDEX_POINTER
			*IndexPtr;

	SDB_CLASS_INFO
			*ClassInfo;

	char	SecName[ SDB_SEC_NAME_SIZE ],
			Buffer[ 132 ];

	int		ClassNum;

	long	Count;


/*
**	Open the index
*/

	if( Index->pDb->IsVirtual() )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME "( %s, %s ): Database is virtual", Index->Name, pDb->FullDbName.c_str() );

	if( !(*Index->pDb->pfIndexOpen)( Index ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to open index" );
	Index->OpenFlag = TRUE;


/*
**	Attach the index to the various security classes
*/

	for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
	{
		// FIX -- If class is not found!!!
		ClassInfo = (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) Index->Classes[ ClassNum ] );
		if( ClassInfo )
		{
			Index->Classes[ ClassNum ] = ClassInfo->Type;

			IndexPtr = (SDB_INDEX_POINTER *) calloc( 1, sizeof( SDB_INDEX_POINTER ));
			IndexPtr->Index = Index;
			IndexPtr->Next	= ClassInfo->IndexList;
			ClassInfo->IndexList = IndexPtr;
		}
	}


/*
**	Run through all the security classes that should be indexed,
**	and add the records to the index
*/

	Count = 0;
	if( !SecDbTransBegin( pDb, "IndexBuild" ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ) failed at begining", Index->Name );

	for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
	{
		SecType = Index->Classes[ ClassNum ];
		*SecName = '\0';

		// FIX -- Use SecDbNameLookupMany

		while( SecDbNameLookup( SecName, SecType, SDB_GET_GREATER, pDb ))
		{
			SecPtr = SecDbGetByName( SecName, pDb, SDB_IGNORE_PATH );
			if( SecPtr )
			{
				if( !SecDbIndexAddIfMatch( Index, SecPtr ))
				{
					SecDbFree( SecPtr );
					return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to add %s to index %s", SecName, Index->Name );
				}
				SecDbFree( SecPtr );
				SecDbRemoveFromDeadPool( pDb, 0, SecName );
			}
			else
				return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to load %s", SecName );

			Count++;
			if( !( Count % SecDbIndexBuildPartCount ) )
			{
				if( !SecDbTransCommit( pDb ))
				{
					SecDbMessage( ERR_STR_DONT_CHANGE );
					return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ) failed after %ld", Index->Name, Count - 100L );
				}
				sprintf( Buffer, "Indexed %ld - %s", Count, SecName );
				SecDbMessage( Buffer );
				if( !SecDbTransBegin( pDb, "IndexBuild" ))
				{
					SecDbMessage( ERR_STR_DONT_CHANGE );
					return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ) failed after %ld", Index->Name, Count );
				}
			}
		}
	}

	if( Count >= SecDbIndexBuildPartCount )
		SecDbMessage( ERR_STR_DONT_CHANGE );

	if( !SecDbTransCommit( pDb ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ) failed after %ld", Index->Name, Count - Count % 100L );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexRebuild
**	Returns: TRUE	- Index rebuilt without error
**			 FALSE	- Error rebuilding index
**	Action : Rebuild an existing index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexRebuild"

int SecDbIndexRebuild(
	char	*Name,	// Name of index to rebuild
	SDB_DB	*pDb	// Database to find index
)
{
	SDB_INDEX
			*Index,
			*NewIndex;

	int		PartNum,
			ClassNum,
			ConstraintNum;

	DT_VALUE
			Value;

	if(	!(Index = SecDbIndexFromName( Name, pDb )))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s ): Failed to get index", Name, pDb->FullDbName.c_str() );

	if( !(NewIndex = SecDbIndexNew( Name, pDb )))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s ): Failed to get new index", Name, pDb->FullDbName.c_str() );

	if( Index->pDb->IsVirtual() )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME "( %s, %s ): Database is virtual", Name, pDb->FullDbName.c_str() );

	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		SecDbIndexAddPart(
				NewIndex,
				Index->Parts[ PartNum ].ValueType,
				Index->Parts[ PartNum ].Flags,
				Index->Parts[ PartNum ].ByteWidth );

	for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
		SecDbIndexAddClass( NewIndex, Index->Classes[ ClassNum ], FALSE );

	NewIndex->Flags	= Index->Flags;

	for( ConstraintNum = 0; ConstraintNum < Index->ConstraintCount; ConstraintNum++ )
	{	
		Value.DataType	= Index->Constraints[ ConstraintNum ].DataType;

		if( Value.DataType == DtDouble )
			Value.Data.Number	= Index->Constraints[ ConstraintNum ].Value.NumericValue;
		else
			Value.Data.Pointer	= Index->Constraints[ ConstraintNum ].Value.StringValue;

		SecDbIndexAddConstraint(
				NewIndex,
				Index->Constraints[ ConstraintNum ].VtName,
				Index->Constraints[ ConstraintNum ].ConstraintType,
				&Value );
	}

	if( !SecDbIndexDestroy( Name, pDb ))
	{
		// FIX - Memory allocated to NewIndex
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s ): Failed to destroy old index", Name, pDb->FullDbName.c_str() );
	}

	return SecDbIndexAdd( NewIndex );
}



/****************************************************************
**	Routine: SecDbIndexAddClass
**	Returns: TRUE	- Security class added to index without error
**			 FALSE	- Error adding new security class to the index
**	Summary: Add a security class to an index.  Normally one should
**			 not add a class to an existing index, but it's allowed
**			 if ModifyExistingIndex is TRUE.  Be very careful if you
**			 use this feature.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexAddClass"

int SecDbIndexAddClass(
	SDB_INDEX		*Index,
	SDB_SEC_TYPE	SecType,
	int				ModifyExistingIndex
)
{
	const char
			*ClassName;

/*
**	Cannot add class to existing index
*/

	if( !Index->BuildingIndex && !ModifyExistingIndex )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME ": Adding class %d to existing index %s", SecType, Index->Name );

	ClassName = SecDbClassNameFromType( SecType );
	if( ClassName == NULL )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get class" );
	
	// Constrained indices not supported on Positions because Postions are subject to 
	// incremental updates. Constrained indices do not work with incremental updates
	// because new security value type values are not established at incremental
	// index update time to allow for checking whether or not constraints are met.
	if( !stricmp( ClassName, "Position" ) && Index->ConstraintCount )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME ": Constrained indices not supported on %s class as it is subject to incremental updates", ClassName );

/*
**	Add the class to the index structure
*/

	Index->ClassCount++;

	if( Index->Classes )
		Index->Classes = (SDB_SEC_TYPE *) realloc( Index->Classes, Index->ClassCount * sizeof( SDB_SEC_TYPE ));
	else
		Index->Classes = (SDB_SEC_TYPE *) malloc( Index->ClassCount * sizeof( SDB_SEC_TYPE ));

	Index->Classes[ Index->ClassCount - 1 ] = SecType;


	return TRUE;
}


/****************************************************************
**	Routine: SecDbIndexDeleteClass
**	Returns: TRUE	- Security class deleted from index without error
**			 FALSE	- Error deleting security class from the index
**	Summary: Delete a security class from an index.   Normally one should
**			 not delete a class from an existing index, but it's allowed
**			 if ModifyExistingIndex is TRUE.  Be very careful if
**			 use this feature.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexDeleteClass"

int SecDbIndexDeleteClass(
	SDB_INDEX		*Index,
	SDB_SEC_TYPE	SecType,
	int				ModifyExistingIndex
)
{
	SDB_SEC_TYPE
			*NewClassList,
			*ClassListPtr;

	int		ClassNum;


/*
**	Cannot remove class from existing index
*/

	if( !Index->BuildingIndex && !ModifyExistingIndex )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME 
					": Deleting class %d from existing index %s", SecType, Index->Name );

/*
**	Find the class within the index structure
*/

	if( !Index->ClassCount )
		return Err( SDB_ERR_CLASS_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s, %d ) @, ClassCount is zero", Index->Name, SecType );

	NewClassList = ClassListPtr = (SDB_SEC_TYPE *) calloc( Index->ClassCount, sizeof( SDB_SEC_TYPE ));
	for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
		if( Index->Classes[ ClassNum ] != SecType )
			*ClassListPtr++ = Index->Classes[ ClassNum ];

	if( (ClassListPtr - NewClassList) == Index->ClassCount )
	{
		free( NewClassList );
		return Err( SDB_ERR_CLASS_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s, %d ) @", Index->Name, SecType );
	}

	Index->ClassCount--;
	free( Index->Classes );
	Index->Classes = NewClassList;

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexDestroy
**	Returns: TRUE	- Index destroyed without error
**			 FALSE	- Error destroying an index
**	Summary: Destroy an index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexDestroy"

int SecDbIndexDestroy(
	char		*Name,		// Name of index to remove
	SDB_DB		*pDb		// Database of index
)
{
	SDB_INDEX
			*Index;

	SDB_TRANS_PART
			Trans;

	SDB_OBJECT
			*SecPtr;


/*
**	Get the index
*/

	// take a local copy of the name, it may be that Name is Index.Name, so after destroy index
	// we can get into trouble
	string
		sName( Name );
	Name = const_cast<char*> ( sName.c_str() );

	if( !pDb || !(Index = SecDbIndexFromName( Name, SecDbDefaultDbUpdateGet( pDb ))))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", Name );
	pDb  = Index->pDb;


/*
**	Get the Security Database object
*/

	if( !( SecPtr = SecDbGetByName( "Security Database", pDb, SDB_IGNORE_PATH )))
		return ErrMore( ARGCHECK_FUNCTION_NAME " '%s' failed", Name );


/*
**	Start the transaction
*/

	strcpy( Trans.Data.Begin.SecName, Index->Name );
	Trans.Data.Begin.SecType		= 0;
	Trans.Data.Begin.DbID			= pDb->DbID;
	Trans.Data.Begin.TransType		= SDB_TRAN_INDEX_DESTROY;
	Trans.Data.Begin.SourceTransID	= 0;
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &Trans, NULL ))
	{
		SecDbFree( SecPtr );
		return( ErrMore( ARGCHECK_FUNCTION_NAME " '%s' failed", Index->Name ));
	}


/*
**	Store the new list of indices
*/

	SecDbIndexClose( Index, pDb, TRUE );
	if( !SecDbUpdate( SecPtr ))
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		SecDbReload( SecPtr );
		SecDbFree( SecPtr );
		return ErrMore( ARGCHECK_FUNCTION_NAME " '%s' failed", Name );
	}
	SecDbFree( SecPtr );


/*
**	Send the message to the driver
*/

	strcpy( Trans.Data.IndexDestroy.IndexName, Name );
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_INDEX_DESTROY, &Trans, NULL ))
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return( ErrMore( ARGCHECK_FUNCTION_NAME " '%s' failed", Name ));
	}

	if( !SecDbTransCommitForce( pDb ))
		return( ErrMore( ARGCHECK_FUNCTION_NAME " '%s' failed", Name ));

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexCheckConstraints
**	Returns: TRUE	- Constraint check completed without error
**			 FALSE	- Error performing constraint checks
**	Action : Determine whether or not a security's value types
**			 fit the constraints of a constrained index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexCheckConstraints"

int SecDbIndexCheckConstraints(
	SDB_INDEX	*Index,		// Index to check against
	SDB_OBJECT  *SecPtr,    // Security to check
	int			*Match
)
{
	DT_VALUE
			*Value;

	SDB_VALUE_TYPE
			ValueType;

	int		ConstraintNum;


	*Match	= TRUE;
	for( ConstraintNum = 0; ConstraintNum < Index->ConstraintCount; ConstraintNum++ )
	{
		ValueType	= SecDbValueTypeFromName( Index->Constraints[ ConstraintNum ].VtName, NULL );
		if( !*Index->Constraints[ ConstraintNum ].VtName || !ValueType || !ValueType->DataType || !ValueType->DataType->Func )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Error getting value type from name '%s'", Index->Constraints[ ConstraintNum ].VtName );
		
		Value		= SecDbGetValue( SecPtr, ValueType );
		if( !Value )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Error getting '%s' value from %s", ValueType->Name, SecPtr->SecData.Name );

		if( Value->DataType == Index->Constraints[ ConstraintNum ].DataType )
		{
			if( Value->DataType == DtDouble )
				*Match	= ( Value->Data.Number == Index->Constraints[ ConstraintNum ].Value.NumericValue );
			else if( Value->DataType == DtString )
				*Match	= !stricmp( (char *) Value->Data.Pointer, Index->Constraints[ ConstraintNum ].Value.StringValue ); 
			else	
				return Err( ERR_INVALID_NUMBER, ARGCHECK_FUNCTION_NAME ": Error comparing unsupported constraint type %s", Index->Constraints[ ConstraintNum ].DataType->Name );

			if( !*Match )
				break;
		}
		else
			return Err( ERR_NOT_CONSISTENT, ARGCHECK_FUNCTION_NAME ": Datatype mismatch between value and constraint for '%s'", Index->Constraints[ ConstraintNum ].VtName );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexValidate
**	Returns: TRUE	- Index is valid (as far as SecDb knows...)
**			 FALSE	- Index invalid
**	Action : Validate that the contents of an index match the
**			 contents of the database
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexValidate"

int SecDbIndexValidate(
	char		*Name,		// Name of index to validate
	SDB_DB		*pDb,		// Database of index
	int			ErrorLimit	// Max number of errors to find before exit
)
{
	DT_VALUE
			*Value;

	SDB_INDEX_POS
			*IndexPosIndex,
			*IndexPosSec;

	SDB_INDEX
			*Index;

	SDB_OBJECT
			*SecPtr;

	SDB_SEC_TYPE
			SecType;

	HASH	*HashValidate;

	char	LookupName[ SDB_SEC_NAME_SIZE ],
			Buffer[ 132 ],
			*ErrBuf,
			*ErrPtr,
			*ErrEndPtr,
			*SecName;

	int		PartNum,
			ClassNum,
			ErrorCount	= 0,
			Match,
			Status;

	short	Constrained;

	long	Count;


/*
**	Get the index
*/

	if( !(Index = SecDbIndexFromName( Name, pDb )))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", Name );

	if( Index->pDb->IsVirtual() )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME "( %s, %s ): Database is virtual", Name, Index->pDb->FullDbName.c_str() );

/*
**	Allocate our own error buffer
*/

	if( !(ErrPtr = ErrBuf = (char *) malloc( SDB_INDEX_VALIDATE_BUFFER_SIZE )))
		return Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME " - @" );
	ErrEndPtr = ErrBuf + (SDB_INDEX_VALIDATE_BUFFER_SIZE - SDB_INDEX_VALIDATE_BUFFER_MARGIN);


/*
**	Go through each item in the index and validate
*/

	IndexPosIndex	= SecDbIndexPosCreate( Index );
	IndexPosSec		= SecDbIndexPosCreate( Index );
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	HashValidate	= HashCreate( "Valid Hash", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, MemPoolCreate() );

	Constrained		= Index->Flags & SDB_INDEX_CONSTRAINED;

	Count = 0;
	for( SecName = SecDbIndexGet( IndexPosIndex, SDB_GET_FIRST );
			SecName;
			SecName = SecDbIndexGet( IndexPosIndex, SDB_GET_NEXT ))
	{
		Count++;
		if( !(Count % 100L) )
		{
			sprintf( Buffer, "Validating %ld - %s", Count, SecName );
			SecDbMessage( Buffer );
		}

		// Get the actual security to compare against
		if( !(SecPtr = SecDbGetByName( SecName, Index->pDb, SDB_IGNORE_PATH )))
		{
			ErrorCount++;
			ErrPtr += sprintf( ErrPtr, "'%s' was in the index, but -- %s\n", SecName, ErrGetString() );
			if( ErrorLimit && (ErrorCount > ErrorLimit ))
				goto CLEANUP;
			if( ErrPtr > ErrEndPtr )
				goto CLEANUP;
			continue;
		}

		// Format the actual data into an index position structure
		for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		{
			Value = SecDbGetValue( SecPtr, Index->Parts[ PartNum ].ValueType );
			if( Value )
				SecDbIndexPosSetValue( IndexPosSec, Index->Parts[ PartNum ].ValueType, Value );
		}

		// If this is a constrained index, make sure the security isn't
		// present in violation of the index constraints
		if( Constrained )
		{
			Status	= SecDbIndexCheckConstraints( Index, SecPtr, &Match );
		
			SecDbFree( SecPtr );	// Free the actual security	

			if( !Status )
				return ErrMore( ARGCHECK_FUNCTION_NAME "Error checking %s (indexed) vs. %s constraints", SecName, Index->Name );

			if( !Match )
			{
				ErrorCount++;
				ErrPtr += sprintf( ErrPtr, "'%s' - Security data doesn't match index constraints\n", SecName );
				if( ErrorLimit && (ErrorCount > ErrorLimit ))
					goto CLEANUP;
				if( ErrPtr > ErrEndPtr )
					goto CLEANUP;
				continue;	
			}
		}
		else
			SecDbFree( SecPtr );	// Free the actual security	

		// Compare the security data with the index data
		if( SecDbIndexPosCompare( IndexPosSec, IndexPosIndex ))
		{
			ErrorCount++;
			ErrPtr += sprintf( ErrPtr, "'%s' - Index data doesn't match security data\n", SecName );
			if( ErrorLimit && (ErrorCount > ErrorLimit ))
				goto CLEANUP;
			if( ErrPtr > ErrEndPtr )
				goto CLEANUP;
		}

		SecName = MemPoolStrDup( HashValidate->MemPool, SecName );
		HashInsert( HashValidate, SecName, SecName );
	}

	if( ERR_DATABASE_FAILURE == ErrNum )
	{
		ErrorCount++;
		goto CLEANUP;
	}


/*
**	Run through all securities in the database that should be in the index
*/

	Count = 0;
	for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
	{
		SecType = Index->Classes[ ClassNum ];

		*LookupName = '\0';
		while( SecDbNameLookup( LookupName, SecType, SDB_GET_GREATER, Index->pDb ))
			if( !HashLookup( HashValidate, LookupName ))
			{
				// If this is a constrained index, it is correct for the security to
				// be absent from the index if it does not match the index constraints
				if( Constrained )
				{
					// Get the actual security to compare against
					if( !(SecPtr = SecDbGetByName( LookupName, Index->pDb, SDB_IGNORE_PATH )))
					{
						ErrorCount++;
						ErrPtr += sprintf( ErrPtr, "'%s' was found in name lookup, but -- %s\n", SecName, ErrGetString() );
						if( ErrorLimit && (ErrorCount > ErrorLimit ))
							goto CLEANUP;
						if( ErrPtr > ErrEndPtr )
							goto CLEANUP;
						continue;
					}

					Status	= SecDbIndexCheckConstraints( Index, SecPtr, &Match );
		
					SecDbFree( SecPtr );	// Free the actual security	

					if( !Status )
						return ErrMore( ARGCHECK_FUNCTION_NAME "Error checking %s (not indexed) vs. %s constraints", LookupName, Index->Name );
				}
				else
					Match	= TRUE;

				if( Match )
				{
					ErrorCount++;
					ErrPtr += sprintf( ErrPtr, "'%s' not in index\n", LookupName );
					if( ErrorLimit && (ErrorCount > ErrorLimit ))
						goto CLEANUP;
					if( ErrPtr > ErrEndPtr )
						goto CLEANUP;
				}
			}
	}


/*
**	Cleanup
*/

CLEANUP:
	SecDbIndexPosDestroy( IndexPosIndex	);
	SecDbIndexPosDestroy( IndexPosSec	);
	MemPoolDestroy( HashValidate->MemPool );
	if( ErrorCount )
		Err( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": %s - @ - %d Errors found\n%s", Name, ErrorCount, ErrBuf );

	free( ErrBuf );
	return( ErrorCount ? FALSE : TRUE );
}


/****************************************************************
**	Routine: SecDbIndexRepairObject
**	Returns: TRUE	- Object repaired without error
**			 FALSE	- Error repairing object
**	Action : Rebuild index information for a single object
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexRepairObject"

int SecDbIndexRepairObject(
	char		*SecName,
	SDB_DB		*pDb		// Database where SecName is to be fixed
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_INDEX_POINTER
			*IndexPtr;

	SDB_INDEX
			*Index;

	char	Description[ SDB_SEC_NAME_SIZE ];

	int		Status = TRUE;

	HASH_ENTRY_PTR
			Ptr;


	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "() @" );
	if( pDb->IsVirtual() )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME "( %s ): Database %s is virtual", SecName, pDb->FullDbName.c_str() );


/*
**	Start the transaction
*/

	sprintf( Description, "IndexRepair %.*s", (int) sizeof( Description ) - 13, SecName );
	if( !SecDbTransBegin( pDb, Description ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to begin transaction" );


/*
**	Try to delete from every index (note not SecDbIndexDeleteIfMatch - we want it gone)
*/

	HASH_FOR( Ptr, SecDbIndexHash )
	{
		Index = (SDB_INDEX *) HashValue( Ptr );
		if( Index->pDb == pDb )
			Status = Status && SecDbIndexDeleteSecurity( Index, SecName );
	}


/*
**	If we can load the security reinsert into correct indices
*/

	SecPtr = SecDbGetByName( SecName, pDb, SDB_REFRESH_CACHE | SDB_IGNORE_PATH );
	if( SecPtr )
	{
		for( IndexPtr = SecPtr->Class->IndexList; IndexPtr; IndexPtr = IndexPtr->Next )
			if( IndexPtr->Index->pDb == SecPtr->Db )
				Status = Status && SecDbIndexAddIfMatch( IndexPtr->Index, SecPtr );

		SecDbFree( SecPtr );
	}


/*
**	Force (so invalide deletes will be ignored)
*/

	if( Status )
	{
		if( !SecDbTransCommitForce( pDb ))
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to commit transaction", SecName );
	}
	else
		SecDbTransAbort( pDb );

	return Status;
}



/****************************************************************
**	Routine: FillInValues
**	Returns: TRUE/FALSE
**	Action : Fill values into a value table
****************************************************************/

static int FillInValues(
	SDB_INDEX	*Index,
	DT_VALUE	*Values,
	SDB_OBJECT	*SecPtr
)
{
	SDB_VALUE_TYPE
			ValueType;

	DT_VALUE
			*Value;

	int		PartNum;

	char	*Str;


	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
	{
		ValueType = Index->Parts[ PartNum ].ValueType;
		if( (Value = SecDbGetValue( SecPtr, ValueType )))
		{
			if( !DTM_ASSIGN( &Values[ PartNum ], Value ))
			{
				while( --PartNum >= 0 )
					DTM_FREE( &Values[ PartNum ] );
				free( Values );
				return FALSE;
			}

			// Truncate strings
			if( Value->DataType == DtString )
			{
				Str = (char *) Values[ PartNum ].Data.Pointer;
				if( strlen( Str ) >= (unsigned int) Index->Parts[ PartNum ].ByteWidth )
					Str[ Index->Parts[ PartNum ].ByteWidth - 1 ] = '\0';
			}
		}
		else if( !DTM_LOW_LIMIT( &Values[ PartNum ], ValueType->DataType ))
			DTM_INIT( &Values[ PartNum ] );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexAddSecurity
**	Returns: TRUE	- Security added to index without error
**			 FALSE	- Couldn't add security to index
**	Action : Add a security to an index
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexAddSecurity"

int SecDbIndexAddSecurity(
	SDB_INDEX		*Index,		// Index to add security to
	SDB_OBJECT		*SecPtr		// Security to add to index
)
{
	SDB_TRANS_PART
			TransPart;


	if( !IndexOpen( Index ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to open index", Index->Name );

/*
**	Build an index position structure from the security
*/

	SecDbTransPartInit( &TransPart, SDB_TRAN_INDEX_INSERT );
	if( !FillInValues( Index, TransPart.Data.IndexOp.Data->Values, SecPtr ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to fill in value" );


/*
**	Add the security to the index
*/

	TransPart.Data.IndexOp.Data->Index = Index;
	strcpy( TransPart.Data.IndexOp.Data->SecName, SecPtr->SecData.Name );
	return SecDbTransactionAdd( Index->pDb, SDB_TRAN_INDEX_INSERT, &TransPart, NULL );
}



/****************************************************************
**	Routine: SecDbIndexDeleteSecurity
**	Returns: TRUE	- Security removed from index without error
**			 FALSE	- Error removing security from index
**	Action : Remove a security from an index
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexDeleteSecurity"

int SecDbIndexDeleteSecurity(
	SDB_INDEX		*Index,			// Index to delete security from
	const char		*SecName		// Name of security to remove from index
)
{
	SDB_TRANS_PART
			TransPart;


	if( !IndexOpen( Index ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", Index->Name );

/*
**	Send the delete message to the driver
*/

	strcpy( TransPart.Data.IndexDelete.IndexName, 	Index->Name );
	strcpy( TransPart.Data.IndexDelete.SecName, 	SecName );
	TransPart.Data.IndexDelete.Index = Index;
	TransPart.Data.IndexDelete.IgnoreErrors = FALSE;
	return SecDbTransactionAdd( Index->pDb, SDB_TRAN_INDEX_DELETE, &TransPart, NULL );
}



/****************************************************************
**	Routine: SecDbIndexUpdateSecurity
**	Returns: TRUE	- Security updated in index without error
**			 FALSE	- Error updating security in index
**	Action : Update a security in an index
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexUpdateSecurity"

int SecDbIndexUpdateSecurity(
	SDB_INDEX		*Index,			// Index to add security to
	SDB_OBJECT		*SecPtr 		// Security to add to index
)
{
	SDB_TRANS_PART
			TransPart;


	if( !IndexOpen( Index ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", Index->Name );

/*
**	Format the new data into an index position structure
*/

	SecDbTransPartInit( &TransPart, SDB_TRAN_INDEX_UPDATE );
	if( !FillInValues( Index, TransPart.Data.IndexOp.Data->Values, SecPtr ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to fill in values" );


/*
**	Update the index database
*/

	TransPart.Data.IndexOp.Data->Index = Index;
	strcpy( TransPart.Data.IndexOp.Data->SecName, SecPtr->SecData.Name );
	return SecDbTransactionAdd( Index->pDb, SDB_TRAN_INDEX_UPDATE, &TransPart, NULL );
}



/****************************************************************
**	Routine: SecDbIndexIncremental
**	Returns: TRUE	- Security updated in index without error
**			 FALSE	- Error updating security in index
**	Action : Update a security in an index
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexIncremental"

int SecDbIndexIncremental(
	SDB_INDEX		*Index,			// Index to add security to
	SDB_OBJECT		*SecPtr 		// Security to add to index
)
{
	SDB_TRANS_PART
			TransPart;


	if( !IndexOpen( Index ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ) Failed to open index", Index->Name );

	TransPart.Data.IndexIncremental.Index = Index;
	strcpy( TransPart.Data.IndexIncremental.SecName, SecPtr->SecData.Name );
	return SecDbTransactionAdd( Index->pDb, SDB_TRAN_INDEX_INCREMENTAL, &TransPart, NULL );

#if 0
	DT_VALUE
			*Values;

// FIX-Should values remain in here?  Maybe for BTRV

/*
**	Format the new data into an index position structure
*/

	int		PartNum;

	DT_VALUE
			*Values;


	SecDbTransPartInit( &TransPart, SDB_TRAN_INDEX_INCREMENTAL );
	Values = TransPart.Data.IndexOp.Data->Values;

	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
	{
		if( !DTM_LOW_LIMIT( &Values[ PartNum ], Index->Parts[ PartNum ].ValueType->DataType ))
			DTM_INIT( &Values[ PartNum ] );
	}


/*
**	Update the index database
*/

	TransPart.Data.IndexOp.Data->Index = Index;
	strcpy( TransPart.Data.IndexOp.Data->SecName, SecPtr->SecData.Name );
	return SecDbTransactionAdd( Index->pDb, SDB_TRAN_INDEX_INCREMENTAL, &TransPart, NULL );
#endif
}



/****************************************************************
**	Routine: SecDbIndexRenameSecurity
**	Returns: TRUE	- Security renamed in index without error
**			 FALSE	- Error renaming security in index
**	Action : Rename a security in an index
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexRenameSecurity"

int SecDbIndexRenameSecurity(
	SDB_INDEX		*Index,			// Index to add security to
	const SDB_SEC_NAME *NewSecName,  	// New name of security
	const SDB_SEC_NAME *OldSecName  	// Old name of security
)
{
	SDB_TRANS_PART
			TransPart;


	if( !IndexOpen( Index ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ) Failed to open index", Index->Name );

	strcpy( TransPart.Data.IndexRename.IndexName, Index->Name );
	strcpy( TransPart.Data.IndexRename.OldSecName, OldSecName );
	strcpy( TransPart.Data.IndexRename.NewSecName, NewSecName );
	TransPart.Data.IndexRename.Index = Index;
	return SecDbTransactionAdd( Index->pDb, SDB_TRAN_INDEX_RENAME, &TransPart, NULL );
}



/****************************************************************
**	Routine: SecDbIndexAddIfMatch
**	Returns: TRUE	- Security added to index without error, or
**			          skipped if index constraints not met
**			 FALSE	- Couldn't add security to index, or error
**			          checking constraints
**	Action : Check index constraints, if any, and add security to
**			 index if appropriate
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexAddIfMatch"

int SecDbIndexAddIfMatch(
	SDB_INDEX		*Index,		// Index to add security to
	SDB_OBJECT		*SecPtr		// Security to add to index
)
{
	int		Want	= TRUE;


	// If this is a constrained index, we may not want to add this particular object

	if( Index->Flags & SDB_INDEX_CONSTRAINED )
		if( !SecDbIndexCheckConstraints( Index, SecPtr, &Want ))
			return ErrMore( ARGCHECK_FUNCTION_NAME "Error checking %s vs. %s constraints", SecPtr->SecData.Name, Index->Name );

	if( Want )
		return SecDbIndexAddSecurity( Index, SecPtr );
	else
		return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexDeleteIfMatch
**	Returns: TRUE	- Security removed from index without error,
**			          or no action taken because not present
**			 FALSE	- Error removing security from index, or 
**			          error checking for presence in index
**	Action : Remove a security from an index
**			 If index has constraints, make sure security is 
**			 present before deleting
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexDeleteIfMatch"

int SecDbIndexDeleteIfMatch(
	SDB_INDEX		*Index,			// Index to delete security from
	const char		*SecName		// Name of security to remove from index
)
{
	SDB_INDEX_POS
			*IndexPos;

	int		Present		= TRUE;


	// If this is a constrained index, the object may not be present in the index

	if( Index->Flags & SDB_INDEX_CONSTRAINED )
	{
		IndexPos	= SecDbIndexPosCreate( Index );
		Present		= SecDbIndexGetByName( IndexPos, SDB_GET_EQUAL, SecName );
	
		// SecDbIndexGetByName returns FALSE for errors as well as absence
		// Check to see if there was an error

		if( !Present && SDB_ERR_OBJECT_NOT_FOUND != ErrNum )	
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Error checking for %s in %s", SecName, Index->Name );
	}

	if( Present )
		return SecDbIndexDeleteSecurity( Index, SecName );
	else
		return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexUpdateCommon
**	Returns: TRUE	- Security represented properly in index
**			 FALSE	- Error updating index or checking constraints
**	Action : Update a security in an index (Incremental or normal)
**			 If index has constraints, proper action could be
**			 add, delete, update, or no action
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexUpdateCommon"

int SecDbIndexUpdateCommon(
	SDB_INDEX		*Index,			// Index to add security to
	SDB_OBJECT		*SecPtr, 		// Security to add to index
	int				Incremental		// Method to use
)
{
	SDB_INDEX_POS
			*IndexPos;

	int		Present		= TRUE,
			Want		= TRUE;


	if( Index->Flags & SDB_INDEX_CONSTRAINED )
	{
		if( !SecDbIndexCheckConstraints( Index, SecPtr, &Want ))
			return ErrMore( ARGCHECK_FUNCTION_NAME "Error checking %s (updated) vs. %s constraints", SecPtr->SecData.Name, Index->Name );

		IndexPos	= SecDbIndexPosCreate( Index );
		Present		= SecDbIndexGetByName( IndexPos, SDB_GET_EQUAL, SecPtr->SecData.Name );
	
		// SecDbIndexGetByName returns FALSE for errors as well as absence - check for errors

		if( !Present && SDB_ERR_OBJECT_NOT_FOUND != ErrNum )	
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Error checking for %s in %s", SecPtr->SecData.Name, Index->Name );
	}

	if( Present && Want )
	{
		if( Incremental )
			return SecDbIndexIncremental( Index, SecPtr );
		else
			return SecDbIndexUpdateSecurity( Index, SecPtr );
	}
	else if( Present && !Want )
		return SecDbIndexDeleteSecurity( Index, SecPtr->SecData.Name );
	else if( !Present && Want )
		return SecDbIndexAddSecurity( Index, SecPtr );
	else
		return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexUpdateIfMatch
**	Returns: TRUE	- Security represented properly in index
**			 FALSE	- Error updating index or checking constraints
**	Action : Update a security in an index
**			 If index has constraints, proper action could be
**			 add, delete, update, or no action
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexUpdateIfMatch"

int SecDbIndexUpdateIfMatch(
	SDB_INDEX		*Index,			// Index to add security to
	SDB_OBJECT		*SecPtr 		// Security to add to index
)
{
	return SecDbIndexUpdateCommon( Index, SecPtr, FALSE );
}



/****************************************************************
**	Routine: SecDbIndexIncrementalIfMatch
**	Returns: TRUE	- Security represented properly in index
**			 FALSE	- Error updating index or checking constraints
**	Action : Update a security in an index (Incremental)
**			 If index has constraints, proper action could be
**			 add, delete, update, or no action
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexIncrementalIfMatch"

int SecDbIndexIncrementalIfMatch(
	SDB_INDEX		*Index,			// Index to add security to
	SDB_OBJECT		*SecPtr 		// Security to add to index
)
{
	return SecDbIndexUpdateCommon( Index, SecPtr, TRUE );
}



/****************************************************************
**	Routine: SecDbIndexRenameIfMatch
**	Returns: TRUE	- Security renamed in index without error, or
**			          no action taken if security does not match
**			          index constraints
**			 FALSE	- Error renaming security in index or 
**			          checking constraints
**	Action : Check index constraints, if any, and rename security
**			 in index if constraints are met
**			 Assumes intra-transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexRenameIfMatch"

int SecDbIndexRenameIfMatch(
	SDB_INDEX			*Index,			// Index to add security to
	SDB_OBJECT			*NewSecPtr,		// Pointer to security to add to index
	const SDB_SEC_NAME 	*OldSecName  	// Old name of security
)
{
	SDB_INDEX_POS
			*IndexPos;

	int		Present		= TRUE,
			Want		= TRUE;


	// If this is a constrained index, the security may not be needed in the index

	if( Index->Flags & SDB_INDEX_CONSTRAINED )
	{
		if( !SecDbIndexCheckConstraints( Index, NewSecPtr, &Want ))
			return ErrMore( ARGCHECK_FUNCTION_NAME "Error checking %s (updated) vs. %s constraints", NewSecPtr->SecData.Name, Index->Name );

		IndexPos	= SecDbIndexPosCreate( Index );
		Present		= SecDbIndexGetByName( IndexPos, SDB_GET_EQUAL, OldSecName );
	
		// SecDbIndexGetByName returns FALSE for errors as well as absence - check for errors

		if( !Present && SDB_ERR_OBJECT_NOT_FOUND != ErrNum )	
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Error checking for %s in %s", OldSecName, Index->Name );

		if( !Present && Want )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": %s is missing from %s", OldSecName, Index->Name );
		else if( Present && !Want )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": %s is improperly present in %s", OldSecName, Index->Name );
	}

	if( Want )
		return SecDbIndexRenameSecurity( Index, NewSecPtr->SecData.Name, OldSecName );
	else
		return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexGet
**	Returns: Object name - Operation found a security name
**			 NULL		 - Operation couldn't find matching object
**	Summary: Get a security name from an index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexGet"

char *SecDbIndexGet(
	SDB_INDEX_POS	*IndexPos,
	int			 	GetType
)
{
	SDB_INDEX
			*Index = IndexPos->Data.Index;

	int		PartNum,
			Success;

	char	*Str;


	if( !IndexOpen( Index ))
		return (char *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ): Failed to open index", Index->Name );

	// If uncommitted transaction, do a merged get
	if( Index->pDb->TransHead )
		Success = SecDbTransProtectedIndexGet( Index->pDb, IndexPos, GetType );

	// Otherwise just get from driver
	else
		Success = SecDbIndexGetDriver( IndexPos, GetType );

	if( !Success )
		return (char *) ErrMoreN( ARGCHECK_FUNCTION_NAME );

	// FIX - Truncate strings to allow old indices to match
	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
	{
		if( IndexPos->Data.Values[ PartNum ].DataType == DtString )
		{
			Str = (char *) IndexPos->Data.Values[ PartNum ].Data.Pointer;
			if( strlen( Str ) >= (unsigned int) Index->Parts[ PartNum ].ByteWidth )
				Str[ Index->Parts[ PartNum ].ByteWidth - 1 ] = '\0';
		}
	}

	return( IndexPos->Data.SecName );
}



/****************************************************************
**	Routine: SecDbIndexGetByName
**	Returns: Object name - Operation found a security name
**			 NULL		 - Operation couldn't find matching object
**	Summary: Get an index record for a security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME				
#define ARGCHECK_FUNCTION_NAME "SecDbIndexGetByName"

int SecDbIndexGetByName(
	SDB_INDEX_POS	*IndexPos,
	int			 	GetType,
	const char		*SecName
)
{
	SDB_INDEX
			*Index = IndexPos->Data.Index;

	int     Success;


	if( !IndexOpen( Index ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed to open index", Index->Name );

	// FIX: If uncommitted transaction, do a merged get
//	if( Index->pDb->TransHead )
//		Success = SecDbTransProtectedIndexGetByName( Index->pDb, IndexPos, GetType, SecName );

	// Otherwise just get from driver
//	else
		Success = SecDbIndexGetByNameDriver( IndexPos, GetType, SecName );

	if( !Success )
		return ErrMore( ARGCHECK_FUNCTION_NAME );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbIndexPosSetValue
**	Returns: TRUE	- Value set without error
**			 FALSE	- Error setting value
**	Action : Set a value within an index position structure
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexPosSetValue"

int SecDbIndexPosSetValue(
	SDB_INDEX_POS	*IndexPos,
	SDB_VALUE_TYPE	ValueType,
	DT_VALUE	  	*Value
)
{
	SDB_INDEX
			*Index = IndexPos->Data.Index;

	int		PartNum;

	char	*Str;


	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		if( ValueType == Index->Parts[ PartNum ].ValueType )
		{
			if( Value->DataType != ValueType->DataType )
				return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME " @ - Setting %s, expected datatype %s not %s",
						ValueType->Name,
						ValueType->DataType->Name,
						Value->DataType->Name );

			DTM_FREE( &IndexPos->Data.Values[ PartNum ] );

			// Really ugly kluge to make an index pos look like what's in the database
			DTM_ASSIGN( &IndexPos->Data.Values[ PartNum ], Value );
			if( Value->DataType == DtString )
			{
				Str = (char *) IndexPos->Data.Values[ PartNum ].Data.Pointer;
				if( strlen( Str ) >= (unsigned int) Index->Parts[ PartNum ].ByteWidth )
					Str[ Index->Parts[ PartNum ].ByteWidth - 1 ] = '\0';
			}

			IndexPos->Modified = TRUE;
			return TRUE;
		}

	return( Err( ERR_NOT_FOUND, ARGCHECK_FUNCTION_NAME " @ - %s( %s )", ValueType ? ValueType->Name : "<null>", Index->Name ));
}



/****************************************************************
**	Routine: SecDbIndexPosGetValue
**	Returns: Pointer to value if successful
**			 NULL if value couldn't be found
**	Summary: Get a value from an index position structure
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexPosGetValue"

DT_VALUE *SecDbIndexPosGetValue(
	SDB_INDEX_POS	*IndexPos,
	SDB_VALUE_TYPE	ValueType
)
{
	SDB_INDEX
			*Index = IndexPos->Data.Index;

	int		PartNum;


	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		if( ValueType == Index->Parts[ PartNum ].ValueType )
			return( &IndexPos->Data.Values[ PartNum ] );

	Err( ERR_NOT_FOUND, ARGCHECK_FUNCTION_NAME ": Index value @ - %s( %s )", ValueType ? ValueType->Name : "<null>", Index->Name );
	return NULL;
}


/****************************************************************
**	Routine: SecDbIndexPosCreate
**	Returns: Pointer to new index position if successful
**			 NULL if index position couldn't be created
**	Summary: Allocate an SDB_INDEX_POS structure
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexPosCreate"

SDB_INDEX_POS *SecDbIndexPosCreate(
	SDB_INDEX	*Index
)
{
	SDB_INDEX_POS
			*IndexPos;

	int		PartNum;


/*
**	Error check
*/

	if( !Index )
		return (SDB_INDEX_POS *) ErrN( SDB_ERR_INDEX_INVALID, ARGCHECK_FUNCTION_NAME ": @" );

	if( !IndexOpen( Index ))
		return (SDB_INDEX_POS *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ): Failed to open index", Index->Name );


/*
**	Create the structure
*/

	IndexPos = (SDB_INDEX_POS *) calloc( 1, sizeof( SDB_INDEX_POS ));
	if( IndexPos )
	{
		IndexPos->Data.Index = Index;

		for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
			if( !DTM_LOW_LIMIT( &IndexPos->Data.Values[ PartNum ], Index->Parts[ PartNum ].ValueType->DataType ))
				DTM_INIT( &IndexPos->Data.Values[ PartNum ] );

		if( !Index->pDb->IsVirtual() )
		{
			if( !(*Index->pDb->pfIndexPosCreate)( IndexPos ) )
			{
				ErrMore( ARGCHECK_FUNCTION_NAME ": Unable to create an index pos in the db driver." );
				free( IndexPos );
				return NULL;
			}
		}
		else
		{
			if( !( Index->pDb->Left->DbType & SDB_NO_SECURITY_DATABASE ) )
				if( NULL == ( IndexPos->Left = SecDbIndexPosCreate( SecDbIndexFromName( Index->Name, Index->pDb->Left ) ) ) )
					return (SDB_INDEX_POS *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ) in %s: @", Index->Name, Index->pDb->Left->FullDbName.c_str() );

			if(    Index->pDb->Right 
				&& !( Index->pDb->Right->DbType & SDB_NO_SECURITY_DATABASE ) )
				if( NULL == ( IndexPos->Right = SecDbIndexPosCreate( SecDbIndexFromName( Index->Name, Index->pDb->Right ) ) ) )
					return (SDB_INDEX_POS *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ) in %s: @", Index->Name, Index->pDb->Right->FullDbName.c_str() );

			// Compresss away a NULL left, for the case where the left had no Security DB.

			if( NULL == IndexPos->Left )
			{
				IndexPos->Left = IndexPos->Right;
				IndexPos->Right = NULL;
			}
		}

		IndexPos->Modified = TRUE;
	}
	else
		Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME "( %s ): @", Index->Name );

	return( IndexPos );
}



/****************************************************************
**	Routine: SecDbIndexPosCopy
**	Returns: Pointer to new copy of index position
**			 NULL if copy couldn't be created
**	Summary: Copy an SDB_INDEX_POS structure
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexPosCopy"

SDB_INDEX_POS *SecDbIndexPosCopy(
	SDB_INDEX_POS *IndexPosSource
)
{
	SDB_INDEX_POS
			*IndexPosTarget;

	SDB_INDEX
			*Index;

	int		PartNum;


/*
**	Error check
*/

	if( !IndexPosSource )
		return (SDB_INDEX_POS *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @" );

	Index = IndexPosSource->Data.Index;

	if( !IndexOpen( Index ))
		return (SDB_INDEX_POS *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ): Failed to open index", Index->Name );


/*
**	Create the structure
*/

	if( !(IndexPosTarget = (SDB_INDEX_POS *) calloc( 1, sizeof( SDB_INDEX_POS ))))
		return (SDB_INDEX_POS *) ErrN( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );

	IndexPosTarget->Data.Index = Index;

	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		DTM_INIT( &IndexPosTarget->Data.Values[ PartNum ] );

	if( !Index->pDb->IsVirtual() )
		(*Index->pDb->pfIndexPosCreate)( IndexPosTarget );
	else
	{
		if( IndexPosSource->Left )
			IndexPosTarget->Left = SecDbIndexPosCopy( IndexPosSource->Left );
		if( IndexPosSource->Right )
			IndexPosTarget->Right = SecDbIndexPosCopy( IndexPosSource->Right );
	}

	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		DTM_ASSIGN( &IndexPosTarget->Data.Values[ PartNum ], &IndexPosSource->Data.Values[ PartNum ] );
	strcpy( IndexPosTarget->Data.SecName, IndexPosSource->Data.SecName );
	IndexPosTarget->Modified = TRUE;

	return( IndexPosTarget );
}



/****************************************************************
**	Routine: SecDbIndexPosDestroy
**	Returns: Nothing
**	Action : Destroy an SDB_INDEX_POS structure
****************************************************************/

void SecDbIndexPosDestroy(
	SDB_INDEX_POS	*IndexPos		// Index position to destroy
)
{
	int		PartNum;


	if( IndexPos )
	{
		if( IndexPos->Data.Index->pDb->pfIndexPosDestroy )
			(*IndexPos->Data.Index->pDb->pfIndexPosDestroy)( IndexPos );
		for( PartNum = 0; PartNum < IndexPos->Data.Index->PartCount; PartNum++ )
			DTM_FREE( &IndexPos->Data.Values[ PartNum ] );

		if( IndexPos->Left )
			SecDbIndexPosDestroy( IndexPos->Left );
		if( IndexPos->Right )
			SecDbIndexPosDestroy( IndexPos->Right );

		free( IndexPos );
	}
}



/****************************************************************
**	Routine: SecDbIndexPosCompare
**	Returns: < 0 IndexPos1  < IndexPos2
**			   0 IndexPos1 == IndexPos2
**	         > 0 IndexPos1  > IndexPos2
**	Action : Compare one index position with another
****************************************************************/

int SecDbIndexPosCompare(
	SDB_INDEX_POS	*IndexPos1,		// Left side of compare
	SDB_INDEX_POS	*IndexPos2		// Right side of compare
)
{
	SDB_INDEX
			*Index = IndexPos1->Data.Index;

	DT_VALUE
			TmpValue;

	int		PartNum;


	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
	{
		if( DT_OP( DT_MSG_LESS_THAN, &TmpValue, &IndexPos1->Data.Values[ PartNum ], &IndexPos2->Data.Values[ PartNum ] ))
		{
			if( TmpValue.Data.Number )
				return( (Index->Parts[ PartNum ].Flags & SDB_INDEX_DESCENDING ) ? 1 : -1 );
		}
		else
			continue;

		if( DT_OP( DT_MSG_GREATER_THAN, &TmpValue, &IndexPos1->Data.Values[ PartNum ], &IndexPos2->Data.Values[ PartNum ] ))
		{
			if( TmpValue.Data.Number )
				return( (Index->Parts[ PartNum ].Flags & SDB_INDEX_DESCENDING ) ? -1 : 1 );
		}
		else
			continue;
	}

	return 0;
}



/****************************************************************
**	Routine: SecDbIndexFromName
**	Returns: A pointer to the index if the name was found
**			 NULL if index couldn't be found
**	Summary: Retrieve an index from the index name
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexFromName"

SDB_INDEX *SecDbIndexFromName(
	const char	*Name,		// Name of index to find
	SDB_DB		*pDb		// Database of index
)
{
	SDB_INDEX
			*Index,
			LookupIndex;


	// Lookup the database ID
	if( !( pDb = DbToPhysicalDb( pDb )))
		return (SDB_INDEX *) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s, <null> ) @", Name );

	strcpy( LookupIndex.Name, Name );
	LookupIndex.pDb				= pDb;
	LookupIndex.IndexID			= 0;
	LookupIndex.DescriptorOnly	= FALSE;

	Index = (SDB_INDEX *) HashLookup( SecDbIndexHash, &LookupIndex );
	if( Index )
	{
		if( !IndexOpen( Index ))
			return (SDB_INDEX *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ): Failed to open index", Index->Name );
		return( Index );
	}

	return (SDB_INDEX *) ErrN( SDB_ERR_INDEX_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s, %s ) @", Name, pDb->FullDbName.c_str() );
}



/****************************************************************
**	Routine: SecDbIndexClose
**	Returns: Nothing
**	Summary: Close an index
****************************************************************/

void SecDbIndexClose(
	SDB_INDEX	*Index,		// If NULL then close all of the indices
	SDB_DB		*pDb,		// Used if above is NULL to determine which indices
	int			RemoveFlag	// TRUE to remove index from hash
)
{
	SDB_INDEX_POINTER
			**IndexPtr,
			*DeleteIndexPtr;

	SDB_CLASS_INFO
			*ClassInfo;

	HASH_ENTRY_PTR
			Ptr;


/*
**	Close one index
*/

	if( Index )
	{
		if( Index->OpenFlag )
		{
			if( Index->pDb->pfIndexClose )
				(*Index->pDb->pfIndexClose)( Index );
			Index->OpenFlag = FALSE;
		}
		if( RemoveFlag )
		{
			HashDelete( SecDbIndexHash, Index );
			for( ; Index->ClassCount > 0; Index->ClassCount-- )
			{
				ClassInfo = (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) Index->Classes[ Index->ClassCount - 1 ] );
				if( ClassInfo )
					for( IndexPtr = &ClassInfo->IndexList; *IndexPtr; IndexPtr = &(*IndexPtr)->Next )
						if( (*IndexPtr)->Index == Index )
						{
							DeleteIndexPtr = *IndexPtr;
							*IndexPtr = (*IndexPtr)->Next;
							free( DeleteIndexPtr );
							break;
						}
			}
			if( Index->Classes )
				free( Index->Classes );
			free( Index );
		}
	}


/*
**	Close all indices
*/

	else
	{
		pDb = DbToPhysicalDb( pDb );
		HASH_FOR( Ptr, SecDbIndexHash )
		{
			Index = (SDB_INDEX *) HashValue( Ptr );
			if( Index->pDb == pDb )
			{
				if( Index->OpenFlag )
				{
					if( Index->pDb->pfIndexClose )
						(*Index->pDb->pfIndexClose)( Index );
					Index->OpenFlag = FALSE;
				}
				if( RemoveFlag )
				{
					HashDelete( SecDbIndexHash, Index );
					for( ; Index->ClassCount > 0; Index->ClassCount-- )
					{
						ClassInfo = (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) Index->Classes[ Index->ClassCount - 1 ] );
						if( ClassInfo )
							for( IndexPtr = &ClassInfo->IndexList; *IndexPtr; IndexPtr = &(*IndexPtr)->Next )
								if( (*IndexPtr)->Index == Index )
								{
									DeleteIndexPtr = *IndexPtr;
									*IndexPtr = (*IndexPtr)->Next;
									free( DeleteIndexPtr );
									break;
								}
					}
					if( Index->Classes )
						free( Index->Classes );
					free( Index );
				}
			}
		}
	}
}



/****************************************************************
**	Routine: SecDbIndexEnumFirst
**	Returns: Pointer to first index in relevant db
**			 NULL - no indices loaded (if ERR_NONE == ErrNum),
**			 else an error occurred
**	Summary: Enumerate the first index
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexEnumFirst"

SDB_INDEX *SecDbIndexEnumFirst(
	SDB_ENUM_PTR	*EnumPtr,		// Enumeration pointer
	SDB_DB			*pDb			// Enumerate indices for this db (or NULL for all dbs)
)
{
	SDB_INDEX
			*Index;


	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));

	(*EnumPtr)->Count = SecDbIndexHash->KeyCount;
	(*EnumPtr)->Ptr = HashFirst( SecDbIndexHash );

	if( HashEnd( (*EnumPtr)->Ptr ))		// No more indexes
		return (SDB_INDEX *) ErrN( ERR_NONE, ARGCHECK_FUNCTION_NAME );

	for( Index = (SDB_INDEX *) HashValue( (*EnumPtr)->Ptr );
		 Index;
		 Index = SecDbIndexEnumNext( *EnumPtr, pDb ) )
	{
		if( !pDb || pDb == Index->pDb )	// Correct db, so just check and return the index
        {
			if( !IndexOpen( Index ))
				return (SDB_INDEX *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s )", Index->Name );
			else
				return Index;
        }
	}
/*
**	Dropped off the end of the loop, with either an error, or no more indexes.
**	Return NULL in either case
*/
	return (SDB_INDEX *) ErrMoreN( ARGCHECK_FUNCTION_NAME );
}



/****************************************************************
**	Routine: SecDbIndexEnumNext
**	Returns: Pointer to next index
**			 NULL - no more indices (if ERR_NONE == ErrNum),
**			 else an error occurred
**	Summary: Return the next index for the relevant db
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIndexEnumNext"

SDB_INDEX *SecDbIndexEnumNext(
	SDB_ENUM_PTR	EnumPtr,		// Enumeration pointer
	SDB_DB			*pDb			// Enumerate indices for this db (or NULL for all dbs)
)
{
	SDB_INDEX
			*Index;


	while( TRUE )
	{
		EnumPtr->Ptr = HashNext( SecDbIndexHash, EnumPtr->Ptr );
		if( HashEnd( EnumPtr->Ptr ))	// No more indexes
			return (SDB_INDEX *) ErrN( ERR_NONE, ARGCHECK_FUNCTION_NAME );
		Index = (SDB_INDEX *) HashValue( EnumPtr->Ptr );

		if( pDb && pDb != Index->pDb )	// Wrong db, so carry on
			continue;

		if( !IndexOpen( Index ))
			return (SDB_INDEX *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s )", Index->Name );

		return Index;
	}
}



/****************************************************************
**	Routine: SecDbIndexEnumClose
**	Returns: Nothing
**	Summary: End enumeration of indexes
****************************************************************/

void SecDbIndexEnumClose(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
		free( EnumPtr );
}



/****************************************************************
**	Routine: SecDbIndexHashFunction
**	Returns: Number between 0 and size - 1
**	Action : Hash value for given
****************************************************************/

unsigned long SecDbIndexHashFunction(
	HASH_KEY	ParmIndex
)
{
	const SDB_INDEX
			*Index = (const SDB_INDEX *) ParmIndex;

	unsigned long
			Result;


	// Note: IndexID is intentionally left out of this hash comparison
	//		 because it is not known until the index is opened.  It is
	//		 used in SecDbIndexCompareFunction() below primarily to
	//		 distinguish among different DescriptorOnly indicies.

	Result  = HashStrHash( Index->Name );
	Result += HashIntHash( (HASH_KEY) (long) Index->pDb );
	return Result;
}



/****************************************************************
**	Routine: SecDbIndexCompareFunction
**	Returns: 0 if the two indices are the same
**			 1 if the two indices are not the same
**	Action : Compares two index structures
****************************************************************/

int SecDbIndexCompareFunction(
	HASH_KEY	ParmIndex1,
	HASH_KEY	ParmIndex2
)
{
	const SDB_INDEX
			*Index1 = (const SDB_INDEX *) ParmIndex1,
			*Index2 = (const SDB_INDEX *) ParmIndex2;


	if(		Index1->pDb != Index2->pDb
		||	Index1->DescriptorOnly != Index2->DescriptorOnly
		||	0 != stricmp( Index1->Name, Index2->Name ))
		return 1;	// No match

	if( !Index1->DescriptorOnly )
		return 0;	// Matches

	// If DescriptorOnly be sure to check the IndexID
	return Index1->IndexID != Index2->IndexID;
}

/****************************************************************
**	Routine: SecDbIndexGetNoLoadFlag
**	Action : get no load flag for index create
****************************************************************/
static int IndexNoLoadFlag;

int SecDbIndexGetNoLoadFlag(
)
{
	return IndexNoLoadFlag;
}

/****************************************************************
**	Routine: SecDbIndexSetNoLoadFlag
**	Returns: 
**	Action : Set no load flag for index create
****************************************************************/

void SecDbIndexSetNoLoadFlag( 
		int iFlag
)
{
	IndexNoLoadFlag = iFlag;
}

