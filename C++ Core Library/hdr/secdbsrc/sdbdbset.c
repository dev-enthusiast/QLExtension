#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbdbset.c,v 1.26 2001/06/29 15:22:16 singhki Exp $"
/****************************************************************
**
**	sdbdbset.c	- DbSet implementation
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbdbset.c,v $
**	Revision 1.26  2001/06/29 15:22:16  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**
**	Revision 1.25  2001/02/28 01:49:06  singhki
**	In case we consider a diddled node as the best node ensure that we
**	have also considered the node in the PhysicalDb else we will get
**	incorrect sharing.
**	
**	Old way of creating the node during the set diddle did not work in all
**	cases because that node could get co-opted for another Db/destroyed by
**	LRU cache.
**	
**	Revision 1.24  2001/01/09 21:44:36  singhki
**	define toString() operator
**	
**	Revision 1.23  2000/07/31 19:18:19  singhki
**	Return BaseDb if no Db in the DbSet is open
**	
**	Revision 1.22  2000/01/14 20:34:34  singhki
**	Only return Db from DbSet if it is Open
**	
**	Revision 1.21  1999/12/07 22:05:33  singhki
**	Add VTI to every node. Points to Class Value Func() by default.
**	New special class static VT, Class Value Func lives on Class object
**	in !Class Objects Db.
**	For the purpose of a DBSET_BEST computation, every Db behaves as if
**	it is of the for Db;Class Objects in order to find these Class VTIs
**	Use Class Value Func to calculate VFs for ~Cast and ~Subscript
**	
**	Revision 1.20  1999/11/01 22:41:52  singhki
**	death to all link warnings
**	
**	Revision 1.19  1999/09/28 21:18:28  singhki
**	Various SDB_DBSET_BEST fixes: Use bool for booleans, define Member if Db is a member of DbSet
**	
**	Revision 1.18  1999/09/24 02:18:32  singhki
**	Make LookupHelper initialized pointer because OS/2 p.o.s. won't initialize vtable of static instances
**	
**	Revision 1.17  1999/09/16 15:44:20  singhki
**	uh, oh, Hash & Compare funcs need to compare the DbPrimary too
**	
**	Revision 1.16  1999/09/02 16:34:44  singhki
**	Use static construced SDB_DBSET_BEST in order to do DbSetBestLookup
**	
**	Revision 1.15  1999/09/02 12:54:45  singhki
**	better hash funcs
**	
**	Revision 1.14  1999/09/01 15:29:34  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.13  1999/07/20 01:41:26  singhki
**	Use Db with min depth (BaseDb) in best lookup for diddles and sets instead of primary Db.
**	Fix split computation in best to also use BaseDb. Removed incorrect NodeCopy
**	from NodeFind when creating a new node.
**	
**	Revision 1.12  1999/06/21 03:07:00  gribbg
**	More const correctness
**	
**	Revision 1.11  1999/06/18 20:08:44  hsuf
**	Provide accessor functions for DbSet to hide data members of DbSet
**	
**	Revision 1.10  1999/06/08 18:41:54  singhki
**	Keep track if any Db in DbSet is shareable in AnyShareable
**	
**	Revision 1.9  1999/06/08 15:16:07  singhki
**	fix split computation. Splits for sim, 1;sim; 2;1;sim, 3;2;1;sim for 1;sim are:
**	sim, 1;sim, 2;1;sim.
**	
**	Revision 1.8  1999/06/04 18:54:50  singhki
**	renamed DbSetCacheInfo to SecDbDbSetCacheInfo
**	
**	Revision 1.7  1999/04/19 15:16:41  singhki
**	Fix Best logic, how did it ever work?
**	
**	Revision 1.6  1999/03/08 18:30:18  rubenb
**	added INCL_ALLOCA for alloca users
**	
**	Revision 1.5  1998/12/17 20:19:46  singhki
**	rename DB_SET to SDB_DBSET
**	
**	Revision 1.4  1998/12/16 22:29:53  singhki
**	fix up best db set algorithms, include Shareable in DB_SET_BEST and use it in NodeFind
**	
**	Revision 1.3  1998/12/09 17:24:14  singhki
**	minor fixes in node split code & fix uninitialized var
**	
**	Revision 1.2  1998/12/08 01:16:42  singhki
**	LeakFix: free Splits
**	
**	Revision 1.1  1998/12/02 23:05:10  singhki
**	Cache DB_SET operations like Union,Best & Remove in order to speed up NodeFind
**
****************************************************************/

