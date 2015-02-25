/****************************************************************
**
**	BSPricerFourier.h	- definition of class BSPricerFourier
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.13 $
**
****************************************************************/

#if !defined( IN_BSPRICERFOURIER_H )
#define IN_BSPRICERFOURIER_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<qgrid/BasePricer.h>
#include	<qgrid/BSPricer.h>
#include	<qenums.h>
#include	<qgrid/enums.h>
#include	<qgrid/FourierGrid.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_QGRID BSPricerFourier : public BasePricer< FourierGrid >
{
public:
	// GsDt support:
	typedef	GsDtGeneric<BSPricerFourier> GsDtType;
	GsString		toString() const;
	inline static const char * typeName(){ return "BSPricerFourier"; }

	BSPricerFourier(
		double				Spot,						// spot
		const GsVector&		Discounts,					// discount factors
		const GsVector&		DiscountTimes,				// times of the discount factors
		const GsVector&		ForwardGrowth,				// forward growth factors
		const GsVector&		ForwardGrowthTimes,			// times of the forward growth factors
		const GsVector&		Volatilities,				// volatilities
		const GsVector&		VolatilityTimes,			// times of the volatilities
		const GsVector*		GridAnchoringVols = 0,		// grid anchoring vols
		const GsVector*		GridAnchoringVolTimes = 0	// times of the grid anchoring vols
	);

	BSPricerFourier( const BSPricerFourier& rhs );

	BSPricerFourier( const BSPricer& rhs );

	~BSPricerFourier();

	// const memberfns
	double						Spot() const { return exp( m_LogSpot ); }
	const GsFuncPiecewiseLinear*	LogDiscounts() const { return &m_LogDiscounts; }
	const GsFuncPiecewiseLinear*	LogForwardGrowth() const { return &m_LogForwardGrowth; }
	const GsFuncPiecewiseLinear*	Variances() const { return &m_Variances; }
	int							GridSize() const { return m_GridSize; }
	int							MultiplierSize() const { return m_TempFun.ncosines(); }
	int							FourierSize() const { return m_FourierSize; }
	int							ScalingFactor() const { return m_ScalingFactor; }
	bool						ResizeFlag() const { return m_Resize; }
	PRICER_RETURN_TYPE			ReturnType() const { return m_ReturnType; }
	double	Range( double Time ) const { return sqrt( m_AnchoringVariances( Time )); }
	void	GridRange( double Time, double& min, double& max ) const;
	void	GridRange( int nResizes, double& min, double& max ) const;
	int		NumberOfResizes( double Time ) const;
	void Resize( FourierGrid& grid, int nResizes ) const;

	// non-const memberfns
	void		  MultiplierSizeSet( int n ) { m_TempFun.resize( n, n ); }
	void		  GridSizeSet( int n ) { m_GridSize = n; }
	void		  FourierSizeSet( int n ) { m_FourierSize = n; }
	inline void	  TermSet( double t );
	void		  ScalingFactorSet( int n ) { m_ScalingFactor = n; m_LogScalingFactor = log( (double) n ); }
	void		  ResizeFlagSet( bool flag ) { m_Resize = flag; }
	void		  ReturnTypeSet( PRICER_RETURN_TYPE ReturnType ) { m_ReturnType = ReturnType; }

	// virtual member functions to inheret from BasePricer<>
	BasePricer< FourierGrid >* copy() const { return new BSPricerFourier( *this ); }

	void SetPayout( FourierGrid& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;

	void StepBack( FourierGrid& Grid, double s, double t, double LowerKO, double UpperKO,
                   double LowerRebate, double UpperRebate, double RebateTime ) const;

	double Price( const FourierGrid& Grid ) const;
	GsDt* Value( const FourierGrid& Grid, double Time, double LowerKO, double UpperKO,
				 double LowerRebate, double UpperRebate, double RebateTime ) const;

	GsString	ModelDescription() const;

	void Multiply( FourierGrid& grid, const GsFuncPiecewiseLinear& f, double t ) const;
	void AddGrids( FourierGrid& Grid, const FourierGrid& Grid2, double Time ) const;
	void AddProduct( FourierGrid& Grid, const FourierGrid& RHSGrid,
					 const GsFuncPiecewiseLinear& f, double t ) const;

	GsDt*   ToGsDt( const FourierGrid& Grid, double Time ) const
	{
		return Grid.toGsDt();
	}


private:
	double					m_LogSpot;
	GsFuncPiecewiseLinear		m_LogDiscounts;			// the log of 1/discount curve
	GsFuncPiecewiseLinear		m_LogForwardGrowth;		// the log of the forward growth curve
	GsFuncPiecewiseLinear		m_Variances;			// the variance curve
	GsFuncPiecewiseLinear		m_AnchoringVariances;	// the grid anchoring variance curve
	int						m_GridSize;				// number of grid points
	int						m_FourierSize;			// number of Fourier terms
	double					m_Term;					// last time
	double					m_FinalDev;				// standard dev at m_Term
	int						m_ScalingFactor;		// scaling factor
	double					m_LogScalingFactor;		// log of scaling factor
	bool					m_Resize;				// true = we resize the grid
	PRICER_RETURN_TYPE		m_ReturnType;			// grid, price, function

	GsVector 				m_TempVector;			// temp storage
	GsFuncFourierSeries		m_TempFun;				// temp storage function
	FourierGrid				m_TempGrid;				// temp grid

	inline GsVector& TempVector() const;
	inline GsFuncFourierSeries& TempFunction() const;
	inline FourierGrid& TempGrid() const;
};

GsVector& BSPricerFourier::TempVector() const
{
	return const_cast< GsVector& >( m_TempVector );
}

GsFuncFourierSeries& BSPricerFourier::TempFunction() const
{
	return const_cast< GsFuncFourierSeries& >( m_TempFun );
}

FourierGrid& BSPricerFourier::TempGrid() const
{
	return const_cast< FourierGrid& >( m_TempGrid );
}

void BSPricerFourier::TermSet(
	double t
)
{
	m_Term = t;
	m_FinalDev = sqrt( m_AnchoringVariances( t ));
}

CC_END_NAMESPACE

#endif

