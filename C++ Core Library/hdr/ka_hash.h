/****************************************************************
**
**	KA_HASH.H	- Templated hash
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/kool_ade/src/ka_hash.h,v 1.5 2001/11/27 22:49:05 procmon Exp $
**
****************************************************************/

#if !defined( IN_KOOL_ADE_KA_HASH_H )
#define IN_KOOL_ADE_KA_HASH_H

#include <functional>
#include <ka_base.h>
#include <hash.h>

template<class TKey, class THashFunc, class TCompare>
class KAHashKey
{
public:
	KAHashKey( TKey KeyNull, TKey KeyDeleted, unsigned InitialSize = 8 )
	{
		this->KeyNull = KeyNull;
		this->KeyDeleted = KeyDeleted;
		Size = NextSize( InitialSize );
		RebuildCount = ( Size * 3 ) / 4;
		KeyCount = DeleteCount = 0;
		Table = new TKey[ Size ];
		for( TKey *Ptr = Table + Size; --Ptr >= Table; )
			*Ptr = KeyNull;
	}
	~KAHashKey()
	{
		delete [] Table;
	}

	unsigned long GetHash( TKey Key )
	{
		return HashFunc( Key ) % Size;
	}

	TKey *Lookup( TKey Key )
	{
 		for( TKey *Ptr = Table + GetHash( Key ); *Ptr != KeyNull; Ptr = Ptr > Table ? Ptr - 1 : Table + Size - 1 )
 			if( *Ptr != KeyDeleted && Compare( *Ptr, Key ))
 				return Ptr;
		return NULL;
	}

	TKey *Insert( TKey Key )
	{
		TKey	*Ptr,
				*InsertPos = NULL;

		for( Ptr = Table + GetHash( Key ); *Ptr != KeyNull; Ptr = Ptr > Table ? Ptr - 1 : Table + Size - 1 )
		{
			if( *Ptr == KeyDeleted )
				InsertPos = Ptr;
			else if( Compare( *Ptr, Key ))
				return Ptr;
		}
		++KeyCount;
		if( !InsertPos )
			InsertPos = Ptr;
		else
			--DeleteCount;

		*InsertPos = Key;
		if( !NeedToRebuild() )
			return InsertPos;

		Rebuild();
		return Lookup( Key );		// inefficient, but then how often does one rebuild?
	}

	bool Delete( TKey Key )
	{
		TKey *Ptr = Lookup( Key );
		if( !Ptr )
			return false;

		--KeyCount;
		if( *( Ptr > Table ? Ptr - 1 : Table + Size - 1 ) == KeyNull )
		{
			// Next bucket is empty so walk back up emptying buckets
			*Ptr = KeyNull;
			while( *( Ptr = ( Ptr == Table + Size - 1 ? Table : Ptr + 1 )) == KeyDeleted )
			{
				*Ptr = KeyNull;
				--DeleteCount;
			}
		}
		else
		{
			// Next bucket is full or deleted, so put in fake null in order to maintain the chain
			*Ptr = KeyDeleted;
			++DeleteCount;
			if( NeedToRebuild() )
				Rebuild();
		}
		return true;
	}

	double Statistics( HASH_USABILITY *Usability );

protected:

	TKey	*Table,
			KeyNull,
			KeyDeleted;

	unsigned
			Size,
			KeyCount,
			DeleteCount,
			RebuildCount;

	TCompare
			Compare;

	THashFunc
			HashFunc;

	int NeedToRebuild()
	{
		return KeyCount + DeleteCount > RebuildCount;
	}

	unsigned NextSize( unsigned CurrentSize )
	{
		return CurrentSize * 2;
	}

	void Rebuild()
	{
		TKey *OldTable = Table, *Ptr = OldTable + Size, *Ptr1;

		Size = NextSize( Size );
		RebuildCount = ( Size * 3 ) / 4;
		DeleteCount = 0;

		Table = new TKey[ Size ];
		for( Ptr1 = Table + Size; --Ptr1 >= Table; )
			*Ptr1 = KeyNull;

		while( --Ptr >= OldTable )
		{
			if( *Ptr != KeyNull && *Ptr != KeyDeleted )
			{
				for( Ptr1 = Table + GetHash( *Ptr ); *Ptr1 != KeyNull; Ptr1 = Ptr1 > Table ? Ptr1 - 1 : Table + Size - 1 );
				*Ptr1 = *Ptr;
			}
		}

		delete [] OldTable;
	}
};

template<class TKey, class TValue, class THashFunc, class TCompare = CC_NS(std,equal_to)<TKey> >
class KAHashMap
{
public:
	struct KeyValue
	{
		TKey	Key;
		TValue	Value;
	};

	KAHashMap( TKey KeyNull, TKey KeyDeleted, TValue ValueNull, unsigned InitialSize = 8 )
	{
		m_KeyNull = KeyNull;
		m_KeyDeleted = KeyDeleted;
		m_ValueNull = ValueNull;
		m_Size = NextSize( InitialSize );
		m_RebuildCount = ( m_Size * 3 ) / 4;
		m_KeyCount = m_DeleteCount = 0;
		m_Table = new KeyValue[ m_Size ];
		for( KeyValue *Ptr = m_Table + m_Size; --Ptr >= m_Table; )
			Ptr->Key = m_KeyNull;
	}
	~KAHashMap()
	{
		delete [] m_Table;
	}

