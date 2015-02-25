/* $Header: /home/cvs/src/num/src/n_gaussp.h,v 1.6 2000/12/05 16:24:44 lowthg Exp $ */
/****************************************************************
**
**	n_gaussp.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_N_GAUSSP_H )
#define IN_N_GAUSSP_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif

EXPORT_C_NUM double N_GaussPoints3[];
EXPORT_C_NUM double N_GaussWeights3[];

EXPORT_C_NUM double N_GaussPoints20[];
EXPORT_C_NUM double N_GaussWeights20[];

EXPORT_C_NUM double N_GaussPointsP24[];
EXPORT_C_NUM double N_GaussWeightsP24[];

EXPORT_C_NUM double N_GaussPointsP48[];
EXPORT_C_NUM double N_GaussWeightsP48[];

#endif

