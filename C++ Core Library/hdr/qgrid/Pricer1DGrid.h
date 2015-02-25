/****************************************************************
**
**	Pricer1DGrid.h	- 1D grid pricer, assumes interest rates
**						are not stochastic
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_PRICER1DGRID_H )
#define IN_PRICER1DGRID_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gscore/GsVector.h>
#include	<qgrid/BasePricer.h>
#include 	<gscore/GsString.h>
#include	<qgrid/SimpleVec.h>
#include	<qgrid/QGridTimeSteps.h>
#include	<qgrid/QGridConversions.h>
#include	<qgrid/GsFuncDKOGrid.h>
#include	<qenums.h>
#include	<qgrid/enums.h>

CC_BEGIN_NAMESPACE( Gs )

#define PRICER1DGRID_MINGRIDTIME    0.01

class EXPORT_CLASS_QGRID Pricer1DGrid : public BasePricer< SimpleVec >
{
public:
	Pricer1DGrid();

	Pricer1DGrid(
		double				Spot,					// spot
		const GsVector&		Discounts,				// discount factors
		const GsVector&		DiscountTimes,			// times of the discount factors
		const GsVector&		ForwardGrowth,			// forward growth factors
		const GsVector&		ForwardGrowthTimes,		// times of the forward growth factors
		const GsVector*		GridAnchoringVols,		// grid anchoring vols
		const GsVector*		GridAnchoringVolTimes	// times of the grid anchoring vols
	)
	{ Construct( Spot, Discounts, DiscountTimes, ForwardGrowth,
                 ForwardGrowthTimes, GridAnchoringVols, GridAnchoringVolTimes ); }

	Pricer1DGrid( const Pricer1DGrid& rhs );

	~Pricer1DGrid() {}
    
	// const memberfns
	void			CachedGridRange( double& logmin, double& logmax ) const;
	inline int 	  	CachednResizes( double Time ) const;
	inline double   DiscountFactor( double t ) const { return exp( -m_LogDiscounts( t ) );}
	inline double   ForwardGrowthFactor( double t ) const { return exp( m_LogForwardGrowth( t ) );}
	void			GridRange( double Time, double& min, double& max ) const;
	void			GridRange( int n, double& logmin, double& logmax ) const;
	inline double 	GridResizeRate() const { return exp( -m_ResizingParam ); }
	inline int		GridSize() const { return m_GridSize; }
	inline double	LogSpot() const { return m_LogSpot; }
	inline int    	MinInitialSteps() const { return m_MinInitialSteps; }
	double 			MinRange() const { return m_MinRange; }
	double 			MaxRange() const { return m_MaxRange; }
	inline int    	MinSteps() const { return m_MinSteps; }
	inline int 	  	nResizes( double Time ) const;
	double 			nStdDevs() const { return m_nStdDevs; }
	inline double 	Range( double Time ) const { return sqrt( m_AnchoringVariances( Time )); }
	inline double	Spot() const { return exp( m_LogSpot ); }
	inline double	tAccuracy() const { return m_TimeSteps.Accuracy(); }
	void 			ToDictionary( GsDtDictionary& dict ) const;

	PRICER_RETURN_TYPE 					ReturnType() const { return m_ReturnType; }
	inline const QGridTimeSteps&		TimeSteps() const { return m_TimeSteps; }
	inline const GsVector& 				Spots( double Time ) const { SetGrid( Time ); return m_TempGrid; }
	inline const GsFuncPiecewiseLinear&	AnchoringVariances() const { return m_AnchoringVariances; }
	inline const GsFuncPiecewiseLinear&	LogDiscounts() const { return m_LogDiscounts; }
	inline const GsFuncPiecewiseLinear&	LogForwardGrowth() const { return m_LogForwardGrowth; }

	void GridFromLinear( GsVector& grid, double Value0, double Value1, double time ) const;
	void GridFromLinear( GsVector& grid, double Value0, double Value1, double LogMin, double LogMax ) const;


	GsFuncDKOGrid* ToFunction(
		const SimpleVec& Grid,
		double Time,
		double LowerKO,
		double UpperKO,
		double LowerRebate,
		double UpperRebate,
		double RebateTime
	) const;

	// non-const memberfns
	void			FromDictionary( const GsDtDictionary& dict );
	void 			GridResizeRateSet( double a ) { m_cache.destroy(); m_ResizingParam = -log( a ); }
	inline void 	GridSizeParamsSet( double LogMinRange, double LogMaxRange, double nStdDevs );
	void 			GridSizeSet( int Size ) { m_cache.destroy(); m_GridSize = Size; m_TempGrid.resize( m_GridSize ); }
	void 			MinInitialStepsSet( int n ) { m_MinInitialSteps = n; }
	void 			MinRangeSet( double LogMinRange ) { m_MinRange = LogMinRange; }
	void 			MinStepsSet( int n ) { m_MinSteps = n; }
	void 			MaxRangeSet( double LogMaxRange ) { m_MaxRange = LogMaxRange; }
	void 			nStdDevsSet( double nStdDevs ) { m_nStdDevs = nStdDevs; }
	inline void 	ReturnTypeSet( PRICER_RETURN_TYPE ReturnType ) { m_ReturnType = ReturnType; }
	void 			ReturnTypeSet( const char* ReturnType );
	inline void 	ReturnTypeSet( const GsString& ReturnType ) { ReturnTypeSet( ReturnType.c_str() ); }
	void          	SpotSet( double S ) { m_LogSpot = log( S ); }
	void 			tAccuracySet( double a ) { m_TimeSteps.SetAccuracy( a ); }
	QGridTimeSteps&	TimeSteps() { return m_TimeSteps; }

	// classes inheriting from Pricer1DGrid should give their own implementations of this
	virtual void StepBack( double* Values, int nValues, double LogMinGrid, double LogMaxGrid,
                           double s, double t, double LowBarrier, double HighBarrier,
                           double LowRebate, double HighRebate, double RebateTime ) const;

	// virtual member functions inherited from BasePricer<>
	void StepBack( SimpleVec& Grid, double s, double t, double LowerKO, double UpperKO,
				   double LowerRebate, double UpperRebate, double RebateTime ) const;

	BasePricer< SimpleVec >* copy() const { return new Pricer1DGrid( *this ); }

	void 	SetPayout( SimpleVec& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;

	double 	Price( const SimpleVec& Grid ) const;

	double 	Price( const SimpleVec& Grid, double LowerKO, double UpperKO, double LowerRebate,
				  double UpperRebate, double RebateTime ) const;


	void	Multiply( SimpleVec& grid, const GsFuncPiecewiseLinear& f, double t ) const;
	void	MaxDbl( SimpleVec& Grid, double a, double t ) const;
	void	AddGrids( SimpleVec& grid1, const SimpleVec& grid2, double t ) const;
	void	SubGrids( SimpleVec& grid1, const SimpleVec& grid2, double t ) const;

	GsDt*   ToGsDt( const SimpleVec& Grid, double Time ) const;
	GsDt*	Value( const SimpleVec& Grid, double Time, double LowerKO, double UpperKO,
				   double LowerRebate, double UpperRebate, double RebateTime ) const;

	void 	AddProduct( SimpleVec& Grid, const SimpleVec& RHSGrid,
                        const GsFuncPiecewiseLinear& f, double t ) const;

	class Cache
	{
	public:
		Cache() { destroy(); }

		inline double Time() const { return m_Time; }
		inline int nResizes() const { return m_nResizes; }
		inline void GetLogRange( double& Min, double& Max ) const;
		inline void GetMarketData( double& LogDiscount, double& LogForward,
								   double& LogVariance ) const;

		inline void destroy();
		inline void SetnResizes( double Time, int n );
		inline void SetLogRange( double LogMin, double LogMax );
		inline void SetMarketData( double LogDiscount, double LogForward,
								   double LogVariance );
	private:
		double m_Time;
		int    m_nResizes;
		double m_LogMinGrid;
		double m_LogMaxGrid;
		double m_LogDiscount;
		double m_LogFwd;
		double m_Variance;
	};

protected:
	double m_LogSpot;								// the initial spot
	GsFuncPiecewiseLinear	m_LogDiscounts;			// the log of 1/discount curve
	GsFuncPiecewiseLinear	m_LogForwardGrowth;		// the log of the forward growth curve
	GsFuncPiecewiseLinear	m_AnchoringVariances;	// the grid anchoring variance curve
	int						m_GridSize;				// number of grid points
	QGridTimeSteps			m_TimeSteps;			// time discretization factor
	double					m_ResizingParam;		// sets grid resizing rate
	int						m_MinSteps;				// min number of steps per stepback
	int						m_MinInitialSteps;		// min number of steps for final stepback
	double					m_MinRange;				// used to calculate the grid size
	double					m_MaxRange;				// ...
	double					m_nStdDevs;				// ...
	PRICER_RETURN_TYPE		m_ReturnType;			// should Value() return a grid of prices?

	// following elts functions are cached, and so can be changed by const member functions
	GsVector				m_TempGrid;				// the grid for Crank-Nicholson
	Cache					m_cache;

	inline GsVector& TempGrid() const;			// returns a non-const reference to m_TempGrid
	inline Cache& GetCache() const;				// returns a non-const reference to m_cache

	void SetGrid( double t ) const;				// is const because it only affects m_TempGrid

	void Construct(
		double				Spot,					// spot
		const GsVector&		Discounts,				// discount factors
		const GsVector&		DiscountTimes,			// times of the discount factors
		const GsVector&		ForwardGrowth,			// forward growth factors
		const GsVector&		ForwardGrowthTimes,		// times of the forward growth factors
		const GsVector*		GridAnchoringVols,		// grid anchoring vols
		const GsVector*		GridAnchoringVolTimes	// times of the grid anchoring vols
	);

};

/****************************************************************
**	Class  : Pricer1DGrid
**	Routine: GridSizeParamsSet
**	Returns: void
**	Action : sets grid sizing parameters
****************************************************************/

