/****************************************************************
**
**	FXHandle.h	- templated handle classes for arbitrary classes
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_FXHANDLE_H )
#define IN_FXHANDLE_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template< class X > class FXHandleReadOnly : public GsHandleReadOnly< X >
{
	public:
		FXHandleReadOnly( const X* rep ) : GsHandleReadOnly< X >( rep ) {};
		FXHandleReadOnly( const FXHandleReadOnly< X >& x ) : GsHandleReadOnly< X >( x ) {}
		~FXHandleReadOnly() {}

		FXHandleReadOnly< X >& operator=( const FXHandleReadOnly& x )
		{
			GsHandleReadOnly< X >::operator=( x );
			return *this;
		}
		FXHandleReadOnly< X >& operator=( const X* Rep )
		{
			GsHandleReadOnly< X >::operator=( Rep );
			return *this;
		}
		const X*	operator->() const { return this->m_rep; }
		operator const X*() const{ return this->m_rep; }
};

CC_END_NAMESPACE

#endif

