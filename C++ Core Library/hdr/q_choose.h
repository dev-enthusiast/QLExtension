/* $Header: /home/cvs/src/quant/src/q_choose.h,v 1.7 2000/03/02 15:01:55 goodfj Exp $ */
/****************************************************************
**
**	Q_CHOOSE.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_Q_CHOOSE_H )
#define IN_Q_CHOOSE_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif

EXPORT_C_QEXOTIC double Q_YouChoose (
	double         spot,          // current spot
	double         strike1,       // strike for call
	double         strike2,       // strike for put
	double         vol1,          // volatility	to choose time
	double         vol2,          // volatility to expiration
	double         choose,        // time to choice
	double         timeToExp,     // time to expiration
	double         ccRd1,         // domestic rate to choose time
	double         ccRd2,         // domestic rate to expiration
	double         ccRf1,         // foreign rate to choose time
	double         ccRf2		  // foreign rate to expiration
	);
	
#endif
