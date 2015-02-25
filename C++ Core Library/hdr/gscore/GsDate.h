/****************************************************************
**
**	GsDate.h - GsDate class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsDate.h,v 1.7 2011/05/16 20:06:08 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_H )
#define IN_GSDATE_H

#include	<gscore/GsString.h>
#include	<gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )							

class GsXInvalidArgument;


/*
**	GsDate - A date class for all FICC date operations.
*/

#ifdef IN_DATE_H
// We have included date.h before this, we have to undef the short forms
// This is bad, but hey compared to ... 
#undef JAN
#undef FEB
#undef MAR
#undef APR
#undef MAY
#undef JUN
#undef JUL
#undef AUG
#undef SEP
#undef OCT
#undef NOV
#undef DEC

#undef MON
#undef TUE
#undef WED
#undef THU
#undef FRI
#undef SAT
#undef SUN

#endif

class EXPORT_CLASS_GSBASE GsDate
{
public:
	typedef GsDtDate GsDtType;

	enum MONTH
	{
		JANUARY   = 1,
		FEBRUARY  = 2,
		MARCH     = 3,
		APRIL     = 4,
		MAY       = 5,
		JUNE      = 6,
		JULY      = 7,
		AUGUST    = 8,
		SEPTEMBER = 9,
		OCTOBER   = 10,
		NOVEMBER  = 11,
		DECEMBER  = 12,
		// And some short names, too
		JAN		  = JANUARY,
		FEB		  = FEBRUARY,
		MAR		  = MARCH,
		APR		  = APRIL,
		JUN		  = JUNE,
		JUL		  = JULY,
		AUG		  = AUGUST,
		SEP		  = SEPTEMBER,
		OCT		  = OCTOBER,
		NOV		  = NOVEMBER,
		DEC		  = DECEMBER
	};

	enum WEEKDAY
	{
		MONDAY		= 1,
		TUESDAY		= 2,
		WEDNESDAY	= 3,
		THURSDAY	= 4,
		FRIDAY		= 5,
		SATURDAY	= 6,
		SUNDAY		= 7,
		MON			= MONDAY,
		TUE			= TUESDAY,
		WED			= WEDNESDAY,
		THU			= THURSDAY,
		FRI			= FRIDAY,
		SAT			= SATURDAY,
		SUN			= SUNDAY
	};

	inline GsDate();
	inline GsDate(const GsDate& date);
	explicit GsDate(unsigned long julianDayNumber);
	explicit GsDate( const GsString& dateString );

	// GsDate is deprecated.  Use fromDMY(), fromMDY(), or fromYMD().
	GsDate( int d, int m, int y );

	unsigned long toDATE() const;
	static GsDate fromDATE(   unsigned long dateDayNumber );
	static GsDate fromJulian( unsigned long julianDayNumber );
	static inline GsDate fromDMY( int d, int m, int y );
	static inline GsDate fromMDY( int m, int d, int y );
	static inline GsDate fromYMD( int y, int m, int d );
	static GsDate today();

	double toExcelDate() const;
	static bool   inSafeRange( const unsigned long );
	static GsDate fromExcelDate( double ExcelDayNumber );
    
 	static GsDate fromIntYMD( unsigned long dateYMD );
	unsigned long toIntYMD() const;

	static GsDate lowLimit();
	static GsDate highLimit();

	inline GsDate& operator = ( const GsDate& date );

	inline GsDate& operator ++ ();
	inline GsDate& operator -- ();

	inline GsDate operator ++ ( int );
	inline GsDate operator -- ( int );

	inline GsDate& operator += ( int s );
	inline GsDate& operator -= ( int s );

    unsigned long getJulianDayNumber() const; // no inputs validity checking

	inline int   getDayOfMonth() const;
	inline MONTH getMonth() const;
	inline int   getYear() const;

	GsString getMonthAsString() const;
	GsString getMonthAsStringWithNumbers() const;
	GsString getYearAsString()	const;


    inline WEEKDAY getWeekDay() const;
	int getDayOfYear() const;
	bool isValid() const;		// FIX-should be inline m_ccyymmdd != 0
	static bool isValidMDY( int month, int day, int year );
	inline static bool isNullDate( const GsDate& Date ) { return Date.m_ccyymmdd == 0 ? true : false; }

	GsString toString() const;
	GsString toStringYYYYMMDDFormat() const;
	GsString toStringDtDateFormat() const;

