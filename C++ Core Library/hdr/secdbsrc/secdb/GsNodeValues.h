// -*-c++-*-
/****************************************************************
**
**	secdb/GsNodeValues.h	- GsNodeValues class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/GsNodeValues.h,v 1.14 2012/06/11 19:43:44 e19351 Exp $ 
**
****************************************************************/

#ifndef IN_SECDB_GSNODEVALUES_H
#define IN_SECDB_GSNODEVALUES_H

#include <iterator>
#include <secdb/base.h>
#include <gscore/types.h>
#include <secnodei.h>
#include <vector>
#include <ccstl.h>

class EXPORT_CLASS_SECDB GsNodeValues
{
protected:
	static DT_VALUE
			s_NullValue;

	int m_size;							// size of vec values

    // prevent copying
    GsNodeValues( GsNodeValues const& Other ); // undefined
    GsNodeValues& operator=( GsNodeValues const& Rhs ); // undefined

public:
    explicit GsNodeValues( int size = 0 ) : m_size( size ) {}
    // use default destructor - do nothing, we do not own m_Node
	virtual ~GsNodeValues()
	{}

	DT_VALUE* operator[]( int Index ) const
	{
	    return this->lookup( Index );
	}

	virtual DT_VALUE* lookup( int Index ) const = 0;

	virtual DT_VALUE* lookup_no_null( int Index ) const = 0;

    double getNumber( int Index ) const
    {
		DT_VALUE* val = this->lookup_no_null( Index );
		return val ? val->Data.Number : HUGE_VAL;
	}

    void const* getPointer( int Index ) const
    {
		DT_VALUE* val = this->lookup_no_null( Index );
		return val ? val->Data.Pointer : 0;
	}

	int size() const { return m_size; }
	
    class DtIter;

    inline DtIter begin();
    inline DtIter end();
};

class EXPORT_CLASS_SECDB SecDbVectorValue : public GsNodeValues
{
private:
	SDB_NODE* m_node; // We do not own this class

    // prevent copying
    SecDbVectorValue( SecDbVectorValue const& Other ); // undefined
    SecDbVectorValue& operator=( SecDbVectorValue const& Rhs ); // undefined

public:
    explicit SecDbVectorValue( SDB_NODE* node = 0 ) : GsNodeValues( node ? node->ChildcGet() : 0 ), m_node( node ) {}
	~SecDbVectorValue()
	{}

	DT_VALUE* lookup( int Index ) const
	{
		DT_VALUE*
				ret = NULL;
  
		SDB_NODE* child = m_node->ChildGet( Index );
		if( !( child && child->ValidGet() && ( ret = child->AccessValue() ) ) )
		{
			DTM_INIT( &s_NullValue );
			ret = &s_NullValue;
		}

		return ret;
	}

	DT_VALUE* lookup_no_null( int Index ) const
	{
		DT_VALUE*
				ret = NULL;
  
		SDB_NODE* child = m_node->ChildGet( Index );
		if( !( child && child->ValidGet() && ( ret = child->AccessValue() ) ) )
			return 0;

		return ret;
	}

	// no need to free old node since we do not own it, simply replace it.
	void set_node( SDB_NODE* node ) { m_node = node; }
};

//
// Convenience Class
//
// usage:
//
//   GsNodeValuesProxy<double> Doubles( ChildData->Arg );
//   for( int ii = 0; ii < ChildData->Arg->size(); ++ii )
//   {
//       // instead of using ChildData->Arg->getNumber( ii )
//       // you can use Doubles[ ii ]
//       func( Doubles[ ii ], ... );
//   }
//
// Note: The template argument is the type you want operator[] to return.

template <class Ret>
class GsNodeValuesProxy
{
public:
    explicit GsNodeValuesProxy( GsNodeValues* values = 0 ) : m_Values( values ) {}
    // use default copy constructor
    // use default destructor
    // use default assignment operator

    Ret operator[]( int Index ) const { return m_Values ? (Ret)m_Values->getNumber( Index ) : (Ret)HUGE_VAL; }

	void operator=( GsNodeValues* values ) { m_Values = values; }

private:
    GsNodeValues* m_Values;
};

//
// Convenience Class
//
// usage:
//
//   GsNodeValuesProxyP<DT_VALUE*> Values( ChildData->Arg );
//   for( int ii = 0; ii < ChildData->Arg->size(); ++ii )
//   {
//       // instead of using (DT_VALUE*)( ChildData->Arg->getPointer( ii ))
//       // you can use Values[ ii ]
//       func( Values[ ii ], ... );
//   }
//
// NOTE: The template argument is the type you want operator[] to return.
//

template <class Ret>
class GsNodeValuesProxyP /* Ugh - SW4.2 can't do partial specialization! */
{
public:
    explicit GsNodeValuesProxyP( GsNodeValues* values = 0 ) : m_Values( values ) {}
    // use default copy constructor
    // use default destructor
    // use default assignment operator

    Ret operator[]( int Index ) const { return m_Values ? (Ret)m_Values->getPointer( Index ) : (Ret)0; }

	void operator=( GsNodeValues* values ) { m_Values = values; }

private:
    GsNodeValues* m_Values;
};

//
// GsNodeValues iterators implementation guts
//

class GsNodeValues::DtIter : public std::iterator<std::bidirectional_iterator_tag, DT_VALUE*>
{
public:
    DtIter() : m_GsNode( 0 ), m_Ind( -1 ) {} // singular iterator
    explicit DtIter( GsNodeValues& GsNode, int index ) : m_GsNode( &GsNode ), m_Ind( index ) {}
    // use default copy constructor
    // use default destructor
    // use default assignment operator

    DT_VALUE& operator*() { return *(m_GsNode->lookup( m_Ind )); }
    DT_VALUE* operator->() { return m_GsNode->lookup( m_Ind ); }

    DtIter& operator++() { ++m_Ind; return *this; }
    DtIter operator++(int) { DtIter tmp( *this ); ++m_Ind; return tmp; }

    DtIter& operator--() { --m_Ind; return *this; }
    DtIter operator--(int) { DtIter tmp( *this ); --m_Ind; return tmp; }

    bool operator==( DtIter const& rhs ) const
	{
		return m_Ind == rhs.m_Ind;
	}

    bool operator!=( DtIter const& rhs ) const { return !(*this == rhs);}

private:
    GsNodeValues* m_GsNode;
    int m_Ind;
};

GsNodeValues::DtIter GsNodeValues::begin()
{
    return DtIter( *this, 0 );
}

GsNodeValues::DtIter GsNodeValues::end()
{
    return DtIter( *this, size() );
}

template<class T_>
struct gs_node_values_extract
{
	T_ operator()( GsNodeValues* values, int i ) const { return (T_) values->getPointer( i ); }
};

template<>
struct gs_node_values_extract<double>
{
	double operator()( GsNodeValues* values, int i ) const { return values->getNumber( i ); }
};

// helper class to deal with lack of proper templated constructors
template<class T_>
struct gs_node_values_array
{
    explicit gs_node_values_array( GsNodeValues* values ) : m_values( values->size() )
	{
		int const sz = values->size();
		gs_node_values_extract<T_> F;
		for( int i = 0; i < sz; ++i )
			m_values[i] = F( values, i );
	}
    // use default destructor

	T_ const* get() const { return &(m_values[0]); }
    T_*       get()       { return &(m_values[0]); }

	T_ const& operator[]( int i ) const { return m_values[i]; }
	T_&       operator[]( int i )       { return m_values[i]; }

    int size() const { return m_values.size(); }

private:
	CC_STL_VECTOR(T_) m_values;
};

#endif
