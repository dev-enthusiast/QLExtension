/****************************************************************
**
**	q_knockout.h	- routines to handle knockout options 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gridpricer/src/q_knockout.h,v 1.1 2001/07/25 14:42:58 demeor Exp $
**
****************************************************************/

#ifndef IN_Q_KNOCKOUT_H
#define IN_Q_KNOCKOUT_H

#ifndef IN_GRIDPRICER_BASE_H
#include <gridpricer_base.h>
#endif

#include <q_grid.h>   // for Q_GRID_FWD_FUNC
#include <qenums.h>   // for Q_UP_DOWN
#include <datatype.h> // for DT_*

struct Q_GRID_DOMAIN; // defined in q_prop.h

EXPORT_C_GRIDPRICER int G_Knockout (
    Q_GRID_DOMAIN    *FactorSpecs,     // grid spacings, low point, number of factors
    DT_VECTOR        *LayerGrid,       // layer specs
	int              D,                // # Factors
    int              ExDex,            // which event?
    DT_VALUE         *DealInfo,        // Structure containing information needed by exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,
    DT_MATRIX        *Values           // X-Layered grid values -- will be overwritten by this function
	);
			 
#endif



