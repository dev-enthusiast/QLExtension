/****************************************************************
**
**	LocalVols.h	- local volatility functions
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_LOCALVOLS_H )
#define IN_LOCALVOLS_H

#include <qgrid/base.h>
#include <gscore/types.h>
#include <gsdt/GsDtArray.h>

CC_BEGIN_NAMESPACE( Gs )

struct MarkedVolSmile
{
	public:
		double			Time;
		double			VolTime;
		int				nStrikes;
		const double*	Strikes;
		const double*	Vols;
		double			ATMStrike;
		double			ATMVol;
};

struct MarkedVolSurface
{
	public:
		int				nMaturities;
		MarkedVolSmile*	Smiles;
};

EXPORT_CPP_QGRID GsMatrix QGridTermVarsToTermVols(
	const GsVector&	Times,		// the times at which we have the variances
	const GsMatrix& TermVars	// the variance matrix
);

EXPORT_CPP_QGRID GsMatrix QGridVarsByDeltaToVarsByStrike(
	double		   		MinDelta,			// minimum delta
	double			   	MaxDelta,			// maximum delta
	int		   			nDeltas,			// number of deltas
	const GsVector&		VolatilityTimes,	// times in the volatility time scale (weighted for weekends etc)
	const GsVector&		ATMStrikes,			// the strikes at the money
	const GsMatrix&		Vars,				// the variances by delta
	const GsVector&		Strikes,			// the strikes which we want
	double		   		LowDelta,			// vols are flat below this delta
	double		   		HighDelta			// vols are flat above this delta
);

EXPORT_CPP_QGRID GsMatrix QGridInterpVarsCubicSpline(
	const GsVector&			Deltas,				// deltas at which we want the variances
	const GsDtArray&		WingVols,			// Wing volatility information
	const GsVector&			VolatilityTimes,	// times in the volatility time scale (weighted for weekends etc)
	double					ExtrapolationFactor	// used to calculate point at which the vols flatten out 
);

EXPORT_CPP_QGRID GsMatrix QGridInterpVarsCubicSpline(
	const GsVector&			Deltas,				// deltas at which we want the variances
	const MarkedVolSurface&	WingVols,			// Wing volatility information
	const GsVector&			VolatilityTimes,	// times in the volatility time scale (weighted for weekends etc)
	double					ExtrapolationFactor	// used to calculate point at which the vols flatten out 
);

EXPORT_CPP_QGRID GsMatrix QGridLocalVolsFromTermVars(
	double					Spot,	   					// spot
	const GsVector&			Discounts,					// discount factors
	const GsVector&			ForwardGrowth,				// forward growth curve
	const GsVector&			Times,						// times
	const GsVector&			Strikes,					// the strikes
	const GsMatrix&			Vars						// term vars, Vars[ i, j ] = variance at t = times[ i ], x = Strikes[ j ] 
);

EXPORT_CPP_QGRID void CreateLocalVolsFromTermVars(
	double					Spot,	   					// spot
	const GsVector&			Discounts,					// discount factors
	const GsVector&			ForwardGrowth,				// forward growth curve
	const GsVector&			Times,						// times
	const GsVector&			Strikes,					// the strikes
	const GsMatrix&			Vars,						// term vars, Vars[ i, j ] = variance at t = times[ i ], x = Strikes[ j ] 
	GsMatrix&				LocalVols					// must be of size Times.size() - 1 by Strikes.size()		
);

EXPORT_CPP_QGRID void CreateLocalVarsFromTermVars(
	double					Spot,	   					// spot
	const GsVector&			Discounts,					// discount factors
	const GsVector&			ForwardGrowth,				// forward growth curve
	const GsVector&			Times,						// times
	const GsVector&			Strikes,					// the strikes
	const GsMatrix&			Vars,						// term vars, Vars[ i, j ] = variance at t = times[ i ], x = Strikes[ j ] 
	double*					LocalVars					// must be matrix of size Times.size() - 1 by Strikes.size() - 2
);

EXPORT_CPP_QGRID void CreateLocalVarsFromCalls(
	double					Spot,	   					// spot
	const GsVector&			Discounts,					// discount factors
	const GsVector&			ForwardGrowth,				// forward growth curve
	const GsVector&			Times,						// times
	const GsVector&			Strikes,					// number of strikes
	const GsMatrix&			Calls,						// call prices
	double*					LocalVars					// must be matrix of size Times.size() - 1 by Strikes.size() - 2
);

EXPORT_CPP_QGRID GsMatrix QGridLocalVarsFromTermVars(
	double					Spot,	   					// spot
	const GsVector&			Discounts,					// discount factors
	const GsVector&			ForwardGrowth,				// forward growth curve
	const GsVector&			Times,						// times
	const GsVector&			Strikes,					// the strikes
	const GsMatrix&			Vars						// term vars, Vars[ i, j ] = variance at t = times[ i ], x = Strikes[ j ] 
);

CC_END_NAMESPACE

#endif

