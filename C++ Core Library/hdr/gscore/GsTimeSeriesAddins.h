/****************************************************************
**
**	gscore/GsTimeSeriesAddins.h	- GsTimeSeries Addin Functions
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsTimeSeriesAddins.h,v 1.2 2001/03/13 13:39:00 goodfj Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSTIMESERIESADDINS_H )
#define IN_GSDATE_GSTIMESERIESADDINS_H

#include <gscore/base.h>
#include <gscore/types.h>
#include <gscore/GsDate.h>
#include <gscore/GsTimeSeries.h>
#include <gscore/GsDoubleVector.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE(Gs)

EXPORT_CPP_GSDATE GsDoubleTimeSeries
		*GsDoubleTimeSeriesNew(const GsDateVector& Dates, const GsDoubleVector& Values);

EXPORT_CPP_GSDATE double
		GsDoubleTimeSeriesAtDate(const GsDoubleTimeSeries& ts, const GsDate& Date),
		GsDoubleTimeSeriesAtIndex(const GsDoubleTimeSeries& ts, int Index),
		GsDoubleTimeSeriesSum( const GsDoubleTimeSeries& TS ),
		GsAddTwo(double A, double B),
		GsAddThree(double A, double B, double C),
		GsSubTwo(double A, double B),
		GsMulTwo( double A, double B ),
		GsDivTwo( double A, double B );
		
EXPORT_CPP_GSDATE GsString
		TestDateToString(const GsDate& Date);

EXPORT_CPP_GSDATE GsDate
		TestDatePlusOne(const GsDate& Date);

CC_END_NAMESPACE

#endif

