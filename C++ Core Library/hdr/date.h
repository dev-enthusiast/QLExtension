/* $Header: /home/cvs/src/date/src/date.h,v 1.72 2012/07/26 17:51:37 e19351 Exp $ */
/****************************************************************
**
**	DATE.H
**
**	$Revision: 1.72 $
**
****************************************************************/

#ifndef IN_DATE_H
#define IN_DATE_H
#define DATE_DEFINITIONS

#include <time.h>
#include <limits.h>

typedef long DATE;

#define BASE_YEAR		52

#define JAN				1
#define FEB				2
#define MAR				3
#define APR				4
#define MAY				5
#define JUN				6
#define JUL				7
#define AUG				8
#define SEP				9
#define OCT				10
#define NOV				11
#define DEC				12

#define MON				0
#define TUE				1
#define WED				2
#define THU				3
#define FRI				4
#define SAT				5
#define SUN				6

#define	DAYS_PER_WEEK				7L
#define SECONDS_PER_MINUTE			60L
#define SECONDS_PER_HOUR			(60L*60L)
#define SECONDS_PER_DAY				(60L*60L*24L)
#define JAN70						(6576L)
#define DATE_LOTUS_ADDEND			(18993L)
#define TIME_TO_DATE(time)			TimeToDate((time))
#define DATEHMS_TO_TIME(date,h,m,s)	DateHmsToTime((date),(h),(m),(s))
#define DATE_TO_TIME(date)			DATEHMS_TO_TIME(date,0,0,0)
#define	DATE_ERR					0L
#define	DATE_MAX					0x2fffffffL
#define	DATE_MAX_DDMMMYY				36525
#define	IS_DATE(date)				((date) > DATE_ERR && (date) <= DATE_MAX )
#define	VALID_DATE(date)			(IS_DATE(date))
#define	RDATE_ERR					DATE_ERR
#define TIME_ERR					(-1L)
#define TIME_MAX					(INT_MAX)
#define VALID_TIME(time)			((time) >= 0L && (time) <= TIME_MAX)

// TIME_TO_DATE and DATEHMS_TO_TIME convert between local dates and UTC time
#define UTCTIME_TO_DATE(time)			((time)/SECONDS_PER_DAY + JAN70)
#define DATEHMS_TO_UTCTIME(date,h,m,s)	((date) < JAN70 ? 0L : ((date) - JAN70) * SECONDS_PER_DAY + (s) + ((m) + (h) * 60L) * 60L)

/* date formats for date_atod() */
#define DATE_AMERICAN				0
#define DATE_EUROPEAN				1


/*
**	Define macros
*/

#define	DAY_OF_WEEK(d)		((d) % DAYS_PER_WEEK)
#define	DATE_IS_WEEKDAY(d)	((((d) % DAYS_PER_WEEK) <= FRI) ? 1 : 0)
#define	DATE_IS_WEEKEND(d)	((((d) % DAYS_PER_WEEK) >= SAT) ? 1 : 0)


// String sizes
#define	DATE_STRING_SIZE	8
#define	TIME_STRING_SIZE	32
#define RDATE_STRING_SIZE	32
#define RTIME_STRING_SIZE	40


/*
**	Method names for DateAdd and RDATE's
**	NOTE: Any changes should be reflected in DateMethodOrdinal in rdate.c.
*/

#define		DATE_METHOD_CALENDAR_MONTHS		'a' // like 'c' but no eom adj unless past last day of month
#define		DATE_METHOD_JULIAN_DAYS			'd' // julian days
#define		DATE_METHOD_BUSINESS_DAYS		'b' // business day
#define		DATE_METHOD_BUSINESS_DAYS_XUS	'u' // add business days ignoring implicit USD holidays
#define		DATE_METHOD_SPOT_DAYS			's' // add spot days (like b but holiday weirdness)

#define		DATE_METHOD_BUSINESS_WEEKS		'w' // weeks but taking into account holidays
#define		DATE_METHOD_BUSINESS_WEEKS_XUS	'g' // same as 'w' except ignore implicit USD holidays

