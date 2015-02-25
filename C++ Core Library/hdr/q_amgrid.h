/* $Header: /home/cvs/src/quant/src/q_amgrid.h,v 1.31 2001/01/23 13:52:48 demeor Exp $ */
/****************************************************************
**
**	Q_AMGRID.H	- 2-factor models for amortizing swaps on base metals
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.31 $
**
****************************************************************/

#if !defined( IN_Q_AMGRID_H )
#define IN_Q_AMGRID_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QAMORT double Q_MRMAmortGridCN (
    Q_OPT_TYPE   callPut,    	// option type	(swap(fwd), call or put)
	double       spot,		 	// current cash price
	double       rangespot,     // spot used for calculating range
    DT_CURVE 	 *strike,    	// strike curve with payment dates
    Q_VECTOR 	 *Amounts,   	// One Amount per payment date
    Q_VECTOR     *LowPrice,  	// low price for 100% amortization
    Q_VECTOR     *MidPrice,  	// mid price for some middle amortization
    Q_VECTOR     *HighPrice, 	// high price for 0% amortization
    Q_VECTOR     *LowPct,    	// percentage at lower level
    Q_VECTOR     *MidPct,    	// percentage of amortization middle level
    Q_VECTOR     *HighPct,   	// percentage at upper level
    double       tenor,         // amortizes against this rolling future
	DT_CURVE     *vol,      	// spot volatility curve
    double       rangevol,   	// volatility to determine range of integration
	DATE         prcDate,       // current pricing date
    Q_VECTOR   	 *AmortIndex,	// indices of amortization dates in payment dates
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	DT_CURVE     *fwdCurve,    	// forward vector
 	Q_VECTOR     *Rd,  		   	// one interest rate per payment
	int          nGrid1,     	// number of gridpoints in short dimension
	int          nGrid2,     	// number of gridpoints in long dimension
	int          nSteps,      	// number of timesteps
	double       tolerance      // tolerance parameter for conjugate gradient
	);

EXPORT_C_QAMORT double Q_MRMBermudaAmort (
	Q_VECTOR     	*types,        // option type of each deal                    
    Q_OPT_STYLE     euroAmer,      // if American, holder can amortize or not     
	double          spot,          // current cash price                          
	double          rangespot,     // spot used for calculating range             
	Q_MATRIX        *strike,       // matrix of strikes, by deal and then by date 
	Q_DATE_VECTOR	*paydates,     // payment dates of amortizing swaps or options
    Q_MATRIX 		*Amounts,      // matrix of amounts, by deal and then by date
    Q_VECTOR        *LowPrice,     // low price for 100% amortization                 
    Q_VECTOR        *MidPrice,     // mid price for some middle amortization          
    Q_VECTOR        *HighPrice,    // high price for 0% amortization                  
    Q_VECTOR        *LowPct,       // percentage at lower level                       
    Q_VECTOR        *MidPct,       // percentage of amortization middle level         
    Q_VECTOR        *HighPct,      // percentage at upper level                       
    double          tenor,         // amortizes against this rolling future           
	DT_CURVE        *vol,          // spot volatility curve                           
    double          rangevol,      // volatility to determine range of integration    
	DATE            prcDate,       // current pricing date                            
    Q_VECTOR   	    *AmortIndex,   // indices of amortization dates in payment dates  
	double          beta,          // mean reversion                                  
	double          longvol,       // long volatility                                 
	double          corr,          // instantaneous correlation between short and long
	DT_CURVE        *fwdCurve,     // forward vector                                  
 	Q_VECTOR        *Rd,           // one interest rate per payment                   
	int             nGrid1,        // number of gridpoints in short dimension         
	int             nGrid2,        // number of gridpoints in long dimension          
	int             nSteps,        // number of timesteps                             
	double          tolerance      // tolerance parameter for conjugate gradient      
	);

EXPORT_C_QAMORT double Q_MRMAmortGridCN2 (
    Q_OPT_TYPE   callPut,    	// option type	(swap(fwd), call or put)
	double       spot,		 	// current cash price
	double       rangespot,     // spot used for calculating range
    DT_CURVE 	 *strike,    	// strike curve with payment dates
    Q_VECTOR 	 *Amounts,   	// One Amount per payment date
    Q_VECTOR     *LowPrice,  	// low price for 100% amortization
    Q_VECTOR     *MidPrice,  	// mid price for some middle amortization
    Q_VECTOR     *HighPrice, 	// high price for 0% amortization
    Q_VECTOR     *LowPct,    	// percentage at lower level
    Q_VECTOR     *MidPct,    	// percentage of amortization middle level
    Q_VECTOR     *HighPct,   	// percentage at upper level
    double       tenor,         // amortizes against this rolling future
	DT_CURVE     *vol,      	// spot volatility curve
    double       rangevol,   	// volatility to determine range of integration
	DATE         prcDate,       // current pricing date
    Q_VECTOR   	 *AmortIndex,	// indices of amortization dates in payment dates
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	DT_CURVE     *fwdCurve,    	// forward vector
 	Q_VECTOR     *Rd,  		   	// one interest rate per payment
	int          nGrid1,     	// number of gridpoints in short dimension
	int          nGrid2,     	// number of gridpoints in long dimension
	int          nSteps,      	// number of timesteps
	double       tolerance      // tolerance parameter for conjugate gradient
	);

