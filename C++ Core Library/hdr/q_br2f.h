/* $Header: /home/cvs/src/supermkt/src/q_br2f.h,v 1.30 1999/12/15 16:33:45 goodfj Exp $ */
/****************************************************************
**
**	Q_BR2F.H	
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.30 $
**
****************************************************************/

#if !defined( IN_Q_BR2F_H )
#define IN_Q_BR2F_H


#if !defined( IN_DATATYPE_H )
#include <datatype.h>
#endif
#if !defined( IN_DATE_H )
#include <date.h>
#endif


// Constants

#define CALIBR_OPTEXP		0  	// use OPTION  expiration times to generate calibration (constant parameter) intervals
#define CALIBR_FUTEXP  		1   // use FUTURES expiration times

								// Message (Notification) flags		   
//#define MES_QUIET			0	// Stay Quiet
//#define MES_NOTIFY	      	1	// Give Warning ('ErrMore')
#define MES_ERROR 			2	// Return error message ('return Err')

//#define CORR_SMALL			1e-6	// allow correlations to be a little larger than 1.0 without complaining

#define METHOD_CALIBRATE	1	// Do standard vol (corr.) CALIBRATION from market vols (term corrs)
#define METHOD_SPECIFY		0	// SPECIFY local volatility (corr.) parameters
#define METHOD_SPECIFY_ONE	-1	// 2-Asset calibration: Specify CONSTANT local correlations (use first element of curve)

#define	SMALL_NUMBER		1e-6

//#define BR2F_CALIBRATE		1	// actually calibrate
//#define BR2F_SPECIFY		0	// don't calibrate, specify local pars

			   
// Type definitions for new models


typedef struct
{
	DT_CURVE		*tp;				// time points for constant parameter intervals
	DT_VECTOR		*sigS,			 	// short vols
			        *cYY_loc,			// local factor covariances YY
					*cYL_loc,			//							YL
					*cLL_loc,			//							LL
					*CYY_sum,			// integrated factor covariances YY
					*CYL_sum,			//								 YL   
					*CLL_sum;			//								 LL   
	DT_CURVE   		*g_fut;				// futures fudge factor curve  

} BR2F_MODEL;



typedef struct
{
	DT_CURVE        *Vals,			// Beta(t) Curve
					*Sum;			// Integrated Beta(t) Curve 

} BR2F_BETA;



typedef struct
{
	DT_CURVE		*tp;				// time points for constant parameter intervals
	DT_VECTOR		*sigS1,			 	// short vols
					*sigS2,				//
					*rhoSS,				// local short-short correlations
			        *cYY_loc,			// local factor covariances YY
					*cYL_loc,			//							YL
					*cLY_loc,			//							LY
					*cLL_loc,			//							LL
					*CYY_sum,			// integrated factor covariances YY
					*CYL_sum,			//								 YL   
					*CLY_sum,			//								 LY   
					*CLL_sum;			//								 LL   
	DT_CURVE   		*g_fut1,			// futures fudge factor curves  
					*g_fut2;			//
 
} BR2F_MODEL_2ASSETS;


typedef struct
{
	DT_CURVE   		*Vals1,			// Beta(t) Curve Asset 1
					*Vals2,			//					   2
					*Vals12,		// Beta(t) Curve (sum of both assets)
					*Sum1,			// Integrated Beta(t) Curve Asset 1
					*Sum2;			//								  2

} BR2F_BETA_2ASSETS;


typedef struct
{
	int       	Defined;				// 0 if the rest does not exist                 		  
	DT_CURVE 	*SampCurve,        		// modified sampling curve 
				*WeightCurve,      		//  		weight	  
				*DataCurve;        		//  		data	  
	DT_ARRAY 	*VolEndDates;           //  		vol end dates 
	double   	FwdRatio;		  		// fwd value ratio 

} XVBR2F_MODCRVS;


  
// Exports 


DLLEXPORT double BR_ExpFast
(
	double				x
);


DLLEXPORT double BR_ExpIntFast 
(
	double				x,			
	double				y				
);


