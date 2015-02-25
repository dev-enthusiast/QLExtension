/* #define GSCVSID "$Header: /home/cvs/src/qcommod/src/gnf_mc.h,v 1.6 2001/10/05 19:27:21 yamama Exp $" */
/****************************************************************
**
**	gnf_mc.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_GNF_MC_H )
#define IN_GNF_MC_H


// Constants
						 
// Macros
					
// Structures

// Exports

  
DLLEXPORT int GnfMcPaths1As_General(
	GNF_MODEL_1AS	*gnf,	   		// Gnf model structure
	DT_TIME			t0,				// initial time (pricing date)
	DT_TCURVE      	*t_fix,			// fiXing times (MC simulation step times); values relate to index vector as described above
	DT_TCURVE		*t_forw,		// Forward times for which prices are needed; values are initial forward prices
	DT_VECTOR		*forw_index,	// fiXing Forward Index vector 
	int				n_mc,			// Number of MC paths
	int				SeedInit,   	// initial seed for random number generation
	int				UseAnti,		// Antithetic path usage flag (use if TRUE)
	DT_MATRIX		**values		// Result: Matrix with each row giving one path (indices as given by index vector)
);
 
DLLEXPORT int GnfMcPaths_General(
	int             n_A,
	GNF_MODEL_1AS   **GnfMs,                // Gnf model structures
	GNF_MODEL_2ASX  **GnfXMs,               // Gnf cross model structures
	DT_TIME			t0,				// initial time (pricing date)
	DT_TCURVE      	*t_fix,			// fiXing times (MC simulation step times); values relate to index vector as described above
	DT_TCURVE	**t_forw,		// Forward times for which prices are needed; values are initial forward prices
	DT_VECTOR       *asset_index,           // fixing Asset Index vector
	DT_VECTOR		*forw_index,	// fiXing Forward Index vector 
	int				n_mc,			// Number of MC paths
	int				SeedInit,   	// initial seed for random number generation
	int				UseAnti,		// Antithetic path usage flag (use if TRUE)
	DT_MATRIX		**values		// Result: Matrix with each row giving one path (indices as given by index vector)
);

DLLEXPORT int GnfMonteCarloPrecalc(
	GNF_MODEL_1AS	*gnf,	   
    DT_TIME 		StartTime,	  		// Start Time
    DT_TCURVE 		*StepTimes,   		// Path node dates
    DT_TCURVE 		*FutExpTimes,   	// Futures exp dates and prices
	QC_MC_PRECALC	**McPre				// Result: Monte Carlo precalculated stuff
);
 

#endif

