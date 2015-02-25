/* $Header: /home/cvs/src/quant/src/q_muldko.h,v 1.17 2000/03/03 14:52:03 goodfj Exp $ */
/****************************************************************
**
**	q_muldko.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.17 $
**
****************************************************************/

#if !defined( IN_Q_MULDKO_H )
#define IN_Q_MULDKO_H

#if !defined( IN_N_CRANK_H )
#include	<n_crank.h>
#endif
#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

int DiscretizationMultiplier(
	int		nBlock,
	CRANK_NICHOLSON_INPUT
			*block,
	double	AssetDiscretizationFactor,
	double	tDiscretizationFactor
);

EXPORT_C_QKNOCK double Q_MultipleDKOc(
	Q_OPT_TYPE    	isCall,	 		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_VECTOR    	*optStyle, 		// Option type (Q_OPT_EURO or Q_OPT_AMER)
	double 			spot,	 		// Underlyer spot price
	double 			strike,	 		// Option strike price
	Q_VECTOR		*endPeriod,		// End of each time period
    Q_VECTOR		*lowOutVal,		// Knock-out price( Strictly negative means no barrier )
    Q_VECTOR		*highOutVal,	// Knock-out price( "	" )
    Q_VECTOR		*lowerRebate, 	// rebate for knocking at the lower level
    Q_VECTOR		*highRebate,	// rebate for knocking at the upper level
    Q_REBATE_TYPE   rebType,	 	// Immediate or delayed till exp date
    Q_VECTOR 		*Vol,			// Underlyer volatility	curve
    Q_VECTOR 		*VolT,      	// grid for volatilities
    Q_VECTOR 		*Rd,     		// Domestic rate curve
    Q_VECTOR 		*RdT,       	// grid for domestic rate curve
    Q_VECTOR 		*Fwd,     		// forward curve
    Q_VECTOR 		*FwdT,	      	// grid for forward curve
    double	 		AssetDiscretizationFactor,// discretization multiplicator in the x direction
    double	 		tDiscretizationFactor, // discretization multiplicator in the t direction
	double			AnchorVol		// Vol used to anchor the grid if the barriers are far apart
);


#endif

