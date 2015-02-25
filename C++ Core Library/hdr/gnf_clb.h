/* #define GSCVSID "$Header: /home/cvs/src/qcommod/src/gnf_clb.h,v 1.22 2007/08/16 15:44:42 e13749 Exp $" */
/****************************************************************
**
**	gnf_clb.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.22 $
**
****************************************************************/

#if !defined( IN_GNF_CLB_H )
#define IN_GNF_CLB_H

#include	<qc_q.h>
#include	<gnf_q.h>

// ------------------------------------------------------------------------------------
// TypeDefs
// ------------------------------------------------------------------------------------


typedef struct
{
	int					ExtendFlag,
						SelfTestFlag;
	double				SelfTest_MaxError;
	int					VcClbBackTo1Flag;
	double				VcClbBackTo1_Years;

} GNF_CLB_INFO; // 'ClbInfo'


typedef struct						
{
	DT_TCURVE		    *t;	   		// [im]    
	DT_MATRIX		    *Rate;      // [f][im] 	

} GNF_MR_RATE; // 'MrRate'


typedef struct
{
	int					n_fact;				// number of model factors
	DT_MATRIX			*FactDiagCoeff;		// Model Diagonalization Coeffs for FACTORS
	DT_VECTOR           *MrRate,			// Meanrev Rates
						*Vol,           	// 'Initial' Values for NON-DIAGONAL Local Vols (i.e. calibration goes from there)
						*Corr;				// 'Initial' Values for NON-DIAGONAL Local Corrs (i.e. calibration goes from there)
//	DT_VECTOR			*VcFact;			// 'Initial' Expiry VolCorrect Factors (calibrate from there)
	double		   		RefBase,			// Base Value for Reference Price (shifted lognormal)
						RefDivisor;			// Divisor for Reference Price

} GNF_ALL1_SPEC_FLAT; // 'All1SpFl'


typedef struct			
{
	int					n_fact;				// number of model factors
	DT_MATRIX			*FactDiagCoeff;		// Model Diagonalization Coeffs for FACTORS
	DT_VECTOR           *MrRate,			// Meanrev Rates
						*Vol,           	// 'Initial' Values for NON-DIAGONAL Local Vols (i.e. calibration goes from there)
						*Corr;				// 'Initial' Values for NON-DIAGONAL Local Corrs (i.e. calibration goes from there)

} GNF_FWDR_SPEC_FLAT; // 'FwdrSpFl'  [specs for FORWARD interest rate model]


typedef struct						
{								   	
	DT_MATRIX		    *VolChg;		// [f][ic] 	
	double				ClbLocMin;			// 

} GNF_COV_CHG;	// 'CovChg'	


typedef struct
{
	DT_VECTOR           *VolChg;			// Local Calibration Vector (RELATIVE to base values)
	double				ClbLocMin;			// Minimum allowed value for Local Calibration Result

} GNF_COV_CHG_FLAT; // 'CovChgFl'

								   
typedef struct						
{								   	
	int					n_fact1,
						n_fact2;
	DT_TCURVE		    *t;				// [ic]    	
	DT_MATRIX		    *Corr;			// [c'][ic]

} GNF_CORR_ND; // 'CorrNd'   	


typedef struct						
{								   	
	int					n_fact1,
						n_fact2;
	DT_VECTOR		    *Corr;			// [c']

} GNF_CORR_ND_FLAT; // 'CorrNdFl'   	


typedef struct						
{								   	
	double				ClbMin,			//
						ClbMax;			// 
	DT_MATRIX		    *CorrChg;		// [f][ic] 	

} GNF_CORR_CHG; // 'CorrChg' 	


typedef struct						
{								   	
	double				ClbMin,			//
						ClbMax;			// 
	DT_VECTOR		    *CorrChg;		// [f]

} GNF_CORR_CHG_FLAT; // 'CorrChg' 	


// ------------------------------------------------------------------------------------
// Exports
// ------------------------------------------------------------------------------------


