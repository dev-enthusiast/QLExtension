/****************************************************************
**
**	MINIMIZATION.H	Minimization routines header
**
**	Copyright 2006 - Constellation Energy Commodities Group - Baltimore, MD
**
**	$Log: minimization.h,v $
**	Revision 1.4  2007/07/07 22:53:12  e13749
**	Initial port from Slang to C of _LIB Elec Math Minimization v2.
**
**	Revision 1.3  2006/10/05 18:30:25  e11600
**	Removed extra GSCVSID
**	
**	Revision 1.2  2006/10/05 14:47:03  e11600
**	Added standard header.
**	
**	
**	
****************************************************************/

#ifndef IN_MINIMIZATION_H
#define IN_MINIMIZATION_H

#include	<portable.h>
#include 	<assert.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<secdb.h>
#include	<secerror.h>
#include	<secexpr.h>
#include	<slangfnc.h>
#include	<sq_arg.h>
#include	<num.h>
#include	<secdb/sdb_exception.h>
#include	<kool_ade.h>
#include	<date.h>
#include	<datatype.h>
#include	<math.h>


#define DT_VECTOR_ELEM( _Vector, _i ) ( ( _Vector )->Elem[ _i ] )


// Function type used as minimization function argument
typedef enum
{
	FUNCTION_ANALYTIC,
	FUNCTION_NUMERIC

} FUNCTION_TYPE;


// Return struct of Analytic functions.
typedef struct
{
	double		Value;
	DT_VECTOR	*Gradient;

} ANALYTIC_FUNC_RET;


// Different gradient methods implemented
typedef enum
{
	GRADIENT_ANALYTIC,
	GRADIENT_FORWARD_DIFFERENCE,
	GRADIENT_CENTRAL_DIFFERENCE,
	GRADIENT_DEFAULT
} GRADIENT_METHOD;

// Different types of Hessian matrix implemented
typedef enum
{
	HESSIAN_FORWARD_FUNCTION_DIFFERENCE,
	HESSIAN_FORWARD_GRADIENT_DIFFERENCE,
	HESSIAN_CENTRAL_FUNCTION_DIFFERENCE,
	HESSIAN_CENTRAL_GRADIENT_DIFFERENCE,
	HESSIAN_BFGS,
	HESSIAN_DEFAULT

} HESSIAN_METHOD;


// Funtion pointer definition
typedef double (*GENERIC_FUNC_VECTOR)( double *x, void *Context );
typedef ANALYTIC_FUNC_RET *(*GENERIC_ANALYTIC_FUNC) ( double *x, void *Context );


// Generic function pointer storage datatype
typedef struct
{
	FUNCTION_TYPE Type;
	GENERIC_FUNC_VECTOR Numeric; 
	GENERIC_ANALYTIC_FUNC Analytic;

} GENERIC_FUNCTION_POINTER;


typedef enum
{

	N_OPTIM_LEVENBERG_MARQUARDT,
	N_OPTIM_QUASI_NEWTON,
	N_OPTIM_FLETCHER_REEVES,
	N_OPTIM_SIMMULATED_ANNEALING

} OPTIM_METHOD;

typedef struct
{
	double Lambda0;
    double LambdaFactor;
    double LambdaTol;

} METHOD_PARAMETER;


typedef struct 
{
	OPTIM_METHOD				Method;
	HESSIAN_METHOD				HessianUpdate;
	GRADIENT_METHOD				GradientUpdate;
	double						GradientTol;
	double						FunctionTol;
	int							MaxIter;
	short						PrintResult;
	double						FailureTol;
	METHOD_PARAMETER			MethodParams;

} OPTIMIZATION_OPTION;


typedef struct
{
	SLANG_NODE		*Root;		
	SLANG_EVAL_FLAG	EvalFlag;	
	SLANG_SCOPE		*Scope;

} SLANG_FUNCTION_ARGS;


#define Hessian_FFD		Hessian_ForwardFunctionDifference
#define Gradient_FD		Gradient_ForwardDifference

DLLEXPORT int N_MinimizeUnconstrained(
	N_GENERIC_FUNC_VECTOR	 func,
	double					*x,
	void					*Context,
	double					 ftol
);

int N_LevenbergMarquardt(
	N_GENERIC_FUNC_VECTOR	 func,
    double					*x0,
    void					*Context,
    OPTIMIZATION_OPTION		*ops
);

int Hessian(
	DT_MATRIX						*H,
    HESSIAN_METHOD					 method,
    N_GENERIC_FUNC_VECTOR			 func,
    double							*x,
	SLANG_N_GENERIC_FUNC_CONTEXT	*Context,
    double							*f0,
    double							*dF,
    double							 r,
    double							*dF0,
    double							*x0,
    GRADIENT_METHOD					 gradMethod
);

int Hessian_ForwardFunctionDifference(
	DT_MATRIX						*H,
    N_GENERIC_FUNC_VECTOR			 func,
    double							*x,
    SLANG_N_GENERIC_FUNC_CONTEXT	*Context,
    double							 r,
    double							*f00,
    double							*dF 
);



int Gradient_ForwardDifference(
    N_GENERIC_FUNC_VECTOR			 func,
    double							*x,
    SLANG_N_GENERIC_FUNC_CONTEXT	*Context,
    double							 r,
    double							*f00,
	DT_VECTOR						*dF
);


#endif
