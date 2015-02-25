/****************************************************************
**
**	Richardson.h	- Richardson iteration for solving linear
**					  systems
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_RICHARDSON_H )
#define IN_RICHARDSON_H

#include <gsquant/base.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
**	Routine: IR
**	Returns: int
**	Action : 
**
** IR solves the unsymmetric linear system Ax = b using 
** Iterative Refinement (preconditioned Richardson iteration).
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
int IR(
	const Matrix&		  A,
	Vector&				  x,
	const Vector&		  b,
    const Preconditioner& M,
	int&			      max_iter,
	Real&				  tol
);

CC_END_NAMESPACE
#endif

