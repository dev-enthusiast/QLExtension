/****************************************************************
**
**	gsquant/enums.h	- Enums for gsquant
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/Enums.h,v 1.46 2001/10/03 13:33:39 deeka Exp $
**
****************************************************************/

#ifndef IN_GSQUANT_ENUMS_H
#define IN_GSQUANT_ENUMS_H

#include	<qenums.h>
#include	<gscore/GsEnum.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	When adding enums to this file:
**	  - First, check if the enum is already in qenums.h and use that
**	  - Next, check if the enum is already in qtypes.h.  If so, work
**		with CORE to move that enum to qenums.h.  NEVER directly
**		include qtypes.h in this file because qtypes.h has lots of
**		extra stuff.
**	  - Finally, if you need to create your own:
**		1. Name it well
**		2. Give all of the constants the same prefix
**		3. Keep in mind that this is a global file.
**	
**	Be sure to add a call to GsEnumRegister() in an appropriate place
**	(probably gsquant/enums.cpp for things added here).
*/


/*
**	A bunch of enums from qenums.h.  We have to GS_ENUM_DECLARE() here
**	because qenums.h is still C and can't know about the C++ template
**	declared by GS_ENUM_DECLARE(), nor can it depend on gscore/GsEnum.h
*/

GS_ENUM_DECLARE( Q_OPT_TYPE );
GS_ENUM_DECLARE( Q_OPT_STYLE );
GS_ENUM_DECLARE( Q_IN_OUT );
GS_ENUM_DECLARE( Q_FADEOUT_TYPE );
GS_ENUM_DECLARE( Q_REBATE_TYPE );
GS_ENUM_DECLARE( Q_RETURN_TYPE );
GS_ENUM_DECLARE( Q_OPT_ON );
GS_ENUM_DECLARE( Q_MARGIN );
GS_ENUM_DECLARE( GsInterpolation );
GS_ENUM_DECLARE( Q_UP_DOWN );

/*
**	Use adjusted or unajusted accrual dates when computing accrual terms?
*/

enum LT_DV01_DIRECTION
{
	LT_DV01D_UP,
	LT_DV01D_DOWN,
	LT_DV01D_CENTER,
	LT_DV01D_DEFAULT
};

GS_ENUM_DECLARE( LT_DV01_DIRECTION );

//  enum LT_ADJUSTED_ACCRUAL
//  {
//  	LT_AA_DEFAULT,
//  	LT_AA_ADJUSTED,
//  	LT_AA_UNADJUSTED
//  };

//  GS_ENUM_DECLARE( LT_ADJUSTED_ACCRUAL );


/*
**	For example, is the SwapPv w.r.t. paying or receiving fixed?
*/

enum LT_PAY_OR_RECEIVE
{
	LT_PR_DEFAULT,
	LT_PR_PAY,
	LT_PR_RECEIVE
};

GS_ENUM_DECLARE( LT_PAY_OR_RECEIVE );


/*
**	Enumerated business day conventions
*/

//  enum LT_BUSINESS_DAY_CONVENTION
//  {
//  	LT_BDC_DEFAULT,
//  	LT_BDC_FOLLOWING,
//  	LT_BDC_MODIFIED_FOLLOWING,
//  	LT_BDC_PREVIOUS,
//  	LT_BDC_UNADJUSTED
//  };

//  GS_ENUM_DECLARE( LT_BUSINESS_DAY_CONVENTION );

//  enum LT_STUB_TYPE
//  {
//  	LT_ST_DEFAULT,
//  	LT_ST_SHORT,
//  	LT_ST_LONG,
//  	LT_ST_FULL
//  };

//  GS_ENUM_DECLARE( LT_STUB_TYPE );

//  enum LT_INCLUDE_END
//  {
//  	LT_IE_DEFAULT,		
//  	LT_IE_ALWAYS,			
//  	LT_IE_NEVER			
//  };
 
// GS_ENUM_DECLARE( LT_INCLUDE_END );

/*
**	Enumerated Stub type
*/

enum LT_STUB
{
	LT_STUB_DEFAULT,
	LT_STUB_FULL_FIRST_CPN
};

GS_ENUM_DECLARE( LT_STUB );

