/****************************************************************
**
**	GsTermCalcRep.h	- Abstract base class for all Term Calc reps
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**  $Header: /home/cvs/src/gsdate/src/gscore/GsTermCalcRep.h,v 1.8 1999/11/04 22:13:30 francm Exp $
**
****************************************************************/

#if !defined( IN_GSTERMCALCREP_H )
#define IN_GSTERMCALCREP_H

#include	<gscore/base.h>
#include	<gscore/GsPTime.h>
#include	<gscore/GsPTimeVector.h>
#include	<gscore/GsCalendar.h>


CC_BEGIN_NAMESPACE( Gs )

class GsDate;
class GsDateGen;
class GsDateVector;

class EXPORT_CLASS_GSDATE GsTermCalcRep
{
public:
	GsTermCalcRep();
	GsTermCalcRep( const GsTermCalcRep& x );
	virtual ~GsTermCalcRep();

	virtual GsTermCalcRep* clone() const = 0;

	virtual int getDaysBetweenDates( const GsDate& beginDate,
									 const GsDate& endDate ) const = 0;

	virtual int getDaysBetweenDatesNC( const GsDate& beginDate,
									   const GsDate& endDate) const = 0;

	virtual GsPTime getTermBetweenDates(int           frequency,
										const GsDate& beginDate,
										const GsDate& endDate,
										const GsDate& reference) const = 0;


    virtual GsPTime getTermBetweenDatesNC(int           frequency,
										  const GsDate& beginDate,
										  const GsDate& endDate,
										  const GsDate& reference) const = 0;

    virtual GsPTimeVector datesToTerms(int                 frequency,
									   const GsDate&       beginDate,
									   const GsDateVector& endDates,
									   const GsDate&       reference) const = 0;

    virtual GsPTimeVector datesToTermsNC(int                 frequency,
										 const GsDate&       beginDate,
										 const GsDateVector& endDates,
										 const GsDate&       reference) const = 0;

    virtual int getNumberOfDaysPerUnitTerm(int year) const = 0;

	virtual GsString toString() const = 0;

	virtual GsString getDayCount() const = 0;
	virtual GsString getDaysPerTerm() const = 0;
	virtual GsString getTermFormula() const = 0;

	friend class GsTermCalc;

	unsigned getCount() { return m_count; }

	const GsCalendar& getCalendar() { return m_calendar; }

protected:
	GsTermCalcRep( const GsString& Name, const GsString& Desc, const GsCalendar& Cal );
	GsString	m_name;
	GsString	m_description;
	GsCalendar	m_calendar;

private:
	void incCount()  { ++m_count; }
	void decCount()  { --m_count; }
	bool isUniqueRep() const  { return m_count == 1; }
	bool hasReferences() const  { return m_count != 0; }

	unsigned m_count;
};

CC_END_NAMESPACE

#endif
