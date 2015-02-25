/****************************************************************
**
**	GSTYPEDVECTOR.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsTypedVector.h,v 1.2 2011/05/16 20:06:08 khodod Exp $
**
****************************************************************/

#ifndef IN_GSBASE_GSTYPEDVECTOR_H
#define IN_GSBASE_GSTYPEDVECTOR_H

#include <gscore/base.h>
#include <stddef.h>
#include <iterator>
#include <vector>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

// Note that this is only a convenience wrapper class
// it makes no guarantees about lifetimes you have to manage that
// yourself.

template<class T>
class GsTypedVector;

template<class T, class Ref, class Ptr>
struct typed_vector_iterator
{
	typedef T value_type;
	typedef Ref reference;
	typedef Ptr pointer;

	typedef typed_vector_iterator<T, Ref, Ptr> Self;

	typedef CC_NS(std,bidirectional_iterator_tag) iterator_category;
	typedef ptrdiff_t difference_type;
		
	typed_vector_iterator( GsTypedVector<T>& vec, int pos ) : m_vec( vec ), m_pos( pos ) {}

	void operator++() { ++m_pos; }
	Self operator++(int) { Self tmp = *this; ++m_pos; return tmp; }

	void operator--() { --m_pos; }
	Self operator--(int) { Self tmp = *this; --m_pos; return tmp; }

	inline reference operator*() const;

	bool operator==( Self const& it ) { return m_pos == it.m_pos; }

private:
	GsTypedVector<T>& m_vec;
	int m_pos;
};

template<class T>
class EXPORT_CLASS_GSBASE GsTypedVector
{
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typed_vector_iterator<value_type,reference,pointer> iterator;

	virtual size_t size() const = 0;

	virtual reference elem( size_t index ) const = 0;

	reference operator[]( size_t index ) const { return elem( index ); }

	iterator begin() { return iterator( *this, 0 ); }
	iterator end() { return iterator( *this, size() ); }

	virtual ~GsTypedVector()
	{}

protected:
	GsTypedVector()
	{}
};

template<class T, class Ref, class Ptr>
		inline typename typed_vector_iterator<T,Ref,Ptr>::reference typed_vector_iterator<T,Ref,Ptr>::operator*() const
{
	return m_vec.elem( m_pos );
}

template<class T>
class GsTypedStlVector : public GsTypedVector<T>
{
public:
	typedef CC_STL_VECTOR(T) rep_t;

	GsTypedStlVector( rep_t& rep ) : m_rep( rep )
	{}
	~GsTypedStlVector()
	{}

	virtual size_t size() const { return m_rep.size(); }

	virtual typename GsTypedVector<T>::reference elem( size_t index ) const { return m_rep[ index ]; }

protected:
	rep_t& m_rep;
};

CC_END_NAMESPACE

#endif 
