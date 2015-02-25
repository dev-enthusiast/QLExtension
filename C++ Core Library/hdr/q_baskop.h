/* $Header: /home/cvs/src/quant/src/q_baskop.h,v 1.19 2000/03/02 14:19:24 goodfj Exp $ */
/****************************************************************
**
**	Q_BASKOP.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.19 $
**
****************************************************************/

#if !defined( IN_Q_BASKOP_H )
#define IN_Q_BASKOP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QUANT int Q_InitializeMatrix (Q_MATRIX *mat, int dim );
EXPORT_C_QUANT int Q_MakeCov(Q_MATRIX *x , Q_MATRIX *cov, Q_MATRIX *ncov, Q_OPTION *opt);
EXPORT_C_QUANT int Q_MakeBinomial (Q_PROB *Prob);
EXPORT_C_QUANT int Q_FindMeans   (Q_MATRIX *cov, Q_PROB *Prob, Q_VARS *Vars, int Dim);
EXPORT_C_QUANT int Q_GramSchmidt(Q_MATRIX *Inp, Q_MATRIX *Out);
EXPORT_C_QUANT int	Q_EvalOption (Q_MATRIX *Inv, Q_PROB *Prob , Q_VARS *Vars, Q_OPTION *Option, int N, int Dim);
EXPORT_C_QUANT int	Q_MakeProb(long int count, int dim,Q_VARS *Vars,Q_PROB *Prob, int N, int Dim);

EXPORT_C_QUANT double Q_SprdOpt (
	Q_OPT_TYPE    callPut,		// option type
	double        fwd1,			// long forward
	double        fwd2,			// short forward
	double        strike,		// strike price
	double        vol1,			// volatility of long
	double        vol2,			// volatility of short
	double        corr,			// correlation between long and short
	double        time,			// time to expiration
	double        ccRd			// discount rate
	);
EXPORT_C_QUANT double Q_BsktOpt (
	Q_OPT_TYPE    callPut,		// option type
	Q_VECTOR      *fwds,		// forwards
	Q_VECTOR	  *wts,			// basket weights
	double        strike,		// strike price
	Q_MATRIX      *vols,		// vols and crossvols
	double        time,			// time to expiration
	double        ccRd			// discount rate
	);
	
EXPORT_C_QUANT double Q_MaxWithinMax (  
	Q_OPT_TYPE  callPut1,	// option type of first asset
	Q_OPT_TYPE  callPut2,	// intrinsic value type of second asset
    double      fwd1,   	// forward price of first asset
    double      fwd2,		// forward price of second asset
    double      strike,		// strike price on first asset
    double      strike2,	// strike price on second asset
    double      multiplier, // the number A referred to in the heading
    double      vol,		// volatility of first asset
	double      vol2,		// volatility of second asset
	double      corr,		// correlation
	double      time,		// time to expiration
	double      ccRd		// domestic interest rate
	);

EXPORT_C_QUANT double Q_MinMaxWithinMax ( 
	Q_OPT_TYPE  callPut,   	// option payoff type on first asset
    double      fwd,   		// forward price of first asset
    double      fwd2,		// forward price of second asset
    double      strike,		// strike price on first asset
    double      strike2,	// strike price on second asset
    double      strike3,	// second strike price on second asset
    double      multiplier, // the number M in the formula above
    double      vol,		// volatility of first asset
	double      vol2,		// volatility of second asset
	double      corr,		// correlation
	double      time,		// time to expiration
	double      ccRd	    // domestic interest rate
	);
EXPORT_C_QUANT double Q_SprdKO (
	Q_OPT_TYPE        isCall,		// option type
	Q_UP_DOWN         upOrDown,		// direction of knockout
	double            spot1,	    // spot of first asset
	double            spot2,		// spot of second asset
	Q_VECTOR          *fwds1,		// vector of forwards for first asset
	Q_VECTOR          *fwds2,		// vector of forwards for second asset
	double            strike,		// strike price
	double            outVal,		// knockout level
	Q_VECTOR          *vol1,		// volatility vector for first asset
	Q_VECTOR          *vol2,		// volatility vector for second asset
	double            corr,			// correlation
	Q_VECTOR          *gridpts,		// vector of gridpoints 
	double            time,			// time to expiration
	int               nSteps,		// number of steps per path
	double            ccRd,			// continuous discount rate
	int               nTrials		// number of paths
	);
	  
EXPORT_C_QUANT double Q_Dispersion (
	Q_VECTOR     *fwds,        // vector of forward prices
	double       totalstrike,  // total strike of all the options
	Q_VECTOR     *vol,         // vector of volatilties
	double       time,         // time to expiration
	int          ordinal       // which strike?
	);

