/****************************************************************
**
**	CORBA_NAMING.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/portable/corba_naming.h,v 1.3 2001/11/27 23:11:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECDBSESSION_CORBA_NAMING_H )
#define IN_SECDBSESSION_CORBA_NAMING_H

#ifdef CORBA_IMPL_IONA_ORBIX
#include <omg/CosNaming.hh>
#elif defined CORBA_IMPL_TAO
#include <CosNamingC.h>
#elif defined CORBA_IMPL_VISIBROKER
#include <CosNaming_c.hh>
#endif


#endif 
