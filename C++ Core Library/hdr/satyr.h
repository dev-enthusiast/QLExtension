/* $Header: /home/cvs/src/satyr/src/satyr.h,v 1.23 2001/11/27 23:21:48 procmon Exp $ */
/****************************************************************
**
**	SATYR.H		SATYR Object header file
**
**	$Revision: 1.23 $
**
****************************************************************/

#if !defined( IN_SATYR_H )
#define IN_SATYR_H

#if !defined( IN_DATE_H )
#	include	<date.h>
#endif
#if !defined( IN_SECDB_H )
#	include	<secdb.h>
#endif
#if !defined( IN_SECOBJ_H )
#	include	<secobj.h>
#endif
#if !defined( IN_GOB_H )
#	include	<gob.h>
#endif

#ifdef BUILDING_SATYR
#define EXPORT_C_SATYR		EXPORT_C_EXPORT
#else
#define EXPORT_C_SATYR		EXPORT_C_IMPORT
#endif


#define JARON_SUFFIX			"_ARON"

DATE DateFromSymbol(
	char		*Symbol
);

int	SatContractSymbolFromDate(
	char			*MarketSymbol,	//	Base Market Symbol
	DATE			ContractDate,	//	Date in Contract month
	char			*Symbol			//	Returned Value
);

DATE SatNearbyContractSymbol(
	DATE			BaseDate,
	char			*LastTradingDayRule,
	char			*NearbyContractRule,
	char			*ValidLetters,
	char			*Calendar
);

DATE SatContractDeliveryDate( GOB_VALUE_FUNC_ARGS );

int SatTsdbSymbolFromRule(
		char	*Symbol,			//	Substitutes an !
		DATE	Date,				//	Substitutes an ^
		double	Strike,				//	Substitutes an /
		char	*Type, 				//	Substitutes an ~
		char	*Rule,				//	Substitution Rule
		char	*Tsdb				//	Pre-allocated buffer
);


#define	SAT_CLASS_NAME				"Cmdty"
#define	SAT_CLASS_NAME_MARKET		SAT_CLASS_NAME " Market"
#define	SAT_CLASS_NAME_CONTRACT		SAT_CLASS_NAME " Contract"
#define	SAT_CLASS_NAME_FUTURE		SAT_CLASS_NAME " Future"
#define	SAT_CLASS_NAME_OPTION		SAT_CLASS_NAME " Option"


#define	SAT_IMPLIED_NAME_MARKET		"%s %s"					// SAT_CLASS_NAME
#define	SAT_IMPLIED_NAME_CONTRACT	"%s %s"					// SAT_CLASS_NAME
#define	SAT_IMPLIED_NAME_FUTURE		"%s Fut %s"				// SAT_CLASS_NAME
#define	SAT_IMPLIED_NAME_OPTION		"%s Opt %s %8.4f %.1s"	// SAT_CLASS_NAME


EXPORT_C_SATYR int
		SatValueFuncClassType(				GOB_VALUE_FUNC_ARGS ),
		SatValueFuncContract(				GOB_VALUE_FUNC_ARGS	),
		SatValueFuncContractContract(		GOB_VALUE_FUNC_ARGS ),
		SatValueFuncContractDate(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncContractImpliedName(	GOB_VALUE_FUNC_ARGS	),
		SatValueFuncContractMarket(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncContractSymbol(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncContractUnderlier(		GOB_VALUE_FUNC_ARGS ),
		SatValueFuncContractValue(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncCross(					GOB_VALUE_FUNC_ARGS	),
		SatValueFuncCrossExists(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncDollarCross(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncDollarSpot(				GOB_VALUE_FUNC_ARGS	),
		SatValueFuncForwardCurve(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncFutureImpliedName(		GOB_VALUE_FUNC_ARGS	),
		SatValueFuncInterestRateCurve(		GOB_VALUE_FUNC_ARGS	),
		SatValueFuncMarket(					GOB_VALUE_FUNC_ARGS	),
		SatValueFuncMarketImpliedName(		GOB_VALUE_FUNC_ARGS	),
		SatValueFuncMarketofContract(		GOB_VALUE_FUNC_ARGS	),
		SatValueFuncMarketQuote(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncMarketVol(				GOB_VALUE_FUNC_ARGS	),
		SatValueFuncMonthLetters(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncOptionImpliedName(		GOB_VALUE_FUNC_ARGS	),
		SatValueFuncPointValue(				GOB_VALUE_FUNC_ARGS	),
		SatValueFuncPricingModel(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncQtyUnitExists(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncQtyUnitSecType(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncSpot(					GOB_VALUE_FUNC_ARGS	),
		SatValueFuncStrip(					GOB_VALUE_FUNC_ARGS	),
		SatValueFuncTickValue(				GOB_VALUE_FUNC_ARGS	),
		SatValueFuncTsdbQuote(				GOB_VALUE_FUNC_ARGS	),
		SatValueFuncTsdbRTSymbol(			GOB_VALUE_FUNC_ARGS	),
		SatValueFuncTsdbSymbol(				GOB_VALUE_FUNC_ARGS	),
		SatValueFuncVolatility(				GOB_VALUE_FUNC_ARGS	),
		SatValueFuncVolCurve(				GOB_VALUE_FUNC_ARGS	);

#endif
