/* $Header: /home/cvs/src/supermkt/src/br2f_ut.h,v 1.5 1999/12/15 16:33:45 goodfj Exp $ */
/****************************************************************
**
**	BR2F_UT.H	
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
****************************************************************/

#if !defined( IN_BR2F_UT_H )
#define IN_BR2F_UT_H


#if !defined( IN_MEANREV_H )
#include <meanrev.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#if !defined( IN_Q_BR2F_H )
#include <q_br2f.h>
#endif
#if !defined( IN_HASH_H )
#include <hash.h>
#endif

// Exports


DLLEXPORT void LoadBeta1Asset( HASH *BetaInfo, BR2F_BETA *beta );

DLLEXPORT void LoadBeta2Assets( HASH *BetaInfoBoth, BR2F_BETA_2ASSETS *betaBoth );

DLLEXPORT void LoadVols1Asset( HASH *Br2fModelPars, BR2F_MODEL *br2f );

DLLEXPORT void LoadVols2Assets( HASH *Br2fModelParsBoth, BR2F_MODEL_2ASSETS *br2fBoth );

DLLEXPORT void LoadModifiedCurves( HASH *ModifiedCurvesIn, XVBR2F_MODCRVS *ModifiedCurvesOut );

DLLEXPORT int Load_Vector( DT_ARRAY *array, Q_VECTOR *vector );

DLLEXPORT int Load_Curve_Vector( DT_ARRAY *array, MR_CURVES_VECTOR *Cvector );


DLLEXPORT int MaxFromArray( int *a, int len );


DLLEXPORT void BR2fAlloc( BR2F_MODEL *br2f, int n );

DLLEXPORT void BR2fPartialAllocNew( BR2F_MODEL *br2f, int n );

DLLEXPORT void BR2fFree( BR2F_MODEL *br2f );

DLLEXPORT void BR2f2AssetsPartialAlloc( BR2F_MODEL_2ASSETS *br2fboth, int n );

DLLEXPORT void BR2f2AssetsPartialAllocNew( BR2F_MODEL_2ASSETS *br2fboth, int n );

DLLEXPORT void BR2f2AssetsFree( BR2F_MODEL_2ASSETS *br2fboth );


DLLEXPORT double **Alloc2DArray( int nx, int ny );

DLLEXPORT void Free2DArray( double **T, int nx );

DLLEXPORT double ***Alloc3DArray( int nx, int ny, int nz );

DLLEXPORT void Free3DArray( double ***T, int nx, int ny );

DLLEXPORT int **Alloc2DArrayInt( int nx, int ny );

DLLEXPORT void Free2DArrayInt( int **T, int nx );


#endif