DLLEXPORT int GnfCalibrStep1As(
	int					n_fact,			//
	DT_MATRIX			*Vol0,			// index IV[i] valid for step #i (B_BE, H_BE, B_EU must be aligned with that)
	DT_MATRIX			*Corr,			//
	DT_MATRIX			*VolChg,		//
	double				ClbLocMin,		//
	DT_VECTOR			*IndexGrid,		//
	double				var_impl,		// market implied variance to calibrate to		
	DT_MATRIX			*CovDiagCoeff,	//
   	QC_VEC_ARRAY		*B_BE,          //         
	QC_VEC_ARRAY		*H_BE,          //         
	DT_VECTOR			*B_EU,          //         
    DT_VECTOR			*VcFact0,		//
	DT_VECTOR			*CovTermBegin,	//
	DT_VECTOR			**CovTermEnd,	// RESULTS:	new term covs
	double				*ClbLocRes,		//          calibrated short vol for this interval
	double				*ClbVcRes		//   	    futures VolCorrect factor
);


DLLEXPORT int GnfBldBlk_CovMat(
	int					n_fact,			// number of model factors
	DT_MATRIX			*FactDiagCoeff,	//
	DT_MATRIX			*Vol0,			//
	DT_MATRIX			*Corr,			//
	DT_MATRIX			*VolChg,		//
	QC_VEC_ARRAY		*IndexGrid,	// size MUST agree with ClbLocRes (but can't check)
	QC_TCRV_ARRAY		*TimeGrid,		// size MUST agree with ClbLocRes 
	DT_TIME				MktDataTime,	// need this in case the input data have 0 length
	DT_VECTOR  			*ClbLocRes,	   	// 
	DT_MATRIX			*CovDiagCoeff,	//
	QC_VEC_ARR2D		*B_BE,         // 
	QC_VEC_ARR2D 		*H_BE,         // 
	GNF_COV_MAT			**CovMat		   	//
);


DLLEXPORT int GnfBldBlk_MrBoth(
	int					n_fact,			// number of model factors
	DT_TCURVE			*Mr_t,		 	// time grid for MrRate
	DT_MATRIX			*MrRate,		// Rate[i] valid from t[i] to t[i+1]
	GNF_MR_BOTH			**MrBoth		// RESULT
);


DLLEXPORT int GnfBldBlk_ExpVc(
	int					n_fact,				// number of model factors
	DT_TCURVE			*Vc0_t,		 		//
	DT_MATRIX			*Vc0_Fact,	  		//
	DT_TCURVE			*ClbVcRes,			// 
	int					VcClbBackTo1Flag,	// if this is set, bring Vc Calibration back to 1 if last calibration is != 1
	double				VcClbBackTo1_Years,	// number of years we take to move VC Factor calibration back
	GNF_EXP_VC			**ExpVc		   		// Result: Gnf model structure

);


DLLEXPORT int GnfBldBlk_ShRef(
	DT_TCURVE			*RefBase,
	double				RefDivisor,
	GNF_SH_REF			**ShRef	  
);


DLLEXPORT int GnfCalibrate1As_bare(
	int					n_fact,			//
	GNF_COV_ND 			*CovNd0,		//
	GNF_COV_CHG  		*CovChg,		// uses time grid given by CovNd; may be EMPTY if no calibration occurs !!!
	GNF_MR_BOTH 		*MrBoth,			//
    GNF_EXP_VC			*ExpVc0,			//
	GNF_CLB_INFO		*ClbInfo,		//
	DT_TIME				MktDataTime,	// Market Data Time
	DT_TCURVE			*MktVariances,		// Market implied Variances from MktDataTime (vs. Option Expirys)
	DT_TCURVE			*MktUndExp,		// Underlying expirys
	GNF_COV_MAT			**CovMat,
	GNF_EXP_VC			**ExpVc,
	int					*CalibrSteps,
	int					*VcUsageFlag    //   let caller know that fudge factor was triggered
);


DLLEXPORT int GnfCalibrate1As_General(
	int					n_fact,			//
	GNF_COV_ND 		*CovNd,		//
	GNF_COV_CHG  		*CovChg,		// uses time grid given by CovNd !!
	GNF_MR_RATE 		*MrRate,			//
    GNF_EXP_VC			*ExpVc0,			//
	GNF_SH_REF			*ShRef,			//
	GNF_CLB_INFO		*ClbInfo,		//
	DT_TIME				MktDataTime,	// Market Data Time
	DT_TCURVE			*Variances,		// Market implied Variances from MktDataTime (vs. Option Expirys)
	DT_TCURVE			*UndExp,		// Underlying expirys
	GNF_MODEL_1AS		**Model1As,		   	// Result: Gnf model structure
	int					*VcUsageFlag    //   let caller know that fudge factor was triggered
);
  

