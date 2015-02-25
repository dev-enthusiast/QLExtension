/* #define GSCVSID "$Header: /home/cvs/src/qcommod/src/gnf_q.h,v 1.42 2014/02/13 23:14:23 e041980 Exp $" */
/****************************************************************
**
**	gnf_q.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.42 $
**	
**  $Log: gnf_q.h,v $
**  Revision 1.42  2014/02/13 23:14:23  e041980
**  Added the newly created function "GnfMrFactor_bare_bis" into the h file
**
**  Revision 1.41  2007/10/11 15:40:35  e13749
**  Implement GnfAvgVol1As1 and fix some errors in covariance calculation.
**  iBug: #41209
**
**  Revision 1.40  2007/09/07 00:56:37  e13749
**  Implement GnfDailyPriceNominal1As1.\niBug: #41209.
**
**  Revision 1.39  2007/07/08 21:24:31  e13749
**  Added (SlangX)GnfDailyVol1As0.
**  iBug: #41209
**
**  Revision 1.38  2007/07/06 22:26:00  e13749
**  Clean up.
**  iBug: #41209
**
**  Revision 1.37  2007/07/05 15:19:57  e13749
**  Added (SlangX)GnfSwaptionVol1As0.
**  iBug: #41209
**
**  Revision 1.36  2007/07/05 00:58:12  e13749
**  (SlangX)GnfMonthlyVol1As0
**  (SlangX)GnfDailyPriceNominal1As0:
**  Streamlined, improved error handling and messaging.
**  iBug: #41209
**
**  Revision 1.35  2007/07/04 00:32:33  e13749
**  Added iBug number.
**  iBug: #41209
**
****************************************************************/

#if !defined( IN_GNF_Q_H )
#define IN_GNF_Q_H

#include <qc_q.h>
#include <gob.h>

// 	Constants

		// These are copied over from supermkt/q_br2f.h; should probably be removed from there some day
#define METHOD_CALIBRATE	1	// Do standard vol (corr.) CALIBRATION from market vols (term corrs)
#define METHOD_SPECIFY		0	// SPECIFY local volatility (corr.) parameters


		 
// Macros
				  
#define COV_INDEX( f1, f2 ) \
	( (f1) >= (f2) ?  ( (f1) * ( (f1) + 1 ) ) / 2 + (f2)  :  ( (f2) * ( (f2) + 1 ) ) / 2 + (f1) )  

#define CORR_INDEX( f1, f2 ) \
	( (f1) > (f2)  ?  ( (f1) * ( (f1) - 1 ) ) / 2 + (f2)  :  ( (f2) > (f1) ? ( (f2) * ( (f2) - 1 )) / 2 + (f1) : -1 ) )
								// the '-1' denotes an error (there is no corr index for i1 = i2)

#define ODR_INDEX( f1, f2, n2 ) \
	( (n2) * (f1) + (f2) )
								
	
// ------------------------------------------------------------------------------------
// TypeDefs
// ------------------------------------------------------------------------------------


// !!! HERE we use the requirement that Cov->Term[0].Value = 0.0 (i.e. t0 is 'VarRefTime')
  
typedef struct						
{								   	
	DT_MATRIX		    *FactDiagCoeff;	// [f][f]
	DT_TCURVE		    *t;				// [ic]    	
	DT_MATRIX		    *Vol;			// [c][ic] 	
	DT_MATRIX		    *Corr;			// [c'][ic] 	

} GNF_COV_ND; // 'CovNd' (time grid should be same as for GNF_COV_MAT; but have it here to be 'self sufficient')

  									
typedef struct						
{								   	
	DT_TCURVE		    *t;				// [ic]    	
	DT_MATRIX		    *Local,			// [c][ic] 	
						*Term,			// [c][ic] 	Required: Cov->Term[c][0] = 0.0 (i.e. Cov->t[0].Date is 'Variance Referece Time')
						*LocalVolNd,  	// [f][ic]
						*LocalCorrNd, 	// [c'][ic]
						*FactDiagCoeff;	// [f][f]

} GNF_COV_MAT; // 'CovMat'		


