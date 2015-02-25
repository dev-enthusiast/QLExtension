/****************************************************************
**
**	qgrid/gridops.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/qgrid/src/qgrid/GridOps.h,v 1.6 2001/03/05 20:57:58 lowthg Exp $
**
****************************************************************/

#if !defined( IN_QGRID_GRIDOPS_H )
#define IN_QGRID_GRIDOPS_H

#include	<qgrid/base.h>
#include	<gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )
                 
EXPORT_CPP_QGRID void QGridLinearRescaleVector(
	double			OldMinLevel,	// minimum of the original range
	double			OldMaxLevel,	// maximum of the original range 
	double			NewMinLevel,    // minimum of the new range 
	double			NewMaxLevel,    // maximum of the new range
	GsVector&		Values,			// the vector of values to be rescaled
	GsVector&		TmpValues		// temp storage vector
);

EXPORT_CPP_QGRID bool QGridLinearRescaleVector(
	double		OldMinLevel,
	double		OldMaxLevel,
	int			Size,
	double		NewMinLevel,
	double		NewMaxLevel,
	double*		Values
);

EXPORT_CPP_QGRID bool QGridLinearRescaleVector(
	double		  OldMin,
	double		  OldMax,
	int			  Size,
	double		  NewMin,
	double		  NewMax,
	const double* ValuesIn,
	double*		  ValuesOut
);

EXPORT_CPP_QGRID GsVector QGridLinearRescaleGsVector(
	double			OldMinLevel,	// minimum of the original range
	double			OldMaxLevel,	// maximum of the original range 
	double			NewMinLevel,    // minimum of the new range 
	double			NewMaxLevel,    // maximum of the new range
	const GsVector&	Values			// the vector of values to be rescaled
);

EXPORT_CPP_QGRID GsVector QGridVectorMaxSmooth(
	const GsVector&	xValues, // the x values
	const GsVector&	yValues, // the y values
	double			a		 // the value to max with
);

EXPORT_CPP_QGRID void VectorMaxSmooth(
	const double*	xValues,	// the vector of x values
	double* 		yValues,	// the vector of y values (input & output)
	int				nValues,	// the number of values
	double			a			// the value to max with
);

EXPORT_CPP_QGRID GsVector QGridVectorMaxProj(
	const GsVector&	xValues, // the x values
	const GsVector&	yValues, // the y values
	double			a		 // the value to max with
);

EXPORT_CPP_QGRID void VectorMaxProj(
	const double*	xValues,	// the vector of x values
	double* 		yValues,	// the vector of y values (input & output)
	int				nValues,	// the number of values
	double			a			// the value to max with
);

EXPORT_CPP_QGRID void SmoothedVectorProj(
	const double*	xValues,	// the vector of x values
	double* 		yValues,	// the vector of y values (input & output)
	int				nValues,	// the number of values
	double*			Vec1,		// work vector 1 (of size nValues)
	double*			Vec2,		// work vector 2 (of size nValues)
	double*			Vec3,		// work vector 3 (of size nValues)
	double*			Vec4		// work vector 4 (of size nValues)
);


CC_END_NAMESPACE
#endif


