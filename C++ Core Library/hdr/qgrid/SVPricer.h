/****************************************************************
**
**	SVPricer.h	- definitition of class SVPricer
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.22 $
**
****************************************************************/

#if !defined( IN_STOCHVOLPRICER_H )
#define IN_STOCHVOLPRICER_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gscore/GsVector.h>
#include	<gscore/GsMatrix.h>
#include	<qgrid/BasePricer.h>
#include	<qgrid/FXHandle.h>
#include	<qgrid/TimeSliceFn2D.h>
#include	<qgrid/FullGrid.h>
#include	<n_adi.h>
#include	<qenums.h>
#include	<qgrid/enums.h>

CC_BEGIN_NAMESPACE( Gs )

/***********************************************************************************
** Class SVPricer: Stochastic Vol Model
**
**  uses model dS/S = (r-g) * dt + a(S,t) * f( V, t ) * dW1
**	where the stochastic vol component V satisfies a one factor SDE
**				dV = mu( V, t ) * dt + gamma( V, t ) * dW2
** here W1 and W2 are Brownian motions, possibly with some correlation
**
***********************************************************************************/

class EXPORT_CLASS_QGRID SVPricer : public BasePricer< FullGrid >
{
	public:
		// GsDt support:
		typedef	GsDtGeneric<SVPricer> GsDtType;
		GsString		toString() const;
		inline static const char * typeName(){ return "SVPricer"; }

		SVPricer();

		SVPricer(
			SV_MODEL_TYPE	Type,						// must be HESTON
			double			Spot,						// the spot
			double			InitialVol,					// initial vol
			double			LongVol,					// mean reversion level
			double			beta,						// mean reversion rate
			double			sigma,						// vol of vol
			double			rho,						// correlation
			const GsVector&	ForwardGrowth,				// forward growth factors
			const GsVector& ForwardGrowthTimes,			// times of forward growth factors
			const GsVector& Discounts,					// discount factors
			const GsVector& DiscountTimes,				// times of discount factors
			const GsVector&	GridAnchoringVols,			// grid anchoring vols
			const GsVector&	GridAnchoringVolTimes		// times of the grid anchoring vols
		);

		~SVPricer();

		GsMatrix			GetDiscRates( double MinLogSpot, double MaxLogSpot, int nSpots, double MinV, double MaxV, int nV, double s, double t ) const
		{ return m_DiscRates( s, t, MinLogSpot, MinV, MaxLogSpot, MaxV, nSpots, nV ); }
		GsMatrix			GetDrift( double MinLogSpot, double MaxLogSpot, int nSpots, double MinV, double MaxV, int nV, double s, double t ) const
		{ return m_Drift( s, t, MinLogSpot, MinV, MaxLogSpot, MaxV, nSpots, nV ); }
		GsMatrix			GetVolFn( double MinLogSpot, double MaxLogSpot, int nSpots, double MinV, double MaxV, int nV, double s, double t ) const
		{ return m_VolFn( s, t, MinLogSpot, MinV, MaxLogSpot, MaxV, nSpots, nV ); }
		GsMatrix			GetVolDrift( double MinLogSpot, double MaxLogSpot, int nSpots, double MinV, double MaxV, int nV, double s, double t ) const
		{ return m_VolDrift( s, t, MinLogSpot, MinV, MaxLogSpot, MaxV, nSpots, nV ); }
		GsMatrix			GetVolVol( double MinLogSpot, double MaxLogSpot, int nSpots, double MinV, double MaxV, int nV, double s, double t ) const
		{ return m_VolVol( s, t, MinLogSpot, MinV, MaxLogSpot, MaxV, nSpots, nV ); }
		GsMatrix			GetSVCorrelations( double MinLogSpot, double MaxLogSpot, int nSpots, double MinV, double MaxV, int nV, double s, double t ) const
		{ return m_SVCorrelations( s, t, MinLogSpot, MinV, MaxLogSpot, MaxV, nSpots, nV ); }

		double	Spot() const { return m_Spot; }
		double  V() const { return m_V; }
		void    SetSpot( double Spot ) { m_Spot = Spot; }
		void    SetV( double V ) { m_V = V; }
		int		SpotGridSize() const { return m_SpotGridSize; }
		void	SpotGridSizeSet( int nGrid ) { m_SpotGridSize = nGrid; }
		int		VolGridSize() const { return m_VolGridSize; }
		void	VolGridSizeSet( int nGrid ) { m_VolGridSize = nGrid; }
		size_t  GridSize() const { return m_SpotGridSize * m_VolGridSize; }
		double	tAccuracy() const { return m_tAccuracy; }
		void	tAccuracySet( double Accuracy ) { m_tAccuracy = Accuracy; }
		double  Tolerance() const { return m_StepTolerance; }
		void	ToleranceSet( double Tol ) { m_StepTolerance = Tol; }
		N_FDMETHOD FDMethod() const { return m_FDMethod; }
		void	FDMethodSet( N_FDMETHOD method ) { m_FDMethod = method; }

		void	Resize( FullGrid& grid, double Time ) const;

		/////////////// set grid to function of spot ////////////////
		void SetPayout(
			GsVector& Values,						// the grid values
			double 			MinLogSpot,				// the min of the spot values (in log space)
			double			MaxLogSpot,				// the max of the spot values (in log space)
			int				nSpots,					// the number of spots
			int				nVols,					// the number of vols
			const 			GsFuncPiecewiseLinear& f	// the payout
		) const;

