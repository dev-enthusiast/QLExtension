/* $Header: /home/cvs/src/quant/src/q_corrko.h,v 1.34 2001/09/21 19:00:07 demeor Exp $ */
/****************************************************************
**
**	Q_CORRKO.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.34 $
**
****************************************************************/

#if !defined( IN_Q_CORRKO_H )
#define IN_Q_CORRKO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QKNOCK double Q_FadeOutDual (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	Q_FADEOUT_TYPE  resFlag,    // 0 if resurrecting, 1 if not
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
	Q_DATE_VECTOR   *fadedates, // fadeout dates    
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nGrid1,     // number of intervals in first dimension
    int             nGrid2		// number of intervals in second dimension
    );

EXPORT_C_QKNOCK double Q_FadeOutDualMC (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	Q_FADEOUT_TYPE  resFlag,    // 0 if resurrecting, 1 if not
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
	Q_DATE_VECTOR   *fadedates, // fadeout dates    
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nPaths,		// number of scenarios for Monte Carlo test
    Q_RETURN_TYPE   retType     // return premium or error
    );

EXPORT_C_QKNOCK double Q_OptDualWindow (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	double          lRebate1, 	// lower rebate for first asset
	double          lRebate2, 	// lower rebate for second asset
	double          uRebate1, 	// upper rebate for first asset
	double          uRebate2, 	// upper rebate for second asset
	Q_REBATE_TYPE   rebType,    // immediate or delayed rebate
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            startKO,    // start of knockout period
	DATE            endKO,      // end of knockout period
	DATE            expDate,    // expiration date
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nGrid1,     // number of intervals in first dimension
    int             nGrid2		// number of intervals in second dimension
    );
                                     
EXPORT_C_QKNOCK double Q_OptDualWindowMC (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	double          lRebate1, 	// lower rebate for first asset
	double          lRebate2, 	// lower rebate for second asset
	double          uRebate1, 	// upper rebate for first asset
	double          uRebate2, 	// upper rebate for second asset
	Q_REBATE_TYPE   rebType,    // immediate or delayed rebate
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            startKO,    // start of knockout period
	DATE            endKO,      // end of knockout period
	DATE            expDate,    // expiration date
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nSteps,     // number of timesteps
    int             nPaths,		// number of scenarios for Monte Carlo test
    Q_RETURN_TYPE   retType     // return premium or error
    );
	
EXPORT_C_QKNOCK double Q_OptDualWindowCN (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	double          lRebate1, 	// lower rebate for first asset
	double          lRebate2, 	// lower rebate for second asset
	double          uRebate1, 	// upper rebate for first asset
	double          uRebate2, 	// upper rebate for second asset
	Q_REBATE_TYPE   rebType,    // immediate or delayed rebate
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            startKO,    // start of knockout period
	DATE            endKO,      // end of knockout period
	DATE            expDate,    // expiration date
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nGrid1,     // number of intervals in first dimension
    int             nGrid2,		// number of intervals in second dimension
	int             nSteps,     // number of time steps in window
	double          tolerance   // tolerance in conjugate gradient solver
    );
EXPORT_C_QKNOCK double Q_FadeOutDualRes (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
	Q_DATE_VECTOR   *fadedates, // fadeout dates    
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2      // second asset forward curve
    );

EXPORT_C_QKNOCK double Q_FadeOutDualCN1 (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	Q_FADEOUT_TYPE  resFlag,    // 0 if resurrecting, 1 if not
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
	Q_DATE_VECTOR   *fadedates, // fadeout dates    
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nGrid1,     // number of intervals in first dimension
    int             nGrid2, 	// number of intervals in second dimension
    int             nSteps,     // number of time steps
    double          tolerance   // tolerance for conjugate gradient solution
    );

EXPORT_C_QKNOCK double Q_FadeOutDualCNR (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	Q_FADEOUT_TYPE  resFlag,    // 0 if resurrecting, 1 if not
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
	Q_DATE_VECTOR   *fadedates, // fadeout dates    
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nGrid1,     // number of intervals in first dimension
    int             nGrid2, 	// number of intervals in second dimension
    int             nSteps,     // number of time steps
    double          tolerance   // tolerance for conjugate gradient solution
    );

EXPORT_C_QKNOCK double Q_OptDualWindowCN2 (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower1, 	// lower knockout level for first asset
	double          lower2, 	// lower knockout level for second asset
	double          upper1, 	// upper knockout level for first asset
	double          upper2, 	// upper knockout level for second asset
	double          lRebate1, 	// lower rebate for first asset
	double          lRebate2, 	// lower rebate for second asset
	double          uRebate1, 	// upper rebate for first asset
	double          uRebate2, 	// upper rebate for second asset
	Q_REBATE_TYPE   rebType,    // immediate or delayed rebate
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            startKO,    // start of knockout period
	DATE            endKO,      // end of knockout period
	DATE            expDate,    // expiration date
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nGrid1,     // number of intervals in first dimension
    int             nGrid2,		// number of intervals in second dimension
	int             nSteps,     // number of time steps in window
	double          tolerance   // tolerance in conjugate gradient solver
    );

EXPORT_C_QKNOCK int Q_ParisianTrigTwoAssetMC (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          lower, 		// lower knockout level for second asset
	double          upper, 		// upper knockout level for second asset
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
	Q_DATE_VECTOR   *kodates, 	// trigger dates    
	int				nTrig,		// number of trigger events
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nPaths,		// number of scenarios for Monte Carlo test
	double          *premium,   // returned premium
	double          *stdErr     // returned standard error
    );

EXPORT_C_QKNOCK int Q_TrigFOEndKOMC (
	Q_OPT_TYPE      callPut,    // call or put on first asset
	Q_UP_DOWN       upOrDown,   // up or down for fadeout
	Q_UP_DOWN       upOrDownKO, // up or down for knockout
	double          spot1,   	// current	spot price of first asset
	double          spot2,   	// current	spot price of second asset
	double          strike,     // strike on first asset
	double          outVal, 	// fadeout level
	double          knockout, 	// knockout level
	DT_CURVE       	*vol1,      // volatility of first asset
	DT_CURVE        *vol2,      // volatility of second asset
	DT_CURVE        *xvol,      // cross volatility between assets
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
	Q_DATE_VECTOR   *fodates, 	// fade dates    
	double          reduction,  // amount of notional lost for each fade
    DT_CURVE        *Rd,        // domestic rate curve
    DT_CURVE        *fwds1,     // first asset forward curve
    DT_CURVE        *fwds2,     // second asset forward curve
    int             nPaths,		// number of scenarios for Monte Carlo test
	double          *premium,   // returned premium
	double          *stdErr     // returned standard error
    );
        		   
#endif

