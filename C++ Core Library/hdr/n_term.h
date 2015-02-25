/* $Header:  */
/****************************************************************
**
**	N_TERM.H	- routines to calculate year fraction
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_N_TERM_H )
#define IN_N_TERM_H

#if !defined( IN_Q_TYPES_H )
#include <qtypes.h>
#endif
			
EXPORT_C_NUM double N_CalcTermWted(
	DATE			start,		// Start date
	DATE			end,	 	// End date
	Q_VECTOR        *weights    // weightings for days of the week
	);

EXPORT_C_NUM double	N_CalcTerm(DATE fr, DATE to, int type);

#endif

