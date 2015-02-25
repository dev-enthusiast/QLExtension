/* #define GSCVSID "$Header: /home/cvs/src/qcommod/src/qc_q.h,v 1.17 2000/07/10 12:39:46 goodfj Exp $" */
/****************************************************************
**
**	qc_q.h	-  qcommod ('qc') generic quant stuff
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.17 $
**
****************************************************************/

#if !defined( IN_QC_Q_H )
#define IN_QC_Q_H


#ifndef _IN_DATATYPE_H
#include <datatype.h>
#endif
#ifndef _IN_QTYPES_H
#include <qtypes.h>
#endif

//#include <qcommod.h>

// 	Constants

#define SECS_PER_365D   	3.1536000e7  	// seconds per year
//#define SECS_PER_360D   	3.1104000e7  	// seconds per 360d

#define QC_TIME_FROM_DATE( Date, TimeZone ) \
		TimeFromDate( Date, TimeZone, 0, 0, 0 )
#define QC_CURVE_TO_TCURVE( CurveP, TimeZone ) \
		CurveToTCurve( CurveP, TimeZone, 0, 0, 0 )
#define QC_DATE_TO_TIME_DTARRAY( DArrayV, TimeZone, TArrayV ) \
		QcDateToTimeDtArray( DArrayV, TimeZone, 0, 0, 0, TArrayV )
		

#define	QC_FREE_STRUCTS_ONLY	0	
#define	QC_FREE_CONTENTS		1

#define LOGCOVAVG_ERROR		1e-12

		 
// Macros

// Element of DT_MATRIX				  
#define	MATR( Matrix, Row, Column )				DT_MATRIX_ELEM( Matrix, Row, Column )

// Elements of QC_VEC_ARRAY					   
#define	QCVA( Qcva, i1, i2 )   					(Qcva)->Vec[ i1 ]->Elem[ i2 ]
// Elements of QC_VEC_ARR2D					   
#define	QCVA2D( Qcva, i1, i2, i3 )   			(Qcva)->VecArray[ i1 ]->Vec[ i2 ]->Elem[ i3 ]

// Number of covariances if there are a given number of factors
#define N_COV( n_fact ) \
	(( n_fact * ( n_fact + 1 )) / 2 )

// Number of correlations if there are a given number of factors
#define N_CORR( n_fact ) \
	(( n_fact * ( n_fact - 1 )) / 2 )

	
// ------------------------------------------------------------------------------------
// TypeDefs
// ------------------------------------------------------------------------------------

  
typedef struct
{
	int				count;
	DT_TCURVE		**TCrv;

} QC_TCRV_ARRAY; // 'TCrvArray'


typedef struct
{
	int				count;
	DT_CURVE		**Crv;

} QC_CRV_ARRAY; // 'CrvArray'


typedef struct
{
	int				count;
	DT_VECTOR		**Vec;

} QC_VEC_ARRAY;	// 'VecArray
  

typedef struct
{
	int				count;
	QC_VEC_ARRAY	**VecArray;

} QC_VEC_ARR2D;	// 'VecArr2d'

  
typedef struct
{
	int				n_Steps,  			// number of MC time steps
					n_Futures,	       	// number of Futures (expirations)
					n_Factors;			// number of model factors
	QC_VEC_ARRAY   	*MrFactStep,		// MR factors w\in MC steps
					*ExpFutCovEntire,	// Futures Covs from Start to StepTime
					*VcFactFut;			// VC factors for Futures Expirys
	QC_VEC_ARR2D   	*FactorCovmatStep,	// Factor Covariances over MC steps
					*MrFactEndFut;		// MR factors from Step End to Futures Expiry

} QC_MC_PRECALC; // McPre


typedef struct
{
	int				Seed,
					UseAnti,
					NextPathIsAnti;
	DT_MATRIX		*ExpRandSave;
	DT_VECTOR		*W,       
					*dW_0,    
					*dW_corr; 

} QC_MC_SAVE; // McSave


typedef struct
{
	int				n_Futures,	       	// number of Futures (expirations)
					n_Factors,			// number of model factors
					n_Grid;				// number of grid points - 1 = max index value for grid point
	DT_VECTOR		*Probs,
					*FutMedians;
	QC_VEC_ARRAY	*FutStepFactor,	
					*FutBigFactor;

} QC_GRID_PRECALC; // GridPre

// ------------------------------------------------------------------------------------
// Exports
// ------------------------------------------------------------------------------------


DLLEXPORT double QcExpFast(
    double x
);


DLLEXPORT double QcExpIntFast(
    double x,	// x
	double y	// exp( -x ); [ must have been precalculated !!]
);


DLLEXPORT int QcLogCovAvg(
	double	ExpCovSum,
	double	PriceAvg1,
	double	PriceAvg2,
	double	dT,
	double	*LogCovAvg	// result
);


DLLEXPORT int QcLogVarAvg(
	double	ExpCovSum,
	double	PriceAvg,
	double	dT,
	double	*LogVarAvg	// result
);


DLLEXPORT int QcCubeInterp_SM_Coeff(
	DT_VECTOR 	*x_Values,      // vector of x values (Quick Deltas, Strikes etc)
	double    	x,              // specific x value
    DT_VECTOR   **y_Coeffs	   	// RESULT: vector of y COEFFICIENTS
);


DLLEXPORT int QcBuildOneMcPath(
	QC_MC_PRECALC	*McPre,				// Precalculated Model Info
	DT_TCURVE 		*FutPricesInit,  	// initial futures prices
	QC_MC_SAVE		*McSave,			// This is ** USED _and_ MODIFIED ** 
	Q_MATRIX		*FutPrices			// Result: Futures prices ('0' if already expired) [NOTE: NOT allocated here]
);


DLLEXPORT int QcInitMcSave(
	int			Seed,
	int			UseAnti,
	int			n_Steps,
	int			n_Futs,
	int			n_Fact,
	QC_MC_SAVE	**McSave
);


DLLEXPORT int QcGridPrecalc(
	QC_MC_PRECALC	*McPre, 
	double			n_Devs, 
	int				n_GridHalf, 
	DT_TCURVE 		*FutPricesInit,  	// initial futures prices
	QC_GRID_PRECALC	**GridPre 			// result: Grid precalculated stuff
);


DLLEXPORT int QcInitGrid(
	int		n_Factors,
	int		**GridIndex
);


DLLEXPORT int QcGetOneGridPoint(
	QC_GRID_PRECALC	*GridPre,		// Precalculated stuff - used here
	int				*GridIndex,		// Grid Indices - they are incremented here
	Q_VECTOR		*FutPrices,		// Futures prices - are modified here
	double			*Prob,			// probability for this grid point - calculated here
	int				*Done			// this is set to 'TRUE' if all grid points have been sampled
);


DLLEXPORT int QcCovarianceAdj_SM(
	double		CovAvg,					// Un-adjusted covariance
	double		FwdRatio1,				// asset 1 ratio of fwds
	double		FwdRatio2,				// asset 2 ratio of fwds
	DT_TIME	 	TodayTime,		   		// start date
	DT_TIME		ExpTime,				// expiration date
	double 		*CovAvgPtr				// (ptr to) adjusted cov
);


DLLEXPORT int QcCholeskyDecomposition(
	Q_MATRIX	*A
);


#endif

