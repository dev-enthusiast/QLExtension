/****************************************************************
**
**	n_cn.h	- Crank-Nicholson routines
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_N_CN_H )
#define IN_N_CN_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif
#if !defined( IN_MEMPOOL_H )
#include <mempool.h>
#endif

typedef struct
{
	int			nTimeSteps;
	double*		Variances;
	double*		LogForwards;
	double*		LogDiscounts;
	double*		LowerRebates;
	double*		UpperRebates;
	int			AllocSize;
} Q_FINITE_DIFFERENCE_PARAMS;

typedef struct
{
	double		LogSpot;
	Q_VECTOR	Variances;
	Q_VECTOR	VarianceTimes;
	Q_VECTOR	LogForwards;
	Q_VECTOR	LogForwardTimes;
	Q_VECTOR	LogDiscounts;
	Q_VECTOR	LogDiscountTimes;
} Q_LOG_MARKET_INFO;

EXPORT_C_NUM int N_TriDiagFactorize(
	int		n,			// size of matrix
	double	a,			// negative of the lower diagonal elements
	double	c,			// negative of the upper diagonal componets
	double	*D			// vector in which to return the pivots
);

EXPORT_C_NUM void N_CNSubstitution(
	int		n,			// size of vector V
	double	*V,			// vector of prices
	double	a,			// lower diagonal component
	double	c,			// upper diagonal component
	double	*D			// vector of pivots (diagonal components of factorized FD matrix
);

EXPORT_C_NUM void N_CNStep(
	int		n,			// size of vector V
	double	*V,			// vector of prices
	double	a,			// lower diagonal component
	double	discount,			// diagonal component
	double	c,			// upper diagonal component
	double	*D			// vector of pivots (diagonal components of factorized FD matrix
);

EXPORT_C_NUM void N_ImplicitStepWithSource(
	double 		a,		// lower diagonal component
	double 		b,		// diagonal component
	double 		c,		// upper diagonal component
	int    		N,		// size of the matrix
	double		lowboundary,  // value at the lower boundary ( i = 0 )
	double		highboundary, // value at the upper boundary ( i = N - 1 )
	double		*V,		// input, output: solution
	double		*RHS,	// R.H.S. : av[ i - 1 ] + b v[ i ] + c v[ i + 1 ] = rhs
	double		*D 		// workspace
);


EXPORT_C_NUM void N_CNStepWithSource(
	double 		a,		// lower diagonal component
	double 		b,		// diagonal component
	double 		c,		// upper diagonal component
	int    		N,		// size of the matrix
	double		lowboundary,  // value at the lower boundary ( i = 0 )
	double		highboundary, // value at the upper boundary ( i = N - 1 )
	double		*V,		// input, output: solution
	double		*RHS,	// R.H.S. : av[ i - 1 ] + b v[ i ] + c v[ i + 1 ] = rhs
	double		*D 		// workspace
);


EXPORT_C_NUM int N_BarrierFactorize(
	int		n,			// size of matrix
	double	a,			// lower diagonal elements
	double	b,			// diagonal components
	double	c,			// upper diagonal componets
	double	Firstb,		// first diagonal component
	double	Firstc,		// first upper diagonal element
	double	Lasta,		// last lower diagonal element
	double	Lastb,		// last diagonal component
	double	*D			// vector in which to return the pivots
);

EXPORT_C_NUM void N_BarrierCNStep(
	int		n,			// size of vector V
	double	*V,			// vector of prices
	double	a,			// lower diagonal component
	double	b,			// diagonal component
	double	c,			// upper diagonal component
	double	Firsta,		// lower diagonal component near lower boundary
	double	Firstb,		// diagonal component near lower boundary
	double	Firstc,		// upper diagonal component near lower boundary
	double	Lasta,		// lower diagonal component near upper boundary
	double	Lastb,		// diagonal component near upper boundary
	double	Lastc,		// upper diagonal component near upper boundary
	double	*D			// vector of pivots (diagonal components of factorized FD matrix
);

EXPORT_C_NUM void N_BarrierCNSubstitution(
	int		n,			// size of vector V
	double	*V,			// vector of prices
	double	a,			// lower diagonal component
	double	b,			// diagonal component
	double	c,			// upper diagonal component
	double	Firsta,		// lower diagonal component near lower boundary
	double	Firstb,		// diagonal component near lower boundary
	double	Firstc,		// upper diagonal component near lower boundary
	double	Lasta,		// lower diagonal component near upper boundary
	double	Lastb,		// diagonal component near upper boundary
	double	Lastc,		// upper diagonal component near upper boundary
	double	*D			// vector of pivots (diagonal components of factorized FD matrix
);

EXPORT_C_NUM int N_DKOSolve(
	double	*V,			// price vector(input as the payout as a function of the log of the spot, output is the present cost)
	double	*vars,		// the variances (i.e. the integral of the squared instantaneous volatility
	double	*dis,		// logarithm of the discount factors (-rt)
	double	*disfwd,	// log of the forward curve ((r-g)t)
	double	minGrid,	// min value of log(Spot) in the grid
	double	maxGrid,	// max value of log(Spot) in the grid
	double	*lowerRebates,	// rebates at top of grid (paid out immediately - pre-discount for later payout)
	double	*upperRebates,	// rebates at bottom of grid (as above)
	int		nGrid,		// number of grid points
	int		nSteps,		// number of time steps
	double	*D			// Temporary storage vector of size at least nGrid
);

EXPORT_C_NUM int N_BulkDKOSolve(
	double	**V,			// pointer to the price vectors(input as the payout as a function of the log of the spot, output is the present cost)
	int		nLayers,		// number of grids on which to apply Crank Nicholson
	double	*vars,			// the variances (i.e. the integral of the squared instantaneous volatility
	double	*disfwd,		// log of the forward curve ((r-g)t)
	double	minGrid,		// min value of log(Spot) in the grid
	double	maxGrid,		// max value of log(Spot) in the grid
	double	*lowerRebates,	// rebates at top of grid (paid out immediately - pre-discount for later payout), can be NULL
	double	*upperRebates,	// rebates at bottom of grid (as above)
	int		nGrid,			// number of grid points
	int		nSteps,			// number of time steps
	double	*D				// pointer to a temporary storage vector of size at least nGrid
);

EXPORT_C_NUM int N_BulkDKOSolveSmoothed(
	double	**V,			// pointer to the price vectors(input as the payout as a function of the log of the spot, output is the present cost)
	int		nLayers,		// number of grids on which to apply Crank Nicholson
	double	*vars,			// the variances (i.e. the integral of the squared instantaneous volatility
	double	*disfwd,		// log of the forward curve ((r-g)t)
	double	minGrid,		// min value of log(Spot) in the grid
	double	maxGrid,		// max value of log(Spot) in the grid
	double	*lowerRebates,	// rebates at top of grid (paid out immediately - pre-discount for later payout), can be NULL
	double	*upperRebates,	// rebates at bottom of grid (as above)
	int		nGrid,			// number of grid points
	int		nSteps,			// number of time steps
	int		nImp,			// number of implicit time steps
	double	*D				// pointer to a temporary storage vector of size at least nGrid
);

EXPORT_C_NUM int N_BulkCNSolve(
	double	**V,			// pointer to the price vectors(input as the payout as a function of the log of the spot, output is the present cost)
	int		nLayers,		// number of grids on which to apply Crank Nicholson
	double	var,			// the variation over the time period
	double	drift,			// net drift across the time period (difference of the logs of the forwards)
	double	minGrid,		// min value of log(Spot) in the grid
	double	maxGrid,		// max value of log(Spot) in the grid
	int		nGrid,			// number of grid points
	int		nSteps,			// number of time steps
	double	*D				// pointer to a temporary storage vector of size at least nGrid
);

EXPORT_C_NUM int N_BulkCNSolveSmooth(
	double	**V,			// pointer to the price vectors(input as the payout as a function of the log of the spot, output is the present cost)
	int		nLayers,		// number of grids on which to apply Crank Nicholson
	double	var,			// the variation over the time period
	double	drift,			// net drift across the time period (difference of the logs of the forwards)
	double	minGrid,		// min value of log(Spot) in the grid
	double	maxGrid,		// max value of log(Spot) in the grid
	int		nGrid,			// number of grid points
	int		nSteps,			// number of time steps
	double	*D				// pointer to a temporary storage vector of size at least nGrid
);

EXPORT_C_NUM int N_DKOBarrierSolve(
	double	*V,					// price vector(input as the payout as a function of the log of the spot, output is the present cost)
	double	*vars,				// the variances (i.e. the integral of the squared instantaneous volatility
	double	*dis,				// logarithm of the discount factors (-rt)
	double	*disfwd,			// log of the forward curve ((r-g)t)
	double	minGrid,			// min value of log(Spot) in the grid
	double	maxGrid,			// max value of log(Spot) in the grid
	double	LowBarrier,			// log(lower barrier)
	double	HighBarrier,		// log(upper barrier)
	double	*lowerRebates,		// rebates at upper barrier (paid out immediately - pre-discount for later payout)
	double	*upperRebates,		// rebates at lower barrier (as above)
	int		nGrid,				// number of grid points
	int		nSteps,				// number of time steps
	double	*D					// pointer to a temporary storage vector of size at least nGrid
);

EXPORT_C_NUM void N_ImplicitStepAlt(
	int		nGridSteps,	// size of vector V - 1, must be >= 1
	double	*V,			// vector of prices
	double	a,			// lower diagonal component
	double	b,			// diagonal component
	double	c,			// upper diagonal component
	double	Firsta,		// lower diagonal component near lower boundary
	double	Firstb,		// diagonal component near lower boundary
	double	Firstc,		// upper diagonal component near lower boundary
	double	Lasta,		// lower diagonal component near upper boundary
	double	Lastb,		// diagonal component near upper boundary
	double	Lastc,		// upper diagonal component near upper boundary
	double	LowerValue,	// new value at lower barrier
	double	UpperValue, // new value at upper barrier
	double	*D			// vector of pivots (diagonal components of factorized FD matrix
);

EXPORT_C_NUM void N_BarrierCNStepAlt(
	int		nGridSteps,	// size of vector V - 1, must be >= 1
	double	*V,			// vector of prices
	double	a,			// lower diagonal component
	double	b,			// diagonal component
	double	c,			// upper diagonal component
	double	Firsta,		// lower diagonal component near lower boundary
	double	Firstb,		// diagonal component near lower boundary
	double	Firstc,		// upper diagonal component near lower boundary
	double	Lasta,		// lower diagonal component near upper boundary
	double	Lastb,		// diagonal component near upper boundary
	double	Lastc,		// upper diagonal component near upper boundary
	double	LowerValue,	// new value at lower barrier
	double	UpperValue, // new value at upper barrier
	double	*D			// vector of pivots (diagonal components of factorized FD matrix
);

EXPORT_C_NUM int N_DKOBarrierSolveAlt(
	double	*V,					// price vector(input as the payout as a function of the log of the spot, output is the present cost)
	double	*vars,				// the variances (i.e. the integral of the squared instantaneous volatility
	double	*dis,				// logarithm of the discount factors (-rt)
	double	*disfwd,			// log of the forward curve ((r-g)t)
	double	minGrid,			// min value of log(Spot) in the grid
	double	maxGrid,			// max value of log(Spot) in the grid
	double	LowBarrier,			// log(lower barrier) HUGE_VAL == no barrier
	double	HighBarrier,		// log(upper barrier) HUGE_VAL == no barrier
	double	*lowerRebates,		// rebates at upper barrier (paid out immediately - pre-discount for later payout)
	double	*upperRebates,		// rebates at lower barrier (as above)
	int		nGrid,				// number of grid points
	int		nSteps,				// number of time steps
	double	*D					// pointer to a temporary storage vector of size at least nGrid
);

EXPORT_C_NUM int N_DKOBarrierSolveCN(
	double	*V,					// price vector(input as the payout as a function of the log of the spot, output is the present cost)
	double	*vars,				// the variances (i.e. the integral of the squared instantaneous volatility
	double	*dis,				// logarithm of the discount factors (-rt)
	double	*disfwd,			// log of the forward curve ((r-g)t)
	double	minGrid,			// min value of log(Spot) in the grid
	double	maxGrid,			// max value of log(Spot) in the grid
	double	LowBarrier,			// log(lower barrier)
	double	HighBarrier,		// log(upper barrier)
	double	*lowerRebates,		// rebates at upper barrier (paid out immediately - pre-discount for later payout)
	double	*upperRebates,		// rebates at lower barrier (as above)
	int		nGrid,				// number of grid points
	int		nSteps,				// number of time steps
	double	*D					// pointer to a temporary storage vector of size at least nGrid
);

EXPORT_C_NUM int N_FiniteDifferenceParamsMalloc(
   	Q_FINITE_DIFFERENCE_PARAMS	*Params,	// the struct of arrays to allocate  
   	int							AllocSize,  // max size of arrays                
   	MEM_POOL					*MemPool    // mempool to be used for allocation 
);

EXPORT_C_NUM int N_GetFiniteDifferenceParams(
   	Q_FINITE_DIFFERENCE_PARAMS	*Params,		// the finite difference params to be returned
   	Q_LOG_MARKET_INFO			*MarketInfo,	// the inputted data
   	double						*Times,			// the times at which to interpolate
   	int							nTimes			// the size of Times
);

EXPORT_C_NUM int N_StepBackDKO(
		double						*V,				// price grid
		double						MinGrid,		// min grid range
		double						MaxGrid,		// max grid range
		int							nGrid,			// number of grid points
		double						*Times,			// time points
		int							nTimeSteps,		// number of time steps
		Q_LOG_MARKET_INFO			*MarketInfo,	// Market data
		Q_FINITE_DIFFERENCE_PARAMS	*FDParams,		// temp storage (at least as big as nTimeSteps + 1 )
		double						Lower,			// lower knockout
		double						Upper,			// upper knockout
		double						LowerRebate,	// lower rebate
		double						UpperRebate,	// upper rebate
		double						Settlement,		// settlement time for rebates ( <= 0 is immediate settlement )
		double						*D				// temp storage of size nGrid
);

EXPORT_C_NUM int N_DKOSolveSpaceSlice(
	double			*V,					// price vector(input as the payout as a function of the log of the spot, output is the present cost)
	double			Spot,				// the spot value for the slice of prices
	double			*Slice,				// returns a price at spot for each time point
	double			*vars,				// the variances (i.e. the integral of the squared instantaneous volatility
	double			*dis,				// logarithm of the discount factors (-rt)
	double			*disfwd,			// log of the forward curve ((r-g)t)
	double			minGrid,			// min value of log(Spot) in the grid
	double			maxGrid,			// max value of log(Spot) in the grid
	double			LowBarrier,			// log(lower barrier)
	double			HighBarrier,		// log(upper barrier)
	double			LowerRebate,		// rebate at lower barrier
	double			UpperRebate,		// rebate at upper barrier
	Q_REBATE_TYPE	RebateType,			// immediate or delayed until the end of the time period
	int				nGrid,				// number of grid points
	int				nSteps,				// number of time steps
	double			*D					// pointer to a temporary storage vector of size at least nGrid
);

EXPORT_C_NUM void N_ImplicitStep(
	int		n,			// size of vector V
	double	*V,			// vector of prices
	double	a,			// lower diagonal component
	double	b,			// diagonal component
	double	c,			// upper diagonal component
	double	*D			// vector of pivots (diagonal components of factorized FD matrix
);


#endif

