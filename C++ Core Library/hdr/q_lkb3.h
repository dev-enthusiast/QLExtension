/* $Header: /home/cvs/src/quant/src/q_lkb3.h,v 1.8 2000/03/02 14:46:26 goodfj Exp $ */
/****************************************************************
**
**	Q_LKB3.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_Q_LKB3_H )
#define IN_Q_LKB3_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QUANT double Q_LookBackFix (
	Q_OPT_TYPE       callPut,           // option type
	double           spot,              // current spot price
	double           strike,            // strike price on extremum
	double           vol,               // volatility 
	double           time,              // time to expiration
	double           ccRd,              // domestic rate
	double           ccRf               // foreign rate
	);

EXPORT_C_QUANT void   Q_LookBack3 (
	Q_OPT_TYPE     callPut,		 // option type
	Q_OPT_STYLE    euroAmer,	 // option style
	double         spot,		 // current spot
	double         currMinMax,	 // current min/max
	double         stkOffset,	 // strike offset
	Q_BEST_WORST   stkType,		 // strike type	(best or worst)
	double         stkDetTime,	 // time to strike setting
	double         timeToExp,	 // time to expiration
	double         shortVol,	 // short volatility
	double         longVol,		 // long volatility
	double         shortDRate,	 // short domestic rate
	double         longDRate,	 // long domestic rate
	double         shortFRate,	 // short foreign rate
	double         longFRate,	 // long foreign rate
	int            fineSteps,	 // number of short timesteps
	int            maxIter,		 // number of paths
	LBRESULTS      *vars		 // structure containing premium, delta and regression data
	);

#endif
