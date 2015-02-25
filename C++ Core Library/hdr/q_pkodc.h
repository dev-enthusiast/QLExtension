/* $Header: /home/cvs/src/quant/src/q_pkodc.h,v 1.15 2000/03/03 14:52:06 goodfj Exp $ */
/****************************************************************
**
**	q_pkodc.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.15 $
**
****************************************************************/

#if !defined( IN_Q_PKODC_H )
#define IN_Q_PKODC_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_PKODCFwd (
	Q_OPT_TYPE    	isCall,				// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_IN_OUT     	inOrOut, 			// appearing or disappearing
	double 			spot,	 			// Underlyer spot price
    double      	lowRebate,   		// rebate paid upon lower ko
    double      	highRebate,   		// rebate paid upon higher ko
    Q_REBATE_TYPE	rebType,			// Immediate or delayed till exp date
    Q_VECTOR 		*Vol,				// Underlyer volatility	curve
    Q_VECTOR 		*VolT,      		// grid for volatilities
    double 			time,	 			// Time to expiration
    Q_VECTOR 		*Rd,     			// Domestic rate curve
    Q_VECTOR 		*RdT,       		// grid for domestic rate curve
    Q_VECTOR 		*Fwd,     			// forward curve
    Q_VECTOR 		*FwdT,      		// grid for forward curve
	double			t0,					// Determination Time
	double			t2,					// End of DKO
	double 			strikeMulOffset, 	// strike = Spot(t0)*strikeMulOffset
	double 			strikeAddOffset, 			// strike = Spot(t0)+strikeAddOffset
	double 			lowKnockoutMulOffset, 		// lowKO  = Spot(t0)*lowknockoutMulOffset
	double 			lowKnockoutAddOffset, 		// lowKO  = Spot(t0)+lowknockoutAddOffset
	double 			highKnockoutMulOffset, 		// highKO = Spot(t0)+highknockoutMulOffset
	double 			highKnockoutAddOffset, 		// highKO = Spot(t0)+highknockoutAddOffset
    double	 		AssetDiscretizationFactor, 	// discretization multiplicator in the x direction
    double	 		tDiscretizationFactor, // discretization multiplicator in the t direction
	double			AnchorVol			// Vol used to anchor the grid if the barriers are far apart
);


#endif

