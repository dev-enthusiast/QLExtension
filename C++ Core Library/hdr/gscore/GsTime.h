/* $Header: /home/cvs/src/gsbase/src/gscore/GsTime.h,v 1.6 2001/11/27 22:41:10 procmon Exp $ */
/****************************************************************
**
**	gscore/GsTime.h	- GsTime class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsTime.h,v 1.6 2001/11/27 22:41:10 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSTIME_H )

#include <gscore/base.h>
#include <gscore/GsTimeZone.h>
#include <gscore/GsDate.h>

CC_BEGIN_NAMESPACE( Gs )

class GsRelativeTime;

/*
**	GsTeim represents a specific moment in the history
**	of the earth.  It is completely independent of 
**	time-zone considerations.
*/

class EXPORT_CLASS_GSBASE GsTime
{
  public:
	typedef class GsDtTime	GsDtType;

	// How many seconds in a day?
	enum {
		SECONDS_PER_DAY = 24 * 3600
	};

	GsTime();
	GsTime( const GsTime &rhs );
	GsTime( const GsDate &rhs );
	~GsTime();

	GsTime		&operator=( const GsTime &Rhs );
	inline bool	operator==( const GsTime &Rhs ) { return( m_SecondsSince1900 == Rhs.m_SecondsSince1900 ); }
	inline bool	operator<( const GsTime &Rhs ) 	{ return( m_SecondsSince1900 < Rhs.m_SecondsSince1900 ); }

	inline GsStatus valid() const;

	inline double secondsSince1900() const { return m_SecondsSince1900; };
	inline double daysSince1900() const { return (   m_SecondsSince1900 
												   / SECONDS_PER_DAY ); };

	GsStatus hourMinuteSecond( int &Hour, int &Minute, 
							   int &Second ) const;

	GsStatus add( const GsRelativeTime &Rhs );

	GsString	toString( const GsTimeZone & = GsTimeZoneGMT ) const;

  private:
	// Our internal representation is the number of seconds since
	// 12:00AM 1900.
	double m_SecondsSince1900;
};

inline GsStatus GsTime::valid() const
{ 
	if (    ( m_SecondsSince1900 >= 0 ) 
		 && ( m_SecondsSince1900 < (   ( GsDate::LAST_YEAR - 1900 ) 
									 * 365.25 * 24 * 3600 ) ) )
		return GS_OK;
	else
		return GS_ERR;
};

CC_END_NAMESPACE

#define IN_GSCORE_GSTIME_H



#endif



