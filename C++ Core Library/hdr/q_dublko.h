/* $Header: /home/cvs/src/quant/src/q_dublko.h,v 1.46 2000/04/12 15:18:18 lowthg Exp $ */
/****************************************************************
**
**	Q_DUBLKO.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.46 $
**
****************************************************************/

#if !defined( IN_Q_DUBLKO_H )
#define IN_Q_DUBLKO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_KODDel (
	Q_OPT_TYPE  	isCall,  // option type
	double 			spot,	 // Underlyer spot price <0 ==> rebates are delayed
	double 			strike,	 // Option strike price <0 ==> Put
    double 			lowout,	 // low Knock-out price
    double 			highout, // high Knock-out price
    double 			rebate,	 // Knockout rebate, <0 ==> delayed
    double 			rebate2, // rebate at higher knockout, <0 ==> delayed
    Q_REBATE_TYPE	rebType, // immediate or delayed rebate
    double 			vol,	 // Underlyer volatility
    double 			time,	 // Time to expiration
    double 			Rd,      // Domestic rate, or risk free rate
    double 			Rf,      // Foreign rate, or dividend rate
    Q_RETURN_TYPE   retType  // return premium or delta
	);

EXPORT_C_QKNOCK double Q_KOD (
	Q_OPT_TYPE  	callPut,	   // call or put
    double      	spot,		   // current spot price 
    double      	strike,		   // strike price 
    double      	lowknockout,   // lower knockout level 
    double      	highknockout,  // upper knockout level
    double      	lowerRebate,   // rebate paid upon lower ko 
    double      	upperRebate,   // rebate paid upon higher ko
    Q_REBATE_TYPE   rebType,	   // rebate type
    double      	vol,		   // volatility
    double      	time,		   // time to expiration
    double      	ccRd,		   // domestic interest rate
    double      	ccRf		   // foreign interest rate
    );
	
EXPORT_C_QKNOCK double Q_KODProb (   
	double      spot,		// Underlyer spot price
	double      lowOutVal,	// Knock-out price
	double      highOutVal,	// Knock-out price
	double      vol,		// Underlyer volatility
	double      time,		// Time to expiration
	double      ccRd,		// Domestic rate, or risk free rate
	double      ccRf		// Foreign rate, or dividend rate
	);
	
EXPORT_C_QKNOCK double Q_KODLife (
	double spot,	 	 // Underlyer spot price
    double lowknockout,	 // Knock-out price
    double highknockout, // Knock-out price
    double vol,		 	 // Underlyer volatility
    double time,	 	 // Time to expiration
    double ccRd,       	 // Domestic rate, or risk free rate
    double ccRf       	 // Foreign rate, or dividend rate
    );
EXPORT_C_QKNOCK double Q_BinKOD (
	Q_OPT_TYPE  callPut,        // call => payoff above strike
    double      spot,           // current spot     	
    double      strike,         // strike price       	
    double      lowknockout,    // lower knockout boundary            	
    double      highknockout,   // upper knockout boundary             	
    double      vol,            // volatility    	
    double      time,           // time to expiration     	
    double      ccRd,           // domestic interest rate     	
    double      ccRf,           // foreign interest rate     	
    double      payoff          // payoff at expiration      	
    );                	

EXPORT_C_QKNOCK double Q_KODAppear (
	Q_OPT_TYPE  	callPut,	   // call or put
	Q_UP_DOWN       upOrDown,      // appears at upper or lower boundary
    double      	spot,		   // current spot price 
    double      	strike,		   // strike price 
    double      	lowknockout,   // lower knockout level 
    double      	highknockout,  // upper knockout level
    double      	vol,		   // volatility
    double      	time,		   // time to expiration
    double      	ccRd,		   // domestic interest rate
    double      	ccRf		   // foreign interest rate
    );
EXPORT_C_QKNOCK double Q_DoubleKODensity (  
	double      spot,			   // current spot
	double      strike,		       // strike, or ending spot
	double      lowknockout,	   // lower knockout price
	double      highknockout,	   // higher knockout price
	double      vol,			   // volatility
	double      time,			   // time to expiration
	double      ccRd,			   // domestic rate
	double      ccRf               // foreign rate
	);

EXPORT_C_QKNOCK double Q_KODAppearKO (
	Q_OPT_TYPE  	callPut,	   // call or put
	Q_UP_DOWN       upOrDown,      // appears at upper or lower boundary
    double      	spot,		   // current spot price 
    double      	strike,		   // strike price 
    double      	lowknockout,   // lower knockout level 
    double      	highknockout,  // upper knockout level
    double      	vol,		   // volatility
    double      	time,		   // time to expiration
    double      	ccRd,		   // domestic interest rate
    double      	ccRf		   // foreign interest rate
    );
