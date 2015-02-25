/* $Header: /home/cvs/src/oz/src/monte.h,v 1.1 1999/12/23 14:35:20 goodfj Exp $ */
/****************************************************************
**
**	MONTE.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	Q_Monte declared here, separately from oz.h, to remove dependency
**	on slangfnc.h. This means that e.g. quant.dll no longer rebuilds
**	every time secdb.h changes.
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_MONTE_H )
#define IN_MONTE_H

#ifndef		IN_QTYPES_H
#include	<qtypes.h>
#endif
#ifndef		IN_DATE_H
#include	<date.h>
#endif
#ifndef		IN_DATATYPE_H
#include	<datatype.h>
#endif


DLLEXPORT int Q_Monte(
	int			*NumVars,		// Number of vars to be simulated
	DT_CURVE  	**Fwds,			// Forward curves.
	DT_CURVE  	**Vols,			// Term vol curves.
	DT_CURVE   **Path,			// Simulated paths will be put here.
	Q_MATRIX	*Corr,			// Correlation  Q_MATRIX.
	DATE		Start,			// Start date for simulation.
	DATE		End, 			// End date for simulation.
	int			*Seed, 			// Seed for random generator.	
	int			nPaths,			// Number of paths to generate.	
	DATE		*DateArray,		// Array of Dates for paths
	int			DateSize		// Size of DateArray
	);		

#endif

