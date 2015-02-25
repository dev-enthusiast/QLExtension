/* $Header: /home/cvs/src/meanrev/src/meanrev.h,v 1.37 1999/08/20 17:36:16 goodfj Exp $ */
/****************************************************************
**
**	MEANREV.H	
**	
**	Export header for meanrev project.
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.37 $
**
****************************************************************/

#if !defined( IN_MEANREV_H )
#define IN_MEANREV_H

#include <datatype.h>
#include <qtypes.h> // for Elem definition

// Type definitions for mean reversion and baskets

typedef struct MR_params
{
  	double 		Beta;			// MR Beta
	double 		LongVol;		// MR Long term vol
	double 		Corr;			// MR Correlation between long and short

} MR_PARAMS ;

typedef struct MR_parvec
{
	unsigned	Elem;		// How many in vector
	MR_PARAMS	*V;			// The data block

} MR_PARAMS_VECTOR;

typedef struct MR_curvec
{
	unsigned	Elem;		// How many in vector
	DT_CURVE	**V;		// a vector of pointers to curves

} MR_CURVES_VECTOR;

typedef struct MR_corrs
{
	DT_CURVE	*CorrSS;	// Correlation between two Short term prices
	double 		CorrSL;		// Correlation between Short and Long term prices
	double 		CorrLS;		// Correlation between Long and Short term prices
	double 		CorrLL;		// Correlation between two Long term prices

} MR_CORRS ;

typedef struct MR_cormat
{
	unsigned	Cols;		// How many rows/cols in matrix (square)
	MR_CORRS	*M;			// The data block

} MR_CORRS_MATRIX;

#define CElemPtr(Mat,Row,Col) (&(Mat->M[(Mat->Cols)*(Row)+Col]))

typedef struct MR_bskcomp
{
	unsigned	WhichCommod;	// Index into arrays by commodity
	long		FutExp;			// Expiration date of desired future
	long		FixDate;		// Date to fix price of future for basket
	double		Price;			// Current price of the future
	double		Weight;			// Weight of this future in basket
	
} MR_BSK_COMPONENT;

typedef struct	MR_bskspecs
{
	unsigned			Elem;	// How many components in basket
	MR_BSK_COMPONENT	*V;		// The data for all components

} MR_BSK_SPECS;

typedef struct M_BskParams
{
	double	LongVol;		// term vol of long position of basket
	double	ShortVol;		// term vol of short postition of basket
	double	Corr;			// correlation of returns of long and short
	double	Current;		// Current value of basket
	double	LongCurrent;	// Current value of long position
	double	ShortCurrent;	// Current value of short position

} M_BSK_PARAMS;

// Exports

DLLEXPORT DT_CURVE *MR_TermVolsOfSpot(
	DT_CURVE		*FutPrices,	// Futures price curve with fut exp dates
	DT_CURVE		*OptVols,	// Option vol curve with option exp dates
	MR_PARAMS		*Params,  	// Beta, LongVol, and Correlation
	long			Today,		// Pricing date
	double			InitialVol	// spot vol today
	);

DLLEXPORT DT_CURVE *MR_TermVolsOfSpotNew(
	DT_CURVE		*FutPrices,	// Futures price curve with fut exp dates
	DT_CURVE		*OptVols,	// Option vol curve with option exp dates
	MR_PARAMS		*Params,  	// Beta, LongVol, and Correlation
	long			Today,		// Pricing date
	double			InitialVol	// spot vol today
	);
	
DLLEXPORT M_BSK_PARAMS *Q_BskVolAndCor(
	Q_VECTOR 	*Weights,	 	// Weights for each object in basket
	Q_VECTOR	*Prices,		// Current prices of each object
	Q_MATRIX	*Cov,			// Term Covariance matrix of objects
	long		ExpDate,		// Expiration date
	long		Today			// pricing date
);

DLLEXPORT M_BSK_PARAMS *Q_TmBskVolAndCor(
	Q_VECTOR 	*Weights,	 	// Weight for each object in basket
	Q_VECTOR	*Prices,		// Current price of each object
	Q_MATRIX	*Cov,			// Term Covariance matrix of objects
	long		ExpDate,		// Expiration date
	long		Today			// pricing date
);

