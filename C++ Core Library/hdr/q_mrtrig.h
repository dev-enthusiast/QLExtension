/* $Header: /home/cvs/src/quant/src/q_mrtrig.h,v 1.9 2000/03/02 16:08:56 goodfj Exp $ */
/****************************************************************
**
**	Q_MRTRIG.H	- more metals option routines
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_Q_MRTRIG_H )
#define IN_Q_MRTRIG_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QMRM double Q_MRTrigOptSwapC (
    Q_OPT_TYPE   callPut,    // option type
    Q_UP_DOWN    upOrDown,   // direction of trigger
	double       spot,		 // current cash price
	double       Long,       // current long price
	Q_VECTOR     *Quantity,  // quantity vector for swap
	double       strike,     // strike price on the swap
	double       outVal,     // trigger on a fixed term future
	double       longvol,    // long volatility
	double       corr,       // instantaneous correlation between short and long
	Q_VECTOR     *beta,      // mean reversion vector
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

EXPORT_C_QMRM double Q_MRTrigOptSwapCS (
    Q_OPT_TYPE   callPut,    // option type
    Q_UP_DOWN    upOrDown,   // direction of trigger
	double       spot,		 // current cash price
	double       Long,       // current long price
	Q_VECTOR     *Quantity,  // quantity vector for swap
	double       strike,     // strike price on the swap
	double       outVal,     // trigger on a fixed term future
	double       longvol,    // long volatility
	double       corr,       // instantaneous correlation between short and long
	Q_VECTOR     *beta,      // mean reversion vector
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



EXPORT_C_QMRM double Q_GetFwdAC (
	Q_VECTOR            *fwds,     // curve forward (futures) prices for oil
	Q_VECTOR            *times,    // times to expirations for futures
	double              spot,      // current cash price of oil
	double              Long,      // current very Long-term price of oil
	Q_VECTOR            *spotVol,  // instantaneous vol between futures dates
	double              LongVol,   // volatility of Long futures price
	double              corr,      // instantaneous correlation
	Q_VECTOR            *beta,     // vector of mean reversion coefficients
	double              t,         // start of term for A function
	double              T          // end of term for A function
	);

EXPORT_C_QMRM double Q_IntegrateVarC (
	Q_VECTOR           *spotVol,  // instantaneous vol between futures dates
	double             LongVol,   // volatility of Long futures price
	double             corr,      // instantaneous correlation
	Q_VECTOR           *beta,     // mean reversion coefficient
	Q_VECTOR		   *times,    // times to expirations for futures
	double             time       // time to integrate to
	);

EXPORT_C_QMRM double Q_InterpAC (
	Q_VECTOR            *fwds,     // curve forward (futures) prices for oil
	Q_VECTOR            *times,    // times to expirations for futures
	double              spot,      // current cash price of oil
	double              Long,      // current very Long-term price of oil
	Q_VECTOR            *beta,     // vector of mean reversion coefficients
	Q_VECTOR            *betaT,    // vector of times for betas
	double              time       // time we are interpolating to
	);

EXPORT_C_QMRM double Q_IntegrateB (
	Q_VECTOR         *beta,     // piecewise constant beta curve
	Q_VECTOR         *times,    // nodes of beta curve
	double           t1,        // start of term
	double           t2         // end of term
	);

EXPORT_C_QMRM double Q_MRMTrigOptCN (
    Q_OPT_TYPE   callPut,    	// option type	(swap(fwd), call or put)
    Q_UP_DOWN    upOrDown,      // direction of knockout
	double       spot,		 	// current cash price
    double	 	 strike,    	// one strike price per payment date
    double	 	 outVal,    	// knockout level
    double       tenor,         // pays off against this rolling tenor
    double       trigtenor,     // knocks out against this rolling tenor
	Q_VECTOR     *vols,      	// term volatility vector
    Q_VECTOR 	 *volTimes,  	// volatility times
    double       rangevol,   	// volatility to determine range of integration
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	double       kotime,        // time to beginning of knockout
	double       time,          // time to expiration
	Q_VECTOR     *fwds,      	// forward vector
	Q_VECTOR     *fwdT,      	// forward nodes
 	double	     ccRd,        	// interest rate
	int          nGrid1,     	// number of gridpoints in short dimension
	int          nGrid2,     	// number of gridpoints in long dimension
	int          nSteps,      	// number of timesteps
	double       tolerance      // tolerance parameter for conjugate gradient
	);

EXPORT_C_QMRM double Q_MRMTrigOptMC (
    Q_OPT_TYPE   	callPut,    // option type	(swap(fwd), call or put)
    Q_UP_DOWN    	upOrDown,   // direction of knockout
	double       	spot,		// current cash price
    double	 	 	strike,    	// one strike price per payment date
    double	 	 	outVal,    	// knockout level
    double       	tenor,      // pays off against this rolling tenor
    double       	trigtenor,  // knocks out against this rolling tenor
	Q_VECTOR     	*vols,      // term volatility vector
    Q_VECTOR 	 	*volTimes,  // volatility times
    double       	rangevol,   // volatility to determine range of integration
	double       	beta,       // mean reversion
	double       	longvol,    // long volatility
	double       	corr,       // instantaneous correlation between short and long
	double       	kotime,     // time to beginning of knockout
	double       	time,       // time to expiration
	Q_VECTOR     	*fwds,      // forward vector
	Q_VECTOR     	*fwdT,      // forward nodes
 	double	     	ccRd,       // interest rate
    int          	nSteps,     // number of timesteps
    int             nPaths,     // number of paths in Monte Carlo
    Q_RETURN_TYPE   retType     // return premium or error
	);
		
#endif