/*
**	Enumerated Compouding type
*/

enum LT_COMPOUNDING
{
	LT_COMPOUNDING_NOT_APPLICABLE,
	LT_COMPOUNDING_APPLICABLE,
	LT_COMPOUNDING_FLAT
};

GS_ENUM_DECLARE( LT_COMPOUNDING );

/*
**	Enumerated Interest Rate Method
**  (How do we treat negative interest rates?)
*/

enum LT_IRM
{
	LT_IRM_NEGATIVE,
	LT_IRM_ZERO
};

GS_ENUM_DECLARE( LT_IRM );

/*
**	Enumerated Payment Date Specification
*/

enum LT_PAYMENT
{
	LT_PAYMENT_DELAYED,
	LT_PAYMENT_EARLY
};

GS_ENUM_DECLARE( LT_PAYMENT );

/*
**	Enumerated Rounding Method
*/

enum LT_ROUNDING
{
	LT_ROUNDING_ROUND,
	LT_ROUNDING_FLOOR,
	LT_ROUNDING_CEILING
};

GS_ENUM_DECLARE( LT_ROUNDING );

/*
**	Enumerated Roll Method
*/

enum LT_ROLL
{
	LT_ROLL_DEFAULT,
	LT_ROLL_IMM,
	LT_ROLL_FRN,
	LT_ROLL_EOM
};

GS_ENUM_DECLARE( LT_ROLL );

/*
**	Enumerated Rounding Method
*/

enum LT_INTERP
{
	LT_INTERP_NOT_APPLICABLE,
	LT_INTERP_LINEAR_APPLICABLE
};

GS_ENUM_DECLARE( LT_INTERP );

/*
**	Enumerated Accrual Method
*/

enum LT_ACCRUAL
{
	LT_ACCRUAL_ADJUSTED,
	LT_ACCRUAL_UNADJUSTED
};

GS_ENUM_DECLARE( LT_ACCRUAL );


/*
**	Enumerated Swaption Settlement Method
*/

enum LT_SWO_SETTLEMENT
{
	LT_SWO_SETTLEMENT_CASH,
	LT_SWO_SETTLEMENT_PHYSICAL
};

GS_ENUM_DECLARE( LT_SWO_SETTLEMENT );

/*
**	Enumerated Swaption Option types
*/

enum LT_SWO_OPTION
{
	LT_SWO_PAYER,
	LT_SWO_RECEIVER,
	LT_SWO_STRADDLE
};

GS_ENUM_DECLARE( LT_SWO_OPTION );

/*
**	Enumerated Cash Settlement Method
*/

enum LT_CASH_SETTLEMENT
{
	LT_CASH_CASH_PRICE,
	LT_CASH_PAR_YIELD_CURVE_ADJ,
	LT_CASH_ZERO_COUPON_YIELD_ADJ,
	LT_CASH_PAR_YIELD_CURVE_UNADJ
};

GS_ENUM_DECLARE( LT_CASH_SETTLEMENT );

/*
**	Enumerated Partial Exercise
*/

enum LT_PARTIAL_EXERCISE
{
	LT_PARTIAL_APPLICABLE,
	LT_PARTIAL_INAPPLICABLE
};

GS_ENUM_DECLARE( LT_PARTIAL_EXERCISE );

/*
**	Enumerated Settlement Rate
*/

enum LT_SETTLEMENT_RATE
{
	LT_SETTLEMENT_ISDA,
	LT_SETTLEMENT_OTHER,
	LT_SETTLEMENT_REF
};

GS_ENUM_DECLARE( LT_SETTLEMENT_RATE );


/*
**	Enumerated Reset types
*/

enum LT_RESET
{
	LT_RESET_IN_ADVANCE,
	LT_RESET_IN_ARREARS
};

GS_ENUM_DECLARE( LT_RESET );

/*
**	Enumerated Confirm types
*/

enum LT_CONFIRM_PROTOCOL
{
	LT_CONFIRM_ISDA,
	LT_CONFIRM_FRABBA
};

GS_ENUM_DECLARE( LT_CONFIRM_PROTOCOL );

/*
**	Enumerated Volatility types
*/

