/* $Header: /home/cvs/src/quant/src/q_avgfut.h,v 1.6 2000/03/03 16:06:48 goodfj Exp $ */
/****************************************************************
**
**	Q_AVGFUT.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_AVGFUT_H )
#define IN_Q_AVGFUT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_DATATYPE_H )
#include <datatype.h>
#endif
#if !defined( IN_DATE_H )
#include <date.h>
#endif


EXPORT_C_QAVG double Q_AvgFwdFut(
	DT_CURVE	*Fut,			// futures curve
	DATE		AvgStartDate,	// start date
	DATE		AvgEndDate		// end date
	);

#endif

