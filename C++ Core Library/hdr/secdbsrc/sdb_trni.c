#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_trni.c,v 1.17 2001/09/04 14:26:30 goodfj Exp $"
/****************************************************************
**
**	SDB_TRNI.C
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_trni.c,v $
**	Revision 1.17  2001/09/04 14:26:30  goodfj
**	Support for IndexGetByName
**
**	Revision 1.16  2000/12/01 19:06:36  singhki
**	Must use PosCompare to include the secname in the pos compare else merge will be wrong
**	
**	Revision 1.15  2000/05/18 17:50:26  singhki
**	Create index pos correctly for a union db during a TransProtectedIndexGet
**	
**	Revision 1.14  1999/09/01 15:29:33  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.13  1999/08/07 01:40:12  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.12  1999/04/07 23:47:36  vengrd
**	Fixed obscure bug in IndexGet with SDB_GET_EQUAL when the only match had been modified within the same transaction.
**	
**	Revision 1.11  1998/10/19 22:26:47  vengrd
**	Eliminated unused local variable.
**	
**	Revision 1.10  1998/10/14 18:20:54  singhki
**	New search paths
**	
**	Revision 1.9.2.1  1998/08/28 23:34:05  singhki
**	search paths, index merge working, some problems with ~casts
**	
**	Revision 1.9  1998/01/02 21:33:11  rubenb
**	added GSCVSID
**	
**	Revision 1.8  1997/02/24 23:30:46  lundek
**	SDB_IGNORE_PATH
**	
**	Revision 1.7  1996/11/05 23:26:53  gribbg
**	BugFix: Add missing break statements for TransIndexModified
**
**	Revision 1.6  1996/09/11 02:53:59  gribbg
**	New Trees Merge
**
**	Revision 1.5.2.2  1996/05/10 15:35:23  gribbg
**	Fix unix warnings
**
**	Revision 1.5.2.1  1996/05/10 11:54:39  gribbg
**	NewTrees: First snapshot
**
**	Revision 1.5  1996/01/16 17:53:06  ERICH
**	NT Compatibility
**
**	   Rev 1.4   15 Jan 1996 15:26:04   GRIBBG
**	NewServer: Transaction/Index Structure/API changes
**
**	   Rev 1.3   06 Jun 1995 15:06:08   GRIBBG
**	Save Memory: Reduce SDB_TRANS_PART size by allocating some parts
**
**	   Rev 1.2   05 Jun 1995 14:05:36   LUNDEK
**	Some structure tags
**	BugFix: Call db driver for IndexPosFromTrans... (Btrieve driver cares)
**
**	   Rev 1.1   19 Dec 1994 13:13:46   RUBENB
**	address unix warnings
**
**	   Rev 1.0   23 Jun 1994 17:04:30   LUNDEK
**	Initial revision.
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdlib.h>
#include	<string.h>
#include	<skiplist.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<secdrv.h>
#include	<secdrive.h>
#include	<sectrans.h>
#include	<sdb_int.h>


/****************************************************************
**	Routine: PosCopy
**	Returns: TRUE or FALSE
**	Action : Copy values from one IndexPos to another
****************************************************************/

static int PosCopy(
	SDB_INDEX_POS	*IndexPosTarget,
	SDB_INDEX_POS	*IndexPosSource
)
{
	SDB_INDEX
			*Index = IndexPosSource->Data.Index;

	int		PartNum;


	for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
	{
		DTM_FREE( &IndexPosTarget->Data.Values[ PartNum ] );
		if( !DTM_ASSIGN( &IndexPosTarget->Data.Values[ PartNum ], &IndexPosSource->Data.Values[ PartNum ] ))
			return( FALSE );
	}
	strcpy( IndexPosTarget->Data.SecName, IndexPosSource->Data.SecName );
	return( TRUE );
}



/****************************************************************
**	Routine: PosCompareValues
**	Returns: 0, <0, or >0 like strcmp
**	Action : Compares only the values of the two IndexPos
****************************************************************/

static int PosCompareValues(
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
	return( 0 );
}



/****************************************************************
**	Routine: PosCompare
**	Returns: 0, <0, or >0 like strcmp
**	Action : Compares the values in the two IndexPos,
**			 if equal, compares SecName
****************************************************************/

