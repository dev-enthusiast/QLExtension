/* $Header: /home/cvs/src/supermkt/src/q_new.h,v 1.26 2007/08/16 22:49:13 e13749 Exp $ */
/****************************************************************
**
**	Q_NEW.H	
**	
**	Export header for some homeless quant functions.
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.26 $
**
****************************************************************/

#if !defined( IN_Q_NEW_H )
#define IN_Q_NEW_H

#include <qc_q.h>
#include <gnf_clb.h>
#include <gnf_q.h>
#include <datatype.h>

// Exports


DLLEXPORT int 
Q_GnfVolDly1As
(
	DT_TIME			 PT, 
	DT_TIME			 ExpTime,
	int				 n_fact,
	DT_VALUE		*CrvOther,
	DT_CURVE		*CurveEDD,
    GNF_COV_ND		*CovNd,
    GNF_MR_RATE     *MrRate,
    GNF_EXP_VC      *ExpVc0,
    GNF_SH_REF      *ShRef,
	DT_CURVE		*DataCrvOpen,
	DT_CURVE		*WghtCrvOpen,
	DT_CURVE		*SampCrvOpen,
	DT_CURVE	   **Vols
);


DLLEXPORT int 
Q_GnfVolAvg1As
(
	DT_TIME			 PT, 
	DT_TIME			 ExpTime,
	int				 n_fact,
	DT_VALUE		*CrvOther,
	DT_CURVE		*CurveEDD,
    GNF_COV_ND		*CovNd,
    GNF_MR_RATE     *MrRate,
    GNF_EXP_VC      *ExpVc0,
    GNF_SH_REF      *ShRef,
	double			*Vol
);



DLLEXPORT int
Q_GnfNominalPriceDly1As
(
	DT_TIME			 PT, 
	DT_TIME			 ExpTime,
	int				 n_fact,
	DT_VALUE		*CrvOther,
	DT_CURVE		*CurveEDD,
    GNF_COV_ND		*CovNd,
    GNF_MR_RATE     *MrRate,
    GNF_EXP_VC      *ExpVc0,
    GNF_SH_REF      *ShRef,
	DT_CURVE		*DataCrvOpen,
	DT_CURVE		*WghtCrvOpen,
	DT_CURVE		*SampCrvOpen,
	DT_VALUE		*Strikes,
	char			*OptType,
	double			*price
);



DLLEXPORT int
Q_GenBasketCollapseMatrix
(
    double    Today,
    double    ExpDate,
    DT_ARRAY  *Weights,
    double    *Prices,
    DT_MATRIX *CovMatrix,
    double    *LVolAvg,
    double    *SVolAvg,
    double    *RhoAvg
);

DLLEXPORT double
Q_GenBasketComputeSigma
(
    double T,
    double Numer,
    double Fwd
);

DLLEXPORT double
Q_GenBasketComputeRho
(
    double T,
    double LVol,
    double SVol,
    double XNumer,
    double LFwd,
    double SFwd
);

DLLEXPORT double
Q_GenBasketComputeCov
(
    double LVol,
    double SVol,
    double Rho
);


DLLEXPORT int
Q_GenBasketForwardsDaily
(
    int      NumSides,
    DT_ARRAY *BasketWeights,
    DT_CURVE **DataCurvesOpen,
    DT_CURVE **LongCurveSum,
    DT_CURVE **ShortCurveSum
);

DLLEXPORT DT_CURVE *
Q_CovarianceDaily
(
    DT_CURVE  *SampCurve,
    DT_MATRIX *Matrix,
    double    Today
);

DLLEXPORT double
Q_CovarianceAvg
(
    DT_CURVE  *WeightCurveX,
    DT_CURVE  *WeightCurveY,
    DT_CURVE  *PriceCurveX,
    DT_CURVE  *PriceCurveY,
    DT_MATRIX *Matrix,
    double    ExpDate,
    double    Today
);


DLLEXPORT double
Q_GenBasketComputeCovariance
(
    double T,
    double Numer,
    double Fwd1Fwd2
);

DLLEXPORT double
Q_GenBasketComputeVariance
(
    double T,
    double Numer,
    double Fwd1Fwd2
);

DLLEXPORT DT_CURVE *
Q_DailyCrossVol
(
    DT_CURVE **DataCurves,
    DT_CURVE **WeightCurves,
    DT_CURVE **SampCurvesOpen,
    double   Today,
    double   ExpDate,
    DT_ARRAY *BasketWeights,
    double   VolAdjL,
    double   VolAdjS,
    double   RhoAdj,
    double   NumSides,
    DT_CURVE **CovarianceCurves
);

DLLEXPORT DT_CURVE *
Q_InstVolsFromTermVols
(
    double   Today,
    DT_CURVE *TermVols
);

#endif






