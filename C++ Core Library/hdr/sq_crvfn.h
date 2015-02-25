/****************************************************************
**
**	sq_crvfn.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.15 $
**
****************************************************************/

#if !defined( IN_SQ_CRVFN_H )
#define IN_SQ_CRVFN_H

#ifndef IN_QTYPES_H
#include <qtypes.h>
#endif
#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif

EXPORT_C_NUM DT_CURVE 
		*SQ_VectorsToCurve( Q_VECTOR *Dates, Q_VECTOR *Values ),
		*SQ_FwdFromDiscount(	DT_CURVE *ValueCurrDiscCurve, DT_CURVE *UnderCurrDiscCurve),
		*SQ_IntRateFromDiscount( DT_CURVE	*DiscCrv, DATE Date ),
		*SQ_QuantosDiscCrv( DT_CURVE *Denominated, DT_CURVE *DiscCrossOver, DT_CURVE *DiscCrossUnder );

EXPORT_C_NUM int
		Q_Initialize_Curve_Matrix( Q_CURVE_MATRIX *mat, int *rows, int *cols ),
		SQ_ArrayToVector( DT_VALUE *arry, Q_VECTOR *vect ),
		SQ_CurveToVectors( DT_CURVE *Curve, Q_VECTOR *Dates, Q_VECTOR	*Values ),
		SQ_ValueFromMatrix( DT_VALUE *Value, Q_MATRIX *Matrix ),
		SQ_ValueFromVector( DT_VALUE *Value, Q_VECTOR *Vector ),
		SQ_ValueToVector( DT_VALUE *Value, Q_VECTOR *Vector ),
		SQ_ValueToCurveMatrix( DT_VALUE *Value, Q_CURVE_MATRIX *Matrix ),
		SQ_ValueToMatrix( DT_VALUE *Value, Q_MATRIX *Matrix ),
		SQ_VectorDatesToTerms( Q_VECTOR *Dates, DATE BaseDate, int DayCountBasis ),
		SQ_VectorTermsToDates( Q_VECTOR *Dates, DATE BaseDate, int DayCountBasis ),
		Q_BoundVectors( double minSpot, double maxSpot, int gridsize, Q_VECTOR *Spots, Q_VECTOR *Prices ),
		Q_BoundMatrix( double minSpot, double maxSpot, int gridsize, int colN, Q_VECTOR *Spots, Q_VECTOR *Prices ),
		Q_BoundMatrixFast( double minValue, double	maxValue, Q_VECTOR	*Spot, Q_VECTOR	*Price, int PriceNum,  
							int	 *SpotInd, int *PriceInd ), 
		Q_DefineGrid( double maxSpot, double minSpot, double rangevol, double gridsizer, double outVal,
						DATE prcDate, DATE expDate, Q_UP_DOWN upOrDown, int *gridsizePtr, 
						Q_VECTOR *Spots, int  *outNumberPtr ),
		N_DefineRanges( double 	 maxRange, double 	 minRange, double prcntgRange, double  rangevol, 
                        DATE 	 prcDate, DATE	 expDate,   int  gridsize, Q_MATRIX *SpotRanges ),
		N_DefineRangesKO( double 	 maxRange, double 	 minRange, double prcntgRange, double  rangevol, 
                        DATE 	 prcDate, DATE	 expDate, DATE KODate, int  gridsize, Q_MATRIX *SpotRanges ),
		N_DefineRangesFromRangeSpot( double maxRange, double minRange, double RangeSpot, double prcntgRange, 
									double rangevol, DATE   prcDate, DATE   expDate, DATE KODate, int gridsize, Q_MATRIX *SpotRanges ),    
		N_SetMinMaxGrid( double maxSpot,  double minSpot,  double rangevol, DATE   prcDate,  DATE   expDate,  
						 int    *gridsize, double *minGrid, double *maxGrid, double factor ),	  
		Q_BoundVectorsFromArray( double	 minSpot, double maxSpot, double *minGrid, double *maxGrid, 
						    	 int *nGrid, double *V, Q_VECTOR *Spots,   Q_VECTOR *Prices    );

#endif

