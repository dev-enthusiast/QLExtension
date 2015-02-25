/****************************************************************
**
**	FourierGrid.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.18 $
**
****************************************************************/

#if !defined( IN_FOURIERGRID_H )
#define IN_FOURIERGRID_H

#include <gsquant/GsFuncFourierSeries.h>
#include <gsquant/GsFuncPiecewiseLinear.h>

CC_BEGIN_NAMESPACE( Gs )

class FourierGrid
{
public:
	inline FourierGrid();
	inline FourierGrid( double MinRange, double MaxRange, int nFourier, int nGrid );
	inline FourierGrid( const FourierGrid& rhs );

	const FourierGrid& operator=( const FourierGrid& rhs );

	inline void resize( int nFourier, int nGrid );
	inline void SetRange( double MinRange, double MaxRange );
	int			GridSize() const { return m_grid.size(); }
	int			nResizes() const { return m_nResizes; }
	void		nResizesSet( int n ) { m_nResizes = n; }
	void 		Resize( int nResizes, double Min );
	double		MinRange() const { return m_series.MinRange(); }
	double		MaxRange() const { return m_series.MaxRange(); }

	inline double operator()( double x, GsVector&temp ) const;
	inline double operator()( double x ) const;
	const FourierGrid& operator*=( double a );
	const FourierGrid& operator+=( double a );
	const FourierGrid& operator+=( const FourierGrid& rhs );
	const FourierGrid& operator-=( const FourierGrid& rhs );
	inline void SetValuesExp( const GsFuncPiecewiseLinear& f, double a );
	void MultiplyExp( const GsFuncPiecewiseLinear& f, double a, double MinRange, double MaxRange,
                      GsVector& temp, GsFuncFourierSeries& g ); // temp & g are temp storage
	GsDt* toGsDt() const;

	void ConvolveNormal( double variance, double mean, GsVector& temp );

	const GsVector& grid() const
	{ if( !m_IsGridValid ) ToGrid(); return m_grid; }
	const GsFuncFourierSeries&	Series() const
	{ if( !m_IsFourierValid ) ToFourier(); return m_series; }
	const GsFuncFourierSeries&	Series( GsVector& temp ) const
	{ if( !m_IsFourierValid ) ToFourier( temp ); return m_series; }

private:
	GsFuncFourierSeries		m_series;
	GsVector				m_grid;
	bool					m_IsFourierValid;
	bool					m_IsGridValid;
	int						m_nResizes;

	void ToGrid() const;					// sets grid values
	void ToFourier() const;					// sets Fourier values
	void ToFourier( GsVector& temp ) const;	// sets Fourier values
};

FourierGrid::FourierGrid()
:
	m_series( 0., 1., 0, 0 ),
	m_IsFourierValid( false ),
	m_IsGridValid( false ),
	m_nResizes( 1 )
{
}

FourierGrid::FourierGrid(
	double MinRange,
	double MaxRange,
	int    nFourier,
	int    nGrid
)
:
	m_series( MinRange, MaxRange, nFourier, nFourier ),
	m_grid( nGrid, 0. ),
	m_IsFourierValid( false ),
	m_IsGridValid( false ),
	m_nResizes( 1 )
{
}

FourierGrid::FourierGrid(
	const FourierGrid& rhs
)
:
	m_series( rhs.m_series ),
	m_grid( rhs.m_grid ),
	m_IsFourierValid( rhs.m_IsFourierValid ),
	m_IsGridValid( rhs.m_IsGridValid ),
	m_nResizes( rhs.m_nResizes )
{

}

void FourierGrid::resize(
	int nFourier,
	int nGrid
)
{
	m_series.resize( nFourier, nFourier );
	m_grid.resize( nGrid, 0. );
	m_IsGridValid = false;
}

void FourierGrid::SetRange(
	double MinRange,
	double MaxRange
)
{
	m_series.SetMinRange( MinRange );
	m_series.SetMaxRange( MaxRange );
}

double FourierGrid::operator()(
	double 					 x,
	GsVector& temp
) const
{
	if( !m_IsFourierValid )
		ToFourier( temp );
	return m_series( x );
}

double FourierGrid::operator()(
	double x
) const
{
	if( !m_IsFourierValid )
		ToFourier();
	return m_series( x );
}

void FourierGrid::SetValuesExp( const GsFuncPiecewiseLinear& f, double a )
{
	m_IsFourierValid = true;
	m_IsGridValid = false;
	m_series *= 0.;
	m_series.AddExp( f, a );
}

CC_END_NAMESPACE

#endif