enum LT_VOL_TYPE
{
	LT_VOL_CONSTANT,
	LT_VOL_CURVE
};

GS_ENUM_DECLARE( LT_VOL_TYPE );


/*
**	Calibration Instruments
**	SeeAlso: FQ_INSTRUMENT_TYPE
*/

enum FQ_CALIB_INSTR
{
	FQ_CI_CAP,
	FQ_CI_FLOOR,
	FQ_CI_SWAPTION
};

GS_ENUM_DECLARE( FQ_CALIB_INSTR );


/*
**	Lots of Instruments
**	SeeAlso: FQ_CALIB_INSTR
*/

enum FQ_INSTRUMENT
{
	FQ_I_CASHFLOW,
	FQ_I_SIMPLE_OPTION,
	FQ_I_CAP,
	FQ_I_FLOOR,
	FQ_I_SWAPTION
};

GS_ENUM_DECLARE( FQ_INSTRUMENT );


/*
**	probability measure types
*/

enum FQ_MEASURE
{
	FQ_RISK_NEUTRAL_MEASURE,
	FQ_FORWARD_MEASURE,
	FQ_ANNUITY_MEASURE
};

GS_ENUM_DECLARE( FQ_MEASURE );


/* 
**	FQCREDIT model types
*/

enum FQCREDIT_MODELTYPES
{
	FQCREDIT_NORMAL    = 1, 
	FQCREDIT_LOGNORMAL = 2
};

GS_ENUM_DECLARE( FQCREDIT_MODELTYPES );

/* 
**	FQCREDIT coupon types
*/

enum FQCREDIT_COUPONTYPES
{
	FQCREDIT_FIX = 0,
	FQCREDIT_FLOAT = 1
};

GS_ENUM_DECLARE (FQCREDIT_COUPONTYPES );

/* 
**	FQCREDIT recovery types
*/

enum FQCREDIT_RECOVERYTYPES
{
	FQCREDIT_PPV = 1,
	FQCREDIT_PFA = 2
};

GS_ENUM_DECLARE( FQCREDIT_RECOVERYTYPES );

/* 
**	FQCREDIT recovery payment methods
*/

enum FQCREDIT_RECOVERYPAYMETHODS
{
	FQCREDIT_PRAPE = 0,
	FQCREDIT_PRI   = 1
};

GS_ENUM_DECLARE( FQCREDIT_RECOVERYPAYMETHODS );

/* 
**	FQCREDIT accrued fee payment methods
*/

enum FQCREDIT_ACCRUEFEEMETHODS
{
	FQCREDIT_APD  = 0,
	FQCREDIT_ANPD = 1
};

GS_ENUM_DECLARE( FQCREDIT_ACCRUEFEEMETHODS );

/* 
**	FQCREDIT curve types
*/

enum FQCREDIT_CURVETYPES
{
	FQCREDIT_CONSTANT   = 0,
	FQCREDIT_STEP       = 1,
	FQCREDIT_STEPSMOOTH = 2,
    FQCREDIT_QUADRATIC  = 3
};

GS_ENUM_DECLARE( FQCREDIT_CURVETYPES );

/* 
**	FQCREDIT special function types
*/

enum FQCREDIT_SPECIALFUNCTYPES
{
	FQCREDIT_NOSPECIAL       = 0,
	FQCREDIT_EXPONENTIAL     = 1,
    FQCREDIT_EXPONENTIALREPO = 2
};

GS_ENUM_DECLARE( FQCREDIT_SPECIALFUNCTYPES );

/*
**	Published rate types.
*/

enum FQ_PUBRATE_TYPES
{
	FQ_PUBRATE_SIMPLE_RATE = 1,
	FQ_PUBRATE_SWAP_RATE,
	FQ_PUBRATE_SWAPTION,
	FQ_PUBRATE_CAP
};
 
GS_ENUM_DECLARE( FQ_PUBRATE_TYPES );


/*
**	Stat by col or row?
*/

enum FQ_STAT_BY
{
	FQ_BY_COL,
	FQ_BY_ROW
};

GS_ENUM_DECLARE( FQ_STAT_BY );


/*
**	Published statistics type
*/

