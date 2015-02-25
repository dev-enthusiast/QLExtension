/* $Header: /home/cvs/src/num/src/num.h,v 1.70 2001/09/16 21:42:08 andret Exp $ */
/****************************************************************
**
**	NUM.H
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.70 $
**
****************************************************************/

#if !defined( IN_NUM_H )
#define IN_NUM_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif

#if !defined( IN_DATATYPE_H )
#include <datatype.h>
#endif

#if !defined( IN_DATE_H )
#include <date.h>
#endif

#if !defined( GRID_WIDTH_NUM )
#define GRID_WIDTH_NUM 5.5
#endif





/*
**	Define enumerated flags used by various routines
*/

typedef enum
{
	N_TRAPEZOID_RULE,
	N_MIDPOINT_RULE,
	N_MIDPOINT_RULE_INFINITE

} N_NEWTON_COTES_FORMULA;

typedef enum
{
	N_RUNGE_KUTTA,
	N_BULIRSCH_STOER

} N_ODE_METHOD;

typedef enum
{
	N_1D_DX_TWO_POINT,
	N_1D_DX2_THREE_POINT,
	N_1D_DX2_FIVE_POINT,
	N_1D_DX4_FIVE_POINT
} N_PARTIAL_DERIVATIVE_1D_RULE;

typedef enum
{
	N_2D_DX_FOUR_POINT,
	N_2D_DX2_NINE_POINT,
	N_2D_DXDY_FOUR_POINT,
	N_2D_DXDY_SEVEN_POINT,
	N_2D_DX2DY2_NINE_POINT,
	N_2D_LAPLACIAN_FIVE_POINT,
	N_2D_LAPLACIAN_NINE_POINT,
	N_2D_BIHARMONIC_THIRTEEN_POINT,
	N_2D_BIHARMONIC_TWENTYFIVE_POINT
} N_PARTIAL_DERIVATIVE_2D_RULE;

typedef enum
{
	N_GAUSS_LEGENDRE_8PT,
	N_GAUSS_LEGENDRE_16PT,
	N_GAUSS_LEGENDRE_24PT,
	N_GAUSS_LEGENDRE_32PT,
	N_GAUSS_LEGENDRE_48PT,
	N_GAUSS_TRIGONOMETRIC_2PT,
	N_GAUSS_TRIGONOMETRIC_4PT,
	N_GAUSS_TRIGONOMETRIC_8PT,
	N_GAUSS_TRIGONOMETRIC_16PT

} N_GAUSS_QUADRATURE_RULE;

typedef enum
{
	N_SPLINE_BDRY_KNOWN_DERIV, 	// f'(a) or f'(b) is specified
	N_SPLINE_BDRY_NATURAL,		// linear behavior at end point
	N_SPLINE_BDRY_PARABOLIC,	// quadratic behavior at end point
	N_SPLINE_BDRY_NOT_A_KNOT,	// two end intervals fitted by one cubic
	N_SPLINE_BDRY_FLAT_TAIL		// flattens to a horizontal line at a specified place outside the given end point

} N_SPLINE_BDRY_CONDITION;

typedef enum
{
	N_SPLINE_EVAL_DEFAULT,		// Evaluate the spline function
	N_SPLINE_EVAL_DERIV,		// Evaluate the derivative of the spline function
	N_SPLINE_EVAL_SECOND_DERIV	// Evaluate the second derivative of the spline function

} N_SPLINE_EVALUATION_TYPE;

typedef enum
{
	N_OPEN_TRANSFORM,
	N_CLOSED_TRANSFORM

} N_TRANSFORM_TYPE;

typedef enum
{
	N_IF_MISSING_IGNORE_OBS,
	N_IF_MISSING_FAIL

} N_IF_MISSING;


/*
**	Define generic callback functions
*/

typedef int (*N_GENERIC_FUNC)( 			double x, void *Context, double *Result );
typedef int (*N_GENERIC_FUNC_2D)(		double x, double y, void *Context, double *Result );
typedef int (*N_GENERIC_FUNC_3D)(		double x, double y, double z, void *Context, double *Result );
typedef int (*N_GENERIC_FUNC_VECTOR)(	double *x, void *Context, double *Result );
typedef int (*N_GENERIC_FUNC_PARAM1_VECTOR)(	double p, double *x, void *Context, double *Result );
typedef int (*N_GENERIC_FUNC_TWO_RESULT)(	double x, void *Context, double *Result0, double *Result1 );

