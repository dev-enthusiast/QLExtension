/* $Header: /home/cvs/src/quant/src/q_smile.h,v 1.40 2000/05/02 14:28:02 demeor Exp $ */
/****************************************************************
**
**	Q_SMILE.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.40 $
**
****************************************************************/

#if !defined( IN_Q_SMILE_H )
#define IN_Q_SMILE_H

#define E(Mat,Row,Col) (Mat->M[(Mat->Cols)*(Col+Row)+Col])
#define El(Mat,Row,Col) (Mat.M[(Mat.Cols)*(Row)+Col])
#define maxof(A,B) (A>B ? A : B)

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#include <num.h>

EXPORT_C_QWINGS double InterpolateVolStrike(
	Q_MATRIX	*vol,			/* volitility matrix */
	Q_VECTOR	*strikes,		/* strikes */
	long int	j,				/* interpolate along which time (j) row */
	double		strike			/* the strike price */	
);


EXPORT_C_QWINGS double InterpolateDiscount(
Q_VECTOR	*z,
Q_VECTOR	*ratetimes,
double  	time
);

EXPORT_C_QWINGS double Q_KODSmile (
    Q_OPT_TYPE		callPut,            // option type
    Q_OPT_STYLE     style,              // European or American
    double			spot,               // current spot price
    double			strike,             // strike price
    double			lowerKO,            // lower knockout level
    double          upperKO,            // upper knockout level
    double			lowerRebate,        // rebate upon lower knockout
    double			upperRebate,        // rebate upon upper knockout
    Q_REBATE_TYPE	rebType,            // immediate or delayed rebate
    Q_MATRIX		*strikes,           // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
    Q_MATRIX		*smiles,            // matrix containing volatility surface
    double			treevol,            // volatility used for setting up tree
    DATE			prcDate,            // pricing date
    DATE			expDate,            // expiration date
    DT_CURVE		*fwdcurve,          // forward curve
    DT_CURVE		*ratecurve,         // domestic rate curve
    int				nsteps              // number of timesteps in the tree
    );

EXPORT_C_QWINGS double Q_CompoundSmile (
    Q_OPT_TYPE		callPut1,            // option type
    Q_OPT_TYPE      callPut,            // type of underlying optin
    Q_OPT_STYLE     style1,             // European or American
    Q_OPT_STYLE     style,              // style of underlying optin
    double			spot,               // current spot price
    double			strike1,            // strike price
    double			strike,             // strike price	of underlying option
    Q_MATRIX		*strikes,           // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
    Q_MATRIX		*smiles,            // matrix containing volatility surface
    double			treevol,            // volatility used for setting up tree
    DATE			prcDate,            // pricing date
    DATE			cmpdExp,            // expiration date
    DATE			expDate,            // expiration of underlying option
    DT_CURVE		*fwdcurve,          // forward curve
    DT_CURVE		*ratecurve,         // domestic rate curve
    int				nsteps              // number of timesteps in the tree
    );

EXPORT_C_QWINGS double Q_KOPDSmile (
    Q_OPT_TYPE		callPut,            // option type
    Q_OPT_STYLE     style,              // European or American
    double			spot,               // current spot price
    double			strike,             // strike price
    double			lowerKO,            // lower knockout level
    double          upperKO,            // upper knockout level
    double			lowerRebate,        // rebate upon lower knockout
    double			upperRebate,        // rebate upon upper knockout
    Q_REBATE_TYPE	rebType,            // immediate or delayed rebate
    Q_MATRIX		*strikes,           // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
    Q_MATRIX		*smiles,            // matrix containing volatility surface
    double			treevol,            // volatility used for setting up tree
    DATE			prcDate,            // pricing date
    DATE			startKO,            // start of knockout period
    DATE			endKO,              // end of knockout period
    DATE			expDate,            // expiration date
    DT_CURVE		*fwdcurve,          // forward curve
    DT_CURVE		*ratecurve,         // domestic rate curve
    int				nsteps              // number of timesteps in the tree
    );
        
