/****************************************************************
**
**	qgrid/DVSolver.h	- Definition of class DVSolver
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: DVPricer.h,v $
**	Revision 1.53  2001/09/19 14:38:22  lowthg
**	removed ToGsDt so it inherits from Pricer1DGrid
**
**	Revision 1.52  2001/09/18 18:10:43  lowthg
**	added ToDictionary memberfunctions
**	
**	Revision 1.51  2001/08/31 19:45:29  lowthg
**	new functions to construct from a GsDtDictionary
**	
**	Revision 1.50  2001/08/29 23:14:45  lowthg
**	Added constructor from GsDtDictionary
**	
**	Revision 1.49  2001/08/29 20:31:01  lowthg
**	removed m_Spots member element of DVPricer
**	
**	Revision 1.48  2001/08/29 15:41:07  lowthg
**	changed DVPricer to inherit from Pricer1DGrid
**	
**	Revision 1.47  2001/08/27 17:09:35  lowthg
**	DV Pricer now inherits from BasePricer< SimpleVec >, not BasePricer< GsVector >
**	
**	Revision 1.46  2001/08/23 16:59:47  demeor
**	added prototypes for CN functions
**	
**	Revision 1.45  2001/08/22 15:21:14  lowthg
**	changed all uses of PiecewiseLinearFn to GsFuncPiecewiseLinear
**	
**	Revision 1.44  2000/10/13 18:14:09  lowthg
**	implemented clone member function
**	
**	Revision 1.43  2000/10/03 17:45:46  lowthg
**	Fixed ToGsDt function
**	
**	Revision 1.42  2000/10/03 16:48:58  lowthg
**	DVPricer now casts grid to a structure
**	
**	Revision 1.41  2000/09/28 19:10:18  lowthg
**	Pricer now stores the knockout segment info
**	
**	Revision 1.40  2000/09/22 13:58:22  lowthg
**	Added new class GsDtPricer which is a GsDt containing a Pricer, and allows
**	operators such as += to be used
**	
**	Revision 1.39  2000/08/24 20:48:19  lowthg
**	renamed FXPricer as BasePricer, FXGridPricer as PricerRep and
**	FXGridPricerT as PricerRepT
**	
**	Revision 1.38  2000/08/24 19:52:16  lowthg
**	removed FX1DPricer, as it is no longer needed
**	
**	Revision 1.37  2000/08/02 16:46:01  lowthg
**	Changed the QGridDVPricer_LocalVols and related functions to take in arbitrary
**	times and spots
**	
**	Revision 1.36  2000/07/18 22:15:10  lowthg
**	Added extra member functions to Pricer class
**	
**	Revision 1.35  2000/07/03 21:29:27  lowthg
**	Created a new class FXVolSurface which is now used by DVPricer to store the local
**	vols
**	
**	Revision 1.34  2000/06/29 13:47:33  lowthg
**	Added MaxGrids and MultiplyGrids to BSPricer
**	
**	Revision 1.33  2000/06/27 13:47:20  lowthg
**	Added extra operators on FXGrid class
**	
**	Revision 1.32  2000/06/23 15:31:03  lowthg
**	Corrected bugs with non-virtual destructors
**	
**	Revision 1.31  2000/06/22 20:12:33  lowthg
**	more implementation of DVPricer via FXGrid
**	
**	Revision 1.30  2000/06/22 16:30:28  lowthg
**	DVPricer and BSPricer now inherit from the templated class FXPricer< FX1DGrid >
**	
**	Revision 1.29  2000/06/20 16:25:02  lowthg
**	corrections to DVPricer::GetCallPrices
**	
**	Revision 1.28  2000/06/19 19:28:23  lowthg
**	corrections to GetCallPrices member function of DVPricer
**	
**	Revision 1.27  2000/06/19 16:40:37  lowthg
**	Extra member functions added to DVPricer to get discount and forward
**	growth factors
**	
**	Revision 1.26  2000/06/02 12:00:00  lowthg
**	started setting things up to create the grid pricing class
**	
**	Revision 1.25  2000/05/19 20:25:46  lowthg
**	minor changes to some memberfunctions
**	
**	Revision 1.24  2000/05/19 12:54:56  lowthg
**	added FX1DPricer class from which DVPricer now derives
**	
**	Revision 1.23  2000/05/18 13:03:22  lowthg
**	ModelDescription member function added
**	
**	Revision 1.22  2000/05/16 11:39:15  lowthg
**	extra constructir for DVPricer
**	
**	Revision 1.21  2000/05/08 11:34:08  lowthg
**	added the inline member function DVPricer::tAccuracy
**	
**	Revision 1.20  2000/05/08 10:44:21  lowthg
**	Modifications made to DVPricer::StepBack
**	
**	Revision 1.19  2000/05/05 16:59:04  lowthg
**	renamed the member m_Means to m_Rates
**	
**	Revision 1.18  2000/05/05 16:41:04  lowthg
**	Added a m_tAccuracy member function to DVPricer
**	
**	Revision 1.17  2000/05/04 14:26:53  lowthg
**	Removed DVPricer::ForwardsStepProbabilities
**	
**	Revision 1.16  2000/04/28 14:56:30  lowthg
**	Added the knockouts & rebates to DVPricer::StepBack
**	
**	Revision 1.15  2000/04/27 19:21:22  lowthg
**	Removed m_FDMethod member element from the DVPricer class.
**	
**	Revision 1.14  2000/04/27 12:15:55  lowthg
**	corrected compilation errors
**	
**	Revision 1.13  2000/04/27 11:59:56  lowthg
**	Added the StepBack member function
**	
**	Revision 1.12  2000/04/20 19:37:54  lowthg
**	Made changes to DVPricer class
**	
**	Revision 1.11  2000/04/19 15:49:26  lowthg
**	corrected bug with forward rates in CreateCNCoefficientsFromTermVars
**	
**	Revision 1.10  2000/02/29 16:41:28  lowthg
**	corrected declarations of memberfunctions
**	
**	Revision 1.9  2000/02/29 16:40:25  lowthg
**	corrected declarations of memberfunctions
**	
**	Revision 1.8  2000/02/03 20:53:11  lowthg
**	Updated the DVPricer class, and added a general Crank-Nicholson for the
**	forward equation.
**	
**	Revision 1.7  2000/02/03 13:58:20  lowthg
**	Added a Crank-Nicholson implied grid
**	
**	Revision 1.6  2000/02/01 19:02:35  lowthg
**	extensions to PiecewiseLinearFn
**	
**	Revision 1.5  2000/01/25 15:36:37  lowthg
**	Exported PiecewiseLinearFn
**	
**	Revision 1.4  2000/01/24 22:05:33  lowthg
**	Added PiecewiseLinearFn, KnockoutInfo (to replace KnockoutData) and
**	FXGrid (to replace FXGridSolver).
**	
**	Revision 1.3  2000/01/19 18:14:05  lowthg
**	Improvements to DVPricer class
**	
**	Revision 1.2  2000/01/18 18:13:38  lowthg
**	Made progress with the DVPricer class
**	
**	Revision 1.1  2000/01/17 22:13:54  lowthg
**	Added the DVPricer class and made changes to existinf FXGridModel &
**	KnockoutData classes.
**	
**
****************************************************************/