enum FQ_STAT_TYPES
{
	FQ_STAT_MEAN,
	FQ_STAT_MEDIAN,
	FQ_STAT_SUM,
	FQ_STAT_MAX,
	FQ_STAT_MIN,
	FQ_STAT_COUNT,
	FQ_STAT_STDDEV
};

GS_ENUM_DECLARE( FQ_STAT_TYPES );

/*
**	Published matrix decomposition method for generalized inverse
*/

enum FQ_GEN_INVERSE_METHOD
{
	FQ_SVD,
	FQ_QR
};

GS_ENUM_DECLARE( FQ_GEN_INVERSE_METHOD );

enum FQ_SWAP_MINMAX_TYPE
{
	FQ_SWAP_MINMAX_OFF,
	FQ_SWAP_MINMAX_RATE,
	FQ_SWAP_MINMAX_FORMULA
};

GS_ENUM_DECLARE( FQ_SWAP_MINMAX_TYPE );

enum LT_STRIKE_TYPE
{
	LT_STRIKE_ATM,
	LT_STRIKE_SPECIFIED
};

GS_ENUM_DECLARE( LT_STRIKE_TYPE );


enum LT_PARAM_FUNC_TYPE
{
	LT_PARAM_FUNC_CONSTANT = 1,
	LT_PARAM_FUNC_PIECEWISE_CONSTANT,
	LT_PARAM_FUNC_BSPLINE
};

GS_ENUM_DECLARE( LT_PARAM_FUNC_TYPE );

enum LT_NUMERICAL_METHOD
{
	LT_NM_BACKWARD_INDUCTION = 1,
	LT_NM_MONTE_CARLO
};

GS_ENUM_DECLARE( LT_NUMERICAL_METHOD );


enum GSXML_PARSE_FLAG
{
	GSXML_STRIP_FIELD_WHITESPACE = 1,
	GSXML_NONVALIDATING = 2,
	GSXML_VALIDATING = 4
};

GS_ENUM_DECLARE( GSXML_PARSE_FLAG );


enum ASYNC_PRIORITY
{
	ASYNC_PRIORITY_LOWEST = 1,
	ASYNC_PRIORITY_LOW,
	ASYNC_PRIORITY_NORMAL,
	ASYNC_PRIORITY_HIGH,
	ASYNC_PRIORITY_HIGHEST
};

GS_ENUM_DECLARE( ASYNC_PRIORITY );

enum LT_EARLY_TERM
{
	LT_EARLY_TERM_NONE,
	LT_EARLY_TERM_MANDATORY,
	LT_EARLY_TERM_OPTIONAL
};

GS_ENUM_DECLARE( LT_EARLY_TERM );

enum LT_OPTION_EXERCISE_STYLE
{
	LT_OPTION_EUROPEAN,
	LT_OPTION_BERMUDA,
	LT_OPTION_AMERICAN
};

GS_ENUM_DECLARE( LT_OPTION_EXERCISE_STYLE );

enum LT_OPTIONAL_TERMINATION_GRANTOR
{
	LT_GRANTOR_BOTH,
	LT_GRANTOR_GS,
	LT_GRANTOR_CP
};

enum EQSP_AVERAGING_TYPE
{
	EQSP_AVERAGING_NONE,
	EQSP_AVERAGING_ONEACH,
	EQSP_AVERAGING_ONFIRST,
	EQSP_AVERAGING_ONLAST
};

GS_ENUM_DECLARE( EQSP_AVERAGING_TYPE );

enum EQSP_SUBSEQUENT_AVERAGING_TYPE
{
	EQSP_SA_ONEACH,
	EQSP_SA_ONFIRST,
	EQSP_SA_ONLAST
};

GS_ENUM_DECLARE( EQSP_SUBSEQUENT_AVERAGING_TYPE );

enum EQSP_AVERAGING_CONVENTION
{
	EQSP_AC_FOLLOWING,
	EQSP_AC_FOLLOWINGINCLUDING,
	EQSP_AC_OF,                        
	EQSP_AC_PRECEEDING,
	EQSP_AC_PRECEEDINGINCLUDING
};


GS_ENUM_DECLARE( EQSP_AVERAGING_CONVENTION );

CC_END_NAMESPACE
#endif











