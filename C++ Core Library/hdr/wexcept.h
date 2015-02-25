/****************************************************************
**
**	wexcept.h	- Window system master header file 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/window/src/wexcept.h,v 1.5 2011/05/16 22:11:32 khodod Exp $
**
****************************************************************/

#ifndef IN_WEXCEPT_H
#define IN_WEXCEPT_H

#include <exception>
#include <string>

CC_BEGIN_NAMESPACE( Gs )

struct EXPORT_CLASS_EXPORT wi_start_exception  : public CC_NS(std, exception)

{
	wi_start_exception( const CC_NS(std, string)& s ) : m_what( s ) { }
	virtual ~wi_start_exception() throw() {}
	virtual const char* what() const throw() { return m_what.c_str(); }
	CC_NS(std, string)	m_what;
};

CC_END_NAMESPACE

#endif

