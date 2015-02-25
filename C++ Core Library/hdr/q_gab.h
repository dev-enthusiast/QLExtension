/* $Header: /home/cvs/src/supermkt/src/q_gab.h,v 1.9 2001/01/26 12:33:04 goodfj Exp $ */
/****************************************************************
**
**	Q_GAB.H	
**	
**	Export header for the Gabillon two-factor model functions.
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_Q_GAB_H )
#define IN_Q_GAB_H

#include <datatype.h>

// Exports

DLLEXPORT int
Q_MRCovarianceTwoDim
(
    double       Today,
    double       ExpDate,
    DT_CURVE     *InstVolsOfSpotFirst,
    DT_CURVE     *InstVolsOfSpotSecond,
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
Q_MRCovarianceSingleCommod
(
    double       Today,
    double       ExpDate,
    DT_CURVE	 *BetaByTime,
    double       CorrLS,
    double       LongVol,
    DT_CURVE     *InstVolsOfSpot,
    DT_CURVE     *SampCurve,
    DT_ARRAY     *VolEndDatesOpen,
    DT_CURVE     *Weights,
    DT_CURVE     *Prices,
    void         **RetValPtr,
    DT_DATA_TYPE RetValType
);

DLLEXPORT int
Q_MRGenerateSVParams
(
    double   	Today,
    double   	ExpDate,
    DT_CURVE 	*InstVolsOfSpot,
    DT_CURVE 	*SampCurve,
    DT_CURVE	 *BetaByTime,
    DT_CURVE 	**SV1Curve,
    DT_CURVE 	**SV2Curve,
    DT_CURVE 	**SV3Curve
);

DLLEXPORT DT_CURVE *
Q_MRInstCorrsShortShort
(
    double   Beta1,
    double   Beta2,
    double   LongVol1,
    double   LongVol2,
    double   MRCorr1,
    double   MRCorr2,
    DT_CURVE *InstVolsOfSpotFirst,
    DT_CURVE *InstVolsOfSpotSecond,
    DT_CURVE *CorrSSTerm,
    double   CorrSL,
    double   CorrLS,
    double   CorrLL,
    double   Today,
    double   ExpDate
);

DLLEXPORT int
Q_GabillonCovariances
(
    DT_CURVE *SVol1,
    double   LVol1,
    double   Beta1,
    DT_CURVE *SVol2,
    double   LVol2,
    double   Beta2,
    DT_CURVE *CorrSS,
    double   CorrSL,
    double   CorrLS,
    double   CorrLL,
    double   D1,
    double   D2,
    double   *CovSS,
    double   *CovSL,
    double   *CovLS,
    double   *CovLL
);

#endif