#define BUILDING_SECDB
#define INCL_ALLOCA
#include	<portable.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secdrv.h>
#include	<sdbdbset.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<datatype.h>
#include	<secexpr.h>
#include	<sdb_int.h>

CC_USING( std::string );

SDB_DBSET_BEST
		*SDB_DBSET_BEST::LookupHelper;

HASH	*SecDbDbSetHash = NULL,
		*SecDbDbSetOpsCache = NULL;

EXPORT_C_SECDB SLANG_RET_CODE
		SlangXSecDbDbSetCacheInfo( SLANG_ARGS );


/****************************************************************
**	Class  : SDB_DBSET
**	Routine: HashFunc
**	Returns: Hash value
**	Action : hashes a db set
****************************************************************/

HASH_BUCKET SDB_DBSET::HashFunc(
	HASH_KEY	Key
)
{
	SDB_DBSET
			*DbSet = (SDB_DBSET *) Key;

	return DtHashQuick( (unsigned char *) DbSet->Dbs, sizeof( *DbSet->Dbs ) * DbSet->NumDbs, DbSet->DbPrimary );
}



/****************************************************************
**	Class  : SDB_DBSET
**	Routine: Compare
**	Returns: 
**	Action : 
****************************************************************/

int SDB_DBSET::Compare(
	HASH_KEY	Key1,
	HASH_KEY	Key2
)
{
	SDB_DBSET
			*DbSet1 = (SDB_DBSET *) Key1,
			*DbSet2 = (SDB_DBSET *) Key2;

	if( !DbSet1 || !DbSet2 )
		return (long) DbSet1 - (long) DbSet2;

	if( DbSet1 == DbSet2 )
		return 0;

	if( DbSet1->NumDbs != DbSet2->NumDbs )
		return DbSet1->NumDbs - DbSet2->NumDbs;

	int i;
	for( i = 0; i < DbSet1->NumDbs; ++i )
		if( DbSet1->Dbs[ i ] != DbSet2->Dbs[ i ] )
			return (long) DbSet1->Dbs[ i ] - (long) DbSet2->Dbs[ i ];

	return DbSet1->DbPrimary - DbSet2->DbPrimary;
}



/****************************************************************
**	Routine: SDB_DBSET::toString
**	Returns: a string
**	Action : what it says
****************************************************************/

string SDB_DBSET::toString(
)
{
	string res;
	if( Size() == 0 )
		return "[]";

	string sep = "";
	res = "[ ";
	for( int i = 0; i < Size(); ++i )
	{
		res += sep;
		if( Elem( i ) == Db() )
			res += "*";
		res += Elem( i )->FullDbName;
		sep = ", ";
	}
	res += " ]";
	return res;
}



/****************************************************************
**	Class  : SDB_DBSET_OPERATION
**	Routine: ~SDB_DBSET_OPERATION
**	Returns: 
**	Action : 
****************************************************************/

SDB_DBSET_OPERATION::~SDB_DBSET_OPERATION(
)
{
}



/****************************************************************
**	Class  : SDB_DBSET_OPERATION
**	Routine: HashFunc
**	Returns: HASH_BUCKET
**	Action : hashes a SDB_DBSET_OPERATION
****************************************************************/

HASH_BUCKET SDB_DBSET_OPERATION::HashFunc(
	HASH_KEY	Key
)
{
	SDB_DBSET_OPERATION *DbOp = (SDB_DBSET_OPERATION *) Key;

	unsigned long
			Hash1 = (long) DbOp->m_DbSet,
			Hash2 = (long) DbOp->m_Db,
			Hash3 = (long) DbOp->Op();

	DT_HASH_MIX( Hash1, Hash2, Hash3 );
	return Hash3;
}



/****************************************************************
**	Class  : SDB_DBSET_OPERATION
**	Routine: Compare
**	Returns: TRUE/FALSE
**	Action : compares two SDB_DBSET_OPERATION's
****************************************************************/

