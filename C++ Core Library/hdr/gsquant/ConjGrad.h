/****************************************************************
**
**	ConjGrad.h	- Conjugant gradient method of solving linear
**		equations
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_CONJGRAD_H )
#define IN_CONJGRAD_H

#include <gsquant/base.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
**	Routine: CG
**	Returns: int
**	Action : 
** 
**  CG solves the symmetric positive definite linear
**  system Ax=b using the Conjugate Gradient method.
** 
**  CG follows the algorithm described on p. 15 in the 
**  SIAM Templates book.
** 
**  The return value indicates convergence within max_iter (input)
**  iterations (0), or no convergence within max_iter iterations (1).
** 
**  Upon successful return, output arguments have the following values:
**   
**         x  --  approximate solution to Ax = b
**  max_iter  --  the number of iterations performed before the
**                tolerance was reached
**       tol  --  the residual after the final iteration
**
****************************************************************/

template < class Matrix, class Vector, class Preconditioner, class Real >
int CG(
	const Matrix& 		  A,
	Vector&				  x,
	const Vector&		  b,
	const Preconditioner& M,
	int&				  max_iter,
	Real&				  tol
);

CC_END_NAMESPACE

#endif

