/****************************************************************
**
**	CORBA.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/portable/corba.h,v 1.6 2001/11/27 23:11:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECDBSESSION_CORBA_H )
#define IN_SECDBSESSION_CORBA_H

#if defined( CORBA_IMPL_TAO )
#define CORBA_ENV_DECL		, CORBA_Environment &ACE_TRY_ENV = TAO_default_environment()
#define CORBA_ENV_DEFN		, CORBA_Environment &
#define CORBA_ENV_DECL0		CORBA_Environment &ACE_TRY_ENV = TAO_default_environment()
#define CORBA_ENV_DEFN0		CORBA_Environment &
#else
#define CORBA_ENV_DECL
#define cORBA_ENV_DEFN
#define CORBA_ENV_DECL0
#define cORBA_ENV_DEFN0
#endif

#if defined(CORBA_IMPL_TAO)
#ifdef WIN32
#include <iostream>

#include <Managed_Types.h>
// PW adds an operator to const char * the tsm in order to print them properly
inline CC_NS( std, ostream ) &operator<<( CC_NS( std, ostream ) &os, TAO_String_Manager &tsm )
{
	os << (const char *)tsm;
	return os;
}
#endif
#endif

#endif 