#define		DATE_METHOD_BUSINESS_MONTHS		'm' // month  (with end of month trading)
#define		DATE_METHOD_BUSINESS_MONTHS_XUS	'h' // same as 'm' except ignore implicit USD holidays
#define		DATE_METHOD_BUSINESS_MONTH_DAY	'j' // add months, preserving as nth bizday of month
#define		DATE_METHOD_FWD_MONTHS			'f' // fwd dates (friday before eom is considered eom)
#define		DATE_METHOD_COUPON_MONTHS		'c' // coupon dates (calendar months)
#define		DATE_METHOD_IMM_MONTHS			'i' // IMM month
#define		DATE_METHOD_LME_MONTHS			'l' // LME month
#define		DATE_METHOD_LME_DATE			'L' // LME date (third Wednesday of month)
#define		DATE_METHOD_PHILLY_MONTHS		'p' // PHILLY month
#define		DATE_METHOD_CBOT_DATE			'C' // CBOT Treasury Option Expiration Rule
#define		DATE_METHOD_CMX_DATE			'H' // High Grade Copper CMX

#define		DATE_METHOD_EOM					'e' // end of month
#define		DATE_METHOD_BUSINESS_EOM		'x' // gets last business day of month (ignores num)
#define		DATE_METHOD_MODIFIED_FOLLOWING	'z'	// modified following business date

#define		DATE_METHOD_BUSINESS_YEARS		'y' // years
#define		DATE_METHOD_BUSINESS_YEARS_XUS	'k' // years except ignore implicit USD holidays

#define		DATE_METHOD_MONDAY				'M' // nth Monday of month
#define		DATE_METHOD_TUESDAY				'T'
#define		DATE_METHOD_WEDNESDAY			'W'
#define		DATE_METHOD_THURSDAY			'R'
#define		DATE_METHOD_FRIDAY				'F'
#define		DATE_METHOD_SATURDAY			'V'
#define		DATE_METHOD_SUNDAY				'Z'

#define		DATE_METHOD_MONDAY_RELATIVE		'N' // nth Monday relative to given date
#define		DATE_METHOD_TUESDAY_RELATIVE	'U'
#define		DATE_METHOD_WEDNESDAY_RELATIVE	'X'
#define		DATE_METHOD_THURSDAY_RELATIVE	'S'
#define		DATE_METHOD_FRIDAY_RELATIVE		'G'
#define		DATE_METHOD_SATURDAY_RELATIVE	'I'
#define		DATE_METHOD_SUNDAY_RELATIVE		'P'

#define		DATE_METHOD_FIRST_OF_MONTH		'J'	// first Julian day of month

#define		DATE_METHOD_EASTER				'E'	// Easter Sunday
#define		DATE_METHOD_WEEKEND_SPLIT 		'K' // weekend split: Sun/Mon to next biz day, otherwise the previous biz day

#define		DATE_METHOD_UNSUPPORTED 		'~'


/*
**	Holidays
*/

typedef struct DateHolidayListStructure
{
	int		Count;

	DATE	*Table;

} DATE_HOLIDAY_LIST;

typedef int DATE_HOLIDAY_CALLBACK(	const char *CalendarName, DATE_HOLIDAY_LIST *RetHolidayData );



/*
**	Relative Dates
*/

typedef DATE RDATE;

#define		RDATE_BASE						0x40000000L
#define		RDATE_OFFSET_M					0x0000FFFFL
#define		RDATE_METHOD_V					20
#define		RDATE_METHOD_M					( 0x000000FFL << RDATE_METHOD_V )
#define		RDATE_OFFSET_MIN				SHRT_MIN
#define		RDATE_OFFSET_MAX				SHRT_MAX

#define		RDATE_BUILD( Num, Method )		(RDATE) ( RDATE_BASE | ((((long) (Method)) << RDATE_METHOD_V ) & RDATE_METHOD_M ) | (((RDATE) (Num)) & RDATE_OFFSET_M ))
#define		IS_RDATE( RDate )				( ((RDATE) (RDate)) & RDATE_BASE )
#define		RDATE_OFFSET( RDate )			((short) ((RDATE) (RDate)))
#define		RDATE_METHOD( RDate )			(char) (( ((RDATE) (RDate)) & RDATE_METHOD_M ) >> RDATE_METHOD_V )


/*
**	Support for sorting rdates
*/

DLLEXPORT const int
		DateMethodOrdinal[ 62 ];			// 1 + DATE_METHOD_UNSUPPORTED - 'A'
