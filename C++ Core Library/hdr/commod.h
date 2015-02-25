/****************************************************************
**
**	COMMOD.H	- Commodity header file
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/commod/src/commod.h,v 1.111 2008/04/23 21:22:35 e63100 Exp $
**
****************************************************************/

#ifndef IN_COMMOD_H
#define IN_COMMOD_H

#include <date.h>
#include <editinfo.h>
#include <gob.h>

class GsNodeValues;

#define COM_MKT_CC					"CC"
#define COM_MKT_SYMBOL_DEFAULT		"WTI"


#define DAILY_ALL 		"Daily/All Days"
#define AT_SETTLEMENT 	"Settlement"
#define GUESS_TYPE 		"Guess"			 // flag for "Guess" price fixes
#define UNVERIFIED_TYPE "Unverified"	 // flag for "Unverified" Ops entered price fixes

/*
**	Calculators
*/

#define CALC_AVERAGE_FWD		"EI: COMMOD AVERAGE FWD CL"
#define CALC_PHYS_DAILY			"EI: COMMOD DAILY PHYS CL"
#define CALC_PHYSICAL			"EI: COMMOD PHYSICAL CL"
#define CALC_OPTION_OA			"EI: COMMOD OPTION OA CL"
#define CALC_OPTION_OL			"EI: COMMOD OPTION OL CL"
#define CALC_OPTION_OTC			"EI: COMMOD OPTION OTC CL"


/*
**	Quanity Unit Macro
*/

// Fix: use 'Base' macro
#define BASE_UNITS(VT, Num, Invert) \
{ VT" Hash",				&DtStructure,	ComValueFuncQuantities,			    Invert,								SDB_CALCULATED						},			\
{ VT" Conversion Unit",		&DtString,		GobValueFuncAlias,		 			"Conversion Unit "Num,				SDB_CALCULATED                      },			\
{ VT" Converted",	 	 	&DtDouble,		GobValueFuncAlias,		 			VT" "Num,							SDB_CALCULATED					    },			\
{ VT" 1",					&DtDouble,		ComValueFuncQuantityNum,			Invert,								SDB_CALCULATED						},			\
{ VT" 2",					&DtDouble,		ComValueFuncQuantityNum,			Invert,								SDB_CALCULATED						},			\
{ VT" 3",					&DtDouble,		ComValueFuncQuantityNum,			Invert,								SDB_CALCULATED						}, 			\
{ VT" 4",					&DtDouble,		ComValueFuncQuantityNum,			Invert,								SDB_CALCULATED						}, 			\
{ VT" 5",					&DtDouble,		ComValueFuncQuantityNum,			Invert,								SDB_CALCULATED						},			\
{ VT" "Num,			    	&DtDouble,		ComValueFuncQuantityNum,			Invert,								SDB_CALCULATED						}

#define RECORD_UNITS(VT, Num) \
BASE_UNITS( VT, Num, NULL)


#define RECORD_PRICE_UNITS(VT, Num) \
{ VT" Quote Divisor",		&DtDouble,		ComValueFuncQuoteDivisor,			VT" Quote Conversion Unit",		    SDB_CALCULATED						},			\
{ VT" Quote",				&DtDouble,		ComValueFuncUnitQuoteValue,			VT,		   						 	SDB_CALCULATED						},			\
BASE_UNITS( VT" Quote", Num, &One)

#define PRICE_UNITS(VT, Num) \
{ VT" Quote Divisor",			&DtDouble,		ComValueFuncQuoteDivisor,			VT" Quote Conversion Unit",		    SDB_CALCULATED						},			\
{ VT" Quote",					&DtDouble,		ComValueFuncUnitQuoteValue,			VT,		   						 	SDB_CALCULATED					},			\
{ VT" Quote Hash",				&DtStructure,	ComValueFuncQuantities,			    &One,								SDB_CALCULATED					},			\
{ VT" Quote Conversion Unit",	&DtString,		GobValueFuncAlias,		 			"Conversion Unit "Num,	        	SDB_CALCULATED                  },			\
{ VT" Quote Converted",	    	&DtDouble,		GobValueFuncAlias,		 			VT" Quote "Num,						SDB_CALCULATED					},			\
{ VT" Quote "Num,				&DtDouble,		ComValueFuncQuantityNum,			&One,								SDB_CALCULATED					}