int SDB_DBSET_OPERATION::Compare(
	HASH_KEY	Key1,
	HASH_KEY	Key2
)
{
	SDB_DBSET_OPERATION
			*DbOp1 = (SDB_DBSET_OPERATION *) Key1,
			*DbOp2 = (SDB_DBSET_OPERATION *) Key2;

	return !( DbOp1->m_DbSet == DbOp2->m_DbSet && DbOp1->m_Db == DbOp2->m_Db && DbOp1->Op() == DbOp2->Op() );
}



/****************************************************************
**	Class  : SDB_DBSET_AND_DB_OP
**	Routine: ~SDB_DBSET_AND_DB_OP
**	Returns: 
**	Action : 
****************************************************************/

SDB_DBSET_AND_DB_OP::~SDB_DBSET_AND_DB_OP(
)
{
}



/****************************************************************
**	Class  : SDB_DBSET_PLUS
**	Routine: ~SDB_DBSET_PLUS
**	Returns: 
**	Action : 
****************************************************************/

SDB_DBSET_PLUS::~SDB_DBSET_PLUS(
)
{
}



/****************************************************************
**	Class  : SDB_DBSET_PLUS
**	Routine: Op
**	Returns: 
**	Action : 
****************************************************************/

DbSetOperator SDB_DBSET_PLUS::Op(
)
{
	return DbPlus;
}



/****************************************************************
**	Class  : SDB_DBSET_PLUS
**	Routine: ResultCompute
**	Returns: SDB_DBSET *
**	Action : compute the result
****************************************************************/

SDB_DBSET *SDB_DBSET_PLUS::ResultCompute(
)
{
	SDB_DBSET DbSet( *m_DbSet );
	DbSet.Union( m_Db );
	return DbSet;
}



/****************************************************************
**	Class  : SDB_DBSET_MINUS
**	Routine: ~SDB_DBSET_MINUS
**	Returns: 
**	Action : 
****************************************************************/

SDB_DBSET_MINUS::~SDB_DBSET_MINUS(
)
{
}



/****************************************************************
**	Class  : SDB_DBSET_MINUS
**	Routine: Op
**	Returns: 
**	Action : 
****************************************************************/

DbSetOperator SDB_DBSET_MINUS::Op(
)
{
	return DbMinus;
}



/****************************************************************
**	Class  : SDB_DBSET_MINUS
**	Routine: ResultCompute
**	Returns: SDB_DBSET *
**	Action : compute the result
****************************************************************/

SDB_DBSET *SDB_DBSET_MINUS::ResultCompute(
)
{
	SDB_DBSET DbSet( *m_DbSet );
	DbSet.Remove( m_Db );
	return DbSet;
}



/****************************************************************
**	Class  : SDB_DBSET_SET_DB_PRIMARY
**	Routine: ~SDB_DBSET_SET_DB_PRIMARY
**	Returns: 
**	Action : 
****************************************************************/

SDB_DBSET_SET_DB_PRIMARY::~SDB_DBSET_SET_DB_PRIMARY(
)
{
}



/****************************************************************
**	Class  : SDB_DBSET_SET_DB_PRIMARY
**	Routine: Op
**	Returns: 
**	Action : 
****************************************************************/

DbSetOperator SDB_DBSET_SET_DB_PRIMARY::Op(
)
{
	return DbSetPrimary;
}



/****************************************************************
**	Class  : SDB_DBSET_SET_DB_PRIMARY
**	Routine: ResultCompute
**	Returns: SDB_DBSET *
**	Action : compute the result
****************************************************************/

SDB_DBSET *SDB_DBSET_SET_DB_PRIMARY::ResultCompute(
)
{
	SDB_DBSET DbSet( *m_DbSet );
	DbSet.DbPrimarySet( m_Db );
	return DbSet;
}



/****************************************************************
**	Class  : SDB_DBSET_PLUS
**	Routine: Clone
**	Returns: SDB_DBSET_AND_DB_OP *
**	Action : clones itself
****************************************************************/

SDB_DBSET_AND_DB_OP *SDB_DBSET_PLUS::Clone(
)
{
	return new SDB_DBSET_PLUS( *this );
}



/****************************************************************
**	Class  : SDB_DBSET_MINUS
**	Routine: Clone
**	Returns: SDB_DBSET_AND_DB_OP *
**	Action : clones itself
****************************************************************/

