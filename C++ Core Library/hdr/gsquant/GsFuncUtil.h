/****************************************************************
**
**	GSFUNCUTIL.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncUtil.h,v 1.5 2000/12/19 15:01:09 jankua Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCUTIL_H )
#define IN_GSQUANT_GSFUNCUTIL_H

#include <gsquant/base.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDiscountCurve.h>
CC_BEGIN_NAMESPACE( Gs )

typedef GsFuncHandle< double, double > GsFuncDoubleDouble;
typedef GsFunc2Handle< double, double, double > GsFuncDoubleDoubleDouble;


EXPORT_CPP_GSQUANT GsFuncDoubleDoubleDouble GsFuncMaxCreate( );
EXPORT_CPP_GSQUANT GsFuncDoubleDoubleDouble GsFuncMinCreate( );

EXPORT_CPP_GSQUANT GsFuncDoubleDouble GsFuncComposeCreate( const GsFuncDoubleDouble &f, const GsFuncDoubleDouble &g );
EXPORT_CPP_GSQUANT GsFuncDoubleDouble GsFuncProductCreate( const GsFuncDoubleDouble &f, const GsFuncDoubleDouble &g );
EXPORT_CPP_GSQUANT GsFuncDoubleDouble GsFuncExpCreate();
EXPORT_CPP_GSQUANT GsDiscountCurve* GsDiscountCurveCreateFromGsFunc( const GsFuncDoubleDouble &f, GsDate RefDate );
CC_END_NAMESPACE

#endif 
