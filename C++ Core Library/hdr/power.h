/* $Header: /home/cvs/src/power/src/power.h,v 1.38 2001/07/24 16:56:09 paints Exp $ */
/****************************************************************
**
**	POWER.H	- Power header file
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.38 $
**
****************************************************************/

#if !defined( IN_POWER_H )
#define IN_POWER_H

#ifndef IN_DTZDATA_H
#include    <dtzdata.h>
#endif
#ifndef IN_TCURVE_H
#include    <tcurve.h>
#endif
#ifndef IN_GOB_H
#include 	<gob.h>
#endif

#define 	DAILY_ALL 			"Daily/All Days"


//	Data Fix Access Macros

#define		SETTLE_METHOD_CASH		1
#define		SETTLE_METHOD_PHYSICAL	0
#define		QUANTITY_ID_SIZE 		2048
#define		EPSILON					1e-6


// in pwrops.c

DT_CURVE *DtCurveBinOp(
	DT_CURVE	*CurveA,
	DT_CURVE	*CurveB,
	int			Operation,
	int			Method
);

DT_TCURVE *DtCurveBinOpRT(
	DT_CURVE	*Curve,
	DT_TCURVE	*TCurve,
	int			Operation,
	int			Method
);

// in pwrnrby.c
typedef struct ChildrenOfFwdCurve
{
	DT_ZDATA
    	*LoadCurve;

    DT_TCURVE
        *FwdCurve;

    DT_CURVE
    	*SamplingCurve,
        *LoadCurveDaily;

    double
        	SamplePer;

    char
    	*LoadCurveType,
        *Key,
		*LocName,
        *SvcType;

} CHILDREN_OF_FWDCURVE;


int ValueFuncForwardCurveDailyGuts(
	GOB_VALUE_FUNC_ARGS,
    CHILDREN_OF_FWDCURVE *ChildDataPassed
);
int ShrValueFuncSpotPrice(
	GOB_VALUE_FUNC_ARGS
);

int ShrValueFuncOpenCurve(
	GOB_VALUE_FUNC_ARGS
);


// in pwrswap.c
int PowerValueFuncExtractComponent(		GOB_VALUE_FUNC_ARGS	);


#endif




