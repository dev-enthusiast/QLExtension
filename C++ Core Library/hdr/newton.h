/****************************************************************
**
**	newton.h	- n-dimensional newton's method with context variable
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/num/src/newton.h,v 1.4 2001/10/12 11:33:10 demeor Exp $
**
****************************************************************/

#if !defined( IN_NEWTON_H )
#define IN_NEWTON_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif

#include <num.h>

EXPORT_C_NUM int N_NewtonND(
	double 				  x[], 	   		// input vector of n doubles
	int 				  n, 			// number of dimensions
	int 				  *check,		// 1 if error, 0 if success
	N_GENERIC_FUNC_VECTOR vecfunc, 		// function from R[n] to R[n]
	int                   MAXITS,       // maximum allowable iterations
	double                TOLF,         // tolerance on maximum of n function outputs
	void 				  *args 	   	// context variable
	);

EXPORT_C_NUM double fmin2(
	int    nn,
	double x[],
	void *args
	);
	
EXPORT_C_NUM void fdjac2(
	int 				  n,        // number of dimensions
	double 				  x[], 	   	// input vector of n doubles
	double 				  fvec[], 	// output vector of n doubles
	double 				  **df,	   	// output matrix
	N_GENERIC_FUNC_VECTOR vecfunc, 	// function from R[n] to R[n] 
	void 				  *args		// context variable
	);

EXPORT_C_NUM int ludcmp(
	double 			**a, 
	int 			n, 
	int 			*indx, 
	double 			*d
	);

EXPORT_C_NUM void lubksb(
	double 			**a, 
	int 			n, 
	int 			*indx, 
	double 			b[]
	);

EXPORT_C_NUM int lnsrch2(
	int n,             // number of dimensions 
	double xold[], 
	double fold, 
	double g[], 
	double p[], 
	double x[],
	double *f, 
	double stpmax, 
	int *check, 
	double (*func)(int n, double [], void *args), 
	void *args
	);
	
#endif
	

