/* $Header $ */
/****************************************************************
**
**	SUPERVF.H	
**	
**	Header for supermkt shared value functions
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_SUPERVF_H )
#define	IN_SUPERVF_H

#include 	<gob.h>

int
		SuperMktValueFuncHistoricCurve(				GOB_VALUE_FUNC_ARGS ),
        SuperMktValueFuncOpenCurve(					GOB_VALUE_FUNC_ARGS );

DLLEXPORT int
        SupermktValueFuncQuantityCurveDaily(		GOB_VALUE_FUNC_ARGS ),
        SupermktValueFuncQuantityCurveCount(    	GOB_VALUE_FUNC_ARGS ),
        SupermktValueFuncQuantityCurvePeak(     	GOB_VALUE_FUNC_ARGS ),
        SupermktValueFuncQuantityCurveSum(     		GOB_VALUE_FUNC_ARGS ),
        SupermktValueFuncQuantityCurvesByTypeDaily( GOB_VALUE_FUNC_ARGS ),
        SupermktValueFuncQuantityCurvesByTypeSum(   GOB_VALUE_FUNC_ARGS );

		 
		 
#endif
