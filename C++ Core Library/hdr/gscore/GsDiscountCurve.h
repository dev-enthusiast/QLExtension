/****************************************************************
**
**	gscore/GsDiscountCurve.h	- GsDiscountCurve class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gscurve/src/gscore/GsDiscountCurve.h,v 1.14 1999/09/15 22:52:17 tsoucp Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSDISCOUNTCURVE_H )
#define IN_GSCORE_GSDISCOUNTCURVE_H

#include	<gscore/base.h>
#include	<gscore/GsException.h>
#include	<gscore/GsCTimeCurve.h>
#include	<gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSCURVE GsDiscountCurve : public GsCTimeCurve
{
public:
	GsDiscountCurve() {}
	virtual ~GsDiscountCurve() {}
	GsDiscountCurve( const GsDiscountCurve& );		
	GsDiscountCurve( GsFuncHandle<double, double>, GsDate refDate );
	GsDiscountCurve& operator = (  const GsDiscountCurve& );   

	virtual bool validateCurve() const;

	// Things to support GsDtGeneric
	typedef GsDtGeneric<GsDiscountCurve> GsDtType;
	inline static const char* typeName() { return "GsDiscountCurve"; }
	GsString toString() const
	{ return "GsDiscountCurve:" + GsCTimeCurve::toString(); }
};


CC_END_NAMESPACE

#endif

