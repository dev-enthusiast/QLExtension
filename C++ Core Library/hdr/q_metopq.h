/* $Header: /home/cvs/src/quant/src/q_metopq.h,v 1.5 2000/03/02 16:08:54 goodfj Exp $ */
/****************************************************************
**
**	Q_METOPQ.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_Q_METOPQ_H )
#define IN_Q_METOPQ_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QMRM double Q_MRTrigOptSwapQ (
    Q_OPT_TYPE   callPut,    // option type
    Q_UP_DOWN    upOrDown,   // direction of trigger
	double       spot,		 // current cash price
	Q_VECTOR     *Quantity,  // quantity vector for swap
	double       strike,     // strike price
	double       outVal,     // trigger on a fixed term future
	double       longvol,    // long volatility
	double       corr,       // instantaneous correlation between short and long
	double       beta,       // mean reversion
	Q_VECTOR     *vols,      // term volatility vector
	Q_VECTOR     *fwdT,      // nodes of the curves
	double       time,       // time to expiration of option
	Q_VECTOR     *times,	 // terms of the legs of the swap
	double       term,       // term of the future that option triggers against
	Q_VECTOR     *fwds,      // forward curve
	double       ccRd,       // interest rate out to expiration
	int          nGrid,      // number of gridpoints in short dimension
	int          nSteps,     // number of coarse timesteps
	int          fineSteps   // each step is divide into this many smaller steps
	);

EXPORT_C_QMRM double Q_MRTrigOptSwapQC (
    Q_OPT_TYPE   callPut,    // option type
    Q_UP_DOWN    upOrDown,   // direction of trigger
	double       spot,		 // current cash price
	Q_VECTOR     *Quantity,  // quantity vector for swap
	double       strike,     // strike price
	double       outVal,     // trigger on a fixed term future
	double       longvol,    // long volatility
	double       corr,       // instantaneous correlation between short and long
	Q_VECTOR     *beta,      // piecewise constant mean reversion
	Q_VECTOR     *vols,      // term volatility vector
	Q_VECTOR     *fwdT,      // nodes of the curves
	double       time,       // time to expiration of option
	Q_VECTOR     *times,	 // terms of the legs of the swap
	double       term,       // term of the future that option triggers against
	Q_VECTOR     *fwds,      // forward curve
	double       ccRd,       // interest rate out to expiration
	int          nGrid,      // number of gridpoints in short dimension
	int          nSteps,     // number of coarse timesteps
	int          fineSteps   // each step is divide into this many smaller steps
	);
	
EXPORT_C_QMRM double Q_MRTrigOptSwapQCS (
    Q_OPT_TYPE   callPut,    // option type
    Q_UP_DOWN    upOrDown,   // direction of trigger
	double       spot,		 // current cash price
	Q_VECTOR     *Quantity,  // quantity vector for swap
	double       strike,     // strike price
	double       outVal,     // trigger on a fixed term future
	double       longvol,    // long volatility
	double       corr,       // instantaneous correlation between short and long
	Q_VECTOR     *beta,      // piecewise constant mean reversion
	Q_VECTOR     *vols,      // term volatility vector
	Q_VECTOR     *fwdT,      // nodes of the curves
	double       time,       // time to expiration of option
	Q_VECTOR     *times,	 // terms of the legs of the swap
	double       term,       // term of the future that option triggers against
	Q_VECTOR     *fwds,      // forward curve
	double       ccRd,       // interest rate out to expiration
	int          nGrid,      // number of gridpoints in short dimension
	int          nSteps,     // number of coarse timesteps
	int          fineSteps   // each step is divide into this many smaller steps
	);
	
#endif

