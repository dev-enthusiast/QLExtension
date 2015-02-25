/****************************************************************
**
**	Q_3FAC.H	routines to handle 3-factor models
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_3FAC_H )
#define IN_Q_3FAC_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_Q3FAC int Q_ThreeFactorPaths (
	double        spot,      	// current dollar per gold spot
	Q_VECTOR      *vols,     	// term exchange rate volatility vector
	Q_VECTOR      *voltimes, 	// term exchange rate volatility grid vector
	double        sigr,       	// normalized standard deviation of domestic rate
	double        sigy,       	// normalized standard deviation of lease rate
	double        rhor,      	// correlation of log(spot) and domestic short rate
	double        rhoy,      	// correlation of log(spot) and lease rate
	double        rhory,      	// correlation of lease rate and domestic short rate
	double        ar,         	// mean reversion coefficient for domestic rate
	double        ay,         	// mean reversion coefficient for lease rate
	double        time,         // time to expiration
	Q_VECTOR      *Rd,       	// domestic term discount factors	 
	Q_VECTOR      *RdT,      	// domestic term interest rate grid	 
	Q_VECTOR      *Rf,       	// gold term discount factors (lease rates)
	Q_VECTOR      *RfT,      	// gold term interest rate grid
	int           npaths,    	// number of paths
	int           nSteps,   	// total number of timestep
	int           tmpSeed,      // initial seed parameter
	Q_MATRIX      *spotPaths,   // returned matrix containing spot price paths
	Q_MATRIX      *RdPaths,     // domestic short rate paths
	Q_MATRIX      *RfPaths      // foreign short rate paths
	);

EXPORT_C_Q3FAC int Q_BackOutSpotVol3 (
    Q_VECTOR 		*vols,        // term exchange rate volatility vector
    Q_VECTOR		*voltimes,    // term exchange rate volatility grid vector
    double          sigr,         // normalized standard deviation of domestic rate
    double          sigy,         // normalized standard deviation of lease rate
    double          rhor,         // correlation of log(spot) and domestic short rate
    double          rhoy,         // correlation of log(spot) and lease rate
    double          rhory,        // correlation of lease rate and domestic short rate
    double          ar,           // mean reversion coefficient for domestic rate
    double          ay,           // mean reversion coefficient for lease rate
    double          time,         // time to expiration
    int             n,            // number of timesteps
    Q_VECTOR        *sigs         // returned spot volatility vector
    );
	
#endif

