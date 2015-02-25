/****************************************************************
**
**	gscore/types.h	- types needed by everything in gscore
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/types.h,v 1.25 2001/11/27 22:41:12 procmon Exp $
**
****************************************************************/

#ifndef IN_GSCORE_TYPES_H
#define IN_GSCORE_TYPES_H

#include	<port_int.h>
#include	<gscore/base.h>
#include    <string>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Forward declare lots of core types
*/

typedef double GsDouble;
typedef int GsInteger;

typedef CC_NS(std, string) GsString;
CC_USING( std::string );

class GsSymbol;
class GsVector;
class GsMatrix;
class GsTensor;
class GsPeriod;

class GsIntegerVector;
class GsStringVector;
class GsDoubleVector;
class GsDateVector;
class GsSymbolVector;
class GsUnsignedVector;

typedef UINT32 GsLocalTypeID;
class GsStreamOut;
class GsStreamIn;
class GsStreamMap;
class GsFactory;



/*
**	GsDate types & templates
*/

template <class _T> class GsTimeSeries;
template <class _T> class GsTimeSeriesNode;

class GsCTime;
class GsCTimeVector;
class GsDate;
class GsDateGen;
class GsDateVector;
class GsDayCountISDA;
class GsDayCount;
class GsHolidaySchedule;
class GsPTimeVector;
class GsRDate;
class GsSymDate;
class GsTermCalc;
class GsPTimeCalc;
class GsRDateCurve;

typedef GsDateGen GsCalendar;


/*
** Some common GsTimeSeries
*/

typedef GsTimeSeries< GsDouble >		GsDoubleTimeSeries;
typedef GsTimeSeries< GsInteger >		GsIntegerTimeSeries;
typedef GsTimeSeries< GsDate >			GsDateTimeSeries;
typedef GsTimeSeries< GsString >		GsStringTimeSeries;
typedef GsTimeSeries< GsSymDate >		GsSymDateTimeSeries;
typedef GsTimeSeries< GsDayCountISDA >	GsDayCountISDATimeSeries;
typedef GsTimeSeries< GsDayCount >		GsDayCountTimeSeries;



/*
**	What to do when passed a pointer
*/

enum GsCopyAction
{
	GS_COPY				= 10,	// Copy the value
	GS_NO_COPY			= 11,	// Don't copy, called function becomes owner
	GS_NO_COPY_NO_FREE	= 12	// Don't copy, but don't free when done with value.
								// Many APIs can't support this, and thus treat
								// GS_NO_COPY_NO_FREE as if it were GS_COPY
};


/*
**	Use this type to get uninitialized (or partially initialized) objects
*/

enum GsUninitialized
{
	GS_UNINIT
};


/*
**	Interpolation methods
*/

enum GsInterpolation
{
	GS_INTERPOLATE_LINEAR,
	GS_INTERPOLATE_LINEAR_EXTRAPOLATE,
	GS_INTERPOLATE_STEP
};


/*
**	Slang Return Codes
*/

enum GsSlangRetCode
{
	GS_SLANG_OK,
	GS_SLANG_ERROR,
	GS_SLANG_ABORT,
	GS_SLANG_TRANSACTION_ABORT,
	GS_SLANG_CONTINUE,
	GS_SLANG_BREAK
};


CC_END_NAMESPACE

#endif

