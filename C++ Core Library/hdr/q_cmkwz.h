/* $Header: /home/cvs/src/quant/src/q_cmkwz.h,v 1.3 1999/12/15 11:45:02 goodfj Exp $ */
/****************************************************************
**
**	Q_CMKWZ.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_Q_CMKWZ_H )
#define IN_Q_CMKWZ_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double
Q_MinVar( Q_MATRIX 	*CovMatrix,		
		  Q_VECTOR 	*RetVector,		
		  double 	ExpReturn,			
		  double	MaxSum,
		  double	MinSum,
		  double	MaxIndividual,
		  double	MinIndividual,
		  int		AssetNumber	 
		  );
	
#endif

