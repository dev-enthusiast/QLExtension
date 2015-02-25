/****************************************************************
**
**	gscore/GsVolatilityCurve.h	- GsVolatilityCurve class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gscurve/src/gscore/GsVolatilityCurve.h,v 1.7 1999/05/21 21:27:52 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSVOLATILITYCURVE_H )
#define IN_GSCORE_GSVOLATILITYCURVE_H

#include	<gscore/base.h>
#include	<gscore/GsException.h>
#include	<gscore/GsCTimeCurve.h>
#include	<gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSCURVE GsVolatilityCurve : public GsCTimeCurve
{
public:
	GsVolatilityCurve();
	virtual ~GsVolatilityCurve();
	GsVolatilityCurve( const GsVolatilityCurve& );		
	GsVolatilityCurve( GsFuncHandle<double, double>, GsDate );
	GsVolatilityCurve( double val, GsDate dt = GsDate::fromMDY(1,1,90) );
	GsVolatilityCurve& operator = (  const GsVolatilityCurve& );   

	// Things to support GsDtGeneric
	typedef GsDtGeneric<GsVolatilityCurve> GsDtType;
	inline static const char* typeName() { return "GsVolatilityCurve"; }
	GsString toString() const;

};

CC_END_NAMESPACE

#endif

