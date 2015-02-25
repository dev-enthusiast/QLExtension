/****************************************************************
**
**	GSTIMEDIFF.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsTimeDiff.h,v 1.8 2001/09/24 12:12:16 tsoucp Exp $
**
****************************************************************/

#ifndef IN_GSBASE_GSTIMEDIFF_H
#define IN_GSBASE_GSTIMEDIFF_H

#include <gscore/base.h>

#include <time.h>

CC_BEGIN_NAMESPACE( Gs )

class GsTimeDiff
{
public:
	inline GsTimeDiff( const GsString& mode );
	~GsTimeDiff() {}

	inline double operator()() const;

private:
	enum TIMINGMODE { CLOCK, TIME, INVALID } m_timingMode;

	clock_t m_clockTime0;
	time_t m_realTime0;
};

// .... Inline ....
GsTimeDiff::GsTimeDiff( const GsString& mode )
{
	if( mode == "CLOCK" )
	{
		m_timingMode = CLOCK;
		m_clockTime0 = clock();
	}

	else if( mode == "TIME" )
	{
		m_timingMode = TIME;
		m_realTime0 = time( NULL );
	}
		
	else
	{
		m_timingMode = INVALID;

		GSX_THROW( GsXInvalidArgument, 
				   "Unknown timing mode [ TIME | CLOCK ]" );
	}
}

double GsTimeDiff::operator()() const
{
	double secs = 0.;
	time_t realTime1;

	switch( m_timingMode )
	{
		case CLOCK :
			secs = ( double ) ( clock() / CLOCKS_PER_SEC )
					- 
					( double ) ( m_clockTime0 / CLOCKS_PER_SEC );
			break;

		case TIME :
			realTime1 = time( NULL );
			secs = difftime( realTime1, m_realTime0 );
			break;

		case INVALID :
			GSX_THROW( GsXInvalidOperation, 
					   "Unknown timing mode [ TIME | CLOCK ]" );
			break;
	}

	return secs;
}

CC_END_NAMESPACE

#endif 
