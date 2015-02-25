/* $Header: /home/cvs/src/oz/src/oz.h,v 1.18 1999/12/23 18:32:09 goodfj Exp $ */
/****************************************************************
**
**	OZ.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.18 $
**
****************************************************************/

#if !defined( IN_OZ_H )
#define IN_OZ_H

#ifndef		IN_QTYPES_H
#include	<qtypes.h>
#endif
#ifndef		IN_NUM_H
#include	<num.h>
#endif
#ifndef		IN_DATE_H
#include	<date.h>
#endif
#ifndef		IN_DATATYPE_H
#include	<datatype.h>
#endif
#ifndef		IN_SLANGFNC_H
#include	<slangfnc.h>
#endif


DLLEXPORT int Q_MonteCarloFactors(
    int			NumFactors,		// Number of factors
	N_GENERIC_FUNC
				Callback,		// Drift/covariance callback
    SLANG_N_GENERIC_FUNC_CONTEXT
				*Context,		// Context structure to pass to callbacks
	DT_CURVE    **Path,			// Simulated paths will be put here
	DATE		Start,			// Start date for simulation
 	DATE		End, 			// End date for simulation
	int			*Seed, 			// Seed for random generator
	int			NumPaths,		// Number of paths to generate
	DATE		*DateArray,		// Array of Dates for paths
	int			DateSize		// Size of DateArray
	);

DLLEXPORT int Q_MonteCarloFactorsNoCallback(
    int			NumFactors,		    // Number of factors
	Q_MATRIX    *CovarianceMatrix,  //
	Q_MATRIX    *CorrelationMatrix, //
	Q_MATRIX    *GeneratorMatrix,   //
	DATE		*DateArray,         // Array of Dates for paths
	int			DateSize,		    // Size of DateArray
	DT_CURVE    **Drifts,           //
	DT_CURVE    **ForwardVols,      //
	double      *LastDrift,         //
	double      *LastVol,           //
	DT_CURVE    **Path,			    // Simulated paths will be put here
	DATE		Start,			    // Start date for simulation
	int			*Seed, 			    // Seed for random generator
	int			NumPaths 		    // Number of paths to generate
);

DLLEXPORT int Q_MonteCarloFactorsGeneric(
    DT_GCURVE   *CorrMatrices,      // Correlation matrices (stepwise constant)
	DT_CURVE    *FactorDates,       // Dates for which we want factor movements
	int			Seed, 			    // Seed for random generator
    int			NumFactors,		    // Number of factors
	int			NumPaths, 		    // Number of paths to generate
    DT_VALUE    *PathsValue         // To be set by called function
);

#endif

