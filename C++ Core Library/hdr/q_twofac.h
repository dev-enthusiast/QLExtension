/****************************************************************
**
**	Q_TWOFAC.H	
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/quant/src/q_twofac.h,v 1.10 2000/03/02 16:08:57 goodfj Exp $
**
****************************************************************/

#if !defined( IN_Q_TWOFAC_H )
#define IN_Q_TWOFAC_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

// Must use typedef to compile on SparcWorks
typedef double (*Q_FUNC_2FAC)( double u, double v );

EXPORT_C_QMRM double Q_MRPosCalibVol ( 
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	double      time,                    // total time to end of calibration
    int         nSteps,                  // number of timesteps
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *termvol,				 // vector of term volatilities
	double      deltaS,                  // spacing of log spot
	int         whichvol                 // which volatility to return
	);

EXPORT_C_QMRM double Q_MRPosCalibMu ( 
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	double      time,                    // total time to end of calibration
    int         nSteps,                  // number of timesteps
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *termvol,				 // vector of term volatilities
	double      deltaS,                  // spacing of log spot
	int         whichvol                 // which volatility to return
	);
	
EXPORT_C_QMRM int Q_CalibrateTwoFac (
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_FUNC_2FAC Func,    				 // arbitrary two variable function
	double      deltat,                  // length of timestep
	int         nSteps,                  // number of timesteps
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *Calls,					 // vector of call values
	Q_VECTOR	*strikes,                // vector of strikes
	double      deltaS,                  // spacing of log spot
	Q_VECTOR    *mu,                     // returned vector of drift terms
	Q_VECTOR    *spotVol                 // returned vector of spot vols
	);

EXPORT_C_QMRM double Q_MRPosCalibVol2 ( 
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_VECTOR    *times,                  // times for which we solve for vol and mu
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *termvol,				 // vector of term volatilities
	double      deltaS,                  // spacing of log spot
	int         whichvol                 // which volatility to return
	);
	
EXPORT_C_QMRM double Q_MRPosCalibMu2 ( 
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_VECTOR    *times,                  // times for which we solve for vol and mu
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *termvol,				 // vector of term volatilities
	double      deltaS,                  // spacing of log spot
	int         whichvol                 // which volatility to return
	);
	
EXPORT_C_QMRM int Q_CalibTwoFacNew (
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_FUNC_2FAC Func,    				 // arbitrary two variable function
	Q_VECTOR    *times,                  // times for which we solve for vol and mu
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *Calls,					 // vector of call values
	Q_VECTOR	*strikes,                // vector of strikes
	double      deltaS,                  // spacing of log spot
	Q_VECTOR    *mu,                     // returned vector of drift terms
	Q_VECTOR    *spotVol                 // returned vector of spot vols
	);
EXPORT_C_QMRM double Q_OptFwdPts2Fac (
	Q_OPT_TYPE  callPut,                 // option type
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      strike,                  // strike on spread between 2 contracts
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_FUNC_2FAC Func,    				 // arbitrary two variable function
	Q_VECTOR    *times,                  // times for which we solve for vol and mu
	int         start,                   // index of expiration date in times
	int         end,                     // index of expiration of futures in times
	Q_VECTOR    *Rd,                     // domestic rate vector
	double      deltaS,                  // spacing of log spot
	Q_VECTOR    *mu,                     // input vector of drift terms
	Q_VECTOR    *spotVol                 // input vector of spot vols
	);
EXPORT_C_QMRM double Q_OptFut2Fac (
	Q_OPT_TYPE  callPut,                 // option type
	Q_OPT_STYLE euroAmer,                // european or american option
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      strike,                  // strike on spread between 2 contracts
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_FUNC_2FAC Func,    				 // arbitrary two variable function
	Q_VECTOR    *times,                  // times for which we solve for vol and mu
	int         start,                   // index of expiration date in times
	int         end,                     // index of expiration of futures in times
	Q_VECTOR    *Rd,                     // domestic rate vector
	double      deltaS,                  // spacing of log spot
	Q_VECTOR    *mu,                     // input vector of drift terms
	Q_VECTOR    *spotVol                 // input vector of spot vols
	);

EXPORT_C_QMRM double Q_MRPosCalibVol3 ( 
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_VECTOR    *times,                  // times for which we solve for vol and mu
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *termvol,				 // vector of term volatilities
	double      deltaS,                  // spacing of log spot
	int         whichvol                 // which volatility to return
	);
	
EXPORT_C_QMRM double Q_MRPosCalibMu3 ( 
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_VECTOR    *times,                  // times for which we solve for vol and mu
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *termvol,				 // vector of term volatilities
	double      deltaS,                  // spacing of log spot
	int         whichvol                 // which volatility to return
	);
	
EXPORT_C_QMRM int Q_CalibTwoFac3 (
	double	    spot,                    // current spot price of commodity
	double      Long,                    // current long price of commodity
	double      longVol,				 // volatility of long price
	double      corr,                    // correlation of short and long
	double      beta,                    // extra parameter, possibly mean reversion
	Q_FUNC_2FAC Func,    				 // arbitrary two variable function
	Q_VECTOR    *times,                  // times for which we solve for vol and mu
	Q_VECTOR    *fwds,                   // forward vector
	Q_VECTOR    *Rd,                     // domestic rate vector
	Q_VECTOR    *Calls,					 // vector of call values
	Q_VECTOR	*strikes,                // vector of strikes
	double      deltaS,                  // spacing of log spot
	Q_VECTOR    *mu,                     // returned vector of drift terms
	Q_VECTOR    *spotVol                 // returned vector of spot vols
	);
#endif