EXPORT_C_QUANT double Q_BsktLongVol (
	Q_VECTOR      *fwds,		// forwards
	Q_VECTOR	  *wts,			// basket weights
	Q_MATRIX      *vols,		// vols and crossvols
	double        time			// time to expiration
	);
	  
EXPORT_C_QUANT double Q_BsktShortVol (
	Q_VECTOR      *fwds,		// forwards
	Q_VECTOR	  *wts,			// basket weights
	Q_MATRIX      *vols,		// vols and crossvols
	double        time			// time to expiration
	);
	  
EXPORT_C_QUANT double Q_BsktCorr (
	Q_VECTOR      *fwds,		// forwards
	Q_VECTOR	  *wts,			// basket weights
	Q_MATRIX      *vols,		// vols and crossvols
	double        time			// time to expiration
	);

EXPORT_C_QUANT double Q_BsktLongFwd (
	Q_VECTOR      *fwds,		// forwards
	Q_VECTOR	  *wts			// basket weights
	);
	  
EXPORT_C_QUANT double Q_BsktShortFwd (
	Q_VECTOR      *fwds,		// forwards
	Q_VECTOR	  *wts			// basket weights
	);

EXPORT_C_QUANT double Q_TimeBasket (
	Q_OPT_TYPE       callPut,     // option type
	Q_VECTOR         *Fwds,       // forward price vector
	Q_VECTOR         *wts,        // weight vector
	double           strike,      // strike price on spread
	Q_VECTOR         *vols,       // volatility vector
	Q_VECTOR         *times,      // times for weighted cashflows
	double           time,        // time to expiration
	double           ccRd         // domestic interest rate
	);
	  
EXPORT_C_QUANT double Q_TimeBasket3Way (
	Q_OPT_TYPE       callPut,     // option type
	Q_VECTOR         *fwds1,      // forward price vector 1
	Q_VECTOR         *fwds2,      // forward price vector 2
	Q_VECTOR         *fwds3,      // forward price vector 3
	Q_VECTOR         *wts1,       // weight vector 1
	Q_VECTOR         *wts2,       // weight vector 2
	Q_VECTOR         *wts3,       // weight vector 3
	double           strike,      // strike price on spread
	Q_VECTOR         *vols1,      // volatility vector
	Q_VECTOR         *vols2,      // volatility vector
	Q_VECTOR         *vols3,      // volatility vector
	double           rho12,       // correlation of 1 and 2
	double           rho13,       // correlation of 1 and 3
	double           rho23,       // correlation of 2 and 3
	Q_VECTOR         *times,      // times for weighted cashflows
	double           time,        // time to expiration
	double           ccRd         // domestic interest rate
	);

EXPORT_C_QUANT int Q_TimeBasketParms (
	Q_VECTOR         *fwds,       // forward price vector
	Q_VECTOR         *wts,        // weight vector
	Q_VECTOR         *vols,       // volatility vector
	Q_VECTOR         *times,      // times for weighted cashflows
	double           time,        // time to expiration
	double           *longfwd,    // returned long side total forward
	double           *shortfwd,   // returned short side total forward
	double           *longvol,    // returned long side volatility
	double           *shortvol,   // returned short side volatility
	double           *corr        // returned short-long correlation
	);
EXPORT_C_QUANT int Q_DispersionV (
	Q_VECTOR     *fwds,        // vector of forward prices
	double       totalstrike,  // total strike of all the options
	Q_VECTOR     *vol,         // vector of volatilties
	double       time,         // time to expiration
	Q_VECTOR     *strike	   // output vector of strikes
	);

EXPORT_C_QUANT double Q_SpreadKnockout (
	Q_OPT_TYPE        callPut,		// option type
	Q_UP_DOWN         upOrDown,		// direction of knockout
	double            spot1,	    // spot of first asset
	double            spot2,		// spot of second asset
	Q_VECTOR          *fwds1,		// vector of forwards for first asset
	Q_VECTOR          *fwds2,		// vector of forwards for second asset
	double            strike,		// strike price
	double            outVal,		// knockout level
	Q_VECTOR          *vol1,		// volatility vector for first asset
	Q_VECTOR          *vol2,		// volatility vector for second asset
	double            corr,			// correlation
	double            beta1,		// mean reversion for first asset
	double            beta2,		// mean reversion for second asset
	Q_VECTOR          *gridpts,		// vector of gridpoints
	double            time,			// time to expiration
	int               nSteps,		// number of steps per path
	double            ccRd,			// continuous discount rate
	int               nTrials		// number of paths
	);
#endif
