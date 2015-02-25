/****************************************************************
**
**	GsCTime.h	- GsCTime class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsCTime.h,v 1.3 2001/11/27 22:41:02 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCTIME_H )
#define IN_GSCTIME_H

#include	<gscore/base.h>
#include	<gscore/GsDate.h>
	  
CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSBASE GsCTime
{
public:
	GsCTime() : m_val( 0 ) {}
	GsCTime( const GsCTime& ctime ) : m_val( ctime.m_val ) {}
	explicit GsCTime( double v ) : m_val( v ) {}
	GsCTime( const GsDate& date1, const GsDate& date2 );
	~GsCTime() {}
	
	operator double() const { return m_val; }
	GsCTime operator + ( const GsCTime& rhs ) const { return GsCTime( m_val + rhs ); }
	GsCTime operator - ( const GsCTime& rhs ) const { return GsCTime( m_val - rhs ); }

	int getDays() const { return int( m_val * getDateConstant() + ( m_val > 0 ? getDateTolerance() : -getDateTolerance() ) ); }

	GsString toString() const;

	inline static const char* typeName() { return "GsCTime"; }
	typedef GsDtGeneric<GsCTime> GsDtType;

private:
	// This is the constant by which integer date differences are normalized.
	// DO NOT RELY ON THIS CONSTANT IN ANY WAY.
	static double getDateConstant() { return 365.2425; }
	
	// Midnight is approximately 23:59:59.99968
	// This is necessary for rounding past midnight when taking the difference between two dates.
	static double getDateTolerance() { return 1e-11; }

	double m_val;
};

inline GsDate& operator += ( GsDate& date, const GsCTime& ctime )
{
	return date += ctime.getDays();
}

inline GsDate& operator -= ( GsDate& date, const GsCTime& ctime )
{
	return date -= ctime.getDays();

}

inline GsDate operator + ( const GsDate& date, const GsCTime& ctime )
{
	GsDate result( date );
	result += ctime;
	return result;
}

inline GsDate operator - ( const GsDate& date, const GsCTime& ctime )
{
	GsDate result( date );
	result -= ctime;
	return result;
}

CC_END_NAMESPACE

#endif

