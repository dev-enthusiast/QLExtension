/* $Header: /home/cvs/src/exopt/src/spotkout.h,v 1.8 2001/01/09 23:53:04 abramjo Exp $ */
/****************************************************************
**
**	SPOTKOUT.H	Header for SPOTKOUT Object
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_SPOTKOUT_H )
#define IN_SPOTKOUT_H
// SPOTKOUT.C

DLLEXPORT int
		SpotValueFuncExerciseKO( 		GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncExpTransKO( 		GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncInvInOrOut( 		GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncInvUpOrDown(		GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncKnockout(			GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncRebateType(		GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncKnockinKO( 		GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncKnockoutKO(		GOB_VALUE_FUNC_ARGS );

DLLEXPORT int 	SpotPayKORebate(		SDB_OBJECT	*SecPtr,
										SDB_M_DATA	*MsgData,
										double		Rebate,
										char		*RebateType,
										double		SpotDate,
										char		*QuantityUnit,
										char		*Denominated,
										double		ExpSettleDate,
										double		Quantity,
										char		*SecTypeDenom,
										DT_STRUCTURE *SettlementInfo );

DT_CURVE *SQ_AdjustedRateCurve(
	DT_CURVE		*DenomRef,
	DT_CURVE		*InterpDates,	  	// Interpolating Dates
	DATE            StartDate,
	RDATE			SpotDays,
	char*			Holiday1,
	char*			Holiday2
	);

int	ExoptValueFuncSpotPriceVt(	GOB_VALUE_FUNC_ARGS );

#endif /* IN_SPOTKOUT_H */
