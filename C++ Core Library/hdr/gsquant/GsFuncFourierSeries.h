/****************************************************************
**
**	GsFuncFourierSeries.h	- Fourier Series form of periodic fn
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.19 $
**
****************************************************************/

#if !defined( IN_GSFUNCFOURIERSERIES_H )
#define IN_GSFUNCFOURIERSERIES_H

#include <gsquant/base.h>
#include <gscore/GsFunc.h>
#include <gscore/types.h>
#include <gscore/GsVector.h>
#include <gsquant/GsFuncSpecial.h>
#include <gsquant/GsFuncPiecewiseLinear.h>
#include <gsdt/GsDt.h>
#include <ccstl.h>
#include <vector>
#include <n_consts.h>

CC_BEGIN_NAMESPACE( Gs )

enum FOURIER_FUNC_TYPE
{
	DELTA,
	NORMAL,
	PERIODIC_GRID
};

class EXPORT_CLASS_GSQUANT GsFuncFourierSeries : public GsFunc<double, double>
{
public:
	// GsDt Support
	typedef GsDtGeneric<GsFuncFourierSeries> GsDtType;
	GsString toString() const;
	inline static const char * typeName() { return "GsFuncFourierSeries"; }

	inline GsFuncFourierSeries( double MinRange, double MaxRange, int nterms );
	inline GsFuncFourierSeries( double MinRange, double MaxRange, int nsines, int ncosines );

	inline GsFuncFourierSeries( double MinRange, double MaxRange, double Constant,
								const GsVector& sines, const GsVector& cosines );

	GsFuncFourierSeries( FOURIER_FUNC_TYPE functype, double x, double MinRange,
						 double MaxRange, int nsin, int ncos ); // functype = DELTA
	GsFuncFourierSeries( FOURIER_FUNC_TYPE functype, double	mean, double variance, double MinRange,
                         double MaxRange, int nsin, int ncos ); // functype = NORMAL
	// Values gets FFT'd
	inline GsFuncFourierSeries( FOURIER_FUNC_TYPE functype, double MinRange,
                                double MaxRange, GsVector& Values ); // functype = PERIODIC_GRID

	inline GsFuncFourierSeries( const GsFuncFourierSeries& rhs );

	virtual ~GsFuncFourierSeries() {}

	void copy( const GsFuncFourierSeries& rhs );
	void copysizes( const GsFuncFourierSeries& rhs );

	inline const GsFuncFourierSeries& operator=( const GsFuncFourierSeries& rhs );

	int nsines() const { return m_sin.size(); }
	int ncosines() const { return m_cos.size(); }
	double CosCoeff( int i ) const { return m_cos[ i ]; }
	double SinCoeff( int i ) const { return m_sin[ i ]; }
	double ConstantTerm() const { return m_constant; }
	void SetCosCoeff( int i, double a ) { m_cos[ i ] = a; }
	void SetSinCoeff( int i, double a ) { m_sin[ i ] = a; }
	void SetConstantTerm( double a ) { m_constant = a; }
	double MinRange() const { return m_MinRange; }
	double MaxRange() const { return m_MaxRange; }
	double period() const { return m_MaxRange - m_MinRange; }
	double AngularVel() const { return 2 * PI / ( m_MaxRange - m_MinRange ); }
	void SetMinRange( double a ) { m_MinRange = a; }
	void SetMaxRange( double a ) { m_MaxRange = a; }
	void Negate();
	void resize( int nsines, int ncosines ) { m_sin.resize( nsines, 0. ); m_cos.resize( ncosines, 0. ); }

	GsDt* toGsDt() const;

	void ConvolveNormal( double mean, double variance );
	void Rotate( double angle );
	void Translate( double distance ) { Rotate( distance * AngularVel()); }

	double operator() ( double x ) const;
	void operator+=( double a ) { m_constant += a; }
	void operator-=( double a ) { m_constant -= a; }
	void operator*=( double a );
	void operator/=( double a ) { operator*=( 1. / a ); }

	// following two functions retain terms of LHS argument
	const GsFuncFourierSeries& operator+=( const GsFuncFourierSeries& rhs );
	const GsFuncFourierSeries& operator-=( const GsFuncFourierSeries& rhs );
	const GsFuncFourierSeries& operator*=( const GsFuncFourierSeries& rhs );

	// n must be power of 2 in following
	void Multiply( const GsFuncFourierSeries& rhs,
				   double* workspace1, double* workspace2,
				   int n );
	inline void Multiply( const GsFuncFourierSeries& rhs, int n );

	// add a + b exp( y ) for y < x
	void AddLeftExpSegment( double a, double b, double x );
	// Adds f(a*exp(x)) for Min < x < Max
	void AddExp( const GsFuncPiecewiseLinear& f, double a, double Min, double Max );
	void AddExp( const GsFuncPiecewiseLinear& f, double a ) { AddExp( f, a, m_MinRange, m_MaxRange ); }

	// nGrid must be power of 2 in following, FFT is used
	void ToGrid( int nGrid, double* Values ) const;
	void FromGrid( int nGrid, double* Values ); // also Fourier transforms Values

	inline GsVector ToGrid( int nGrid ) const;
	inline void FromGrid( const GsVector& Values ); // Values.size() must be power of 2

	void Rescale( int n );

	void HalveRange();

private:
	double m_MinRange; // min of fundamental interval
	double m_MaxRange; // max of fundamental interval 
	double m_constant; // constant term
	GsVector m_sin;	// sine expansion
	GsVector m_cos;	// cosine expansion
};

