/****************************************************************
**
**	gscore/GsTimeSeries.h	- GsTimeSeries class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsTimeSeries.h,v 1.25 2012/07/19 13:24:24 e19351 Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSTIMESERIES_H )
#define IN_GSDATE_GSTIMESERIES_H

#include <vector>
#include <ccstl.h>
#include <gscore/base.h>
#include <gscore/types.h>
#include <gscore/GsDate.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsDateVector.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE(Gs)


template <class T>
class GsTimeSeriesNode
{
public:
	typedef T        value_type;
	typedef T&       reference;
	typedef const T& const_reference;

	GsTimeSeriesNode() : m_Date(), m_Value(T()) {}
	GsTimeSeriesNode(const GsDate& Date, const T& Value = T()) : m_Date(Date), m_Value(Value) {}
	GsTimeSeriesNode(const GsTimeSeriesNode<T>& Rhs) 
    { 
		m_Date = Rhs.m_Date; 
		m_Value = Rhs.m_Value; 
	}

	~GsTimeSeriesNode();

	GsTimeSeriesNode<T>& operator=(const GsTimeSeriesNode<T>& Rhs) 
    {
		if (this != &Rhs) 
		{
			m_Date = Rhs.m_Date; 
			m_Value = Rhs.m_Value;
		}	
		return *this;
	}

	bool operator==(const GsTimeSeriesNode<T>& Rhs) const { return m_Date == Rhs.m_Date; }
	bool operator<(const GsTimeSeriesNode<T>& Rhs) const { return m_Date < Rhs.m_Date; }

	inline const GsDate& date() const { return m_Date; }

	inline reference value()  { return m_Value; }
	inline const_reference value() const { return m_Value; }

private:
	GsDate  	m_Date;
	value_type  m_Value;
};

 
/*
**  
*/

template <class T>
class GsTimeSeries
{
public:
	typedef GsTimeSeriesNode< T >		        node_type;
	typedef typename node_type::value_type		value_type;

	typedef CC_STL_VECTOR(node_type)	        vec_type;
	typedef typename vec_type::iterator	        iterator;
	typedef typename vec_type::const_iterator	const_iterator;
	typedef typename vec_type::size_type		size_type;

	static const char* typeName(); // { return GsTimeSeriesType<T>((T*)0 ); }
	GsString toString() const;

	typedef GsDtGeneric<GsTimeSeries< T> > GsDtType;
	
//	FIX-We might like to use:
//		GsTimeSeries( size_type N = 0 );
//	but, MSVC5.x does not seem to like to generate all of the template funcs.

	GsTimeSeries( size_t N = 0 );
	GsTimeSeries( const GsDateVector& Dates );
	GsTimeSeries( const GsTimeSeries<T>& Rhs );
	~GsTimeSeries();

	GsTimeSeries<T>& operator=( GsTimeSeries<T>& Rhs )
    {
		if( this != &Rhs )
			m_Data = Rhs.m_Data;
		return *this;
	}

	size_type size() const { return m_Data.size(); }
	size_type capacity() const { return m_Data.capacity(); }
	void reserve( size_type N ) { m_Data.reserve( N ); }

	iterator begin() { return m_Data.begin(); }
	const_iterator begin() const { return m_Data.begin(); }

	iterator end() { return m_Data.end(); }
	const_iterator end() const { return m_Data.end(); }

	void clear() { m_Data.clear(); }

	// Maybe Range check
#ifndef GS_RANGE_CHECK
	node_type& operator[](size_type Index) { return at( Index ); }
	const node_type& operator[](size_type Index) const { return at( Index ); }
#else
	node_type& operator[](size_type Index) { return m_Data[Index]; }
	const node_type& operator[](size_type Index) const { return m_Data[Index]; }
#endif

	node_type& operator()(size_type Index) { return (*this)[ Index ]; }
	const node_type& operator()(size_type Index) const { return (*this)[Index]; }

	// Throw exceptions
	node_type& atErr( const GsDate& Date ) const;
	// FIX-should be overloaded atErr(), but MSVC5 gets confused
	node_type& atErrIndex( size_type Index ) const;

	//Always Range check

	node_type& at( size_type Index )
    {
		if( Index < size() )
			return m_Data[Index];
		else
			return atErrIndex( Index );
	}
	
	const node_type& at(size_type Index) const 
    {
		if( Index < size() )
			return m_Data[Index];
		else
			return atErrIndex( Index );
	}

	node_type& at(const GsDate& Date);
	const node_type& at(const GsDate& Date) const;

	node_type& front() { return m_Data.front(); }
	const node_type& front() const { return m_Data.front(); }
	
	node_type& back() { return m_Data.back(); }
	const node_type& back() const { return m_Data.back(); }

	// Duplicate date is treated as 'replace value at date'.
	void insert( const GsDate& Date, const typename GsTimeSeries<T>::value_type& Value );
	iterator remove( iterator& Start, iterator& End ) { return m_Data.erase( Start, End ); }

	void dates( const GsDateVector& Dates );
	GsDateVector dates() const;

private:
	vec_type	m_Data;
};

CC_END_NAMESPACE

#endif