DLLEXPORT double BR_B
(
	DT_CURVE	*BetaSum,		// Integral of beta(t)
	DATE     	t1,				// 'from'
	DATE     	t2				// 'to'
);


DLLEXPORT int BR_B_h_h2
(
	DT_CURVE	*BetaVals,		// Beta(t)
	DATE     	t1,				// 'from'
	DATE   		t2,				// 'to'
	double		*res_B,			// results: B
	double   	*res_h,			// 		   	h
	double     	*res_h2 		// 		   	h2
);


DLLEXPORT int BR2f_Calibrate1Asset
(
	DATE				t0,				// today (Pricing Date)
	double				rhoSL,			// Long Short (SL) correlation
	double				sigL,			// Long volatility
	DT_CURVE			*vols,			// Market implied Vols (Curve with dates)
	DT_CURVE			*futexp,		// futures expiration for above vols
	double				sigS_min,		// minimum value for interpolated short vol
	int					calibr_time,	// Calibration time flag
	BR2F_MODEL			*br2f,			// output: model pars (covariances) from interpolation
	BR2F_BETA			*beta,			// Beta Curve and Integral
	int					calibr_method,	// Calibrate from Implied Vols [1] / Specify Local Vols [0]
	int					*fudge_use_flag	// Indicates whether fudge factor was used (yes -> TRUE)
);
            
    
DLLEXPORT int BR2f_LocalCovs1Asset
(
	BR2F_MODEL			*br2f,			// model pars (covariances) from interpolation
	BR2F_BETA			*beta,			// Beta Curve and Integral
	DATE				tx,	 			// fixing date of first future 
	double				*CYY,			// return local covariances: YY
    double				*CYL,			//					 		 YL
	double				*CLL			//							 LL
);


DLLEXPORT int BR2f_LocalCovs1AssetForw
(
	BR2F_MODEL			*br2f,			// model pars (covariances) from interpolation
	BR2F_BETA			*beta,			// Beta Curve and Integral
	DATE				txa,	 		// start date of forward interval
	DATE				txb,	 		// end date of forward interval
	double				*CYY,			// return local covariances: YY
    double				*CYL,			//					 		 YL
	double				*CLL			//							 LL
);


DLLEXPORT int BR2f_FutCov1Asset
(
	BR2F_MODEL			*br2f,			// model pars (covariances) from interpolation
	BR2F_BETA			*beta,		 	// Beta Curve and Integral
	DATE				tx1, 	 		// fixing date of first future 
	DATE				tx2,    		// fixing date of second future
	DATE				tf1,			// expiration date of first future
	DATE				tf2,			// expiration date of second future
	double				*Cov			// return: covariance
);


DLLEXPORT int BR2f_FutVolCorrect1Asset
(
	BR2F_MODEL			*br2f,			// model pars (covariances) from interpolation
	DATE				tf, 	 		// future expiration date
	double				*FVC			// return: Futures Vol Correction ('Fudge') factor
);


DLLEXPORT int BR2f_FutCov1AssetForw
(
	BR2F_MODEL			*br2f,			// model pars (covariances) from interpolation
	BR2F_BETA			*beta,		 	// Beta Curve and Integral
	DATE				txa,	 		// start date of forward interval
	DATE				txb,	 		// end date of forward interval
	DATE				tf1,			// expiration date of first future
	DATE				tf2,			// expiration date of second future
	double				*Cov			// return: Covariance
);


DLLEXPORT int BR2f_Covariances1Asset_SM
(
	BR2F_MODEL			*br2f,			// model pars (covariances) from interpolation
	BR2F_BETA			*beta,		 	// Beta Curve and Integral
	DT_CURVE   			*SampCurve,		// Curve containing fixing dates
	DT_ARRAY			*FutExp,   		// Curve containing futures expiration times
	DT_CURVE			*Weights,		// futures weights
	DT_CURVE			*Prices,		// futures prices
	DATE				ExpDate,		// used for variance -> volatility conversion
	void				**RetValPtr,  	// for returning stuff
	DT_DATA_TYPE		RetValType		// What are we returning??
);


