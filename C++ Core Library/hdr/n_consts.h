/* $Header: /home/cvs/src/num/src/n_consts.h,v 1.9 2012/10/15 12:34:34 e19351 Exp $*/
/****************************************************************
**
**	N_CONSTS.H	
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_N_CONSTS_H )
#define IN_N_CONSTS_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif

#define ONE_OVER_TWO_PI 0.15915494309189533576
#define SQRTPI        	1.77245385090551602729
#define SQRT2PI       	2.50662827463100050242
#define SQRT2PI_INV     0.39894228040143267794
#define SEED      		-2134   		/* seed for random number generator        */
#define NGAUSS  		24		  		/* number of points in Gauss quadrature    */
#define BIGGAUSS  		48		  		/* number of points in big Gauss quadrature    */
#define PI          	3.14159265358979324
#define DENS(x) 		( SQRT2PI_INV * exp(-0.5*(x)*(x)))
#define TINY       		2.22559e-308 
#define BIG_NUMBER   	1.40359222e+217
#define EPSILON 		.0001
#define SMALL_VAL		0.0000001

#define FUDGE_FACTOR (1. + 1000. * 0.000000000000000222096)
#define indicator(x) (((x) > 0.) ? 1. : ((x) < 0.) ? 0. : .5)

EXPORT_C_NUM const double 
		Q_TT[24],
		Q_WW[24],
		Q_TT48[48],
		Q_WW48[48];

#endif

