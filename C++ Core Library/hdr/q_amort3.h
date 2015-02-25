/****************************************************************
**
**	Q_AMORT3.H	- routines that don't fit in q_amort.c
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.44 $
**
****************************************************************/

#if !defined( IN_Q_AMORT3_H )
#define IN_Q_AMORT3_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QAMORT double Q_OptAmortDoubleFloat (
	Q_OPT_TYPE      callPut,        // option type of swaption
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	double 			Strike,	 		// strike of swaption
    double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	Q_VECTOR        *AmountsL,      // Amounts that lease payments are based on
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    DATE            exDate,         // exercise date of the option
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_AmortKOStrip(
	Q_OPT_TYPE		callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
	Q_VECTOR        *MidPct,        // percentage of amortization middle level
	Q_VECTOR        *HighPct,       // percentage at upper level
	DT_CURVE 		*vol,			// volatility curve
	double          rangevol,       // volatility used to calculate the range of integration
	DATE            prcDate,        // pricing date
	Q_DATE_VECTOR   *AmortDates,  	// amortization times
	DATE            koDate,         // start of knockout period
	DATE            endko,          // end of knockout period
	DT_CURVE 		*RdCurve,  		// domestic rate curve
	DT_CURVE 		*fwdCurve,   	// forward curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
	);

