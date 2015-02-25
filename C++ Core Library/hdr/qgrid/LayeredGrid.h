/****************************************************************
**
**	LayeredGrid.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_LAYEREDGRID_H )
#define IN_LAYEREDGRID_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )

class LayeredGridRep
{
	public:
		double*				Points;
		LayeredGridRep**	SubGrids;
		int					nSubGrids;
		int					Size;
		int					Offset;
};

class EXPORT_CLASS_QGRID LayeredGrid
{
	public:
		typedef GsDtGeneric<LayeredGrid> GsDtType;
		GsString toString() const;
		inline static const char* typeName() { return "LayeredGrid"; }
	
		LayeredGrid() { create( 0, 0 ); }
		LayeredGrid( int nGrids, int nPoints ) { create( nGrids, nPoints ); }
		LayeredGrid( const LayeredGrid& rhs ) { copy( rhs ); }
		const LayeredGrid& operator=( const LayeredGrid& rhs );
		~LayeredGrid();

		const LayeredGridRep& GridRep() const { return *m_grids; }
		LayeredGridRep& GridRep() { return *m_grids; }
		const double* Points() const { return m_points; }
		double* Points() { return m_points; }
	
	private:
		double*					m_points;
		LayeredGridRep*	 		m_grids;
		int						m_npoints;
		int						m_ngrids;

		void create( int nGrids, int nPoints );
		void destroy();
		void copy( const LayeredGrid& rhs );
};

CC_END_NAMESPACE

#endif

