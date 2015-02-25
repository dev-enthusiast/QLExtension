/* $Header: /home/cvs/src/quant/src/q_either.h,v 1.24 2001/11/27 23:13:46 procmon Exp $ */
/****************************************************************
**
**	Q_EITHER.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.24 $
**
****************************************************************/

#if !defined( IN_Q_EITHER_H )
#define IN_Q_EITHER_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#include <num.h> 

#define Q_OPT_PAY( OptType, Forw, Strike ) \
	(( (OptType) == Q_OPT_CALL     )  ?  max( (Forw)-(Strike), 0.0 )  :  \
	(( (OptType) == Q_OPT_PUT      )  ?  max( (Strike)-(Forw), 0.0 )  :  \
   	(( (OptType) == Q_OPT_FWD_BUY  )  ?     ( (Forw)-(Strike) )       :	 \
	(( (OptType) == Q_OPT_FWD_SELL )  ?     ( (Strike)-(Forw) )       :  HUGE_VAL ))))

#define Q_MAX_MIN( BestWorst, X1, X2 ) \
	(( (BestWorst) == Q_BEST_STRIKE  )  ?  max( X1, X2 )  :  \
	(( (BestWorst) == Q_WORST_STRIKE )  ?  min( X1, X2 )  :  HUGE_VAL ))

EXPORT_C_QUANT double Q_EO (
	Q_OPT_TYPE   callPut,        // option type
	Q_BEST_WORST bestWorst,      // best or worst strike
	double       Fwd1,			 // forward price of first currency
	double       Fwd2,			 // forward price of second currency
	double       strike,		 // strike price
	double       Sigma1,		 // volatility of first currency
	double       Sigma2,		 // volatility of second currency
	double       Rho,			 // correlation
	double       Time,			 // time to expiration
	double       Disc			 // domestic interest rate
	);

EXPORT_C_QUANT double Q_EOMulti (
	Q_OPT_TYPE   callPut,        // option type
	Q_BEST_WORST bestWorst,      // best or worst payoff
	Q_VECTOR     *Fwds,			 // forward prices
	Q_VECTOR     *strike,		 // strike prices
	Q_MATRIX     *Corr,          // correlation matrix (vols on diagonal)
	double       Time,			 // time to expiration
	double       ccRd,			 // domestic interest rate
	int          N               // fineness of grid in each dimension
	);

EXPORT_C_QUANT int Q_MakeProb1(
	long int 		count, 
	int 			dim, 
	Q_VECTOR        *Probab,
	Q_VECTOR        *Var,
	Q_PROB 			*Prob, 
	int 			N, 
	int 			Dim
	);
	  
EXPORT_C_QUANT int Q_MakeCovariance(
	Q_MATRIX   *x , 
	Q_MATRIX   *cov, 
	Q_MATRIX   *ncov, 
	double     time
	);
	  
EXPORT_C_QUANT int  Q_FindMeans1 (
	Q_MATRIX     *cov, 
	Q_PROB       *Prob, 
	int          Dim,
	Q_VECTOR     *Means
	);
	  
EXPORT_C_QUANT double Q_EO_2AssetGrid (
	Q_OPT_TYPE   	type1,      	// option type	for first currency
	Q_OPT_TYPE   	type2,      	// option type for second currency
	Q_BEST_WORST 	best_worst,     // best or worst strike on either-or
	double       	fwd1,			// forward price of first currency
	double       	fwd2,			// forward price of second currency
	double       	strike1,		// strike price for first currency
	double       	strike2,		// strike price for second currency
	double       	sig1,			// volatility of first currency
	double       	sig2,  	 	 	// volatility of second currency
	double       	rho,			// correlation
	double			t1,				// time to expiration
	double			t2,				// time to expiration
	double		 	n_dev,			// number of std devs on each side
	int			 	n_pts			// number of grid points (really have one more)
	);

EXPORT_C_QUANT double Q_EO_2AssetFast (
	Q_OPT_TYPE   	type1,      	// option type	for first currency
	Q_OPT_TYPE   	type2,      	// option type for second currency
	Q_BEST_WORST 	best_worst,     // best or worst strike on either-or
	double       	fwd1,			// forward price of first currency
	double       	fwd2,			// forward price of second currency
	double       	strike1,		// strike price for first currency
	double       	strike2,		// strike price for second currency
	double       	sig1,			// vol of first currency
	double       	sig2,  	 	 	// vol of second currency
	double       	rho,			// correlation
	double			t1,				// time to expiration
	double			t2,				// time to expiration
	double		 	n_dev,			// number of std devs on each side
	int			 	n_pts			// number of grid points (really have one more)
	);