DLLEXPORT int BR2f_Calibrate2Assets
(
	DATE				t0,			     // today (Pricing Date)
	BR2F_MODEL			*br2fA1,	     // input model pars for asset 1
	BR2F_MODEL 			*br2fA2,	     // input model pars for asset 2
	BR2F_BETA			*betaA1,  	     // Beta Curve and Integral (asset 1)
	BR2F_BETA			*betaA2,  	     // Beta Curve and Integral (asset 2)
	DT_CURVE			*rhoSS,		     // mixed correlations:	SS (curve)
	double				rhoSL,		     //					   	SL
	double				rhoLS,		     //						LS
	double				rhoLL,		     //						LL
	int					calibr_method,   // Calibrate from Term Corrs [1] / Specify Local Corrs [0] 
	BR2F_MODEL_2ASSETS	*br2fBoth, 	     // output: 2-Asset model info
	BR2F_BETA_2ASSETS	*betaBoth, 	     // 	Beta Curves and Integrals
	int					*bad_corr_flag,  // 	flag indicating 'bad'        ( >1 or <-1 )      local correlation
	int					*degen_corr_flag // 	flag indicating 'degenerate' ( ill determined ) local correlation
);

					
DLLEXPORT int BR2f_Calibrate2Assets_Corr1
(
	DATE				t0,				// today (Pricing Date)
	BR2F_MODEL			*br2fA1,		// input model pars for asset 1
	BR2F_MODEL 			*br2fA2,		// input model pars for asset 2
	BR2F_BETA			*betaA1,	 	// Beta Curve and Integral
	BR2F_BETA			*betaA2,	 	// Beta Curve and Integral
	BR2F_MODEL_2ASSETS	*br2fBoth, 		// output: 2-Asset model info
	BR2F_BETA_2ASSETS	*betaBoth  	 	// Beta Curves and Integrals
);


DLLEXPORT int BR2f_LocalCovs2Assets
(
	BR2F_MODEL_2ASSETS 	*br2fBoth,	// 2-Asset model info
	BR2F_BETA_2ASSETS	*betaBoth, 	// Beta Curves and Integrals
	DATE				tx,	 		// covariance expiration (end) date 
	double				*CYY,		// return local covariances: YY
    double				*CYL,		//					 		 YL
    double				*CLY,		//					 		 LY
	double				*CLL		//							 LL
);


DLLEXPORT int BR2f_LocalCovs2AssetsForw
(
	BR2F_MODEL_2ASSETS 	*br2fBoth,	// 2-Asset model info
	BR2F_BETA_2ASSETS	*betaBoth, 	// Beta Curves and Integrals
	DATE				txa,	    // start date of forward interval
	DATE				txb,	 	// end date of forward interval
	double				*CYY,		// return local covariances: YY
    double				*CYL,		//					 		 YL
    double				*CLY,		//					 		 LY
	double				*CLL		//							 LL
);


DLLEXPORT int BR2f_FutCov2Assets
(
	BR2F_MODEL_2ASSETS 	*br2fBoth,		// model pars (covariances) from interpolation
	BR2F_BETA_2ASSETS	*betaBoth,  	// Beta Curves and Integrals
	DATE				tx1, 	 		// fixing date of first future 
	DATE				tx2,       		// fixing date of second future
	DATE				tf1,			// expiration date of first future
	DATE				tf2,			// expiration date of second future
	double				*Cov			// return: covariance
);


DLLEXPORT int BR2f_FutVolCorrect2Assets
(
	BR2F_MODEL_2ASSETS	*br2fBoth,	   	// model pars (covariances) from interpolation
	DATE				tf, 	 		// future expiration date
	int					asset,			// Asset #1 or #2
	double				*FVC			// return: Futures Vol Correction ('Fudge') factor
);


