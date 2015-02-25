/****************************************************************
**
**	GsDateGen.h	-
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsDateGen.h,v 1.43 2001/11/27 22:43:09 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDATEGEN_H )
#define IN_GSDATEGEN_H

#include	<gscore/base.h>
#include	<gscore/types.h>
#include	<gscore/gsdt_fwd.h>
#include	<gscore/GsDate.h>
#include	<gscore/GsString.h>
#include	<gscore/GsPeriod.h>
#include	<gscore/GsDateVector.h>
#include	<gscore/GsDateGenRep.h>
#include	<gscore/GsDateEnums.h>
#include	<gscore/GsException.h>

CC_BEGIN_NAMESPACE( Gs )

class GsDgTable;

class EXPORT_CLASS_GSDATE GsDateGen
{
public:
	GsDateGen();
	GsDateGen( const GsDateGen& x );
	explicit GsDateGen( const GsString& name );
	GsDateGen( GsDateGenRep* rep );
	GsDateGen( const GsString& hol, const GsString& rounding, const GsString& monthIncr,
               const GsString& invalidIncr, const GsString& roll );
	~GsDateGen();

	const GsDateGen& operator = ( const GsDateGen& x );

	// Compares two m_rep pointers, sort of a proxy for operator== without a deep compare
	bool compareRepPtrs( const GsDateGen& x ) const
	{
		return ( m_rep == x.m_rep );
	}

	// date shifting functions
	GsDate addToDate(const GsDate& startDate, const GsPeriod& offset ) const
	{
		return m_rep->addToDate( startDate, offset );
	}

	GsDate addMonthsToDate(const GsDate& startDate, int months ) const
	{
		return m_rep->addMonthsToDate( startDate, months );
	}

	// holiday schedule look up functions
	bool isABusinessDate( const GsDate& date ) const
	{
		return m_rep->isABusinessDate( date );
	}

	bool isANonBusinessDate( const GsDate& date ) const
	{
		return m_rep->isANonBusinessDate( date );
	}

	int  getNumberOfBusinessDaysBetweenDates( const GsDate& start, const GsDate& end ) const
	{
		return m_rep->getNumberOfBusinessDaysBetweenDates( start, end );
	}

	int  getNumberOfNonBusinessDaysBetweenDates( const GsDate& start, const GsDate& end ) const
    {
		return m_rep->getNumberOfNonBusinessDaysBetweenDates( start, end );
	}

	// rounding convention + holiday schedule
	GsDate roundToBusinessDate( const GsDate& date ) const
	{
		return m_rep->roundToBusinessDate( date );
	}

	// periodic ( example, coupon, reset ) dates generation
	bool isAPeriodicDate( const GsPeriod& period,
						  const GsDate&   date,
						  const GsDate&   referenceDate) const
	{
		return m_rep->isAPeriodicDate( period, date, referenceDate );
	}

	GsDate getNextPeriodicDate( const GsPeriod& period,
								const GsDate&   date,
								const GsDate&   referenceDate) const
	{
		return m_rep->getNextPeriodicDate( period, date, referenceDate );
	}

	GsDate getPreviousPeriodicDate( const GsPeriod& period,
									const GsDate&   date,
									const GsDate&   referenceDate) const
	{
		return m_rep->getPreviousPeriodicDate( period, date, referenceDate );
	}

	GsDateVector generateDateSeries( const GsDate&   start,
									 bool            includeStart,
									 const GsDate&   end,
									 bool            includeEnd,
									 const GsPeriod& period,
									 const GsDate&   referenceDate) const
	{
		return m_rep->generateDateSeries( start,  includeStart,
                                          end,    includeEnd,
                                          period, referenceDate );
	}

	// period counting functions, called by GsTermCalculator
	int getNumberOfPeriodsBetweenDates( const GsPeriod& period,
										const GsDate&   start,
										const GsDate&   end,
										const GsDate&   referenceDate) const
	{
		return m_rep->getNumberOfPeriodsBetweenDates( period, start, end, referenceDate );
	}

	int getNumberOfPeriodsBetweenDates( const GsPeriod& period,
										const GsDate&   start,
										const GsDate&   end,
										const GsDate&   referenceDate,
										GsDate&         pDateBeforeStart,
										GsDate&         pDateAfterStart,
										GsDate&         pDateBeforeEnd,
										GsDate&         pDateAfterEnd) const
	{
		return m_rep->getNumberOfPeriodsBetweenDates(
				period, start, end, referenceDate, pDateBeforeStart,
				pDateAfterStart, pDateBeforeEnd, pDateAfterEnd );
	}

	GsDateGenRep*       getRepForModification();
	const GsDateGenRep* getRep() const;
	void                setRep( GsDateGenRep* rep );
	GsString			getDescription() const
	{
		return m_rep->getDescription();
	}

	static void addGenerator( const GsString& name, GsDateGen* x );
	static void addGenerator( const GsString& name, const GsDateGen& x )
	{
		addGenerator( name, new GsDateGen( x ) );
	}
	static void removeGenerator( const GsString& name );

	// GsDt support
	typedef GsDtGeneric<GsDateGen> GsDtType;
	GsString toString() const;
	GsString getShortDescription() const;

	inline static const char* typeName() { return "GsDateGen"; }

	// Streaming support.
	void StreamStore( GsStreamOut &Stream ) const;
	void StreamRead( GsStreamIn &Stream );

	// Makes it easier to create calendars from ISDA conventions
	static GsDateGen createIsdaCalendar( 
		LT_BUSINESS_DAY_CONVENTION bdc, // f, mf, or p. (from slang, use LT_BDC_ constants)
		const GsString& holidays		// New York|London, for example
	);

	LT_BUSINESS_DAY_CONVENTION bdcFromDateGen() const;

private:
	static const GsDateGen* findGenerator( const GsString& name );
	static GsDateGen* parseGenerator( const GsString& name );

	static GsDgTable& getGeneratorTable();

	GsDateGenRep* m_rep;
};


// wrapped using autofunc

EXPORT_CPP_GSDATE GsCalendar
	GsDateCalendar( GsString calendarName );

EXPORT_CPP_GSDATE
GsCalendar GsDateCalendarFromParams(
	GsString holidaySchedule,
	GsString roundingConvention,
	GsString monthIncrement,
	GsString invalidIncrement,
	GsString roll
);

EXPORT_CPP_GSDATE
GsCalendar GsDateDateGen(
	GsString holidaySchedule,
	GsString roundingConvention,
	GsString monthIncrement,
	GsString invalidIncrement,
	GsString roll
);

EXPORT_CPP_GSDATE
GsDateVector GsDateGenSeries(
	GsDateGen	dg,
	GsDate		start,
	bool		includeStart,
	GsDate		end,
	bool		includeEnd,
	GsPeriod	period,
	GsDate		referenceDate
);

EXPORT_CPP_GSDATE
GsDateTimeSeries* GsDateGenTimeSeries(
	GsDateGen	dg,
	GsDate		start,
	bool		includeStart,
	GsDate		end,
	bool		includeEnd,
	GsPeriod	period,
	GsDate		referenceDate
);

EXPORT_CPP_GSDATE
GsDate GsDateAdd(
	GsDate		start,
	GsPeriod	period,
	GsDateGen	dg
);


EXPORT_CPP_GSDATE
double GsDateDiff(
	GsDate start,
	GsDate stop
);

EXPORT_CPP_GSDATE bool
	GsDateIsABusinessDate(		GsDateGen dg, GsDate When ),
	GsDateIsANonBusinessDate(	GsDateGen dg, GsDate When );

EXPORT_CPP_GSDATE
GsDateGen GsDateCreateIsdaCalendar(
	LT_BUSINESS_DAY_CONVENTION bdc, // f, mf, or p. (from slang, use LT_BDC_ constants)
	const GsString& holidays		// New York|London, for example
);




EXPORT_CPP_GSDATE bool GsDateIsABusDay(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE bool GsDateIsANonBusDay(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE bool GsDateIsAHoliday(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE bool GsDateIsAWeekend(
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDateNextBusDay(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDateNextNonBusDay(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDateNextHoliday(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDatePrevBusDay(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDatePrevNonBusDay(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDatePrevHoliday(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDateVector GsDateListHolidays( 
	const GsString& Holidays,
	const GsDate& Start,
	const GsDate& End 
);

EXPORT_CPP_GSDATE int GsDateCountBusDays( 
	const GsString& Holidays,
	const GsDate& Start,
	const GsDate& End 
);

EXPORT_CPP_GSDATE int GsDateCountNonBusDays( 
	const GsString& Holidays,
	const GsDate& Start,
	const GsDate& End 
);

EXPORT_CPP_GSDATE GsDate GsDateModFolBdc(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDateFolBdc(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDatePrevBdc(
	const GsString& Holidays,
	const GsDate& When 
);

EXPORT_CPP_GSDATE GsDate GsDateAddPeriod(
	const GsString& Holidays,
	LT_BUSINESS_DAY_CONVENTION bdc, 
	const GsDate& Start,
	const GsPeriod& Period
);

EXPORT_CPP_GSDATE GsDate GsDateNextDay(
	const GsDate& When,					// Start<DEFAULT>GsDate::today()</DEFAULT>
	int Weekday							// 0 is anyday, 1 Mon, 2 Tue... 7 Sun<DEFAULT>0</DEFAULT>
);

EXPORT_CPP_GSDATE GsDate GsDatePrevDay(
	const GsDate& When,					// Start<DEFAULT>GsDate::today()</DEFAULT>
	int Weekday							// 0 is anyday, 1 Mon, 2 Tue... 7 Sun<DEFAULT>0</DEFAULT>
);

EXPORT_CPP_GSDATE int GsDateGetWeekDay(
	const GsDate& When 					// When, 1 is Mon, 2 is Tue... 7 is Sun<DEFAULT>GsDate::today()</DEFAULT>
);

EXPORT_CPP_GSDATE GsString GsDateYYYYMMDD(
	const GsDate& When 					// Formats When to eg 20001225<DEFAULT>GsDate::today()</DEFAULT>
);

EXPORT_CPP_GSDATE GsString GsDateDDMMMYY(
	const GsDate& When 					// Formats When to eg 06Jun00, useful for Market Prefix<DEFAULT>GsDate::today()</DEFAULT>
);


EXPORT_CPP_GSDATE GsDate GsDateLastBusDayofMonth(
	const GsString& Holidays,			// eg London or New York|London<DEFAULT>GsString("")</DEFAULT>
	const GsDate& date 					// Date
);

EXPORT_CPP_GSDATE bool GsDateIsLastBusDayofMonth(
	const GsString& Holidays,			// eg London or New York|London<DEFAULT>GsString("")</DEFAULT>
	const GsDate& date 					// Date
);



CC_END_NAMESPACE

#endif

