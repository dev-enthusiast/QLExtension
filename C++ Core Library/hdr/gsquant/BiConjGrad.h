/****************************************************************
**
**	BiConjGrad.h	- Bi-conjugate gradient method of solving
**		linear equations
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_BICONJGRAD_H )
#define IN_BICONJGRAD_H

#include <gsquant/base.h>

CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
**	Routine: BiCG
**	Returns: int
**	Action : 
**
** BiCG solves the unsymmetric linear system Ax = b 
** using the Preconditioned BiConjugate Gradient method
**
** BiCG follows the algorithm described on p. 22 of the 
** SIAM Templates book.
**
** The return value indicates convergence within max_iter (input)
** iterations (0), or no convergence within max_iter iterations (1).
**
** Upon successful return, output arguments have the following values:
**  
**        x  --  approximate solution to Ax = b
** max_iter  --  the number of iterations performed before the
**               tolerance was reached
**      tol  --  the residual after the final iteration
**  
******************************************************************/

template < class Matrix, class Vector, class Preconditioner, class Real >
int BiCG(
	const Matrix&		  A,
	Vector&				  x,
	const Vector&		  b,
    const Preconditioner& M,
	int&			      max_iter,
	Real&				  tol
);

CC_END_NAMESPACE

#endif

