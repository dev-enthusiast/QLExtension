/* $Header: /home/cvs/src/quant/src/q_volopt.h,v 1.21 2000/07/25 12:15:28 demeor Exp $ */
/****************************************************************
**
**	Q_VOLOPT.H
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.21 $
**
****************************************************************/

#if !defined( IN_Q_VOLOPT_H )
#define IN_Q_VOLOPT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT int Q_CompoundOption2 (
	Q_OPT_TYPE   callPut1,         // option type of compound option
	Q_OPT_TYPE   callPut2,         // option type of second option
	Q_OPT_STYLE  amerEuro,         // style of second option
	double       spot,             // current spot price
	double       strike1,          // strike of compound option
	double       strike2,          // strike of second option
	Q_VECTOR     *vol,             // current volatility
	double       volvol,           // volatility of volatility
	double       corr,             // correlation of volatility and spot
	double       time1,            // time to exp of compound option
	double       time2,            // time to exp of second option
	Q_VECTOR     *times,		   // vector of gridpoints for volatility
	double       ccRd1,            // domestic rate to time1
	double       ccRd2,            // domestic rate to time2
	double       ccRf1,            // foreign rate to time1,
	double       ccRf2,            // foreign rate to time2,
    int          nSteps,           // number of timesteps to time1
    int          nPaths,           // number of paths
    double       *premium,         // returned premium
    double       *delta            // returned delta
   	);


EXPORT_C_QUANT double Q_CmpdG (
	Q_OPT_TYPE   callPut1,         // option type of compound option
	Q_OPT_TYPE   callPut2,         // option type of second option
	Q_OPT_STYLE  amerEuro,         // style of second option
	double       spot,             // current spot price
	double       strike1,          // strike of compound option
	double       strike2,          // strike of second option
	double       vol1,             // volatility to time1
	double       vol2,  		   // volatility to time2
	double       time1,            // time to exp of compound option
	double       time2,            // time to exp of second option
	double       ccRd1,            // domestic rate to time1
	double       ccRd2,            // domestic rate to time2
	double       ccRf1,            // foreign rate to time1,
	double       ccRf2             // foreign rate to time2,
   	);

EXPORT_C_QUANT double Q_CmpdSVMC (
	Q_OPT_TYPE   	callPut1,         // option type of compound option
	Q_OPT_TYPE   	callPut2,         // option type of second option
	Q_OPT_STYLE  	amerEuro,         // style of second option
	double       	spot,             // current spot price
	double       	strike1,          // strike of compound option
	double       	strike2,          // strike of second option
	Q_VECTOR     	*vol,             // current volatility
	double       	volvol,           // volatility of volatility
	double       	corr,             // correlation of volatility and spot
	double       	time1,            // time to exp of compound option
	double       	time2,            // time to exp of second option
	Q_VECTOR     	*times,		   	  // vector of gridpoints for volatility
	double       	Rd1,              // domestic rate to time1
	double       	Rd2,              // domestic rate to time2
	double       	Rf1,              // foreign rate to time1,
	double       	Rf2,              // foreign rate to time2,
    int          	nSteps,           // number of timesteps to time1
    int          	nPaths,           // number of paths
    Q_RETURN_TYPE	retType           // return premium or delta
   	);

EXPORT_C_QUANT double Q_GetVolT (
	double     term,				 // time period from today
	Q_VECTOR   *Grid,				 // times for term volatilities
	Q_VECTOR   *VolCurve			 // term volatilities
	);

EXPORT_C_QUANT double Q_GetVolC(
	DATE		today,		// today's date
	DATE		theDate,	// volatility out to this date
	DT_CURVE	*VolCurve	// volatility curve
	);

EXPORT_C_QUANT double Q_GetVolTermB (
	double     term,				 // time period from today
	double     beta,                 // mean reversion coefficient
	Q_VECTOR   *Grid,				 // times for term volatilities
	Q_VECTOR   *VolCurve			 // term volatilities
	);
EXPORT_C_QUANT double Q_GetVolW (
    DATE       today,		   // today's date
    DATE       theDate,	       // date for interpolated volatility
	Q_VECTOR   *Grid,		   // gridpoints for vol curve
	Q_VECTOR   *VolCurve,	   // volatility curve
	double     lambda		   // number of days a weekend is worth
	);

EXPORT_C_QUANT double Q_InterpVolD(
    DATE       today,		   // today's date
    DATE       theDate,	       // date for interpolated volatility
	Q_VECTOR   *VolDates,      // array of dates for volatility curve
	Q_VECTOR   *VolCurve,      // volatility curve
	Q_VECTOR   *weights        // number of days each day is worth, sum = 7
	);

EXPORT_C_QUANT double Q_GetVolG(
    DATE       today,		   // today's date
    DATE       theDate,	       // date for interpolated volatility
	DATE       *VolDates,      // array of dates for volatility curve
	Q_VECTOR   *VolCurve,      // volatility curve
	Q_VECTOR   *weights        // number of days each day is worth, sum = 7
	);