EXPORT_C_QAMORT double Q_AmortAdjFloatSwap (
	Q_UP_DOWN       upOrDown,       // knockout direction
	int             adjFlag,        // 0 if multiplicative, 1 if we subtract adjustment
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *LoLeasePrc,    // low spot price for lease adjustment
	Q_VECTOR        *HiLeasePrc,    // high spot price for lease adjustment
	Q_VECTOR        *LoLeasePct,    // lease adjustment at low spot
	Q_VECTOR        *HiLeasePct,    // lease adjustment at high spot
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_AmortAdjFloatSwapMC (
	Q_UP_DOWN       upOrDown,       // knockout direction
	int             adjFlag,        // 0 if multiplicative, 1 if we subtract adjustment
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *LoLeasePrc,    // low spot price for lease adjustment
	Q_VECTOR        *HiLeasePrc,    // high spot price for lease adjustment
	Q_VECTOR        *LoLeasePct,    // lease adjustment at low spot
	Q_VECTOR        *HiLeasePct,    // lease adjustment at high spot
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );
    	
EXPORT_C_QAMORT double	Q_AmortMaturityOptionMC(
	 Q_OPT_TYPE		OptionType,		    // Q_OPT_CALL, Q_OPT_PUT, or Q_OPT_FWD
	 double 		Strike,	 		    // Option strike price
	 DATE			PricingDate,		// Pricing Date
	 DATE			FirstFixingDate,	// First Fixing Date
	 DATE			LastFixingDate,		// Last Fixing Date
	 DATE			ExpirationDate,		// Expiration Date
	 double			LowAmortPrice,		// Lower Amortization Price
	 double			HighAmortPrice,		// High Amortization Price
	 double			LowAmortAmount,		// Lower Amortization Amount
	 double			HighAmortAmount,	// High Amortization Amount
	 DT_CURVE  		*FixingCurve,		// Fixing curve.
	 DT_CURVE  		*FwdCurve,			// Forward curve.
	 DT_CURVE  		*VolCurve,			// Term vol curve.
	 DT_CURVE  		*DiscCurve,			// Discount Factor curve.
	 double			StdErrTolerance,	// Stop if StdErr is less than this, else NumPathsBy2 reached
	 int          	NumPathsBy2, 		// Maximum Num Monte Carlo paths
	 int			Seed				// MC seed (large negative number is best)
	);

EXPORT_C_QAMORT double Q_AmortizeOnceB (
	Q_VECTOR        		*types,         		// option types of each deal
	double					spot,                   // current spot price
	Q_MATRIX        		*strike,    		    // matrix of strikes - one row per deal
	Q_DATE_VECTOR   		*paydates,      		// payment dates 
	Q_MATRIX 				*Amounts,       		// One Amount per payment date
	double					LowPrice,               // low price for 100 amortization
	double				    HighPrice,              // high price for 0 amortization
	double					LowPct,                 // Percentage at lower level
	double					HighPct,                // Percentage at higher level
	Q_VECTOR				*vol,                   // piecewise constant spot volatility vector
	Q_VECTOR				*volTimes,              // volatility times
	double					beta,                   // mean reversion coefficient
	double					longVol,                // volatility of long price
	double					corr,                   // correlation between short and long
	DATE					prcDate,                // pricing date
	DATE					AmortDate,              // amortization date
	DATE                    LagDate,                // amortization only kicks in after this
	double					tenor,                  // tenor for amortization
	Q_VECTOR				*Rd,                    // one domestic rate per payment time
	Q_VECTOR				*fwds,                  // vector of forwards at payment dates
	Q_VECTOR				*fwdT                   // array of times for forwards
	);
	
EXPORT_C_QAMORT double Q_AmortizeOnceAvgB (
	Q_VECTOR        		*types,         		// option types
    double 					spot,           		// Underlyer spot price
    Q_MATRIX 				*strike,        		// one strike vector per deal
	Q_DATE_VECTOR   		*paydates,      		// payment dates
    Q_MATRIX 				*Amounts,       		// One Amount per payment date for each deal
    double					LowPrice,               // low price for 100 amortization
	double				    HighPrice,              // high price for 0 amortization
    double					LowPct,                 // Percentage at lower level
    double					HighPct,                // Percentage at higher level
    Q_VECTOR				*vol,                   // piecewise constant spot volatility vector
    Q_VECTOR				*volTimes,              // volatility times
    double					beta,                   // mean reversion coefficient
    double					longVol,                // volatility of long price
    double					corr,                   // correlation between short and long
    DATE					prcDate,                // pricing date
    DATE					AmortDate,              // amortization date
    DATE                    LagDate,                // amortization only kicks in after this
    double					tenor,                  // tenor for amortization
    double                  avgtenor,               // tenor for averaging
	int						firstavg,               // when averaging kicks in
    Q_VECTOR				*Rd,                    // one domestic rate per payment time
    Q_VECTOR				*fwds,                  // vector of forwards
    Q_VECTOR				*fwdT                   // time grid for forwards
    );

EXPORT_C_QAMORT int Q_MRMAmortOptMCLagTB (
    Q_VECTOR        *types,         // option types of each deal
    double 			spot,           // Underlyer spot price
    Q_MATRIX        *strike,        // matrix of strikes - one row per deal
	Q_DATE_VECTOR   *paydates,      // payment dates 
    Q_MATRIX 		*Amounts,       // One Amount per payment date
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    double          tenor,          // amortizes against this rolling future
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    DATE            prcDate,        // pricing date
    Q_VECTOR   		*AmortIndex,  	// indices of amortization dates in pay dates
    Q_VECTOR   		*LagIndex,  	// amortization kicks in on these dates
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error,         // returned standard error
	double          *rawprem,       // returned raw premium
	double          *rawerror       // returned raw error
    );

EXPORT_C_QAMORT double Q_AmortSwapKODW(
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // Number of ounces for each payment
	double          lowerKO,        // lower knockout level
	double          upperKO,        // upper knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization dates
    DATE            lowerkoDate,    // start of lower knockout period
    DATE            upperkoDate,	// start of upper knockout period
    DATE            endko,          // end of knockout periods
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_AmortOptKODMC (
	Q_OPT_TYPE		callPut,        // call, put or forward
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          lowerKO,        // lower knockout level
	double          upperKO,        // upper knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            lowerkoDate,    // start of lower knockout period
    DATE            upperkoDate,	// start of upper knockout period
    DATE            endko,			// end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );

EXPORT_C_QAMORT int Q_MRMAmortOptMCLagAvg2B (
	Q_VECTOR        *types,         // option types
    double 			spot,           // Underlyer spot price
    Q_MATRIX 		*strike,        // one strike vector per deal
	Q_DATE_VECTOR   *paydates,      // payment dates
    Q_MATRIX 		*Amounts,       // One Amount per payment date for each deal
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    double          tenor,          // amortizes against this rolling future
    int             firstavg,       // first payoff based on averaging
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    DATE            prcDate,        // pricing date
    Q_VECTOR   		*AmortIndex,  	// indices of amortization dates in pay dates
    Q_VECTOR   		*LagIndex,  	// amortization kicks in on these dates
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error,         // returned standard error
	double          *rawprem,       // returned raw premium
	double          *rawerror       // returned raw error
    );

EXPORT_C_QAMORT double Q_VarQuantOption (
    Q_OPT_TYPE				callPut,                // call or put or forward
    double					strike,     	        // strike price
    double					LowPrice,               // low price for 100 amortization
    double				    HighPrice,              // high price for 0 amortization
    double					LowPct,                 // Percentage at lower level
    double					HighPct,                // Percentage at higher level
    double					AmortVol,               // vol to quantity date
    double 					expVol,                 // vol to expiration
    DATE					prcDate,                // pricing date
    DATE					AmortDate,              // date when amount of payoff is set
    DATE                    expDate,                // expiration date of the option
    double					ccRd,                   // domestic rate to expiration
    double					AmortFwd,               // forward to quantity date
    double 					expFwd                  // forward to expiration
    );

EXPORT_C_QAMORT double Q_AmortOptLagW(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	double          rangespot,      // spot used for centering the grid
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_AmortOptLagMC (
	Q_OPT_TYPE		callPut,        // call, put or forward
	Q_UP_DOWN       upOrDown,       // knockout direction
	int             zeroflag,       // 1 if "zero lag", otherwise
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,  	// dates where amortization kicks in
    DATE            koDate,         // start of knockout period
    DATE            endko,			// end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );
      
EXPORT_C_QAMORT double Q_AmortOptLagMCMod (
	Q_OPT_TYPE		callPut,        // call, put or forward
	Q_UP_DOWN       upOrDown,       // knockout direction
	int             zeroflag,       // 1 if "zero lag", otherwise
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,  	// dates where amortization kicks in
    DATE            koDate,         // start of knockout period
    DATE            endko,			// end of knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
    DT_CURVE 		*fwdGrowthCurve,   	// forward growth curve
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );


EXPORT_C_QAMORT double Q_AmortOptZeroLag (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	double          rangespot,      // spot used for centering the grid
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );
	
EXPORT_C_QAMORT int Q_AmortOptZeroLagVec (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double          minSpot,        // smallest Spot for which we need accurate value
	double          maxSpot,        // largest Spot for which we need accurate value
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
	DT_CURVE		*ForeignDisc,	// Foreign currency discount curve
	int				efficient,		// boolean indicating whether the buyer has a right not to amortize			
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
    Q_VECTOR		*SpotVec,		// returned array of spots
	Q_VECTOR		*OptionPrice	// returned array of option prices
	);

EXPORT_C_QAMORT int Q_AmortOptZeroLagFFTZeroBC (
	Q_OPT_TYPE      callPut,        // option type
	double          rangespot,      // spot used for centering the grid
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
	DT_CURVE		*ForeignDisc,	// Foreign currency discount curve
    int 			gridlog,		// gridsize as a power of 2
	Q_VECTOR		*SpotVec,		// returned array of spots
	Q_VECTOR		*OptionPrice	// returned array of option prices
    );



EXPORT_C_QAMORT int Q_AmortOptLagWVec(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double          minSpot,        // smallest Spot for which we need accurate value
	double          maxSpot,        // largest Spot for which we need accurate value
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
	DT_CURVE		*ForeignDisc, 	// foreign currency discount curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
    Q_VECTOR		*SpotVec,		// returned array of spots
	Q_VECTOR		*OptionPrice	// returned array of option prices
	);

EXPORT_C_QAMORT double Q_SVAmortSwap (
    Q_UP_DOWN    	upOrDown, 		// direction of knockout
    double       	spot,     		// current forward price at expiration
	Q_VECTOR 		*strike,   		// one strike price	per payment date
	Q_VECTOR 		*Amounts,      	// Notional for each payment
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    double       	outVal,   		// knockout level
    double          initvol,        // initial volatility
    Q_VECTOR        *lvol,     		// vector of limiting volatilities
    Q_VECTOR        *volT,          // vector of volatility nodes
	double       	beta,     		// mean reversion factor on volatility
	double       	sigma,    		// coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      		// correlation of spot and volatility
    Q_VECTOR   		*AmortIndex,	// indices of amortization dates in payment dates
    double          kotime,   	 	// beginning of knockout period
    Q_VECTOR 		*times,	 		// payment times
	Q_VECTOR     	*fwds,    		// forward vector
	Q_VECTOR     	*fwdT,    		// nodes for forward curve
	Q_VECTOR        *Rd,      		// one domestic rate per payment
    int             nSteps,   		// number of timesteps
    int             nPaths,  		// number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType			// return premium or error or raw premium
	);

EXPORT_C_QAMORT int Q_AmortOptZeroLagFFT (
	Q_OPT_TYPE      callPut,        // option type
	double          rangespot,      // spot used for centering the grid
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdGrowth,   	// one forward price per payment time
	int          	gridlog,		// gridsize as power of 2
	Q_VECTOR        *SpotVec,		// returned spot array
	Q_VECTOR        *OptionPrice    // return option price array
    );

EXPORT_C_QAMORT int Q_MRMAmortOptMCLagT (
	Q_OPT_TYPE      callPut,        // option type
    double 			spot,           // Underlyer spot price
    DT_CURVE 		*strike,        // dates and strikes
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    double          tenor,          // amortizes against this rolling future
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    DATE            prcDate,        // pricing date
    Q_VECTOR   		*AmortIndex,  	// indices of amortization dates in pay dates
    Q_VECTOR   		*LagIndex,  	// amortization kicks in on these dates
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error,         // returned standard error
	double          *rawprem,       // returned raw premium
	double          *rawerror       // returned raw error
    );

EXPORT_C_QAMORT int Q_MRMAmortOptMCLagT (
	Q_OPT_TYPE      callPut,        // option type
    double 			spot,           // Underlyer spot price
    DT_CURVE 		*strike,        // dates and strikes
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    double          tenor,          // amortizes against this rolling future
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    DATE            prcDate,        // pricing date
    Q_VECTOR   		*AmortIndex,  	// indices of amortization dates in pay dates
    Q_VECTOR   		*LagIndex,  	// amortization kicks in on these dates
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error,         // returned standard error
	double          *rawprem,       // returned raw premium
	double          *rawerror       // returned raw error
    );

EXPORT_C_QAMORT double Q_AmortizeOnce (
    Q_OPT_TYPE				callPut,                // call or put
    double					spot,                   // current spot price
    DT_CURVE				*strike,                // strike price curve
    Q_VECTOR				*Amounts,               // notional on each payment
    double					LowPrice,               // low price for 100 amortization
    double				    HighPrice,              // high price for 0 amortization
    double					LowPct,                 // Percentage at lower level
    double					HighPct,                // Percentage at higher level
    Q_VECTOR				*vol,                   // piecewise constant spot volatility vector
    Q_VECTOR				*volTimes,              // volatility times
    double					beta,                   // mean reversion coefficient
    double					longVol,                // volatility of long price
    double					corr,                   // correlation between short and long
    DATE					prcDate,                // pricing date
    DATE					AmortDate,              // amortization date
    DATE                    LagDate,                // amortization only kicks in after this
    double					tenor,                  // tenor for amortization
    Q_VECTOR				*Rd,                    // one domestic rate per payment time
    Q_VECTOR				*fwds,                  // vector of forwards at payment dates
    Q_VECTOR				*fwdT                   // array of times for forwards
    );

EXPORT_C_QAMORT int Q_MRMAmortOptMCLagAvg2 (
	Q_OPT_TYPE      callPut,        // option type
    double 			spot,           // Underlyer spot price
    DT_CURVE 		*strike,        // strike curve
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    double          tenor,          // amortizes against this rolling future
    int             firstavg,       // first payoff based on averaging
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    DATE            prcDate,        // pricing date
    Q_VECTOR   		*AmortIndex,  	// indices of amortization dates in pay dates
    Q_VECTOR   		*LagIndex,  	// amortization kicks in on these dates
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error,         // returned standard error
	double          *rawprem,       // returned raw premium
	double          *rawerror       // returned raw error
    );

EXPORT_C_QAMORT double Q_AmortizeOnceAvg (
    Q_OPT_TYPE				callPut,                // call or put
    double					spot,                   // current spot price
    DT_CURVE				*strike,                // strike price curve
    Q_VECTOR				*Amounts,               // notional on each payment
    double					LowPrice,               // low price for 100 amortization
	double				    HighPrice,              // high price for 0 amortization
    double					LowPct,                 // Percentage at lower level
    double					HighPct,                // Percentage at higher level
    Q_VECTOR				*vol,                   // piecewise constant spot volatility vector
    Q_VECTOR				*volTimes,              // volatility times
    double					beta,                   // mean reversion coefficient
    double					longVol,                // volatility of long price
    double					corr,                   // correlation between short and long
    DATE					prcDate,                // pricing date
    DATE					AmortDate,              // amortization date
    DATE                    LagDate,                // amortization only kicks in after this
    double					tenor,                  // tenor for amortization
    double                  avgtenor,               // tenor for averaging
	int						firstavg,               // when averaging kicks in
    Q_VECTOR				*Rd,                    // one domestic rate per payment time
    Q_VECTOR				*fwds,                  // vector of forwards
    Q_VECTOR				*fwdT                   // time grid for forwards
    );
	
EXPORT_C_QAMORT int Q_AmortOptZeroLagSqueezed (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double          minSpot,        // smallest Spot for which we need accurate value
	double          maxSpot,        // largest Spot for which we need accurate value
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
	DT_CURVE		*ForeignDisc, 	// Foreign currency discount curve
	int				efficient,		// boolean indicating whether the buyer has a right not to amortize			
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	Q_VECTOR		*SpotVec,		// returned spot array
	Q_VECTOR		*OptionPrice	// returned option price array
	);

EXPORT_C_QAMORT int Q_MRMAmortOptMCLagJ (
	Q_OPT_TYPE      callPut,        // option type
    double 			spot,           // Underlyer spot price
    DT_CURVE 		*strike,        // dates and strikes
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    double          tenor,          // amortizes against this rolling future
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
	Q_VECTOR        *jumpMean,      // jump means of mean-reverting and non-mean-reverting factors
	Q_MATRIX        *jumpCov,       // jump covariance matrix
	double          lambda,         // jump frequency
    DATE            prcDate,        // pricing date
    Q_VECTOR   		*AmortIndex,  	// indices of amortization dates in pay dates
    Q_VECTOR   		*LagIndex,  	// amortization kicks in on these dates
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error,         // returned standard error
	double          *rawprem,       // returned raw premium
	double          *rawerror       // returned raw error
    );

#endif

