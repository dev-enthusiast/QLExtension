/* #define GSCVSID "$Header: /home/cvs/src/qcommod/src/gnf_afrw.h,v 1.20 2000/07/27 21:34:51 brunds Exp $" */
/****************************************************************
**
**	gnf_afrw.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.20 $
**
****************************************************************/

#if !defined( IN_GNF_AFRW_H )
#define IN_GNF_AFRW_H


#include <hash.h>
#include <qc_q.h>
#include <gnf_q.h>
#include <gnf_clb.h>

  
// 	Constants

	 
		   
// Local Exports


DLLEXPORT GNF_COV_MAT* AllocGnfCovMat(
	int		n_fact,
	int		len_Cov
);


DLLEXPORT int FreeGnfCovMat(
	GNF_COV_MAT		*CovMat,		// model structure to be free'd
	int				WhatToFree
);

				   
DLLEXPORT GNF_MR_BOTH* AllocGnfMrBoth(
	int		n_fact,
	int 	len_Mr
);


DLLEXPORT int FreeGnfMrBoth(
	GNF_MR_BOTH		*MrBoth,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT GNF_EXP_VC* AllocGnfExpVc(
	int		n_fact,
	int 	len_Vc
);


DLLEXPORT int FreeGnfExpVc(
	GNF_EXP_VC		*ExpVc,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT GNF_SH_REF* AllocGnfShRef(
	int 	len_Ref
);


DLLEXPORT int FreeGnfShRef(
	GNF_SH_REF		*ShRef,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT GNF_MODEL_1AS *AllocGnfModel1As(
	int				n_fact,			// number of model factors
	int				len_Mr,			// """"""""" time intervals with constant MeanRev rates
	int				len_Cov,		// """""""""""""""""""""""""""""""""""""" local Covs
	int				len_Vc,  		// """"""""" Expiry times with VolCorrect    factors
	int				len_Ref	  		// """"""""" 
);


DLLEXPORT int FreeGnfModel1As(
	GNF_MODEL_1AS	*Model1As,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int FreeGnfModel2Asx(
	GNF_MODEL_2ASX	*Model2Asx,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int ReadGnfCovMat(
//	HASH  		*HashIn,
	DT_VALUE  		*ValIn,
	GNF_COV_MAT	**CovMat
);


DLLEXPORT int ReadGnfMrBoth(
	DT_VALUE  		*ValIn,
	GNF_MR_BOTH	**MrBoth
);


DLLEXPORT int ReadGnfExpVc(
	DT_VALUE  		*ValIn,
	GNF_EXP_VC		**ExpVc
);


DLLEXPORT int ReadGnfShRef(
	DT_VALUE  		*ValIn,
	GNF_SH_REF		**ShRef
);
 

DLLEXPORT int ReadGnfModel1As(         
	DT_VALUE  		*ValIn,
	GNF_MODEL_1AS   **Model1As
);


DLLEXPORT int ReadGnfAll1SpFl(
	DT_VALUE			*ValIn,
	GNF_ALL1_SPEC_FLAT	**All1SpFl
);


DLLEXPORT int ReadGnfCovChgFl(
	DT_VALUE			*ValIn,
	GNF_COV_CHG_FLAT	**CovChgFl
);


DLLEXPORT int WriteGnfCovMat(
	GNF_COV_MAT		*CovMat,
	DT_VALUE		*ValOut
);


DLLEXPORT int WriteGnfMrBoth(
	GNF_MR_BOTH		*MrBoth,
	DT_VALUE		*ValOut
);


DLLEXPORT int WriteGnfExpVc(
	GNF_EXP_VC     	*ExpVc,
	DT_VALUE		*ValOut
);


DLLEXPORT int WriteGnfShRef(
	GNF_SH_REF		*ShRef,
	DT_VALUE		*ValOut
);


DLLEXPORT int WriteGnfModel1As( 
	GNF_MODEL_1AS	*Model1As,
	DT_VALUE		*ValOut
);


DLLEXPORT int ReadGnfModel2Asx(         
	DT_VALUE  	  	*ValIn, 
	GNF_MODEL_2ASX   **Model2Asx
);


DLLEXPORT int ReadGnfMrRate(
	DT_VALUE  		*ValIn,
	GNF_MR_RATE		**MrRate
);


DLLEXPORT int ReadGnfClbInfo(
	DT_VALUE  		*ValIn,
	GNF_CLB_INFO	**ClbInfo
);


DLLEXPORT int ReadGnfCovNd(
	DT_VALUE  		*ValIn,
	GNF_COV_ND   	**CovNd
);


DLLEXPORT int ReadGnfCovChg(
	DT_VALUE  		*ValIn,
	GNF_COV_CHG   	**CovChg
);


DLLEXPORT GNF_MR_RATE* AllocGnfMrRate(
	int		n_fact,
	int		len_Mr
);


DLLEXPORT GNF_COV_ND* AllocGnfCovNd(
	int		n_fact,
	int		len_Cov
);


DLLEXPORT GNF_COV_CHG* AllocGnfCovChg(
	int		n_fact,
	int		len_Cov
);


DLLEXPORT int FreeGnfMrRate(
	GNF_MR_RATE		*MrRate,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int FreeGnfCovNd(
	GNF_COV_ND	*CovNd,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int FreeGnfCovOdr(
	GNF_COV_ODR		*CovOdr,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int FreeGnfCovChg(
	GNF_COV_CHG		*CovChg,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT GNF_COV_ODR* AllocGnfCovOdr(
	int		n_fact1,
	int		n_fact2,
	int		len_Cov
);


DLLEXPORT GNF_CORR_ND* AllocGnfCorrNd(
	int		n_fact1,
	int		n_fact2,
	int		len_Corr
);


DLLEXPORT int FreeGnfCorrNd(
	GNF_CORR_ND  	*CorrNd,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT GNF_CORR_CHG* AllocGnfCorrChg(
	int		n_fact1,
	int		n_fact2,
	int		len_Corr
);


DLLEXPORT int FreeGnfCorrChg(
	GNF_CORR_CHG  	*CorrChg,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int WriteGnfModel2Asx( 
	GNF_MODEL_2ASX	*Model2Asx,
	DT_VALUE		*ValOut
);


DLLEXPORT int ReadGnfCorrChg(
	DT_VALUE  		*ValIn,
	GNF_CORR_CHG   	**CorrChg
);


DLLEXPORT int FreeGnfCorrNdFl(
	GNF_CORR_ND_FLAT  	*CorrNdFl,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int ReadGnfCorrNdFl(
	DT_VALUE  		*ValIn,
	GNF_CORR_ND_FLAT   	**CorrNdFl
);


DLLEXPORT int FreeGnfCorrChgFl(
	GNF_CORR_CHG_FLAT  	*CorrChgFl,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int ReadGnfCorrChgFl(
	DT_VALUE  		*ValIn,
	GNF_CORR_CHG_FLAT   	**CorrChgFl
);


DLLEXPORT int ReadGnfCorrNd(
	DT_VALUE  		*ValIn,
	GNF_CORR_ND   	**CorrNd
);


DLLEXPORT int ReadGnfCorrNdFl(
	DT_VALUE  		*ValIn,
	GNF_CORR_ND_FLAT   	**CorrNdFl
);


DLLEXPORT GNF_MR_BOTH* CopyGnfMrBoth(
	GNF_MR_BOTH		*MrBoth_from
);


DLLEXPORT GNF_EXP_VC* CopyGnfExpVc(
	GNF_EXP_VC		*ExpVc_from
);


DLLEXPORT GNF_SH_REF* CopyGnfShRef(
	GNF_SH_REF		*ShRef_from
);


DLLEXPORT int ReadGnfFwdrSpFl(
	DT_VALUE		*ValIn,
	GNF_FWDR_SPEC_FLAT	**FwdrSpFl
);

  
DLLEXPORT int WriteGnfAll1SpFl(
	GNF_ALL1_SPEC_FLAT	*All1SpFl,
	DT_VALUE			*ValOut
);
  

DLLEXPORT GNF_ALL1_SPEC_FLAT* AllocGnfAll1SpFl(
	int		n_fact
);


DLLEXPORT int FreeGnfAll1SpFl(
	GNF_ALL1_SPEC_FLAT	*All1SpFl,		// model structure to be free'd
	int					WhatToFree
);


DLLEXPORT int FreeGnfCovChgFl(
	GNF_COV_CHG_FLAT	*CovChgFl,		// model structure to be free'd
	int					WhatToFree
);


DLLEXPORT int WriteGnfCovNd(
	GNF_COV_ND		*CovNd,
	DT_VALUE		*ValOut
);


DLLEXPORT int WriteGnfCovChg(
	GNF_COV_CHG		*CovChg,
	DT_VALUE		*ValOut
);


DLLEXPORT int WriteGnfMrRate(
	GNF_MR_RATE   	*MrRate,
	DT_VALUE		*ValOut
);


DLLEXPORT int WriteGnfCorrNd(
	GNF_CORR_ND		*CorrNd,
	DT_VALUE		*ValOut
);


DLLEXPORT int WriteGnfCorrChg(
	GNF_CORR_CHG		*CorrChg,
	DT_VALUE		*ValOut
);


DLLEXPORT GNF_MODEL_MANY* AllocLiteGnfModelMany(
	int		n_models
);


DLLEXPORT int ReadGnfCovOdr(
	DT_VALUE  	*ValIn,
	GNF_COV_ODR	**CovOdr
);


DLLEXPORT int WriteGnfCovOdr(
	GNF_COV_ODR		*CovOdr,
	DT_VALUE		*ValOut
);


DLLEXPORT int LoadGnfModel1As(
	HASH			*HashIn,
	GNF_MODEL_1AS	**Model1As
);


DLLEXPORT int LoadGnfModel2Asx(
	HASH			*HashIn,
	GNF_MODEL_2ASX	**Model2Asx
);


DLLEXPORT GNF_COV_ODR* CopyGnfCovOdr(
	GNF_COV_ODR		*CovOdr_from
);


DLLEXPORT GNF_MODEL_2ASX* CopyGnfModel2Asx(
	GNF_MODEL_2ASX 		*Model2Asx_from
);


#endif






