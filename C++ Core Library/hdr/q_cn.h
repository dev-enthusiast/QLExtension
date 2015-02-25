/****************************************************************
**
**	q_cn.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.31 $
**
****************************************************************/

#if !defined( IN_Q_CN_H )
#define IN_Q_CN_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#include <mempool.h>
#include <n_cn.h>

EXPORT_C_QUANT double Q_MultisegmentDKO(
	Q_OPT_TYPE    	OptionType,				// Option type (Q_OPT_PUT, Q_OPT_CALL or Q_OPT_FWD)
	Q_IN_OUT     	inOrOut,				// appearing or disappearing
	double			spot,					// spot
	double			strike,					// strike
	Q_VECTOR		*LowerBarrierLevels,	// lower barrier levels
	Q_VECTOR		*LowerBarrierTimes,		// start times of each lower barrier
	Q_VECTOR		*UpperBarrierLevels,	// upper barrier levels
	Q_VECTOR		*UpperBarrierTimes,		// start times of each upper barrier
	Q_VECTOR		*lowerRebate,			// upper rebates
	Q_VECTOR		*upperRebate,			// upper rebates
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
    Q_VECTOR 		*Volatilities,			// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,		// times for volatilities
    double 			term,	 				// Time to expiration
    Q_VECTOR 		*Discounts,				// Domestic discount factors
    Q_VECTOR 		*DiscountTimes,			// times for discount factors
    Q_VECTOR 		*Forwards, 				// forwards
    Q_VECTOR 		*ForwardTimes,			// times for forwards
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	double			RangeVol				// volatility used to set up the grid
);

EXPORT_C_QUANT double Q_WindowedDKO(
	Q_OPT_TYPE    	OptionType,			// Option type (Q_OPT_PUT, Q_OPT_CALL, Q_OPT_FWD )
	Q_IN_OUT     	inOrOut, 			// appearing or disappearing
	double 			spot,	 			// Underlyer spot price
	double 			strike,	 			// Option strike price
	double			LowBarrier,			// Low Barrier
    double 			HighBarrier,		// High Barrier
	double			LowerRebate,		// Knockout rebate at the low barrier
    double 			UpperRebate,	 	// Knockout rebate at the high barrier
	double			StartLow,			// start time of low barrier
	double			EndLow,				// end time of low barrier
	double			StartHigh,			// start time of high barrier
	double			EndHigh,			// end time of high barrier
    Q_REBATE_TYPE	rebType,			// Immediate or delayed till exp date
    Q_VECTOR 		*Volatilities,		// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,	// times for volatilities
    double 			term,	 			// Time to expiration
    Q_VECTOR 		*Discounts,			// Domestic discount factors
    Q_VECTOR 		*DiscountTimes,		// times for discount factors
    Q_VECTOR 		*Forwards, 			// forwards
    Q_VECTOR 		*ForwardTimes,		// times for forwards
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of grid points
	double			RangeVol			// volatility used to set up the grid
);

EXPORT_C_QUANT int Q_ResizeGrid(
	double*		MinGrid,		// minimum of the grid range (input + output)
	double*		MaxGrid,		// maximum of the grid range (input + output)
	int*		nGrid,			// number of points in the grid	(input + output)
	int			nLayers,		// number of layers
	double**	PtrLayers,		// pointer to nLayers pointers to grids
	double		MinRange,		// desired minimum range
	double		MaxRange,		// desired maximum range
	double*		TempV  			// temporary work vector of size nGrid
);

EXPORT_C_QUANT void Q_ShiftGrid(
	double	*V,					// pointer to values on the grid
	double	deltaX,				// grid step size
	double	ShiftAmount,		// distance to shift the grid
	int		nGrid
	);

EXPORT_C_QUANT int Q_TermSetUp( 
    Q_VECTOR 		*Volatilities,		// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,	// times for volatilities
    Q_VECTOR 		*Discounts,			// Domestic discount factors
    Q_VECTOR 		*DiscountTimes,		// times for discount factors
    Q_VECTOR 		*Forwards, 			// forwards
    Q_VECTOR 		*ForwardTimes,		// times for forwards
	double			*Times,				// the times on the grid
	int          	nSteps, 			// number of timesteps (not the number of times)
	double			**Variations,		// vector of variations of the log of the spot
	double			**LogDiscounts,		// vector of the logarithms of the discount factors
	double			**LogForwards,		// vector of the logarithms of the forwards
	MEM_POOL		*MemPool			// the mempool
);

EXPORT_C_QUANT int Q_SetPayout(
	double			*V,
	Q_OPT_TYPE		OptionType,
	double			strike,
	double			minGrid,
	double			maxGrid,
	int				nGrid
);

