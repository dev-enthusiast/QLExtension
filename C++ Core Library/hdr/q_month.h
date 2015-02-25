/* $Header: /home/cvs/src/quant/src/q_month.h,v 1.4 2000/03/03 17:54:32 goodfj Exp $ */
/****************************************************************
**
**	Q_MONTH.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_Q_MONTH_H )
#define IN_Q_MONTH_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif

EXPORT_C_QBASIC DATE Q_AddCMonth(
	DATE	dat,  // any date
	int		num	  // number of months
	);
	
EXPORT_C_QBASIC DATE Q_AddMonth(
	DATE dat,  // any date
	int num	   // number of months
	);

#endif

