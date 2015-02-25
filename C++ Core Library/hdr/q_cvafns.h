/****************************************************************
**
**	q_cvafns.h	- header files for q_cvafns.c functions
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_Q_CVAFNS_H )
#define IN_Q_CVAFNS_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QUANT int Q_WindowedDKOMatrix(
	Q_OPT_TYPE    	OptionType,			// Option type (Q_OPT_PUT, Q_OPT_CALL, Q_OPT_FWD )
	Q_IN_OUT     	inOrOut, 			// appearing or disappearing
	double          minSpot,        	// smallest Spot for which we need accurate value
	double          maxSpot,        	// largest Spot for which we need accurate value
	double 			strike,	 			// Option strike price
	double			LowBarrier,			// Low Barrier
	double 			HighBarrier,		// High Barrier
	double			LowerRebate,		// Knockout rebate at the low barrier
	double 			UpperRebate,	 	// Knockout rebate at the high barrier
	Q_REBATE_TYPE	RebateType,			// Immediate or delayed till exp date
	double			RangeVol,			// volatility used to fix the grid size
	DATE			startLow,			// start time of low barrier
	DATE			endLow,				// end time of low barrier
	DATE			startHigh,			// start time of high barrier
	DATE			endHigh,			// end time of high barrier
	DATE			expDate,			// expiry
	DT_CURVE 		*Vols,				// Underlyer volatility	curve
    DT_CURVE 		*DiscCrv,  			// domestic discount curve
	DT_CURVE		*ForeignDisc, 		// Foreign currency discount curve
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of grid points
	Q_DATE_VECTOR	*DateVec,			// times when we want the output
	Q_VECTOR	  	*SpotVec,			// spot grid, input and output
	Q_VECTOR		**PriceMat			// prices grid, input and output
);

													   
EXPORT_C_QUANT int Q_AmortOptZeroLagMatrix(
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
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
	DT_CURVE		*ForeignDisc, 	// Foreign currency discount curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	Q_DATE_VECTOR	*DateVec,		// times when we want the output
	Q_VECTOR		*SpotVec,		// returned spot array
    Q_VECTOR		**PriceMat		// returned option price matrix, includes values in space and time directions
);

EXPORT_C_QUANT int Q_OptMatrix(
	Q_OPT_TYPE		OptType,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			strike,			// strike
	DATE			expDate,		// exp date
	DT_CURVE		*Vols,			// volatility curve
	DT_CURVE		*DomesticDisc,	// domestic discount curve
	DT_CURVE		*ForeignDisc,	// foreign discount curve
	Q_DATE_VECTOR   *DateVec,		// dates when we need the prices
	double			minSpot,		// minimal grid value
	double 			maxSpot, 		// maximal grid value
	int				nGrid,			// number of output grid points
	Q_VECTOR		*SpotVec,		// output grid values
	Q_VECTOR		**PriceMat		// output prices
);

EXPORT_C_QUANT int Q_MultisegmentDKOMatrix(
	Q_OPT_TYPE    	OptionType,				// Option type (Q_OPT_PUT, Q_OPT_CALL or Q_OPT_FWD)
	Q_IN_OUT     	inOrOut, 				// appearing or disappearing
	double          minSpot,        		// smallest Spot for which we need accurate value
	double          maxSpot,        		// largest Spot for which we need accurate value
	double			strike,					// strike
	double			RangeVol,				// volatility used to set up the grid
	Q_VECTOR		*LowerBarrierLevels,	// lower barrier levels
	Q_VECTOR		*UpperBarrierLevels,	// upper barrier levels
	Q_DATE_VECTOR	*lowerBarrierDates,		// End times of each lower barrier
	Q_DATE_VECTOR	*upperBarrierDates,		// End times of each upper barrier
	Q_VECTOR		*lowerRebate,			// lower rebates
	Q_VECTOR		*upperRebate,			// upper rebates
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
	DATE			expDate,				// exp date
	DT_CURVE 		*Vols,					// Underlyer volatility	curve
	DT_CURVE		*DiscCrv,				// Discount Curve
	DT_CURVE		*ForeignDisc, 			// Foreign currency discount curve
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	Q_DATE_VECTOR	*OutDates,				// times when we want the output
	Q_VECTOR	  	*SpotVec,				// spot grid, input and output
	Q_VECTOR		**PriceMat				// prices grid, input and output
);

EXPORT_C_QUANT int Q_BoundMatrixFromMatrix(
		double		minSpot,			// the smallest spot of the output vector ( in spot space )
		double		maxSpot,			// the largest spot of the output vector ( in spot space ) 
		double		*logMinGrid,		// the lowest point on the grid          ( in log space ) 
		double		*logMaxGrid,		// the highest point on the grid         ( in log space ) 
		int 		*nGrid,				// the gridsize
		double		**V,				// matrix of values
		Q_VECTOR	*Spots,				// the spots vector to initialize
		Q_VECTOR	**PriceMat,			// matrix where the values are copied to
		int			MatSize				// matrix size
);

#endif

