/****************************************************************
**
**	LTSWAPCURVE.H	- LtSwapCurve class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltaddin/src/ltaddin/LtSwapCurve.h,v 1.38 2011/08/12 19:03:15 khodod Exp $
**
****************************************************************/

#if !defined( IN_LTADDIN_LTSWAPCURVE_H )
#define IN_LTADDIN_LTSWAPCURVE_H

#include <ltaddin/base.h>
#include <secdb/LtSecDbObj.h>
#include <gsdt/GsDtGeneric.h>
#include <gscore/GsTimeSeries.h>
#include <gscore/GsTsHandle.h>
#include <gscore/GsPeriod.h>
#include <gscore/GsDayCount.h>
#include <gscore/GsRDate.h>
#include <gscore/GsRDateLazyDate.h>
#include <gscore/GsDate.h>
#include <gscore/GsDateGen.h>
#include <gsquant/Enums.h>
#include <gscore/GsDiscountCurve.h>
#include <cccomp.h> 

// Not sure if all these are needed (perhaps for GsDtDictionary.h?)
#include <gsdt/GsDtDictionary.h>
#include <gsdt/GsDtDayCount.h>
#include <gscore/types.h>
#include <gscore/gsdt_fwd.h>

#define LT_SWAP_ARGS									\
    const GsRDate& EffectiveDate,            			\
    const GsRDate& Tenor,                    			\
	double FloatSpreadBp,								\
    const GsRDate& FixedFreq,              				\
    const GsRDate& FloatFreq,           				\
    const GsRDate& FloatCompoundingFreq,           		\
    const GsDayCount& FixedDayCount,     				\
    const GsDayCount& FloatDayCount,  					\
	double FirstReset,                        			\
    const GsString& BusinessDays,               		\
    const GsString& FloatRate,                          \
    enum LT_STUB_TYPE FixedStartStub,                   \
    enum LT_STUB_TYPE FixedEndStub,                     \
    enum LT_STUB_TYPE FloatStartStub,                   \
    enum LT_STUB_TYPE FloatEndStub,                     \
    enum LT_ADJUSTED_ACCRUAL FixedAdjust,           	\
    enum LT_ADJUSTED_ACCRUAL FloatAdjust,           	\
    enum LT_BUSINESS_DAY_CONVENTION FixedBusDayConv,	\
    enum LT_BUSINESS_DAY_CONVENTION FloatBusDayConv

#define LT_SWAP_ARGS_WITH_DICTIONARY			\
    const GsRDate& EffectiveDate,				\
    const GsRDate& Tenor,						\
    const GsRDate& FixedFreq,					\
    const GsRDate& FloatFreq,					\
    const GsRDate& FloatCompoundingFreq,		\
    const GsDayCount& FixedDayCount,			\
    const GsDayCount& FloatDayCount,			\
    double FirstReset,							\
    const GsDtDictionary& NamedArguments

#define LT_SWAP_PARMS	                \
    EffectiveDate,                      \
    Tenor,                              \
	FloatSpreadBp,	                	\
    FixedFreq,                          \
    FloatFreq,                          \
    FloatCompoundingFreq,               \
    FixedDayCount,     	                \
    FloatDayCount,  	                \
	FirstReset,                         \
    BusinessDays,                       \
    FloatRate,                          \
    FixedStartStub,                     \
    FixedEndStub,                       \
    FloatStartStub,                     \
    FloatEndStub,                       \
    FixedAdjust,                        \
    FloatAdjust,                        \
    FixedBusDayConv,	                \
    FloatBusDayConv

