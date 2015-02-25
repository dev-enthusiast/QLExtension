/****************************************************************
**
**	q_svbond.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/quant/src/q_svbond.h,v 1.9 2000/10/17 13:02:21 demeor Exp $
**
****************************************************************/

#if !defined( IN_Q_SVBOND_H )
#define IN_Q_SVBOND_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#include <num.h>
	  
EXPORT_C_QWINGS int Q_SVBondOpt2 (
    Q_OPT_TYPE   	      callPut,       // option type                                        
    double       	      spot,          // current bond yield                                 
    double       	      strike,        // strike price                                       
	N_GENERIC_FUNC_2D     BondPrice,     // bond pricing function with yield and time as inputs
	void                  *context,      // structure containing coupon, period, maturity data for bond
	Q_VECTOR      	      *lvol,         // vector of mean volatilities                        
	Q_VECTOR 		      *lvolT,        // time vector for lvol                               
	double                initvol,       // initial short-term vol                             
	Q_VECTOR       	      *beta,         // mean reversion factor on volatility                
	Q_VECTOR       	      *sigma,        // coeff of sqrt(vol) in stochastic term for vol      
	Q_VECTOR       	      *corr,         // correlation of spot and volatility                 
	double                start,         // start of option period                             
	double       	      time,          // time to expiration                                 
	Q_VECTOR     	      *fwds,         // forward vector                                     
	Q_VECTOR     	      *fwdT,         // forward time vector                                
	double                ccRd,          // rate used for discounting                          
	Q_VECTOR		      *SVec,         // output spot vector, pre-allocated                  
	Q_VECTOR		      *VVec,         // output volatility vector, pre-allocated            
    Q_MATRIX              *OptMat        // output option price matrix, pre-allocated          
	);	

EXPORT_C_QWINGS int Q_SVBondOptATM2 (
    Q_OPT_TYPE   	      callPut,       // option type
    double       		  spot,          // current bond yield
	N_GENERIC_FUNC_2D     BondPrice,     // bond pricing function with yield and time as inputs
	void                  *context,      // structure containing coupon, period, maturity data for bond
	Q_VECTOR      	      *lvol,         // vector of mean volatilities                      
	Q_VECTOR 		      *lvolT,        // time vector for lvol                             
	double                initvol,       // initial short-term vol                           
	Q_VECTOR       	      *beta,         // mean reversion factor on volatility              
	Q_VECTOR       	      *sigma,        // coeff of sqrt(vol) in stochastic term for vol    
	Q_VECTOR       	      *corr,         // correlation of spot and volatility               
	double                start,         // start of option period                           
	double       	      time,          // time to expiration                               
	Q_VECTOR     	      *fwds,         // forward vector                                   
	Q_VECTOR     	      *fwdT,         // forward time vector                              
	double                ccRd,          // rate used for discounting                        
	Q_VECTOR		      *SVec,         // output spot vector, pre-allocated                
	Q_VECTOR		      *VVec,         // output volatility vector, pre-allocated          
    Q_MATRIX              *OptMat        // output option price matrix, pre-allocated        
	);	

EXPORT_C_QWINGS int Q_SVBondData (
    Q_OPT_TYPE   	callPut,      // option type                                          
    double       	spot,         // current bond yield                                   
	Q_VECTOR      	*lvol,        // vector of mean volatilities                          
	Q_VECTOR 		*lvolT,       // time vector for lvol                                 
	double          initvol,      // initial short-term vol                               
	Q_VECTOR       	*beta,        // mean reversion factor on volatility                  
	Q_VECTOR       	*sigma,       // coeff of sqrt(vol) in stochastic term for vol        
	Q_VECTOR       	*corr,        // correlation of spot and volatility                   
	double          realstart,    // start of realized volatility period                  
	double          start,        // start of option period                               
	double       	time,         // time to expiration                                   
	Q_VECTOR     	*fwds,        // forward vector                                       
	Q_VECTOR     	*fwdT,        // forward time vector                                  
	int             nSteps,       // number of steps in Monte Carlo                       
	int             nPaths,       // number of paths                                      
	Q_VECTOR        *SVec,        // input spot vector in grid                            
	Q_VECTOR        *VVec,        // input vol vector in grid                             
	Q_MATRIX        *OptMat,      // input option prices on grid at start of bond         
	Q_VECTOR		*spots,       // bond yields at T1 in paths                           
    Q_VECTOR        *pathopt,     // bond option values at T1 in paths                    
    Q_VECTOR        *realvol      // realized volatility calculated between start and time
	);	

