/****************************************************************
**
**	qgrid/ForwardFD.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/qgrid/src/qgrid/ForwardFD.h,v 1.2 2000/02/03 20:53:11 lowthg Exp $
**
****************************************************************/

#if !defined( IN_QGRID_FORWARDFD_H )
#define IN_QGRID_FORWARDFD_H

#include	<qgrid/base.h>
#include	<gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )

void QGridForwardStep(
	const double		ForwardsStep,	// the log of the ratio of the forwards across the step
	const double		DiscountStep,	// ditto for discounts
	const GsVector&		GridPoints,		// the grid points.
	GsVector&			Values,			// the values on the grid ( changed by this function )
	const GsVector&		Variances,		// the variance across the time step for each grid point
	GsVector&			Temp,			// temporary storage vector (actually returns the reciprocals of the pivots of Gaussian Elimination )
	GsVector&			a,				// returns the lower diagonal elements of the transposed generator matrix.
	GsVector&			c				// returns the upper diagonal elements of the generator matrix.
);

void QGridForwardCNStep(
	const double*	L,					// lower diagonal coefficients of generator matrix (size = nGrid)
	const double*	U,					// upper diagonal coefficients of generator matrix (size = nGrid)
	double			Mean,				// negative sums of rows of generator matix
	const double*	ValuesIn,			// initial Arrow-Debreu prices (size = nGrid )
	double*			ValuesOut,			// final Arrow-Debreu prices (size = nGrid, can equal ValuesIn ) 
	double*			Temp,				// temporary storage vector (size = nGrid) 
	int				nGrid				// number of grid points 
);

CC_END_NAMESPACE
#endif

