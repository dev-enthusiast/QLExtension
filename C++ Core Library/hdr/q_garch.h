/****************************************************************
**
**	q_garch.h	- Prototypes for functions in q_garch.c
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_Q_GARCH_H )
#define IN_Q_GARCH_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_GARCH_Asymmetric(
	Q_OPT_TYPE		OptType,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			Spot,				// current spot price
	double			Strike,				// strike of option
	double			OverRate, 			// over ccy rate; risk-free rate of the numeraire
	double			UnderRate,			// under ccy rate; risk-neutral drift = Over Rate - Under Rate
	double			Eps,				// min vol^2
	double			Delta,				// dependence on last vol
	double			Alpha,				// dependence on last ( spot ret - Gamma * vol )^2
	double			Gamma,				// size of asymmetry in spot dependence
	double			FirstVol,			// needed to start of vol series
	double			FirstSpotReturn,	// ditto.
	double			TimeStep,			// used for simulation and for GARCH process
	long			NumberOfSteps,		// TimeStep * NumberOfSteps = time to exp
	long			NumberOfRuns,		// # of simulation runs to perform
	int				Seed,				// seed for the RNG
	Q_RETURN_TYPE	RetType				// Return type (Q_PREMIUM, Q_DELTA, ... )
);

#endif