DLLEXPORT M_BSK_PARAMS *MR_DenomFutBsk(
	MR_PARAMS_VECTOR	*Params,		// All the MR parameters for commods
	MR_CURVES_VECTOR	*SpotVolCurves,	// All the spot vol curves for commods
	MR_CORRS_MATRIX		*CorrsMat,		// MR_CORRS between pairs of DIFFERENT commods ( ignores diagonal elements of matrix )
	MR_BSK_SPECS		*BasketSpecs,	// Specifies weights, which futures, when
	Q_VECTOR			*CurrPrices,	// Spot prices of each currency
	MR_CURVES_VECTOR	*CurrVolCrvMat,	// Matrix of currency vol and cross vol curves as a vector
	Q_VECTOR			*CurrAmts,		// Amount of each currency in the basket
	Q_VECTOR			*CommodDenom,	// Which currency cross to use for each commod
	long				ExpDate,		// Expiration date of option
	long				Today			// pricing date
);

DLLEXPORT double MR_FutVol(
	long		FutExp,			// Futures expiration date
	long		VolExp,			// Desired date for vol
	long		Today,			// Pricing Date
	double		Beta,			// MR Parameter Beta
	double		Corr,			// MR Parameter Correlation
	double		LVol,			// MR Parameter Long Term Vol
	DT_CURVE	*SpotVols		// Spot term vol curve
);

DLLEXPORT double MR_FutVolNew(
	long		FutExp,			// Futures expiration date
	long		VolExp,			// Desired date for vol
	long		Today,			// Pricing Date
	double		Beta,			// MR Parameter Beta
	double		Corr,			// MR Parameter Correlation
	double		LVol,			// MR Parameter Long Term Vol
	DT_CURVE	*SpotVols		// Spot instantaneous vol curve
);

DLLEXPORT double MR_FutCov(
	long		FutExp1,		// Futures expiration date 1
	long		FutExp2,		// Futures expiration date 2
	long		VolExp,			// Desired date for cov
	long		Today,			// Pricing Date
	double		Beta,			// MR Parameter Beta
	double		Corr,			// MR Parameter Correlation
	double		LVol,			// MR Parameter Long Term Vol
	DT_CURVE	*SpotVols		// Spot term vol curve
);

DLLEXPORT double MR_AccumFwd( 
	DT_ARRAY	*FutWts,		        // MR Futures Weights array
	double		Strike,			        // Strike of accum fwd
    double		StrikePerturbedUp,		// Strike perturbed
    double		StrikePerturbedDown,	// Strike perturbed
	double		AccumLevel,		        // Accumulation level ( strike +/- premium )
	double		Rebate,			        // Per day rebate if outside floor/ceiling
	long		Today,			        // Pricing Date
	long		ExpSetDate,		        // Expiration Settlement date 
	double		IntRate,		        // Domestic interest rate
	double		Beta,			        // MR Parameter Beta
	double		Corr,			        // MR Parameter Correlation
	double		LVol,			        // MR Parameter Long Term Vol
	DT_CURVE	*SpotVols,		        // MR Term Vols of Spot Process
    DT_CURVE	*SpotVolsPerturbedUp,	// MR Term Vols of Spot Process for K+
    DT_CURVE	*SpotVolsPerturbedDown,	// MR Term Vols of Spot Process for K-
	double		VolAdj,			        // Volatility adjustment
	double		PriceAdj,		        // Price adjustment
	double		CrossVol 		        // Location cross vol
);

DLLEXPORT double MR_OptionStrip( 
	DT_ARRAY	*FutWts,		// MR Futures Weights array
	double		Strike,			// Strike of accum fwd
	Q_OPT_TYPE	CallPut,		// Option Type
	long		Today,			// Pricing Date
	long		ExpSetDate,		// Expiration Settlement date 
	double		IntRate,		// Domestic interest rate
	double		Beta,			// MR Parameter Beta
	double		Corr,			// MR Parameter Correlation
	double		LVol,			// MR Parameter Long Term Vol
	DT_CURVE	*SpotVols,		// MR Term Vols of Spot Process
	double		VolAdj,			// Volatility adjustment
	double		PriceAdj,		// Price adjustment
	double		CrossVol 		// Location cross vol
);