DLLEXPORT int BR2f_FutCov2AssetsForw
(
	BR2F_MODEL_2ASSETS 	*br2fBoth,		// model pars (covariances) from interpolation
	BR2F_BETA_2ASSETS	*betaBoth,  	// Beta Curves and Integrals
	DATE				txa,	 		// start date of forward interval
	DATE				txb,	 		// end date of forward interval
	DATE				tf1,			// expiration date of first future
	DATE				tf2,			// expiration date of second future
	double				*Cov			// return: Covariance
);


DLLEXPORT int BR2f_Covariances2Assets_SM
(
	BR2F_MODEL_2ASSETS	*br2fBoth, 		// 2-Asset model info
	BR2F_BETA_2ASSETS	*betaBoth,  	// Beta Curves and Integrals
	DT_CURVE   			*SampCurve1,   	// Curve containing fixing dates (Asset 1)
	DT_CURVE   			*SampCurve2,   	// Curve containing fixing dates (Asset 2)
	DT_ARRAY			*FutExp1,  		// Curve containing futures expiration times (Asset 1)
	DT_ARRAY			*FutExp2,  		// Curve containing futures expiration times (Asset 2)
	DT_CURVE			*Weights1,		// futures weights (Asset 1)
	DT_CURVE			*Weights2,		// futures weights (Asset 2)
	DT_CURVE			*Prices1,		// futures prices (Asset 1)
	DT_CURVE			*Prices2,		// futures prices (Asset 2)
	DATE				ExpDate,		// used for variance -> volatility conversion
	void				**RetValPtr,  	// for returning stuff
	DT_DATA_TYPE		RetValType		// What are we returning??
);


DLLEXPORT	int	BR2F_BetaInfoFromCurve(
    DT_CURVE 	*BetaByTime,         // Beta by Time Curve
    double 		BetaFlat,            // flat Beta value (for extrapolation)
    DATE 		PricingDate,         // start date of Beta Curve and Integral
	DATE 		LastBetaInfoDate,    // last date for which Beta Info is needed
    BR2F_BETA	*beta                // (output) Beta Info structure
);


DLLEXPORT int	BR2f_CovarianceAdj_SM
(
	double		CovAvg,					// Un-adjusted covariance
	double		FwdRatio1,				// asset 1 ratio of fwds
	double		FwdRatio2,				// asset 2 ratio of fwds
	DATE	 	Today,					// start date
	DATE 		ExpDate,				// expiration date
	double 		*CovAvgPtr				// (ptr to) adjusted cov
);


DLLEXPORT int BR2f_PrecalcMRinfo1As(
	BR2F_BETA			*beta,			// Beta Curve and Integral
	DATE   				tc_prev,		// end of PREVious Calibration interval (beginning of current)
	DATE				tx,				// option eXpiration date (VID)
	DATE				tc,				// end of current Calibration interval
	DATE				tf,				// Futures expiration date (VED)
	double				*BCX, 			// RESULTS: tc_prev to tx - MR factor          
	double				*hCX1,          //          tc_prev to tx - MR kernel (plain)  
	double				*hCX2,          //          tc_prev to tx - MR kernel (squared)
	double				*BCC,           //          tc_prev to tc - MR factor          
	double				*hCC1,          //          tc_prev to tc - MR kernel (plain)  
	double				*hCC2,          //          tc_prev to tc - MR kernel (squared)
	double				*BXF            //          tx      to tf - MR factor          
);


DLLEXPORT int BR2f_Calibrate1Step1As(
	double				rhoSL,			// Long Short (SL) correlation
	double				sigL,			// Long volatility
	double				sigS_min,		// minimum value for interpolated short vol
	double				vol_impl,		// implied market vol to calibrate to		
	DATE   				t0,				// model start date (date for which market info is valid)
	DATE   				tc_prev,		// end of previous calibration interval (beginning of current)
	DATE				tx,				// option expiration date (VID)
	double				BCX, 			// MR info: tc_prev to tx - MR factor          
	double				hCX1,           //          tc_prev to tx - MR kernel (plain)  
	double				hCX2,           //          tc_prev to tx - MR kernel (squared)
	double				BXF,            //          tx      to tf - MR factor          
	double 				CYY_prev,   	// integrated covs up to tc_prev: YY 
	double 				CYL_prev,   	// 								  YL
	double 				CLL_prev,   	// 			    	 		      LL 
	double				*sigS,	   		// RESULTS: calibrated short vol for this interval
	double				*g_fut,			//   	futures vol correct ('fudge') factor
	int					*fudge_use_flag	// 		flag indicating fudge factor usage (yes = TRUE) in calibration step
);


