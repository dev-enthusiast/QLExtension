/* $Header: /home/cvs/src/supermkt/src/supermkt.h,v 1.7 1998/10/09 22:27:23 procmon Exp $ */
/****************************************************************
**
**	SUPERMKT.H	
**	
**	Export header for supermkt functions needed by other projects.
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_SUPERMKT_H )
#define	IN_SUPERMKT_H


#include <meanrev.h>
#include <gob.h>


// Exports

DLLEXPORT int
    SupermktValueFuncVolSurfaceObj( GOB_VALUE_FUNC_ARGS ),
    NBasketValueFuncNearbyObj(      GOB_VALUE_FUNC_ARGS );


// Local exports

DT_GCURVE *
    SuperMktGenDailyCovMatrix
    (
       int      NumSides,
       DT_CURVE **SampCurvesOpen,
       DT_CURVE **CovarianceCurves
    );

int
    SuperMktGenLongShortDailyCurves
    (
        double     ExpDate,
        DT_ARRAY   *BasketWeights,
        DT_CURVE   **DataCurvesBySide,
        DT_CURVE   **WeightCurvesBySide,
        double     Today,
        double     VolAdjL,
        double     VolAdjS,
        double     RhoAdj,
        DT_GCURVE  *CovMatrixDaily,
        DT_CURVE   **LVolDailyCurve,
        DT_CURVE   **SVolDailyCurve,
        DT_CURVE   **RhoDailyCurve
    );

DT_MATRIX *
    SuperMktGenCovMatrixAvg
    (
        int        NumSides,
        double     *CovarianceAvgNums
    );

		 
		 
#endif






