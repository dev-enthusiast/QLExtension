/* $Header: /home/cvs/src/datatype/src/dttcurve.h,v 1.9 2008/10/24 15:34:02 zhangzho Exp $ */
/****************************************************************
**
**	TCURVE.H - Miscellaneous time curve support functions.
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#ifndef IN_DTTCURVE_H
#define IN_DTTCURVE_H

#ifndef IN_DATE_H
#include <date.h>
#endif
#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif

// number of seconds in an hour; default sample period for TCurves.
#define DEFAULT_STEP 3600


DLLEXPORT DT_TCURVE *CurveAppendCurve(
    DT_CURVE	*Curve1,
    DT_CURVE	*Curve2
);

DLLEXPORT DT_CURVE *MakeFrequencyTimeCurve(
	DT_CURVE	*DatesCurve,
	char		*StartTimeStr,
	char		*EndTimeStr,
    double		InitValue,
    double      SamplePeriod,
    int			EFA_Day,
    int			SkipDSTHE1
);

DLLEXPORT DT_CURVE	*TCurveDailyHours( 
    DT_TCURVE	*Curve,
    DT_CURVE	*SampleCurve,
	double		SamplePer,
    int			Method,
    int			ZapZeros,
    int			Normalize
);

DLLEXPORT DT_TCURVE	*TCurveSample(
    DT_TCURVE	*Curve,
	double		SamplePer,
    time_t      StartTime,
    time_t		EndTime,    
    int     	Method,
    double		ErrorSubsValue
);

DLLEXPORT DT_TCURVE *TCurveConcat(
	DT_TCURVE *, 
	DT_TCURVE *, 
	time_t, 
	time_t
);

DLLEXPORT time_t TCurveSamplePeriod(
	DT_TCURVE *
);

DLLEXPORT DT_TCURVE *TCurveFromCurve( 
	DT_CURVE *Curve, 
	int SamplePeriod
);

DLLEXPORT DT_CURVE  *CurveFromTCurve(
	DT_TCURVE *
);

DLLEXPORT DT_GCURVE	*TCurveByDay( 
    DT_TCURVE	*Curve,
    DT_CURVE	*SampleCurve,
	double		SamplePer,
    int			Method,
    int			ZapZeros,
	int			Normalize
);


DLLEXPORT DT_CURVE    *TCurveDailyMinMaxDiv2( 
    DT_TCURVE    *Curve,
    DT_CURVE     *SampleCurve,
    double       SamplePer,
    int          Method,
    int          ZapZeros
);


DLLEXPORT DT_DOUBLE	TCurvePeak( 
	DT_TCURVE	*TCurve
);	
													  

DLLEXPORT DT_TCURVE	*TCurveTrim( 
    DT_TCURVE	*TCurve,
	time_t		StartTime,
    time_t		EndTime
);

DLLEXPORT int TCurveInterpolate(
	DT_TCURVE	*SourceCurve,
	DT_VALUE	*TargetTimes,
	int			Method,
	DT_VALUE	*Ret
);

DLLEXPORT DT_TCURVE	*TCurveOperateSampled(
    DT_TCURVE	*Curve1,
    DT_TCURVE	*Curve2,
	double		SamplePer,
    time_t      StartTime,
    time_t		EndTime,    
    int			Operation,
    int     	Method,
    int			ZapZeros,
    double		ErrorSubsValue
);


DLLEXPORT DT_CURVE	*TCurveZap(
    DT_CURVE	*Crv,
	double		Value,
	double		Epsilon
);

DLLEXPORT DT_CURVE	*TCurve2Filter(
    DT_CURVE	*Crv1,
	double		Epsilon
);

DLLEXPORT DT_CURVE	*TCurveIntersection(
    DT_CURVE	*Crv1,
    DT_CURVE	*Crv2
);

DLLEXPORT DT_CURVE	*TCurveMissing(
    DT_CURVE	*Crv1,
    DT_CURVE	*Crv2
);

DLLEXPORT DT_CURVE	*CurveDeleteZeros( 
    DT_CURVE	*Curve,
	double		Epsilon
);

DLLEXPORT DT_CURVE	*CurveTrim( 
    DT_CURVE	*Curve,
	DATE		StartDate,
    DATE		EndDate
);

DLLEXPORT DT_TCURVE	*TCurveRandomize(
    DT_TCURVE	*Curve,
	double		Std,
    int			Seed
);

DLLEXPORT DT_CURVE	*TCurveZap(
    DT_CURVE	*TCrv,
	double		Value,
	double		Epsilon
);

DLLEXPORT DT_TCURVE *DateArraysToTCurve(
    DT_CURVE    *DatesCurve,
    DATE        StartDate,
    DATE        EndDate,
    DT_VALUE    *Data,
    double		SamplPer,
    int			NormalizeFlag
);

DLLEXPORT DT_CURVE	*TCurveFilterKnots(
	DT_CURVE	*SrcCrv,				//Source
	DT_CURVE	*FltCrv,				//Filter
	double		SampPer,
	int			InterpMethod,
	int			ErrorSubs,
	int			ZapZeros
);

					 
#endif // IN_DTTCURVE_H


