/****************************************************************
**
**	gsquant/CurveUtil.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/CurveUtil.h,v 1.2 1999/08/17 20:23:59 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_CURVEUTIL_H )
#define IN_GSQUANT_CURVEUTIL_H

#include	<gsquant/base.h>
#include	<gscore/types.h>
#include	<gscore/GsCTimeCurve.h>
#include	<gscore/GsTimeSeries.h>
#include	<dtcore.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsCTimeCurve
		GsCurveFromTimeSeries(	const GsDoubleTimeSeries& Ts, GsInterpolation InterpMethod, GsDate BaseDate );

EXPORT_CPP_GSQUANT double
		GsCurveAtDate(	const GsCTimeCurve& Ts, GsDate  When ),
		GsCurveAt(		const GsCTimeCurve& Ts, GsCTime When );

CC_END_NAMESPACE
#endif