EXPORT_C_QUANT double Q_VolOptFwdKappa (
	Q_OPT_TYPE      callPut,         // compound option type
	double          fixedsum,        // fixed amount of overlying currency
	double          strike,          // strike price of compound option
	double          pctfwd,          // strike of second option as pct of fwd
    double          fwdvol,          // forward-forward volatility
    double          volvol,          // volatility of volatility
    double          discvol,         // volatility of forward discount rate
    double          corr,            // correlation between disc and vol
    double          t1,              // time to exp of compound option
    double          t2,              // time to exp of currency option
    double          fwdrate,         // forward-forward discount rate
    double          ccRd             // discount rate to t1
    );

EXPORT_C_QUANT double Q_VolOptFwdRho (
	Q_OPT_TYPE      callPut,         // compound option type
	double          fixedsum,        // fixed amount of overlying currency
	double          strike,          // strike price of compound option
	double          pctfwd,          // strike of second option as pct of fwd
    double          fwdvol,          // forward-forward volatility
    double          volvol,          // volatility of volatility
    double          discvol,         // volatility of forward discount rate
    double          corr,            // correlation between disc and vol
    double          t1,              // time to exp of compound option
    double          t2,              // time to exp of currency option
    double          fwdrate,         // forward-forward discount rate
    double          ccRd             // discount rate to t1
    );

EXPORT_C_QUANT double Q_InstOptPV (
	Q_OPT_TYPE     callPut,    // compound option type
	double         spot,       // current spot price
	double         strike,     // strike on currency option
	Q_VECTOR       *install,   // vector of installment payments
	double         vol,        // volatility
	Q_VECTOR       *times,     // installment payment times
	double         time,       // time to expiration
	double         ccRd,       // domestic interest rate
	double         ccRf,       // foreign interest rate
	int            N           // total number of timesteps
	);

EXPORT_C_QUANT double Q_InstOptPVCap (
	Q_OPT_TYPE     callPut,    // option type (Q_OPT_CALL, Q_OPT_PUT, Q_OPT_FWD)
	double         spot,       // current spot price
	double         LowStrike,  // Cap for Put, or Floor for Call and Fwd
	double         strike,     // strike on currency option
	double         HighStrike, // Cap for Call and Fwd, or Floor for Put
	Q_VECTOR       *install,   // vector of installment payments
	double         vol,        // volatility
	Q_VECTOR       *times,     // installment payment times
	double         time,       // time to expiration
	double         ccRd,       // domestic interest rate
	double         ccRf,       // foreign interest rate
	int            N           // total number of timesteps
	);

EXPORT_C_QUANT double Q_VolForward (
	Q_VECTOR       *vol,      // volatility curve
	double         strike,    // strike on volatility
	double         time,      // time to expiration of contract
	double         settle,    // time to settlement
	double         endfwd,    // total time to end of forward vol
	Q_VECTOR       *times,    // times for volatiity curve
	double         ccRd       // discount rate
	);

EXPORT_C_QUANT double Q_VolOptFwd (
	Q_OPT_TYPE      callPut,         // compound option type
	double          fixedsum,        // fixed amount of overlying currency
	double          strike,          // strike price of compound option
	double          pctfwd,          // strike of second option as pct of fwd
	double          fwdvol,          // forward-forward volatility
	double          volvol,          // volatility of volatility
	double          discvol,         // volatility of forward discount rate
	double          corr,            // correlation between disc and vol
	double          t1,              // time to exp of compound option
	double          t2,              // time to exp of currency option
	double          fwdrate,         // forward-forward discount rate
	double          ccRd             // discount rate to t1
	);

EXPORT_C_QUANT double Q_VolOption2 (
	Q_OPT_TYPE      callPut,         // compound option type
	double          fixedsum,        // fixed amount of overlying currency
	double          strike,          // strike price of compound option
	double          fwdvol,          // forward-forward volatility
	double          volvol,          // volatility of volatility
	double          t1,              // time to exp of compound option
	double          t2,              // time to exp of currency option
	double          fwdrate,         // forward-forward discount rate
	double          fwddiv,          // forward-forward foreign rate
	double          ccRd             // discount rate to t1
	);

EXPORT_C_QUANT double Q_VolOptImpVol (
	double          fixedsum,        // fixed amount of overlying currency
	double          strike,          // strike price of compound option
	double          pct,             // strike of second option as pct of spot
	double          fwdvol,          // forward-forward volatility
	double          t2,              // time to exp of currency option
	double          fwdrate,         // forward-forward discount rate
	double          fwddiv           // forward-forward foreign rate
	);

EXPORT_C_QUANT double Q_GetVol (
	DATE       today,		   // today's date
	DATE       theDate,		   // reference date
	Q_VECTOR   *Grid,		   // vector of gridpoints (times)
	Q_VECTOR   *VolCurve	   // vector of volatilities
	);

#endif