#define	DATE_METHOD_ORDINAL( _Method )		( (_Method) < 'A' || (_Method) > DATE_METHOD_UNSUPPORTED ? DATE_METHOD_UNSUPPORTED : DateMethodOrdinal[ ( (_Method) - 'A' ) ] )
#define	RDATE_METHOD_ORDINAL( _RDate )		( IS_RDATE( _RDate ) ? DATE_METHOD_ORDINAL( RDATE_METHOD( _RDate )) : DATE_METHOD_UNSUPPORTED )

DLLEXPORT int
		RDateCompare( RDATE RDate1, RDATE RDate2, DATE BaseDate, const char *Ccy1, const char *Ccy2 );
#define	RDATE_BASE_DEFAULT					JAN70


DLLEXPORT const char
		* const DateMonthName[ 12 ];

DLLEXPORT const char
		SQ_ContractMonthLetter[];	// == " FGHJKMNQUVXZ"

/*
**	RDATE API and mixed case DATE API names
*/

DLLEXPORT DATE
		RDateAdd(			RDATE RDate, DATE Date, const char *Ccy1, const char *Ccy2 ),
		RDateSubtract(		RDATE RDate, DATE Date, const char *Ccy1, const char *Ccy2 );

DLLEXPORT char
		*RDateToString(		char *Buffer, RDATE RDate );

DLLEXPORT RDATE
		RDateFromString(	const char *String, RDATE *pRDate );

#define	DateFromString( String, pDate )										( -1 == date_atod( (String), (pDate) ) ? *((DATE *) (pDate)) : DATE_ERR )

DLLEXPORT int
		RDateIsValid( RDATE RDate );

/*
**	Relative Times
*/

typedef int RTIME_OFFSET;
typedef struct DateRTimeStructure
{
	RDATE			 RDate;

	RTIME_OFFSET	 Offset;

	char			*Location;

} RTIME;

#define RTIME_OFFSET_BASE	((RTIME_OFFSET)0)
#define RTIME_OFFSET_MIN	((RTIME_OFFSET)0)		// 00:00:00
#define RTIME_OFFSET_MAX	((RTIME_OFFSET)86399)	// 23:59:59

// RTIME API in rtime.c
DLLEXPORT RTIME
	   *RTimeCreate(			const RDATE RDate, const RTIME_OFFSET Offset, const char *Location ),
	   *RTimeCopy(				const RTIME *RTime ),
	   *RTimeFromString(		const char *String );

DLLEXPORT void
		RTimeDestroy(			RTIME *RTime );

DLLEXPORT char
	   *RTimeToString(          char *Buffer, const size_t BufferSize, const RTIME *RTime );

DLLEXPORT int
		RTimeOffsetToHMS(		const RTIME_OFFSET Offset, int *pHours, int *pMinutes, int *pSeconds );

DLLEXPORT char
	   *RTimeOffsetToString(	char *Buffer, const RTIME_OFFSET Offset );

DLLEXPORT RTIME_OFFSET
		RTimeOffsetFromString(	const char *String );



/*
**	Prototype functions
*/

// in datetime.c

typedef enum
{
	TM_FMT_DEFAULT   = 0,   // 0am-11:59am, 12-12:59, 1pm-11:59pm
	TM_FMT_1TO12AMPM = 1,	// 12am-11:59am, 12pm-11:59pm
	TM_FMT_MILITARY  = 2	// 0-23:59
} TM_FMT;

DLLEXPORT DATE
		TimeToDate( 		time_t Time );
DLLEXPORT time_t
		DateHmsToTime( 		DATE Date, int Hour, int Minute, int Second ),
		TimeLocalToGmt( 	time_t LocalTime, const char *TimeZone ),
		TimeGmtToLocal( 	time_t GmTime, const char *TimeZone );

DLLEXPORT long
		TimeSecsPastMidnight( 	time_t Time);

DLLEXPORT int
		DateHmsToTimeStruct(DATE Date, int Hour, int Minute, int Second, struct tm *TimeStruct );


// in timestr.c
DLLEXPORT char
		*TimeToString( 		char *Buffer, time_t Time, const char *TimeZone, TM_FMT TimeFmt = TM_FMT_DEFAULT );

DLLEXPORT time_t
		TimeFromString( 	const char *String );

// in time.c
DLLEXPORT time_t
		TimeFromDate( DATE Date, const char *TimeZone, int Hour, int Minute, int Second );

