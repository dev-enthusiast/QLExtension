/* $Header: /home/cvs/src/gsdate/src/gscore/GsPeriod.h,v 1.16 2001/11/27 22:43:10 procmon Exp $ */
/****************************************************************
**
**	GsPeriod.h	-
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.16 $
**
****************************************************************/

#if !defined( IN_GSPERIOD_H )
#define IN_GSPERIOD_H

#include	<gscore/base.h>
#include	<gscore/GsString.h>
#include	<gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

class GsXInvalidArgument;

/*
**	GsPeriod exists to contain an integer number of periods, for example,
**	"one month" or "2 business days".
**
**	SeeAlso: GsDate, GsRDate
*/

class EXPORT_CLASS_GSDATE GsPeriod
{
public:
	typedef GsDtGeneric<GsPeriod> GsDtType;
	inline static const char* typeName() { return "GsPeriod"; }
	GsString toString() const;

	enum UNIT
	{
		INVALID,
		DAYS,			// "d"
		BUSINESS_DAYS,	// "b"
		WEEKS,			// "w"
		MONTHS,			// "m"
		YEARS,			// "y"
		BUSINESSDAYS = BUSINESS_DAYS
	};

	GsPeriod()
	:	m_count( 0 ),
		m_unit( INVALID )
	{}
	
	GsPeriod( const GsPeriod& x )
	:	m_count( x.m_count ),
		m_unit( x.m_unit )
	{}
	
	GsPeriod( int count, UNIT unit )
	:	m_count( count ),
		m_unit( unit )
	{}
	
	GsPeriod( int count, const GsString& unitname )
	:	m_count( count ),
		m_unit( convertStringToUnit( unitname ))
	{}
	
	explicit GsPeriod( const GsString& s );
	
	explicit GsPeriod( int frequency );

    GsPeriod& operator = ( const GsPeriod& rhs )
    {
		m_count = rhs.m_count;
		m_unit = rhs.m_unit;
		return *this;
    }

    int getCount() const
    {
	    return m_count;
    }

    void setCount( int count )
    {
	    m_count = count;
    }

	UNIT getUnit() const
	{
		return m_unit;
	}

	void setUnit( UNIT unit )
	{
		m_unit = unit;
	}

	GsString getUnitName() const
	{
		return convertUnitToString( m_unit );
	}

    void setUnitName( const GsString& unitname )
	{
		m_unit = convertStringToUnit( unitname );
	}

    int getFrequency() const
    {
	    return convertPeriodToFrequency( m_count, m_unit );
    }

	int isValid() const
	{
		return isValidUnit( m_unit );
	}

    static bool isValidFrequency( int freq );
    static bool isValidUnit( UNIT unit );

    static const GsString& convertUnitToString( UNIT unit );

private:
	static void convertFrequencyToPeriod( int freq, int& count, UNIT& unit );
    static int  convertPeriodToFrequency( int count, UNIT unit );

    static UNIT convertStringToUnit( const GsString& unitname );

    int		m_count;
    UNIT	m_unit;
};

inline bool operator == ( const GsPeriod& lhs, const GsPeriod& rhs )
{
	return ( (lhs.getCount() == rhs.getCount()) && (lhs.getUnit() == rhs.getUnit()) );
}

inline bool operator != ( const GsPeriod& lhs, const GsPeriod& rhs )
{
	return !( lhs == rhs );
}


/*
**	Addin wrappers
*/

EXPORT_CPP_GSDATE GsPeriod
		GsPeriodFromFreq(	int frequency ),
		GsPeriodFromString( GsString inString );

EXPORT_CPP_GSDATE int
		GsPeriodToFreq( GsPeriod );

CC_END_NAMESPACE

#endif

