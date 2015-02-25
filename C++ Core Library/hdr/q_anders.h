/****************************************************************
**
**	q_anders.h	- routine using the Andersen model
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/quant/src/q_anders.h,v 1.13 2001/09/04 13:14:44 demeor Exp $
**
****************************************************************/

#if !defined( IN_Q_ANDERS_H )
#define IN_Q_ANDERS_H
#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#include <num.h>

EXPORT_C_QWINGS int Q_AmortAndersen (
	Q_OPT_TYPE         callPut,        // option type                             
	Q_UP_DOWN          upOrDown,       // knockout direction                      
	int                efficient,      // 1 means you can choose not to amortize, 0 otherwise
	double             minSpot,        // minimum spot we need accurate  answers for
	double             maxSpot,        // maximum spot we need accurate  answers for
	DT_CURVE 		   *strike,        // strike curve                            
	Q_VECTOR           *Amounts,       // number of ounces per payment            
	double             outVal,         // knockout level                          
	Q_VECTOR           *LowPrice,      // low price for 100% amortization         
	Q_VECTOR           *MidPrice,      // mid price for some middle amortization  
	Q_VECTOR           *HighPrice,     // high price for 0% amortization          
	Q_VECTOR           *LowPct,        // percentage at lower level               
    Q_VECTOR           *MidPct,        // percentage of amortization middle level 
    Q_VECTOR           *HighPct,       // percentage at upper level               
	N_GENERIC_FUNC_3D  localvol,   	   // 3-dimensional function for local volatility
	void               *context,       // context for local vol function
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
    double             rangevol,       // volatility used to calculate the range of integration
    DATE               prcDate,        // pricing date                         
    Q_DATE_VECTOR      *AmortDates,    // amortization times                   
    Q_DATE_VECTOR      *AmortLag,      // dates when amortizations kick in     
    DATE               koDate,         // start of knockout period             
    DATE               endko,          // end of knockout period               
    DT_CURVE 		   *disc,          // domestic discount curve                  
    DT_CURVE 		   *fdisc,         // one forward price per payment time   
	int          	   nSteps,         // number of timesteps                  
	Q_VECTOR           *SpotVec,       // vector of spots    
	Q_VECTOR           *OptionPrice    // vector of prices
	);			  

EXPORT_C_QWINGS int Q_AmortAndersenL (
	Q_OPT_TYPE         callPut,        // option type                             
	Q_UP_DOWN          upOrDown,       // knockout direction                      
	int                efficient,      // 1 means you can choose not to amortize, 0 otherwise
	double             minSpot,        // minimum spot we need accurate  answers for
	double             maxSpot,        // maximum spot we need accurate  answers for
	DT_CURVE 		   *strike,        // strike curve                            
	Q_VECTOR           *Amounts,       // number of ounces per payment            
	double             outVal,         // knockout level                          
	Q_VECTOR           *LowPrice,      // low price for 100% amortization         
	Q_VECTOR           *MidPrice,      // mid price for some middle amortization  
	Q_VECTOR           *HighPrice,     // high price for 0% amortization          
	Q_VECTOR           *LowPct,        // percentage at lower level               
    Q_VECTOR           *MidPct,        // percentage of amortization middle level 
    Q_VECTOR           *HighPct,       // percentage at upper level               
	Q_VECTOR           *Times,         // times for local volatilities
	Q_VECTOR           *Spots,         // spot vector for local volatilities
	Q_MATRIX           *volMatrix,     // local volatility matrix
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
    double             rangevol,       // volatility used to calculate the range of integration
    DATE               prcDate,        // pricing date                         
    Q_DATE_VECTOR      *AmortDates,    // amortization times                   
    Q_DATE_VECTOR      *AmortLag,      // dates when amortizations kick in     
    DATE               koDate,         // start of knockout period             
    DATE               endko,          // end of knockout period               
    DT_CURVE 		   *disc,          // domestic discount curve                  
    DT_CURVE 		   *fdisc,         // one forward price per payment time   
	int          	   nSteps,         // number of timesteps                  
	Q_VECTOR           *SpotVec,       // vector of spots    
	Q_VECTOR           *OptionPrice    // vector of prices
    );			
	
EXPORT_C_QWINGS int Q_OptPortOptAndersen (
	Q_OPT_TYPE         callPut,        // option type                             
	Q_VECTOR           *types,         // types of underlying options
	double             minSpot,        // minimum spot we need accurate  answers for
	double             maxSpot,        // maximum spot we need accurate  answers for
	double             strike,         // strike of compound option
	Q_VECTOR 		   *strikes,       // strike curve for underlying options
	Q_DATE_VECTOR      *paydates,      // expiration dates of underlying options
	Q_VECTOR           *Amounts,       // number of ounces per payment            
	N_GENERIC_FUNC_3D  localvol,   	   // 3-dimensional function for local volatility
	void               *context,       // context for local vol function
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
    double             rangevol,       // volatility used to calculate the range of integration
    DATE               prcDate,        // pricing date                         
    DATE               cmpdExp,        // expiration date of compound option
    DT_CURVE 		   *disc,          // domestic discount curve                  
    DT_CURVE 		   *fdisc,         // one forward price per payment time   
	int          	   nSteps,         // number of timesteps                  
	Q_VECTOR           *SpotVec,       // vector of spots    
	Q_VECTOR           *OptionPrice    // vector of prices
    );			
	