/* in addate.c */
DLLEXPORT DATE
		DateAdd(			int Num, char Method, DATE Dat, const char *Cal1, const char *Cal2 );


/*
**	Backward compatibility
*/

#define date_month_name	DateMonthName
#define	adate( 			Num, Method, Date, Ccy1, Ccy2 )						DateAdd( (Num), (Method), (Date), (Ccy1), (Ccy2) )
#define	idatto( 		Buffer, Date )										DateToString( (Buffer), (Date) )
#define weekday(		Date )												DateWeekdayName( (Date) )
#define datetomdy(		Date, pMonth, pDay, pYear )							DateToMdy( (Date), (pMonth), (pDay), (pYear ) )
#define mdytodat(		Month, Day, Year )			 						DateFromMdy( (Month), (Day), (Year ) )


DLLEXPORT long
		date_diff_360(		DATE date1, DATE date2);

/* in atod.c */
DLLEXPORT DATE
//		DateFromString(		const char *s, DATE *date ),	// general char string to DATE
		date_next_business(	DATE x );

DLLEXPORT int
		date_atod( 			const char *s, DATE *date ),	// obsolete: use DateFromString
		allint(				const char *s ),				/* returns 1 if s is all int's */
		date_atot(			const char *s, time_t *date);	/* general char string to time */

/* in dates.c */
DLLEXPORT char
		*DateToString(		char *OutBuf, DATE Date );		// DATE to formatted string

DLLEXPORT const char
		*DateWeekdayName(	DATE Date );

DLLEXPORT DATE
		DateToday(			void ),							// return todays date
		date(				void ),							// obsolete: use DateToday
		DateFromMdy(		int Month, int Day, int Year ),	// m,d,y to DATE
		dayc(				DATE *idat );					// int[](m,d,y) to DATE

DLLEXPORT void
		DateToMdy(			DATE Date, int *pMonth, int *pDay, int *pYear ),
		dayu(				DATE *out, DATE datin );

DLLEXPORT int
		DateYearIsLeapYear(	int Year );

DLLEXPORT int
		IsDSTChangeDate( DATE Date, char *pszTimeZone ),
		IsDST( DATE Date, int Hour, char *pszTimeZone );

DLLEXPORT long
		CountWeekdays(		DATE Date1, DATE Date2 );

DLLEXPORT int
		DateLastDayOfMonth(	DATE Date );

/* in holiday.c */
DLLEXPORT DATE
		non_holiday(					const char *currency_name, DATE d ),
		DateNonHoliday2(				DATE d, const char *Calendar1, const char *Calendar2 );

DLLEXPORT int
		date_holiday_cache(				int n ),			// obsolete
		holiday(						const char *currency_name, DATE d ),
        weekendholiday(                 const char *currency_name, DATE d );


DLLEXPORT void
		DateHolidaysRegisterCallback(	DATE_HOLIDAY_CALLBACK Callback ),
		DateHolidaysRefresh( 			char *CalendarName );

DLLEXPORT long
		DateHolidaysMemSize(	   		void );

DLLEXPORT DATE_HOLIDAY_LIST
        *DateLoadHolidayData(			const char *CalendarName ),
        *DateLoadWeekendHoliday(        const char *CalendarName );


/* in landate.c */
DLLEXPORT int
		isonlan(	void );

DLLEXPORT DATE
		landate(	void );

/* in moddate.c */
DLLEXPORT DATE
		date_modified(			char *file_name );

DLLEXPORT long
		time_modified(			char *file_name );

/* in stdconv.c */
DLLEXPORT DATE
		std_conv(				char *in );				/* YYYYMMDD to DATE */

DLLEXPORT void
		std_uconv(				char *out, DATE day );	/*  DATE to YYYYMMDD*/

/* in ipol.c */
DLLEXPORT void
		linear_interpolation(	int n, double *x_array, double *y_array, double x, double *py );

/* in daterule.c */
DLLEXPORT DATE
		DateRuleApply(			DATE Date, const char *Rule, const char *Cal1, const char *Cal2 );

DLLEXPORT int
		DateRuleCheck(			const char *DateRule );

/* in sleep.c */
#if defined( BORLAND ) || defined( WATCOM )
#	include <dos.h>
#elif defined( MICROSOFT ) || defined( IBM_CPP )
	extern void sleep(	int Seconds		);
#endif

#endif
