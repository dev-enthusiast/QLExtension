/****************************************************************
**
**	gscore/GsIndexCurve.h	- GsIndexCurve class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gscurve/src/gscore/GsIndexCurve.h,v 1.9 1999/05/21 21:27:51 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSINDEXCURVE_H )
#define IN_GSCORE_GSINDEXCURVE_H

#include	<gscore/base.h>
#include	<gscore/GsException.h>
#include	<gscore/GsCTimeCurve.h>
#include	<gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSCURVE GsIndexCurve : public GsCTimeCurve
{
public:
	GsIndexCurve();
	virtual ~GsIndexCurve();
	GsIndexCurve( const GsIndexCurve& );		
	GsIndexCurve( GsFuncHandle<double, double>, GsDate );
	GsIndexCurve( GsFuncHandle<GsDate, double>, GsDate );
	GsIndexCurve( double val, GsDate dt = GsDate::fromMDY(1,1,90) );
	GsIndexCurve& operator = (  const GsIndexCurve& );   

	// Things to support GsDtGeneric
	typedef GsDtGeneric<GsIndexCurve> GsDtType;
	inline static const char* typeName() { return "GsIndexCurve"; }
	GsString toString() const;

};

CC_END_NAMESPACE

#endif

