/****************************************************************
**
**	GsTermCalc.h	-
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsTermCalc.h,v 1.26 1999/11/29 19:57:42 lischf Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSTERMCALC_H )
#define IN_GSCORE_GSTERMCALC_H

#include	<gscore/base.h>
#include	<gscore/gsdt_fwd.h>
#include	<gscore/GsPTime.h>
#include	<gscore/GsPTimeVector.h>
#include	<gscore/GsTermCalcRep.h>

CC_BEGIN_NAMESPACE( Gs )

class GsDate;
class GsDateVector;
class GsXInvalidArgument;
class GsTcTable;
class GsTermCalc;

class EXPORT_CLASS_GSDATE GsTermCalc
{
public:
	GsTermCalc();
	GsTermCalc( const GsTermCalc& x );
	explicit GsTermCalc( const GsString& name );
	GsTermCalc( const GsDateGen& Cal, const GsString& dayCount,
				const GsString& daysPerTerm, const GsString& termFormula );
	GsTermCalc( const GsString& holidaySchedule, const GsString& roundingConvention,
                const GsString& monthIncrement, const GsString& invalidIncrement,
                const GsString& roll, const GsString& dayCount,
				const GsString& daysPerTerm, const GsString& termFormula );
	GsTermCalc( GsTermCalcRep* rep );
	~GsTermCalc();


	GsTermCalc& operator =( const GsTermCalc& x );

	int getDaysBetweenDates( const GsDate& beginDate, const GsDate& endDate ) const
	{
		return m_rep->getDaysBetweenDates( beginDate, endDate );
	}

	int getDaysBetweenDatesNC( const GsDate& beginDate, const GsDate& endDate ) const
	{
		return m_rep->getDaysBetweenDatesNC( beginDate, endDate );
	}

	GsPTime getTermBetweenDates(int           frequency,
								const GsDate& beginDate,
								const GsDate& endDate,
								const GsDate& reference ) const
	{
		return m_rep->getTermBetweenDates( frequency, beginDate, endDate, reference );
	}


	GsPTime getTermBetweenDatesNC(int           frequency,
								  const GsDate& beginDate,
								  const GsDate& endDate,
								  const GsDate& reference) const
    {
		return m_rep->getTermBetweenDatesNC( frequency, beginDate, endDate, reference );
	}

	GsPTimeVector datesToTerms(int                 frequency,
							   const GsDate&       beginDate,
							   const GsDateVector& endDates,
							   const GsDate&       reference) const
	{
		return m_rep->datesToTerms( frequency, beginDate, endDates, reference );
	}

	GsPTimeVector datesToTermsNC(int                 frequency,
								 const GsDate&       beginDate,
								 const GsDateVector& endDates,
								 const GsDate&       reference) const
	{
		return m_rep->datesToTermsNC( frequency, beginDate, endDates, reference );
	}

	int getNumberOfDaysPerUnitTerm(int year) const
	{
		return m_rep->getNumberOfDaysPerUnitTerm( year );
	}

	static void addCalculator( const GsString& name, const GsTermCalc& x );
	static void removeCalculator( const GsString& name );

	typedef GsDtGeneric<GsTermCalc> GsDtType;
	GsString toString() const;
	inline static const char* typeName() { return "GsTermCalc"; }


	const GsCalendar& getCalendar() const	{ return m_rep->getCalendar(); }
	const GsString getDaysPerTerm() const	{ return m_rep->getDaysPerTerm(); }
	const GsString getDayCount() const		{ return m_rep->getDayCount(); }
	const GsString getTermFormula() const	{ return m_rep->getTermFormula(); }

private:
	// Rep functions I don't think anyone uses
	const GsTermCalcRep* getRep() const;
	void                 setRep( GsTermCalcRep* rep );
	GsTermCalcRep* getRepForModification();

	static GsTcTable& getCalculatorTable();
	static const GsTermCalc* findCalculator( const GsString& name );

	GsTermCalcRep* m_rep;
};


// wrapped using autofunc

EXPORT_CPP_GSDATE GsTermCalc
	GsDateTermCalcFromName( const GsString& termCalcName );

EXPORT_CPP_GSDATE GsTermCalc GsDateTermCalc(
	const GsString& holidaySchedule,
	const GsString& roundingConvention,
	const GsString& monthIncrement,
	const GsString& invalidIncrement,
	const GsString& roll,
	const GsString& dayCount,
	const GsString& daysPerTerm,
	const GsString& termFormula
	);

EXPORT_CPP_GSDATE GsTermCalc GsDateTermCalcFromCal(
	const GsCalendar& calendar,
	const GsString& dayCount,
	const GsString& daysPerTerm,
	const GsString& termFormula
	);

EXPORT_CPP_GSDATE GsCalendar
	GsDateTermCalcGetCalendar( const GsTermCalc& termCalc );

EXPORT_CPP_GSDATE GsTermCalc
	GsDateTermCalcChangeCalendar(
		const GsTermCalc&	termCalc,
		const GsCalendar&	calendar
	),
	GsDateTermCalcChangeDayCount(
		const GsTermCalc&	termCalc,
		const GsString&		dayCount
	),
	GsDateTermCalcChangeDaysPerTerm(
		const GsTermCalc&	termCalc,
		const GsString&		daysPerTerm
	),
	GsDateTermCalcChangeTermFormula(
		const GsTermCalc&	termCalc,
		const GsString& 	termFormula
	);


EXPORT_CPP_GSDATE
double GsDateGetTerm(
	const GsDate& start,
	const GsDate& end,
	int frequency,
	const GsDate& reference,
	const GsTermCalc&
	);

EXPORT_CPP_GSDATE GsString GsDateTermCalcGetDaysPerTerm( const GsTermCalc& );
EXPORT_CPP_GSDATE GsString GsDateTermCalcGetDayCount( const GsTermCalc& );


CC_END_NAMESPACE

#endif
