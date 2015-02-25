/* $Header: /home/cvs/src/quant/src/q_fwd.h,v 1.2 2000/03/03 17:54:29 goodfj Exp $ */
/****************************************************************
**
**	Q_FWD.H
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/


#if !defined( IN_Q_FWD_H )
#define IN_Q_FWD_H

#include <q_base.h>

EXPORT_C_QBASIC double	Q_CalcFwd(double spot, double rd, double rf, double term);
#endif
