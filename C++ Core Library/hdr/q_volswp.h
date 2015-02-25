/* $Header: /home/cvs/src/quant/src/q_volswp.h,v 1.8 2000/03/02 15:01:58 goodfj Exp $ */
/****************************************************************
**
**	Q_VOLSWP.H
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_Q_VOLSWP_H )
#define IN_Q_VOLSWP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QEXOTIC double Q_OptVolSwap (
	Q_OPT_TYPE     callPut,     // option type (can also be forward)
	double         spot1,       // first spot price
	double         spot2,       // second spot price
	Q_VECTOR       *hist1,     	// historical first prices
	Q_VECTOR       *hist2,      // historical second prices
	double         strike,      // fixed side of the swap
	Q_VECTOR       *vol1,       // volatility vector of first price
	Q_VECTOR       *vol2,       // volatility vector of second price
	Q_VECTOR       *xvol,       // cross volatility vector
	Q_VECTOR       *volT1,      // volatility vector of first price
	Q_VECTOR       *volT2,      // volatility vector of second price
	Q_VECTOR       *xvolT,      // cross volatility vector
	double         time,        // time to expiration
	double         stub,        // time to next fix
	int            nfix,        // number of fixes to expiration
	double         ccRd,        // domestic interest rate
	Q_VECTOR       *fwd1,       // first forward price vector
	Q_VECTOR       *fwd2,       // second forward price vector
	Q_VECTOR       *fwdT1,      // first forward time vector
	Q_VECTOR       *fwdT2,      // second forward time vector
	int            nPaths       // number of paths in monte-carlo run
	);
#endif

