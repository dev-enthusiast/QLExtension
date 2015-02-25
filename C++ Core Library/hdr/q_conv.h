/* $Header: /home/cvs/src/quant/src/q_conv.h,v 1.3 1999/12/15 11:45:03 goodfj Exp $ */
/****************************************************************
**
**	Q_CONV.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_Q_CONV_H )
#define IN_Q_CONV_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QUANT double Q_Conv (
	Q_VECTOR    *xVector, // Vector 1
	Q_VECTOR    *yVector, // Vector 2
	int			term	  // term
	);

EXPORT_C_QUANT double Q_DeConv (
	Q_VECTOR    *xVector, // Vector 1
	Q_VECTOR    *yVector, // Vector 2
	int			isign,	  //
	int			term	  // term
	);

#endif

