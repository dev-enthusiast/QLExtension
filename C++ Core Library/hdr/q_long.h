/* $Header: /home/cvs/src/quant/src/q_long.h,v 1.9 2000/03/02 11:53:40 goodfj Exp $ */
/****************************************************************
**
**	Q_LONG.H	
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_Q_LONG_H )
#define IN_Q_LONG_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

typedef struct q_long_opt
{
	double		value,      // value of option
				error,      // error of Monte Carlo (if applicable)
				termvol,	// term volatility
				skew,		// skew	of distribution of log(spot)
				kurtosis,   // kurtosis
				disc,       // average domestic discount
				fdisc,      // average foreign discount
				fdiscnq,
				forward;    // computed forward

} Q_LONG_OPT;

EXPORT_C_Q3FAC int Q_LongCallSpread ( 
	double            Spot,      // current spot price
	double            strike1,   // lower strike
	double            strike2,   // upper strike
	double            svol,      // initial spot volatility
	double            rvol,      // initial domestic rate volatility (normal)
	double            yvol,      // initial foreign rate volatility (log-normal)
	double            rhosr,     // correlation (spot, domestic)
	double            rhosy,     // correlation (spot, foreign)
	double            rhory,     // correlation (domestic, foreign)
	double            time,      // time in years
	double            r0,        // initial domestic rate
	double            y0,        // initial foreign rate
	double            theta1,    // drift term for r
	double            theta2,    // drift term for log(y)
	double            beta1,     // mean reversion for r
	double            beta2,     // mean reversion for log(y)
	int               nSteps,    // number of time steps
	int               nPaths,    // number of Monte Carlo simulations
	Q_LONG_OPT        *vars      // returned structure containing lots of stuff	
	);
	
	
EXPORT_C_Q3FAC int Q_LongCallSpreadNN ( 
	double            Spot,      // current spot price
	double            strike1,   // lower strike
	double            strike2,   // upper strike
	double            svol,      // initial spot volatility
	double            rvol,      // initial domestic rate volatility (normal)
	double            yvol,      // initial foreign rate volatility (log-normal)
	double            rhosr,     // correlation (spot, domestic)
	double            rhosy,     // correlation (spot, foreign)
	double            rhory,     // correlation (domestic, foreign)
	double            time,      // time in years
	double            r0,        // initial domestic rate
	double            y0,        // initial foreign rate
	double            theta1,    // drift term for r
	double            theta2,    // drift term for log(y)
	double            beta1,     // mean reversion for r
	double            beta2,     // mean reversion for log(y)
	int               nSteps,    // number of time steps
	int               nPaths,    // number of Monte Carlo simulations
	Q_LONG_OPT        *vars      // returned structure containing lots of stuff	
	);

// obsoleted by Q_ThreeFactorPaths in q_3fac.c
EXPORT_C_Q3FAC int Q_LDPaths (
	double            spot,      // current spot price
	double            svol,      // initial spot volatility
	double            rvol,      // initial domestic rate volatility (normal)
	double            yvol,      // initial foreign rate volatility (normal)
	double            rhosr,     // correlation (spot, domestic)
	double            rhosy,     // correlation (spot, foreign)
	double            rhory,     // correlation (domestic, foreign)
	double            r0,        // initial domestic rate
	double            y0,        // initial foreign rate
	double            theta1,    // drift term for r
	double            theta2,    // drift term for y
	double            beta1,     // mean reversion for r
	double            beta2,     // mean reversion for y
	
	int              *Seed,      // Seed for random number generator
	Q_VECTOR		 *Dates,     // Array of Dates for Paths
	Q_MATRIX		 *Paths      // simulated path will put here
);

EXPORT_C_Q3FAC double Q_OptA2facCN (
	Q_OPT_TYPE	callPut,    // option type (call or put)
	double		spot,		// current cash price
	double		strike,		// strike
	double		longvol,    // long volatility
	double		beta,       // mean reversion	
	double		corr,       // inst. correlation between short and long
	double		exptime,	// time to expiration.
	Q_VECTOR    *vols,      // term volatility vector
	Q_VECTOR 	*volT, 	 	// volatility times
	Q_VECTOR    *fwds,      // forward vector
	Q_VECTOR    *fwdT,      // forward nodes
 	Q_VECTOR	*Rds,       // domestic interest vector
	Q_VECTOR	*RdT,		// domestic interest nodes
	double      rangevol,   // volatility to determine range of integration
	int         nGrid1,     // number of gridpoints in short dimension
	int         nGrid2,     // number of gridpoints in long dimension
	int         nSteps,     // number of timesteps
	double      tolerance	// tolerance parameter for conjugate gradient
);



#endif

