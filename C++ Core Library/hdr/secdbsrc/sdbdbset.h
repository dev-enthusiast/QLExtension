/****************************************************************
**
**	SDBDBSET.H	- a SDB_DBSET
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdbdbset.h,v 1.34 2004/02/02 18:17:27 khodod Exp $
**
****************************************************************/

#if !defined( IN_SDBDBSET_H )
#define IN_SDBDBSET_H

#include	<secdb.h>
#include    <secdrv.h>
#include	<sdb_int.h>
#include	<string>


// This header is included by both C and C++ compilers.
// The following section is only used by C++ compilers.

#if defined( __cplusplus )

#include	<sdb_base.h>
#include	<hash.h>
#include	<kool_ade.h>


// various globals

EXPORT_C_SECDB HASH
		*SecDbDbSetHash,				// unique hash of dbsets
		*SecDbDbSetOpsCache;			// cache of DbSet operations


// operations on a DbSet

enum DbSetOperator
{
	DbPlus,								// DbSet + Db
	DbMinus,							// DbSet - Db
	DbBest,								// Best( DbSet, Db )
	DbSetPrimary						// Set DbPrimary of DbSet
};

class EXPORT_CLASS_SECDB SDB_DBSET_OPERATION
{
public:
	SDB_DBSET_OPERATION( SDB_DBSET *DbSet, SDB_DB *Db )
	{
		m_DbSet = DbSet;
		m_Db = Db;
	};
	virtual ~SDB_DBSET_OPERATION();

	virtual DbSetOperator
			Op() = 0;

	static HASH_BUCKET HashFunc( HASH_KEY Key );
	static int Compare( HASH_KEY Key1, HASH_KEY Key2 );

	SDB_DBSET *DbSet()
	{
		return m_DbSet;
	}

	void Set( SDB_DBSET *DbSet, SDB_DB *Db )
	{
		m_DbSet = DbSet;
		m_Db = Db;
	}

	SDB_DB *Db()
	{
		return m_Db;
	}

protected:
	SDB_DBSET	*m_DbSet;

	SDB_DB	*m_Db;
};

struct EXPORT_CLASS_SECDB SDB_DBSET
{
public:
	typedef SDB_DB** iterator;
	typedef SDB_DB* const* const_iterator;

	SDB_DBSET()
	{
		Dbs = NULL;
		NumDbs = MaxDbs = 0;
		DbPrimary = -1;
	}

	SDB_DBSET( SDB_DB_ID DbId )
	{
		Init( SecDbDbFromDbID( DbId ));
	}

	SDB_DBSET( SDB_DB *Db )
	{
		Init( Db ? Db : SecDbRootDb );
	}
	SDB_DBSET( const SDB_DBSET &Src )
	{
		CopyFrom( Src );
	}
	~SDB_DBSET()
	{
		free( Dbs );
	}

	int Size() const { return NumDbs; }
	int size() const { return NumDbs; }

	SDB_DB *Elem( int i )
	{
		return Dbs[ i ];
	}

	SDB_DB *operator[]( int i )
	{
		return Dbs[ i ];
	}

	SDB_DB **begin()
	{
		return Dbs;
	}

	SDB_DB **end()
	{
		return Dbs + NumDbs;
	}

	// Position in DbSet where Db ought to be
	// note that to check if Db is a member, you must still
	// check that Pos < NumDbs && *Pos == Db
	int MemberPos( SDB_DB *Db )
	{
		SDB_DB	**Start = begin(), **End = end();
		while( Start < End )
		{
			SDB_DB **Middle = Start + ( End - Start ) / 2;

			if( *Middle == Db )
				return Middle - begin();
			else if( CompareDbs( (*Middle), Db ) < 0 )
				Start = Middle + 1;
			else
				End = Middle;
		}
		return Start - begin();
	}

	int Member( SDB_DB *Db )
	{
		int Pos = MemberPos( Db );

		return Pos < Size() && Dbs[ Pos ] == Db;
	}

	int Union( SDB_DB *Db )
	{
		int Pos = MemberPos( Db );

		if( Pos < Size() && Dbs[ Pos ] == Db )
			return FALSE;

		GrowDbs( ++NumDbs );
		memmove( Dbs + Pos + 1, Dbs + Pos, ( NumDbs - Pos - 1 ) * sizeof( SDB_DB * ));
		Dbs[ Pos ] = Db;
		if( DbPrimary >= Pos || DbPrimary < 0 )
			++DbPrimary;
		return TRUE;
	}
	int Union( SDB_DB_ID DbId )
	{
		return Union( SecDbDbFromDbID( DbId ));
	}

	// true if removed, false if not found
	int Remove( SDB_DB *Db )
	{
		int Pos = MemberPos( Db );

		if( Pos >= Size() || Dbs[ Pos ] != Db )
			return FALSE;

		--NumDbs;
		memmove( Dbs + Pos, Dbs + Pos + 1, ( NumDbs - Pos ) * sizeof( SDB_DB * ));
		if( DbPrimary >= Pos && --DbPrimary < 0 && NumDbs > 0 )
			DbPrimary = 0;
		return TRUE;
	}

