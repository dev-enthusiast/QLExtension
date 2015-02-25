/****************************************************************
**
**	q_parisian.h	- routines to handle Parisian options 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gridpricer/src/q_parisian.h,v 1.8 2001/09/05 17:02:52 rozena Exp $
**
****************************************************************/

#ifndef IN_Q_PARIS_H
#define IN_Q_PARIS_H

#ifndef IN_GRIDPRICER_BASE_H
#include <gridpricer_base.h>
#endif

#include <q_grid.h>   // for Q_GRID_FWD_FUNC
#include <qenums.h>   // for Q_UP_DOWN
#include <datatype.h> // for DT_*

struct Q_GRID_DOMAIN; // defined in q_prop.h

EXPORT_C_GRIDPRICER int G_TriggerParisian (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE         *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_Parisian (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE         *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_ParisianBasket (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE         *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_ParisianBasketPortfolio(
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE         *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_SmoothKO (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
	int              D,                // # Factors involving the knockout
	int              zeroflag,         // 1 if you ignore Layer2 and put in zeros
	Q_UP_DOWN        upOrDown,         // up or down
	double           outVal,           // knockout level
    double           *Layer,           // current values, which are modified, possibly with more than D dimensions
    DT_VECTOR        *Layer0,          // values that you go to if you don't knock out
    DT_VECTOR        *Layer2,          // values that you knock into
	double           *Grid             // precomputed D-dim grid, based on the factor specs
	);

EXPORT_C_GRIDPRICER int G_TriggerParisianQ (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE         *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
	);
			 
#endif



