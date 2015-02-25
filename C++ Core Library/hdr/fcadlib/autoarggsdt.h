/****************************************************************
**
**	AUTOARGGSDT.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/autoarggsdt.h,v 1.2 2001/01/05 14:52:10 vengrd Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_AUTOARGGSDT_H )
#define IN_FCADLIB_AUTOARGGSDT_H

#include <fcadlib/base.h>
#include <fcadlib/autoarg.h>


CC_BEGIN_NAMESPACE( Gs )


// Specialization for GsDt

AutoArgRef<GsDt>::AutoArgRef( const AutoArgRef< GsDt > &Rhs )
:
m_delete( TRUE ),
m_ptr( Rhs.m_cPtr ? Rhs.m_cPtr->Copy() : Rhs.m_ptr )
{
	m_cPtr = m_ptr;
}


CC_END_NAMESPACE

#endif 