static int PosCompare(
	SDB_INDEX_POS	*IndexPos1,		// Left side of compare
	SDB_INDEX_POS	*IndexPos2		// Right side of compare
)
{
	int		RetVal;


	if( !( RetVal = PosCompareValues( IndexPos1, IndexPos2 )))
		return( stricmp( IndexPos1->Data.SecName, IndexPos2->Data.SecName ));
	else
		return( RetVal );
}



/****************************************************************
**	Routine: IndexPosFromTransPart
**	Returns: Allocated IndexPos with values from TransPart
**	Action : Allocates an IndexPos and just copies the values from
**			 TransPart
****************************************************************/

static SDB_INDEX_POS *IndexPosFromTransPart(
	SDB_DB			*pDb,
	SDB_TRANS_PART	*TransPart
)
{
	SDB_INDEX
			*Index;

	int		PartNum,
			Status = TRUE;

	SDB_INDEX_POS
			*IndexPos;
			
	SDB_OBJECT
			*SecPtr;


	if( TransPart->Type == SDB_TRAN_INDEX_INCREMENTAL )
	{
		Index = TransPart->Data.IndexIncremental.Index;
		if( !( IndexPos = SecDbIndexPosCreate( Index )))
			return( NULL );

		// Get the values directly from the object
		SecPtr = SecDbGetByName( TransPart->Data.IndexIncremental.SecName, pDb, SDB_IGNORE_PATH );
		Status = !!SecPtr;
		for( PartNum = 0; Status && PartNum < Index->PartCount; PartNum++ )
		{
			DT_VALUE	*Value;

			DTM_FREE( &IndexPos->Data.Values[ PartNum ]);
			Status =	( Value = SecDbGetValue( SecPtr, Index->Parts[ PartNum ].ValueType ))
					&& 	DTM_ASSIGN( &IndexPos->Data.Values[ PartNum ], Value );
		}
		SecDbFree( SecPtr );
		if( !Status )
			return( NULL );
		strcpy( IndexPos->Data.SecName, TransPart->Data.IndexIncremental.SecName );
	}
	else
	{
		Index = TransPart->Data.IndexOp.Data->Index;
		if( !( IndexPos = SecDbIndexPosCreate( Index )))
			return( NULL );

		for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		{
			DTM_FREE( &IndexPos->Data.Values[ PartNum ]);
			if( !DTM_ASSIGN( &IndexPos->Data.Values[ PartNum ], &TransPart->Data.IndexOp.Data->Values[ PartNum ] ))
				return( NULL );
		}
		strcpy( IndexPos->Data.SecName, TransPart->Data.IndexOp.Data->SecName );
	}

	IndexPos->Modified = TRUE;
	return( IndexPos );
}



/****************************************************************
**	Routine: TransIndexGet
**	Returns: New IndexPos or NULL
**	Action : IndexGet from current transaction
****************************************************************/