EXPORT_C_QWINGS double Q_OptPortOpt1Asset1TenorTrees(
    Q_OPT_TYPE		callPut,               // option type
    double			spot,                  // current spot price
    double			strike,                // strike price
	Q_VECTOR        *UnderlyingQuantities, // quantities of underlying options
	Q_VECTOR        *UnderlyingTypes,      // option type of underlying as doubles
	Q_VECTOR        *Underlyingstrikes,    // strikes of underlying options
    Q_MATRIX		*strikes,              // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,             // nodes for vol smile surface
    Q_MATRIX		*smiles,               // matrix containing volatility surface
    double			treevol,               // volatility used for setting up tree
    DATE			prcDate,               // pricing date
    DATE			cmpdExp,               // expiration date
    DATE			expDate,               // expiration of underlying option
    DATE			sttlDate,              // settlement of underlying option
    DT_CURVE		*fwdcurve,             // forward curve
    DT_CURVE		*discCurve,            // domestic discount curve
    int				nTimeSteps             // number of timesteps in the tree
);


EXPORT_C_QWINGS double Q_CreditBond (
    double			spot,               // current spot price
	DT_CURVE        *cfcurve, 			// curve of cash flows that make up the bond
    Q_MATRIX		*strikes,           // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
    Q_MATRIX		*smiles,            // matrix containing volatility surface
    double			treevol,            // volatility used for setting up tree
    DATE			prcDate,            // pricing date
    DATE			expDate,            // date of last cash flow
    DT_CURVE		*fwdcurve,          // forward curve
    DT_CURVE		*ratecurve,         // domestic rate curve
	DT_CURVE        *refcurve,			// reference spots for computing default probabilities
    Q_FN_TYPE		fntype,				// function type
	DT_CURVE        *alpha,             // constant coefficient of default probability - setp up level
    double          beta,           	// coefficient of spot/fwd in default probability
	double			elbow,				// elbow function parameter
    double			recovery,           // how much you get per dollar if other party defaults
    int				nsteps,             // number of timesteps in the tree
    int				denomtype           // 1 if tree in terms of hard ccy / soft ccy (debt denom in hard ccy), otherwise -1
    );

EXPORT_C_QWINGS double Q_CreditFX (
	Q_OPT_TYPE		tradetype,			// type of trade, Q_FWD, Q_OPT_CALL or Q_OPT_PUT
    double			spot,               // current spot price
    double			strike,             // strike price
    double          buysell,            // GS buys (1) or Sells (-1) Denominator
    Q_MATRIX		*strikes,           // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
    Q_MATRIX		*smiles,            // matrix containing volatility surface
    double			treevol,            // volatility used for setting up tree
    DATE			prcDate,            // pricing date
    DATE			expDate,            // expiration date
    DT_CURVE		*fwdcurve,          // forward curve
    DT_CURVE		*ratecurve,         // domestic rate curve
	DT_CURVE        *refcurve,			// reference spots for computing default probabilities
    Q_FN_TYPE		fntype,				// function type
	DT_CURVE        *alpha,             // constant coefficient of default probability - setp up level
    double          beta,           	// coefficient of spot/fwd in default probability
	double			elbow,				// elbow function parameter
    double			recovery,           // how much you get per dollar if other party defaults
    double			margintrigger,      // margin trigger denominated in over ccy
    int				nsteps,             // number of timesteps in the tree
    int				denomtype           // 1 if tree in terms of hard ccy / soft ccy (debt denom in hard ccy), otherwise -1
    );
	
EXPORT_C_QWINGS double Q_TrdPNLOptSmile (
	double			spot,               // current spot price
	double			pnl0,				// initial PNL in denominated currency
	Q_MATRIX		*strikes,           // strikes for vol smile surface
	Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
	Q_MATRIX		*smiles,            // matrix containing volatility surface
	double			treevol,            // volatility used for setting up tree
	DATE			prcDate,            // pricing date
	DATE			expDate,            // expiration date
	DT_CURVE		*fwdcurve,          // forward curve
	DT_CURVE		*ratecurve,         // interest rate curve for denominated currency
	int				nsteps,             // number of timesteps in the tree
	int				flags		        // flags for optional behaviour
    );