EXPORT_C_QWINGS int Q_HestonFD (
    double       	spot,     		// spot used in setting the range
	Q_VECTOR      	*lvol,    		// vector of mean volatilities
	Q_VECTOR 		*lvolT,   		// time vector for lvol
	double          initvol,  		// initial short-term vol
	Q_VECTOR       	*beta,    		// mean reversion factor on volatility
	Q_VECTOR       	*sigma,   		// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR       	*corr,    		// correlation of spot and volatility
	double          start,    		// start of stepping period
	double       	time,     		// time to expiration
	Q_VECTOR     	*fwdGrowth,     // forward growth vector
	Q_VECTOR     	*fwdT,    		// forward time vector
 	Q_VECTOR		*SVec,    		// output spot vector, pre-allocated
	Q_VECTOR		*VVec,    		// output volatility vector, pre-allocated
    Q_MATRIX        *OptMat   		// output option price matrix, which gets modified
	);	

EXPORT_C_QWINGS int Q_SVBondOptFitter2 (
    double       	      spot,      // current bond yield      
	Q_MATRIX              *strikes,  // strike prices for calls 
    Q_MATRIX       	      *prices,   // prices of calls         
	N_GENERIC_FUNC_2D     BondPrice, // bond pricing function with yield and time as inputs
	void                  *context,  // structure containing coupon, period, maturity data for bond
	double                initvol,   // initial short-term vol                                      
	Q_VECTOR              *times,    // times to expiration                                         
	Q_VECTOR     	      *fwds,     // forward vector                                              
	Q_VECTOR     	      *fwdT,     // forward time vector                                         
	Q_VECTOR              *ccRd,     // one discount rate per option expiration                     
	int                   nGrid1,    // gridsize in spot dimension                                  
	int                   nGrid2,    // gridsize in volatility dimension                            
	double                tolerance, // parameter determining when to stop amoeba                   
	Q_VECTOR       	      *lvol,     // backed out limiting volatilities                            
	Q_VECTOR       	      *beta,     // backed out mean reversion factor on volatility              
	Q_VECTOR       	      *sigma,    // backed out coeff of sqrt(vol) in stochastic term for vol    
	Q_VECTOR       	      *corr,     // backed out correlation of spot and volatility               
	Q_VECTOR              *Error     // vector of least-squares errors                              
	);

EXPORT_C_QWINGS int Q_SVBondOptFitOne2 (
    double       	      spot,      // current bond yield     
	Q_VECTOR              *strikes,  // strike prices for calls
    Q_VECTOR       	      *prices,   // prices of calls        
	N_GENERIC_FUNC_2D     BondPrice, // bond pricing function with yield and time as inputs
	void                  *context,  // structure containing coupon, period, maturity data for bond
	double                initvol,   // initial short-term vol                                   
	Q_VECTOR              *times,    // times to expiration of the options                       
	Q_VECTOR     	      *fwds,     // forward vector                                           
	Q_VECTOR     	      *fwdT,     // forward time vector                                      
	Q_VECTOR              *ccRd,     // one discount rate per option expiration                  
	int                   nGrid1,    // gridsize in spot dimension                               
	int                   nGrid2,    // gridsize in volatility dimension                         
	int                   fixed,     // 1 fixes beta, 2 corr, 3 lvol, 4 sigma, 0 none
	double                tolerance, // parameter determining when to stop amoeba                
	double       	      *lvol,     // backed out limiting volatility                           
	double       	      *beta,     // backed out mean reversion factor on volatility           
	double       	      *sigma,    // backed out coeff of sqrt(vol) in stochastic term for vol 
	double       	      *corr,     // backed out correlation of spot and volatility            
	double                *Error,    // vector of least-squares error                            
	double                *options,  // returned array of option values
	int                   fittype    // 0 means price-based, 1 means volatility-based
	);

EXPORT_C_QWINGS double Q_BondOpt (
    Q_OPT_TYPE   	      callPut,       // option type
	double   	  	      fwdyield,      // forward yield
    double       	      strike,        // strike price                                       
	N_GENERIC_FUNC_2D     BondPrice,     // bond pricing function with yield and time as inputs
	void                  *context,      // structure containing coupon, period, maturity data for bond
	double       	      vol,           // constant volatility
	double       	      time,          // time to expiration of option
	double                ccRd,          // rate used for discounting                        
	int                   npts			 // number of points in integral
	);	