DLLEXPORT M_BSK_PARAMS *MR_FutBasket(
	long	Today,				// Pricing Date
	long	ExpDate,			// Expiration Date of basket option
	int		NumComponents,		// number of futures components in the basket
	double	*Weights,			// basket weights for each component
	double	*Prices,			// current prices of each component in deal's currency
	long	*FixDates,			// fix times of each component
	double	*ExpTimes,			// futures expiration times of each
	int		*Commods,			// index of which commod each component is
	int		NumCommods,			// how many different commodities are there
	double	*Betas,				// the MR betas for each commodity
	double	*LongVols,			// the MR Long term vols for each commodity
	double	*Corrs,				// the MR correlations for each commodity
	DT_CURVE	
			**SpotVolCurves,	// the spot vol curves for each commodity
	MR_CORRS
			**InterCommodCorrs,	// the inter-commodity correlations between commods
	int		*CommodDenom,		// index of which currency each commod is denominated in
	int		NumCurrs,			// number of different currencies (deal denominated is index 0)
	DT_CURVE
			**CurrVolCurves,	// matrix of cross vols of curr vs denominated and each other
	double	*CurrComps			// amount of each currency in basket (times cross to denom curr)
);

DLLEXPORT M_BSK_PARAMS *MR_FutBasketNew(
	long		Today,				// Pricing Date
	long		ExpDate,			// Expiration Date of basket option
	int			NumComponents,		// number of futures components in the basket
	double		*Weights,			// basket weights for each component
	double		*Prices,			// current prices of each component in deal's currency
	long		*FixDates,			// fix times of each component
	double		*ExpTimes,			// futures expiration times of each
	int			*Commods,			// index of which commod each component is
	int			NumCommods,			// how many different commodities are there
	double		*Betas,				// the MR betas for each commodity
	double		*LongVols,			// the MR Long term vols for each commodity
	double		*Corrs,				// the MR correlations for each commodity
	DT_CURVE	**SpotVolCurves,	// the term spot vol curves for each commodity
	MR_CORRS	**InterCommodCorrs,	// the inter-commodity correlations between commods
	int			*CommodDenom,		// index of which currency each commod is denominated in
	int			NumCurrs,			// number of different currencies (deal denominated is index 0)
	DT_CURVE	**CurrVolCurves,	// matrix of cross vols of curr vs denominated and each other
	double		*CurrComps			// amount of each currency in basket (times cross to denom curr)
	);

DLLEXPORT M_BSK_PARAMS *MR_FutBasketNewB(
	long		Today,				// Pricing Date
	long		ExpDate,			// Expiration Date of basket option
	int			NumComponents,		// number of futures components in the basket
	double		*Weights,			// basket weights for each component
	double		*Prices,			// current prices of each component in deal's currency
	long		*FixDates,			// fix times of each component
	double		*ExpTimes,			// futures expiration times of each
	int			*Commods,			// index of which commod each component is
	int			NumCommods,			// how many different commodities are there
	Q_VECTOR	*Betas,				// the MR betas vectors for each commodity
	double		*LongVols,			// the MR Long term vols for each commodity
	double		*Corrs,				// the MR correlations for each commodity
	DT_CURVE	**SpotVolCurves,	// the instantaneous spot vol curves for each commodity
	MR_CORRS	**InterCommodCorrs,	// the inter-commodity correlations between commods
	int			*CommodDenom,		// index of which currency each commod is denominated in
	int			NumCurrs,			// number of different currencies (deal denominated is index 0)
	DT_CURVE	**CurrVolCurves,	// matrix of cross vols of curr vs denominated and each other
	double		*CurrComps			// amount of each currency in basket (times cross to denom curr)
	);
	
DLLEXPORT int MR_SpotAndTermVols (
	DT_CURVE	 *FutPrices,  // Futures price curve with fut exp dates
	DT_CURVE	 *OptVols,	  // Option vol curve with option exp dates
	DATE         Today,       // pricing date
    DATE         ExpDate,     // expiration date
    double		 beta,		  // mean reversion coefficient
    double       FSV,         // First spot vol
	double		 longvol,	  // volatility of long-term price
	double		 corr,        // instantaneous correlation of spot and long-term price
	DT_CURVE     **SpotVolsInst,  // returned instantaneous spot vols
	DT_CURVE     **TermVols,      // returned full-term vols of spot out to futures dates
	DT_CURVE     **TermCov        // returned full-term covariance short/long
	);

