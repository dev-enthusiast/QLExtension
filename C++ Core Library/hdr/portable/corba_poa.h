/****************************************************************
**
**	CORBA_POA.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/portable/corba_poa.h,v 1.7 2001/11/27 23:11:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECDBSESSION_CORBA_POA_H )
#define IN_SECDBSESSION_CORBA_POA_H

#ifdef CORBA_IMPL_IONA_ORBIX
#include <omg/PortableServer.hh>
#elif defined CORBA_IMPL_TAO
#include <PortableServer/PortableServerC.h>
#include <PortableServer/Servant_Base.h>

#include <ORB.h>
#elif defined CORBA_IMPL_VISIBROKER
#include <poa_c.hh>
#include <poatypes.h>
#include <dynimpl.h>
#include <srequest.h>
#endif

#endif 