static SDB_INDEX_POS *TransIndexGet(
	SDB_DB			*pDb,
	SDB_INDEX_POS	*IndexPos,
	int			 	GetType
)
{
	SDB_INDEX_POS
			*TempIndexPos;

	int		BreakLoop	= FALSE;

	SKIP_LIST
			*SkipListByName	= NULL,
			*SkipListByData = NULL;

	SKIP_LIST_NODE
			*Enum;

	SDB_TRANS_PART
			*TransPart;

	char	Sv;


/*
**	Get list of changes to applicable index; multiple changes to the same
**	SecName only take the last change.
*/

	for( TransPart = pDb->TransTail; TransPart; TransPart = TransPart->Prev )
	{
		switch( TransPart->Type )
		{
			case SDB_TRAN_INDEX_INSERT:
			case SDB_TRAN_INDEX_UPDATE:
				if( IndexPos->Data.Index == TransPart->Data.IndexOp.Data->Index )
				{
					if( !SkipListByName )
						SkipListByName = SkipListCreate( SKIP_LIST_STRICMP, SKIP_LIST_DUPE_REJECT );
					SkipListInsert( SkipListByName, TransPart->Data.IndexOp.Data->SecName, TransPart );
				}
				break;

			case SDB_TRAN_INDEX_INCREMENTAL:
				if( IndexPos->Data.Index == TransPart->Data.IndexIncremental.Index )
				{
					if( !SkipListByName )
						SkipListByName = SkipListCreate( SKIP_LIST_STRICMP, SKIP_LIST_DUPE_REJECT );
					SkipListInsert( SkipListByName, TransPart->Data.IndexIncremental.SecName, TransPart );
				}
				break;

			case SDB_TRAN_INDEX_DELETE:
				if( IndexPos->Data.Index == TransPart->Data.IndexDelete.Index )
				{
					if( !SkipListByName )
						SkipListByName = SkipListCreate( SKIP_LIST_STRICMP, SKIP_LIST_DUPE_REJECT );
					SkipListInsert( SkipListByName, TransPart->Data.IndexDelete.SecName, TransPart );
				}
				break;

			case SDB_TRAN_INDEX_DESTROY:
				BreakLoop = 0 == stricmp( IndexPos->Data.Index->Name, TransPart->Data.IndexDestroy.IndexName );
				break;

			default:
				break;	// Otherwise skip it
		}
		if( BreakLoop )
			break;
	}
	if( !SkipListByName )
		return( NULL );


/*
**	Resort the list by PosCompare instead of SecName
*/

	SKIP_LIST_FOR( Enum, SkipListByName )
	{
		TransPart = (SDB_TRANS_PART *) SkipListValue( Enum );
		if( SDB_TRAN_INDEX_DELETE == TransPart->Type )
			continue;
		if( !SkipListByData )
			SkipListByData = SkipListCreate( (SKIP_LIST_CMP) PosCompare, SKIP_LIST_DUPE_REJECT );
		TempIndexPos = IndexPosFromTransPart( pDb, TransPart );
		SkipListInsert( SkipListByData, TempIndexPos, TempIndexPos );
	}
	SkipListDestroy( SkipListByName );
	if( !SkipListByData )
		return( NULL );


/*
**	Look up desired entry in data skiplist
*/

	switch( GetType )
	{
		case SDB_GET_EQUAL:
			Sv = IndexPos->Data.SecName[ 0 ];
			IndexPos->Data.SecName[ 0 ] = '\0';
			TempIndexPos = (SDB_INDEX_POS *) SkipListGetGE( SkipListByData, IndexPos, NULL );
			IndexPos->Data.SecName[ 0 ] = Sv;
			if( TempIndexPos && 0 != PosCompareValues( IndexPos, TempIndexPos ))
				TempIndexPos = NULL;
			break;

		case SDB_GET_FIRST:
			TempIndexPos = (SDB_INDEX_POS *) SkipListGetFirst( SkipListByData, NULL );
			break;
		case SDB_GET_LAST:
			TempIndexPos = (SDB_INDEX_POS *) SkipListGetLast( SkipListByData, NULL );
			break;
		case SDB_GET_LE:
			TempIndexPos = (SDB_INDEX_POS *) SkipListGetLE( SkipListByData, IndexPos, NULL );
			break;
		case SDB_GET_GE:
			TempIndexPos = (SDB_INDEX_POS *) SkipListGetGE( SkipListByData, IndexPos, NULL );
			break;

		case SDB_GET_PREV:
		case SDB_GET_LESS:
			TempIndexPos = (SDB_INDEX_POS *) SkipListGetLess( SkipListByData, IndexPos, NULL );
			break;

		case SDB_GET_NEXT:
		case SDB_GET_GREATER:
			TempIndexPos = (SDB_INDEX_POS *) SkipListGetGreater( SkipListByData, IndexPos, NULL );
			break;

		default:
			TempIndexPos = NULL;
			break;
	}

/*
**	Free the skip list
*/

	SKIP_LIST_FOR( Enum, SkipListByData )
	{
		if( SkipListValue( Enum ) != TempIndexPos )
			SecDbIndexPosDestroy( (SDB_INDEX_POS *) SkipListValue( Enum ));
	};
	SkipListDestroy( SkipListByData );
	return( TempIndexPos );
}



