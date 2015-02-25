/****************************************************************
**
**	GsDateGenRep.h	-
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsDateGenRep.h,v 1.13 1999/10/29 14:44:24 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSDATEGENREP_H )
#define IN_GSCORE_GSDATEGENREP_H

#include	<gscore/base.h>
#include	<gscore/types.h>
#include	<gscore/GsDate.h>
#include	<gscore/GsDateVector.h>

CC_BEGIN_NAMESPACE( Gs )

class GsPeriod;

class EXPORT_CLASS_GSDATE GsDateGenRep
{
public:
	GsDateGenRep();
	GsDateGenRep(const GsDateGenRep& x);
	virtual ~GsDateGenRep();

	virtual GsDateGenRep* clone() const = 0;

	// date shifting functions
	virtual GsDate addToDate(const GsDate& startDate, const GsPeriod& offset ) const = 0;
	virtual GsDate addMonthsToDate(const GsDate& startDate, int months ) const = 0;

	// holiday schedule look up functions
	virtual bool isABusinessDate( const GsDate& date ) const = 0;
	virtual bool isANonBusinessDate( const GsDate& date ) const = 0;

	virtual int  getNumberOfBusinessDaysBetweenDates( const GsDate& start, const GsDate& end ) const = 0;
	virtual int  getNumberOfNonBusinessDaysBetweenDates( const GsDate& start, const GsDate& end ) const = 0;

	// rounding convention + holiday schedule
	virtual GsDate roundToBusinessDate( const GsDate& date ) const = 0;

	// periodic ( example, coupon, reset ) dates generation
	virtual bool isAPeriodicDate( const GsPeriod& period,
								  const GsDate&   date,
								  const GsDate&   referenceDate) const = 0;

	virtual GsDate getNextPeriodicDate( const GsPeriod& period,
										const GsDate&   date,
										const GsDate&   referenceDate) const = 0;

	virtual GsDate getPreviousPeriodicDate( const GsPeriod& period,
											const GsDate&   date,
											const GsDate&   referenceDate) const = 0;

	virtual  GsDateVector generateDateSeries( const GsDate&   start,
											  bool            includeStart,
											  const GsDate&   end,
											  bool            includeEnd,
											  const GsPeriod& period,
											  const GsDate&   referenceDate) const = 0;

	// period counting functions, called by GsTermCalculator
	virtual int getNumberOfPeriodsBetweenDates( const GsPeriod& period,
												const GsDate&   start,
												const GsDate&   end,
												const GsDate&   referenceDate) const = 0;

	virtual int getNumberOfPeriodsBetweenDates( const GsPeriod& period,
												const GsDate&   start,
												const GsDate&   end,
												const GsDate&   referenceDate,
												GsDate&         pDateBeforeStart,
												GsDate&         pDateAfterStart,
												GsDate&         pDateBeforeEnd,
												GsDate&         pDateAfterEnd) const = 0;

	// Attribute access
	virtual GsString toString() const = 0;
	virtual GsString getDescription() const = 0;
	virtual GsString getShortDescription() const = 0;

	friend class GsDateGen;

	// Streaming support.
	virtual GsLocalTypeID LocalTypeID() const = 0;
	virtual void StreamStore( GsStreamOut &Stream ) const;
	virtual void StreamRead( GsStreamIn &Stream );
	static GsDateGenRep *LocalEmptyInstance( GsLocalTypeID LocalTypeID );

private:
	void incCount() { ++m_count; }
	void decCount() { --m_count; }
	bool isUniqueRep() const { return m_count == 1; }
	bool hasReferences() const { return m_count != 0; }

	unsigned m_count;
};

CC_END_NAMESPACE

#endif










