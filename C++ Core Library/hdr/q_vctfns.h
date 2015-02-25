/****************************************************************
**
**	q_vctfns.h			
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_Q_VCTFNS_H )
#define IN_Q_VCTFNS_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#include <mempool.h>
#include <n_cn.h>

EXPORT_C_QUANT int Q_CheckTermStructureFromCurves(
	DATE		prcDate,			// pricing date
	DT_CURVE 	*Vols,				// Underlyer volatility	curve
	DT_CURVE	*FwdFactors,		// Normalized Forward Curve
	DT_CURVE	*DicsCrv,			// Discount Curve
	Q_VECTOR	*Variances,
	Q_VECTOR	*VarianceTimes,
	Q_VECTOR	*LogDiscounts,
	Q_VECTOR	*LogDiscountTimes,
	Q_VECTOR	*LogForwards,
	Q_VECTOR	*LogForwardTimes
);

EXPORT_C_QUANT int Q_CheckTermStructureFromCurvesAlt(
		Q_LOG_MARKET_INFO	*MarketInfo,		// structure to return the data
		DATE				prcDate,
		DT_CURVE			*Vols,				// the volatilities
		DT_CURVE			*FwdFactors,		// forward growth factors
		DT_CURVE			*DiscCrv,			// domestic discount curve
		MEM_POOL			*MemPool			// the mempool for allocating space for the vectors
);

EXPORT_C_QUANT int Q_WindowedDKOVec(
	Q_OPT_TYPE    	OptionType,			// Option type (Q_OPT_PUT, Q_OPT_CALL, Q_OPT_FWD )
	Q_IN_OUT     	inOrOut, 			// appearing or disappearing
	double          minSpot,        	// smallest Spot for which we need accurate value
	double          maxSpot,        	// largest Spot for which we need accurate value
	double 			strike,	 			// Option strike price
	double			LowBarrier,			// Low Barrier
	double 			HighBarrier,		// High Barrier
	double			LowerRebate,		// Knockout rebate at the low barrier
	double 			UpperRebate,	 	// Knockout rebate at the high barrier
	double			RangeVol,			// volatility used to fix the grid size
	DATE			StartLow,			// start time of low barrier
	DATE			EndLow,				// end time of low barrier
	DATE			StartHigh,			// start time of high barrier
	DATE			EndHigh,			// end time of high barrier
    DATE            prcDate,        	// pricing date
	DATE			expDate,			// expiry
	Q_REBATE_TYPE	RebateType,			// Immediate or delayed till exp date
	DT_CURVE 		*Vols,				// Underlyer volatility	curve
    DT_CURVE 		*DiscCrv,	  		// domestic discount curve
	DT_CURVE		*ForeignDisc, 		// Foreign currency discount curve
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of grid points
	Q_VECTOR		*Spots,				// spot grid, input and output
	Q_VECTOR		*Prices				// prices grid, input and output
);

EXPORT_C_QUANT int Q_MultisegmentDKOVec(
	Q_OPT_TYPE    	OptionType,				// Option type (Q_OPT_PUT, Q_OPT_CALL or Q_OPT_FWD)
	Q_IN_OUT     	inOrOut, 				// appearing or disappearing
	double          minSpot,        		// smallest Spot for which we need accurate value
	double          maxSpot,        		// largest Spot for which we need accurate value
	double			strike,					// strike
	Q_VECTOR		*LowerBarrierLevels,	// lower barrier levels
	Q_VECTOR		*UpperBarrierLevels,	// upper barrier levels
	Q_DATE_VECTOR	*lowerBarrierDates,		// End times of each lower barrier
	Q_DATE_VECTOR	*upperBarrierDates,		// End times of each upper barrier
	Q_VECTOR		*lowerRebate,			// lower rebates
	Q_VECTOR		*upperRebate,			// upper rebates
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
	double			RangeVol,				// volatility used to set up the grid
	DATE			prcDate,				// pricing date
	DATE			expDate,				// exp date
	DT_CURVE 		*Vols,					// Underlyer volatility	curve
	DT_CURVE		*FwdFactors,			// Normalized Forward Curve
	DT_CURVE		*DiscCrv,				// Discount Curve
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	Q_VECTOR		*SpotVec,				// spot grid, input and output
	Q_VECTOR		*PriceVec				// prices grid, input and output
);
 
EXPORT_C_QUANT int Q_TestMinMaxKnockouts(
		double		*LogLowerBarriers,		// vector to return logs of lower barriers
		double		*LogUpperBarriers,		// vector to return logs of upper barriers
		double		*MaxUpperBarrier,		// vector of maxes of upper barriers
		double		*MinLowerBarrier,		// vector of min of lower barriers
		double		*KnockoutTime, 			// returns time that we are certain to knockout (otherwise HUGE_VAL)
		double		*Rebate,				// returns rebate when we are certain to knock out
		double		LogMinSpot,				// logarithm of minSpot
		double		LogMaxSpot,				// logarithm of maxSpot
		Q_VECTOR	*LowerBarrierLevels,	// the lower knockouts
		Q_VECTOR	*UpperBarrierLevels,	// the upper knockouts
		Q_VECTOR	*LowerBarrierTimes,		// the end times of the lower knockouts
		Q_VECTOR	*UpperBarrierTimes,		// the end times of the upper knockouts
		Q_VECTOR	*LowerRebates,			// the rebates at the lower knockouts
		Q_VECTOR	*UpperRebates,			// the rebates at the upper knockouts
		double		Term,					// time to maturity
		double		MinGrid,				// the minimum of the grid range
		double		MaxGrid					// the maximum of the grid range
);

EXPORT_C_QUANT int Q_InterpolateDKOVec(
	double     logMinSpot,        	// smallest Spot for which we need accurate value ( in log space )
	double     logMaxSpot,        	// largest Spot for which we need accurate value  ( in log space )
	Q_VECTOR   *Spots,				// array of spots, input and output
	Q_VECTOR   *Prices,				// array of prices, input and output
	double	   logLower, 			// lowerko, log space
	double	   logUpper,			// upper ko, log space
	double	   lowValue,			// value at the lower boundary
	double	   highValue			// value at the upper boundary
);

#endif