DLLEXPORT int BR2f_Integrate1Step1As(
	double				rhoSL,			// Long Short (SL) correlation
	double				sigL,			// Long volatility
	DATE   				tc_prev,		// end of previous calibration interval (beginning of current)
	DATE				tc,				// end of current  calibration interval
	double				BCC,            // MR info:	tc_prev to tc - MR factor          
	double				hCC1,           //          tc_prev to tc - MR kernel (plain)  
	double				hCC2,           //          tc_prev to tc - MR kernel (squared)
	double 				CYY_prev,   	// integrated covs up to tc_prev: YY 
	double 				CYL_prev,   	// 								  YL
	double 				CLL_prev,   	// 			    	 		      LL 
	double				sigS,	   		// calibrated (or specified) short vol for this calibration interval
	double 				*CYY,   		// RESULTS: integrated YY cov
	double 				*CYL,   		//          integrated YL cov
	double 				*CLL,    		//          integrated LL cov
	double 				*cYY_loc,      	//          local	   YY cov [ note: local covs not needed for next calibration step ]
	double 				*cYL_loc,      	//          local	   YL cov [	provided for convenience of calibrator object ]
	double 				*cLL_loc      	//          local	   LL cov 

);



DLLEXPORT int BR2f_BuildModelFromLocal1As(
	DT_CURVE			*g_fut,			// curve with futures volcorrect factors ( date[0] = tc[0]; value[0] = 1 )
	DT_CURVE			*tc,	 		// calibration dates ( tc[0].Date = Pricing Date; tc[i].Value = i )
	DT_VECTOR			*sigS,			// short vols; sigS[0] = 0; sigS[i] valid from tc[i-1] to tc[i]
	double				sigL,			// long vol
	double				rhoSL,			// long-short correlation
	DT_VECTOR			*BCC,			// precalculated MR info; BCC[0] = 0; BCC[i] for period tc[i-1] to tc[i]
	DT_VECTOR			*hCC1,			//						  [ as for BCC ]
	DT_VECTOR			*hCC2,			//                        [ as for BCC ]
	BR2F_MODEL			*br2f			// RESULT: BR2f model pars (built from local vols)
);


DLLEXPORT int BR2f_PrecalcMRinfo2As(
	BR2F_BETA_2ASSETS	*betaBoth, 		// Beta Curves and Integrals for both assets
	DATE   				tc12_prev,		// end point of previous 2-asset calibration interval (given by date of PREVious corr info)
	DATE				tc12_end,		// end point of current  2-asset calibration interval (given by date of CURRent  corr info)
	DT_CURVE			*tc1,			// calibration dates (asset 1) -- [ only for t > tc12_prev and < tc12_end ('=' also ok) ]
	DT_CURVE			*tc2,			// calibration dates (asset 2) -- [ -""- ] 
	DT_CURVE			**tc12,			// RESULTS: calibration time point curve [starts / ends with tc12_prev / tc12_end ]
	DT_VECTOR			**BCC1,			// 		MR factors - asset 1 		( BCC1[ i ] is for tc12[ i-1 ] to tc12[ i ] )
	DT_VECTOR			**BCC2,			// 		MR factors - asset 2 		( -""- )
	DT_VECTOR			**hCC1,			// 		MR kernels - asset 1      	( -""- ) 
	DT_VECTOR			**hCC2,			// 		MR kernels - asset 2      	( -""- ) 
	DT_VECTOR			**hCC12			// 		MR kernels - both assets  	( -""- ) 
);