EXPORT_C_QWINGS int Q_SVBondOptATMF (
    Q_OPT_TYPE   	      callPut,       // option type
    double       		  spot,          // current bond yield
	N_GENERIC_FUNC_2D     BondPrice,     // bond pricing function with yield and time as inputs
	void                  *context,      // structure containing coupon, period, maturity data for bond
	Q_VECTOR      	      *lvol,         // vector of mean volatilities                      
	Q_VECTOR 		      *lvolT,        // time vector for lvol                             
	double                initvol,       // initial short-term vol                           
	Q_VECTOR       	      *beta,         // mean reversion factor on volatility              
	Q_VECTOR       	      *sigma,        // coeff of sqrt(vol) in stochastic term for vol    
	Q_VECTOR       	      *corr,         // correlation of spot and volatility               
	double                start,         // start of option period                           
	double       	      time,          // time to expiration                               
	Q_VECTOR     	      *fwds,         // forward vector                                   
	Q_VECTOR     	      *fwdT,         // forward time vector                              
	double                ccRd,          // rate used for discounting                        
	Q_VECTOR		      *SVec,         // output spot vector, pre-allocated                
	Q_VECTOR		      *VVec,         // output volatility vector, pre-allocated          
    Q_MATRIX              *OptMat        // output option price matrix, pre-allocated        
	);

EXPORT_C_QWINGS double Q_BondImpVol (
    Q_OPT_TYPE   	      callPut,       // option type
	double   	  	      fwdyield,      // forward yield
    double       	      strike,        // strike price                                       
	N_GENERIC_FUNC_2D     BondPrice,     // bond pricing function with yield and time as inputs
	void                  *context,      // structure containing coupon, period, maturity data for bond
	double       	      time,          // time to expiration of option
	double                ccRd,          // rate used for discounting                        
	int                   npts,			 // number of points in integral
	double				  price,		 // Option premium
	double			      EstimatedVol	 // first guess for volatility
	);	

typedef struct {
	Q_OPT_TYPE   	      callPut;   // option type                                           
	double       	      spot;      // current spot yield                                    
	Q_VECTOR       	      *strikes;  // array of strikes for expiration T                     
	Q_VECTOR              *prices;   // market option prices, used for objective function     
	N_GENERIC_FUNC_2D     BondPrice; // bond pricing function with yield and time as inputs
	void                  *context;  // structure containing coupon, period, maturity data for bond
	Q_VECTOR      	      *lvol;     // array of mean-reverting limiting volatilities 
	Q_VECTOR 		      *lvolT;    // nodes for lvol, beta, corr and sigma          
	double                initvol;   // initial instantaneous volatility              
	Q_VECTOR       	      *beta;     // mean reversion coefficients                   
	Q_VECTOR       	      *sigma;    // vol of vol                                    
	Q_VECTOR       	      *corr;     // correlation between the 2 Wiener terms        
	double       	      time;      // time to expiration                            
	Q_VECTOR     	      *fwds;     // forward yield curve                           
	Q_VECTOR     	      *fwdT;     // nodes for forward yield curve                 
	double                ccRd;      // rate used for discounting                     
	int                   nGrid1;    // size of spot grid                             
	int                   nGrid2;    // size of volatility grid                       
	} Q_BONDOPT_PARMS2;		

typedef struct {
	Q_OPT_TYPE   	      callPut;   // option type                                       
	double       	      spot;      // current spot yield                                
	Q_VECTOR       	      *strikes;  // array of strikes for expiration T                 
	Q_VECTOR              *prices;   // market option prices, used for objective function 
	N_GENERIC_FUNC_2D     BondPrice; // bond pricing function with yield and time as inputs
	void                  *context;  // structure containing coupon, period, maturity data for bond
	double                initvol;   // initial instantaneous volatility       
	Q_VECTOR       	      *times;    // times to expiration of options         
	Q_VECTOR     	      *fwds;     // forward yield curve                    
	Q_VECTOR     	      *fwdT;     // nodes for forward yield curve          
	Q_VECTOR              *ccRd;     // rates used for discounting the options 
	int                   nGrid1;    // size of spot grid                      
	int                   nGrid2;    // size of volatility grid                
	double                extra;     // extra parameter that is fixed
	int                   fixed;     // 1 fixes beta, 2 corr, 3 lvol, 4 sigma, 0 none 
	double                *options;  // returned option values, if desired
	int                   fittype;   // 0 means price-based, 1 means volatility-based
	} Q_BONDOPT_PARMS_SIMPLE2;		
				  
#endif

