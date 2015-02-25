/****************************************************************
**
**	GSHASH.H	-
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsHash.h,v 1.54 2012/07/19 13:12:15 e19351 Exp $
**
****************************************************************/

#if !defined( IN_GSHASH_H )
#define IN_GSHASH_H

#include <memory>
#include <functional>

#include <gscore/base.h>
#include <gscore/GsHashForward.h>

CC_BEGIN_NAMESPACE(Gs)


template< class Key >
class GsHashFunctorCast
{
public:
	inline unsigned long operator()( Key KeyToCast ) const 
	{
		return (unsigned long) KeyToCast;
	}
};



/****************************************************************
** Class	   : GsHashMap
** Description : A map implemented as a hash table.
****************************************************************/

template<
	class KeyT,
	class ValueT,
	class HashFunctor,
	class Compare						// Default value for this arg is already specified in GsHashForward.h
>
class GsHashMap {
public:
	struct KeyValue {
		KeyT 	m_Key;
		ValueT	m_Value;

		inline KeyValue( const KeyT &Key, const ValueT &Value ) : m_Key( Key ), m_Value( Value ) {}
//		inline KeyValue( KeyT  Key, ValueT  Value ) : m_Key( Key ), m_Value( Value ) {}

		inline const KeyT   & Key()   const { return m_Key;   }
		inline const ValueT & Value() const { return m_Value; }
	};

	struct KeyValueInitializer
	{
		KeyT	m_Key;
		ValueT	m_Value;
	};

	class Entry;
	class ConstEntry;
	class Iterator;
	class ConstIterator;

private:
	friend class Entry;
	friend class Iterator;
	friend class ConstEntry;
	friend class ConstIterator;

	struct InternalNode {
		KeyValue
				m_KeyValue;

		InternalNode
				*m_BucketNext,
				*m_BucketPrevious,
				*m_AllNext,
				*m_AllPrevious;

		unsigned long
				m_NumElements,
				m_MaxElementsBeforeGrowth;

		inline InternalNode( const KeyT &Key, const ValueT &Value,
							 InternalNode *BucketNext, InternalNode *BucketPrevious,
							 InternalNode *AllNext, InternalNode *AllPrevious ) 
		:
			m_KeyValue( Key, Value ),
			m_BucketNext( BucketNext ),
			m_BucketPrevious( BucketPrevious ),
			m_AllNext( AllNext ),
			m_AllPrevious( AllPrevious )
		{
			m_AllNext->m_AllPrevious 	   = m_AllPrevious->m_AllNext 		= this;
			m_BucketNext->m_BucketPrevious = m_BucketPrevious->m_BucketNext = this;
		}
	};

	typedef 	  InternalNode 		*InternalNodeP;
	typedef const InternalNode *ConstInternalNodeP;

	// Typedef to work around VC5.0 Internal compiler error
	typedef KeyValueInitializer KeyValueStruct;

	// The sentinal is at the beginning and end of the list of all
	// nodes.  It is also pointed to by the first and last element of
	// the list in each bucket.  This simplifies a lot of code that
	// now does not have to check for special cases involving NULL
	// pointers.

	// The m_BucketNext and m_BucketPrevious members of the sentinal
	// are meaningless.  We should never rely on them to point to
	// anything in particular.  They are, however, occasionally
	// changed, by e.g. ConstReference::Delete(), but this is again
	// just to eliminate special case NULL checks.
	InternalNode
			m_NodeSentinal;

	InternalNodeP
			*m_Buckets;

	unsigned long
			m_Size,
			m_NumElements,
			m_MaxElementsBeforeGrowth;

	HashFunctor
			m_Hash;

	// Helpers for construction, destruction and assignment.
	void	Initialize( unsigned long InitialSize );
	void	Destroy();
	void 	CopyFromRhs( const GsHashMap< KeyT, ValueT, HashFunctor, Compare > &rhs );

public:
	GsHashMap( unsigned long InitialSize = 17 );
	GsHashMap( const GsHashMap< KeyT, ValueT, HashFunctor, Compare > &rhs );
	// This function should take a pointer but VC5.0 barfs so I had to do a typedef and make it a reference, arghh
	GsHashMap( const KeyValueStruct &InitialPairs, unsigned long NumPairs );
	~GsHashMap() ;

	GsHashMap< KeyT, ValueT, HashFunctor, Compare > &operator=( const GsHashMap< KeyT, ValueT, HashFunctor, Compare > &rhs );