EXPORT_C_QUANT int Q_ChooseGrid(
	double			spot,				// the spot
	int				*nGrid,				// number of grid points
	double			*minGrid,			// minimum grid value
	double			*maxGrid,			// maximum grid value
	double			term,				// time to expiration
	double			RangeVol			// volatility used to set up the grid
);

EXPORT_C_QUANT int	Q_FixGridPoints(
	double			*minGrid,			// minimum grid level
	double			*maxGrid,			// maximum grid level
	int				*nGrid,				// number of grid points
	int				*LowIndex,			// LowIndex
	int				*HighIndex,			// HighIndex
	double			a,					// first point to fix the grid
	double			b					// second point to fix the grid
);

EXPORT_C_QUANT int Q_CheckTermStructure(
	double		*spot,
	Q_VECTOR	*Volatilities,
	Q_VECTOR	*VolatilityTimes,
	Q_VECTOR	*Discounts,
	Q_VECTOR	*DiscountTimes,
	Q_VECTOR	*Forwards,
	Q_VECTOR	*ForwardTimes,
	Q_VECTOR	*Variances,
	Q_VECTOR	*VarianceTimes,
	Q_VECTOR	*LogDiscounts,
	Q_VECTOR	*LogDiscountTimes,
	Q_VECTOR	*LogForwards,
	Q_VECTOR	*LogForwardTimes
);

EXPORT_C_QUANT double Q_InterpolateDKOGrid(
   	double		Spot,			// value at which to interpolate
   	double		*V,				// values on the grid
   	double		MinGrid,		// minimum grid value
   	double		MaxGrid,		// maximum grid value
   	int			nGrid,			// number of points on the grid
   	double		Lower,			// lower knockout ( HUGE_VAL == no knockout)
   	double		Upper,			// upper knockout ( HUGE_VAL == no knockout) 
   	double		LowerRebate,	// lower rebate
   	double		UpperRebate		// upper rebate
);

EXPORT_C_QUANT double Q_InterpolateDKO(
	double		Spot,			// value at which to interpolate, NOT log space
	Q_VECTOR	*Values,        // values on the grid
	double		LogMinGrid,		// minimum grid value (in log space)
	double		LogMaxGrid,		// maximum grid value (in log space)
	double		Lower,			// lower knockout ( <= 0 for no knockout ), NOT log space
	double		Upper,			// upper knockout ( <= 0 for no knockout ), NOT log space
	double		LowerRebate,	// lower rebate
	double		UpperRebate		// upper rebate
);

EXPORT_C_QUANT int Q_CheckTermStructureAlt(
   	Q_LOG_MARKET_INFO	*MarketInfo,		// structure to return the data                     
   	double				Spot,               // the spot                                         
   	Q_VECTOR			*Volatilities,      // the volatilities                                 
   	Q_VECTOR			*VolatilityTimes,   // the times of the volatilities                    
   	Q_VECTOR			*Discounts,         // the discount factors                             
   	Q_VECTOR			*DiscountTimes,     // the times of the discounts                       
   	Q_VECTOR			*Forwards,          // the forwards                                     
   	Q_VECTOR			*ForwardTimes,      // the times of the forwards                        
   	MEM_POOL			*MemPool            // the mempool for allocating space for the vectors 
);

EXPORT_C_QUANT int Q_GetFDTimeVector(
	double		*Times,			// the vector of times to be filled in
	int			*nTimeSteps,	// the number of time steps returned
	double		StartTime,		// first time point
	double		EndTime,		// last time point
	double		DeltaT,			// required time step
	int			MinTimeSteps	// minimum number of time steps
);

EXPORT_C_QUANT int Q_TestKnockouts(
   	double		*LogLowerBarriers,		// vector to return logs of lower barriers
   	double		*LogUpperBarriers,		// vector to return logs of upper barriers
   	double		*MaxUpperBarrier,		// vector of maxes of upper barriers
   	double		*MinLowerBarrier,		// vector of min of lower barriers
   	double		*KnockoutTime, 			// returns time that we are certain to knockout (otherwise HUGE_VAL)
   	double		*Rebate,				// returns rebate when we are certain to knock out
   	double		LogSpot,				// logarithm of spot
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

EXPORT_C_QUANT int Q_FiniteDifferenceParamsMalloc(
	Q_FINITE_DIFFERENCE_PARAMS	*Params,	// the struct of arrays to allocate
	int							AllocSize,	// max size of arrays
	MEM_POOL					*MemPool	// mempool to be used for allocation
);

#endif
