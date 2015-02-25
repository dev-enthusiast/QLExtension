/****************************************************************
**
**  gsquant/transdistribution.h	- Distribution transformation functions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/transdistribution.h,v 1.1 1999/05/06 05:16:33 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_TRANSDISTRIBUTION_H )
#define IN_GSQUANT_TRANSDISTRIBUTION_H

#include	<gsquant/base.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT double
		InverseCumulativeNormalDistribution(double Val);

EXPORT_CPP_GSQUANT double
		CumulativeNormalDistribution(double Val);

EXPORT_CPP_GSQUANT double
		NormalDistribution(double x);

CC_END_NAMESPACE

#endif 

