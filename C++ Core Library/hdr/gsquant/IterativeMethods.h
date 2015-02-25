/****************************************************************
**
**	IterativeMethods.h	- exported IML++ (iterative methods
**		library) functions. These are used to solve a linear
**		equation of the form Ax = b where x and b are GsVectors
**		and A is a GsMatrix.
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_ITERATIVEMETHODS_H )
#define IN_ITERATIVEMETHODS_H

#include	<gsquant/base.h>
#include	<gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsVector GsConjugateGradients(
	const GsMatrix&		A,		 // the multiplying matrix
	const GsVector&		b,		 // the vector on the RHS
	const GsMatrix&		M,		 // the preconditioner
	double				tol,	 // the tolerance
	int					max_iter // maximum number of iterations
);

EXPORT_CPP_GSQUANT GsVector GsBiConjugateGradients(
	const GsMatrix&		A,		 // the multiplying matrix
	const GsVector&		b,		 // the vector on the RHS
	const GsMatrix&		M,		 // the preconditioner
	double				tol,	 // the tolerance
	int					max_iter // maximum number of iterations
);

EXPORT_CPP_GSQUANT GsVector GsConjugateGradientSquared(
	const GsMatrix&		A,		 // the multiplying matrix
	const GsVector&		b,		 // the vector on the RHS
	const GsMatrix&		M,		 // the preconditioner
	double				tol,	 // the tolerance
	int					max_iter // maximum number of iterations
);

EXPORT_CPP_GSQUANT GsVector GsBiConjugateGradientStab(
	const GsMatrix&		A,		 // the multiplying matrix
	const GsVector&		b,		 // the vector on the RHS
	const GsMatrix&		M,		 // the preconditioner
	double				tol,	 // the tolerance
	int					max_iter // maximum number of iterations
);

EXPORT_CPP_GSQUANT GsVector GsRichardsonIterate(
	const GsMatrix&		A,		 // the multiplying matrix
	const GsVector&		b,		 // the vector on the RHS
	const GsMatrix&		M,		 // the preconditioner
	double				tol,	 // the tolerance
	int					max_iter // maximum number of iterations
);

EXPORT_CPP_GSQUANT GsVector GsQuasiMinRes(
	const GsMatrix&		A,		 // the multiplying matrix
	const GsVector&		b,		 // the vector on the RHS
	const GsMatrix&		M1,		 // the preconditioner
	const GsMatrix&		M2,		 // the preconditioner
	double				tol,	 // the tolerance
	int					max_iter // maximum number of iterations
);

CC_END_NAMESPACE
#endif