typedef struct						
{								   	
	DT_TCURVE		    *t;			// [ic]    	
	DT_MATRIX		    *Local,		// [c][ic] 	
						*Term;		// [c][ic] 	Required: Cov->Term[c][0] = 0.0 (i.e. Cov->t[0].Date is 'Variance Referece Time')
} GNF_COV_ODR; // 'CovOdr'		


typedef struct						
{
	DT_TCURVE		    *t;	   		// [im]    
	DT_MATRIX		    *Rate,		// [f][im] 
						*RateSum;   // [f][im] 	
} GNF_MR_BOTH; // 'MrBoth' 

/*
typedef struct						 
{
	DT_TCURVE		    *t;	   		// [im]    	
	DT_MATRIX		    *Rate1,		// [f][im] 	
					    *Rate2,		// [f][im] 	
						*RateSum1,  // [f][im] 	
						*RateSum2;  // [f][im] 	
} GNF_MR_BOTH2;
*/

typedef struct						
{
	DT_TCURVE		    *t;			// [iv]	   	
	DT_MATRIX		    *Fact;		// [f][iv]	

} GNF_EXP_VC; // 'ExpVc'


typedef struct						
{
	DT_TCURVE		    *Base;		// [is]	   	
	double				Divisor;	// 			

} GNF_SH_REF; // 'ShRef'		


typedef struct		
{
	DT_TIME			    MktDataTime;	
	int					n_fact;			
						
	GNF_COV_MAT			*CovMat;			
	GNF_MR_BOTH			*MrBoth;			
	GNF_EXP_VC			*ExpVc;			
	GNF_SH_REF			*ShRef;			

} GNF_MODEL_1AS; // 'Model1As'			


typedef struct		
{
	DT_TIME			    MktDataTime;	
	int					n_fact1,        
						n_fact2;		
						
	GNF_COV_ODR			*CovOdr;			

	GNF_MR_BOTH	   		*MrBoth1,
						*MrBoth2;			
	GNF_EXP_VC			*ExpVc1,           
						*ExpVc2;			
	GNF_SH_REF			*ShRef1,          
						*ShRef2;			

} GNF_MODEL_2ASX;	// 'Model2Asx'	


// These two not really 'finally designed' and 'put to use' yet ...

typedef struct		
{
	DT_TIME			    MktDataTime;	
	int					n_fact;			
						
	GNF_COV_MAT			*CovMat;			
	GNF_MR_BOTH			*MrBoth;			

	int					n_assets;		
											// 'a' in comments below labels assets; a = 0, 1, ..., n_assets - 1
	DT_VECTOR			**indx;				// For each asset 'a' tells which model factor indices are used
	GNF_EXP_VC			**ExpVc;			// ExpVc[a] = VC factors for asset 'a'; has size (i.e. # of factors) indx[a]->Size
	GNF_SH_REF			**ShRef;	  		// ShRef[a] = Shift Ref  for asset 'a'

} GNF_MODEL_SHARED;	// 'ModelShd'	


typedef struct		
{
	DT_TIME			    MktDataTime;	
	int					n_models;  			// number of models (assets) we have collected here 	
						                    
	int					*n_fact;			// 'n' of each below [ n = n_models ]
	GNF_COV_MAT			**CovMat;			//
	GNF_MR_BOTH			**MrBoth;			//
	GNF_EXP_VC			**ExpVc;			//
	GNF_SH_REF			**ShRef;			//

	GNF_COV_ODR			**CovOdr;	   		// 'N_CORR( n ) = n * ( n-1 ) / 2' of those; in usual 'correlation count' ordering 10, 20, 21, 30, ...
	int					*FlipOdr;			// tells us if the 'Odr' must be 'flipped' (TRUE) or has the right orientation (FALSE)

} GNF_MODEL_MANY; // 'ModelMany' 	


/*
typedef struct		
{
	DT_TIME			    MktDataTime;	
	int					n_assets;		
	int					*n_fact;			// 1 per asset
						
	GNF_COV_ODR			**CovOdr;	   		// n * ( n-1 ) / 2 of those

	GNF_MR_BOTH			**MrBoth;				// 1 per asset
	GNF_COV_MAT			**CovMat;			    // 1 per asset
	GNF_EXP_VC			**ExpVc;			    // 1 per asset
	GNF_SH_REF			**ShRef;			    // 1 per asset

} GNF_MODEL_MANY; // 'ModelMany' 	
*/


