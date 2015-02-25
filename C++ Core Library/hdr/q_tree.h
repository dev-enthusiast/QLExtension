/* $Header: /home/cvs/src/quant/src/q_tree.h,v 1.6 2000/03/02 12:17:41 goodfj Exp $ */
/****************************************************************
**
**	Q_TREE.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_TREE_H )
#define IN_Q_TREE_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_LiborShortRate (
	int            size,       // number of timesteps in tree
	Q_VECTOR       *discount,  // vector of discount factors
	double         vol,        // volatility
	double         deltat,     // size of timestep
	int            ordinal     // which continuous short rate to return
	);
EXPORT_C_QUANT int Q_CalcShortRates (
	int            size,       // number of timesteps in tree
	Q_VECTOR       *discount,  // vector of discount factors
	double         vol,        // volatility
	double         deltat,     // size of timestep
	Q_VECTOR       *rate       // return vector of centered short rates
	);

#endif