#define LT_MAKE_NAMED_ARGS																												\
    const GsDouble* PtrFloatSpreadBp;																									\
    const GsString* PtrBusinessDays;																									\
    const GsString* PtrFloatRate;																										\
    const GsString* StringFixedAdjust;																									\
    const GsString* StringFloatAdjust;																									\
    const GsString* StringFixedBusDayConv;																								\
    const GsString* StringFloatBusDayConv;																								\
    const GsString* StringFixedStartStub;																								\
    const GsString* StringFixedEndStub;																									\
    const GsString* StringFloatStartStub;																								\
    const GsString* StringFloatEndStub;																									\
																																		\
	double FloatSpreadBp;																												\
    GsString BusinessDays;																												\
    GsString FloatRate;																													\
    LT_ADJUSTED_ACCRUAL FixedAdjust;																									\
    LT_ADJUSTED_ACCRUAL FloatAdjust;																									\
    LT_BUSINESS_DAY_CONVENTION FixedBusDayConv;																							\
    LT_BUSINESS_DAY_CONVENTION FloatBusDayConv;																							\
    LT_STUB_TYPE FixedStartStub;																										\
    LT_STUB_TYPE FixedEndStub;																											\
    LT_STUB_TYPE FloatStartStub;																										\
    LT_STUB_TYPE FloatEndStub;																											\
																																		\
	GsDtDictionaryElement( NamedArguments, GsString( "FloatSpreadBp" ),        PtrFloatSpreadBp           );							\
	GsDtDictionaryElement( NamedArguments, GsString( "BusinessDays" ),         PtrBusinessDays            );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FloatRate" ),            PtrFloatRate               );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FixedAdjust" ),          StringFixedAdjust          );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FloatAdjust" ),          StringFloatAdjust          );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FixedBusDayConv" ),      StringFixedBusDayConv      );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FloatBusDayConv" ),      StringFloatBusDayConv      );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FixedStartStub" ),   	   StringFixedStartStub       );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FixedEndStub" ),         StringFixedEndStub         );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FloatStartStub" ),       StringFloatStartStub       );							\
	GsDtDictionaryElement( NamedArguments, GsString( "FloatEndStub" ),         StringFloatEndStub         );							\
																																		\
	if( PtrFloatSpreadBp )																												\
		FloatSpreadBp = *PtrFloatSpreadBp;																								\
	else																																\
		FloatSpreadBp = 0.0;																											\
																																		\
	if( PtrBusinessDays )																												\
		BusinessDays = *PtrBusinessDays;																								\
	else																																\
		BusinessDays = Curve.defBusinessDays();																							\
																																		\
    if( PtrFloatRate )																													\
        FloatRate = *PtrFloatRate;																										\
    else																																\
		FloatRate = Curve.defFloatRate();																								\
																																		\
	if( StringFixedAdjust == NULL )																										\
		FixedAdjust = LT_AA_DEFAULT;																									\
	else																																\
		FixedAdjust = (LT_ADJUSTED_ACCRUAL) GsEnumLookup( "LT_ADJUSTED_ACCRUAL", StringFixedAdjust->c_str() );							\
																																		\
	if( StringFloatAdjust == NULL )																										\
		FloatAdjust = LT_AA_DEFAULT;																									\
	else																																\
		FloatAdjust = (LT_ADJUSTED_ACCRUAL) GsEnumLookup( "LT_ADJUSTED_ACCRUAL", StringFloatAdjust->c_str() );							\
																																		\
	if( StringFixedBusDayConv == NULL )																									\
		FixedBusDayConv = LT_BDC_DEFAULT;																								\
	else																																\
		FixedBusDayConv = (LT_BUSINESS_DAY_CONVENTION) GsEnumLookup( "LT_BUSINESS_DAY_CONVENTION", StringFixedBusDayConv->c_str() );	\
																																		\
	if( StringFloatBusDayConv == NULL )																									\
		FloatBusDayConv = LT_BDC_DEFAULT;																								\
	else																																\
		FloatBusDayConv = (LT_BUSINESS_DAY_CONVENTION) GsEnumLookup( "LT_BUSINESS_DAY_CONVENTION", StringFloatBusDayConv->c_str() );	\
																																		\
	if( StringFixedStartStub == NULL )																									\
		FixedStartStub = LT_ST_SHORT;																									\
	else																																\
		FixedStartStub = (LT_STUB_TYPE) GsEnumLookup( "LT_STUB_TYPE", StringFixedStartStub->c_str() );									\
																																		\
	if( StringFixedEndStub == NULL )																									\
		FixedEndStub = LT_ST_SHORT;																										\
	else																																\
		FixedEndStub = (LT_STUB_TYPE) GsEnumLookup( "LT_STUB_TYPE", StringFixedEndStub->c_str() );										\
																																		\
	if( StringFloatStartStub == NULL )																									\
		FloatStartStub = LT_ST_SHORT;																									\
	else																																\
		FloatStartStub = (LT_STUB_TYPE) GsEnumLookup( "LT_STUB_TYPE", StringFloatStartStub->c_str() );									\
																																		\
	if( StringFloatEndStub == NULL )																									\
		FloatEndStub = LT_ST_SHORT;																										\
	else																																\
		FloatEndStub = (LT_STUB_TYPE) GsEnumLookup( "LT_STUB_TYPE", StringFloatEndStub->c_str() );

#define LT_OPEN_DIDDLE_SCOPE															\
    DT_VALUE* DtVal = m_CurveObject.getValue( "SwapScope", DtDiddleScope );				\
																						\
	if( !DtVal )																		\
		GSX_THROW( GsXInvalidOperation, "Failed to get diddle scope from slang ufo" );	\
																						\
    DT_DIDDLE_SCOPE* DidScope = (DT_DIDDLE_SCOPE*) DtVal->Data.Pointer;					\
    {																					\
		SdbDiddleScopeUse myDiddleScopeUseObject( DidScope );

#define LT_CLOSE_DIDDLE_SCOPE_AND_RETURN												\
	}																					\
	return toReturn;

#define LT_DIDDLE_EFFECTIVE_DATE																														\
	LtDtArgs EffectiveDateArg; SDB_DIDDLE_ID EffectiveDateId = SDB_DIDDLE_ID_ERROR;																		\
	if( ! ( EffectiveDate    == defEffectiveDate() ) )																   									\
    {																																					\
		LtDtArgAdd( EffectiveDateArg, new GsRDate::GsDtType( EffectiveDate ) );																			\
		EffectiveDateId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "EffectiveDate", LT_DT_SET_ARG( EffectiveDateArg ) );						\
	}																															

#define LT_DIDDLE_TENOR																																	\
	LtDtArgs TenorArg; SDB_DIDDLE_ID TenorId = SDB_DIDDLE_ID_ERROR;																						\
    LtDtArgAdd( TenorArg, new GsRDate::GsDtType( Tenor ) );																								\
	TenorId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "Tenor", LT_DT_SET_ARG( TenorArg ) );								