SDB_DBSET_AND_DB_OP *SDB_DBSET_MINUS::Clone(
)
{
	return new SDB_DBSET_MINUS( *this );
}



/****************************************************************
**	Class  : SDB_DBSET_SET_DB_PRIMARY
**	Routine: Clone
**	Returns: SDB_DBSET_AND_DB_OP *
**	Action : clones itself
****************************************************************/

SDB_DBSET_AND_DB_OP *SDB_DBSET_SET_DB_PRIMARY::Clone(
)
{
	return new SDB_DBSET_SET_DB_PRIMARY( *this );
}



/****************************************************************
**	Class  : SDB_DBSET_BEST
**	Routine: SDB_DBSET_BEST
**	Returns: 
**	Action : 
****************************************************************/

SDB_DBSET_BEST::SDB_DBSET_BEST(
	SDB_DBSET	*DbSet,
	SDB_DB	*Db
)
: SDB_DBSET_OPERATION( DbSet, Db )
{
	m_FreeSplits = false;
}



/****************************************************************
**	Class  : SDB_DBSET_BEST
**	Routine: ~SDB_DBSET_BEST
**	Returns: 
**	Action : 
****************************************************************/

SDB_DBSET_BEST::~SDB_DBSET_BEST(
)
{
	if( m_FreeSplits )
		delete m_Splits;
}



/****************************************************************
**	Class  : SDB_DBSET_BEST
**	Routine: Op
**	Returns: 
**	Action : 
****************************************************************/

DbSetOperator SDB_DBSET_BEST::Op(
)
{
	return DbBest;
}



/****************************************************************
**	Class  : SDB_DBSET_BEST
**	Routine: Compute
**	Returns: 
**	Action : 
****************************************************************/