EXPORT_C_QWINGS double Q_AmortSmile (
    Q_OPT_TYPE		callPut,            // option type for each cashflow
    double			spot,               // current spot price
    DT_CURVE		*strike,            // strike curve for cashflows
	Q_VECTOR        *Amounts,       	// number of ounces per payment
    double			lowerKO,            // lower knockout level
    double          upperKO,            // upper knockout level
	Q_VECTOR        *LowPrice,      	// low price for 100% amortization
	Q_VECTOR        *MidPrice,      	// mid price for some middle amortization
	Q_VECTOR        *HighPrice,     	// high price for 0% amortization
	Q_VECTOR        *LowPct,        	// percentage at lower level
    Q_VECTOR        *MidPct,        	// percentage of amortization middle level
    Q_VECTOR        *HighPct,       	// percentage at upper level
    Q_MATRIX		*strikes,           // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
    Q_MATRIX		*smiles,            // matrix containing volatility surface
    double			treevol,            // volatility used for setting up tree
    DATE			prcDate,            // pricing date
    Q_DATE_VECTOR   *AmortDates, 	 	// amortization dates
    DATE			startKO,            // start of knockout period
    DATE			endKO,              // end of knockout period
    DT_CURVE		*fwdcurve,          // forward curve
    DT_CURVE		*ratecurve,         // domestic rate curve
    int				nsteps              // number of timesteps in the tree
    );
	
EXPORT_C_QWINGS double Q_AmortFloatSmile (
    double			spot,               // current spot price
    double      	basefloat, 		    // to-date accumulation of floating payments
    DT_CURVE		*strike,            // strike curve for cashflows
	Q_VECTOR        *Amounts,       	// number of ounces per payment
    double			lowerKO,            // lower knockout level
    double          upperKO,            // upper knockout level
	Q_VECTOR        *LowPrice,      	// low price for 100% amortization
	Q_VECTOR        *MidPrice,      	// mid price for some middle amortization
	Q_VECTOR        *HighPrice,     	// high price for 0% amortization
	Q_VECTOR        *LowPct,        	// percentage at lower level
    Q_VECTOR        *MidPct,        	// percentage of amortization middle level
    Q_VECTOR        *HighPct,       	// percentage at upper level
    Q_MATRIX		*strikes,           // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
    Q_MATRIX		*smiles,            // matrix containing volatility surface
    double			treevol,            // volatility used for setting up tree
    DATE			prcDate,            // pricing date
    Q_DATE_VECTOR   *AmortDates, 	 	// amortization dates
    DATE			startKO,            // start of knockout period
    DATE			endKO,              // end of knockout period
    DT_CURVE		*fwdcurve,          // forward curve
    DT_CURVE		*ratecurve,         // domestic rate curve
    int				nsteps              // number of timesteps in the tree
    );

EXPORT_C_QWINGS double Q_OptSmileGen (
    double			spot,               // current spot price
	N_GENERIC_FUNC  payoff,             // arbitrary payoff function
	void            *context,           // context for payoff function
    Q_MATRIX		*strikes,           // strikes for vol smile surface
    Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
    Q_MATRIX		*smiles,            // matrix containing volatility surface
    double			treevol,            // volatility used for setting up tree
    DATE			prcDate,            // pricing date
    DATE			expDate,            // expiration date
    DT_CURVE		*fwdcurve,          // forward curve
    DT_CURVE		*ratecurve,         // domestic rate curve
    int				nsteps              // number of timesteps in the tree
    );
	  
EXPORT_C_QWINGS double Q_OptPortOptSmile (
	Q_OPT_TYPE		callPut,            // option type for compound option
	Q_VECTOR        *types,             // option types for underlying options
	double			spot,               // current spot price
	double          cmpdStrike,         // strike for compound option
	Q_VECTOR		*strike,            // strikes for underlying options
	Q_DATE_VECTOR	*paydates,          // expiration dates of underlying options
	Q_VECTOR        *Amounts,       	// notional amounts on underlying options
	Q_MATRIX		*strikes,           // strikes for vol smile surface
	Q_DATE_VECTOR	*voldates,          // nodes for vol smile surface
	Q_MATRIX		*smiles,            // matrix containing volatility surface
	double			treevol,            // volatility used for setting up tree
	DATE			prcDate,            // pricing date
	DATE			expDate,            // expiration date
	DT_CURVE		*fwdcurve,          // forward curve
	DT_CURVE		*ratecurve,         // domestic rate curve
	int				nsteps              // number of timesteps in the tree
    );
	  
#endif