DLLEXPORT int GnfGenFromFlat_All1Sp(
	GNF_ALL1_SPEC_FLAT	*All1SpFl,		//
	double			  	ModelYears,		//
	DT_TIME				MktDataTime,	// Market Data Time
	GNF_COV_ND 		**CovNd,		//
	GNF_MR_RATE 		**MrRate,			//
    GNF_EXP_VC			**ExpVc0,			//
	GNF_SH_REF			**ShRef			//
);


DLLEXPORT int GnfGenFromFlat_CovChg(
	int					n_fact,			//
	GNF_COV_CHG_FLAT	*CovChgFl,		//
	GNF_COV_CHG  		**CovChg		// uses time grid given by CovNd !!
);


DLLEXPORT int GnfCalibrate1As_Flat(
	GNF_ALL1_SPEC_FLAT	*All1SpFl,		//
	GNF_COV_CHG_FLAT	*CovChgFl,		//
	double			  	ModelYears,		//
	GNF_CLB_INFO		*ClbInfo,		//
	DT_TIME				MktDataTime,	// Market Data Time
	DT_TCURVE			*Variances,		// Market implied Variances from MktDataTime (vs. Option Expirys)
	DT_TCURVE			*UndExp,		// Underlying expirys
	GNF_MODEL_1AS		**Model1As,		   	// Result: Gnf model structure
	int					*VcUsageFlag    //   let caller know if fudge factor was triggered
);


DLLEXPORT int GnfBuild1As_General(
	int					n_fact,			//
	GNF_COV_ND 		*CovNd,		//
	GNF_MR_RATE 		*MrRate,			//
    GNF_EXP_VC			*ExpVc0,			//
	GNF_SH_REF			*ShRef,			//
	DT_TIME				MktDataTime,	// Market Data Time
	GNF_MODEL_1AS		**Model1As		   	// Result: Gnf model structure
);


DLLEXPORT int GnfBuild1As_Flat(
	GNF_ALL1_SPEC_FLAT	*All1SpFl,		//
	double			  	ModelYears,		//
	DT_TIME				MktDataTime,	// Market Data Time
	GNF_MODEL_1AS		**Model1As		   	// Result: Gnf model structure
);


DLLEXPORT int GnfCalibrStep2Asx(
	int                 n_fact1,           
	int                 n_fact2,           
	DT_MATRIX           *Vol1,             
	DT_MATRIX           *Vol2,             
	DT_MATRIX           *Corr0,            
	DT_MATRIX           *CorrChg,          
	double              ClbMin,            
	double              ClbMax,            
	DT_VECTOR           *IndexGridVol1,    
	DT_VECTOR           *IndexGridVol2,    
	DT_VECTOR           *IndexGridCorr,    
	double              CovImpl,           
	DT_MATRIX           *FactDiagCoeff1,   
	DT_MATRIX           *FactDiagCoeff2,   
	QC_VEC_ARRAY       *B_BE1,            
	QC_VEC_ARRAY       *B_BE2,            
	QC_VEC_ARRAY       *H_BE12,           
	DT_VECTOR           *B_EU1,            
	DT_VECTOR           *B_EU2,            
	DT_VECTOR           *VcFact1,          
	DT_VECTOR           *VcFact2,          
	DT_VECTOR           *CovTermBegin,     
	DT_VECTOR           **CovTermEnd,      
	double              *ClbRes,           
	int                 *OutOfBoundsFlag,  
	int                 *DegenCorrFlag     
);


DLLEXPORT int GnfModelVerify2Asx(
	GNF_MODEL_2ASX		*Model2Asx,		   	
	GNF_MODEL_1AS		*Model1As1,		   	
	GNF_MODEL_1AS		*Model1As2,		   	
	DT_TIME				MktDataTime,	
	int					n_test_in,	   	
	DT_TCURVE			*Covariances,		
	DT_TCURVE			*UndExp1,		
	DT_TCURVE			*UndExp2,		
	double				MaxError		
);