	// The primary Db for this DbSet
	SDB_DB *Db()
	{
		return NumDbs > 0 && DbPrimary >= 0 ? Dbs[ DbPrimary ] : NULL;
	}
	void DbPrimarySet( SDB_DB *Db )
	{
		int Pos = MemberPos( Db );

		if( Pos >= Size() || Dbs[ Pos ] != Db )
			return;
		DbPrimary = Pos;
	}

	// The Db with the shortest path length
	SDB_DB *BaseDb()
	{
		return Size() > 0 ? Dbs[ 0 ] : NULL;
	}

	const SDB_DBSET &operator=( const SDB_DBSET &Src )
	{
		// check for self assignment
		if(this == &Src)
			return *this;

		free( Dbs );
		CopyFrom( Src );
		return *this;
	}

	operator SDB_DBSET *()
	{
		SDB_DBSET *DbSet = (SDB_DBSET *) HashLookup( SecDbDbSetHash, this );
		if( !DbSet )
		{
			DbSet = new SDB_DBSET( *this );
			HashInsert( SecDbDbSetHash, DbSet, DbSet );
		}
		return DbSet;
	}

	static HASH_BUCKET HashFunc( HASH_KEY DbSet );
	static int Compare( HASH_KEY DbSet1, HASH_KEY DbSet2 );

	CC_NS(std,string) toString();

	struct dbset_less {
		bool operator() ( SDB_DBSET const * const p, SDB_DBSET const * const q ) const
		{
			return Compare( static_cast< HASH_KEY >( const_cast< SDB_DBSET * > ( p ) ), 
							static_cast< HASH_KEY >( const_cast< SDB_DBSET * > ( q ) ) ) < 0;
		}
	};

protected:
	SDB_DB	**Dbs;

	int		MaxDbs,						// size of Dbs array
			NumDbs,						// number of valid Dbs in array
			DbPrimary;					// index of Primary Db

	void GrowDbs( int Num )
	{
		if( Num > MaxDbs )
			MaxDbs = Num + 4;
		Dbs = (SDB_DB **) realloc( Dbs, sizeof( SDB_DB * ) * MaxDbs );
	}

	void Init( SDB_DB *Db )
	{
		Dbs = NULL;
		NumDbs = MaxDbs = 0;

		GrowDbs( 1 );
		DbPrimary = 0;
		Dbs[ NumDbs++ ] = Db;
	}

	void CopyFrom( const SDB_DBSET &Src )
	{
		NumDbs = MaxDbs = Src.NumDbs;
		DbPrimary = Src.DbPrimary;
		Dbs = (SDB_DB **) malloc( sizeof( SDB_DB * ) * NumDbs );
		memcpy( Dbs, Src.Dbs, sizeof( SDB_DB * ) * NumDbs );
	}

	int CompareDbs( SDB_DB *Db1, SDB_DB *Db2 )
	{
		// make sure that ClassObjectsDb is always the BaseDb if it belongs to a dbset
		if( Db1 == SecDbClassObjectsDb )
			return -1;
		else if( Db2 == SecDbClassObjectsDb )
			return 1;
		else if( Db1->DbDepth != Db2->DbDepth )
			return Db1->DbDepth - Db2->DbDepth;
		return (long) Db1 - (long) Db2;
	}
};


inline int DbSubPathPos( SDB_DB *Parent, SDB_DB *Db )
{
	for( int pos = 0; Parent && Parent->DbDepth >= Db->DbDepth; Parent = Parent->Right, ++pos )
	{
		if( Parent == Db )
		{
			return pos;
		}
		else if( Parent->DbOperator == SDB_DB_UNION )
		{
			SDB_DB *Db1, *Db2;
			// Db2 will assuredly run out before Db1 becase Parent's DbDepth > Db's DbDepth
			for( Db1 = Parent, Db2 = Db; Db2 && Db2->DbOperator == SDB_DB_UNION && Db1->Left == Db2->Left; Db1 = Db1->Right, Db2 = Db2->Right );
			if( !Db2 )					// if Db2 ran out then the match was successful
			{
				return pos;
			}
		}
	}
	return -1;
}

inline int DbSubPathPosSize( SDB_DB *Parent, SDB_DB *Db, int &Size )
{
	Size = Db->DbDepth;
	int pos;
	for( ; Db; Db = Db->Right, --Size )
		if( ( pos = DbSubPathPos( Parent, Db )) != -1 )
			return pos;
	return -1;
}

class EXPORT_CLASS_SECDB SDB_DBSET_AND_DB_OP : public SDB_DBSET_OPERATION
{
public:
	SDB_DBSET_AND_DB_OP( SDB_DBSET *DbSet, SDB_DB *Db ) : SDB_DBSET_OPERATION( DbSet, Db )
	{
		m_Result = NULL;
	}
	virtual ~SDB_DBSET_AND_DB_OP();

	virtual SDB_DBSET_AND_DB_OP *Clone() = 0;

