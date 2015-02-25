/****************************************************************
**
**	fcadlib/autoarg.h	- AutoArg template class
**
**	Similar to auto_ptr, but used for AdLib args
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/autoarg.h,v 1.16 2001/11/27 21:41:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_AUTOARG_H )
#define IN_FCADLIB_AUTOARG_H

#include <gscore/GsDate.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	The AutoArg class holds a pointer or const pointer to a value.
**	The pointer will be deleted if m_delete is True.
**	Use operator*() to get a const T &
*/

template<class T>
class AutoArgRef
{
private:
	GsBool		m_delete;	// Delete m_ptr when done?
	T			*m_ptr;
	const T	*m_cPtr;	
public:
	AutoArgRef()
	:	m_delete( FALSE ),
		m_ptr( 0 ),
		m_cPtr( 0 )
	{
	}
	AutoArgRef( T *Ptr, GsBool Delete = TRUE )
	:	m_delete( Delete ),
		m_ptr( Ptr ),
		m_cPtr( Ptr )
	{
	}
	AutoArgRef( const AutoArgRef< T > &Rhs )
	:
		m_delete( TRUE ),
		m_ptr( new T( *Rhs.m_cPtr ) )
	{
		m_cPtr = m_ptr;		
	}
	~AutoArgRef()
	{
		if( m_delete )
			delete m_ptr;
	}
	AutoArgRef &operator=( const AutoArgRef &Rhs )
	{
		m_ptr = new T( *Rhs.m_cPtr );
		m_cPtr = m_ptr;
		m_delete = TRUE;

		return *this;
	}
	void operator=( const T &Rhs )
	{
		if( m_delete )
			delete m_ptr;
		m_ptr = new T( Rhs );
		m_cPtr = m_ptr;
		m_delete = TRUE;
	}
	void operator=( T *Rhs )
	{
		if( m_delete )
			delete m_ptr;
		m_ptr    = Rhs;
		m_cPtr   = m_ptr;
		m_delete = TRUE;
	}
#if 0
	void operator=( const T *Rhs )
	{
		if( m_delete )
			delete m_ptr;
		m_ptr    = NULL;
		m_cPtr   = Rhs;
		m_delete = FALSE;
	}
#endif
	bool isValid() { return m_cPtr != NULL; }

	const T &operator*()  { return *m_cPtr; }
//	operator const T &() const { return *m_cPtr; }
	const T *cPtr() { return m_cPtr; }
	T		 *ptr()  { return m_ptr; }
	void cPtrSet( const T *cPtr )
	{
		m_cPtr   = cPtr;
		m_ptr    = 0;
		m_delete = FALSE;
	}
	void ptrSetCopy( const T& Val )
	{
		m_ptr    = new T( Val );
		m_cPtr   = m_ptr;
		m_delete = TRUE;
	}
	void ptrSet( T *Ptr, GsBool Delete = TRUE )
	{
		m_cPtr   = Ptr;
		m_ptr    = Ptr;
		m_delete = Delete;
	}
	T &ptrGet()  // can't overload operator* to differ only by return type
	{
		return *m_ptr;
	}
	void deleteSet( GsBool Delete )
	{
		m_delete = Delete;
	}
};


CC_END_NAMESPACE

#endif

