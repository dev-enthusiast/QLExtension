/* $Header: /home/cvs/src/spot/src/spot.h,v 1.36 2000/11/29 23:14:50 abramjo Exp $ */
/****************************************************************
**
**	SPOT.H
**
**	Copyright 1992,1993,1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.36 $
**
****************************************************************/

#if !defined( IN_SPOT_H )
#define IN_SPOT_H

#include <editinfo.h>
#include <date.h>
#include <gob.h>


/*
**	Methods for options (pass as ValueData to GobValueFuncMethodStruct)
*/

DLLEXPORT GOB_HASH_PAIR
		SpotMethodsOpt[],
		SpotMethodsMapOpt[];


/*
**	Global EDIT_INFO constants
*/

DLLEXPORT EDIT_INFO
		SpotExerciseStructElems[],
		SpotExerciseElement;


DLLEXPORT GOB_WHERE_INFO
		SpotWhereDomIntRate[],
		SpotWhereForIntRate[];

#define		SPOT_DOMESTIC_INTEREST_RATE		{ "Domestic Interest Rate", &DtDouble, GobValueFuncWhere, SpotWhereDomIntRate, SDB_ALIAS }
#define		SPOT_FOREIGN_INTEREST_RATE      { "Foreign Interest Rate",  &DtDouble, GobValueFuncWhere, SpotWhereForIntRate, SDB_ALIAS }



/*
**	Declare global function prototypes
*/


// SPOTMKT.C

int		SpotFindMarketDate			( char *Market, char *ValueName, double TargetDate, char *MarketDateBuffer );


// SPOTOPS.C

DLLEXPORT DATE
		SpotSettleDate				( DATE ExpirationDate, int DaysToSpot, char *Currency1, char *Currency2 );

DLLEXPORT char
		*SpotSigOut					( char *Buffer, double Number, int Width ),
		*SpotCrossOut				( char *OutBuf, char *Cross );

DLLEXPORT int
		SpotMakeForward				( const char *QuantityUnit, double SettleDate, char *ForwardName ),

		/*
		** MtlMakeMetalForward() has been moved into the spot dll from the metal dll for the sake of SpotMakePhysicalForward.
		**/
		MtlMakeMetalForward			(char *QuantityUnit, double SettleDate, char *MetalClassShortName, char *DeliveryLocation, char *ForwardName),

		SpotMakePhysicalForward		( const char *QuantityUnit, double  SettleDate, const char *SecurityType, DT_STRUCTURE *SettlementInfo, char *ForwardName ),
		SpotMakeRepoForward			( const char *QuantityUnit, double SettleDate, char *RepoForwardName ),
		SpotMakeMarginSpot			( const char *QuantityUnit, char *MargSpotName ),
		SpotMakeLME					( const char *QuantityUnit, const char *Denominated, double SettleDate, char *ForwardName ),
		SpotMakeFuture				( const char *MarketDate, char *FutureName ),
		SpotMakeOption				( const char *Denominated, const char *QuantityUnit, const char *OptionStyle, const char *OptionType, const char *ExpLocation, double Strike, DATE ExpirationDate, DATE SettleDate, int CashPayment, char *OptionName ),
		SpotValidateExerPrice		( SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, double Price );

DLLEXPORT void
		SpotExerciseAppend			( DT_VALUE *ExerciseArray, double Quantity, char *Security );

DLLEXPORT int
		SpotValueFuncDenom			( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncQuantUnit		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncCross			( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncCrossExtra		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncExch			( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncMkt			( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncMktDate		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncExpMonth		( GOB_VALUE_FUNC_ARGS );


// SPOTOOPS.C

DLLEXPORT int
		SpotValueFuncExpiration		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncPctPrem		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncInvPctPrem		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncTtlPrem		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncInvQuantity	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncInvTtlPrem		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncInvPrem		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncStrike			( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncInvOptionType	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncTotalDelta		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncInvTotalDelta	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncSettleDate		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncNormalCross	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncNormalStrike	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncNormalType		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncNormalPrice	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncNormalQuant	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncPremium		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncAssign			( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncTradeTypesBinary(GOB_VALUE_FUNC_ARGS );

DLLEXPORT int
		SpotBackoutPrice			( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType,	double TargetAnswer ),
		SpotStartOpt				( SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData,	SDB_VALUE_TYPE_INFO *ValueTable ),
		SpotSetTrade				( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, char *PassThroughValueName, double PassThroughValue, int Interactive, char *PaymentUnitValueName );



// SPOTOPT.C

DLLEXPORT int
		SpotValueFuncPriceOpt		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncExpTransOpt	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncDaysToSetlOpt	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncImpliedVolOpt	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncForwardRate	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncExerciseOpt	( GOB_VALUE_FUNC_ARGS );


// SPOTXOPT.C

DLLEXPORT int
		SpotValueFuncExpDateXOpt	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncExpLocXOpt		( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncLotQuantity	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncQuoteDivisor	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncCustomerXOpt	( GOB_VALUE_FUNC_ARGS ),
		SpotValueFuncListedPrice	( GOB_VALUE_FUNC_ARGS );

DLLEXPORT int
		SpotValidateKnockout(		SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, double Price );


// SPOTFLOW.C

DLLEXPORT int
		SpotValidatePriceFlow( 		SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, double Price );


// SPOTTRAN.C

DLLEXPORT int
		SpotValueFuncTransRule(		GOB_VALUE_FUNC_ARGS );

// SPOTAOPT.C

DLLEXPORT int
		SpotObjFuncXOptAvg(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData);

// CS_OPT.C

DLLEXPORT int
		SpotObjFuncCSOpt(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData);
       
 
#endif