#define		CALL_N_GENERIC_FUNC( f, x, c, r, caller )													\
			{																							\
				if( !(*(f))( x, c, r))																	\
					return ErrMore( #caller ": Generic function evaluation failed" );					\
																										\
				if( !VALID_DOUBLE( *r ))																\
					return Err( ERR_OVERFLOW, #caller ": Generic function returned invalid double" );	\
			}																							\

#define		CALL_N_GENERIC_FUNC_2D( f, x, y, c, r, caller )												\
			{																							\
				if( !(*(f))( x, y, c, r))																\
					return ErrMore( #caller ": Generic function evaluation failed" );					\
																										\
				if( !VALID_DOUBLE( *r ))																\
					return Err( ERR_OVERFLOW, #caller ": Generic function returned invalid double" );	\
			}																							\

#define		CALL_N_GENERIC_FUNC_3D( f, x, y, z, c, r, caller )												\
			{																							\
				if( !(*(f))( x, y, z, c, r))																\
					return ErrMore( #caller ": Generic function evaluation failed" );					\
																										\
				if( !VALID_DOUBLE( *r ))																\
					return Err( ERR_OVERFLOW, #caller ": Generic function returned invalid double" );	\
			}																							\

#define		CALL_N_GENERIC_FUNC_VECTOR( f, x, c, r, caller )													\
			{																							\
				if( !(*(f))( x, c, r ) )																	\
					return ErrMore( #caller ": Generic function evaluation failed" );					\
																										\
				if( !VALID_DOUBLE( *r ) )																\
					return Err( ERR_OVERFLOW, #caller ": Generic function returned invalid double" );	\
                        }
/* same as above with invalid double return */
#define		CALL_N_GENERIC_FUNC_VECTOR_D( f, x, c, r, caller )													\
			{																							\
				if( !(*(f))( x, c, r ) )																	\
					return ErrMoreD( #caller ": Generic function evaluation failed" );					\
																										\
				if( !VALID_DOUBLE( *r ) )																\
					return ErrD( ERR_OVERFLOW, #caller ": Generic function returned invalid double" );	\
            }

#define		CALL_N_GENERIC_FUNC_PARAM1_VECTOR( f, p, x, c, r, caller )													\
			{																							\
				if( !(*(f))( p, x, c, r ) )																	\
					return ErrMore( #caller ": Generic function evaluation failed" );					\
																										\
				if( !VALID_DOUBLE( *r ) )																\
					return Err( ERR_OVERFLOW, #caller ": Generic function returned invalid double" );	\
			}																							\

#define		N_GENERIC_WRAPPER( Function, Wrapper )														\
			int Wrapper( double x, void* ignore, double *r )											\
			{																							\
				*r = (Function)( x );																	\
				return TRUE;																			\
			}																							\

#define		N_GENERIC_WRAPPER_2D( Function, Wrapper )													\
			int Wrapper( double x, double y, void* ignore, double *r )									\
			{														  									\
				*r = (Function)( x, y );							  									\
				return TRUE;										  									\
			}														  									\

#define		N_GENERIC_WRAPPER_VECTOR( Function, Wrapper )													\
			int Wrapper( double *x, void* ignore, double *r )														\
			{																							\
				*r = (Function)( x );																		\
				return TRUE;																			\
			}																							\

#define		N_GENERIC_WRAPPER_PARAM1_VECTOR( Function, Wrapper )													\
			int Wrapper( double p, double *x, void* ignore, double *r )														\
			{																							\
				*r = (Function)( p, x );																		\
				return TRUE;																			\
			}																							\



/*
**	Define data structures used by various routines
*/

typedef struct
{
	DT_VECTOR
			*Values;

	DT_MATRIX
			*Vectors;

} NUM_EIGEN_VALUES;

typedef struct NMatrixDimStructure
{
	int		Row,
			Col;
}
N_MATRIX_DIMENSION;

typedef struct NMatrixStructure
{
	N_MATRIX_DIMENSION
			Alloc,
			Size;

	double	**Elem;
}
N_MATRIX;

typedef struct NSplineStructure
{
	DT_VECTOR
			*x,		// x values
			*y,		// y(x) values
			*M;		// Spline moments: M(x) = y''(x)

} N_SPLINE;

typedef struct NProdSumStructure
{
	int		NumVars,			// Number of variables
			NumObs;				// Number of (valid) observations

	N_IF_MISSING
			MissingDataFlag;	// Action to take for missing data

	DT_VECTOR
			*Means;				// Arithmetic means

	DT_MATRIX
			*ProductSums;		// Lower triangular matrix of sum of
								// crossproducts (sum of squares in
								// diagonal)

	double	FreqSum,			// SUM( Frequency )
			FreqWgtSum;			// SUM( Frequency * Weight )

} N_PROD_SUM;

typedef struct NRegressionStructure N_REGRESSION;

struct NRegressionStructure
{
	DT_VECTOR
			*Coeff,		// Fitted coefficients
			*CoeffVar;	// Estimated variance of each coefficient

	double	Intercept,	// Intercept = Y(0)
			RSS,		// Residual sum of squares = [rr]
			RSq,		// R squared = [YY]/[yy]
			RSqAdj,		// Adjusted R squared from (1-R[adj]**2)/(1-R[y]**2) = (N-1)/(N-K-1)
			FValue,		// F value = ([YY]/[rr]) * (N-K-1)/K
			RMS;		// Residual mean square (estimated var.(y)) = [rr]/(N-K-1)

	N_REGRESSION
			*Next;
};


/*
**	Declare functions
*/

EXPORT_C_NUM int
 		N_QPPSolve(						DT_VECTOR *VectorA, DT_MATRIX *MatrixA, DT_MATRIX *MatrixB,	DT_VECTOR *VectorB, DT_VECTOR **ResultVec );

EXPORT_C_NUM int
 		N_MatrixEigenFind(				DT_MATRIX *Matrix, NUM_EIGEN_VALUES *EigenValues );

EXPORT_C_NUM double
		N_PolyInterp(					DT_VECTOR *XVals, DT_VECTOR *YVals, double XInterp, double *YInterp ),
		N_RatInterp(					DT_VECTOR *XVals, DT_VECTOR *YVals, double XInterp, double *YInterp ),
		N_PolyExtrapZero(				int StepNum, DT_VECTOR *XVals, DT_VECTOR *YVals, double *YExtrap, DT_VECTOR *Column ),
		N_RatExtrapZero(				int StepNum, DT_VECTOR *XVals, DT_VECTOR *YVals, double *YExtrap, DT_VECTOR *Column );

EXPORT_C_NUM int
		N_RombergIntegration(			N_GENERIC_FUNC Func, void *Context, double a, double b, int Order, N_NEWTON_COTES_FORMULA Rule, double Tol, double *Answer ),
		N_GaussQuadrature(				N_GENERIC_FUNC Func, void *Context, double a, double b, N_GAUSS_QUADRATURE_RULE Rule, double *Answer ),
		N_BrentMin(						N_GENERIC_FUNC Func, void *Context, double Low, double High, double Mid, double Tolerance, double *Result ),
		N_Bracket(						N_GENERIC_FUNC Func, void *Context, double A, double B, double *Low, double *Mid, double *High, double *LowVal, double *MidVal, double *HighVal ),
		N_BrentRoot(					N_GENERIC_FUNC Func, void *Context, double fval, double Low, double High, double Tolerance, double *Result ),
		N_Newton(						N_GENERIC_FUNC_TWO_RESULT, void *Context, double fvalue, double InitialGuess, double Tolerance, double MaxIter, double *Result ),
		N_SecantRoot(					N_GENERIC_FUNC Func, void *Context, double fval, double InitVal, double Tolerance, double *Result ),
		N_IntegrateODE(					N_ODE_METHOD Method, N_GENERIC_FUNC_2D FCalc, void *Context, double XInit, double XEnd, double YInit, double Eps, double HInit, double HMin,
										int *NumGood, int *NumBad, double DxSav, DT_VECTOR *XRet, DT_VECTOR *YRet );

EXPORT_C_NUM int
		N_FirstPrime(					int	n );

EXPORT_C_NUM DT_VECTOR
		**N_FaureSequence(				int	Dimension, int Npts, double Beginning ),
		**N_FaureGaussianSequence(		int	Dimension, int Npts, double Beginning );

EXPORT_C_NUM double
		N_CumBiNormDreznerOld(			double a, double b, double rho ),
		N_CumBiNormDrezner(				double a, double b, double rho ),
		N_CumBiNormYDM(					double a, double b, double rho ),
		N_CumBiNormIMSL(				double a, double b, double rho ),
		N_CumNorm(  					double Value ),
		N_CumNormInv(  					double Value );

EXPORT_C_NUM int
		IsPowerofTwo(					int PossiblePowerOfTwo );

EXPORT_C_NUM int
		N_ComplexFFT(   				DT_COMPLEX *Data, int Size  ),
		N_ComplexFFTInv( 				DT_COMPLEX *Data, int Size  ),
		N_ComplexFFTND(   				DT_COMPLEX *Data, int *Size, int dim  ),
		N_ComplexFFTNDInv( 				DT_COMPLEX *Data, int *Size, int dim  ),
		N_RealFFT(   					double *Data, int Size ),
		N_RealFFTInv(  					double *Data, int Size ),
		N_SineTransform(  				double *Data, int Size ),
		N_SineTransformInv( 			double *Data, int Size ),
		N_CosineTransform(  			double *Data, int Size ),
		N_CosineTransformInv(			double *Data, int Size ),
		N_CosineOpenTransform(  		double *Data, int Size ),
		N_CosineOpenTransformInv(  		double *Data, int Size );

EXPORT_C_NUM int N_RealFFTND(
	double      *Values,    // real input values, to be FFT'd
	int			*Size,		// Must be powers of 2
	int         dim,		// number of dimensions
	DT_COMPLEX	*Data		// output complex data, top dim is size n/2+1 instead of n
	);

EXPORT_C_NUM int N_RealFFTNDInv(
	DT_COMPLEX	*Data,		// Data to be FFT'd with top dimension size n/2+1 instead of n
	int			*Size,		// Must be powers of 2
	int         dim,		// number of dimensions
	double      *Values     // real input values, to be FFT'd
	);

EXPORT_C_NUM DT_COMPLEX
		*N_RealFFTValue(				double *Data, int Size, int index, DT_COMPLEX *Answer );

EXPORT_C_NUM int
		N_ClenshawCurtisAdaptiveQuad(	N_GENERIC_FUNC f, void *Context, double a, double b, int *NumPoints, double *Answer, double *ErrTolerance ),
		N_ClenshawCurtisProgressiveQuad(N_GENERIC_FUNC f, void *Context, double a, double b, int *NumPoints, double *Answer, double *ErrTolerance ),
		N_ClenshawCurtisQuad(			N_GENERIC_FUNC f, void *Context, double a, double b, int NumPoints, double *Answer, double *ErrEstimate );

EXPORT_C_NUM N_SPLINE
		*N_SplineInit(					DT_VECTOR *XVals, DT_VECTOR *YVals, N_SPLINE *Spline );

EXPORT_C_NUM int
		N_SplineCalc(					N_SPLINE *Spline, N_SPLINE_BDRY_CONDITION LeftBdryCond, N_SPLINE_BDRY_CONDITION RightBdryCond, double LeftData, double RightData ),
		N_SplineCalcSimple(				N_SPLINE *Spline, N_SPLINE_BDRY_CONDITION LeftBdryCond, N_SPLINE_BDRY_CONDITION RightBdryCond, double LeftData, double RightData );

EXPORT_C_NUM double
		N_SplineEval(					N_SPLINE *Spline, double XValue ),
		N_SplineEvalDeriv(				N_SPLINE *Spline, double XValue ),
		N_SplineEvalSecondDeriv(		N_SPLINE *Spline, double XValue ),
		N_SplineEvalSimple(				N_SPLINE *Spline, double XValue );

EXPORT_C_NUM void
		N_SplineFree(					N_SPLINE *Spline );

EXPORT_C_NUM DT_VECTOR
		*N_ChebyshevTransform(			N_GENERIC_FUNC Func, void *Context, double a, double b, N_TRANSFORM_TYPE Type, int NumPoints, DT_VECTOR *Approx );

EXPORT_C_NUM int
		ChebyshevTransformFuncEval(		N_GENERIC_FUNC Func, void *Context,	double a, double b,	N_TRANSFORM_TYPE Type, DT_VECTOR *FuncVals );

EXPORT_C_NUM double
		N_ChebyshevEval(				DT_VECTOR *Approx, N_TRANSFORM_TYPE Type, double  a, double  b, double  x );

EXPORT_C_NUM double
		N_Ran1(							int *Seed ),
		N_Ran3(							int *Seed ),
		N_Ran4(							int *Seed ),
		N_GaussDeviate(					int *Seed );

EXPORT_C_NUM int
		N_RandomBit( unsigned long *piseed );

EXPORT_C_NUM int
		N_Amoeba(						double **p, double y[], int NumDimensions, double FTol, N_GENERIC_FUNC_VECTOR Funk, void *Context, int *NFunk );

EXPORT_C_NUM int
		N_AR1(							double Alpha, int Seed, int NumPaths, DT_VECTOR *Means, DT_VECTOR *Vols, DT_MATRIX *Samples );

EXPORT_C_NUM int
		N_ProdSumAddObs(				N_PROD_SUM *ProdSum, DT_VECTOR *Obs, double Frequency, double Weight ),
		N_ProdSumFinish(				N_PROD_SUM *ProdSum );

EXPORT_C_NUM void
		N_ProdSumFree(					N_PROD_SUM *ProdSum );

EXPORT_C_NUM N_PROD_SUM
		*N_ProdSumInit(					int NumberOfVariables, N_IF_MISSING MissingDataFlag );

EXPORT_C_NUM DT_MATRIX
		*N_Correlation(					N_PROD_SUM *ProdSum ),
		*N_Covariance(					N_PROD_SUM *ProdSum );

EXPORT_C_NUM N_REGRESSION
		*N_Regression(					N_PROD_SUM *ProdSum, int DependentVar ),
		*N_RegressionsBest(				N_PROD_SUM *ProdSum, int DependentVar ),
		*N_RegressionsAll(				N_PROD_SUM *ProdSum, int DependentVar );

EXPORT_C_NUM void
		N_RegressionFree(				N_REGRESSION *Regression );

EXPORT_C_NUM int
		N_ShanksAccelerator(			double SeriesTerm, double ErrTol, DT_VECTOR *Work, double *Answer );

EXPORT_C_NUM N_MATRIX
		*N_MatrixAlloc( 				int RowSize, int ColSize ),
		*N_MatrixCopy( 					N_MATRIX *SourceMatrix );

EXPORT_C_NUM void
		N_MatrixFree( 					N_MATRIX *Matrix ),
		N_MatrixResize( 				N_MATRIX *Matrix, int NewRowSize, int NewColSize );

EXPORT_C_NUM double
		N_expm1(						double x ),
		N_expm1x(						double x ),
		N_log1p(						double x ),
		N_loggamma(						double x ),
		N_factorial(					int n    ),
		N_Beta(							double a, double b ),
		N_IncompleteBeta( 				double a, double b, double x ),
		N_IncompleteGamma(				double a, double x );


EXPORT_C_NUM int
		N_QuadRootReal(					double	a, double	b, 	double	c, 	double	*rlarge, double	*rsmall );

EXPORT_C_NUM int
		N_Partial_Derivative_1D( N_GENERIC_FUNC f, double x0, void *Context, N_PARTIAL_DERIVATIVE_1D_RULE Rule, double Stepsize, double *Answer ),
		N_Partial_Derivative_2D( N_GENERIC_FUNC_2D f, double x0, double y0, void *Context, N_PARTIAL_DERIVATIVE_2D_RULE Rule, double Stepsize, double *Answer );

#endif

