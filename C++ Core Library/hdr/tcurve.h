/* $Header: /home/cvs/src/datatype/src/tcurve.h,v 1.2 2005/11/29 17:50:31 e45019 Exp $ */
/****************************************************************
**
**	TCURVE.H - Miscellaneous time curve support functions.
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#ifndef IN_TCURVE_H
#define IN_TCURVE_H

#ifndef IN_HASH_H
#include <hash.h>
#endif
#ifndef IN_DATE_H
#include <date.h>
#endif
#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif

// number of seconds in an hour; default sample period for TCurves.
#define DEFAULT_STEP 3600

DLLEXPORT int PushTimeZone( char *pszTimeZone );
DLLEXPORT int PopTimeZone();

DLLEXPORT DT_TCURVE *FrequencyValsToTCurve(
    HASH        *FreqDefs,
    HASH        *FreqValues,
    DATE        StartDate,
    DATE        EndDate,
    double        SamplePer,
    DT_CURVE    *SamplingCurve,
    char        *pszTimeZone
);

DLLEXPORT DT_TCURVE *FrequencyDefToTCurve(
    char		*Key,
	HASH		*FreqDefs,
	DATE		StartDate,
    DATE		EndDate,
    double		InitValue,
    double		SamplePer,
    DT_CURVE	*SamplingCurve,
    DT_CURVE	*ExcludeDates,
    char        *pszTimeZone
);

					 
#endif // IN_TCURVE_H