#if !defined( IN_QGRID_DVPRICER_H )
#define IN_QGRID_DVPRICER_H

#include	<qgrid/base.h>
#include	<gscore/GsMatrix.h>
#include	<qgrid/Pricer1DGrid.h>
#include	<qgrid/FXVolSurface.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_QGRID DVPricer : public Pricer1DGrid
{
	public:
		// GsDt support:
		typedef	GsDtGeneric<DVPricer> GsDtType;
		GsString		toString() const;
		inline static const char * typeName(){ return "DVPricer"; }

		inline DVPricer() {}

		DVPricer(
			double					Spot,	   					// spot
			const GsVector&			Discounts,					// discount factors
			const GsVector&			DiscountTimes,				// discount factors
			const GsVector&			ForwardGrowth,				// forward growth curve
			const GsVector&			ForwardGrowthTimes,			// forward growth curve
			const GsVector&			Times,						// times for call prices
			const GsVector&			Strikes,					// strikes for call prices
			const GsMatrix&			Calls						// call prices
		);

		DVPricer(
			double					Spot,	   					// spot
			const GsVector&			Discounts,					// discount factors
			const GsVector&			DiscountTimes,				// discount factor times
			const GsVector&			ForwardGrowth,				// forward growth curve
			const GsVector&			ForwardGrowthTimes,			// forward growth times
			const GsVector&			Times,						// times
			const GsVector&			Strikes,					// number of strikes
			const GsMatrix&			Data,						// call price surface or equivalent
			DVPRICER_IN_DATA_TYPE	DataType					// what Data represents ( DVPRICER_CALLS, DVPRICER_TERMVOLS, DVPRICER_TERMVARS )
		);

		DVPricer(
			double				Spot,						// spot
			const GsVector&		Discounts,					// discount factors
			const GsVector&		DiscountTimes,				// discount factor times
			const GsVector&		ForwardGrowth,				// forward growth factors
			const GsVector&		ForwardGrowthTimes,			// forward growth times
			const GsVector&		Times,						// times
			const GsVector&		Strikes,					// strikes
			const GsMatrix&		LocalVars,					// local var matrix
			const GsVector&		Spots						// spots at which to perform Crank-Nicholson
		);

		inline DVPricer(const GsDtDictionary& dict ) { FromDictionary( dict ); }

		DVPricer( const DVPricer& );
		~DVPricer();

		// const memberfunctions
		GsMatrix					GetCallPrices( double Spot, double Time, const GsVector& TimesOut,
                                                   const GsMatrix& StrikesOut ) const;
		GsMatrix					LocalVols() const;
		inline GsMatrix				LocalVols( const GsVector& Times, const GsVector& Spots ) const { return m_Vols.LocalVols( Times, Spots ); }
		inline double				MaxTime() const { return m_Vols.MaxTime(); }
		inline const GsVector&		SpotGrid() const { SetGrid( MaxTime() ); return TempGrid(); }
		inline const GsVector&		SpotGrid( double t ) const { SetGrid( t ); return TempGrid(); }
		void						StepForwards( double* Values, double s,double t	) const;
		inline const GsVector&		TimeGrid() const { return m_Vols.Times(); }
		GsDtDictionary*				ToDictionary() const;
		void						ToDictionary( GsDtDictionary& dict ) const;

		// non-const memberfunctions
		void FromDictionary( const GsDtDictionary& dict );


		// classes inheriting from Pricer1DGrid should give their own implementations of this
		void StepBack( double* Values, int nValues, double LogMinGrid, double LogMaxGrid,
                       double s, double t, double LowBarrier, double HighBarrier,
                       double LowRebate, double HighRebate, double RebateTime ) const;

		// necessary virtual member functions
		BasePricer< SimpleVec >* copy() const { return new DVPricer( *this ); }

		GsString	ModelDescription() const;

	protected:
		FXVolSurface		m_Vols;				// vol surface
};

EXPORT_CPP_QGRID void AdjustLowerBarrierCNCoefficients(
	double	Dxm,				// size of the grid step containing lower KO
	double	Dxp,				// size of the grid step above lower KO
	double	NewDxm,				// distance of lower KO from next grid point / LowerDxm
	double*	L,					// lower diagonal CN element at grid point after lower KO
	double*	U					// upper diagonal CN element at grid point after lower KO
	);

EXPORT_CPP_QGRID void SingleCNStep(
	double*			Values,			// the price grid, must be of size nGrid
	int				nGrid,			// size of grid
	double			LowRebate,		// the lower rebate
	double			HighRebate, 	// the upper rebate
	const double*	L,				// the lower diagonal components
	const double*	U,				// the upper diagional components
	double			Mean,			// the mean
	double*			D				// temporary storage vector, of size nGrid
	);

CC_END_NAMESPACE

#endif