DLLEXPORT int GnfCalibrate2Asx_General(
	GNF_MODEL_1AS		*Model1As1,			//
	GNF_MODEL_1AS		*Model1As2,			//
	GNF_CORR_ND 		*CorrNd,		//
	GNF_CORR_CHG 		*CorrChg,		//
	GNF_CLB_INFO		*ClbInfo,		//
	DT_TIME				MktDataTime,	
	DT_TCURVE			*MktCorrs,		// Market implied Variances from MktDataTime (vs. Option Expirys)
	DT_TCURVE			*MktUndExp1,   	// Underlying expirys
	DT_TCURVE			*MktUndExp2,   	// Underlying expirys
	GNF_MODEL_2ASX		**Model2Asx,		   	// Result: Gnf model structure
	int					*BadCorrFlag,	// 		flag indicating 'bad' ( outside specified bounds ) local correlation (yes = TRUE) resulting in this calibration step
	int					*DegenForrFlag	// 		flag indicating 'degenerate' ( ill determined )    local correlation (yes = TRUE) resulting in this calibration step
);
		  

DLLEXPORT int GnfCalibrate2Asx_Flat(
	GNF_MODEL_1AS		*Model1As1,			//
	GNF_MODEL_1AS		*Model1As2,			//
	GNF_CORR_ND_FLAT	*CorrNdFl,		//
	GNF_CORR_CHG_FLAT	*CorrChgFl,		//
	double			  	ModelYears,		//
	GNF_CLB_INFO		*ClbInfo,		//
	DT_TIME				MktDataTime,	// Market Data Time
	DT_TCURVE			*MktCorrs,		// Market implied Variances from MktDataTime (vs. Option Expirys)
	DT_TCURVE			*MktUndExp1,		// Underlying expirys
	DT_TCURVE			*MktUndExp2,		// Underlying expirys
	GNF_MODEL_2ASX		**Model2Asx,		   	// Result: Gnf model structure
	int					*OutOfBoundsFlag,	// 		flag indicating 'bad' ( outside specified bounds ) local correlation (yes = TRUE) resulting in this calibration step
	int					*DegenCorrFlag	// 		flag indicating 'degenerate' ( ill determined )    local correlation (yes = TRUE) resulting in this calibration step
);


DLLEXPORT int GnfBuild2Asx_General(
	GNF_MODEL_1AS		*Model1As1,			//
	GNF_MODEL_1AS		*Model1As2,			//
	GNF_CORR_ND   	*CorrNd,		//
	DT_TIME				MktDataTime,	// Market Data Time
	GNF_MODEL_2ASX		**Model2Asx		   	// Result: Gnf model structure
);


DLLEXPORT int GnfBuild2Asx_Flat(
	GNF_MODEL_1AS		*Model1As1,			//
	GNF_MODEL_1AS		*Model1As2,			//
	GNF_CORR_ND_FLAT	*CorrNdFl,		//
	double			  	ModelYears,		//
	DT_TIME				MktDataTime,	// Market Data Time
	GNF_MODEL_2ASX		**Model2Asx		   	// Result: Gnf model structure
);


DLLEXPORT GNF_MR_BOTH* GnfBldBlkTrivial1_MrBoth(
	DT_TIME		MktDataTime,
	DT_TIME		ModelEndTime
);


DLLEXPORT GNF_EXP_VC* GnfBldBlkTrivial_ExpVc(
	int			n_fact,
	DT_TIME		MktDataTime,
	DT_TIME		ModelEndTime
);


DLLEXPORT GNF_SH_REF* GnfBldBlkTrivial_ShRef(
	DT_TIME		MktDataTime,
	DT_TIME		ModelEndTime
);


DLLEXPORT int GnfModelVerifyCcyIr(
	GNF_MODEL_1AS		*Model_Ccy,		   	
	GNF_MODEL_1AS		*Model_Ir1,		   	
	GNF_MODEL_1AS		*Model_Ir2,		   	
	GNF_MODEL_2ASX		*Model_CcyIr1,		   	
	GNF_MODEL_2ASX		*Model_CcyIr2,		   	
	GNF_MODEL_2ASX		*Model_Ir1Ir2,		   	
	DT_TIME				MktDataTime,	
	int					n_test_in,	   	
	DT_TCURVE			*Variances,		
	DT_TCURVE			*UndExp,
	double				MaxError		
);


