/* $Header: /home/cvs/src/quant/src/q_wrap.h,v 1.1 2000/02/25 17:40:47 goodfj Exp $ */
/****************************************************************
**
**	Q_WRAP.H	Wrappers for fns from other dlls. These exist only
**				to be called from wrapper projects, such as
**				squant, tsquant, matlab and exquant. They must 
**				NOT be used in other projects.
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_Q_WRAP_H )
#define IN_Q_WRAP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QWRAP double Q_CumNorm( 
	double x 			  // arbitrary real number
	);

EXPORT_C_QWRAP double Q_CumNormInv( 
	double x 			  // 0.0 <= x <= 1.0
	);

EXPORT_C_QWRAP double Q_VectorInterp(
	Q_VECTOR	*xVector,	// vector of x values
	Q_VECTOR	*yVector,	// vector of y values
	double		xValue		// arbitrary x value
	);

EXPORT_C_QWRAP double Q_CubeInterp( 	
    Q_VECTOR    *xVector,	   // vector of x values
    Q_VECTOR    *yVector,	   // vector of y values
    double	    x 			   // arbitrary x value
    );

EXPORT_C_QWRAP double Q_GeoInterp(
    Q_VECTOR    *xVector,	   // vector of x values
    Q_VECTOR    *yVector,	   // vector of y values
    double	    xValue		   // arbitrary x value
    );
#endif

