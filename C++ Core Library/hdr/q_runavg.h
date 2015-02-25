/* $Header: /home/cvs/src/quant/src/q_runavg.h,v 1.14 2000/03/23 15:39:03 goodfj Exp $ */
/****************************************************************
**
**	Q_RUNAVG.H  - routines to handle knockouts based on running 
**				  average
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.14 $
**
****************************************************************/

#if !defined( IN_Q_RUNAVG_H )
#define IN_Q_RUNAVG_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_KOOnRunAvg3 (
	Q_OPT_TYPE       callPut,           // option type
	Q_UP_DOWN        upOrDown,          // direction of knockout
	int              contflag,          // 1 if continuous ko, 0 otherwise
	double           spot,              // current spot price
	double           strike,            // strike price on average
	double           average,           // to-date average
	int              npoints,           // number of points already averaged
	double           outVal,            // knockout level on spot	
	Q_VECTOR         *vols,             // volatility vector for averaging times
	double           lastvol,           // volatility to expiration
	double           time,              // time to expiration
	Q_VECTOR         *times,            // averaging times
	double           ccRd,              // domestic rate 
	Q_VECTOR         *fwds,             // forward price vector
	double           lastfwd,           // forward price at expiration
	int              nPaths,            // number of trials in monte carlo
	Q_RETURN_TYPE    retType            // return premium or error
	);
#endif

