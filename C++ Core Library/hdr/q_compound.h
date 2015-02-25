/* $Header: /home/cvs/src/gridpricer/src/q_compound.h,v 1.4 2001/08/31 14:18:24 graysa Exp $ */
/****************************************************************
**
**	q_compound.h	Exercise functions for options on portfolios of basket options
**
**	Copyright 2000 - Goldman Sachs & Company - New York
**
**  $Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_Q_COMPOUND_H )
#define IN_Q_COMPOUND_H

#if !defined( IN_GRIDPRICER_BASE_H )
#include <gridpricer_base.h>
#endif

EXPORT_C_GRIDPRICER double *PortfolioBasketGuts(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE       *DealInfo      // Structure containing information needed by exercise function
);

EXPORT_C_GRIDPRICER double *SingleConditionalBasketValues(
    DT_VECTOR *FutPrices, 
	DT_ARRAY *FutCmpArray, 
	DT_ARRAY *Basket, 
	DT_MATRIX *TermCov,
	Q_GRID_DOMAIN *FactorSpecs, 
	int D,
	int M,
	double *expfuture
);


EXPORT_C_GRIDPRICER int G_InstallmentSwap(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);


EXPORT_C_GRIDPRICER int G_AmortSwap(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);


EXPORT_C_GRIDPRICER int G_SeriesCmpStruct(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);


EXPORT_C_GRIDPRICER int G_DeferrableSwap(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_Compound(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_EitherOrGen (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE         *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
);
	  
#endif
