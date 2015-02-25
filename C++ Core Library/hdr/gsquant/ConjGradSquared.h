/****************************************************************
**
**	ConjGradSquared.h	- Templated conjugated gradient squared
**						  function
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_CONJGRADSQUARED_H )
#define IN_CONJGRADSQUARED_H

#include <gsquant/base.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
**	Routine: CGS
**	Returns: int
**	Action : 
**
** CGS solves the unsymmetric linear system Ax = b 
** using the Conjugate Gradient Squared method
**
** CGS follows the algorithm described on p. 26 of the 
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
int CGS(
	const Matrix &A,
	Vector &x,
	const Vector &b,
    const Preconditioner &M,
	int &max_iter,
	Real &tol
);

CC_END_NAMESPACE
#endif

