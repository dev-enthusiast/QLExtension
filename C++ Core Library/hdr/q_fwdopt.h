/* $Header: /home/cvs/src/quant/src/q_fwdopt.h,v 1.8 2000/03/02 13:32:53 goodfj Exp $ */
/****************************************************************
**
**	Q_FWDOPT.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_Q_FWDOPT_H )
#define IN_Q_FWDOPT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QRARE int Q_OptFwdPts (
	Q_OPT_TYPE      callPut,       // option type
	double   		spot,          // current spot
	double   		strike,        // strike on forward points
	double   		vol,           // spot volatility
	double   		dvol,          // term domestic rate volatility
	double   		fvol,          // term foreign rate volatility
	double   		dcorr,         // correlation of spot and domestic rate
	double   		fcorr,         // correlation of spot and foreign rate
	double   		dfcorr,        // correlation of foreign and domestic rates
	double   		timeToExp,     // time to expiration of option
	double   		term,          // term of forward points in option
	Q_VECTOR 		*times,        // vector of gridpoints
	Q_VECTOR 		*ccRd,         // vector of domestic discount rates
	Q_VECTOR 		*fwdpts,       // vector of forward points
	double   		*premRet,      // returned option premium
	double   		*delta,        // returned spot delta
	double   		*ddelta,       // returned domestic rate sensitivity
	double   		*fdelta        // returned foreign rate sensitivity
	);

#endif
