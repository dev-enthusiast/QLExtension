/* #define GSCVSID "$Header: /home/cvs/src/qcommod/src/qc_afrw.h,v 1.10 2000/03/09 03:16:54 brunds Exp $" */
/****************************************************************
**
**	qc_afrw.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_QC_AFRW_H )
#define IN_QC_AFRW_H


#include <qc_q.h>
			   
			      
// Constants


// DllExports

DLLEXPORT QC_TCRV_ARRAY* AllocLiteQcTCrvArray(
	int		count
);


DLLEXPORT int FreeQcTCrvArray(
	QC_TCRV_ARRAY	*TCrvArray,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT QC_VEC_ARRAY* AllocLiteQcVecArray(
	int		count
);


DLLEXPORT int FreeQcVecArray(
	QC_VEC_ARRAY	*VecArray,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int ReadQcVecArray(
	DT_VALUE  		*ValIn,
	QC_VEC_ARRAY   **VecArray
);


DLLEXPORT int WriteQcVecArray(
	QC_VEC_ARRAY	*VecArray,
	DT_VALUE		*ValOut
);


DLLEXPORT int FreeQcVecArr2d(
	QC_VEC_ARR2D	*VecArr2d,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int ReadQcVecArr2d(
	DT_VALUE  		*ValIn,
	QC_VEC_ARR2D   **VecArr2d
);


DLLEXPORT int ReadQcTCrvArray(
	DT_VALUE  		*ValIn,
	QC_TCRV_ARRAY   **TCrvArray
);


DLLEXPORT QC_VEC_ARRAY* AllocQcVecArray(
	int		count,
	int		size
);


DLLEXPORT int FreeQcCrvArray(
	QC_CRV_ARRAY	*CrvArray,		// model structure to be free'd
	int				WhatToFree
);


DLLEXPORT int ReadQcCrvArray(
	DT_VALUE  		*ValIn,
	QC_CRV_ARRAY    **CrvArray
);


DLLEXPORT QC_VEC_ARR2D* AllocLiteQcVecArr2d(
	int		CountHi,
	int		CountLo
);


DLLEXPORT QC_VEC_ARR2D* Alloc1LevelQcVecArr2d(
	int		count
);


DLLEXPORT int FreeQcMcPre(
	QC_MC_PRECALC	*McPre		// model structure to be free'd
);


DLLEXPORT int FreeQcMcSave(
	QC_MC_SAVE	*McSave			// model structure to be free'd
);


DLLEXPORT int FreeQcGridPre(
	QC_GRID_PRECALC	*GridPre		// model structure to be free'd
);


#endif