EXPORT_C_QWINGS int Q_OptPortOptAndersenL (
	Q_OPT_TYPE         callPut,        // option type                             
	Q_VECTOR           *types,         // types of underlying options
	double             minSpot,        // minimum spot we need accurate  answers for
	double             maxSpot,        // maximum spot we need accurate  answers for
	double             strike,         // strike of compound option
	Q_VECTOR 		   *strikes,       // strike curve for underlying options
	Q_DATE_VECTOR      *paydates,      // expiration dates of underlying options
	Q_VECTOR           *Amounts,       // number of ounces per payment            
	Q_VECTOR           *Times,         // times for local volatilities
	Q_VECTOR           *Spots,         // spot vector for local volatilities
	Q_MATRIX           *volMatrix,     // local volatility matrix
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
    double             rangevol,       // volatility used to calculate the range of integration
    DATE               prcDate,        // pricing date                         
    DATE               cmpdExp,        // expiration date of compound option
    DT_CURVE 		   *disc,          // domestic discount curve                  
    DT_CURVE 		   *fdisc,         // one forward price per payment time   
	int          	   nSteps,         // number of timesteps                  
	Q_VECTOR           *SpotVec,       // vector of spots    
	Q_VECTOR           *OptionPrice    // vector of prices
    );			
	
EXPORT_C_QWINGS int Q_AndersenCalibrate (
	double             lowX,           // start of grid in log(spot)
	double             dx,             // grid spacing in log space
	int                nGrid,          // grid size - must be a power of 2
	Q_VECTOR           *times,         // vector of times where we want calibration
	N_GENERIC_FUNC_2D  impliedvol, 	   // 2-dimensional function for implied volatility
	void               *context,       // context for local vol function
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
	Q_VECTOR           *fwds,          // forwards at the times given
	Q_VECTOR           *disc,          // discount factors at the times given
	Q_MATRIX           *VarJ           // returned matrix of local var corrections for jumps
	);

EXPORT_C_QWINGS int Q_AndersenCalibrate2 (
	double             lowX,           // start of grid in log(spot)
	double             dx,             // grid spacing in log space
	int                nGrid,          // grid size - must be a power of 2
	Q_VECTOR           *times,         // vector of times where we want calibration
	N_GENERIC_FUNC_2D  impliedvol, 	   // 2-dimensional function for implied volatility
	void               *context,       // context for local vol function
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
	Q_VECTOR           *fwds,          // forwards at the times given
	Q_VECTOR           *disc,          // discount factors at the times given
	Q_MATRIX           *VarJ           // returned matrix of local var corrections for jumps
	);
	
EXPORT_C_QWINGS int Q_AndersenCalibrate3 (
	double             spot,           // current spot
	double             lowX,           // start of grid in log(spot)
	double             dx,             // grid spacing in log space
	int                nGrid,          // grid size - must be a power of 2
	Q_VECTOR           *times,         // vector of times where we want calibration
	N_GENERIC_FUNC_2D  impliedvol, 	   // 2-dimensional function for implied volatility
	void               *context,       // context for local vol function
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
	Q_VECTOR           *fwds,          // forwards at the times given
	Q_VECTOR           *disc,          // discount factors at the times given
	Q_MATRIX           *LocalVol       // returned matrix of local vols
	);

EXPORT_C_QWINGS int Q_AndersenCalibrate4 (
	double             spot,           // current spot
	double             lowX,           // start of grid in log(spot)
	double             dx,             // grid spacing in log space
	int                nGrid,          // grid size - must be a power of 2
	Q_VECTOR           *times,         // vector of times where we want calibration
	N_GENERIC_FUNC_2D  impliedvol, 	   // 2-dimensional function for implied volatility
	void               *context,       // context for local vol function
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
	Q_VECTOR           *fwds,          // forwards at the times given
	Q_VECTOR           *disc,          // discount factors at the times given
	Q_MATRIX           *LocalVol       // returned matrix of local vols
	);
	
EXPORT_C_QWINGS int Q_AndersenCalibrate5 (
	double             spot,           // current spot
	double             lowX,           // start of grid in log(spot)
	double             dx,             // grid spacing in log space
	int                nGrid,          // grid size - must be a power of 2
	Q_VECTOR           *times,         // vector of times where we want calibration
	Q_MATRIX           *termvar,       // implied term variances at the times and gridpoints
	double             jumpMean,   	   // mean of jump in log space              
	double			   jumpSD,         // standard deviation of jump             
	double			   lambda,         // frequency of jumps
	Q_VECTOR           *fwds,          // forwards at the times given
	Q_VECTOR           *disc,          // discount factors at the times given
	Q_MATRIX           *LocalVolSq     // returned matrix of local vols
	);
	
#endif
	
