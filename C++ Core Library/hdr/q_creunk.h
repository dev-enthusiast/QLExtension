/* $Header: /home/cvs/src/quant/src/q_creunk.h,v 1.20 2001/11/27 23:13:46 procmon Exp $ */
/****************************************************************
**
**	Q_CREUNK.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.20 $
**
****************************************************************/

#if !defined( IN_Q_CREUNK_H )
#define IN_Q_CREUNK_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_OptA (  
	Q_OPT_TYPE  	callPut,                 // option type	
	double 			spot,					 // spot price
	double 			strike,					 // strike price
	double 			volatility,				 // volatility
	double 			time_to_expiration,		 // time to expiration
	double 			ccRd,	 			     // domestic rate
	double 			ccRf,	 			     // foreign rate or yield
	int	 			nstep,					 // Number of time steps
	int	 			ngrid,					 // Number of grid points
	Q_RETURN_TYPE	retType                  // return premium or delta
	);

EXPORT_C_QUANT double Q_OptABdry (  
	Q_OPT_TYPE  	callPut,                 // option type	
	double 			spot,					 // spot price
	double 			strike,					 // strike price
	double 			volatility,				 // volatility
	double 			time_to_expiration,		 // time to expiration
	double 			ccRd,	 			     // domestic rate
	double 			ccRf,	 			     // foreign rate or yield
	int	 			nstep,					 // Number of time steps
	int	 			ngrid 					 // Number of grid points
	);
	
EXPORT_C_QUANT double Q_OptASetl (  
	Q_OPT_TYPE  	callPut,                 // option type	
	double 			spot,					 // spot price
	double 			strike,					 // strike price
	double 			volatility,				 // volatility
	double 			time_to_expiration,		 // time to expiration
	double 			time_to_settle,			 // time to settle
	double 			ccRd,	 			     // domestic rate
	double 			ccRf,	 			     // foreign rate or yield
	int	 			nstep,					 // Number of time steps
	int	 			ngrid,					 // Number of grid points
	Q_RETURN_TYPE	retType                  // return premium or delta
	);
	
EXPORT_C_QUANT double Q_OptASetlBdry (  
	Q_OPT_TYPE  	callPut,                 // option type	
	double 			spot,					 // spot price
	double 			strike,					 // strike price
	double 			volatility,				 // volatility
	double 			time_to_expiration,		 // time to expiration
	double 			time_to_settle,			 // time to settle
	double 			ccRd,	 			     // domestic rate
	double 			ccRf,	 			     // foreign rate or yield
	int	 			nstep,					 // Number of time steps
	int	 			ngrid 					 // Number of grid points
	);

EXPORT_C_QUANT double Q_OptAC (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double          startex,    // start of exercise period
    double          endex,      // end of exercise period
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );
	
EXPORT_C_QUANT int Q_FindBoundary ( 
	Q_OPT_TYPE    callPut,      // option type
	Q_VECTOR      *tempVec,     // current option values at j+1 timestep
	double        A,            // lower diagonal coefficient
	double        B,            // diagonal coefficient
	double        C,            // upper diagonal coefficient
	Q_VECTOR      *R,           // right side constant vector
	Q_VECTOR      *I,           // intrinsic value vector
	double        upper,      	// upper boundary value
	double        lower         // lower boundary value
	);

EXPORT_C_QUANT double Q_OptAVarK (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	Q_VECTOR 		*Kvec,	 	// Option strike price curve
	Q_VECTOR 		*KT,        // Option strike price grid
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QUANT double Q_OptAVarVol (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    Q_MATRIX 		*volmat,	// Underlyer instantaneous volatilities 
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
    double          range       // ratio of highest gridpoint to spot
    );
      
EXPORT_C_QUANT int Q_FindBoundaryV ( 
	Q_OPT_TYPE    callPut,      // option type
	Q_VECTOR      *tempVec,     // current option values at j+1 timestep
	Q_VECTOR      *A,           // lower diagonal coefficient
	Q_VECTOR      *B,           // diagonal coefficient
	Q_VECTOR      *C,           // upper diagonal coefficient
	Q_VECTOR      *R,           // right side constant vector
	Q_VECTOR      *I,           // intrinsic value vector
	double        upper,      	// upper boundary value
	double        lower         // lower boundary value
	);

EXPORT_C_QUANT double Q_InstallOptCN (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR        *times,     // Installment payment times
    Q_VECTOR        *payments,  // Installment payments
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QUANT double Q_PrincProtNote (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          principal,  // principal of note
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR        *times,     // Installment payment times
    Q_VECTOR        *quantity,  // quantity of option after each payment time
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QUANT double Q_FwdAC (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double          beginExer,  // start of exercise period
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

#if 0
EXPORT_C_QUANT double Q_InstallOptKO (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          outVal,     // knockout price at installment times
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR        *times,     // Installment payment times
    Q_VECTOR        *payments,  // Installment payments
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );
#endif

EXPORT_C_QUANT double Q_InstallOptCapCN (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double          LowStrike,  // Cap for Put, or Floor for Call; 0 or neg means none
	double 			strike,	 	// Option strike price
	double          HighStrike, // Cap for Call or Floor for Put; 0 or neg means none
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR        *times,     // Installment payment times
    Q_VECTOR        *payments,  // Installment payments
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );
            	    				
EXPORT_C_QUANT int Q_Creunk	(
	Q_OPT_TYPE	optype,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 		spot,
	double 		strike,
	double 		volatility,
	double 		time_to_expiration,
	double 		domestic_interest_rate,
	double 		foreign_interest_rate,
	int	 		nsteps,		// Number of time steps
	int	 		ngrid,			// Number of grid points
	double 		*price,
	double 		*delta,
	double 		*bdry 		// early exercise trigger
	);

EXPORT_C_QUANT int Q_OptACVec (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double          minSpot,    // smallest Spot for which we need accurate value
	double          maxSpot,    // largest Spot for which we need accurate value
	double 			strike,	 	// Option strike price
    double			rangevol,	// grid anchoring vol
	DT_CURVE		*vol,		// Underlyer volatility	curve
    double          startex,    // start of exercise period
    double          endex,      // end of exercise period
    DATE			prcDate,	// pricing date
    DATE			expDate,	// maturity
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
	DT_CURVE		*ForeignDisc,	// foreign currency discount curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize,	// number of intervals in price space
	Q_VECTOR		*SpotVec,		// returned array of spots
	Q_VECTOR		*OptionPrice	// returned array of option prices
    );

EXPORT_C_QUANT int Q_InstallOptCapCNVec (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double          minSpot,        // smallest Spot for which we need accurate value
	double          maxSpot,        // largest Spot for which we need accurate value
	double          LowStrike,  // Cap for Put, or Floor for Call; 0 or neg means none
	double 			strike,	 	// Option strike price
	double          HighStrike, // Cap for Call or Floor for Put; 0 or neg means none
    DATE			prcDate,	// pricing date
    DATE			expDate,	// maturity 
    double			rangevol,   // grid anchoring vol
    DT_CURVE 		*vol,			// volatility curve
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
    DT_CURVE 		*ForeignDisc,  	// foreign currency discount curve
	DT_CURVE		*InstallPmts,	// the curve of installments
	int          	nSteps, 	// number of timesteps
	int          	gridsize,	// number of intervals in price space
    Q_VECTOR		*SpotVec,		// returned vector of spots
	Q_VECTOR		*OptionPrice	// returned vector of option prices
    );
	
#endif