	unsigned long GetHash( TKey Key )
	{
		return m_HashFunc( Key ) % m_Size;
	}

	KeyValue *Lookup( TKey Key )
	{
 		for( KeyValue *Ptr = m_Table + GetHash( Key ); !m_Compare( Ptr->Key, m_KeyNull ); Ptr = Ptr > m_Table ? Ptr - 1 : m_Table + m_Size - 1 )
 			if( !m_Compare( Ptr->Key, m_KeyDeleted ) && m_Compare( Ptr->Key, Key ))
 				return Ptr;
		return NULL;
	}

	TValue LookupValue( TKey Key )
	{
 		for( KeyValue *Ptr = m_Table + GetHash( Key ); !m_Compare( Ptr->Key, m_KeyNull ); Ptr = Ptr > m_Table ? Ptr - 1 : m_Table + m_Size - 1 )
 			if( !m_Compare( Ptr->Key, m_KeyDeleted ) && m_Compare( Ptr->Key, Key ))
 				return Ptr->Value;
		return m_ValueNull;
	}

	KeyValue *Insert( TKey Key, TValue Value )
	{
		KeyValue
				*Ptr,
				*InsertPos = NULL;

		for( Ptr = m_Table + GetHash( Key ); !m_Compare( Ptr->Key, m_KeyNull ); Ptr = Ptr > m_Table ? Ptr - 1 : m_Table + m_Size - 1 )
		{
			if( !m_Compare( Ptr->Key, m_KeyDeleted ))
				InsertPos = Ptr;
			else if( m_Compare( Ptr->Key, Key ))
				return Ptr;
		}
		++m_KeyCount;
		if( !InsertPos )
			InsertPos = Ptr;
		else
			--m_DeleteCount;

		InsertPos->Key = Key;
		InsertPos->Value = Value;
		if( !NeedToRebuild() )
			return InsertPos;

		Rebuild();
		return Lookup( Key );		// inefficient, but then how often does one rebuild?
	}

	bool Delete( TKey Key, TValue *Value = NULL )
	{
		KeyValue *Ptr = Lookup( Key );
		if( !Ptr )
			return false;
		if( Value )
			*Value = Ptr->Value;

		--m_KeyCount;
		if( m_Compare( ( Ptr > m_Table ? Ptr - 1 : m_Table + m_Size - 1 )->Key, m_KeyNull ))
		{
			// Next bucket is empty so walk back up emptying buckets
			Ptr->Key = m_KeyNull;
			while( m_Compare( ( Ptr = ( Ptr == m_Table + m_Size - 1 ? m_Table : Ptr + 1 ))->Key, m_KeyDeleted ))
			{
				Ptr->Key = m_KeyNull;
				--m_DeleteCount;
			}
		}
		else
		{
			// Next bucket is full or deleted, so put in fake null in order to maintain the chain
			Ptr->Key = m_KeyDeleted;
			++m_DeleteCount;
			if( NeedToRebuild() )
				Rebuild();
		}
		return true;
	}

    KeyValue *Table()    { return m_Table; }

    unsigned Size()      { return m_Size; }
	unsigned KeyCount()  { return m_KeyCount; }

    TKey KeyNull()       { return m_KeyNull; }
    TKey KeyDeleted()    { return m_KeyDeleted; }

    double Statistics( HASH_USABILITY *Usability );

protected:

	KeyValue
			*m_Table;

	TKey	m_KeyNull,
			m_KeyDeleted;

	unsigned
			m_Size,
			m_KeyCount,
			m_DeleteCount,
			m_RebuildCount;

	TValue	m_ValueNull;

	TCompare
			m_Compare;

	THashFunc
			m_HashFunc;

	int NeedToRebuild()
	{
		return m_KeyCount + m_DeleteCount > m_RebuildCount;
	}

	unsigned NextSize( unsigned CurrentSize )
	{
		return CurrentSize * 2;
	}

	void Rebuild()
	{
		KeyValue *OldTable = m_Table, *Ptr = OldTable + m_Size, *Ptr1;

		m_Size = NextSize( m_Size );
		m_RebuildCount = ( m_Size * 3 ) / 4;
		m_DeleteCount = 0;

		m_Table = new KeyValue[ m_Size ];
		for( Ptr1 = m_Table + m_Size; --Ptr1 >= m_Table; )
			Ptr1->Key = m_KeyNull;

		while( --Ptr >= OldTable )
		{
			if( !m_Compare( Ptr->Key, m_KeyNull ) && !m_Compare( Ptr->Key, m_KeyDeleted ))
			{
				for( Ptr1 = m_Table + GetHash( Ptr->Key ); Ptr1->Key != m_KeyNull; Ptr1 = Ptr1 > m_Table ? Ptr1 - 1 : m_Table + m_Size - 1 );
				*Ptr1 = *Ptr;
			}
		}

		delete [] OldTable;
	}
};

#define KAHashMapFor( Entry, Map )													\
	for( Entry = Map->Table() + Map->Size() - 1; Entry >= Map->Table(); --Entry )	\
		if( Entry->Key != Map->KeyNull() && Entry->Key != Map->KeyDeleted() )

#endif 
