/* $Header: /home/cvs/src/quant/src/q_metop2.h,v 1.8 2000/03/02 16:08:54 goodfj Exp $ */
/****************************************************************
**
**	Q_METOP2.H	- more metals option routines
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_Q_METOP2_H )
#define IN_Q_METOP2_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QMRM double Q_MRFutCovC (
	Q_VECTOR        *futexp,    // times to expirations of futures
	Q_VECTOR        *spotvol,   // spot volatilities between futures dates
	Q_VECTOR        *termvol,   // term volatilities to expirations
	Q_VECTOR        *termcov,   // term cov(spot,long) to expirations
	double          t1,         // expiration of first futures price
	double          t2,         // expiration of second futures price
	double          time,       // term of this covariance
	Q_VECTOR		*beta,		// mean reversion coefficient
	double			LongVol,	// volatility of long-term price
	double			Correlation	// instantaneous correlation of spot and long-term price
	);
#endif

