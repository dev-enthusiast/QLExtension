/* #define GSCVSID "$Header: /home/cvs/src/qcommod/src/qc_ut.h,v 1.9 2000/06/28 22:45:59 brunds Exp $" */
/****************************************************************
**
**	qc_ut.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_QC_UT_H )
#define IN_QC_UT_H


//#include <qcommod.h>
//#include <gnf_q.h>
//#include <gnf_ias.h>
//#include <gnf_clb.h>
//#include <hash.h>

		   
// Local Exports


DLLEXPORT int LookAtMatrix(
	DT_MATRIX	*m
);


DLLEXPORT int Vector2ColumnInMatrix(
	DT_VECTOR	*vector,
	DT_MATRIX	*matrix,
	int			col
);


DLLEXPORT int VectorFromColumnInMatrix(
	DT_VECTOR	*vector,
	DT_MATRIX	*matrix,
	int			col
);	


DLLEXPORT int QcMergeCurvesGiveIndices_bare(
	int				n_curves,
	DT_CURVE		**curves,		//
	DT_CURVE		**MergedCurve,	// RESULT: 
	QC_VEC_ARRAY	**IndexVectors
);


DLLEXPORT int QcMergeCurvesGiveIndices(
	QC_CRV_ARRAY	*CrvArray,		//
	DT_CURVE		**MergedCurve,	// RESULT: 
	QC_VEC_ARRAY	**IndexVectors	//
);


DLLEXPORT int QcMergeCurvesGiveIndices(
	QC_CRV_ARRAY	*CrvArray,		//
	DT_CURVE		**MergedCurve,	// RESULT: 
	QC_VEC_ARRAY	**IndexVectors	//
);


DLLEXPORT int QcMergeTCurvesGiveIndices(
	QC_TCRV_ARRAY	*TCrvArray,		//
	DT_TCURVE		**MergedTCurve,	// RESULT: 
	QC_VEC_ARRAY	**IndexVectors	//
);
  

DLLEXPORT int QMatrix2VecArray(
	Q_MATRIX		*Qmatrix,
	QC_VEC_ARRAY	**VecArray
);


DLLEXPORT int DtVector2QVector(
	DT_VECTOR		*DtVectorVar,
	Q_VECTOR		*QVector
);


DLLEXPORT int DtMatrix2QMatrix(
	DT_MATRIX		*DtMatrixVar,
	Q_MATRIX		*QMatrix
);


DLLEXPORT int QcDateToTimeDtArray(
	DT_VALUE	*DateArrayVal,
	const char	*TimeZone,
	int			Hour,
	int			Minute,
	int			Second,
	DT_VALUE	*TimeArrayVal
);

		  
#endif

