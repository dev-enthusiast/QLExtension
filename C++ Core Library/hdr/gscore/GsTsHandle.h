/****************************************************************
**
**	gscore/GsTsHandle	- class GsTshandle.h
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsTsHandle.h,v 1.16 2011/05/17 16:10:58 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSTSHANDLE_H )
#define IN_GSDATE_GSTSHANDLE_H


#include	<gscore/GsRefCount.h>
#include	<gscore/GsHandle.h>
#include	<gscore/GsString.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	This class let you wrap any non ref counting class, and make it into
**	one, for use with the GsTimeSeries class.  Use this class when the
**	timeseries you want to create is a timeseries of objects which are
**	not cheap to copy.
**
**	e.g. DO NOT USE
**		GsTimeSeries < GsVector >
**	USE INSTEAD
**		GsTimeSeries < GsTsHandle < GsVector > >
**
**	You then use this as if you had defined a GsTimeSeries< GsVector* >,
**	only you do not have to worry about the memory management.
*/


template <class _T> class GsTsHandle;

template <class _T>
class GsTsRefCount : public GsRefCount
{
public:
	typedef _T* pointer_type;
	typedef const _T& const_reference_type;

	GsTsRefCount() : m_Ptr(0) {}
	GsTsRefCount(pointer_type Ptr) : m_Ptr( Ptr ) {}
	GsTsRefCount(const_reference_type Ref) : m_Ptr( new _T( Ref ) ) {}
	~GsTsRefCount() { if ( m_Ptr ) delete m_Ptr;}

  	inline void incCount() { GsRefCount::incCount(); }
  	inline void decCount() { GsRefCount::decCount(); }

	inline pointer_type getPtr() const { return m_Ptr; }
private:
	pointer_type 
			m_Ptr;
};



template <class _T>
class GsTsHandle : GsHandleShared < GsTsRefCount < _T > >
{
public:

	typedef GsTsRefCount< _T > refcount_type;
	typedef typename GsTsRefCount< _T >::pointer_type pointer_type;
	typedef typename GsTsRefCount< _T >::const_reference_type const_reference_type;

	GsTsHandle() : GsHandleShared< refcount_type >( 0 ) {}
	GsTsHandle( const GsTsHandle< _T >& Rhs ) : GsHandleShared< refcount_type >( Rhs ) {}
	GsTsHandle( pointer_type Rhs ) : GsHandleShared< refcount_type >( new refcount_type(Rhs) ) {}
	GsTsHandle( refcount_type Rhs ) : GsHandleShared< refcount_type >( new refcount_type(Rhs) ) {}


	GsTsHandle< _T >& operator=( const GsTsHandle< _T >& Rhs )
	{
		GsHandleShared< refcount_type >::operator=( Rhs );
		return *this;
	}

	GsTsHandle< _T >& operator=( pointer_type Rhs )
	{
		GsHandleShared< refcount_type >::operator=( GsTsHandle< _T >( Rhs ) );
		return *this;
	}

	GsTsHandle< _T >& operator=( const_reference_type Rhs )
	{
		GsHandleShared< refcount_type >::operator=( GsTsHandle< _T >( Rhs ) );
		return *this;
	}

	pointer_type operator->() const { return this->m_rep->getPtr(); }
	operator pointer_type()   const	{ return this->m_rep->getPtr(); }
	pointer_type getPtr() 	  const	{ return this->m_rep->getPtr(); }

	inline static const char* typeName() { return _T::typeName(); }

	GsString toString() const;
};

CC_END_NAMESPACE

#endif 

