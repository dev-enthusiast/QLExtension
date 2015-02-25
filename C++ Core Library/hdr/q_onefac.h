/* $Header: /home/cvs/src/supermkt/src/q_onefac.h,v 1.8 2001/01/26 12:33:05 goodfj Exp $ */
/****************************************************************
**
**	Q_ONEFAC.H	
**	
**	Export header for the one-factor model functions.
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_Q_ONEFAC_H )
#define IN_Q_ONEFAC_H

#include <datatype.h>

// Exports

DLLEXPORT int
Q_CovarianceOneFactor
(
    DT_CURVE     *SampCurveX,
    DT_CURVE     *SampCurveY,
    DT_CURVE     *VolCurveXD,
    DT_CURVE     *VolCurveYD,
    DT_CURVE     *VolCurveXY,
    DT_CURVE     *WeightsFirst,
    DT_CURVE     *PricesFirst,
    DT_CURVE     *WeightsSecond,
    DT_CURVE     *PricesSecond,
    DATE         ExpDate,
    DATE         Today,
    int			 WalkHalfMatrix,
    void         **RetValPtr,
    DT_DATA_TYPE RetValType
);

DLLEXPORT double
Q_OneFactorCovariance
(
    DT_CURVE *Vols,
    double   D1,
    double   D2
);

#endif