EXPORT_C_QKNOCK double Q_KODCondMin (  
	double      endpoint,	   // endpoint of random walk
	double      minimum,	   // reference minimum
	double      maximum,       // attained maximum
	double      sigma  		   // standard deviation (i.e. vol*sqrt(t))
	);
EXPORT_C_QKNOCK double Q_KODCoi (
	Q_OPT_TYPE    	callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_IN_OUT     	inOrOut, 			// appearing or disappearing
	double 			spot,	 			// Underlyer spot price
	double 			strike,	 			// Option strike price
    double      	lowknockout,   		// lower knockout level
    double      	highknockout,  		// upper knockout level
    double      	lowerRebate,   		// rebate paid upon lower ko
    double      	upperRebate,   		// rebate paid upon higher ko
    Q_REBATE_TYPE	rebType,			// Immediate or delayed till exp date
    Q_VECTOR 		*vol,				// Underlyer volatility	curve
    Q_VECTOR 		*volT,      		// grid for volatilities
    double 			time,	 			// Time to expiration
    Q_VECTOR 		*Rd,     			// Domestic rate curve
    Q_VECTOR 		*rdT,       		// grid for domestic rate curve
    Q_VECTOR 		*fwds,     			// forward curve
    Q_VECTOR 		*fwdT,      		// grid for forward curve
	int          	nSteps, 			// number of timesteps
	int          	gridsize			// number of intervals in price space
    );
EXPORT_C_QKNOCK double Q_BinKODCoi (
	Q_OPT_TYPE    	callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_IN_OUT     	inOrOut, 			// appearing or disappearing
	double 			spot,	 			// Underlyer spot price
	double 			strike,	 			// Option strike price
    double      	lowknockout,   		// lower knockout level
    double      	highknockout,  		// upper knockout level
    double      	lowerRebate,   		// rebate paid upon lower ko
    double      	upperRebate,   		// rebate paid upon higher ko
    Q_REBATE_TYPE	rebType,			// Immediate or delayed till exp date
    Q_VECTOR 		*vol,				// Underlyer volatility	curve
    Q_VECTOR 		*volT,      		// grid for volatilities
    double 			time,	 			// Time to expiration
    Q_VECTOR 		*Rd,     			// Domestic rate curve
    Q_VECTOR 		*rdT,       		// grid for domestic rate curve
    Q_VECTOR 		*fwds,     			// forward curve
    Q_VECTOR 		*fwdT,      		// grid for forward curve
    double          payoff,				// payoff if in the money at expiration
	int          	nSteps, 			// number of timesteps
	int          	gridsize			// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_KODInterval(
	Q_OPT_TYPE      callPut,   		// option type
    double      	spot,	   		// current spot price
    double      	strike,	   		// strike price on option
    double      	lowerKO,   		// lower knockout level
    double      	upperKO,   		// upper knockout level
    double      	lowerRebate,	// rebate upon lower knockout
    double      	upperRebate,	// rebate upon upper knockout
    Q_REBATE_TYPE   rebType,   		// immediate or delayed rebate
    double      	vol0,	   		// volatility to start of ko period
    double      	vol1,	   		// volatility to end of ko period
    double      	vol2,	   		// volatility to expiration
    double      	t0,		   		// start of ko period
    double      	t1,		   		// end of knockout period
    double      	t2,		   		// time to expiration
    double      	ccRd0,	   		// domestic rate to start of ko period
    double      	ccRd1,	   		// domestic rate to end of ko period
    double      	ccRd2,	   		// domestic rate to expiration
    double      	ccRf0,	   		// foreign rate to start of ko period
    double      	ccRf1,	   		// foreign rate to end of ko period
    double      	ccRf2	   		// foreign rate to expiration
    );

EXPORT_C_QKNOCK double Q_PKOD (
	Q_OPT_TYPE  	callPut,		 // option type
    double      	spot,			 // current spot price
    double      	strike,			 // strike price
    double      	lowknockout,	 // lower knockout price
    double      	highknockout,	 // higher knockout price
    double      	lowerRebate,     // rebate at lower knockout
    double      	upperRebate,     // rebate at higher knockout
    Q_REBATE_TYPE   rebType,         // rebate type
    double      	vol1,			 // short volatility
    double      	vol2,			 // long volatility
    double      	t1,	             // end of knockout period
    double      	t2,	             // maturity
    double      	ccRd1,			 // short domestic rate
    double      	ccRd2,			 // long domestic rate
    double      	ccRf1,			 // short foreign rate
    double      	ccRf2			 // long foreign rate
    );