	bool operator==( const GsHashMap< KeyT, ValueT, HashFunctor, Compare > &rhs ) const;

	size_t size() const { return m_NumElements; }

    void InitializeBucket( const KeyT &Key, InternalNode *next );

	// An entry in the table.  Use * or -> to dereference.

	class Entry
	{

	protected:
		InternalNodeP
				m_Node;
	    GsHashMap
	            *m_HashMap;

	public:

		typedef KeyValue   value_type;
		typedef KeyValue&  reference_type;

		Entry( GsHashMap *HashMap = NULL, InternalNodeP Node = NULL ) : m_Node( Node ) { m_HashMap = HashMap; }
		Entry( const Entry &Rhs ) 	 : m_Node( Rhs.m_Node ), m_HashMap( Rhs.m_HashMap ) {}

		inline bool operator==( const Entry &Rhs ) const  { return ( m_Node == Rhs.m_Node ); }
		inline bool operator!=( const Entry &Rhs ) const  { return ( m_Node != Rhs.m_Node ); }

		inline KeyValue& operator* () const  { return   m_Node->m_KeyValue ; }
		inline KeyValue* operator->() const  { return &(m_Node->m_KeyValue); }

		inline operator bool() const { return ( NULL != m_HashMap && m_Node != &m_HashMap->m_NodeSentinal ); }

		// FIX - MSVC5.0 refuses to instatiate this if it is defined
		// outside this class declaration.  If this can be resolved,
		// we can move it out.

		void Delete()
		{
			if ( m_Node->m_BucketPrevious == &m_HashMap->m_NodeSentinal ) 
				m_HashMap->InitializeBucket( m_Node->m_KeyValue.m_Key, m_Node->m_BucketNext );

			m_Node->m_BucketPrevious->m_BucketNext = m_Node->m_BucketNext;
			m_Node->m_BucketNext->m_BucketPrevious = m_Node->m_BucketPrevious;

			m_Node->m_AllPrevious->m_AllNext = m_Node->m_AllNext;
			m_Node->m_AllNext->m_AllPrevious = m_Node->m_AllPrevious;

			delete m_Node;

			m_Node = NULL;

			--m_HashMap->m_NumElements;
		}

		friend class GsHashMap< KeyT, ValueT, HashFunctor, Compare >;
		friend class ConstEntry;
	};


	// Iterator over the contents of the table.  Like an entry, but
	// with iteration, hence the inheritence.

	class Iterator : public Entry
	{

	public:
        typedef CC_NS(std,bidirectional_iterator_tag) iterator_category;
        typedef void                                  difference_type;
        typedef typename Entry::value_type*           pointer;
        typedef typename Entry::value_type&           reference;

	    Iterator( GsHashMap *HashMap, InternalNodeP Node )  : Entry( HashMap, Node ) {}
		Iterator( const Iterator &Rhs )  : Entry( Rhs.m_HashMap, Rhs.m_Node ) {}
		Iterator( const Entry &Rhs ) : Entry( Rhs ) {} 
		Iterator( GsHashMap *Hash, const KeyT &Rhs ) 
		: Entry( Hash->Lookup( Rhs ) )
		{			 			 	
		}

		inline Iterator operator++() 
		{
			this->m_Node = this->m_Node->m_AllNext;
			return *this;
		}

		inline Iterator operator++( int ) 
		{
			Iterator Tmp = *this;

			this->m_Node = this->m_Node->m_AllNext;
			return Tmp;
		}

		inline Iterator operator--() 
		{
			this->m_Node = this->m_Node->m_AllPrevious;
			return *this;
		}

		inline Iterator operator--( int ) 
		{
			Iterator Tmp = *this;

			this->m_Node = this->m_Node->m_AllPrevious;
			return Tmp;
		}

		inline void Delete() 
		{
			Iterator
					Next = *this;

			++Next;

			Entry::Delete();

			*this = Next;
		}

		friend class ConstIterator;
	};

	// An entry in the table.  Use * or -> to dereference.

	class ConstEntry
	{

	protected:
		ConstInternalNodeP
				m_Node;
	    const GsHashMap
	            *m_HashMap;

	public:
		typedef KeyValue value_type;

        typedef CC_NS(std,bidirectional_iterator_tag) iterator_category;
        typedef void                                  difference_type;
        typedef value_type*                           pointer;
        typedef value_type&                           reference;

