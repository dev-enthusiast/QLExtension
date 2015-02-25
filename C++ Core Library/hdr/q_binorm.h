/* $Header: /home/cvs/src/quant/src/q_binorm.h,v 1.14 2000/03/06 14:33:56 goodfj Exp $ */
/****************************************************************
**
**	Q_BINORM.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.14 $
**
****************************************************************/

#if !defined( IN_Q_BINORM_H )
#define IN_Q_BINORM_H

#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif

typedef struct {
	int        MatElem;
	
	Q_MATRIX   **Mat;
} Q_GRID3D;	

EXPORT_C_QBASIC double
		Q_BivariateNormal( double X, double Y, double RHO );

EXPORT_C_QBASIC double Q_BinDual (
	Q_OPT_TYPE       callPut,	// call means above strike wins
    Q_OPT_TYPE       callPut2,  // FIX
	double           spot,      // current spot
	double           spot2,		// current second spot
	double           strike,    // strike price on first asset
	double           strike2,   // strike price on second asset
	double           vol,       // volatility of first asset
	double           vol2,      // volatility of second asset
	double           corr,      // correlation as log normal variables
	double           time,      // time to expiration
	double           ccRd,      // domestic rate
	double           ccRf,      // first foreign or dividend rate
	double           ccRf2,     // second foreign or dividend rate
	double           payoff		// payoff upon being in the money
	);

EXPORT_C_QBASIC double Q_Interp3DGrid(
    Q_VECTOR    *x1Vector,	   // vector of x values 1st dimension
    Q_VECTOR    *x2Vector,	   // vector of x values 2nd dimension
    Q_VECTOR    *x3Vector,	   // vector of x values 3rd dimension
    Q_GRID3D    *yGrid,	       // 3-dimensional grid of y values
    double	    x1Value,	   // arbitrary x value	1st dimension
    double	    x2Value,	   // arbitrary x value	2nd dimension
    double	    x3Value		   // arbitrary x value	3rd dimension
    );

EXPORT_C_QBASIC double Q_CumulativeNormalHP(double a );

EXPORT_C_QBASIC double Q_BiLogNormalPDF	(
	double 	r,
	double	r0,
	double	sg1,
	double	mu1,
	double 	s,
	double	s0,
	double	sg2,
	double	mu2,
	double  rho
	);

#endif

