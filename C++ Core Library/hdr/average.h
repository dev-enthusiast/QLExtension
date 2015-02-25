/* $Header: /home/cvs/src/average/src/average.h,v 1.30 2000/04/24 11:08:09 singhki Exp $ */
/****************************************************************
**
**	AVERAGE.H
**
**	$Revision: 1.30 $
**
****************************************************************/

#if !defined( IN_AVERAGE_H )
#define IN_AVERAGE_H

#include	<secdb.h>
#include	<editinfo.h>
#include	<outform.h>
#include	<date.h>

#define DAILY_BUSINESS			"Daily/Business Days"

//	AVGOOPS.C

int
	AvgValueFuncFirstDate			( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncFirstSampleDate		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncLastSampleDate		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncNextSampleDate		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncAverageValue		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncRawAverageValue		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncWhereAverageValue	( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncNetSamplingWeight	( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncWeightedHistory		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncFwdSampleCurve		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncForwardCurve		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncSamplingCurveStored	( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncDataCurve			( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncPremiumAmount		( GOB_VALUE_FUNC_ARGS ),
	AvgValueFuncAvgTerm				( GOB_VALUE_FUNC_ARGS );

DLLEXPORT int 	AvgValueFuncExpiration			( GOB_VALUE_FUNC_ARGS );
DLLEXPORT int	AvgValueFuncSettleDate			( GOB_VALUE_FUNC_ARGS );
DLLEXPORT int	AvgValueFuncAvgVol				( GOB_VALUE_FUNC_ARGS );

//	AVGOPS.C
int TsdbExpressionDescription(
	char		*Expression,
	char		*Description,
	int			DescSize
),

	CheckHolsNWeekends(
	DATE				Date,
	SDB_OBJECT			*SecPtr,
	SDB_M_DATA			*MsgData,
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo
);

DLLEXPORT int AvgMakeAverageFwd(
	char	*Denominated,
	char	*QuantityUnit,
	char	*ExpLocation,
	DATE	FirstDate,
	DATE	LastDate,
	DATE	ExpirationDate,
	DATE	SettleDate,
	char	*AvgFreq,
	char	*DataSource,
	double	PremiumPercent,
	double	PremiumAmount,
	DT_CURVE
			*SamplingCurve,
	char	*AverageFwdName
);

#endif

