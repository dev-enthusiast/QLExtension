/****************************************************************
**
**	QuasiMinRes.h	- Quasi-Minimal Residual Without Lookahead
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_QUASIMINRES_H )
#define IN_QUASIMINRES_H

#include <gsquant/base.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
**	Routine: QMR
**	Returns: int
**	Action : 
**
** QMR.h solves the unsymmetric linear system Ax = b using the
** Quasi-Minimal Residual method following the algorithm as described
** on p. 24 in the SIAM Templates book.
**
**   -------------------------------------------------------------
**   return value     indicates
**   ------------     ---------------------
**        0           convergence within max_iter iterations
**        1           no convergence after max_iter iterations
**                    breakdown in:
**        2             rho
**        3             beta
**        4             gamma
**        5             delta
**        6             ep
**        7             xi
**   -------------------------------------------------------------
**   
** Upon successful return, output arguments have the following values:
**
**        x  --  approximate solution to Ax=b
** max_iter  --  the number of iterations performed before the
**               tolerance was reached
**      tol  --  the residual after the final iteration
**
******************************************************************/

template < class Matrix, class Vector, class Preconditioner1,
           class Preconditioner2, class Real >
int QMR(
	const Matrix&		   A,
	Vector&				   x,
	const Vector&		   b,
	const Preconditioner1& M1, 
    const Preconditioner2& M2,
	int&				   max_iter,
	Real&				   tol
);

CC_END_NAMESPACE
#endif

