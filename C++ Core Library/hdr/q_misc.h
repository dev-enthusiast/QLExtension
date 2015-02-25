/* $Header: /home/cvs/src/quant/src/q_misc.h,v 1.7 2000/03/23 12:57:29 goodfj Exp $ */
/****************************************************************
**
**	Q_MISC.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_Q_MISC_H )
#define IN_Q_MISC_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QBASIC double Q_gcount(
	Q_VECTOR	*Vector,  // vector of values
	double		value	  // arbitrary 
	);

EXPORT_C_QBASIC double Q_SumSub (
	Q_VECTOR       *Range,  // RANGE
	int            Start,   // Start-o-Range
	int            Finish   // End-O-Range
	);
	
EXPORT_C_QBASIC double Q_ProbaNoKOBrownianBridge(
	Q_UP_DOWN	updown,             // direction of knockout     
	double		vol2T,              // variance
	double		x,                  // log( Price[i] / Price[i-1] )
	double		y                   // log( KO       / Price[i-1] )
);

#endif
