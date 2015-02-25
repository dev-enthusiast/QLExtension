/* $Header: /home/cvs/src/quant/src/q_lme.h,v 1.5 2000/03/03 17:54:31 goodfj Exp $ */
/****************************************************************
**
**	Q_LME.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_Q_LME_H )
#define IN_Q_LME_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_DATE_H )
#include <date.h>
#endif

EXPORT_C_QBASIC DATE Q_NextLMEDay2(
	DATE	ThisDate,  // Current date
	int		nMonths	   // number of months in the future
	);
	
EXPORT_C_QBASIC DATE Q_NextLMEDay(
	DATE	ThisDate  // current date
	);

EXPORT_C_QBASIC DATE Q_NextIMMDate(
	DATE	ThisDate  // current date
	);
	
#endif