DLLEXPORT int MR_SpotAndTermVolsBBT
(
	DT_CURVE	 *FutPrices,  // Futures price curve with fut exp dates
	DT_CURVE	 *OptVols,	  // Option vol curve with option exp dates
	DATE         Today,       // pricing date
    DATE         ExpDate,     // expiration date
	DT_CURVE	 *BetaByTime, // mean reversion coefficient
    double       FSV,         // First spot vol
	double		 longvol,	  // volatility of long-term price
	double		 corr,        // instantaneous correlation of spot and long-term price
	DT_CURVE     **SpotVolsInst,  // returned instantaneous spot vols
	DT_CURVE     **TermVols,      // returned full-term vols of spot out to futures dates
	DT_CURVE     **TermCov        // returned full-term covariance short/long
);

DLLEXPORT double MR_OptPortOpt (
	Q_OPT_TYPE   		callPut,     // option type of compound option
	Q_VECTOR     		*types,      // option type of underlying as doubles
	double       		strike,      // strike of compound option
	Q_VECTOR     		*quantity,   // quantities of underlying options
	Q_VECTOR     		*strikes,    // strikes of underlying options
	Q_VECTOR     		*svol,  	 // piecewise constant spot volatility vector
	double       		longvol,     // volatility of long-term price
	double       		beta,        // mean reversion coefficient
	double       		corr,        // correlation of long and short prices
	Q_DATE_VECTOR   	*grid,       // expiration dates of futures
	DATE         		prcDate,     // today's date
	DATE         		cmpded,      // time to exp of compound option
	MR_CURVES_VECTOR   	*baskets,    // curves containing dates and weights for each basket
	MR_CURVES_VECTOR   	*indices,    // curves containing dates and indices 
	Q_DATE_VECTOR       *expdate,    // expiration dates of the options
	Q_DATE_VECTOR       *settle,     // expiration settlement dates of the options
    Q_VECTOR     		*rd,         // domestic rate vector at futures expirations
    Q_VECTOR     		*fut,        // futures price vector
    int             	npts 		 // number of nodes for integration
	);

DLLEXPORT double Q_OptPortOptGen (
	Q_OPT_TYPE   		callPut,     // option type of compound option
	Q_VECTOR     		*types,      // option type of underlying as doubles
	double       		strike,      // strike of compound option
	Q_VECTOR     		*quantity,   // quantities of underlying options
	Q_VECTOR     		*strikes,    // strikes of underlying options
	Q_VECTOR     		*vols,  	 // volatilities of the baskets
	double              vol1,        // volatility of 1st nearby to cmpded
	double              vol2,        // volatility of 2nd nearby to cmpded
	Q_VECTOR     		*corr1,  	 // corr(1st nearby, baskets)
	Q_VECTOR     		*corr2,  	 // corr(2nd nearby, baskets)
	double              corr12,      // corr(1st nearby, 2nd nearby)
	DT_CURVE		   	*futures,    // futures curve
	DATE         		prcDate,     // today's date
	DATE         		cmpded,      // expiration date of compound option
	MR_CURVES_VECTOR   	*baskets,    // curves containing dates and weights for each basket
	MR_CURVES_VECTOR   	*indices,    // curves containing dates and indices 
	Q_DATE_VECTOR       *expdate,    // expiration dates of the options
	Q_DATE_VECTOR       *settle,     // expiration settlement dates of the options
    Q_VECTOR     		*rd,         // domestic rate vector at futures expirations
    Q_VECTOR     		*AStar,      //	functions which express any later
    Q_VECTOR     		*BStar,      //	future in terms of the first 2 nearbys
    int             	npts 		 // number of nodes for integration
	);

DLLEXPORT int MR_CreateAandB (
	DT_CURVE     		*svol,  	 // piecewise constant spot volatility vector
	double       		longvol,     // volatility of long-term price
	double       		beta,        // mean reversion coefficient
	double       		corr,        // correlation of long and short prices
	DT_CURVE		   	*futures,    // futures curve
	DATE         		prcDate,     // today's date
	DATE         		expdate,     // nearby as of this date
	DT_CURVE            *AStar,		 // returned vector of A values
	DT_CURVE            *BStar		 // returned vector of B values
	);
				
DLLEXPORT double
MR_TermVolOfSpotHodgdon
(
    double          FutDate,
    double          OptExpDate,
    double          TermVol,
    double          Beta,
    double          LVol,
    double          RhoLS,
    double          Today,
    double          InitialVol
);

#endif


