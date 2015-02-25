/****************************************************************
**
**	q_ld.h	- Header for q_ld.c
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_Q_LD_H )
#define IN_Q_LD_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

			   
EXPORT_C_QUANT double Q_OptImpVolLD(
	double	RdT,				// over-ccy term zero-coupon interest rate
	double	RfT,				// under-ccy term zero-coupon interest rate
	double	Term,				// time from pricing to expiration
	double	StlTerm,			// time from spot date to expiration settlement date
	double	Sigma,				// FX spot process volatility
	double	Epsd,				// over rate standard deviation - not a vol, since it's a normal process
	double	Epsf,				// under rate standard deviation
	double	Betad,				// over rate mean reversion strength
	double	Betaf,				// under rate mean reversion strength
	double	Rhosd,				// correlation between spot and over rate
	double	Rhosf,				// correlation between spot and under rate
	double	Rhodf				// correlation between over rate and under rate
);

EXPORT_C_QUANT double Q_OptLD(
	Q_OPT_TYPE		OptType,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			Spot,			// current FX spot
	double			Strike,			// option strike price
	double			RdT,	   		// over-ccy term zero-coupon interest rate
	double			RfT,	   		// under-ccy term zero-coupon interest rate
	double			Term,	   		// time from pricing to expiration
	double			StlTerm,   		// time from spot date to expiration settlement date
	double			Sigma,	   		// FX spot process volatility
	double			Epsd,	   		// over rate standard deviation - not a vol, since it's a normal process
	double			Epsf,	   		// under rate standard deviation
	double			Betad,	   		// over rate mean reversion strength
	double			Betaf,	   		// under rate mean reversion strength
	double			Rhosd,	    	// correlation between spot and over rate
	double			Rhosf,			// correlation between spot and under rate
	double			Rhodf,			// correlation between over rate and under rate
	Q_RETURN_TYPE	RetType			// Return type (Q_PREMIUM, Q_DELTA, ... )
);


#endif

