/* $Header: /home/cvs/src/num/src/n_mulfok.h,v 1.12 2000/12/05 16:08:23 lowthg Exp $ */
/****************************************************************
**
**	n_mulfok.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined( IN_N_MULFOK_H )
#define IN_N_MULFOK_H

#include	<n_crank.h>

EXPORT_C_NUM double N_FADE_OUT_Compute_BlackScholes(
	CRANK_NICHOLSON_INPUT
			*block,
	int		howManyBlock,
	double	x0,
	CRANK_CURVE *cc
);

EXPORT_C_NUM void N_FADE_OUT_Free(
	int	nBlock,
	CRANK_NICHOLSON_INPUT
		*block
);

#endif