EXPORT_C_QAMORT double Q_MRMAmortGridCN3 (
    Q_OPT_TYPE   callPut,    	// option type	(swap(fwd), call or put)
	double       spot,		 	// current cash price
	double       rangespot,     // spot used for calculating range
    DT_CURVE 	 *strike,    	// strike curve with payment dates
    Q_VECTOR 	 *Amounts,   	// One Amount per payment date
    Q_VECTOR     *LowPrice,  	// low price for 100% amortization
    Q_VECTOR     *MidPrice,  	// mid price for some middle amortization
    Q_VECTOR     *HighPrice, 	// high price for 0% amortization
    Q_VECTOR     *LowPct,    	// percentage at lower level
    Q_VECTOR     *MidPct,    	// percentage of amortization middle level
    Q_VECTOR     *HighPct,   	// percentage at upper level
    double       tenor,         // amortizes against this rolling future
	DT_CURVE     *vol,      	// spot volatility curve
    double       rangevol,   	// volatility to determine range of integration
	DATE         prcDate,       // current pricing date
    Q_VECTOR   	 *AmortIndex,	// indices of amortization dates in payment dates
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	DT_CURVE     *fwdCurve,    	// forward vector
 	Q_VECTOR     *Rd,  		   	// one interest rate per payment
	int          nGrid1,     	// number of gridpoints in short dimension
	int          nGrid2,     	// number of gridpoints in long dimension
	int          nSteps,      	// number of timesteps
	double       tolerance      // tolerance parameter for conjugate gradient
	);

EXPORT_C_QAMORT int Q_MRMAmortGridFFT (
    Q_OPT_TYPE   callPut,    	// option type	(swap(fwd), call or put)
	double       rangespot,     // spot used for calculating range
    DT_CURVE 	 *strike,    	// strike curve with payment dates
    Q_VECTOR 	 *Amounts,   	// One Amount per payment date
    Q_VECTOR     *LowPrice,  	// low price for 100% amortization
    Q_VECTOR     *MidPrice,  	// mid price for some middle amortization
    Q_VECTOR     *HighPrice, 	// high price for 0% amortization
    Q_VECTOR     *LowPct,    	// percentage at lower level
    Q_VECTOR     *MidPct,    	// percentage of amortization middle level
    Q_VECTOR     *HighPct,   	// percentage at upper level
    double       tenor,         // amortizes against this rolling future
	DT_CURVE     *vol,      	// spot volatility curve
    double       rangevol,   	// volatility to determine range of integration
	DATE         prcDate,       // current pricing date
    Q_VECTOR   	 *AmortIndex,	// indices of amortization dates in payment dates
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	DT_CURVE     *fwdCurve,    	// forward growth vector
 	Q_VECTOR     *Rd,  		   	// one interest rate per payment
	int          gridlog1,     	// log2 of number of gridpoints in short dimension
	int          gridlog2,     	// log2 of number of gridpoints in long dimension
	Q_VECTOR     *Spot,         // returned vector of values F(0,expiry)
	Q_VECTOR     *longvec,      // returned vector of long prices
	Q_VECTOR     *Values        // returned vector of option prices
	);

EXPORT_C_QAMORT double Q_MRMInterp (
	double       spot,          // current spot price
	DATE         prcDate,       // current pricing date
	DATE         expDate,       // expiration date of the deal
	DT_CURVE     *fwdCurve,    	// forward growth vector
	Q_VECTOR     *Spot,         // returned vector of values F(0,expiry)
	Q_VECTOR     *longvec,      // returned vector of long prices
	Q_VECTOR     *Values        // returned vector of option prices
	);

EXPORT_C_QAMORT int Q_MRMAmortGridFFT2(
    Q_OPT_TYPE   callPut,    	// option type	(swap(fwd), call or put)
	double       rangespot,     // spot used for calculating range
    DT_CURVE 	 *strike,    	// strike curve with payment dates
    Q_VECTOR 	 *Amounts,   	// One Amount per payment date
    Q_VECTOR     *LowPrice,  	// low price for 100% amortization
    Q_VECTOR     *MidPrice,  	// mid price for some middle amortization
    Q_VECTOR     *HighPrice, 	// high price for 0% amortization
    Q_VECTOR     *LowPct,    	// percentage at lower level
    Q_VECTOR     *MidPct,    	// percentage of amortization middle level
    Q_VECTOR     *HighPct,   	// percentage at upper level
    double       tenor,         // amortizes against this rolling future
	DT_CURVE     *vol,      	// spot volatility curve
    DT_CURVE     *rangevol1,   	// range volatility curve for spot
	double       rangevol2,     // range volatility for the second factor
	DATE         prcDate,       // current pricing date
    Q_VECTOR   	 *AmortIndex,	// indices of amortization dates in payment dates
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	DT_CURVE     *fwdCurve,    	// forward growth vector
 	Q_VECTOR     *Rd,  		   	// one interest rate per payment
	int          gridlog1,     	// log2 of number of gridpoints in short dimension
	int          gridlog2,     	// log2 of number of gridpoints in long dimension
	int          FFTflag,       // 0 if FourierStepND, 1 if FourierStepC
	Q_VECTOR     *XVec,         // returned vector of first factor
	Q_VECTOR     *YVec,         // returned vector of second factor
	Q_VECTOR     *Values        // returned vector of values F(0,expiry)
	);

