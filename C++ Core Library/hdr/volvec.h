/* $Header: /home/cvs/src/supermkt/src/volvec.h,v 1.3 2006/10/27 03:37:16 boothm Exp $ */
/****************************************************************
**
**	VOLVEC.H
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**	
****************************************************************/

#if !defined( IN_VOLVEC_H )
#define IN_VOLVEC_H

#include <datatype.h>

#define VECTOR_ERR ( VOL_VECTOR *)	NULL

/*
**	Curve interpolation methods
*/

#define	VEC_EXTRAPOLATE			0x0000
#define	VEC_INTERPOLATE_ONLY	0x0001
#define	VEC_INTERPOLATE_FLAT	0x0002
#define	VEC_KNOTS_ONLY			0x0003
#define	VEC_INTERPOLATE_STEP	0x0004
#define	VEC_INTERPOLATE_PRESTEP	0x0005
#define	VEC_INTERPOLATE_CUBIC	0x0006

#define DATE_METHOD_NORMAL 	DT_INTERPOLATE_FLAT
#define VEC_METHOD_NORMAL 	VEC_INTERPOLATE_CUBIC
#define VEC_METHOD_MAX 		VEC_INTERPOLATE_PRESTEP


/*
**	Curve data type
*/

typedef struct VolPointStructure
{
	double	Strike,					// Strike
			Value;					// Corresponding Vol

} VOL_VECTOR_POINT;

typedef struct VolVecStructure
{
	int		PointCount,				// Number of Points
			PointAlloc;				// Allocated size of curve

	VOL_VECTOR_POINT
			*Points;				// Point buffer

} VOL_VECTOR;


DLLEXPORT VOL_VECTOR
		*VolVecAlloc(			int PointCount ),
		*VolVecAlign(			VOL_VECTOR *VectorA, VOL_VECTOR *VectorB, int Method ),
		*VolVecCopy(			VOL_VECTOR *Vector ),
		*VolVecAssign(			VOL_VECTOR *TargetVector, VOL_VECTOR *SourceVector ),
		*VolVecOperate(			VOL_VECTOR *Vector, int Operation, double Value ),
		*VolVecSort(			VOL_VECTOR *Vector );

DLLEXPORT int		
                VolVecAppend(			VOL_VECTOR *Vector, VOL_VECTOR_POINT *Point );

DLLEXPORT void	
                VolVecFree(				VOL_VECTOR *Vector );

DLLEXPORT double
		VolVecInterpolate(		VOL_VECTOR *Vector, double Strike, int Method );

DLLEXPORT int
		VolVecSearch(		VOL_VECTOR *Vector, double Strike );

DLLEXPORT int
		PointToStructure(		VOL_VECTOR_POINT *Point, DT_VALUE *Struct );

DLLEXPORT DT_DATA_TYPE
		DtVolVector;

#endif



