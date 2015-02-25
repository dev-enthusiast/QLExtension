/* $Header: /home/cvs/src/num/src/n_lu.h,v 1.8 1998/12/14 17:50:16 chun Exp $ */
/****************************************************************
**
**	n_lu.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_N_LU_H )
#define IN_N_LU_H

int lu_find_u(
	double	*a, 	// a[0..N-1] diagonal 		input
	double	*b, 	// b[0..N-2] lower diagonal input
	double 	*c, 	// c[0..N-2] upper diagonal input
	double	*u, 	// u[0..N-1] diagonal 		output
	int 	N		// Size						input
);

void lu_solver(
	double	*a, 	// as in lu_find_u.
	double	*b, 	// as in lu_find_u.
	double	*c, 	// as in lu_find_u.
	double	*u, 	// u[0..N-1] obtained from lu_find_u.
	double	*x, 	// x[0..N-1] Solution.
	double	*q, 	// q[0..N-1] RHS.
	int 	N,		// Size.
	double 	*workSpace // work space of size N.
);

void lu_solver_american(
	double	*a, 	// as in lu_find_u.
	double	*b, 	// as in lu_find_u.
	double	*c, 	// as in lu_find_u.
	double	*u, 	// u[0..N-1] obtained from lu_find_u.
	double	*x, 	// x[0..N-1] Solution.
	double	*q, 	// q[0..N-1] RHS.
	double	*ex, 	// ex[0..N-1] RHS.
	int 	N,		// Size.
	double 	*workSpace // work space of size N.
);

int lu_find_u_fast(
	double	a,
	double	b,
	double 	c,
	double	*u,
	int 	N
);

void lu_solver_fast(
	double	a,
	double	b,
	double	c,
	double	*u,
	double	*x,
	double	*q,
	int 	N, 
	double 	*workSpace
);

void lu_solver_american_fast(
	double	a,
	double	b,
	double	c,
	double	*u,
	double	*x,
	double	*q,
	double	*ex,
	int 	N,	
	double 	*workSpace
);


#endif