// ------------------------------------------------------------------------------------
// Exports
// ------------------------------------------------------------------------------------


DLLEXPORT int GnfMonthlyVol1As0(
	double	*MR_factors,	//?
	double	*VC_factors,	//?
	int		 n_factors,		//?
	double	 t,				//?
	double	*T,				//?
	int		 n_days,		//?
	double	*Vol			//?
);


DLLEXPORT int GnfDailyPriceNominal1As0(
	double		*MR_factors,
	double		*VC_factors,
	int			 numfactors,
	double		*t,
	double		*T,
	int			 numdays,
	double		 fwd,
	double		 strike,
	Q_OPT_TYPE	 opt_type,
	double		*price,
	double		*volmin,
	double		*volmax
);


DLLEXPORT int GnfDailyPriceNominal1As1(
    DT_MATRIX   *VolCorrectFactors,
    DT_MATRIX   *MrRates,
    DT_MATRIX   *LocalVols,
    DT_MATRIX   *LocalCorrs,
    DT_VECTOR   *TimeGrid,
    DT_VECTOR   *t,
    DT_VECTOR   *T,
    DT_VALUE    *Fwds,
    DT_VALUE    *Strikes,
    Q_OPT_TYPE   OptType,
	double		*Price,
	double		*vols
);


DLLEXPORT int GnfAvgVol1As1(
    DT_MATRIX   *VolCorrectFactors,
    DT_MATRIX   *MrRates,
    DT_MATRIX   *LocalVols,
    DT_MATRIX   *LocalCorrs,
    DT_VECTOR   *TimeGrid,
    double       t,
    DT_VECTOR   *T,
    DT_VALUE    *Fwds,
	double		*vol
);


DLLEXPORT int GnfDailyVol1As0(
	double		*MR_factors,
	double		*VC_factors,
	int			 n_factors,
	double		*t,
	double		*T,
	int			 n_days,
	double		 fwd,
	double		 strike,
	double		*vol
);


DLLEXPORT int GnfSwaptionVol1As0(
	DT_VECTOR	 *MR_factors,
	DT_MATRIX	 *VC_factors,
	DATE		  today,
	DATE		  T_option,
	DT_CURVE	 *fwds,
	double		 *vol
);


DLLEXPORT int GnfBuildCovDiagCoeff(
	DT_MATRIX			*FactDiagCoeff,	// Model Diagonalization Coeffs for FACTORS
	DT_MATRIX		   **CovDiagCoeff	// Result: Model Diagonalization Coeffs for COVS
);


DLLEXPORT int GnfCovsFromVolsAndCorrs(
	DT_VECTOR			*Vols,		// Volatilities 
	DT_VECTOR			*Corrs,		// Correlations
	DT_VECTOR		   **Covs		// Result: Covariances
);


DLLEXPORT int GnfCovVector2Matrix(
	DT_VECTOR			*Cov_V,		// Covariance Vector
	DT_MATRIX		   **Cov_M		// Result: Covariance Matrix
);


DLLEXPORT int GnfCovVector2QMatrix(
	DT_VECTOR			*Cov_V,		// Covariance Vector
	Q_MATRIX			*Cov_M		// Result: Covariance Matrix
);


DLLEXPORT int GnfCovVectorIntoQMatrix(
	DT_VECTOR			*Cov_V,		// Covariance Vector
	Q_MATRIX			*Cov_M,		// Result: Covariance Matrix
	int                             offset
);


DLLEXPORT int GnfXCovVectorIntoQMatrix(
	DT_VECTOR			*Cov_V,		// Covariance Vector
	Q_MATRIX			*Cov_M,		// Result: Covariance Matrix
	int                             n_fact1,
	int                             n_fact2,
	int                             offset1,
	int                             offset2
);


DLLEXPORT int GnfMrFactKern1As_bare(
	GNF_MR_BOTH				*MrBoth,		   		// 
	int					n_fact,
	DT_TIME	   	 		t1,				// Want Meanrev factors and kernels from time t1 to time t2
	DT_TIME				t2,				// 
	DT_VECTOR			**MrFactor,	   	// Results: Vector of MeanRev factors
	DT_VECTOR			**MrKernel	   	//       	Vector of MeanRev kernels
);


