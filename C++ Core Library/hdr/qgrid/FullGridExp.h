/****************************************************************
**
**	FullGridExp.h	- exported member functions for 2d grid
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_FULLGRIDEXP_H )
#define IN_FULLGRIDEXP_H

#include <qgrid/base.h>
#include <gscore/types.h>
#include <qgrid/enums.h>
#include <qgrid/FullGrid.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID FullGrid QGridCreate2DGrid(
	int		nGridx,		// number of grid points in first dimension
	int		nGridy,		// number of grid points in second dimension
	double	Minx,		// minimum grid level in first dimension
	double	Miny,		// minimum grid level in second dimension
	double	Maxx,		// maximum grid level in first dimension
	double	Maxy		// maximum grid level in second dimension
);

EXPORT_CPP_QGRID FullGrid FullGrid_SetValuesDbl(
	const FullGrid& grid,		// the grid
	double			a			// value set to the grid
);

EXPORT_CPP_QGRID FullGrid FullGrid_MultiplyDbl(
	const FullGrid& grid,		// the grid
	double			a			// value set to the grid
);

EXPORT_CPP_QGRID FullGrid FullGrid_AddDbl(
	const FullGrid& grid,		// the grid
	double			a			// value to add
);

EXPORT_CPP_QGRID FullGrid FullGrid_SmoothMultiplyLogSpace(
	const FullGrid&			 grid,  // the grid
	const GsFuncPiecewiseLinear& f,		// function to multiply by
	int						 dim	// dimension on which f depends
);

EXPORT_CPP_QGRID FullGrid FullGrid_MultiplyPwl(
	const FullGrid&			 grid,  // the grid
	const GsFuncPiecewiseLinear& f,		// function to multiply by
	int						 dim	// dimension on which f depends
);

EXPORT_CPP_QGRID GsVector FullGrid_GetValues(
	const FullGrid& grid	// the grid
);

EXPORT_CPP_QGRID FullGrid FullGrid_SetValues(
	const FullGrid& grid,	// the grid
	const GsVector& Values 	// the values
);

EXPORT_CPP_QGRID FullGrid FullGrid_Slice(
	const FullGrid& grid,	// the grid
	int				dim,	// dimension which to slice
	double			xVal	// position at which to slice
);

EXPORT_CPP_QGRID double FullGrid_Interp(
	const FullGrid& grid,	// the grid
	const GsVector&	p		// point at which to get the value
);

EXPORT_CPP_QGRID FullGrid FullGrid_LinearRescale(
	const FullGrid& grid,	// the grid
	int				dim,	// the dimension to rescale
	double			Min,	// new minimum value of the range
	double			Max		// new maximum value of the range
);

CC_END_NAMESPACE

#endif

