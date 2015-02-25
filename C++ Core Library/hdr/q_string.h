/* $Header: /home/cvs/src/quant/src/q_string.h,v 1.21 2000/03/02 15:01:57 goodfj Exp $ */
/****************************************************************
**
**	Q_STRING.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.21 $
**
****************************************************************/

#if !defined( IN_Q_STRING_H )
#define IN_Q_STRING_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QEXOTIC double Q_StringNew (
	Q_OPT_TYPE        callPut,     // option type
	Q_UP_DOWN         upOrDown,    // direction in which option continues
	double            spot,		   // current spot price
	double            strike,	   // strike price
	double            trigger,     // trigger for continuing option
	Q_VECTOR          *vol,		   // volatility
	Q_VECTOR          *times, 	   // vector of times to exp
	Q_VECTOR          *ccRd,	   // vector of domestic interest rates
	Q_VECTOR          *fwds,	   // vector of forward prices
	int               nGrid        // number of gridpoints for integration
	);

EXPORT_C_QEXOTIC double Q_Rabbit(
    Q_UP_DOWN	ud,   		// up or down 
    Q_OPT_TYPE 	cp,    		// call or put
    double		s,          // spot
    double		k,          // strike
    double		ko,         // Exercise Level
    int			max,        // number of times option can be exercised
    Q_VECTOR *	vol,    	// vol out to expiration dates
    Q_VECTOR * 	volt,    	// times of expiration dates
    Q_VECTOR *	fwd,    	// forwards out to expiration settlement dates
    Q_VECTOR *	ccrd,       // domestic rates vector
    Q_VECTOR * 	fwdt,    	// times of settlement dates
    int			maxIter     // number of monte carlo paths
);
#endif

