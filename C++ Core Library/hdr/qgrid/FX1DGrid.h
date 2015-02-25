/****************************************************************
**
**	FX1DGrid.h	- definition of 1 dimensional grid class
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_FX1DGRID_H )
#define IN_FX1DGRID_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gscore/GsVector.h>
#include	<gsquant/GsFuncPiecewiseLinear.h>
#include	<qenums.h>
#include	<qgrid/enums.h>

CC_BEGIN_NAMESPACE( Gs )

class FX1DGrid
{
	public:
		typedef double*			iterator;
		typedef const double*	const_iterator;

		FX1DGrid();
		FX1DGrid( size_t size ) { Create( size ); }
		FX1DGrid( const double* xValues, int size ) { Create( size, xValues ); }
		FX1DGrid( const GsVector& xValues ) { Create( xValues.size(), xValues.begin()); }
		FX1DGrid( const double* xValues, const double* yValues, size_t size );
		FX1DGrid( const GsVector& xValues, const GsVector& yValues );
		FX1DGrid( const FX1DGrid& rhs );

		virtual ~FX1DGrid();

		const FX1DGrid& operator=( const FX1DGrid& rhs );

		const double*	xValues() const { return m_xValues; }
		double*			xValues() { return m_xValues; }
		const double*	yValues() const { return m_yValues; }
		double*			yValues() { return m_yValues; }

		iterator		begin()	{ return yValues(); }
		const_iterator	begin() const { return yValues(); }
		size_t			size() const { return m_Size; }

		double&			operator()( int i ) { return m_yValues[ i ]; }
		double&			operator[]( int i ) { return m_yValues[ i ]; }
		const double&	operator()( int i ) const { return m_yValues[ i ]; }
		const double&	operator[]( int i ) const { return m_yValues[ i ]; }

	private:
		double*		m_xValues;		// the grid points (use GsVector for the reference counting)
		double*		m_yValues;		// the values of the grid
		size_t		m_nAlloc;		// Allocated space
		size_t		m_Size;			// the size

		void	Create( size_t size );
		void	Create( size_t size, const double* xValues );
		void	m_TestValidity() const;	// generates an exception if grid is not defined properly
};

CC_END_NAMESPACE

#endif


