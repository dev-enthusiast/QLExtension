/* $Header: /home/cvs/src/quant/src/q_error.h,v 1.16 2000/03/30 13:31:12 goodfj Exp $ */
/****************************************************************
**
**	q_error.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.16 $
**
****************************************************************/

#if !defined( IN_Q_ERROR_H )
#define IN_Q_ERROR_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QBASIC int Q_VECTOR_is_strictly_positive( Q_VECTOR	*V );
EXPORT_C_QBASIC int Q_VECTOR_is_positive( Q_VECTOR	*V );
EXPORT_C_QBASIC int Q_VECTOR_is_strictly_increasing( Q_VECTOR	*V );
EXPORT_C_QBASIC int Q_VECTOR_Throw_Negative_Time( Q_VECTOR *T, Q_VECTOR *fT ); 
EXPORT_C_QBASIC int Q_VECTOR_Throw_Negative_Time2( Q_VECTOR *T ); 
EXPORT_C_QBASIC int is_Variance_Increasing( Q_VECTOR *T, Q_VECTOR *Vol );

#endif