/****************************************************************
**	Routine: TransIndexModified
**	Returns: TRUE if modified or deleted in current transaction or FALSE ow
**	Action : Look through current transaction
****************************************************************/

static int TransIndexModified(
	SDB_DB			*pDb,
	SDB_INDEX_POS	*IndexPos
)
{
	SDB_TRANS_PART
			*TransPart;


	for( TransPart = pDb->TransTail; TransPart; TransPart = TransPart->Prev )
	{
		switch( TransPart->Type )
		{
			case SDB_TRAN_INDEX_INSERT:
			case SDB_TRAN_INDEX_UPDATE:
				if( IndexPos->Data.Index == TransPart->Data.IndexOp.Data->Index
						&& 0 == stricmp( IndexPos->Data.SecName, TransPart->Data.IndexOp.Data->SecName ))
					return TRUE;
				break;

			case SDB_TRAN_INDEX_INCREMENTAL:
				if( IndexPos->Data.Index == TransPart->Data.IndexIncremental.Index
						&& 0 == stricmp( IndexPos->Data.SecName, TransPart->Data.IndexIncremental.SecName ))
					return TRUE;
				break;

			case SDB_TRAN_INDEX_DELETE:
				if( IndexPos->Data.Index == TransPart->Data.IndexDelete.Index
						&& 0 == stricmp( IndexPos->Data.SecName, TransPart->Data.IndexDelete.SecName ))
					return TRUE;
				break;

			case SDB_TRAN_INDEX_DESTROY:
				if( 0 == stricmp( IndexPos->Data.Index->Name, TransPart->Data.IndexDestroy.IndexName ))
					return FALSE;
				break;

			default:
				break;
		}
	}
	return FALSE;
}



/****************************************************************
**	Routine: SecDbIndexGetDriver
**	Returns: TRUE/FALSE
**	Action : do a merged index get from underlying databases
****************************************************************/

int SecDbIndexGetDriver(
	SDB_INDEX_POS	*IndexPos,
	int				GetType
)
{
	if( !IndexPos->Right )				// no right index pos
	{
		if( !IndexPos->Left )			// non virtual, get straight from driver
			return (*IndexPos->Data.Index->pDb->pfIndexGet)( IndexPos, GetType );

		// get from left and copy up
		PosCopy( IndexPos->Left, IndexPos );
		if( !SecDbIndexGetDriver( IndexPos->Left, GetType ))
			return FALSE;

		PosCopy( IndexPos, IndexPos->Left );
		return IndexPos->Modified = TRUE;
	}

	// try to get from left
	PosCopy( IndexPos->Left, IndexPos );
	if( !SecDbIndexGetDriver( IndexPos->Left, GetType )) // if failed, get from right and return
	{
		PosCopy( IndexPos->Right, IndexPos );
		if( !SecDbIndexGetDriver( IndexPos->Right, GetType ))
			return FALSE;

		PosCopy( IndexPos, IndexPos->Right );
		return IndexPos->Modified = TRUE;
	}

	// try to get from right
	PosCopy( IndexPos->Right, IndexPos );
	if( !SecDbIndexGetDriver( IndexPos->Right, GetType )) // if failed, return left result
	{
		PosCopy( IndexPos, IndexPos->Left );
		return IndexPos->Modified = TRUE;
	}

	// need to merge results
	PosCopy( IndexPos, ( (PosCompare( IndexPos->Left, IndexPos->Right ) >= 0) != SecDbMergeMax( GetType ) )
			 ? IndexPos->Right : IndexPos->Left );
	return IndexPos->Modified = TRUE;
}


/****************************************************************
**	Routine: SecDbIndexGetByNameDriver
**	Returns: TRUE/FALSE
**	Action : do a merged index get from underlying databases
****************************************************************/

