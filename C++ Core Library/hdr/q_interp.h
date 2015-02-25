/****************************************************************
**
**	Q_INTERP.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/quant/src/q_interp.h,v 1.10 2000/03/06 15:25:34 goodfj Exp $
**
****************************************************************/

#if !defined( IN_Q_INTERP_H )
#define IN_Q_INTERP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
	
EXPORT_C_QBASIC double Q_LinearInterp(
	double   time,       // time in the future
	double   startval,   // current value
	Q_VECTOR *times,     // vector of times in the future
	Q_VECTOR *vec        // vector of values associated with these times
);

EXPORT_C_QBASIC double	Q_Interpolate(	DATE StartDate, double StartVal, DATE EndDate, double EndVal, DATE FindDate );

#endif