DLLEXPORT int BR2f_Calibrate1Step2As(
	double				rhoSS_term,	        // SS term corr to be calibrated to (or SS local corr, if we don't really calibrate)
	double				rhoSL,		        // constant mixed local corrs: SL
	double				rhoLS,		        //							   LS 
	double				rhoLL,		        //							   LL 
	double				sigL1,		        // first asset long vol
	double				sigL2,		        // second asset long vol
	int					calibr_method,	    // = METHOD_CALIBRATE (regular calibration) or = METHOD_SPECIFY (if specify local corr)
	DT_CURVE			*tc12,		        // calibration time point curve for current interval 
	DT_VECTOR			*BCC1,		        // MR factors - asset 1         ( BCC1[ i ] is for tc12[ i-1 ] to tc12[ i ] ) 
	DT_VECTOR			*BCC2,		        // MR factors - asset 2         ( -""- )                                     
	DT_VECTOR			*hCC1,		        // MR kernels - asset 1         ( -""- )                                     
	DT_VECTOR			*hCC2,		        // MR kernels - asset 2         ( -""- )                                     
	DT_VECTOR			*hCC12,		        // MR kernels - both assets     ( -""- )                                     
    DT_VECTOR			*sigS1,		        // local vols (asset 1)         ( -""- )
	DT_VECTOR			*sigS2,		        // local vols (asset 2)         ( -""- )
	double				Var1,		        // first  asset variance to end date of current interval
	double				Var2,		        // second asset variance to end date of current interval
	double 				CYY_prev,   	    // integrated covs (YY) up to beginning of this calibration interval (tc12[0])
	double 				CYL_prev,   	    //   				YL
	double 				CLY_prev,   	    // 	       			LY
	double 				CLL_prev,   	    // 	          		LL
	double				*rhoSS_loc,	        // RESULTS: local correlation for this time step
	double 				*CYY,               // 		integrated covs (YY) up to end of this calibration interval (tc12['last'])
	double 				*CYL,               // 						 YL
	double 				*CLY,               // 						 LY
	double 				*CLL,               // 				   		 LL
	int					*bad_corr_flag,		// 		flag indicating 'bad'        ( >1 or <-1 )      local correlation (yes = TRUE) resulting in this calibration step
	int					*degen_corr_flag	// 		flag indicating 'degenerate' ( ill determined ) local correlation (yes = TRUE) resulting in this calibration step
);


DLLEXPORT int BR2f_BuildModelFromLocal2As(
	DT_CURVE			*g_fut1,		// curve with futures volcorrect factors ( asset 1; date[0] = tc[0]; value[0] = 1 )
	DT_CURVE			*g_fut2,		// curve with futures volcorrect factors ( asset 2; date[0] = tc[0]; value[0] = 1 )
	double				sigL1,			// long vol	( asset 1 )
	double				sigL2,			// long vol	( asset 2 )
	double				rhoSL,			// local correlations:	SL (constant)
	double				rhoLS,			//						LS (constant)
	double				rhoLL,			//						LL (constant)
	DT_CURVE			*tc12,	 		// calibration dates ( tc12[0].Date = Pricing Date; tc12[i].Value = i )
    DT_VECTOR			*rhoSS_loc,		// local SS correlation; value[i] valid from tc12[i-1] to tc12[i]
	DT_VECTOR			*sigS1,			// short vol asset 1	 -""-
	DT_VECTOR			*sigS2,			// short vol asset 2     -""-
	DT_VECTOR			*BCC1,			// MR factor asset 1     -""-
	DT_VECTOR			*BCC2,			// MR factor asset 2     -""-
	DT_VECTOR			*hCC1,			// MR kernel asset 1	 -""-
	DT_VECTOR			*hCC2,			// MR kernel asset 2	 -""-
	DT_VECTOR			*hCC12,			// MR kernel mixed		 -""-
	BR2F_MODEL_2ASSETS	*br2fBoth		// RESULT: BR2f model pars (built from local vols)
);



#endif