	friend bool operator <  ( const GsDate& d1, const GsDate& d2 );
	friend bool operator <= ( const GsDate& d1, const GsDate& d2 );
	friend bool operator >  ( const GsDate& d1, const GsDate& d2 );
	friend bool operator >= ( const GsDate& d1, const GsDate& d2 );
	friend bool operator == ( const GsDate& d1, const GsDate& d2 );
	friend bool operator != ( const GsDate& d1, const GsDate& d2 );

	static int getDaysInMonth( MONTH month, int year );
	static int getDaysInYear( unsigned year );
	static bool isLeapYear( unsigned year );

	bool isLastDayOfMonth() const;

	static GsDate getFirstDayOfMonth( MONTH month, int year );
	static GsDate getLastDayOfMonth( MONTH month, int year );

	static GsDate getNthWeekDayOfMonth( int n, WEEKDAY weekday, MONTH month, int year );
    static GsDate getLastWeekDayOfMonth( WEEKDAY weekday, MONTH month, int year );

	static int getLeapDaysBetweenDates( const GsDate& date1, const GsDate& date2 );
	static int getDaysBetweenDates( const GsDate& dateHigh, const GsDate& dateLow )
	{
		return dateHigh.getJulianDayNumber() - dateLow.getJulianDayNumber();
	}

	static int getPivotYear()  { return s_pivotYear; }
	static void setPivotYear( int pivot )  { s_pivotYear = pivot; }

	static const GsString& getMonthName( const MONTH& month );
	inline static const GsString& getMonthName( int month ) { return( getMonthName( MONTH( month ))); }

	static char getContractName( const MONTH& month );
	inline static char getContractName( int month ) { return( getContractName( MONTH( month ))); }

	static bool enableSafeDateCheck( bool Enable );

	friend class GsDtDate;

private:
	static const char		s_contractName[14];			// ' ', 'f', 'g', ..., 'z', '\0'
    static const int		s_daysInMonth[13];			// [1..31]
    static const int		s_daysInMonthNonLeap[13];	// [1..31]
    static int				s_pivotYear;				// for interpreting 2-digit years, e.g., 50

	GsStatus fromString( const GsString& dateString );
    static void convertJulianToMDY( unsigned long julian,
								    char&         month,
								    char&         day,
								    short&        year );

    void addOneDay();
    void subtractOneDay();
    void subtractNDays( int days );
    void addNDays( int days );

	// This union is constructed so that dates can be compared using only the
	// m_ccyymmdd field.  Thus, the BIG/LITTLE endian tests.
	union
	{
		int m_ccyymmdd;		// 0 == only legal invalid date
		struct
		{
#if BYTE_ORDER == LITTLE_ENDIAN
			char  m_day;		// 0 or 1..31
			char  m_month;		// 0 or 1..12
			short m_year;		// 0 or 100..3000 or so
#else /* BYTE_ORDER == BIG_ENDIAN */
			short m_year;		// 0 or 100..3000 or so
			char  m_month;		// 0 or 1..12
			char  m_day;		// 0 or 1..31
#endif /* BYTE_ORDER */
		} m_data;
	};

	// Consistency verification on any date operation
	bool	internalAssertions();
};

// everything is inline, so no need for export macros
class SafeDateCheckSentry
{
public:
    SafeDateCheckSentry( bool new_value )
	  : OldEnableDateCheck( GsDate::enableSafeDateCheck( new_value ) )
	{}

	~SafeDateCheckSentry()
	{
	    GsDate::enableSafeDateCheck( OldEnableDateCheck );
	}

private:
	// undefined to prevent copying
	SafeDateCheckSentry( SafeDateCheckSentry const& );
	SafeDateCheckSentry& operator=( SafeDateCheckSentry const& );

	bool const OldEnableDateCheck;
};

#ifndef CC_NO_OVERLOADED_ENUMS
// FIX-Why do we want MONTH to behave like this?
// FIX-It would break loops like for( MONTH m=JAN; m<=DEC; m++ )
// FIX-And, it adds more code.
inline GsDate::MONTH& operator ++ ( GsDate::MONTH& m )
{
	if( m > GsDate::DECEMBER )
		m = GsDate::DECEMBER;
	return( m = GsDate::MONTH( int( m ) + 1 ));
}

// FIX-Same comments as MONTH++
inline GsDate::MONTH& operator -- ( GsDate::MONTH& m )
{
	if( m < GsDate::JANUARY )
		m = GsDate::JANUARY;
	return( m = GsDate::MONTH( int( m ) - 1 ));
}