#define DISINHERIT_PRICE_UNITS(VT, Num) \
{ VT,							GOB_DISINHERIT	}, \
{ VT" Quote Divisor",			GOB_DISINHERIT	}, \
{ VT" Quote",					GOB_DISINHERIT	}, \
{ VT" Quote Hash",				GOB_DISINHERIT	}, \
{ VT" Quote Conversion Unit",	GOB_DISINHERIT	}, \
{ VT" Quote Converted",			GOB_DISINHERIT	}, \
{ VT" Quote "Num,				GOB_DISINHERIT	}

#define INHERIT_RECORD_PRICE_UNITS(Class, VT, Num) \
GOB_INHERIT_VALUE( Class, VT" Quote Divisor"    	), \
GOB_INHERIT_VALUE( Class, VT" Quote"				), \
GOB_INHERIT_VALUE( Class, VT" Quote Hash"			), \
GOB_INHERIT_VALUE( Class, VT" Quote Conversion Unit"), \
GOB_INHERIT_VALUE( Class, VT" Quote Converted"  	), \
GOB_INHERIT_VALUE( Class, VT" Quote 1"				), \
GOB_INHERIT_VALUE( Class, VT" Quote 2"				), \
GOB_INHERIT_VALUE( Class, VT" Quote 3"				), \
GOB_INHERIT_VALUE( Class, VT" Quote 4"				), \
GOB_INHERIT_VALUE( Class, VT" Quote 5"				), \
GOB_INHERIT_VALUE( Class, VT" Quote "Num			)

#define QUANTITY_UNIT 			"6"
#define STRIKE_UNIT 			"7"
#define SPOT_PRICE_UNIT 		"8"
#define PRICE_ADJUST_UNIT	 	"9"

/*
**	Defualt Objects
*/

#define DEFAULT_SWAP "Swap Null"
#define DEFAULT_PORT "Default Port"


/*
**	Shared definitions
*/


#define	BEAN_CLASS				"Bean"
#define	BEAN_CLASS_FUTURE		BEAN_CLASS	   " Future"


#define	COM_CLASS				"Commod"
#define	COM_CLASS_AVG_FWD		COM_CLASS " Average Forward"
#define	COM_CLASS_CONTRACT		COM_CLASS " Contract"
#define	COM_CLASS_CONTRACT_LOC	COM_CLASS " Contract Location"
#define	COM_CLASS_FUTURE		COM_CLASS " Future"
#define	COM_CLASS_SWAP			COM_CLASS " Generic Swap"
#define	COM_CLASS_OPTION		COM_CLASS " Option"
#define	COM_CLASS_INV			COM_CLASS " Inventory"
#define	COM_IMPLIED_NAME_MARKET	COM_CLASS " %.3s"

#define	COM_PRICE_TYPE_DEFAULT		""

// Index "Contracts By Market" invalid for given market

#define	COM_MSG_INDEX_INVALIDATE	SDB_MSG_USER+0

#define COMMOD_LOC_NONE		"Commod Location None"


/*
**	commod specific datatypes
*/

/*
**	Data structure for initializing Trade VT controls
**
**	FIX: This needs to be moved to GOB.H
*/

struct COM_TRADE_CONTROL
{
	char	*VT;

	double	Flags;

};


struct EXPLOSION_VTI_DATA
{
	char *SecType;
	double InMemory;
};

/*
**	String Tables
*/


extern char
		**StringTableDeliveryLocation,
		**StringTableSwapQuantityType,
		**StringTableQuantityUnit,
		**StringTableContractMonth,
		**StringTableLocationType,
		**StringTableMarket,
		**StringTablePaymentFrequency,
		**StrTabFixType,
		**StrTabRollType,
		**StrTabUnits,
		**StrTabDelivery,
		**StrTabEFPPayment,
		**StrTabExercise,
		**StringTablePriceType,
		**StringTableCommodFamily;


/*
**	Functions
*/

DATE	ComDateFromSymbol( const char *Symbol );

