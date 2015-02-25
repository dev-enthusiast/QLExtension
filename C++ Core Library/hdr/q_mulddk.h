/****************************************************************
**
**	q_mulddk.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_Q_MULDDK_H )
#define IN_Q_MULDDK_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_MultipleDiscreteDKOc(
	Q_OPT_TYPE    	isCall,	 		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 		// Underlyer spot price
	double 			strike,	 		// Option strike price
	Q_VECTOR		*QV_endPeriod,	// End of each time period
    Q_VECTOR		*QV_lowOutVal,	// Knock-out price( Strictly negative means no barrier )
    Q_VECTOR		*QV_highOutVal,	// Knock-out price( "	" )
    Q_VECTOR		*QV_lowRebate, 	// rebate for knocking at the lower continuous level
    Q_VECTOR		*QV_highRebate,	// rebate for knocking at the upper continuous level
    Q_REBATE_TYPE   rebType,	 	// Immediate or delayed till exp date
	Q_VECTOR		*QV_KOTimes,	// Knock out low level
	Q_VECTOR		*QV_lowDKO,		// Knock out low level
	Q_VECTOR		*QV_highDKO,	// Knock out high level
	Q_VECTOR		*QV_lowDRebate,	// Fade out low rebate
	Q_VECTOR		*QV_highDRebate,// Fade out high rebate
    Q_VECTOR 		*QV_Vol,		// Underlyer volatility	curve
    Q_VECTOR 		*QV_VolT,      	// grid for volatilities
    Q_VECTOR 		*QV_Rd,     	// Domestic rate curve
    Q_VECTOR 		*QV_RdT,       	// grid for domestic rate curve
    Q_VECTOR 		*QV_Fwd,     	// forward curve
    Q_VECTOR 		*QV_FwdT,	    // grid for forward curve
    double	 		AssetDiscretizationFactor, // discretization multiplicator in the x direction
    double	 		tDiscretizationFactor, // discretization multiplicator in the t direction
	double			AnchorVol		// Vol used to anchor the grid if the barriers are far apart
);

#endif

