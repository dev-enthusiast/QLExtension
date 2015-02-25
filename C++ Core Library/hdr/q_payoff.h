/****************************************************************
**
**	q_payoff.h	- payoff functions for generalized Monte Carlo
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gridpricer/src/q_payoff.h,v 1.3 2001/08/31 16:02:01 demeor Exp $
**
****************************************************************/

#if !defined( IN_Q_PAYOFF_H )
#define IN_Q_PAYOFF_H

#if !defined( IN_GRIDPRICER_BASE_H )
#include <gridpricer_base.h>
#endif

EXPORT_C_GRIDPRICER int G_EitherOrPayoff(
    double        *Spots,        // input vector of spots
    DT_VALUE      *DealInfo,     // Structure containing information needed by payoff function
    double        *Value         // computed payoff value
	);

EXPORT_C_GRIDPRICER int G_SamplePayoff(
    double        *Spots,        // input vector of spots
    DT_VALUE      *DealInfo,     // Structure containing information needed by payoff function
    double        *Value         // computed payoff value
	);

EXPORT_C_GRIDPRICER int G_OsakaDealPayoff(
    double        *Asset,        // input array of spots
    DT_VALUE      *DealInfo,     // Structure containing information needed by payoff function
    double        *Value         // computed payoff value
	);

#endif

