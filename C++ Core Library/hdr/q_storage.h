/* $Header: /home/cvs/src/gridpricer/src/q_storage.h,v 1.1 2001/04/04 19:40:15 terraa Exp $ */
/****************************************************************
**
**	q_storage.h	Exercise functions for grid pricer
**
**	Copyright 2000 - Goldman Sachs & Company - New York
**
**	$Revision: 1.1 $
**	
****************************************************************/

#if !defined( IN_Q_STORAGE_H )
#define IN_Q_STORAGE_H

#if !defined( IN_GRIDPRICER_BASE_H )
#include <gridpricer_base.h>
#endif

EXPORT_C_GRIDPRICER int G_StorageOption(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_StorageOptionV2(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_StorageOptionV3(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_StorageOptionV4(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_NewStorageOption(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_IntrinsicStorage( 
    DT_VALUE         *DealInfo      // Structure containing information needed by exercise function
);

EXPORT_C_GRIDPRICER int G_IntrinsicStorageNew( 
    DT_VALUE         *DealInfo,      // Structure containing information needed by exercise function
    DT_VALUE         *GridSpecs      // Structure containing information needed for general diffusion
);


#endif