		ConstEntry( const GsHashMap *HashMap = NULL, ConstInternalNodeP Node = NULL )  : m_Node( Node ), m_HashMap( HashMap ) {}
		ConstEntry( const ConstEntry &Rhs ) 	 : m_Node( Rhs.m_Node ) { m_HashMap = Rhs.m_HashMap; }
		ConstEntry( const      Entry &Rhs ) 	 : m_Node( Rhs.m_Node ) { m_HashMap = Rhs.m_HashMap; }

		inline operator bool() const { return ( NULL != m_HashMap ); }

		inline bool operator==( const ConstEntry &Rhs ) const  { return ( m_Node == Rhs.m_Node ); }
		inline bool operator!=( const ConstEntry &Rhs ) const  { return ( m_Node != Rhs.m_Node ); }

		inline const KeyValue& operator* () const  { return   m_Node->m_KeyValue ; }
		inline const KeyValue* operator->() const  { return &(m_Node->m_KeyValue); }

		friend class GsHashMap< KeyT, ValueT, HashFunctor, Compare >;
	};


	// Iterator over the contents of the table.  Like an entry, but
	// with iteration, hence the inheritence.

	class ConstIterator : public ConstEntry
	{

	public:
		ConstIterator( const GsHashMap *HashMap, ConstInternalNodeP Node ) : ConstEntry( HashMap, Node ) {}
		ConstIterator( const ConstIterator &Rhs )  : ConstEntry( Rhs.m_HashMap, Rhs.m_Node ) {}
		ConstIterator( const      Iterator &Rhs )  : ConstEntry( Rhs.m_HashMap, Rhs.m_Node ) {}
		ConstIterator( const ConstEntry &Rhs ) : ConstEntry( Rhs ) {} 
		ConstIterator( const Entry &Rhs ) : ConstEntry( Rhs ) {} 

		inline ConstIterator operator++() 
		{
			this->m_Node = this->m_Node->m_AllNext;
			return *this;
		}

		inline ConstIterator operator++( int ) 
		{
			ConstIterator Tmp = *this;

			this->m_Node = this->m_Node->m_AllNext;
			return Tmp;
		}

		inline ConstIterator operator--() 
		{
			this->m_Node = this->m_Node->m_AllPrevious;
			return *this;
		}

		inline ConstIterator operator--( int ) 
		{
			ConstIterator Tmp = *this;

			this->m_Node = this->m_Node->m_AllPrevious;
			return Tmp;
		}
	};

	inline Iterator Begin()   { return Iterator( this, m_NodeSentinal.m_AllNext ); }
	inline Iterator End()	  { return Iterator( this, &m_NodeSentinal		  	); }

	inline ConstIterator Begin() const	{ return ConstIterator( this, m_NodeSentinal.m_AllNext 	); }
	inline ConstIterator End()	 const	{ return ConstIterator( this, &m_NodeSentinal		  	); }

	// And more STL-like versions.

	typedef KeyValue 		value_type;

	typedef Iterator 		iterator;
	typedef ConstIterator 	const_iterator;

    typedef ptrdiff_t                      difference_type;
    typedef value_type*                    pointer;
    typedef value_type&                    reference;
	typedef const value_type&              const_reference;

	inline iterator begin()   { return Begin(); }
	inline iterator end()	  { return End(); 	}

	inline const_iterator begin() const	{ return Begin();	}
	inline const_iterator end()	  const	{ return End(); 	}

	iterator insert( iterator Iter, const value_type &KV ) { return Iterator( this, Insert( KV.m_Key, KV.m_Value ) ); }

	inline unsigned long Hash( const KeyT &Key ) const { return m_Hash( Key ); }

    InternalNodeP Insert( const KeyT &Key, const ValueT &Value );

	Entry LookupWithHash( const KeyT &Key, unsigned long Hash );
	inline Entry Lookup( const KeyT &Key )  { return LookupWithHash( Key, Hash( Key ) ); }
	inline Entry find( const KeyT &Key ) { return Lookup( Key ); }

	ConstEntry LookupWithHash( const KeyT &Key, unsigned long Hash ) const;
	inline ConstEntry Lookup( const KeyT &Key ) const { return LookupWithHash( Key, Hash( Key ) ); }
	inline ConstEntry find( const KeyT &Key ) const { return Lookup( Key ); }

	inline size_t erase( const KeyT &Key )
	{
		Entry entry = find( Key );
		if( !entry )
			return 0;
		entry.Delete();
		return 1;
	}

	void Resize( unsigned long NewSize ) ;

	ValueT &operator[] ( const KeyT &Key );
	const ValueT &operator[] ( const KeyT &Key ) const;
};



CC_END_NAMESPACE


#endif




