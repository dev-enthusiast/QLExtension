/****************************************************************
**
**	q_crossvol.h	- Prototypes for q_crossvol.c functions.
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_Q_CROSSVOL_H )
#define IN_Q_CROSSVOL_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_CrossVolScaleFactor(
	Q_VECTOR * Vols,					// set of volatilities for the expiration date
	Q_VECTOR * VolStrikes,				// strikes for those vols (ordered from smallest to largest)
	double     Forward,					// forward point at expiration
	double	   TimeToExp				// time to expiration
);

EXPORT_C_QUANT double Q_CrossVolLogSpotShift(
	Q_VECTOR * Vols,
	Q_VECTOR * VolStrikes,
	double     Forward,
	double     TimeToExp,
	double	   Rho,
	double     Cross12B,	// req'd
	double     Cross13B	// req'd
);

EXPORT_C_QUANT double Q_CrossVolCallPrice(
	 Q_VECTOR * Cross12Vols,
	 Q_VECTOR * Cross12VolStrikes,
	 Q_VECTOR * Cross13Vols,
	 Q_VECTOR * Cross13VolStrikes,
	 double     Cross12Forward,
	 double     Cross13Forward,
	 double     TimeToExp,
	 double     Strike,
	 double     Rho,
	 double     r2			// risk-free interest rate in ccy2 to expiration
);
  
#endif