EXPORT_C_QUANT double Q_EO_MultiAssetMC (
	Q_VECTOR   		*OptTypes,		// option types
	Q_BEST_WORST 	BestWorst,    	// get best or worst of legs
	Q_VECTOR       	*Fwds,			// forward prices
	Q_VECTOR      	*Strikes,		// strikes
	Q_MATRIX     	*Corrs,  		// correlation matrix (vols on diagonal)
	Q_VECTOR		*t,				// time to expiration
	int			 	n_MC,			// number of MC samplings
	int				SeedInit   		// initial seed for random number generation
	);

EXPORT_C_QUANT double Q_EO2Dgrid (
	Q_OPT_TYPE   	type1,      	// option type	for first currency
	Q_OPT_TYPE   	type2,      	// option type for second currency
	Q_BEST_WORST 	best_worst,     // best or worst strike on either-or
	double       	fwd1,			// forward price of first currency
	double       	fwd2,			// forward price of second currency
	double       	strike1,		// strike price for first currency
	double       	strike2,		// strike price for second currency
	double       	sig1,			// volatility of first currency
	double       	sig2,  	 	 	// volatility of second currency
	double       	rho,			// correlation
	double			t,				// time to expiration
	double		 	n_dev,			// number of std devs on each side
	int			 	n_pts			// number of grid points (really have one more)
	);

EXPORT_C_QUANT double Q_EO2Dfast (
	Q_OPT_TYPE   	type1,      	// option type	for first currency
	Q_OPT_TYPE   	type2,      	// option type for second currency
	Q_BEST_WORST 	best_worst,     // best or worst strike on either-or
	double       	fwd1,			// forward price of first currency
	double       	fwd2,			// forward price of second currency
	double       	strike1,		// strike price for first currency
	double       	strike2,		// strike price for second currency
	double       	sig1,			// vol of first currency
	double       	sig2,  	 	 	// vol of second currency
	double       	rho,			// correlation
	double			t,				// time to expiration
	double		 	n_dev,			// number of std devs on each side
	int			 	n_pts			// number of grid points (really have one more)
	);

EXPORT_C_QUANT double Q_EOmulti_MC (
	Q_VECTOR   		*OptTypes,		// option types
	Q_BEST_WORST 	BestWorst,    	// get best or worst of legs
	Q_VECTOR       	*Fwds,			// forward prices
	Q_VECTOR      	*Strikes,		// strikes
	Q_MATRIX     	*Corrs,  		// correlation matrix (vols on diagonal)
	double			t,				// time to expiration
	int			 	n_MC,			// number of MC samplings
	int				SeedInit   		// initial seed for random number generation
	);

EXPORT_C_QUANT int Q_ArbitraryPayoffMC (
	Q_VECTOR       			*Fwds,			// forward prices
	N_GENERIC_FUNC_VECTOR 	func,    		// arbitrary payoff function
	void	       	        *context,       // additional arguments needed for payoff function
	Q_MATRIX     	        *covs,          // covariance matrix
	double                  disc,           // discount factor to expiration
	int			 	        n_MC,           // number of MC samplings                    
	int				        SeedInit,       // initial seed for random number generation 
	double     				*value,         // returned value
	double                  *error          // returned error
	);

EXPORT_C_QUANT int Q_OneStepPaths (
	Q_VECTOR       			*Fwds,			// forward prices
	Q_MATRIX     	        *Corrs,         // correlation matrix (vols on diagonal)     
	double			        t,              // time to expiration                        
	int			 	        n_MC,           // number of MC samplings                    
	int				        SeedInit,       // initial seed for random number generation 
	Q_MATRIX                *Paths          // returned set of spot vectors, arranged in a matrix
	);

EXPORT_C_QUANT double Q_EO_2BasketChooser (
	Q_OPT_TYPE   	type,      		// option type
	Q_BEST_WORST 	best_worst,     // best or worst strike on either-or
	double       	fwd1,			// forward price of first commodity
	double       	fwd2,			// forward price of second commodity
	double       	fwd3,			// forward price of basket commodity
	double       	sig1,			// vol of first commodity
	double       	sig2,  	 	 	// vol of second commodity
	double       	sig3,			// vol of third commodity
	double       	rho12,			// correlation
	double       	rho13,			// correlation
	double       	rho23,			// correlation
	double       	strike,			// basket strike
	double			t,				// time to expiration
	double		 	n_dev,			// number of std devs on each side
	int			 	n_pts			// number of grid points (really have one more)
	);

#endif