void Pricer1DGrid::GridSizeParamsSet(
	double LogMinRange, // Min of time 0 range (logspace)
	double LogMaxRange, // Max of time 0 range (logspace)
	double nStdDevs 	// no. of std devs to go outside time 0 range
)
{
	m_MinRange = LogMinRange;
	m_MaxRange = LogMaxRange;
	m_nStdDevs = nStdDevs;
}


/****************************************************************
**	Class  : Pricer1DGrid
**	Routine: TempGrid
**	Returns: GsVector&
**	Action : returns non-const reference to m_TempGrid
****************************************************************/

GsVector& Pricer1DGrid::TempGrid() const
{
	return const_cast<GsVector&>( m_TempGrid );
}

/****************************************************************
**	Class  : Pricer1DGrid
**	Routine: nResizes
**	Returns: int
**	Action : returns number of grid resizes
****************************************************************/

int Pricer1DGrid::nResizes(
	double	Time		// time of grid
) const
{
	if( Time < PRICER1DGRID_MINGRIDTIME )
		Time = PRICER1DGRID_MINGRIDTIME;

	return( (int) ceil( log( Time ) / m_ResizingParam) );
}

/****************************************************************
**	Class  : Pricer1DGrid
**	Routine: CachednResizes
**	Returns: int
**	Action : returns number of grid resizes
****************************************************************/