EXPORT_C_QKNOCK double Q_KiKoPrtl (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDownKI, // Direction of Knockin
	Q_UP_DOWN     	upOrDownKO, // Direction of Knockout
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    double 			inVal,  	// knockin barrier
    double 			outVal,  	// knockout barrier
    double 			rebate,	 	// rebate upon knockout
    Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double          kotime,     // start of knockout period
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_KiKoDouble (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_TYPE    	locp, 		// Option type after lower knockin
	Q_OPT_TYPE    	hicp,	    // Option type after upper knockin
	double 			spot,	 	// Underlyer spot price
	double          strike,     // strike if no knockin occurs
	double 			lostrike,	// lower option strike price
	double 			histrike,	// upper option strike price
    double 			lowerInval, // lower knockin barrier
    double 			upperInval, // lower knockin barrier
    double          loloko,     // lower ko barrier after hitting lower knockin
    double          lohiko,     // upper ko barrier after hitting lower knockin
    double          hiloko,     // lower ko barrier after hitting upper knockin
    double          hihiko,     // upper ko barrier after hitting upper knockin
    double          loloreb,    // lower rebate after hitting lower knockin
    double          lohireb,    // upper rebate after hitting lower knockin
    double          hiloreb,    // lower rebate after hitting upper knockin
    double          hihireb,    // upper rebate after hitting upper knockin
    Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize,	// number of intervals in price space
	double			anchorvol	// Vol used to anchor the grid if the barriers are far apart
    );

EXPORT_C_QKNOCK double Q_KODBridge (
	  double      strike,		   // strike, or ending spot
	  double      lowknockout,	   // lower knockout price
	  double      highknockout,	   // higher knockout price
	  double      vol			   // volatility
	  );

EXPORT_C_QKNOCK void Q_DoubleKnockOut(
	Q_OPT_TYPE    	isCall,	 		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 		// Underlyer spot price
	double 			strike,	 		// Option strike price
	double 			lowOutVal,		// Knock-out price
	double 			highOutVal,		// Knock-out price
	double 			lowerRebate, 	// rebate for knocking at the lower level
	double 			upperRebate,	// rebate for knocking at the upper level
	Q_REBATE_TYPE   rebType,	 	// Immediate or delayed till exp date
	double 			vol,		 	// Underlyer volatility
	double 			time,	 		// Time to expiration
	double 			settle,   		// Time to settlement
	double 			ccRd,     		// Domestic rate, or risk free rate
	double 			ccRf,     		// Foreign rate, or dividend rate
	double 			*premRet, 		// Returned option premium
	double 			*deltaRet 		// Returned option delta
	);

EXPORT_C_QKNOCK double Q_DoubleKnockOutProb(
	double spot,	 	 // Underlyer spot price
	double lowknockout,	 // Knock-out price
	double highknockout, // Knock-out price
	double vol,		 	 // Underlyer volatility
	double time,	 	 // Time to expiration
	double settle,   	 // Time to settlement
	double ccRd,     	 // Domestic rate, or risk free rate
	double ccRf     	 // Foreign rate, or dividend rate
	);

EXPORT_C_QKNOCK double Q_DoubleKOAvgLife(
	double spot,	 	 // Underlyer spot price
	double lowknockout,	 // Knock-out price
	double highknockout, // Knock-out price
	double vol,		 	 // Underlyer volatility
	double time,	 	 // Time to expiration
	double settle,   	 // Time to settlement
	double ccRd,     	 // Domestic rate, or risk free rate
	double ccRf     	 // Foreign rate, or dividend rate
	);

EXPORT_C_QKNOCK void Q_MasterDoubleKO(
	Q_OPT_TYPE    	isCall,     	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_ALIVE_DEAD    knockedOut, 	// Already knocked out or not
	Q_IN_OUT    	inOrOut,    	// knockin or knockout
	double 			spot,	 		// Underlyer spot price
	double 			strike,	 		// Option strike price
	double 			lowOutVal,		// Knock-out price
	double 			highOutVal,		// Knock-out price
	double 			lowerRebate, 	// rebate for knocking at the lower level
	double 			upperRebate,	// rebate for knocking at the upper level
	Q_REBATE_TYPE   rebType,	 	// Immediate or delayed till exp date
	double 			vol,		 	// Underlyer volatility
	double 			time,	 		// Time to expiration
	double 			settle,   		// Time to settlement
	double 			ccRd,     		// Domestic rate, or risk free rate
	double 			ccRf,     		// Foreign rate, or dividend rate
	double 			*premRet, 		// Returned option premium
	double 			*deltaRet 		// Returned option delta
	);

EXPORT_C_QKNOCK double Q_MultisegDKIDKO(
	Q_OPT_TYPE    	OptionType,				// Option type (Q_OPT_PUT, Q_OPT_CALL or Q_OPT_FWD)
	double			Strike,					// strike
	Q_VECTOR		*LowerBarrierLevels,	// lower barrier levels
	Q_VECTOR		*LowerBarrierTimes,		// End times of each lower barrier
	Q_VECTOR		*UpperBarrierLevels,	// upper barrier levels
	Q_VECTOR		*UpperBarrierTimes,		// End times of each upper barrier
	double 			Term,	 				// Time to expiration
	double			DownQuantity,			// Quantity of lower KO
	Q_OPT_TYPE		OptionTypeDown,			// Option type of lower KO
	double			StrikeDown,				// strike of lower KO
	double			TermDown,				// term of lower KO
	double			KODownLow,				// lower knockout of lower KO
	double			KODownHigh,				// upper knockout of lower KO
	double			RebateDownLow,			// lower rebate of lower KO
	double			RebateDownHigh,			// upper rebate of lower KO
	double			UpQuantity,				// Quantity of upper KO
	Q_OPT_TYPE		OptionTypeUp,			// Option type of upper KO    
	double			StrikeUp,				// strike of upper KO         
	double			TermUp,					// term of upper KO           
	double			KOUpLow,                // lower knockout of upper KO 
	double			KOUpHigh,               // upper knockout of upper KO 
	double			RebateUpLow,            // lower rebate of upper KO   
	double			RebateUpHigh,           // upper rebate of upper KO   
	double			Spot,					// spot
	Q_VECTOR 		*Volatilities,			// Underlyer volatility	curve
	Q_VECTOR 		*VolatilityTimes,		// times for volatilities
	Q_VECTOR 		*Discounts,				// Domestic discount factors
	Q_VECTOR 		*DiscountTimes,			// times for discount factors
	Q_VECTOR 		*Forwards, 				// forwards
	Q_VECTOR 		*ForwardTimes,			// times for forwards
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	double			ReferenceSpot,			// spot to set the grid range
	double			RangeVol				// volatility used to set up the grid
);
	
EXPORT_C_QKNOCK int	Q_MultiDKIDKO_MC(
	Q_OPT_TYPE    	OptionType,			// Option type (Q_OPT_PUT, Q_OPT_CALL or Q_OPT_FWD)
	double			Strike,				// strike
	DATE		  	PricingDate,		// Pricing Date
	Q_VECTOR		*EndTimes,			// End time of each segment
	Q_VECTOR		*LowKOBarriers,		// Knock-out barrier( Strictly negative means no barrier )
	Q_VECTOR		*HighKOBarriers,	// Knock-out price( "	" )
	double			DownQty,			// Qty of lower KO
	Q_OPT_TYPE		DownOptionType,		// Option type of lower KO
	double			DownStrike,			// strike of lower KO
	double			DownTerm,			// term of lower KO
	double			DownKOLow,			// lower knockout of lower KO
	double			DownKOHigh,			// upper knockout of lower KO
	double			DownRebateLow,		// lower rebate of lower KO
	double			DownRebateHigh,		// upper rebate of lower KO
	double			UpQty,				// Qty of upper KO option
	Q_OPT_TYPE		UpOptionType,		// Option type of upper KO    
	double			UpStrike,			// strike of upper KO         
	double			UpTerm,				// term of upper KO           
	double			UpKOLow,			// lower knockout of upper KO 
	double			UpKOHigh,			// upper knockout of upper KO 
	double			UpRebateLow,		// lower rebate of upper KO   
	double			UpRebateHigh,		// upper rebate of upper KO   
	DT_CURVE  		*FwdCurve,			// Forward curve.
	DT_CURVE  		*VolCurve,			// Term vol curve.
	DT_CURVE  		*DiscFactorCurve,	// Discount Factor curve.
	double			StdErrTolerance,	// Stop if StdErr is less than this, else NumPathsBy2 reached
	int          	NumPathsBy2, 		// Maximum Num Monte Carlo paths
	int				Seed,				// MC seed (large negative number is best)
	double			*Premium,			// the price returned
	double			*StdError			// the standard deviation of the premium
);
	
	
#endif
