/* $Header: /home/cvs/src/supermkt/src/q_br2f_x.h,v 1.5 1999/12/15 16:33:45 goodfj Exp $ */
/****************************************************************
**
**	Q_BR2F.H	
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
****************************************************************/

#if !defined( IN_Q_BR2F_X_H )
#define IN_Q_BR2F_X_H


#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#if !defined( IN_MEANREV_H )
#include <meanrev.h>
#endif
#if !defined( IN_DATE_H )
#include <date.h>
#endif

  
// Exports 


DLLEXPORT int BR2F_OptPortOpt
(
	BR2F_MODEL			*br2f,		 // model pars (covariances) from interpolation (assume agrees with pricing date)
	BR2F_BETA			*beta,	     // Beta Curve and Integral
	Q_OPT_TYPE   		callPut,     // option type of compound option
	double       		strike,      // strike of compound option
	DATE         		t_cmpd,      // expiration date of compound option
	Q_VECTOR     		*quantity,   // quantities of underlying options
	Q_VECTOR     		*types,      // option type of underlying as doubles
	Q_VECTOR     		*strikes,    // strikes of underlying options
	MR_CURVES_VECTOR   	*baskets,    // curves containing dates and weights for each basket
	MR_CURVES_VECTOR   	*indices,    // curves containing dates and indices
	DT_CURVE		   	*futures,    // futures curve
	double				p_exp,		 // discounting factor (pricing date to exp. date)
    Q_VECTOR     		*p_settle,   // discounting factors from pricing date to settle dates of underl. options
    int             	n_pts, 		 // number of nodes (per st. dev.) for integration
    int             	n_devs,		 // number st. devs for integration
	double              *opt_price   // result (compound option price)
);

	  
#endif