int Pricer1DGrid::CachednResizes(
	double	Time		// time of grid
) const
{
	if( Time == m_cache.Time() )
		return m_cache.nResizes();

	int n = nResizes( Time );
	GetCache().SetnResizes( Time, n );

	return n;
}

/****************************************************************
**	Class  : Pricer1DGrid
**	Routine: GetCache
**	Returns: Pricer1DGrid::Cache&
**	Action : gets non-const reference to m_cache
****************************************************************/

Pricer1DGrid::Cache& Pricer1DGrid::GetCache() const
{
	return const_cast<Cache&>( m_cache );
}

/****************************************************************
**	Class  : Pricer1DGrid::Cache
**	Routine: destroy
**	Returns: void
**	Action : destroys cache
****************************************************************/

void Pricer1DGrid::Cache::destroy()
{
	m_Time = m_LogMinGrid = m_LogMaxGrid =
	m_LogDiscount = m_LogFwd = m_Variance = HUGE_VAL;
}

/****************************************************************
**	Class  : Pricer1DGrid::Cache
**	Routine: SetnResizes
**	Returns: void
**	Action : sets number of resizes & changes time of cache
****************************************************************/

void Pricer1DGrid::Cache::SetnResizes( double Time, int n )
{
	m_Time = Time;
	m_nResizes = n;
	m_LogMinGrid = m_LogMaxGrid = m_LogDiscount =
	m_LogFwd = m_Variance = HUGE_VAL;
}

/****************************************************************
**	Class  : Pricer1DGrid::Cache
**	Routine: GetLogRange
**	Returns: void
**	Action : gets range in log-space, if cached
****************************************************************/

void Pricer1DGrid::Cache::GetLogRange(
	double& Min,
	double& Max
) const
{
	Min = m_LogMinGrid;
	Max = m_LogMaxGrid;
}

/****************************************************************
**	Class  : Pricer1DGrid::Cache
**	Routine: SetLogRange
**	Returns: void
**	Action : sets range in log-space
****************************************************************/

void Pricer1DGrid::Cache::SetLogRange(
	double Min,
	double Max
)
{
	m_LogMinGrid = Min;
	m_LogMaxGrid = Max;
}

/****************************************************************
**	Class  : Pricer1DGrid::Cache
**	Routine: GetMarketData
**	Returns: void
**	Action : gets discount, fwd, variance
****************************************************************/

void Pricer1DGrid::Cache::GetMarketData(
	double& LogDiscount,
	double& LogForward,
	double& Variance
) const
{
	LogDiscount = m_LogDiscount;
	LogForward  = m_LogFwd;
	Variance    = m_Variance;
}

/****************************************************************
**	Class  : Pricer1DGrid::Cache
**	Routine: SetMarketData
**	Returns: void
**	Action : gets discount, fwd, variance
****************************************************************/

void Pricer1DGrid::Cache::SetMarketData(
	double LogDiscount,
	double LogForward,
	double Variance
)
{
	m_LogDiscount = LogDiscount;
	m_LogFwd      = LogForward;
	m_Variance    = Variance;
}

CC_END_NAMESPACE

#endif

