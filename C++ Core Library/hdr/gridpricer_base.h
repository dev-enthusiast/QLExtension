/* $Header: /home/cvs/src/gridpricer/src/gridpricer_base.h,v 1.1 2001/04/04 19:39:14 terraa Exp $ */
/****************************************************************
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_GRIDPRICER_BASE_H )
#define IN_GRIDPRICER_BASE_H

#if defined( BUILDING_GRIDPRICER )

#define EXPORT_C_GRIDPRICER		EXPORT_C_EXPORT
#define EXPORT_CPP_GRIDPRICER	EXPORT_CPP_EXPORT
#define EXPORT_CLASS_GRIDPRICER	EXPORT_CLASS_EXPORT

#else

#define EXPORT_C_GRIDPRICER		EXPORT_C_IMPORT
#define EXPORT_CPP_GRIDPRICER	EXPORT_CPP_IMPORT
#define EXPORT_CLASS_GRIDPRICER	EXPORT_CLASS_IMPORT

#endif

#endif 