#endif


EXPORT_CPP_GSBASE CC_OSTREAM& operator << ( CC_OSTREAM& s, const GsDate& date );
EXPORT_CPP_GSBASE CC_ISTREAM& operator >> ( CC_ISTREAM& s, GsDate& date );


/****************************************************************
**	Routine: GsDate::GsDate
**	Returns: constructor
**	Action : Contruct default, invalid date
****************************************************************/

inline GsDate::GsDate()
:	m_ccyymmdd(0)
{
}

/****************************************************************
**	Routine: GsDate::GsDate
**	Returns: constructor
**	Action : Contruct from another date
****************************************************************/

inline GsDate::GsDate( const GsDate& o )
	: m_ccyymmdd( o.m_ccyymmdd )
{
}



/****************************************************************
**	Routine: GsDate::fromDMY
**	Returns: GsDate
**	Action : Create a date from day, month, year args
****************************************************************/

inline GsDate GsDate::fromDMY(
	int day,
	int month,
	int year
)
{
	return GsDate( day, month, year );
}



/****************************************************************
**	Routine: GsDate::fromMDY
**	Returns: GsDate
**	Action : Create a date from month, day, year args
****************************************************************/

inline GsDate GsDate::fromMDY(
	int month,
	int day,
	int year
)
{
	return GsDate( day, month, year );
}



/****************************************************************
**	Routine: GsDate::fromYMD
**	Returns: GsDate
**	Action : Create a date from year, month, day args
****************************************************************/

inline GsDate GsDate::fromYMD(
	int year,
	int month,
	int day
)
{
	return GsDate( day, month, year );
}



inline GsDate& GsDate::operator = ( const GsDate& o )
{
	m_ccyymmdd = o.m_ccyymmdd;
	return *this;
}

inline int GsDate::getDayOfMonth() const
{
	return m_data.m_day;
}

inline GsDate::MONTH GsDate::getMonth() const
{
	return CC_STATIC_CAST( const GsDate::MONTH, m_data.m_month );
}

inline int GsDate::getYear() const
{
	return m_data.m_year;
}

inline bool operator < (const GsDate& d1, const GsDate& d2)
{
	return d1.m_ccyymmdd < d2.m_ccyymmdd;
}

inline bool operator <= (const GsDate& d1, const GsDate& d2)
{
	return d1.m_ccyymmdd <= d2.m_ccyymmdd;
}

inline bool operator > (const GsDate& d1, const GsDate& d2)
{
	return d1.m_ccyymmdd > d2.m_ccyymmdd;
}

inline bool operator >= (const GsDate& d1, const GsDate& d2)
{
	return d1.m_ccyymmdd >= d2.m_ccyymmdd;
}

inline bool operator == (const GsDate& d1, const GsDate& d2)
{
	return d1.m_ccyymmdd == d2.m_ccyymmdd;
}

inline bool operator != (const GsDate& d1, const GsDate& d2)
{
	return d1.m_ccyymmdd != d2.m_ccyymmdd;
}

inline GsDate::WEEKDAY GsDate::getWeekDay() const
{
   return CC_STATIC_CAST( const GsDate::WEEKDAY, getJulianDayNumber() % 7 + 1 );
}

inline GsDate& GsDate::operator ++ ()
{
   addOneDay();
   return *this;
}

inline GsDate& GsDate::operator -- ()
{
   subtractOneDay();
   return *this;
}

inline GsDate GsDate::operator ++ ( int )
{
    GsDate ret( *this );
	addOneDay();
    return ret;
}

inline GsDate GsDate::operator -- ( int )
{
    GsDate ret( *this );
	subtractOneDay();
	return ret;

}

inline GsDate& GsDate::operator += ( int days )
{
	if( days < 0 )
	{
		return *this -= (-days);
	}
	addNDays( days );
	return *this;
}

inline GsDate& GsDate::operator -= ( int days )
{
	if( days < 0 )
	{
		return *this += -days;
	}
	subtractNDays( days );
	return *this;
}

inline  GsDate operator + (const GsDate& d, int s)
{
    GsDate ret(d);
    ret+=s;
    return ret;
}

inline GsDate operator + (int s, const GsDate& d)
{
    GsDate ret(d);
    ret+=s;
    return ret;
}

inline GsDate operator - (const GsDate& d, int s)
{
    GsDate ret(d);
    ret-=s;
    return ret;
}


CC_END_NAMESPACE

#endif