	SDB_DBSET *Result()
	{
		if( !m_Result )
		{
			SDB_DBSET_AND_DB_OP *Cached = (SDB_DBSET_AND_DB_OP *)HashLookup( SecDbDbSetOpsCache, (HASH_KEY) this );

			if( !Cached )
			{
				m_Result = ResultCompute();

				SDB_DBSET_AND_DB_OP *DbSetOp = this->Clone();
				if( !HashInsert( SecDbDbSetOpsCache, (HASH_KEY) DbSetOp, (HASH_VALUE) DbSetOp ))
					delete DbSetOp;
			}
			else
				m_Result = Cached->m_Result;
		}
		return m_Result;
	}

	operator SDB_DBSET *()
	{
		return Result();
	}

	virtual SDB_DBSET *ResultCompute() = 0;

protected:
	SDB_DBSET	*m_Result;
};

class EXPORT_CLASS_SECDB SDB_DBSET_PLUS : public SDB_DBSET_AND_DB_OP
{
public:
	SDB_DBSET_PLUS( SDB_DBSET *DbSet, SDB_DB *Db ) : SDB_DBSET_AND_DB_OP( DbSet, Db )
	{}
	virtual ~SDB_DBSET_PLUS();

	virtual DbSetOperator Op();

	virtual SDB_DBSET *ResultCompute();

	virtual SDB_DBSET_AND_DB_OP *Clone();
};

class EXPORT_CLASS_SECDB SDB_DBSET_MINUS : public SDB_DBSET_AND_DB_OP
{
public:
	SDB_DBSET_MINUS( SDB_DBSET *DbSet, SDB_DB *Db ) : SDB_DBSET_AND_DB_OP( DbSet, Db )
	{}
	virtual ~SDB_DBSET_MINUS();

	virtual DbSetOperator Op();

	virtual SDB_DBSET *ResultCompute();

	virtual SDB_DBSET_AND_DB_OP *Clone();
};

class EXPORT_CLASS_SECDB SDB_DBSET_SET_DB_PRIMARY : public SDB_DBSET_AND_DB_OP
{
public:
	SDB_DBSET_SET_DB_PRIMARY( SDB_DBSET *DbSet, SDB_DB *Db ) : SDB_DBSET_AND_DB_OP( DbSet, Db )
	{}
	virtual ~SDB_DBSET_SET_DB_PRIMARY();

	virtual DbSetOperator Op();

	virtual SDB_DBSET *ResultCompute();

	virtual SDB_DBSET_AND_DB_OP *Clone();
};

class EXPORT_CLASS_SECDB SDB_DBSET_BEST : public SDB_DBSET_OPERATION
{
public:
	SDB_DBSET_BEST( SDB_DBSET *DbSet, SDB_DB *Db );
	virtual ~SDB_DBSET_BEST();

	virtual DbSetOperator
			Op();

	int		m_BestPosition,
			m_BestSubPathSize,
			m_NumSplits;

	int		*m_Splits;

	bool	m_Shareable,				// All Dbs in m_DbSet are of the form Foo;m_Db
			m_AnyShareable,				// At least one Db in m_DbSet is of the form Foo;m_Db
			m_Member,					// m_Db is a member of m_DbSet
			m_HavePhysicalDb;			// At least one of the Dbs is actually a physical Db

	int		Compute();

	SDB_DB	*SplitDb( int i )
	{
		return m_DbSet->Elem( m_Splits[ i ]);
	}

	inline static SDB_DBSET_BEST *Lookup( SDB_DBSET *DbSet, SDB_DB *Db )
	{
		LookupHelper->Set( DbSet, Db );

		SDB_DBSET_BEST
				*Cached = (SDB_DBSET_BEST *) HashLookup( SecDbDbSetOpsCache, (HASH_KEY) LookupHelper );

		if( !Cached )
		{
			Cached = new SDB_DBSET_BEST( DbSet, Db );
			if( !Cached->Compute() || !HashInsert( SecDbDbSetOpsCache, (HASH_KEY) Cached, (HASH_VALUE) Cached ))
			{
				delete Cached;
				Cached = NULL;
			}
		}
		return Cached;
	}

private:
	bool	m_FreeSplits;

	static SDB_DBSET_BEST
			*LookupHelper;

	friend int
			SecDbInitialize( SDB_MSG_FUNC, SDB_MSG_FUNC, const char *, const char * );
};

inline int DbSetBestGreater( SDB_DBSET_BEST *Set1, SDB_DBSET_BEST *Set2 )
{
	// A DbSetBest doesn't count unless it's BestPosition is non singular
	if( !Set1 || Set1->m_BestPosition == INT_MAX )
		return 0;

	if( !Set2 )
		return 1;

	return Set1->m_BestPosition < Set2->m_BestPosition
			|| ( Set1->m_BestPosition == Set2->m_BestPosition
				 && Set1->m_BestSubPathSize > Set2->m_BestSubPathSize );
}

#endif 	// _cplusplus


#endif