GsFuncFourierSeries::GsFuncFourierSeries(
	double MinRange,
	double MaxRange,
	int nterms
)
:
	m_MinRange( MinRange ),
	m_MaxRange( MaxRange ),
	m_constant( 0. ),
	m_sin( nterms - 1, 0. ),
	m_cos( nterms, 0. )
{
}

GsFuncFourierSeries::GsFuncFourierSeries(
	double MinRange,
	double MaxRange,
	int nsines,
	int ncosines
)
:
	m_MinRange( MinRange ),
	m_MaxRange( MaxRange ),
	m_constant( 0. ),
	m_sin( nsines, 0. ),
	m_cos( ncosines, 0. )
{}

GsFuncFourierSeries::GsFuncFourierSeries(
	double 						   MinRange,
	double 						   MaxRange,
	double						   Constant,
	const GsVector& sines,
	const GsVector& cosines
)
:
	m_MinRange( MinRange ),
	m_MaxRange( MaxRange ),
	m_constant( Constant ),
	m_sin( sines ),
	m_cos( cosines )
{
}

GsFuncFourierSeries::GsFuncFourierSeries(
	FOURIER_FUNC_TYPE functype,
	double 			  MinRange,
	double 			  MaxRange,
	GsVector&		  Values
)
:
	m_MinRange( MinRange ),
	m_MaxRange( MaxRange ),
	m_sin( Values.size() - 1 ),
	m_cos( Values.size() )
{
	if( functype != PERIODIC_GRID )
		GSX_THROW( GsXInvalidArgument, "functype is not PERIODIC_GRID" );
	FromGrid( Values );
}

GsFuncFourierSeries::GsFuncFourierSeries(
	const GsFuncFourierSeries& rhs
)
:
	m_MinRange( rhs.m_MinRange ),
	m_MaxRange( rhs.m_MaxRange ),
	m_constant( rhs.m_constant ),
	m_sin( rhs.m_sin ),
	m_cos( rhs.m_cos )
{
}

const GsFuncFourierSeries& GsFuncFourierSeries::operator=(
	const GsFuncFourierSeries& rhs
)
{
	m_MinRange = rhs.m_MinRange;
	m_MaxRange = rhs.m_MaxRange;
	m_constant = rhs.m_constant;
	m_sin = rhs.m_sin;
	m_cos = rhs.m_cos;

	return *this;
}

GsVector GsFuncFourierSeries::ToGrid(
	int nGrid
) const
{
	GsVector RetVals( nGrid );
	ToGrid( nGrid, RetVals.begin() );
	return RetVals;
}

void GsFuncFourierSeries::FromGrid(
	const GsVector& Values
)
{
	GsVector TempVals = Values;
	FromGrid( TempVals.size(), TempVals.begin() );
}

void GsFuncFourierSeries::Multiply(
	const GsFuncFourierSeries& rhs,
	int n
)
{
	GsVector workspace1( n, 0. );
	GsVector workspace2( n, 0. );
	Multiply( rhs, &workspace1[0], &workspace2[0], n );
}

// ... Addins ...

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeriesCreate(
	double 			MinRange,
	double 			MaxRange,
	double			Constant,
	const GsVector& sines,
	const GsVector&	cosines
);

EXPORT_CPP_GSQUANT GsFuncScFunc GsFuncFromFourierSeries(
	const GsFuncFourierSeries&	f
);

EXPORT_CPP_GSQUANT GsDt* GsFuncFourierSeries_toGsDt(
	const GsFuncFourierSeries&	f
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_ConvolveNormal(
	const GsFuncFourierSeries&	f,
	double						mean,
	double						variance
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_Translate(
	const GsFuncFourierSeries&	f,
	double						distance
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_DeltaFn(
	double x,			// point at which delta fn is centred
	double MinRange,	// min of fundamental range
	double MaxRange,	// max of fundamental range
	int	   n			// number of terms in Fourier series
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_Normal(
	double mean,		// mean of distn
	double variance,	// variance of distn
	double MinRange,	// min of fundamental range
	double MaxRange,	// max of fundamental range
	int	   n			// number of terms in Fourier series
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_FromGrid(
	double						MinRange,	// minimim of the range
	double						MaxRange,	// maximim of the range
	const GsVector&				Values		// the grid values
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_AddLinearExpSeg(
	const GsFuncFourierSeries&	f,          // the GsFuncFourierSeries
	double						a,			// adds a + b * exp( x )
	double						b,			// ...
	double						min,		// for min < x < max
	double						max			// ...
);

EXPORT_CPP_GSQUANT GsVector GsFuncFourierSeries_ToGrid(
	const GsFuncFourierSeries&	f,          // the GsFuncFourierSeries
	int							nGrid		// the grid size
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_Add(
	const GsFuncFourierSeries&	f,          // first GsFuncFourierSeries
	const GsFuncFourierSeries&	g          	// second GsFuncFourierSeries
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_Multiply(
	const GsFuncFourierSeries&	f,          // first GsFuncFourierSeries
	const GsFuncFourierSeries&	g          	// second GsFuncFourierSeries
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_Negate(
	const GsFuncFourierSeries&	f          // the GsFuncFourierSeries
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_Rescale(
	const GsFuncFourierSeries&	f,        	// the GsFuncFourierSeries
	int							n			// factor by which to rescale
);

EXPORT_CPP_GSQUANT GsFuncFourierSeries GsFuncFourierSeries_HalveRange(
	const GsFuncFourierSeries&	f        	// the GsFuncFourierSeries
);

CC_END_NAMESPACE

#endif