EXPORT_C_QAMORT int Q_MRMChooseSwap(
	double       rangespot,     // spot used for calculating range
    DT_CURVE 	 *strike,    	// strikes and exercise dates
    Q_VECTOR 	 *Amounts,   	// One Amount per exercise date
	Q_VECTOR     *paydates,     // array of vectors of payment dates
	DT_CURVE     *vol,      	// spot volatility curve
    DT_CURVE     *rangevol1,   	// range volatility curve for spot
    double       rangevol2,   	// range volatility for long
	DATE         prcDate,       // current pricing date
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	DT_CURVE     *fwdCurve,    	// forward growth curve
 	DT_CURVE     *disc,   	   	// discount curve
	int          nGrid1,     	// number of gridpoints in short dimension
	int          nGrid2,     	// number of gridpoints in long dimension
	Q_VECTOR     *XVec,         // returned vector of first factor
	Q_VECTOR     *YVec,         // returned vector of second factor
	Q_VECTOR     *Values        // returned vector of values F(0,expiry)
	);

EXPORT_C_QAMORT double Q_MRMInterp2(
	double       spot,          // current spot price
	double       rangespot,     // range spot used in creating the grid
	Q_VECTOR     *XVec,         // returned vector of values F(0,expiry)
	Q_VECTOR     *YVec,         // returned vector of long prices
	Q_VECTOR     *Values        // returned vector of option prices
	);

EXPORT_C_QAMORT int Q_MRMChooseSwapFFT(
	double       rangespot,     // spot used for calculating range
    DT_CURVE 	 *strike,    	// strikes and exercise dates
    Q_VECTOR 	 *Amounts,   	// One Amount per exercise date
	Q_VECTOR     *paydates,     // array of vectors of payment dates
	DT_CURVE     *vol,      	// spot volatility curve
    DT_CURVE     *rangevol1,   	// range volatility curve for spot
    double       rangevol2,   	// range volatility for long
	DATE         prcDate,       // current pricing date
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	DT_CURVE     *fwdCurve,    	// forward growth curve
 	DT_CURVE     *disc,   	   	// discount curve
	int          gridlog1,     	// log2 of number of gridpoints in short dimension
	int          gridlog2,     	// log2 of number of gridpoints in long dimension
	int          M,             // number of swaps we have to choose
	Q_VECTOR     *XVec,         // returned vector of first factor
	Q_VECTOR     *YVec,         // returned vector of second factor
	Q_VECTOR     *Values        // returned m vectors of values F(0,expiry)
	);

EXPORT_C_QAMORT int Q_MRMAmortGridFJ(
    Q_OPT_TYPE   callPut,    	// option type	(swap(fwd), call or put)
	double       rangespot,     // spot used for calculating range
    DT_CURVE 	 *strike,    	// strike curve with payment dates
    Q_VECTOR 	 *Amounts,   	// One Amount per payment date
    Q_VECTOR     *LowPrice,  	// low price for 100% amortization
    Q_VECTOR     *MidPrice,  	// mid price for some middle amortization
    Q_VECTOR     *HighPrice, 	// high price for 0% amortization
    Q_VECTOR     *LowPct,    	// percentage at lower level
    Q_VECTOR     *MidPct,    	// percentage of amortization middle level
    Q_VECTOR     *HighPct,   	// percentage at upper level
    double       tenor,         // amortizes against this rolling future
	DT_CURVE     *vol,      	// spot volatility curve
    DT_CURVE     *rangevol1,   	// range volatility curve for spot
    double       rangevol2,   	// range volatility for long
	Q_VECTOR     *jumpMean,     // jump means of mean-reverting and non-mean-reverting factors
	Q_MATRIX     *jumpCov,      // jump covariance matrix
	double       lambda,        // jump frequency
	DATE         prcDate,       // current pricing date
    Q_VECTOR   	 *AmortIndex,	// indices of amortization dates in payment dates
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	DT_CURVE     *fwdCurve,    	// forward growth vector
 	Q_VECTOR     *Rd,  		   	// one interest rate per payment
	int          gridlog1,     	// log2 of number of gridpoints in short dimension
	int          gridlog2,     	// log2 of number of gridpoints in long dimension
	Q_VECTOR     *XVec,         // returned vector of first factor
	Q_VECTOR     *YVec,         // returned vector of second factor
	Q_VECTOR     *Values        // returned vector of values F(0,expiry)
	);
		
#endif

