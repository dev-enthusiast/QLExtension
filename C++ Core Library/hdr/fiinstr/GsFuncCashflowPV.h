/****************************************************************
**
**	GSFUNCCASHFLOWPV.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsFuncCashflowPV.h,v 1.3 1999/03/06 20:45:11 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCCASHFLOWPV_H )
#define IN_GSFUNCCASHFLOWPV_H

#include <fiinstr/base.h>

#include <gscore/GsDate.h>
#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDiscountCurve.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_FIINSTR
double cashflowPV(GsFuncHandle<GsDate, double> flows, 
				  GsFuncHandle<GsDate, double> discounts);

EXPORT_CPP_FIINSTR
double cashflowPV(GsFuncHandle<GsDate, double> flows, 
				  const GsDiscountCurve&);

EXPORT_CPP_FIINSTR
double cashflowPV(GsFuncHandle<double, double> flows, 
				  GsFuncHandle<double, double> discounts);

CC_END_NAMESPACE

#endif 