		////////// interpolate off grid with cubic spline /////////////
		double Price(
			const GsVector& Values,
			double			MinLogSpot,             // the min of the spot values (in log space) 
			double			MaxLogSpot,				// the max of the spot values (in log space)
			int				nSpots,					// the number of spots
			double			MinV,					// the min of V
			double			MaxV,					// the max of V
			int				nVols					// the number of vols
		) const;

		////////// step in time /////////////
		void Step(
			double* 		Values,
			double			MinLogSpot,             // the min of the spot values (in log space) 
			double			MaxLogSpot,				// the max of the spot values (in log space)
			int				nSpots,					// the number of spots
			double			MinV,					// the min of V
			double			MaxV,					// the max of V
			int				nVols,					// the number of vols
			double			LowRebate,				// rebate at min spot
			double			HighRebate,				// rebate at max spot
			double			s,						// initial time
			double			t,						// final time
			bool			Backwards				// true for backwards, false for forwards
		) const;

		////////// step back in time /////////////
		void StepBack(
			double* 		Values,					// grid values
			double			MinLogSpot,             // the min of the spot values (in log space) 
			double			MaxLogSpot,				// the max of the spot values (in log space)
			int				nSpots,					// the number of spots
			double			MinV,					// the min of V
			double			MaxV,					// the max of V
			int				nVols,					// the number of vols
			double			LowRebate,				// rebate at min spot
			double			HighRebate,				// rebate at max spot
			double			s,						// initial time
			double			t						// final time
		) const
		{
			Step( Values, MinLogSpot, MaxLogSpot, nSpots, MinV, MaxV, nVols, LowRebate,
                  HighRebate, s, t, true );
		}

		////////// step forwards in time /////////////
		void StepForwards(
			double* 		Values,					// grid values
			double			MinLogSpot,             // the min of the spot values (in log space) 
			double			MaxLogSpot,				// the max of the spot values (in log space)
			int				nSpots,					// the number of spots
			double			MinV,					// the min of V
			double			MaxV,					// the max of V
			int				nVols,					// the number of vols
			double			s,						// initial time
			double			t						// final time
		) const
		{
			Step( Values, MinLogSpot, MaxLogSpot, nSpots, MinV, MaxV, nVols, 0.,
                  0., s, t, false );
		}

		void GetInitialDensity(
			GsVector&	Values,					// grid density values
			double		MinLogSpot,             // the min of the spot values (in log space) 
			double		MaxLogSpot,				// the max of the spot values (in log space)
			int			nSpots,					// the number of spots
			double		MinV,					// the min of V
			double		MaxV,					// the max of V
			int			nVols					// the number of vols
		) const;

		double	SpotRange( double Time ) const { return sqrt( m_AnchoringVariances( Time )); }
		void  	GetLogSpotRange( double Time, double& MinLogSpot, double& MaxLogSpot ) const;
		void  	GetVolRange( double Time, double& MinV, double& MaxV ) const;

		// required member functions for generic pricer stuff
		BasePricer< FullGrid >* copy() const { return new SVPricer( *this ); }
		void   SetPayout( FullGrid& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;
		void   StepBack( FullGrid& Grid, double s, double Time, double LowerKO, double UpperKO,
                         double LowerRebate, double UpperRebate, double RebateTime ) const;
		double Price( const FullGrid& Grid ) const;
		void   Multiply( FullGrid& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;
		void   MaxDbl( FullGrid& Grid, double a, double Time ) const;
        
		GsString	ModelDescription() const;

	private:
		double				m_Spot;					// the initial spot
		double				m_V;					// the initial 'vol'
		GsFuncPiecewiseLinear	m_LogDiscounts;			// the log of 1/discount curve
		GsFuncPiecewiseLinear	m_LogForwardGrowth;		// the log of the forward growth curve
		GsFuncPiecewiseLinear	m_AnchoringVariances;	// grid anchoring variance curve
		GsFuncPiecewiseLinear	m_AnchoringVolVars;		// vol grid anchoring vol variance curve
		GsFuncPiecewiseLinear	m_VolGrowthFactors;		// vol grid growth factors
		double				m_MinV;					// minimum value of V on the grid
		double				m_MaxV;					// maximum value of V on the grid
		int					m_SpotGridSize;			// spot grid size
		int					m_VolGridSize;			// vol grid size
		double				m_tAccuracy;			// time discretization factor
		double				m_StepTolerance;		// tolerance used for the Crank-Nicolson in each step

		TimeSliceFn2D 		m_DiscRates;			// discount rates
		TimeSliceFn2D 		m_Drift;				// drift of spot
		TimeSliceFn2D 		m_VolFn;				// part of vol due to function of V and S
		TimeSliceFn2D		m_VolDrift;				// drift of vol
		TimeSliceFn2D 		m_VolVol;				// vol of vol
		TimeSliceFn2D  		m_SVCorrelations;		// correlations of spot and vol

		N_FDMETHOD			m_FDMethod;				// type of solver ( EXPLICIT,ADI, etc )

//		FXVolSurface		m_Vols;					// vol surface
};

CC_END_NAMESPACE

#endif

