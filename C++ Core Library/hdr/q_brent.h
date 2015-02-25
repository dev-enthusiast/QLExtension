/****************************************************************
**
**	q_brent.h	- Brent root-finding
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_Q_BRENT_H )
#define IN_Q_BRENT_H

#ifndef IN_Q_BASE_H
#include <q_base.h>
#endif

typedef double	(*Q_BRENTROOTFUNC)( double Value, void *args );

EXPORT_C_QBASIC double Q_BrentRoot(
	Q_BRENTROOTFUNC f,
	double			Low,
	double			High,
	void			*Args,
	double			Tolerance
);

#endif

