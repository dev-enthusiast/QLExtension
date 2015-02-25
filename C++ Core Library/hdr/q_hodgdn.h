/* $Header: /home/cvs/src/supermkt/src/q_hodgdn.h,v 1.6 2001/01/26 12:33:04 goodfj Exp $ */
/****************************************************************
**
**	Q_HODGDON.H	
**	
**	Export header for the Hodgdon two-factor model functions.
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_HODGDN_H )
#define IN_Q_HODGDN_H

#include <datatype.h>

// Exports

DLLEXPORT int
Q_HodgdonCovarianceMulti
(
    double       Today,
    double       ExpDate,
    DT_CURVE     *TermVolsOfSpotFirst,
    DT_CURVE     *TermVolsOfSpotSecond,
    double       BetaFirst,
    double       BetaSecond,
    double       LongVolFirst,
    double       LongVolSecond,
    DT_CURVE     *CorrSS,
    double       CorrSL,
    double       CorrLS,
    double       CorrLL,
    DT_CURVE     *FirstSampCurve,
    DT_CURVE     *SecondSampCurve,
    DT_ARRAY     *VolEndDatesFirst,
    DT_ARRAY     *VolEndDatesSecond,
    DT_CURVE     *WeightsFirst,
    DT_CURVE     *PricesFirst,
    DT_CURVE     *WeightsSecond,
    DT_CURVE     *PricesSecond,
    void         **RetValPtr,
    DT_DATA_TYPE RetValType
);

DLLEXPORT int
Q_HodgdonCovarianceSingle
(
    double       Today,
    double       ExpDate,
    DT_CURVE     *TermVolsOfSpot,
    double       Beta,
    double       LongVol,
    double       CorrLS,
    DT_CURVE     *SampCurve,
    DT_ARRAY     *VolEndDates,
    DT_CURVE     *Weights,
    DT_CURVE     *Prices,
    void         **RetValPtr,
    DT_DATA_TYPE RetValType
);

DLLEXPORT DT_CURVE *
Q_HodgdonGenerateTermVolCurve
(
    double   Today,
    double   VolEndDate,
    DT_CURVE *TermVolsOfSpot,
    double   Beta,
    double   LongVol,
    double   Rho
);

#endif