DLLEXPORT int GnfMrFactKern1As_V(
	GNF_MR_BOTH	   		*MrBoth,		   		// 
	int					n_fact,
	DT_TCURVE			*tX,
	QC_VEC_ARRAY		**MrFactor,
	QC_VEC_ARRAY		**MrKernel
);


DLLEXPORT int GnfFactorCovs1As_bare_0(
	GNF_COV_MAT	   	*CovMat,		   		// 
	GNF_MR_BOTH			*MrBoth,		   		// 
	int				n_fact,				
	DT_TIME			tx,
	DT_VECTOR		**Cov	   	//     result: Factor Covs
);


DLLEXPORT int GnfFactorCovs1As_bare(
	GNF_COV_MAT	   	*CovMat,		   		// 
	GNF_MR_BOTH			*MrBoth,		   		// 
	int				n_fact,				
	DT_TIME	   	 	t1,				// Want Meanrev factors and kernels from time t1 to time t2
	DT_TIME			t2,				// 
	DT_VECTOR		**Cov	   		//      Factor Covs
);


DLLEXPORT int GnfFactorCovs1As(
	GNF_MODEL_1AS	*Model1As,		   		// 
	DT_TIME	   	 	t1,				// Want Meanrev factors and kernels from time t1 to time t2
	DT_TIME			t2,				// 
	DT_VECTOR		**Cov	   		//      Factor Covs
);


DLLEXPORT int GnfMrFactors_bare_bis(
	GNF_MR_BOTH			*MrBoth,		   		// 
	int				n_fact,				
	DT_TIME			t1,					// Want Meanrev factor from time t1 to time t2
	DT_TIME			t2,					// 
	DT_VECTOR		**MrFactor			// Result: 	Vector of Meanrev factors before being exponentiated in  GnfMrFactors_bare
);


DLLEXPORT int GnfMrFactors_bare(
	GNF_MR_BOTH			*MrBoth,		   		// 
	int				n_fact,				
	DT_TIME			t1,					// Want Meanrev factor from time t1 to time t2
	DT_TIME			t2,					// 
	DT_VECTOR		**MrFactor			// Result: 	Vector of Meanrev factors 
);


DLLEXPORT int GnfMrFactors1As(
	GNF_MODEL_1AS	*Model1As,		   		// 
	DT_TIME			t1,					// Want Meanrev factor from time t1 to time t2
	DT_TIME			t2,					// 
	DT_VECTOR		**MrFactor			// Result: 	Vector of Meanrev factors
);


DLLEXPORT int GnfVolCorrectFactors_bare(
	GNF_EXP_VC		*ExpVc,
	int				n_fact,
	DT_TIME			T,
	DT_VECTOR		**VcFact			// Result: Vector of Meanrev factors
);


DLLEXPORT int GnfVolCorrectFactors1As(
	GNF_MODEL_1AS	*Model1As,		   		// 
	DT_TIME			T,
	DT_VECTOR		**VcFact			// Result: Vector of Meanrev factors
);


DLLEXPORT int GnfLogCov1As_bare(
	GNF_COV_MAT		   	*CovMat,		   		// 
	GNF_MR_BOTH				*MrBoth,		   		// 
	GNF_EXP_VC			*ExpVc,
	int					n_fact,
	DT_TIME	   	 		txa,				// 
	DT_TIME				txb,				// 
	DT_TIME	   	 		tf1,				// 
	DT_TIME				tf2,				// 
	double				*Cov_F	   		// Result:     Futures Covs
);


DLLEXPORT int GnfLogCov1As(
	GNF_MODEL_1AS	   	*Model1As,		   		// 
	DT_TIME	   	 		txa,				// 
	DT_TIME				txb,				// 
	DT_TIME	   	 		tf1,				// 
	DT_TIME				tf2,				// 
	double				*Cov_F	   		// Result:     Futures Covs
);