#define LT_DIDDLE_FLOAT_SPREAD_BP																														\
	LtDtArgs FloatSpreadBpArg; SDB_DIDDLE_ID FloatSpreadBpId = SDB_DIDDLE_ID_ERROR;																		\
    if( ! ( FloatSpreadBp == 0.0                       ) )															   									\
    {																																					\
        LtDtArgAdd( FloatSpreadBpArg, FloatSpreadBp );																									\
		FloatSpreadBpId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatSpreadBp", LT_DT_SET_ARG( FloatSpreadBpArg ) );						\
	}																															

#define LT_DIDDLE_FIXED_FREQ																															\
	LtDtArgs FixedFreqArg; SDB_DIDDLE_ID FixedFreqId = SDB_DIDDLE_ID_ERROR;																				\
    if( ! ( FixedFreq == defFixedFreq() ) )															   													\
    {																																					\
        LtDtArgAdd( FixedFreqArg, new GsRDate::GsDtType( FixedFreq ) );																					\
		FixedFreqId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FixedNominalFreq", LT_DT_SET_ARG( FixedFreqArg ) );							\
	}																															

#define LT_DIDDLE_FLOAT_FREQ																															\
	LtDtArgs FloatFreqArg; SDB_DIDDLE_ID FloatFreqId = SDB_DIDDLE_ID_ERROR;																				\
    if( ! ( FloatFreq == defFloatFreq() ) )																			   									\
    {																																					\
        LtDtArgAdd( FloatFreqArg, new GsRDate::GsDtType( FloatFreq ) );																					\
		FloatFreqId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatNominalFreq", LT_DT_SET_ARG( FloatFreqArg ) );							\
	}																															

#define LT_DIDDLE_FLOAT_COMPOUNDING_FREQ																												\
	LtDtArgs FloatCompoundingFreqArg; SDB_DIDDLE_ID FloatCompoundingFreqId = SDB_DIDDLE_ID_ERROR;														\
    if( ! ( FloatCompoundingFreq == defFloatCompoundingFreq() ) )																						\
    {																																					\
        LtDtArgAdd( FloatCompoundingFreqArg, new GsRDate::GsDtType( FloatCompoundingFreq ) );															\
		FloatCompoundingFreqId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatCompoundingFreq", LT_DT_SET_ARG( FloatCompoundingFreqArg ) );\
	}																															

#define LT_DIDDLE_FIXED_DAY_COUNT																														\
	LtDtArgs FixedDayCountArg; SDB_DIDDLE_ID FixedDayCountId = SDB_DIDDLE_ID_ERROR;																		\
    if( ! ( FixedDayCount == defFixedDayCount() ) )															   											\
    {																																					\
        LtDtArgAdd( FixedDayCountArg, new GsDayCount::GsDtType( FixedDayCount ) );																		\
		FixedDayCountId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FixedDayCount", LT_DT_SET_ARG( FixedDayCountArg ) );						\
	}																															

#define LT_DIDDLE_FLOAT_DAY_COUNT																														\
	LtDtArgs FloatDayCountArg; SDB_DIDDLE_ID FloatDayCountId = SDB_DIDDLE_ID_ERROR;																		\
    if( ! ( FloatDayCount    == defFloatDayCount() )	)															   									\
    {																																					\
        LtDtArgAdd( FloatDayCountArg, new GsDayCount::GsDtType( FloatDayCount ) );																		\
		FloatDayCountId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatDayCount", LT_DT_SET_ARG( FloatDayCountArg ) );						\
	}																															

#define LT_DIDDLE_FIRST_RESET																															\
	LtDtArgs FirstResetArg; SDB_DIDDLE_ID FirstResetId = SDB_DIDDLE_ID_ERROR;																			\
    if( ! ( FirstReset       == defFirstReset()    )	)															   									\
    {																																					\
        LtDtArgAdd( FirstResetArg, FirstReset );																										\
		FirstResetId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FirstReset", LT_DT_SET_ARG( FirstResetArg ) );								\
	}																															

#define LT_DIDDLE_BUSINESS_DAYS																															\
	LtDtArgs BusinessDaysArg; SDB_DIDDLE_ID BusinessDaysId = SDB_DIDDLE_ID_ERROR;																		\
    if( ! ( StrICmp( BusinessDays, defBusinessDays() ) == 0  ) )													   									\
    {																																					\
        LtDtArgAdd( BusinessDaysArg, BusinessDays );																									\
		BusinessDaysId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "BusinessDays", LT_DT_SET_ARG( BusinessDaysArg ) );						\
	}																															

#define LT_DIDDLE_FLOAT_RATE																															\
	LtDtArgs FloatRateArg; SDB_DIDDLE_ID FloatRateId = SDB_DIDDLE_ID_ERROR;																				\
    if( ! ( StrICmp( FloatRate, defFloatRate()	) == 0  ) )															   									\
    {																																					\
        LtDtArgAdd( FloatRateArg, FloatRate );																											\
		FloatRateId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatRate", LT_DT_SET_ARG( FloatRateArg ) );									\
	}																															

#define LT_DIDDLE_FIXED_START_STUB																														\
	LtDtArgs FixedStartStubArg; SDB_DIDDLE_ID FixedStartStubId = SDB_DIDDLE_ID_ERROR;																	\
    if( ! ( FixedStartStub   == LT_ST_SHORT      )  )																   									\
    {																																					\
        LtDtArgAdd( FixedStartStubArg, (double) ( FixedStartStub ) );																					\
		FixedStartStubId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FixedStartStubType", LT_DT_SET_ARG( FixedStartStubArg ) );				\
	}																															

#define LT_DIDDLE_FIXED_END_STUB																														\
	LtDtArgs FixedEndStubArg; SDB_DIDDLE_ID FixedEndStubId = SDB_DIDDLE_ID_ERROR;																		\
    if( ! ( FixedEndStub     == LT_ST_SHORT      )  )																   									\
    {																																					\
        LtDtArgAdd( FixedEndStubArg, (double) FixedEndStub );																							\
		FixedEndStubId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FixedEndStubType", LT_DT_SET_ARG( FixedEndStubArg ) );					\
	}																															

#define LT_DIDDLE_FLOAT_START_STUB																														\
	LtDtArgs FloatStartStubArg; SDB_DIDDLE_ID FloatStartStubId = SDB_DIDDLE_ID_ERROR;																	\
    if( ! ( FloatStartStub   == LT_ST_SHORT      )  )																   									\
    {																																					\
        LtDtArgAdd( FloatStartStubArg, (double) ( FloatStartStub ) );																					\
		FloatStartStubId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatStartStubType", LT_DT_SET_ARG( FloatStartStubArg ) );				\
	}																															

#define LT_DIDDLE_FLOAT_END_STUB																														\
	LtDtArgs FloatEndStubArg; SDB_DIDDLE_ID FloatEndStubId = SDB_DIDDLE_ID_ERROR;																		\
    if( ! ( FloatEndStub     == LT_ST_SHORT      )  )																   									\
    {																																					\
        LtDtArgAdd( FloatEndStubArg, (double) ( FloatEndStub ) );																						\
		FloatEndStubId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatEndStubType", LT_DT_SET_ARG( FloatEndStubArg ) );					\
	}																															

#define LT_DIDDLE_FIXED_ADJUST																															\
	LtDtArgs FixedAdjustArg; SDB_DIDDLE_ID FixedAdjustId = SDB_DIDDLE_ID_ERROR;																			\
    if( ! ( FixedAdjust      == LT_AA_DEFAULT    )	)																   									\
    {																																					\
        LtDtArgAdd( FixedAdjustArg, (double) ( FixedAdjust ) );																							\
		FixedAdjustId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FixedAdjusted", LT_DT_SET_ARG( FixedAdjustArg ) );							\
	}																															

#define LT_DIDDLE_FLOAT_ADJUST																															\
	LtDtArgs FloatAdjustArg; SDB_DIDDLE_ID FloatAdjustId = SDB_DIDDLE_ID_ERROR;																			\
    if( ! ( FloatAdjust      == LT_AA_DEFAULT    )	)																   									\
    {																																					\
        LtDtArgAdd( FloatAdjustArg, (double) ( FloatAdjust ) );																							\
		FloatAdjustId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatAdjusted", LT_DT_SET_ARG( FloatAdjustArg ) );							\
	}																															

#define LT_DIDDLE_FIXED_BUS_DAY_CONV																													\
	LtDtArgs FixedBusDayConvArg; SDB_DIDDLE_ID FixedBusDayConvId = SDB_DIDDLE_ID_ERROR;																	\
    if( ! ( FixedBusDayConv  == LT_BDC_DEFAULT   )	)																						   			\
    {																																					\
        LtDtArgAdd( FixedBusDayConvArg, (double) ( FixedBusDayConv ) );																					\
		FixedBusDayConvId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FixedBusinessDayConvention", LT_DT_SET_ARG( FixedBusDayConvArg ) );	\
	}																															

#define LT_DIDDLE_FLOAT_BUS_DAY_CONV																													\
	LtDtArgs FloatBusDayConvArg; SDB_DIDDLE_ID FloatBusDayConvId = SDB_DIDDLE_ID_ERROR;																	\
    if( ! ( FloatBusDayConv  == LT_BDC_DEFAULT   )	)																									\
    {																																					\
        LtDtArgAdd( FloatBusDayConvArg, (double) ( FloatBusDayConv ) );																					\
		FloatBusDayConvId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FloatBusinessDayConvention", LT_DT_SET_ARG( FloatBusDayConvArg ) );	\
	}

#define LT_DIDDLE_FIXED_FREQ_ZERO_COUPON																												\
	LtDtArgs FixedFreqArg; SDB_DIDDLE_ID FixedFreqId = SDB_DIDDLE_ID_ERROR;																				\
    if( ! ( FixedFreq == defZeroCouponPeriod() ) )																										\
	{																																					\
		LtDtArgAdd( FixedFreqArg, new GsRDate::GsDtType( FixedFreq ) );																					\
		FixedFreqId = CC_MUTABLE( LtSwapCurve )->m_CurveObject.setDiddle( "FixedNominalFreq", LT_DT_SET_ARG( FixedFreqArg ) );							\
	}

#define LT_REMOVE_DIDDLE_EFFECTIVE_DATE																													\
	if( EffectiveDateId != SDB_DIDDLE_ID_ERROR )																										\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "EffectiveDate", LT_DT_SET_ARG( EffectiveDateArg ), EffectiveDateId );	
#define LT_REMOVE_DIDDLE_TENOR																															\
	if( TenorId != SDB_DIDDLE_ID_ERROR )																												\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "Tenor", LT_DT_SET_ARG( TenorArg ), TenorId );												
#define LT_REMOVE_DIDDLE_FLOAT_SPREAD_BP																												\
	if( FloatSpreadBpId != SDB_DIDDLE_ID_ERROR )																										\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatSpreadBp", LT_DT_SET_ARG( FloatSpreadBpArg ), FloatSpreadBpId );	
#define LT_REMOVE_DIDDLE_FIXED_FREQ																														\
	if( FixedFreqId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FixedNominalFreq", LT_DT_SET_ARG( FixedFreqArg ), FixedFreqId );		
#define LT_REMOVE_DIDDLE_FLOAT_FREQ																														\
	if( FloatFreqId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatNominalFreq", LT_DT_SET_ARG( FloatFreqArg ), FloatFreqId );							
#define LT_REMOVE_DIDDLE_FLOAT_COMPOUNDING_FREQ																											\
	if( FloatCompoundingFreqId != SDB_DIDDLE_ID_ERROR )																									\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatCompoundingFreq", LT_DT_SET_ARG( FloatCompoundingFreqArg ), FloatCompoundingFreqId );
#define LT_REMOVE_DIDDLE_FIXED_DAY_COUNT																												\
	if( FixedDayCountId != SDB_DIDDLE_ID_ERROR )																										\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FixedDayCount", LT_DT_SET_ARG( FixedDayCountArg ), FixedDayCountId );	
#define LT_REMOVE_DIDDLE_FLOAT_DAY_COUNT																												\
	if( FloatDayCountId != SDB_DIDDLE_ID_ERROR )																										\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatDayCount", LT_DT_SET_ARG( FloatDayCountArg ), FloatDayCountId );	
#define LT_REMOVE_DIDDLE_FIRST_RESET																													\
	if( FirstResetId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FirstReset", LT_DT_SET_ARG( FirstResetArg ), FirstResetId );						
#define LT_REMOVE_DIDDLE_BUSINESS_DAYS																													\
	if( BusinessDaysId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "BusinessDays", LT_DT_SET_ARG( BusinessDaysArg ), BusinessDaysId );							
#define LT_REMOVE_DIDDLE_FLOAT_RATE																														\
	if( FloatRateId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatRate", LT_DT_SET_ARG( FloatRateArg ), FloatRateId );									
#define LT_REMOVE_DIDDLE_FIXED_START_STUB																												\
	if( FixedStartStubId != SDB_DIDDLE_ID_ERROR )																										\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FixedStartStubType", LT_DT_SET_ARG( FixedStartStubArg ), FixedStartStubId );				
#define LT_REMOVE_DIDDLE_FIXED_END_STUB																													\
	if( FixedEndStubId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FixedEndStubType", LT_DT_SET_ARG( FixedEndStubArg ), FixedEndStubId );						
#define LT_REMOVE_DIDDLE_FLOAT_START_STUB																												\
	if( FloatStartStubId != SDB_DIDDLE_ID_ERROR )																										\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatStartStubType", LT_DT_SET_ARG( FloatStartStubArg ), FloatStartStubId );				
#define LT_REMOVE_DIDDLE_FLOAT_END_STUB																													\
	if( FloatEndStubId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatEndStubType", LT_DT_SET_ARG( FloatEndStubArg ), FloatEndStubId );						
#define LT_REMOVE_DIDDLE_FIXED_ADJUST																													\
	if( FixedAdjustId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FixedAdjusted", LT_DT_SET_ARG( FixedAdjustArg ), FixedAdjustId );							
#define LT_REMOVE_DIDDLE_FLOAT_ADJUST																													\
	if( FloatAdjustId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatAdjusted", LT_DT_SET_ARG( FloatAdjustArg ), FloatAdjustId );							
#define LT_REMOVE_DIDDLE_FIXED_BUS_DAY_CONV																												\
	if( FixedBusDayConvId != SDB_DIDDLE_ID_ERROR )																										\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FixedBusinessDayConvention", LT_DT_SET_ARG( FixedBusDayConvArg ), FixedBusDayConvId );		
#define LT_REMOVE_DIDDLE_FLOAT_BUS_DAY_CONV																												\
	if( FloatBusDayConvId != SDB_DIDDLE_ID_ERROR )																										\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FloatBusinessDayConvention", LT_DT_SET_ARG( FloatBusDayConvArg ), FloatBusDayConvId );		
#define LT_REMOVE_DIDDLE_FIXED_FREQ_ZERO_COUPON																											\
	if( FixedFreqId != SDB_DIDDLE_ID_ERROR )																											\
		CC_MUTABLE( LtSwapCurve )->m_CurveObject.removeDiddle( "FixedNominalFreq",	LT_DT_SET_ARG( FixedFreqArg ), FixedFreqId );
						
CC_BEGIN_NAMESPACE( Gs )

class LtSecDbObj;
class EXPORT_CLASS_LTADDIN LtSwapCurve
{
	mutable LtSecDbObj m_CurveObject; // This is mutable so that I can apply/remove diddles

public:

	LtSwapCurve();
	LtSwapCurve( const GsString& CurveName, const GsDate& CurveDate );
	LtSwapCurve( const LtSecDbObj& SecDbObject );
	LtSwapCurve( const LtSwapCurve& Other );
	LtSwapCurve& operator=( const LtSwapCurve& Rhs );
	~LtSwapCurve();

	LtSwapCurve(
		const GsString& CurveName,          
		const GsRDate& DataDate,            
		const GsRDate& CurveDate,           
		const GsDtDictionary& TagsAndValues,
		const GsString& DataLocation,       
		bool AugmentTagsWithDbInstrs        
	    );

  	GsRDate defEffectiveDate() const; 			
	GsRDate defFixedFreq() const; 		
  	GsRDate defFloatFreq() const; 		
	GsRDate defFloatCompoundingFreq() const;
 	GsDayCount defFixedDayCount() const;
  	GsDayCount defFloatDayCount() const;	
  	double defFirstReset() const;
	GsString defBusinessDays() const;
	GsPeriod defZeroCouponPeriod() const;
  	//GsString defFixedBusDayConv() const;
  	//GsString defFloatBusDayConv() const;
	GsPeriod defDV01CompoundingFreq() const;

	GsDate getCurveDate() const;
	GsString getCurveName() const;
	GsDiscountCurve getDiscountCurve () const;
	GsDtDictionary* getTagsAndValues() const;
	GsDtDictionary* getMarketData() const;
	GsDate getDataDate() const;
	GsString getDataLocation() const;
	bool getAugmentTagsWithDbInstrs() const;

	GsDtDictionary* getInstrumentConventions(
		const GsString& Instrument,		
		const GsString& Key         		
	    ) const;

	double getTagValue(
		const GsString& Tag
    	) const;

	double getDiscountFactor( 
		const GsDate& TheDate 
	    ) const;

	double getSwapRate( 
		LT_SWAP_ARGS 
	    ) const;

	GsString getSwapInfo( 
		LT_SWAP_ARGS 
	    ) const;

	double getSwapPV(
		double CouponRate,                        
		LT_SWAP_ARGS 
	    ) const;

	double getSwapDV01ParYield(
		double CouponRate,
		enum LT_DV01_DIRECTION ShockDirection, 
		double ShockBp, 
		const GsPeriod& ParYieldCompoundingFreq, 
		LT_SWAP_ARGS
	    ) const;

	double getSwapDV01ZeroYield(
		double CouponRate,
		enum LT_DV01_DIRECTION ShockDirection, 
		double ShockBp, 
		const GsPeriod& ParYieldCompoundingFreq, 
		LT_SWAP_ARGS
	    ) const;

	double getSwapDV01Ifr(
		double CouponRate,
		enum LT_DV01_DIRECTION ShockDirection, 
		double ShockBp, 
		LT_SWAP_ARGS
	    ) const;

	GsDtDictionary* getSwapDV01ByInstrument(
		double CouponRate,
		enum LT_DV01_DIRECTION ShockDirection, 
		double ShockBp, 
		LT_SWAP_ARGS
	    ) const;

	double getSwapConvexityParYield(
		double CouponRate,
		enum LT_DV01_DIRECTION ShockDirection, 
		double ShockBp, 
		const GsPeriod& ParYieldCompoundingFreq, 
		LT_SWAP_ARGS
	    ) const;

	double getSwapConvexityZeroYield(
		double CouponRate,
		enum LT_DV01_DIRECTION ShockDirection, 
		double ShockBp, 
		const GsPeriod& ParYieldCompoundingFreq, 
		LT_SWAP_ARGS
	    ) const;

	double getSwapConvexityIfr(
		double CouponRate,
		enum LT_DV01_DIRECTION ShockDirection, 
		double ShockBp, 
		LT_SWAP_ARGS
	    ) const;

	GsDtDictionary* getSwapConvexityByInstrument(
		double CouponRate,
		enum LT_DV01_DIRECTION ShockDirection, 
		double ShockBp, 
		LT_SWAP_ARGS
	    ) const;

	double getAnnuityPv(
		const GsRDate& EffectiveDate,            		
		const GsRDate& Tenor,                    		
		const GsRDate& FixedFreq,              			
		const GsDayCount& FixedDayCount,     			
		const GsString& BusinessDays,               	
		enum LT_STUB_TYPE FixedStartStub,               
		enum LT_STUB_TYPE FixedEndStub,                 
		enum LT_ADJUSTED_ACCRUAL FixedAdjust,           
		enum LT_BUSINESS_DAY_CONVENTION FixedBusDayConv
	    ) const;

 	double getForwardRate(
		const GsRDate& EffectiveDate,            			
		const GsRDate& Tenor,                    			
		const GsDayCount& FloatDayCount,  				
		const GsString& BusinessDays,               		
		enum LT_BUSINESS_DAY_CONVENTION FloatBusDayConv
	    ) const;

 	double getZeroCouponRate(
		const GsRDate& EffectiveDate,            			
		const GsRDate& Tenor,                    			
		const GsPeriod& FixedFreq,
		const GsDayCount& FixedDayCount,     			
		const GsString& BusinessDays,               		
		enum LT_BUSINESS_DAY_CONVENTION FixedBusDayConv
 	    ) const;

	GsString defFloatRate(
	    ) const
	{	
		return GsString( "Default" );
		// static GsString DefaultString = GsString( "Default" );	
		// return DefaultString;
	}	

	

	GsString toString() const { return typeName(); }
	typedef GsDtGeneric<LtSwapCurve> GsDtType;
	inline static const char* typeName() { return "LtSwapCurve"; }

	static GsRDate defDataDate()				{	return GsRDate( new GsRDateLazyDate( GsString( "Close" ) ) );   				}
	static GsRDate defCurveDate()				{	return GsRDate( new GsRDateLazyDate( GsString( "t" ) ) );   					}
	static GsDtDictionary GetEmptyDictionary()	{	return GsDtDictionary(); 														}
};

/*
** TimeSeries Stuff
*/

typedef GsTimeSeries < GsTsHandle < LtSwapCurve > > LtSwapCurveTimeSeries;

/*
** Addin Functions
*/

EXPORT_CPP_LTADDIN LtSwapCurve* LtCurveCreate( 
    const GsString& CurveName,           
    const GsRDate& DataDate,             
    const GsRDate& CurveDate,            
    const GsDtDictionary& TagsAndValues, 
    const GsString& DataLocation,        
    bool AugmentTagsWithDbInstrs         
	);

EXPORT_CPP_LTADDIN GsDate LtCurveDate(
    const LtSwapCurve& Curve                  
	);

EXPORT_CPP_LTADDIN GsString LtCurveName(
    const LtSwapCurve& Curve                  
	);

EXPORT_CPP_LTADDIN double LtCurveTagValue(
    const LtSwapCurve& Curve,                 
    const GsString& Tag                       
	);

EXPORT_CPP_LTADDIN GsDtDictionary*  LtCurveTagsAndValues(
    const LtSwapCurve& Curve                  
	);

EXPORT_CPP_LTADDIN GsDtDictionary*  LtCurveMarketData(
    const LtSwapCurve& Curve                  
	);

EXPORT_CPP_LTADDIN GsDiscountCurve LtDiscountCurve(
    const LtSwapCurve& Curve                  
	);

EXPORT_CPP_LTADDIN double LtDiscountFactor(
    const LtSwapCurve& Curve,                 
    const GsDate& TheDate                     
	);

EXPORT_CPP_LTADDIN double LtSwapRate( 
	const LtSwapCurve& Curve, 
	LT_SWAP_ARGS_WITH_DICTIONARY 
    );

EXPORT_CPP_LTADDIN GsString LtSwapInfo( 
	const LtSwapCurve& Curve, 
	LT_SWAP_ARGS_WITH_DICTIONARY 
    );

EXPORT_CPP_LTADDIN double LtSwapPV(
    const LtSwapCurve& Curve,                 
    double CouponRate,                        
	LT_SWAP_ARGS_WITH_DICTIONARY  
	);

EXPORT_CPP_LTADDIN double LtSwapDV01ParYield(
    const LtSwapCurve& Curve,				  
    double CouponRate,						  
    const GsRDate& EffectiveDate,			  
    const GsRDate& Tenor,				      
    const GsRDate& FixedFreq,				  
    const GsRDate& FloatFreq,				  
    const GsRDate& FloatCompoundingFreq,	  
    const GsDayCount& FixedDayCount,		  
    const GsDayCount& FloatDayCount,		  
    double FirstReset,					      
	LT_DV01_DIRECTION ShockDirection,	  	  
	double ShockBp,		  					  
	const GsPeriod& ParYieldCompoundingFreq,  
    const GsDtDictionary& NamedArguments	  
    );

EXPORT_CPP_LTADDIN double LtSwapDV01ZeroYield(
    const LtSwapCurve& Curve,				  
    double CouponRate,						  
    const GsRDate& EffectiveDate,			  
    const GsRDate& Tenor,				      
    const GsRDate& FixedFreq,				  
    const GsRDate& FloatFreq,				  
    const GsRDate& FloatCompoundingFreq,	  
    const GsDayCount& FixedDayCount,		  
    const GsDayCount& FloatDayCount,		  
    double FirstReset,					      
	LT_DV01_DIRECTION ShockDirection,	  	  
	double ShockBp,		  					  
	const GsPeriod& ParYieldCompoundingFreq,  
    const GsDtDictionary& NamedArguments	  
    );

EXPORT_CPP_LTADDIN double LtSwapDV01Ifr(
    const LtSwapCurve& Curve,				  
    double CouponRate,						  
    const GsRDate& EffectiveDate,			  
    const GsRDate& Tenor,				      
    const GsRDate& FixedFreq,				  
    const GsRDate& FloatFreq,				  
    const GsRDate& FloatCompoundingFreq,	  
    const GsDayCount& FixedDayCount,		  
    const GsDayCount& FloatDayCount,		  
    double FirstReset,					      
	LT_DV01_DIRECTION ShockDirection,	  	  
	double ShockBp,		  					  
    const GsDtDictionary& NamedArguments	  
    );

EXPORT_CPP_LTADDIN GsDtDictionary* LtSwapDV01ByInstrument(
    const LtSwapCurve& Curve,				  
    double CouponRate,						  
    const GsRDate& EffectiveDate,			  
    const GsRDate& Tenor,				      
    const GsRDate& FixedFreq,				  
    const GsRDate& FloatFreq,				  
    const GsRDate& FloatCompoundingFreq,	  
    const GsDayCount& FixedDayCount,		  
    const GsDayCount& FloatDayCount,		  
    double FirstReset,					      
	LT_DV01_DIRECTION ShockDirection,	  	  
	double ShockBp,		  					  
    const GsDtDictionary& NamedArguments	  
    );

EXPORT_CPP_LTADDIN double LtSwapConvexityParYield(
    const LtSwapCurve& Curve,				  
    double CouponRate,						  
    const GsRDate& EffectiveDate,			  
    const GsRDate& Tenor,				      
    const GsRDate& FixedFreq,				  
    const GsRDate& FloatFreq,				  
    const GsRDate& FloatCompoundingFreq,	  
    const GsDayCount& FixedDayCount,		  
    const GsDayCount& FloatDayCount,		  
    double FirstReset,					      
	LT_DV01_DIRECTION ShockDirection,	  	  
	double ShockBp,		  					  
	const GsPeriod& ParYieldCompoundingFreq,  
    const GsDtDictionary& NamedArguments	  
    );

EXPORT_CPP_LTADDIN double LtSwapConvexityZeroYield(
    const LtSwapCurve& Curve,				  
    double CouponRate,						  
    const GsRDate& EffectiveDate,			  
    const GsRDate& Tenor,				      
    const GsRDate& FixedFreq,				  
    const GsRDate& FloatFreq,				  
    const GsRDate& FloatCompoundingFreq,	  
    const GsDayCount& FixedDayCount,		  
    const GsDayCount& FloatDayCount,		  
    double FirstReset,					      
	LT_DV01_DIRECTION ShockDirection,	  	  
	double ShockBp,		  					  
	const GsPeriod& ParYieldCompoundingFreq,  
    const GsDtDictionary& NamedArguments	  
    );

EXPORT_CPP_LTADDIN double LtSwapConvexityIfr(
    const LtSwapCurve& Curve,				  
    double CouponRate,						  
    const GsRDate& EffectiveDate,			  
    const GsRDate& Tenor,				      
    const GsRDate& FixedFreq,				  
    const GsRDate& FloatFreq,				  
    const GsRDate& FloatCompoundingFreq,	  
    const GsDayCount& FixedDayCount,		  
    const GsDayCount& FloatDayCount,		  
    double FirstReset,					      
	LT_DV01_DIRECTION ShockDirection,	  	  
	double ShockBp,		  					  
    const GsDtDictionary& NamedArguments	  
    );

EXPORT_CPP_LTADDIN GsDtDictionary* LtSwapConvexityByInstrument(
    const LtSwapCurve& Curve,				  
    double CouponRate,						  
    const GsRDate& EffectiveDate,			  
    const GsRDate& Tenor,				      
    const GsRDate& FixedFreq,				  
    const GsRDate& FloatFreq,				  
    const GsRDate& FloatCompoundingFreq,	  
    const GsDayCount& FixedDayCount,		  
    const GsDayCount& FloatDayCount,		  
    double FirstReset,					      
	LT_DV01_DIRECTION ShockDirection,	  	  
	double ShockBp,		  					  
    const GsDtDictionary& NamedArguments	  
    );

EXPORT_CPP_LTADDIN double LtAnnuityPv(
    const LtSwapCurve& Curve,                 
    const GsRDate& EffectiveDate,				
    const GsRDate& Tenor,						
    const GsRDate& FixedFreq,					
    const GsDayCount& FixedDayCount,			
    const GsDtDictionary& NamedArguments
	);

EXPORT_CPP_LTADDIN double LtForwardRate(
    const LtSwapCurve& Curve,                 
    const GsRDate& EffectiveDate,            			
    const GsRDate& Tenor,                    			
    const GsDayCount& FloatDayCount,  				
    const GsString& BusinessDays,               		
    enum LT_BUSINESS_DAY_CONVENTION FloatBusDayConv
    );

EXPORT_CPP_LTADDIN double LtZeroCouponRate(
    const LtSwapCurve& Curve,                 
    const GsRDate& EffectiveDate,            			
    const GsRDate& Tenor,                    			
	const GsPeriod& FixedFreq,
    const GsDayCount& FixedDayCount,     			
    const GsString& BusinessDays,               		
    enum LT_BUSINESS_DAY_CONVENTION FixedBusDayConv	
	);

EXPORT_CPP_LTADDIN double LtSwap100BpAnnuityPv(
	const GsDiscountCurve& DfCurve,
	const GsTermCalc& AnnuityTc,
	const GsDateVector& AnnuityPayDateVec,
	const GsDateVector& AnnuityAccrualDateVec, 
	const GsDate& TcReferenceDate,				
	double TcFreq				
    );

EXPORT_CPP_LTADDIN GsRDate LtUseRt(
	double IsVolatile
    );

EXPORT_CPP_LTADDIN GsDate LtCurveDataDate(
    const LtSwapCurve& Curve                  
    );

EXPORT_CPP_LTADDIN GsString LtCurveDataLocation(
    const LtSwapCurve& Curve                  
    );

EXPORT_CPP_LTADDIN bool LtCurveAugmentTagsWithDbInstrs(
    const LtSwapCurve& Curve                  
    );

EXPORT_CPP_LTADDIN GsDtDictionary* LtInstrumentConventions(
    const LtSwapCurve& Curve,                 
	const GsString& Instrument,		
	const GsString& Key         		
    );

EXPORT_CPP_LTADDIN double LtSwapCompoundFloatPv(
	const GsDiscountCurve& DfCurve,		
	const GsDateVector& PayDateVec,		
	const GsDateVector& AccrualDateVec,	
	const GsDateGen& RateStartDg,		
	const GsPeriod& RateTenor,			
	const GsDateGen& RateTenorDg,		
	const GsTermCalc& RateTc,			
	double RateSpreadBp,				
	double FirstReset,					
	const GsTermCalc& PaymentTc,		
	int CompTermsPerPayTerm,			
	const GsDate& TcReferenceDate,				
	double TcFreq						
    );

EXPORT_CPP_LTADDIN GsRDate LtDefaultEffectiveDate( const LtSwapCurve& Curve );
EXPORT_CPP_LTADDIN GsRDate LtDefaultFixedFreq( const LtSwapCurve& Curve );
EXPORT_CPP_LTADDIN GsRDate LtDefaultFloatFreq( const LtSwapCurve& Curve );
EXPORT_CPP_LTADDIN GsRDate LtDefaultFloatCompoundingFreq( const LtSwapCurve& Curve );
EXPORT_CPP_LTADDIN GsDayCount LtDefaultFixedDayCount( const LtSwapCurve& Curve );
EXPORT_CPP_LTADDIN GsDayCount LtDefaultFloatDayCount( const LtSwapCurve& Curve );
EXPORT_CPP_LTADDIN GsString LtDefaultBusinessDays( const LtSwapCurve& Curve );
//EXPORT_CPP_LTADDIN GsString LtDefaultFixedBusDayConv( const LtSwapCurve& Curve );
//EXPORT_CPP_LTADDIN GsString LtDefaultFloatBusDayConv( const LtSwapCurve& Curve );

CC_END_NAMESPACE

#endif



