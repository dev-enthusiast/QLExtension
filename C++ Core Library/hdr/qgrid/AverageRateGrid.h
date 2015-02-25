/****************************************************************
**
**	AverageRateGrid.h	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.25 $
**
****************************************************************/

#if !defined( IN_AVERAGERATEGRID_H )
#define IN_AVERAGERATEGRID_H

#include <qgrid/base.h>
#include <qgrid/BSPricer.h>
#include <gsdt/GsDtDictionary.h>
#include <gscore/types.h>
#include <qenums.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CPP_QGRID AverageRateGrid
{
public:
	// GsDt support:
	typedef	GsDtGeneric<AverageRateGrid> GsDtType;
	GsString		toString() const;
	inline static const char * typeName(){ return "AverageRateGrid"; }

	AverageRateGrid( const AverageRateGrid& rhs );

	AverageRateGrid(
		double MinX,	// min of log(spot)
		double MaxX,	// max of log(spot)
		double MinA,	// min of average
		double MaxA,	// max of average
		int	   nGrid,	// size of first grid
		int    nLayers,	// number of X layers
		const int*   StepSize // step sizes of layers relative to first grid
	) { construct( MinX, MaxX, MinA, MaxA, nGrid, nLayers, StepSize ); validify(); }

	AverageRateGrid( const GsDtDictionary& rhs );

	~AverageRateGrid();

	// const memberfns
	GsDt* ToGsDt() const;
	GsDt* ToGsDt( const double* Values ) const;
	size_t gridsize() const { return m_GridSize[ 0 ]; }
	size_t size() const { return m_TotalSize; }
	double MinGrid() const { return m_Min; }
	double MaxGrid() const { return m_Min + ( m_GridSize[ 0 ] - 1 ) * m_dx; }
	double StepSize() const { return m_dx; }

	// grid operations
	void CreateLimitedAverageStrike( double* Values, Q_OPT_TYPE OptionType, double Offset,
                                     double MinStrike, double MaxStrike, double RangeOffset,
									 double skew ) const;
	inline void CreateLimitedAverageStrike( double* Values, Q_OPT_TYPE OptionType, double Offset,
                                     double MinStrike, double MaxStrike ) const;
                                            
	void CreateAverageRateOption( double* Values, Q_OPT_TYPE OptionType, double Strike,
                                  double SpotMultiplier, double RangeOffset, double skew ) const;
	inline void CreateAverageRateOption( double* Values, Q_OPT_TYPE OptionType, double Strike,
                                         double SpotMultiplier ) const;
	void CreatePayout( double* Values, const double* c, double k1, const double* k2 ) const;
	void CreatePayout( double* Values, const double* c, double k1, const double* k2,
                       double RangeOffset, double skew ) const;

	void CreateInverseAverage(
		double*		  Values, 	// grid values
		double*		  coeffs,	// coefficients
		double*		  consts,	// const terms
		double*		  offsets,	// offsets
		double*		  MinRange, // Min of range for each spot
		double*		  MaxRange  // Max of range for each spot
	) const;

	void CreateInvAvgStrikeOption(
		double*    Values,
		Q_OPT_TYPE OptionType,
		double     StrikeOffset,
		double     AverageOffset,
		double     SpotFactor,
		double     Quantity,
		double	   RangeOffset,
		double	   skew
	) const;

	void StepBack( double* Values, const BSPricer& pricer, double s, double t, double LowBarrier,
				   double HighBarrier, double LowRebate, double HighRebate,
				   double RebateTime ) const;
	void AddAveragingPoint( double* Values, double weight ) const;
	double Evaluate( const double* Values, double logspot, double average ) const;
	void Assign( double* Values, double a ) const;
	void GetLayer( const double* Values, double average, double* layer ) const;

	void Multiply( double* Values, double* Multipliers ) const; // note - Multipliers is affected
	void LinearResize( double* Values, double NewMin, double NewMax, double* TempValues ) const;

	// non-const memberfns
	const AverageRateGrid& operator=( const AverageRateGrid& rhs );
	inline void SetAverageRange( double MinA, double MaxA );
	inline void SetXRange( double MinX, double MaxX );

private:
	int	   m_nLayers;	 	// number of layers
	double m_Min;		 	// value at index 0
	double m_dx;		 	// grid step size
	int	   *m_MinPoints; 	// index of min point of each layer
	int	   *m_StepSize;  	// step size of each layer (as multiple of m_dx)
	int    *m_GridSize;	 	// size of each layer
	int	   m_TotalSize;		// total number of doubles allocated
	double m_MinLayerRange;
	double m_MaxLayerRange;

	void allocate();
	void copy( const AverageRateGrid& rhs );
	void construct(
		double MinX,	 // min of log(spot)
		double MaxX,	 // max of log(spot)
		double MinA,	 // min of average
		double MaxA,	 // max of average
		int	   nGrid,	 // size of first grid
		int    nLayers,	 // number of X layers
		const int*   StepSize  // step sizes of layers relative to first grid
	);
	void validify() const; // throws exception if object is not valid
};

void AverageRateGrid::SetAverageRange(
	double MinA,
	double MaxA
)
{
	m_MinLayerRange = MinA;
	m_MaxLayerRange = MaxA;
}

void AverageRateGrid::SetXRange(
	double MinX,
	double MaxX
)
{
	m_Min = MinX;
	m_dx = ( MaxX - MinX ) / ( m_GridSize[ 0 ] - 1 );
}

void AverageRateGrid::CreateLimitedAverageStrike(
	double* Values,
	Q_OPT_TYPE OptionType,
	double Offset,
    double MinStrike,
	double MaxStrike
) const
{
	CreateLimitedAverageStrike( Values, OptionType, Offset,
								MinStrike, MaxStrike, m_MinLayerRange,
                                0. );
}

void AverageRateGrid::CreateAverageRateOption(
	double* Values,
	Q_OPT_TYPE OptionType,
    double Strike,
    double SpotMultiplier
) const
{
	CreateAverageRateOption( Values, OptionType, Strike, SpotMultiplier,
							 m_MinLayerRange, 0. );
}

CC_END_NAMESPACE

#endif

