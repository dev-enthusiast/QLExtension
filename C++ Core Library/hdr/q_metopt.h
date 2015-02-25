/* $Header: /home/cvs/src/quant/src/q_metopt.h,v 1.11 2000/03/02 16:08:55 goodfj Exp $ */
/****************************************************************
**
**	Q_METOPT.H	- routines to handle option routines for base metals,
**				  which require a two factor mean-reversion model,
**				  but don't work the same way as oil.
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.11 $
**
****************************************************************/

#if !defined( IN_Q_METOPT_H )
#define IN_Q_METOPT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QMRM double Q_MRTrigOptSwap (
    Q_OPT_TYPE   callPut,    // option type
    Q_UP_DOWN    upOrDown,   // direction of trigger
	double       spot,		 // current cash price
	double       Long,       // current long price
	Q_VECTOR     *Quantity,  // quantity vector for swap
	double       strike,     // strike price on the swap
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
	int          nGrid1,     // number of gridpoints in short dimension
	int          nGrid2,     // number of gridpoints in long dimension
	int          nSteps      // number of timesteps
	);

EXPORT_C_QMRM double Q_MRTrigOptSwapS (
    Q_OPT_TYPE   callPut,    // option type
    Q_UP_DOWN    upOrDown,   // direction of trigger
	double       spot,		 // current cash price
	double       Long,       // current long price
	Q_VECTOR     *Quantity,  // quantity vector for swap
	double       strike,     // strike price on the swap
	double       outVal,     // trigger on a fixed term future
	double       longvol,    // long volatility
	double       corr,       // instantaneous correlation between short and long
	double       beta,       // mean reversion
	Q_VECTOR     *vols,      // piecewise constant spot volatilities
	Q_VECTOR     *fwdT,      // nodes of the curves
	double       time,       // time to expiration of option
	Q_VECTOR     *times,	 // terms of the legs of the swap
	double       term,       // term of the future that option triggers against
	Q_VECTOR     *fwds,      // forward curve
	double       ccRd,       // interest rate out to expiration
	int          nGrid1,     // number of gridpoints in short dimension
	int          nGrid2,     // number of gridpoints in long dimension
	int          nSteps      // number of timesteps
	);

#endif

