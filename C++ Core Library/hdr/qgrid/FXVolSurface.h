/****************************************************************
**
**	FXVolSurface.h	- definition of FXVolSurface class
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_FXVOLSURFACE_H )
#define IN_FXVOLSURFACE_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gscore/GsMatrix.h>
#include	<gscore/GsVector.h>
#include	<qenums.h>
#include	<qgrid/enums.h>
#include	<gsquant/GsFuncPiecewiseLinear.h>

CC_BEGIN_NAMESPACE( Gs )

class FXVolSurface
{
	public:
		FXVolSurface() {}

		FXVolSurface(
			const GsVector&				Times,						// times
			const GsVector&				Strikes,					// strikes
			const GsMatrix&				LocalVars					// local var matrix
		);

		FXVolSurface(
			double						Spot,	   					// spot
			const GsFuncPiecewiseLinear&	LogDiscounts,				// discount factors
			const GsFuncPiecewiseLinear&	LogForwardGrowth,			// forward growth curve
			const GsVector&				Times,						// times
			const GsVector&				Strikes,					// number of strikes
			const GsMatrix&				Data,						// call price surface or equivalent
			DVPRICER_IN_DATA_TYPE		DataType					// what Data represents ( DVPRICER_CALLS, DVPRICER_TERMVOLS, DVPRICER_TERMVARS )
		);

		FXVolSurface(
			double						Spot,	   					// spot
			const GsFuncPiecewiseLinear&	LogDiscounts,				// discount factors
			const GsFuncPiecewiseLinear&	LogForwardGrowth,			// forward growth curve
			const GsVector&				Times,						// times
			const GsVector&				Strikes,					// number of strikes
			const GsMatrix&				Calls						// call prices
		);

		inline ~FXVolSurface() {}

		// const member functions

		void GetCumLocalVars(
			double			Time,
			const double*	Spots,
			int				nSpots,
			double*			Vars
		) const;

		void GetCumulativeCNCoefficients(
			double			time,
			const GsVector&	Spots,
			double			ForwardRate,
			double*			L,
			double*			U
		) const;

		GsMatrix				LocalVols() const;
		GsMatrix				LocalVols( const GsVector& Times, const GsVector& Spots ) const;
		inline double			MaxTime() const { return m_Times[ m_Times.size() - 1 ]; }
		inline const GsVector&	Spots() const { return m_Spots; }
		inline const GsVector&	Times() const { return m_Times; }
		void 					ToDictionary( GsDtDictionary& dict ) const;
	
		// non-const member functions
		void 				FromDictionary( const GsDtDictionary& dict );
		inline GsVector&	Spots() { return m_Spots; }
		inline GsVector&	Times() { return m_Times; }

	private:
		GsMatrix	m_CumLocalVars;	// the cumulative local vars
		GsVector	m_Spots;		// the spots
		GsVector	m_Times;		// the times
};

CC_END_NAMESPACE

#endif

