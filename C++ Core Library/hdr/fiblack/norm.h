/***********************************************************************
**
**	norm.h - define various normal distribution functions
**
**	Copyright 1998 - Goldman Sachs & Co. - New York
**
**	$Log: norm.h,v $
**	Revision 1.3  1999/11/09 02:20:21  xuyi
**	wing delta (implied strike from delta)
**
**	Revision 1.2  1998/10/21 23:36:37  keirsb
**	initial revision
**	
**
***********************************************************************/

#if !defined( IN_FIBLACK_NORM_H )
#define IN_FIBLACK_NORM_H

#include <fiblack/base.h>

EXPORT_C_FIBLACK	double	npdf( double x );
EXPORT_C_FIBLACK	double	inverse_npdf( double x );
EXPORT_C_FIBLACK	double	ncdf( double x );
EXPORT_C_FIBLACK	double	inverse_ncdf( double x );
EXPORT_C_FIBLACK	double	n2pdf( double x, double y, double r );
EXPORT_C_FIBLACK	double	n2cdf( double x, double y, double r );
EXPORT_C_FIBLACK	double	dn2dx( double x, double y, double r );
EXPORT_C_FIBLACK	double	dn2dr( double x, double y, double r );
EXPORT_C_FIBLACK	double	dn2dx2( double x, double y, double r );

#endif
