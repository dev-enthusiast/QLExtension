/* $Header: /home/cvs/src/quant/src/q_newlkb.h,v 1.13 2000/03/23 12:58:20 whited Exp $ */
/****************************************************************
**
**	Q_NEWLKB.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.13 $
**
****************************************************************/

#if !defined( IN_Q_NEWLKB_H )
#define IN_Q_NEWLKB_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QRARE double Q_LBFixedStrkD (
	Q_OPT_TYPE 		callput,	// put or call on extremum
	double      	spot, 		// current security price    
	double			strike,		// fixed price for premium calculation               
	double      	minmax,     // current min/max
	Q_VECTOR		*vol,		// volatility vector
	double          time,       // time to expiration
	Q_VECTOR		*times,		// sampling times
	double			ccRd,  		// domestic interest rate to expiration
	Q_VECTOR		*fwds,		// forward prices at the sampling points
	int         	nPaths,     // number of paths in Monte Carlo
	Q_RETURN_TYPE	retType		// return premium or delta
	);
	
EXPORT_C_QRARE double Q_LBFixedStrk (
	Q_OPT_TYPE 	callput,	    	// put or call
	double		spot,				// current security price    
	double		fixed_strike,		// fixed price for premium calculation               
	double		vol,				// volatility, % per year    
	double		exp_time,			// maturity, in years        
	double		rd,					// carry,      % per year    
	double		rf,					// short rate, % per year    
	double		minmax 				// min/max up 'til now           
	);

EXPORT_C_QRARE double Q_LBRangeQtosCap (
	Q_OPT_TYPE 		callput,    	// put or call
	double      	spot, 			// current security price
	double			strike,			// fixed price for premium calculation               
	double          texp,       	// time to expiration
	Q_VECTOR		*times,			// sampling times
	Q_VECTOR		*volDenOver,	// vector of vols for payout asset per numerator of the LB cross
	Q_VECTOR		*volDenUnder,	// vector of vols for payout asset per denominator of the LB cross
	Q_VECTOR		*volOverUnder,	// vector of vols for the LB cross
	Q_VECTOR		*fwds,			// forward prices at the sampling points for the LB cross
	double      	currentmin, 	// current min, negative number if there is none
	double      	currentmax, 	// current max, negative number if there is none
	double			cap,			// cap on payout amount
	double			ccRd,  			// denominated interest rate to expiration
	long         	nPaths      	// number of paths in Monte Carlo
	);

#endif