DATE	ComSatNearbyContractSymbol( DATE BaseDate, const char *LastTradingDayRule, const char *NearbyContractRule, const char *ValidLetters, const char *Calendar );


int		ComSatContractSymbolFromDate(
				const char	*MarketSymbol,		// Base Market Symbol
				DATE		ContractDate,		// Date in Contract month
				char		*Symbol				// Returned Value
		);

int		ComSatTsdbSymbolFromRule(
				const char	*Symbol,			// Substitutes an !
				DATE		Date,				// Substitutes an ^
				double		Strike,				// Substitutes an /
				const char	*Type, 				// Substitutes an ~
				const char	*Rule,				// Substitution Rule
				char		*Tsdb				// Pre-allocated buffer
		);

extern EDIT_INFO_ACTION CallBackDeliveryLocation(
		EDIT_INFO_CALL_BACK	*CallBack,
		EDIT_INFO_EVENT		Event,
		void				*EventArg
);

// in comops.c

void EditInfoInitComOpt(
	EDIT_INFO_INIT	*Self,
	int				ForceReinit
);


// In COMVF.C

void	ComIndexInvalidate			 ( char *SecName );


		// Exported Value funcs
DLLEXPORT int
		ComValueFuncEmptyStruct		 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncFltSecurityPointers  	( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncIndexStructure			( GOB_VALUE_FUNC_ARGS ),
        ComValueFuncMkSecFmStructInCache    ( GOB_VALUE_FUNC_ARGS ),
        ComValueFuncMakeExplosionSecs       ( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncQuantityNum				( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncSecurityNames	  		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncSecurityPointers  		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncTradeControlStruct		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncSwaptionVolActConts		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncSwaptionOOMM			( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncSwaptionFutWts			( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncChildVNoVal			    ( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncChildDynamicNoVal       ( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncExtractComponentNoVal   ( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncSwaptionCurrAmts		( GOB_VALUE_FUNC_ARGS );

DLLEXPORT DT_CURVE *FindSpotVolCurve(
	GsNodeValues** TermVolVecs, // SDB_DYNAMIC_POINTERS *TermVolVecs,
	GsNodeValues** VolVecs,     // SDB_DYNAMIC_POINTERS *VolVecs,
	GsNodeValues** ExpDates,    // SDB_DYNAMIC_NUMBERS	*ExpDates,
	GsNodeValues** FutDates,    // SDB_DYNAMIC_NUMBERS  *FutDates,
    int	StartElem,
    int NumElems,
    double OOMult,
    double FirstVol,
    double ExpDate,
    double Today
);

		// Value funcs
int

		// NOTE: a bunch of these are only used once and could be made static
		// in individual files, as noted below.

		ComValueFuncAllCrossFwdPoints		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncAllCurrencies	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncAllCurrencyAmounts		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncAllCurrencyCrosses		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncActiveContracts	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncExpDate			 		( GOB_VALUE_FUNC_ARGS ), // only used in combasec.c, could move
		ComValueFuncExpSettleDate	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncObsExpSettleDate  		( GOB_VALUE_FUNC_ARGS ), // only used in comfut.c
		ComValueFuncCashPhys		 		( GOB_VALUE_FUNC_ARGS ),// only used in comopt.c
		ComValueFuncCancelEffects	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncChildren		 		( GOB_VALUE_FUNC_ARGS ),// only in comswap.c
		ComValueFuncContract		 		( GOB_VALUE_FUNC_ARGS ),// only in combasec.c
		ComValueFuncConMarketSymbol	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncContractMonth	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncContractObject	 		( GOB_VALUE_FUNC_ARGS ), // only in combasec.c
		ComValueFuncDeleteable		 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncForwardSpotPrice 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncImpliedNameAvgFwd		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncImpliedNameAvgOpt		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncImpliedNameOL	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncImpliedNameOTC	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncImpliedNameSwap 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncLocationType			( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncLocationObject	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncMarket			 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncMRParamsArray	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncMRTermVolsOfSpotHodgdon ( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncQuantities				( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncQuoteValue		 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncParentPtrInfo 	 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncParentNumberInfo  		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncParentPosEffects  		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncParentPosAssembly 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncParentTradeInfo   		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncPriceFixes		  		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncQuoteDivisor	  		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncRecChildren		  		( GOB_VALUE_FUNC_ARGS ), // only in comswap.c
		ComValueFuncSwapSpotPrice	  		( GOB_VALUE_FUNC_ARGS ), // only in comswap.c
		ComValueFuncTapDisplayLocation		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncTapDisplayActCons 		( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncUnitQuoteValue		  	( GOB_VALUE_FUNC_ARGS ),
		ComValueFuncVolatility				( GOB_VALUE_FUNC_ARGS ), // only in comotc.c
		ComValueFuncSortedDateArray			( GOB_VALUE_FUNC_ARGS );


		// Misc


int		ComMakeFuture(				 char	*Market,
									 char	*Contract,
									 char	*DelivLoc,
									 char	*PriceType,
									 char	*FutureName,
									 int	Bean	);


int		ComMakePhysicalLookalike(
								     char		*Id,
									 char 		*SampleFreq,
									 DATE		StartDate,
									 DATE		EndDate,
									 char		*MarketSymbol,
									 char		*Location,
									 DT_CURVE	*QuantityFixCurve,
									 DT_CURVE	*SampleCurve,
									 void       *Parameters,
									 char		*PhysName,
									 char		*ActualType );

int		ComMakeDailyPhysical(
									 DATE		DeliveryDate,
									 char		*MarketSymbol,
									 char		*Location,
									 char		*PhysName );

        
									 
int		ComMakeAverageForward(
								     char		*Id,
									 char 		*SampleFreq,
									 DATE		StartDate,
									 DATE		EndDate,
									 char		*MarketSymbol,
									 char		*Location,
                                     char       *PriceType,
									 DT_CURVE	*QuantityFixCurve,
									 DT_CURVE	*SampleCurve,
									 char		*PhysName,
									 char		*ActualType,
                                     char       *Denominated,
                                     double     PriceAdjust,
                                     double     RollType,
                                     double     RollAdjust,
                                     DATE       ExpSetDate,
                                     char       *ContractMonth );


int		ComMakeSwapPayment( 		 char		*Denominated,
									 DT_ARRAY  *Quantities,
									 DT_ARRAY  *Securities,
									 char 		*Parent,
									 char 		*Portfolio1,
									 char 		*Portfolio2,
		 							 int		SecType,
									 char		*PaymentName);

int     ComOptionPrice(          	 const char	*ValueTypeName,
                              		 const char	*SecurityName,
		                             double		Expiration,
		                             double		Settlement,
		                             double		PricingDate,
		                             double		MarketQuote,
		                             double		Strike,
		                             double		DomesticIntRate,
		                             double		Volatility,
		                             const char	*OptionType,
		                             const char	*OptionStyle,
		                             const char	*ExerInstrument,
		                             double		MarkDaily,
		                             double		*Price,
		                             double		*Delta );

double  ComOptionImpliedVols(		const char	*ValueTypeName,
									const char	*SecurityName,
									double		Expiration,
									double		Settlement,
									double		PricingDate,
									double		MarketQuote,
									double		Strike,
									double		DomesticIntRate,
									double		Volatility,
									const char	*OptionType,
									const char	*OptionStyle,
									const char	*ExerInstrument,
									double		MarkDaily,
									double		SpotPrice );


SDB_OBJECT		
		*ComMakeSwap( 		 		char		*Denominated,
									DATE	    ExpirationDate,
									DT_ARRAY    *Quantities,
									DT_ARRAY    *Securities,
									DT_ARRAY    *PosEffects,
									DT_ARRAY    *PosTrans,
									char 	    *DealID,
									char 	    *Parent,
									char 	    *TradeType,
									char 	    *Head,
									char 	    *Portfolio1,
									char 	    *Portfolio2,
									double	    SwapID,
									double	    EffQuantity,
									double	    Count,
		 							int		    SecType );

		 							 
void 	SpotExerciseAppendSecPtrs(
									DT_VALUE	*ExerciseArray,
									double		Quantity,
									SDB_OBJECT	*Security
);

// Shushes

#endif