int SecDbIndexGetByNameDriver(
	SDB_INDEX_POS	*IndexPos,
	int			 	GetType,
	const char		*SecName
)
{
	if( !IndexPos->Right )				// no right index pos
	{
		if( !IndexPos->Left )			// non virtual, get straight from driver
			return (*IndexPos->Data.Index->pDb->pfIndexGetByName)( IndexPos, GetType, SecName );

		// get from left and copy up
		if( !SecDbIndexGetByNameDriver( IndexPos->Left, GetType, SecName ))
			return FALSE;

		PosCopy( IndexPos, IndexPos->Left );
		return IndexPos->Modified = TRUE;
	}

	// try to get from left
	if( !SecDbIndexGetByNameDriver( IndexPos->Left, GetType, SecName )) // if failed, get from right and return
	{
		if( !SecDbIndexGetByNameDriver( IndexPos->Right, GetType, SecName ))
			return FALSE;

		PosCopy( IndexPos, IndexPos->Right );
		return IndexPos->Modified = TRUE;
	}

	// try to get from right
	if( !SecDbIndexGetByNameDriver( IndexPos->Right, GetType, SecName )) // if failed, return left result
	{
		PosCopy( IndexPos, IndexPos->Left );
		return IndexPos->Modified = TRUE;
	}

	// need to merge results
	PosCopy( IndexPos, ( (PosCompare( IndexPos->Left, IndexPos->Right ) >= 0) != SecDbMergeMax( GetType ) )
			 ? IndexPos->Right : IndexPos->Left );
	return IndexPos->Modified = TRUE;
}




/****************************************************************
**	Routine: SecDbTransProtectedIndexGet
**	Returns: TRUE or FALSE
**	Action : Interleave the current transaction index operations
**			 with the db driver pfIndexGet to get the name of
**			 a security based upon the index as though the current
**			 transaction had been committed
****************************************************************/

int SecDbTransProtectedIndexGet(
	SDB_DB			*pDb,
	SDB_INDEX_POS	*IndexPos,
	int			 	GetType
)
{
	SDB_INDEX_POS
			*RetTrans,
			*CheckEquality;

	int		GetTypeDriver = GetType,
			RetDriver,
			FoundInTrans = FALSE;


/*
**	Get index pos from uncommitted transaction
*/

	RetTrans = TransIndexGet( pDb, IndexPos, GetType );


/*
**	Get index pos from driver.  If the returned entry has been deleted
**	or modified by current transaction, disregard it and go get
**	the next logical entry from driver.
*/

	CheckEquality = NULL;
	while( TRUE )
	{
		if( !( RetDriver = SecDbIndexGetDriver( IndexPos, GetTypeDriver )))
			break;

		if( !TransIndexModified( pDb, IndexPos ))
		{
			if( CheckEquality )
				RetDriver = 0 == PosCompareValues( IndexPos, CheckEquality );

			break;
		}

		if( SDB_GET_GE == GetTypeDriver || SDB_GET_FIRST == GetTypeDriver )
			GetTypeDriver = SDB_GET_GREATER;

		else if( SDB_GET_LE == GetTypeDriver || SDB_GET_LAST == GetTypeDriver )
			GetTypeDriver = SDB_GET_LESS;

		else if( SDB_GET_EQUAL == GetTypeDriver )
		{
			GetTypeDriver = SDB_GET_NEXT;	// Get next with same data but possibly different SecName
			CheckEquality = SecDbIndexPosCopy( IndexPos );
		}
	}
	if( CheckEquality )
		SecDbIndexPosDestroy( CheckEquality );


/*
**	Choose which index pos to use
*/

	if( !RetTrans )
		return( RetDriver );

	if( !RetDriver )
		FoundInTrans = TRUE;
	else
		switch( GetType )
		{
			case SDB_GET_GE:	  		// Take least
			case SDB_GET_FIRST:
			case SDB_GET_NEXT:
			case SDB_GET_GREATER:
			case SDB_GET_EQUAL:			// SecName must be different -- take least
			default:
				FoundInTrans = PosCompare( RetTrans, IndexPos ) < 0;
				break;

			case SDB_GET_LE:	 		// Take greatest
			case SDB_GET_LAST:
			case SDB_GET_PREV:
			case SDB_GET_LESS:
				FoundInTrans = PosCompare( RetTrans, IndexPos ) > 0;
				break;
		}


/*
**	Move values into real index pos
*/

	if( FoundInTrans )
	{
		PosCopy( IndexPos, RetTrans );
		IndexPos->Modified = TRUE;
	}

	SecDbIndexPosDestroy( RetTrans );
	return( TRUE );
}
