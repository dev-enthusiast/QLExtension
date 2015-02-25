/****************************************************************
**
**	AverageRatePricer.h	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.16 $
**
****************************************************************/

#if !defined( IN_AVERAGERATEPRICER_H )
#define IN_AVERAGERATEPRICER_H

#include <qgrid/BSPricer.h>
#include <qgrid/AverageRateGrid.h>

CC_BEGIN_NAMESPACE( Gs )

class Pricer;

Pricer CreateAverageRatePricer(
	const BSPricer& model,	        // the BSPricer
	Q_OPT_TYPE      OptionType,	    // call/put/fwd
	double          Strike,   		// strike
	double          Expiration,	    // time to payout
	int				nAverages,		// number of averages
	const double*	AveragingTimes, // times to do averaging
	const double*   Weights,		// averaging weights
	double          InitialAverage, // weighted initial value of average
	int				nLayers,	    // number of layers
	const int*   	StepSizes,	    // step sizes of layers rel to first layer
	double			MaxSkewFactor = 0. // restricts amount of skew of spot-average range
);

Pricer CreateLASPricer(
	const BSPricer& model,	        // the BSPricer
	Q_OPT_TYPE      OptionType,	    // call/put/fwd
	double          StrikeOffset,   // offset for strike
	double          MinStrike,	    // strike floor
	double          MaxStrike,	    // strike cap
	double          Expiration,	    // time to payout
	int				nAverages,		// number of averages
	const double*   AveragingTimes, // times to do averaging
	const double*   Weights,		// averaging weights
	double          InitialAverage, // weighted initial value of average
	int				nLayers,	    // number of layers
	const int*      StepSizes,	    // step sizes of layers rel to first layer
	double			MaxSkewFactor = 0. // restricts amount of skew of spot-average range
);

Pricer CreateInvAvgStrikePricer(
	const BSPricer& model,	        // the BSPricer
	Q_OPT_TYPE      OptionType,	    // call/put/fwd
	double          StrikeOffset,   // offset for strike
	double          Expiration,	    // time to payout
	int				nAverages,		// number of averages
	const double*   AveragingTimes, // times to do averaging
	const double*   Weights,		// averaging weights
	double          InitialAverage, // weighted initial value of average
	int				nLayers,	    // number of layers
	const int*      StepSizes,	    // step sizes of layers rel to first layer
	double			MaxSkewFactor = 0. // restricts amount of skew of spot-average range
);

class EXPORT_CLASS_QGRID AverageRatePricer : public BasePricer< SimpleVec >
{
public:
	// GsDt support:
	typedef	GsDtGeneric<AverageRatePricer> GsDtType;
	GsString		toString() const;
	inline static const char * typeName(){ return "AverageRatePricer"; }

	inline AverageRatePricer( const BSPricer& pricer, int nAverages, const double* AveragingTimes,
                              const double* weights, double InitialAverage,
                              int nLayers = 21, const int *StepSizes = 0, bool DoRange = true );

	inline AverageRatePricer( const BSPricer& pricer, const GsVector& AveragingTimes,
                              const GsVector& weights, double InitialAverage,
                              int nLayers = 21, const int *StepSizes = 0, bool DoRange = true );

	// const member fns
	size_t GridSize() const { return m_grid.gridsize(); }
	size_t TotalSize() const { return m_grid.size(); }
	void SetLASPayout( double* Values, Q_OPT_TYPE OptionType, double time,
					   double offset, double minstrike, double maxstrike
                       ) const;
	void SetLASPayout( double* Values, Q_OPT_TYPE OptionType, double time,
					   double offset, double minstrike, double maxstrike,
					   double RangeOffset, double skew ) const;
	void SetAverageRatePayout( double* Values, Q_OPT_TYPE OptionType, double time,
                               double strike, double SpotMultiplier ) const;
	void SetAverageRatePayout( double* Values, Q_OPT_TYPE OptionType, double time,
                               double strike, double SpotMultiplier, double RangeOffset,
							   double skew ) const;
	void SetInvAvgStrikePayout( double* Values, Q_OPT_TYPE OptionType, double time,
                                double StrikeOffset, double SpotFactor, double Quantity,
								double RangeOffset, double skew ) const;

	void CalculateRange( double Expiration, double InitialAverage, double& skew,
						 double& RangeOffset, double& RangeSize, double& MinRange,
						 double MaxSkewFactor = 0. ) const;

	void SetAverageRange( double Min, double Max ) { m_grid.SetAverageRange( Min, Max ); }


	//BasePricer<> virtual memberfns
	void StepBack(
		SimpleVec& grid,
		double s,
		double t,
		double LowBarrier,
		double HighBarrier,
		double LowRebate,
		double HighRebate,
		double RebateTime
	) const;

	double Price( const SimpleVec& Grid, double LowerKO, double UpperKO, double LowerRebate,
                  double UpperRebate, double RebateTime ) const;

	void Multiply( SimpleVec& grid, const GsFuncPiecewiseLinear& f, double t ) const;

	GsDt* ToGsDt( const SimpleVec& Grid, double Time ) const;
	GsString ModelDescription() const;

private:
	BSPricer	m_pricer;			// BSPricer used for stepping back
	double		m_average;			// initial value of the average
	GsVector	m_AveragingTimes;	// times used to calculate average
	GsVector	m_Weights;			// averaging weights

	AverageRateGrid m_grid;			// stored grid, so only values needed for stepping back

	void construct( bool DoRange = true );

	// following over-rides constness
	inline void SetGridXRange( double MinX, double MaxX ) const;
	inline void SetGridXRange( double t ) const;

	void StepBackNoAverage(
		double *Values,
		double s,
		double t,
		double LowBarrier,
		double HighBarrier,
		double LowRebate,
		double HighRebate,
		double RebateTime
	) const;
};

void AverageRatePricer::SetGridXRange(
	double t
) const
{
	double MinX, MaxX;
	m_pricer.CachednResizes( t );
	m_pricer.CachedGridRange( MinX, MaxX );
	const_cast<AverageRateGrid&>( m_grid ).SetXRange( MinX, MaxX );
}

void AverageRatePricer::SetGridXRange(
	double MinX,
	double MaxX
) const
{
	const_cast<AverageRateGrid&>( m_grid ).SetXRange( MinX, MaxX );
}

AverageRatePricer::AverageRatePricer(
	const BSPricer& pricer,
	int nAverages,
	const double* AveragingTimes,
    const double* weights,
	double InitialAverage,
    int nLayers,
	const int *StepSizes,
	bool DoRange
)
:
	m_pricer( pricer ),
	m_average( InitialAverage ),
	m_AveragingTimes( nAverages, AveragingTimes ),
	m_Weights( nAverages, weights ),
	m_grid( 0, 1, 0, 1, pricer.GridSize(), nLayers, StepSizes ) // initialise range to anything
{
	construct( DoRange );
}

AverageRatePricer::AverageRatePricer(
	const BSPricer& pricer,
	const GsVector& AveragingTimes,
    const GsVector& weights,
	double InitialAverage,
    int nLayers,
    const int *StepSizes,
    bool DoRange
)
:
	m_pricer( pricer ),
	m_average( InitialAverage ),
	m_AveragingTimes( AveragingTimes ),
	m_Weights( weights ),
	m_grid( 0, 1, 0, 1, pricer.GridSize(), nLayers, StepSizes ) // initialise range to anything
{
	if( AveragingTimes.size() != weights.size() )
		GSX_THROW( GsXInvalidArgument, "AveragingTimes and weights are different sizes" );
	construct( DoRange );
}


CC_END_NAMESPACE

#endif

