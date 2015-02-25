/****************************************************************
**
**	gscore/GsCTimeCurve.h	- GsCTimeCurve class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gscurve/src/gscore/GsCTimeCurve.h,v 1.15 2001/02/28 13:04:22 higgid Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSCTIMECURVE_H )
#define IN_GSCORE_GSCTIMECURVE_H

#include	<gscore/base.h>
#include	<gscore/GsException.h>
#include 	<gscore/GsCTime.h>
#include	<gscore/GsFunc.h>
#include	<gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSCURVE GsCTimeCurve
{
public:
	GsCTimeCurve();
	virtual ~GsCTimeCurve();
	GsCTimeCurve( const GsCTimeCurve& other );
	GsCTimeCurve( GsFuncHandle<double, double> discFunc, GsDate refDate );
	GsCTimeCurve( GsFuncHandle<GsDate, double> discFunc, GsDate refDate );

	GsCTimeCurve&  operator = ( const GsCTimeCurve& other );
	virtual bool operator == ( const GsCTimeCurve& other ) const;

	virtual bool validateCurve() const;
	virtual bool hasIdenticalRep( const GsCTimeCurve& other ) const;
	double eval( const GsCTime& when ) const { return m_func( when ); }
	double eval( const GsDate& when ) const
    { return m_func( GsCTime( when, m_ref )); }

	double operator () ( const GsCTime& when ) const { return eval( when ); }
	double operator () ( const GsDate& when ) const	{ return eval( when ); }

	GsFuncHandle<double, double> getFunc() const { return m_func; }
	GsDate getRefDate() const { return m_ref; }

	void setFunc( GsFuncHandle<double, double> );
	void setRefDate( GsDate r ) { m_ref = r; }

	GsString toString() const;
	inline static const char* typeName() { return "GsCTimeCurve"; }
	typedef GsDtGeneric<GsCTimeCurve> GsDtType;

private:
	GsFuncHandle<double, double>	m_func;
	GsDate							m_ref;
};

CC_END_NAMESPACE

#endif







