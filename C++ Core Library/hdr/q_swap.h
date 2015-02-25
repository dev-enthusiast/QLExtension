/* $Header: /home/cvs/src/quant/src/q_swap.h,v 1.10 2001/10/25 00:49:00 demeor Exp $ */
/****************************************************************
**
**	Q_SWAP.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_Q_SWAP_H )
#define IN_Q_SWAP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QBASIC double Q_SwapDiff (
	Q_VECTOR       *ccRd,         // vector of domestic (US) rates
	Q_VECTOR       *ccRf,         // vector of foreign (DM) rates
	double         fixed,         // fixed rate
	Q_VECTOR       *xVol,         // vector of exchange rate volatilities
	Q_VECTOR       *lVol,         // vector of DM Libor rate volatilities
	double         corr,          // correlation 
	DATE           today,         // today's date
	DATE           settle,        // settlement date
	DATE           firstDate,     // first payment date
	int            nPayments,     // number of payments
	int            period         // number of months between payments
	);

EXPORT_C_QBASIC double Q_SwapPv   (
	Q_VECTOR       *ccRd,         // vector of domestic (US) rates
	double         fixed,         // fixed rate
	DATE           today,         // today's date
	DATE           settle,        // settlement date
	DATE           firstDate,     // first payment date
	int            nPayments,     // number of payments
	int            period         // number of months between payments
	);
	
EXPORT_C_QBASIC double Q_GetZero (
	DATE       today,	  // today's date
	DATE       theDate,	  // arbitrary date in the future
	Q_VECTOR   *Grid,	  // times for discount curve
	Q_VECTOR   *Discount  // curve of discount factors
	);

EXPORT_C_QBASIC double Q_GetZeroC(
	DATE       today,	  // today's date
	DATE       theDate,	  // arbitrary date in the future
	DT_CURVE   *Discount  // vector of discount factors
	);

EXPORT_C_QBASIC double Q_GetZeroT (    
	double     term,	  // time from the present
	Q_VECTOR   *Grid,	  // times for discount curve
	Q_VECTOR   *Discount  // curve of discount factors
	);

EXPORT_C_QBASIC double Q_SwapZeroCurve ( 
	Q_VECTOR       *simpleRates,   // vector of simple rates
	Q_VECTOR       *simpleTimes,   // vector of gridpoints for simple rates
	Q_VECTOR       *swapRates,	   // vector of swap rates
	Q_VECTOR       *swapTimes,	   // vector of gridpoints for swap rates
	double         Period,         // length of coupon period for swap rates
	Q_DAY_COUNT    method,         // daycount method for simple rates
	double         time            // arbitrary time from present
	);
	  
EXPORT_C_QBASIC double Q_SwapZeroCurve2 ( 
	Q_VECTOR       *simpleRates,   // vector of simple forward-forward rates
	Q_VECTOR       *simpleTimes,   // vector of gridpoints for simple rates
	Q_VECTOR       *swapRates,	   // vector of swap rates
	Q_VECTOR       *swapTimes,	   // vector of gridpoints for swap rates
	double         Period,         // length of coupon period for swap rates
	Q_DAY_COUNT    method,         // daycount method for simple rates
	double         time            // arbitrary time from present
	);

EXPORT_C_QBASIC double Q_FwdSwap   (
	Q_VECTOR	*ZeroCurve,	  // vector of discount factors
	int			Start,        // Start-o-Range
	int			Finish        // End-O-Range
	);

EXPORT_C_QBASIC int Q_SwapZeroCurveV (
	Q_VECTOR       *simpleRates,   // vector of simple rates
	Q_VECTOR       *simpleTimes,   // vector of gridpoints for simple rates
	Q_VECTOR       *swapRates,	   // vector of swap rates
	Q_VECTOR       *swapTimes,	   // vector of gridpoints for swap rates
	Q_VECTOR       *Discount,	   // output vector of discount factors
	Q_VECTOR       *Times,		   // output vector of gridpoints
	double         Period,         // length of coupon period for swap rates
	Q_DAY_COUNT    dayCount		   // daycount method for simple rates
	);

EXPORT_C_QBASIC int Q_SwapZeroCurveV2 (
	Q_VECTOR       *simpleRates,   // vector of simple rates
	Q_VECTOR       *simpleTimes,   // vector of gridpoints for simple rates
	Q_VECTOR       *swapRates,	   // vector of swap rates
	Q_VECTOR       *swapTimes,	   // vector of gridpoints for swap rates
	Q_VECTOR       *Discount,	   // output vector of discount factors
	Q_VECTOR       *Times,		   // output vector of gridpoints
	double         Period,         // length of coupon period for swap rates
	Q_DAY_COUNT    dayCount		   // daycount method for simple rates
	);

EXPORT_C_QBASIC double Q_QuantosDiscAdj(
    Q_VECTOR	*covs,      // piecewise instantaneous covariances of foreign rate, spot
    Q_VECTOR	*times,     // times for covariances
    double		alpha,      // mean reversion of foreign rate
    double		time        // expiration of term
    );

#endif

