/****************************************************************
**
**	gscore/GsForwardFXCurve.h	- GsForwardFXCurve class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gscurve/src/gscore/GsForwardFXCurve.h,v 1.2 1999/05/21 21:27:51 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSFORWARDFXCURVE_H )
#define IN_GSCORE_GSFORWARDFXCURVE_H

#include	<gscore/base.h>
#include	<gscore/GsException.h>
#include	<gscore/GsCTimeCurve.h>
#include	<gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSCURVE GsForwardFXCurve : public GsCTimeCurve
{
public:
	GsForwardFXCurve();
	virtual ~GsForwardFXCurve();
	GsForwardFXCurve( const GsForwardFXCurve& );

	GsForwardFXCurve( const GsCTimeCurve&, const GsCTimeCurve&, double );
	GsForwardFXCurve( const GsCTimeCurve&, const GsCTimeCurve&, double, GsDate );

	GsForwardFXCurve( GsFuncHandle<double, double>, 
					  GsFuncHandle<double, double>, 
					  GsDate, double, GsDate );

	GsForwardFXCurve( GsFuncHandle<double, double>, GsDate );

	GsForwardFXCurve& operator = (  const GsForwardFXCurve& );   

	virtual bool validateCurve() const;

	// Things to support GsDtGeneric
	typedef GsDtGeneric<GsForwardFXCurve> GsDtType;
	inline static const char* typeName() { return "GsForwardFXCurve"; }
	GsString toString() const;

private:

};

CC_END_NAMESPACE

#endif