int SDB_DBSET_BEST::Compute(
)
{
	m_BestPosition = INT_MAX;
	m_BestSubPathSize = 0;
	m_FreeSplits = true;
	m_Shareable = true;
	m_AnyShareable = false;
	m_Member = false;
	m_HavePhysicalDb = false;

	int *Splits = (int *) alloca( m_DbSet->Size() * sizeof( int ));
	m_NumSplits = 0;
	m_Splits = NULL;

	for( SDB_DB **iter = m_DbSet->begin(); iter < m_DbSet->end(); ++iter )
	{
		if( (*iter)->DbPhysical == (*iter) )
			m_HavePhysicalDb = true;

		if( m_Db == *iter )
		{
			m_BestPosition = 0;
			m_BestSubPathSize = m_Db->DbDepth;
			m_Member = true;

			// if node is not being shared then a new node will be created, so we must add this
			// to the list of splits
			Splits[ m_NumSplits++ ] = iter - m_DbSet->begin();
			continue;
		}

		// If Db is of the form
		// x;DbSetElem;y where x & y may be empty, and DbSetElem is a subpath of *iter,
		// then this is deemed worthy of being a best.

		int SubPathSize;
		int DbPos = DbSubPathPosSize( m_Db, *iter, SubPathSize );

 		if( DbPos == -1 )
 		{
 			if( m_Db == SecDbClassObjectsDb )
 				DbPos = DbSubPathPosSize( m_DbSet->BaseDb(), *iter, SubPathSize ) | 0x10000;
 			else if( *iter == SecDbClassObjectsDb )	// this clearly only works if there does not exist foo;Class Objects
 				DbPos = DbSubPathPosSize( m_Db, m_Db->DbPhysical, SubPathSize ) | 0x10000;
 		}

		if( DbPos != -1 )
		{
			// If only a SubPath of DbSetElem is contained in Db, then demote the DbPos so that it can never
			// take precedence over a more complete subpath
			DbPos += m_Db->DbDepth * ( (*iter)->DbDepth - SubPathSize );
			if( DbPos < m_BestPosition || ( DbPos == m_BestPosition && SubPathSize > m_BestSubPathSize ))
			{
				m_BestPosition = DbPos;
				m_BestSubPathSize = SubPathSize;
			}
		}

		// However this logic will not share if
		// Db => p;q;t and DbSetElem => q;r;t. But since q;r;t will probably have been generated by the existance of
		// q or t, the right node will have gotten shared. If this proves insufficient, it would
		// be worth revisiting the above code here.

		// If m_Db is a better Db for DbSetElem than Db( Node ) then DbSetElem will prefer to follow
		// m_Db when a the node splits so add it to the list of splits if necessary
		DbPos = DbSubPathPosSize( *iter, m_Db, SubPathSize );

		// Shareable iff DbSetElem is of the form Foo;m_Db where Foo may be empty or DbSetElem == Class Objects
		// again this clearly only works if there does not exist X;Class Objects where X is non empty
		bool Shareable = !( SubPathSize != m_Db->DbDepth || DbPos == -1 || DbPos + m_Db->DbDepth < (*iter)->DbDepth ) || ( *iter == SecDbClassObjectsDb && m_Db->DbPhysical == m_Db );
		
		if( Shareable )
			m_AnyShareable = true;
		else
			m_Shareable = false;

		if( DbPos != -1 )
		{
			DbPos += (*iter)->DbDepth * ( m_Db->DbDepth - SubPathSize );

			int DbPos1 = DbSubPathPosSize( *iter, m_DbSet->BaseDb(), SubPathSize );
			if( DbPos1 == -1 || DbPos < DbPos1 + (*iter)->DbDepth * ( m_DbSet->BaseDb()->DbDepth - SubPathSize ))
				Splits[ m_NumSplits++ ] = iter - m_DbSet->begin();
		}
	}
	if( m_NumSplits )
	{
		m_Splits = new int[ m_NumSplits ];
		memcpy( m_Splits, Splits, sizeof( int ) * m_NumSplits );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SlangXSecDbDbSetCacheInfo
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for DbSetCacheInfo
****************************************************************/

SLANG_RET_CODE SlangXSecDbDbSetCacheInfo( SLANG_ARGS )
{
	HASH_ENTRY_PTR
			HashPtr;

	DT_VALUE
			RetValue;

	DTM_ALLOC( &RetValue, DtStructure );
	HASH_FOR( HashPtr, SecDbDbSetOpsCache )
	{
		SDB_DBSET_OPERATION
				*Op = (SDB_DBSET_OPERATION *) HashPtr->Key;

		DT_VALUE
				Elem;

		const char
				*OpName;

		if( Op->Op() == DbBest )
		{
			SDB_DBSET_BEST *Best = (SDB_DBSET_BEST *) Op;

			DTM_ALLOC( &Elem, DtStructure );
			DtComponentSetNumber( &Elem, "Position", DtDouble, Best->m_BestPosition );
			DtComponentSetNumber( &Elem, "SubPathSize", DtDouble, Best->m_BestSubPathSize );

			if( Best->m_NumSplits )
			{
				DT_VALUE
						Splits,
						Value;

				DTM_ALLOC( &Splits, DtArray );
				Value.DataType = DtString;
				for( int i = 0; i < Best->m_NumSplits; ++i )
				{
					Value.Data.Pointer = Best->DbSet()->Elem( Best->m_Splits[ i ])->FullDbName.c_str();
					DtAppend( &Splits, &Value );
				}
				DtComponentSet( &Elem, "Splits", &Splits );
				DTM_FREE( &Splits );
			}
			if( Best->m_Shareable )
				DtComponentSetNumber( &Elem, "Shareable", DtDouble, 1 );
			if( Best->m_AnyShareable )
				DtComponentSetNumber( &Elem, "AnyShareable", DtDouble, 1 );

			OpName = "Best";
		}
		else
		{
			switch( Op->Op() )
			{
				case DbPlus:
					OpName = "Plus";
					break;

				case DbMinus:
					OpName = "Minus";
					break;

				case DbSetPrimary:
					OpName = "PrimaryDb";
					break;

				default:
					OpName = "Unknown";
			}

			SDB_DBSET_AND_DB_OP *DbOp = (SDB_DBSET_AND_DB_OP *) Op;
			Elem.DataType = DtString;
			Elem.Data.Pointer = strdup( DbOp->Result()->toString().c_str() );
		}

		char *ElemName = StrPaste( OpName, "( ", Op->DbSet()->toString().c_str(), ", ", Op->Db()->FullDbName.c_str(), " )", NULL );

		DtComponentSet( &RetValue, ElemName, &Elem );
		DTM_FREE( &Elem );
		free( ElemName );
	}

	Ret->Type = SLANG_TYPE_VALUE;
	Ret->Data.Value = RetValue;

	return SLANG_OK;
}
