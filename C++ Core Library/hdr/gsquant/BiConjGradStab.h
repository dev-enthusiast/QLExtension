/****************************************************************
**
**	BiConjGradStab.h	- Stabilised conjugant gradient method of
**						  solving linear equations
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_BICONJGRADSTAB_H )
#define IN_BICONJGRADSTAB_H

#include <gsquant/base.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
**	Routine: BiCGSTAB
**	Returns: int
**	Action : 
**
** BiCGSTAB solves the unsymmetric linear system Ax = b 
** using the Preconditioned BiConjugate Gradient Stabilized method
**
** BiCGSTAB follows the algorithm described on p. 27 of the 
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
int BiCGSTAB(
	const Matrix&		  A,
	Vector&				  x,
	const Vector&		  b,
    const Preconditioner& M,
	int&				  max_iter,
	Real&				  tol
);

CC_END_NAMESPACE
#endif

