/****************************************************************
**
**	BSPricer.h	- definition of class BSPricer
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.54 $
**
****************************************************************/

#if !defined( IN_BSPRICER_H )
#define IN_BSPRICER_H

#include <qgrid/Pricer1DGrid.h>

CC_BEGIN_NAMESPACE( Gs )


class AverageRatePricer;

/***********************************************************************************
** Class BSPricer: Black-Scholes Model
***********************************************************************************/

class EXPORT_CLASS_QGRID BSPricer : public Pricer1DGrid
{
public:
	// GsDt support:
	typedef	GsDtGeneric<BSPricer> GsDtType;
	GsString		toString() const;
	inline static const char * typeName(){ return "BSPricer"; }

	inline BSPricer() : Pricer1DGrid() {}
	BSPricer(
		double				Spot,						// spot
		const GsVector&		Discounts,					// discount factors
		const GsVector&		DiscountTimes,				// times of the discount factors
		const GsVector&		ForwardGrowth,				// forward growth factors
		const GsVector&		ForwardGrowthTimes,			// times of the forward growth factors
		const GsVector&		Volatilities,				// volatilities
		const GsVector&		VolatilityTimes,			// times of the volatilities
		const GsVector*		GridAnchoringVols = 0,		// grid anchoring vols
		const GsVector*		GridAnchoringVolTimes = 0	// times of the grid anchoring vols
	):
		Pricer1DGrid( Spot, Discounts, DiscountTimes, ForwardGrowth, ForwardGrowthTimes,
					  GridAnchoringVols, GridAnchoringVolTimes )
	{ Construct( Volatilities, VolatilityTimes ); }


	BSPricer( const GsDtDictionary& dict ) { FromDictionary( dict ); }

	inline BSPricer( const BSPricer& rhs ) : Pricer1DGrid( rhs ), m_Variances( rhs.m_Variances ) {}

	~BSPricer() {}

	// const memberfns
	inline const GsFuncPiecewiseLinear&	Variances() const { return m_Variances; }

	void	StepBack( double* Values, double LogMinGrid, double LogMaxGrid,
					  double s, double t, double LowBarrier, double HighBarrier,
					  double LowRebate, double HighRebate, double RebateTime ) const;

	void	StepBack( double* Values, int GridSize, double LogMinGrid, double LogMaxGrid,
					  double s, double t, double LowBarrier, double HighBarrier,
					  double LowRebate, double HighRebate, double RebateTime,
					  double LogDiscount, double LogForward, double Variance,
					  double& NewLogDiscount, double& NewLogForward, double& NewVariance
					  ) const;

	GsDtDictionary* ToDictionary() const;
	void            ToDictionary( GsDtDictionary& dict ) const;

	void StepBack( double* Values, int nValues, double LogMinGrid, double LogMaxGrid,
                   double s, double t, double LowBarrier, double HighBarrier,
                   double LowRebate, double HighRebate, double RebateTime ) const;

	// virtual member functions inherited from BasePricer<>
	BasePricer< SimpleVec >* copy() const { return new BSPricer( *this ); }

	GsString	ModelDescription() const;

	void FromDictionary( const GsDtDictionary& dict );

private:

	friend class AverageRatePricer;

	GsFuncPiecewiseLinear	m_Variances;			// the variance curve

	void StepBack( double* Values, int gridsize, double LogMinGrid, double LogMaxGrid,
				   double s, double t, double dt, double dLogDiscount,
				   double dLogForward, double dVariance ) const;

	void Construct( const GsVector&	Volatilities, const GsVector& VolatilityTimes );
};


CC_END_NAMESPACE

#endif

