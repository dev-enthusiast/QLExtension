/* $Header: /home/cvs/src/gridpricer/src/q_exer.h,v 1.2 2001/04/05 13:59:58 demeor Exp $ */
/****************************************************************
**
**	q_exer.h	Exercise functions for grid pricer
**
**	Copyright 1999 - Goldman Sachs & Company - New York
**
**	$Revision: 1.2 $
**	
****************************************************************/

#if !defined( IN_Q_EXER_H )
#define IN_Q_EXER_H

#if !defined( IN_GRIDPRICER_BASE_H )
#include <gridpricer_base.h>
#endif

#include <q_compound.h>     // Include all exercise function headers through q_exer.h.  Are they actually needed, given that we dynalink them?
#include <q_storage.h>

EXPORT_C_GRIDPRICER int G_OneFactorEuropeanOption(
    Q_GRID_DOMAIN *FactorSpecs,  // Domain specification for factors
    DT_VECTOR     *LayerGrid,    // Domain specification for layer
    int           D,             // # Factors
    int           ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_SimpleBermudanOption(
    Q_GRID_DOMAIN *FactorSpace,   // Domain specification for factors
    DT_VECTOR     *LayerGrid,     // Domain specification for layer
    int           N,              // # Factors
    int           TimeStep,       // Number of diffusion timesteps for this interval
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_BermudanOption(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_ArrowDebreuPrice(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_TestFuture(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_ForwardStrip(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_NofM(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

#if 1
EXPORT_C_GRIDPRICER int G_BermudanStructureEnergyTwoFactor(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_InstallmentOptionEnergy(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);
#endif

EXPORT_C_GRIDPRICER int G_AmortSwapZeroLag(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_AmortSwapLag(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_ChooseSwapNofM(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_EitherOr(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // This time step
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);


EXPORT_C_GRIDPRICER int ForwardFromGrid( 
    double price,
    DT_VECTOR *weight,
    DT_MATRIX *TermCov,
    Q_GRID_DOMAIN *GridSpecs, // Grid specification
    int D,                    // Grid Dimension
    double *future            // Futures array to fill in
);

EXPORT_C_GRIDPRICER int G_BuildFutures(
    int           FutDex,          // Contract: 1 = first nearby, etc.
    DT_VALUE      *DealInfo,       // Contains futures construction info
    DT_ARRAY      *ExDates,        // Exercise date array
    int            ExDex,          // This exercise date index
    Q_GRID_DOMAIN *GridSpecs,      // Grid specification
    int            D,              // Grid Dimension
    double         *future         // Return handle for futures array
);

EXPORT_C_GRIDPRICER int NthNearby(
    DT_VALUE *DealInfo,            // Contains futures construction info
    double ExDate,                 // This exercise date index
    int contract,                  // which nearby?  
    int *FutDex                    // index corresponding to contract at exdex
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


EXPORT_C_GRIDPRICER int G_GM(
    Q_GRID_DOMAIN  *FactorSpecs,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    int            D,             // # Factors
    int            ExDex,         // Index for this exercise date
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_PowerPlant (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);
	  
EXPORT_C_GRIDPRICER int G_TwoAssetCompound (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE      *DealInfo,     // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int FwdFromGridA (
	double          fwd,			 // forward at expiration T
	DT_VECTOR       *Weight,		 // forward at pricing point t
	DT_MATRIX       *Cov,			 // covariance matrix, not needed
	Q_GRID_DOMAIN   *FactorSpec,	 // one-dimensional factor specs
	int             D,				 // must be 1
	double          *grid			 // returned grid
); 

EXPORT_C_GRIDPRICER int G_OptionStrip (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE      	 *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
);
	  
EXPORT_C_GRIDPRICER int G_TwoAssetCompoundGen (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE         *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
);
	  
#endif
