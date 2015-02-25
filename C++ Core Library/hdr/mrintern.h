/* $Header: /home/cvs/src/meanrev/src/mrintern.h,v 1.6 1998/10/09 22:15:49 procmon Exp $ */
/****************************************************************
**
**	MRINTERN.H	
**
**	Internal header for meanrev project
**	
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_MRINTERN_H )
#define IN_MRINTERN_H

#include 	<meanrev.h>

// Internally useful routines

int MR_ParamsOK(
	MR_PARAMS		*Params  	// Beta, LongVol, and Corr
	);
	
int MR_CorrsOK(
	MR_CORRS	*Corrs		// Correlations of L and S between commods
);

DLLEXPORT double MR_InterpSpotVol(
	DT_CURVE		*SpotVols,	// Term vols of spot and dates for them
	long			ExpDate,	// Desired expiration
	long			Today		// Pricing date
	);
	
double *MR_InterpManySpotVols(
	DT_CURVE		*SpotVols,	// Term vols of spot and dates for them
	long			StartDate,	// desired start date of sequence
	long			EndDate,	// desired end date of sequence
	long			Today		// Pricing date
	);
	
double *InterpManyCurrVols(
	DT_CURVE		*Vols,		// vol curve
	long			StartDate,	// desired start date of sequence
	long			EndDate,	// desired end date of sequence
	long			Today		// Pricing date
	);
	
double MR_ArbFutCov(
	MR_PARAMS		*Params1,  	// Beta, LongVol, and Corr of 1
	double			*SpotVols1,	// Spot term vol curve of 1 as array of values
	double			exp1,		// Meanrev factor for 1 exp( - beta( T - t )
	double			B1,			// exp( - beta * t ) for 1
	MR_PARAMS	 	*Params2,  	// Beta, LongVol, and Corr of 2
	double		 	*SpotVols2,	// Spot term vol curve of 2 as array of values
	double			exp2,		// Meanrev factor for 2	exp( - beta( T - t )
	double			B2,			// exp( - beta * t ) for 2
	MR_CORRS	 	*Corrs,		// Correlations of L and S between commods
	long			FirstDate,	// first date for spot term vol curves
	long			mindate,	// min of two fix dates
	long			Today  		// Pricing date
);

double CalcCurrCov(
	double				**CurrVolCrvMat,// Matrix of currency vol and cross vols 
	int					i,				// First currency index
	int					j,				// Second currency index
	int					NumCurr,		// total number of currencies 
	long				ExpDate,		// Expiration date
	long				FirstDate,		// First date for CurrVolCrvMat
	long				Today			// Pricing Date
);

M_BSK_PARAMS *M_BadParams( 
	void 
);

M_BSK_PARAMS  *MR_TimeBasket (
	DT_CURVE            *Weights,       // set of dates and weights
	Q_VECTOR            *WhichCommod,   // which commodity
	Q_VECTOR            *Index,         // which future
	MR_CURVES_VECTOR    *SpotVol,       // all the spot volatility curves
	Q_MATRIX            *CorrSS,        // short-short correlation matrix
	Q_MATRIX            *CorrSL,        // short-long corrs - asymmetric
	Q_MATRIX            *CorrLL,        // long-long correlation matrix
	Q_VECTOR            *Beta,          // vector of mean reversion coefficients
	Q_VECTOR            *LongVol,       // vector of long-term volatilities
	DATE                prcDate,        // today's date
	DATE                expDate,        // expiration date 
	MR_CURVES_VECTOR	*FutCurves      // all the futures curves
	);

M_BSK_PARAMS  *MR_TimeBasketC (
	DT_CURVE            *Weights,       // set of dates and weights
	Q_VECTOR            *WhichCommod,   // which commodity
	Q_VECTOR            *Index,         // which future
	MR_CURVES_VECTOR    *SpotVol,       // all the spot volatility curves
	MR_CORRS_MATRIX		*Corrs,         // matrix of correlation structures
	Q_VECTOR            *Beta,          // vector of mean reversion coefficients
	Q_VECTOR            *LongVol,       // vector of long-term volatilities
	DATE                prcDate,        // today's date
	DATE                expDate,        // expiration date 
	MR_CURVES_VECTOR	*FutCurves      // all the futures curves
	);

M_BSK_PARAMS  *MR_TimeBasketC2 (
	Q_DATE_VECTOR       *Fixes,         // set of dates for futures prices
	Q_VECTOR            *Weights,       // set of weights, one for each fix
	Q_VECTOR            *WhichCommod,   // which commodity
	Q_VECTOR            *Index,         // which future
	MR_CURVES_VECTOR    *SpotVol,       // all the spot volatility curves
	MR_CORRS_MATRIX		*Corrs,         // matrix of correlation structures
	Q_VECTOR            *Beta,          // vector of mean reversion coefficients
	Q_VECTOR            *LongVol,       // vector of long-term volatilities
	DATE                prcDate,        // today's date
	DATE                expDate,        // expiration date 
	MR_CURVES_VECTOR	*FutCurves      // all the futures curves
	);
		
#endif