DLLEXPORT GNF_COV_CHG* GnfBldBlkExtract1_CovChg(
	int				index,
	GNF_COV_ND		*CovNd
);


DLLEXPORT GNF_COV_ND* GnfBldBlkTrivial1_CovNd(
	DT_TIME		MktDataTime,
	DT_TIME		ModelEndTime
);


DLLEXPORT int GnfAll1SpFlFromFwdr(
	GNF_FWDR_SPEC_FLAT	*FwdrSpFl,		//
	GNF_ALL1_SPEC_FLAT	**All1SpFl		//
);


DLLEXPORT int GnfGenFromFlat_CorrNd(
	GNF_CORR_ND_FLAT	*CorrNdFl,		//
	double			  	ModelYears,		//
	DT_TIME				MktDataTime,	// Market Data Time
	GNF_CORR_ND 		**CorrNd		// RESULT
);


DLLEXPORT int GnfGenFromFlat_CorrChg(
	int					n_fact1,		//
	int					n_fact2,		//
	GNF_CORR_CHG_FLAT	*CorrChgFl,		//
	GNF_CORR_CHG  		**CorrChg		// RESULT 
);


DLLEXPORT int GnfCalibrate1As_Trivial(
	double			  	ModelYears,		//
	GNF_CLB_INFO		*ClbInfo,		//
	DT_TIME				MktDataTime,	// Market Data Time
	DT_TCURVE			*MktVariances,	// Market implied Variances from MktDataTime (vs. Option Expirys)
	DT_TCURVE			*MktUndExp,		// Underlying expirys
	GNF_MODEL_1AS		**Model1As,		// Result: Gnf model structure
	int					*VcUsageFlag    //   let caller know if fudge factor was triggered
);


DLLEXPORT int GnfCalibrCcyThruIr_General(
	GNF_MODEL_1AS		*Model_Ir1,   	//
	GNF_MODEL_1AS		*Model_Ir2,	    //
    GNF_CORR_ND			*CorrNd_CcyIr1,	//
    GNF_CORR_ND			*CorrNd_CcyIr2,	//
    GNF_CORR_ND			*CorrNd_Ir1Ir2,	//
	GNF_CLB_INFO		*ClbInfo,		//
	DT_TIME				MktDataTime,	// Market Data Time
	DT_TCURVE			*MktVariances,	// Market implied Variances from MktDataTime (vs. Option Expirys)
	DT_TCURVE			*MktUndExp,   	// Underlying expirys
	GNF_MODEL_1AS		**Model_Ccy,	// RESULT: 
	GNF_MODEL_2ASX		**Model_CcyIr1, //  
	GNF_MODEL_2ASX		**Model_CcyIr2, //  
	int					*VcUsageFlag    //   let caller know that fudge factor was triggered
);


DLLEXPORT int GnfBldBlk_CovOdr(                                                         
	int                 n_fact1,           	//
	int                 n_fact2,           	//
	DT_MATRIX           *Vol1,             	//
	DT_MATRIX           *Vol2,             	//
	DT_MATRIX           *Corr0,            	//
	DT_MATRIX           *CorrChg,          	//
	QC_VEC_ARRAY        *IndexGridVol1,		//
	QC_VEC_ARRAY        *IndexGridVol2,		//
	QC_VEC_ARRAY        *IndexGridCorr,		//
	QC_TCRV_ARRAY		*TimeGrid,			//
	DT_TIME				MktDataTime,		//
	DT_VECTOR			*ClbRes,			//
	DT_MATRIX           *FactDiagCoeff1,   	//
	DT_MATRIX           *FactDiagCoeff2,   	//
	QC_VEC_ARR2D        *B_BE1,				//
	QC_VEC_ARR2D        *B_BE2,				//
	QC_VEC_ARR2D        *H_BE12,			//
	GNF_COV_ODR			**CovOdr			// RESULT
);


#endif
														   