DLLEXPORT int GnfMrFactKern2Asx_V(
	GNF_MR_BOTH	   		*MrBoth1,		   		// 
	GNF_MR_BOTH	   		*MrBoth2,		   		// 
	int					n_fact1,
	int					n_fact2,
	DT_TCURVE			*tX,
	QC_VEC_ARRAY		**MrFactor1,
	QC_VEC_ARRAY		**MrFactor2,
	QC_VEC_ARRAY		**MrKernel12
);


DLLEXPORT int GnfFactorCovs2Asx(
	GNF_MODEL_2ASX	*Model2Asx,     		// 
	DT_TIME	   	 	txa,			//
	DT_TIME			txb,			// 
	DT_VECTOR		**Cov	   		//  Result:    Factor Covs
);


DLLEXPORT int GnfLogCov2Asx(
	GNF_MODEL_2ASX		*Model2Asx,		   		// 
	DT_TIME	   	 		txa,				// 
	DT_TIME				txb,				// 
	DT_TIME	   	 		tf1,				// 
	DT_TIME				tf2,				// 
	double				*Cov_F	   		// Result:     Futures Cov
);

DLLEXPORT int GnfLogCov1As_SM_Avg(
	GNF_MODEL_1AS		*Model1As,		// 
	DT_TIME				PriceTime,		// Pricing Time
	DT_TCURVE      		*SampT_Weight,	// 
	DT_ARRAY			*VolEndT,   	// 
	DT_CURVE			*SampD_Price,	// 
	DT_TIME				ExpTime,		// expiration date
	double				*LogVarAvg  	// Result: (Log) Covariance Avg
);

DLLEXPORT int GnfLogCov1As_SM_Matrix(
	GNF_MODEL_1AS		*Model1As,		// 
	DT_TIME				PriceTime,		// Pricing Time
	DT_TCURVE      		*SampT_Weight,	// 
	DT_ARRAY			*VolEndT,   	// 
	DT_CURVE			*SampD_Price,	// 
	DT_TIME				ExpTime,		// expiration date
	DT_MATRIX			**LogVarAvg  	// Result: (Log) Covariance Avg
);

DLLEXPORT int GnfLogCov1As_SM_Daily(
	GNF_MODEL_1AS		*Model1As,		// 
	DT_TIME				PriceTime,		// Pricing Time
	DT_TCURVE      		*SampT_Weight,	// 
	DT_ARRAY			*VolEndT,   	// 
	DT_CURVE			*SampD_Price,	// 
	DT_TIME				ExpTime,		// expiration date
	DT_CURVE			**LogVarAvg  	// Result: (Log) Covariance Avg
);

DLLEXPORT int GnfLogCov1As_SM(
	GNF_MODEL_1AS		*Model1As,		// 
	DT_TIME				PriceTime,		// Pricing Time
	DT_TCURVE      		*SampT_Weight,	// Curve containing fixing dates
	DT_ARRAY			*VolEndT,   	// Curve containing futures expiration times
	DT_CURVE			*SampD_Price,	// futures prices
	DT_TIME				ExpTime,		// expiration date
	void				**RetValPtr,  	// for returning stuff
	DT_DATA_TYPE		RetValType		// What are we returning??
);


DLLEXPORT int GnfMrFactKern2Asx_bare(
	GNF_MR_BOTH			*MrBoth1,		   		// 
	GNF_MR_BOTH			*MrBoth2,		   		// 
	int					n_fact1,			//
	int					n_fact2,			//
	DT_TIME	   	 		txa,				// Want Meanrev factors and kernels from time txa to time txb
	DT_TIME				txb,				// 
	DT_VECTOR			**MrFactor1,	   	// Results: Vector of MeanRev factors
	DT_VECTOR			**MrFactor2,	   	//          Vector of MeanRev factors
	DT_VECTOR			**MrKernel12	   	//       	Vector of MeanRev kernels
);


DLLEXPORT int GnfCalcAll1As_sub(
	GNF_COV_MAT		   	*CovMat,		   		// 
	GNF_MR_BOTH		   	*MrBoth,		   		// 
	GNF_EXP_VC			*ExpVc,
	int					n_fact,
	DT_TIME	   	 		txa,				// 
	DT_TIME				txb,				// 
	DT_TIME	   	 		tf1,				// 
	DT_TIME				tf2,				// 
	DT_VECTOR			**FactCovs,
	DT_VECTOR			**B_b1,
	DT_VECTOR			**B_b2,
	DT_VECTOR			**g_F1,
	DT_VECTOR			**g_F2,
	double				*Cov_F	   		// Result:     Futures Cov
);


