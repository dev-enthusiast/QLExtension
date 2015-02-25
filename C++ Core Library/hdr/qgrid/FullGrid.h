/****************************************************************
**
**	FullGrid2D.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_FULLGRID_H )
#define IN_FULLGRID_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gscore/GsVector.h>
#include	<gscore/GsTensor.h>
#include	<qenums.h>
#include	<qgrid/enums.h>
#include	<gsquant/GsFuncPiecewiseLinear.h>
#include	<vector>
#include	<ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_QGRID FullGrid
{
	public:
		// GsDt support
		typedef GsDtGeneric<FullGrid> GsDtType;
		GsString toString() const;
		inline static const char * typeName(){ return "FullGrid"; }
		FullGrid();

		// create a 2d grid
		FullGrid(
			int		nGridx,	// number of grid points in first dimension
			int		nGridy,	// number of grid points in second dimension
			double	Minx,	// minimum grid level in first dimension
			double	Miny,	// minimum grid level in second dimension
			double	Maxx,	// maximum grid level in first dimension
			double	Maxy	// maximum grid level in second dimension
		);

		void Create2D(
			int		nGridx,	// number of grid points in first dimension
			int		nGridy,	// number of grid points in second dimension
			double	Minx,	// minimum grid level in first dimension
			double	Miny,	// minimum grid level in second dimension
			double	Maxx,	// maximum grid level in first dimension
			double	Maxy	// maximum grid level in second dimension
		);

		FullGrid( const FullGrid& rhs )
		:
			m_Dimension( rhs.m_Dimension ),
			m_nGrid( rhs.m_nGrid ),
			m_step( rhs.m_step ),
			m_MinGrid( rhs.m_MinGrid ),
			m_MaxGrid( rhs.m_MaxGrid ),
			m_Values( rhs.m_Values )
		{}

		~FullGrid() {}
        
		const FullGrid& operator*=( double a );
		const FullGrid& operator+=( double a );
		const FullGrid& operator-=( double a ) { return operator+=( -a ); }
		const FullGrid& operator+=( const FullGrid& rhs );
		const FullGrid& operator-=( const FullGrid& rhs );
		const FullGrid& operator=( double a );

		double			operator()( const double* Vec ) const;
		double			operator()( const GsVector& Vec ) const;

		int nDimensions() const { return m_Dimension; }
		int	nGrid( int dim ) const { return m_nGrid[ dim ]; }
		double MinGrid( int dim ) const { return m_MinGrid[ dim ]; }
		double MaxGrid( int dim ) const { return m_MaxGrid[ dim ]; }

		size_t	  	size() const { return m_Values.size(); }
		CC_STL_VECTOR( double )& Values() { return m_Values; }
		double*					 data() { return &m_Values[0]; }
		void		SetValues( const CC_STL_VECTOR( double )& Values ) { m_Values = Values; }
		inline void	SetValues( const double* Values );
		const 		CC_STL_VECTOR( double )& Values() const { return m_Values; }

		void			Q_Tensor( Q_TENSOR& tensor );
		Q_TENSOR		Q_Tensor() { Q_TENSOR tensor; Q_Tensor( tensor ); return tensor; }

		void		Slice( int dim, double xVal );

		void	SmoothMultiplyLogSpace( const GsFuncPiecewiseLinear& f, int dim );
		void	Multiply( const GsFuncPiecewiseLinear& f, int dim );

		void	LinearRescale(
					int 	  dim,		// dimension along which to do the rescaling
					double    Min,		// new minimum of the grid range
					double 	  Max		// new maximum of the grid range
				);

	private:
		int		   				m_Dimension;	// dimension of grid
		CC_STL_VECTOR( int )	m_nGrid;		// size in each dimension
		CC_STL_VECTOR( int )	m_step;			// step in each dimension
		CC_STL_VECTOR( double )	m_MinGrid;		// min grid value in each dimension
		CC_STL_VECTOR( double ) m_MaxGrid;		// max grid value in each dimension

		CC_STL_VECTOR( double )	m_Values;		// values on the grid
};

void FullGrid::SetValues(
	const double* Values
)
{
	m_Values = CC_STL_VECTOR( double )( Values, Values + m_Values.size() );
}

CC_END_NAMESPACE

#endif