DLLEXPORT int GnfCalcAll1As(
	GNF_MODEL_1AS		*Model1As,		   		// 
   	DT_TIME	   	 		txa,			// 
	DT_TIME				txb,			// 
	DT_TIME	   	 		tf1,			// 
	DT_TIME				tf2,			// 
	DT_VECTOR			**FactCovs,		//
	DT_VECTOR			**B_b1,			//
	DT_VECTOR			**B_b2,			//
	DT_VECTOR			**g_F1,			//
	DT_VECTOR			**g_F2,			//
	double				*Cov_F	   		// 
);


DLLEXPORT int GnfCalcAll1As_Exp1T(
	GNF_MODEL_1AS		*Model1As,		// 
   	DT_TIME	   	 		txa,			// 
	DT_TIME				txb,			// 
	DT_TIME	   	 		tf,			// 
	DT_VECTOR			**MrFactor,			//
	DT_VECTOR			**VolCorrect,			//
	double				*LogCov	   		// 
);

DLLEXPORT int GnfLogCov2Asx_SM_Avg(
	GNF_MODEL_2ASX		*Model2Asx,		   	// Gnf Model
	DT_TIME				PriceTime,		// Pricing Time
	DT_TCURVE  			*SampT_Weight1,   	// 
	DT_TCURVE      		*SampT_Weight2,   	// 
	DT_ARRAY			*VolEndT1,  		// 
	DT_ARRAY			*VolEndT2,  		// 
	DT_CURVE			*SampD_Price1,		// 
	DT_CURVE			*SampD_Price2,		// 
	DT_TIME				ExpTime,		// expiration date
	double				*LogVarAvg  	// Result: (Log) Covariance Avg
);

DLLEXPORT int GnfLogCov2Asx_SM_Matrix(
	GNF_MODEL_2ASX		*Model2Asx,		   	// Gnf Model
	DT_TIME				PriceTime,		// Pricing Time
	DT_TCURVE  			*SampT_Weight1,   	// 
	DT_TCURVE      		*SampT_Weight2,   	// 
	DT_ARRAY			*VolEndT1,  		// 
	DT_ARRAY			*VolEndT2,  		// 
	DT_CURVE			*SampD_Price1,		// 
	DT_CURVE			*SampD_Price2,		// 
	DT_TIME				ExpTime,		// expiration date
	DT_MATRIX			**LogVarAvg  	// Result: (Log) Covariance Matrix
);

DLLEXPORT int GnfLogCov2Asx_SM_Daily(
	GNF_MODEL_2ASX		*Model2Asx,		   	// Gnf Model
	DT_TIME				PriceTime,		// Pricing Time
	DT_TCURVE  			*SampT_Weight1,   	// 
	DT_TCURVE      		*SampT_Weight2,   	// 
	DT_ARRAY			*VolEndT1,  		// 
	DT_ARRAY			*VolEndT2,  		// 
	DT_CURVE			*SampD_Price1,		// 
	DT_CURVE			*SampD_Price2,		// 
	DT_TIME				ExpTime,		// expiration date
	DT_CURVE			**LogVarAvg  	// Result: (Log) Covariance Curve
);

DLLEXPORT int GnfLogCov2Asx_SM(
	GNF_MODEL_2ASX		*Model2Asx,		   	// Gnf Model
	DT_TIME				PriceTime,			// pricing time
	DT_TCURVE  			*SampT_Weight1,   	// 
	DT_TCURVE      		*SampT_Weight2,   	// 
	DT_ARRAY			*VolEndT1,  		// 
	DT_ARRAY			*VolEndT2,  		// 
	DT_CURVE			*SampD_Price1,		// 
	DT_CURVE			*SampD_Price2,		// 
	DT_TIME				ExpTime,			// expiration date
	void				**RetValPtr,  		// for returning stuff
	DT_DATA_TYPE		RetValType			// What are we returning??
);


#endif