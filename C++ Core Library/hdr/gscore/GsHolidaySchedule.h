/****************************************************************
**
**	GsHolidaySchedule.h	-
**
**	Copyright 1998-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsHolidaySchedule.h,v 1.9 2000/01/10 20:03:51 huangp Exp $
**
****************************************************************/

#if !defined( IN_GSHOLIDAYSCHEDULE_H )
#define IN_GSHOLIDAYSCHEDULE_H

#include	<gscore/base.h>
#include	<gscore/GsHolidayScheduleRep.h>
#include	<gscore/GsString.h>
#include	<gscore/GsDate.h>
#include	<gscore/GsStream.h>

CC_BEGIN_NAMESPACE( Gs )

class GsHsTable;

class EXPORT_CLASS_GSDATE GsHolidaySchedule
{
public:
	GsHolidaySchedule();
	GsHolidaySchedule( const GsHolidaySchedule& x );
	explicit GsHolidaySchedule( const GsString& name );
	GsHolidaySchedule( GsHolidayScheduleRep* rep );
	~GsHolidaySchedule();

	const GsHolidaySchedule& operator = ( const GsHolidaySchedule& x );

	GsString getName() const
	{
		return m_rep->getName();
	}

	void setName( const GsString& name )
	{
		m_rep->setName( name );
	}

	bool isABusinessDate( const GsDate& date ) const
	{
		return m_rep->isABusinessDate( date );
	}

	bool isANonBusinessDate( const GsDate& date ) const
	{
		return m_rep->isANonBusinessDate( date );
	}

	bool isAHolidayDate( const GsDate& date ) const
	{
		return m_rep->isAHolidayDate( date );
	}

	bool isAWeekendDate( const GsDate& date ) const
	{
		return m_rep->isAWeekendDate( date );
	}

	GsString getDateDescription( const GsDate& date ) const
	{
		return m_rep->getDateDescription( date );
	}

	GsDate getNthBusinessDate( const GsDate& date, int n ) const
	{
		return m_rep->getNthBusinessDate( date, n );
	}

	GsDate getNthNonBusinessDate( const GsDate& date, int n ) const
	{
		return m_rep->getNthNonBusinessDate( date, n );
	}

	int getNumberOfBusinessDaysBetweenDates(const GsDate& beginDate,
											const GsDate& endDate) const
	{
		return m_rep->getNumberOfBusinessDaysBetweenDates( beginDate, endDate );
	}

	int getNumberOfNonBusinessDaysBetweenDates(const GsDate& beginDate,
											   const GsDate& endDate) const
	{
		return m_rep->getNumberOfNonBusinessDaysBetweenDates( beginDate, endDate );
	}

	GsDate getNextBusinessDate(const GsDate& date) const
	{
		return m_rep->getNextBusinessDate( date );
	}

	GsDate getPreviousBusinessDate(const GsDate& date) const
	{
		return m_rep->getPreviousBusinessDate( date );
	}

	GsDate getNextNonBusinessDate(const GsDate& date) const
	{
		return m_rep->getNextNonBusinessDate( date );
	}

	GsDate getPreviousNonBusinessDate(const GsDate& date) const
	{
		return m_rep->getPreviousNonBusinessDate( date );
	}

	GsDate getNextHolidayDate(const GsDate& date) const
	{
		return m_rep->getNextHolidayDate( date );
	}

	GsDate getPreviousHolidayDate(const GsDate& date) const
	{
		return m_rep->getPreviousHolidayDate( date );
	}

	void optimize();

	GsHolidayScheduleRep*       getRepForModification();
	const GsHolidayScheduleRep* getRep() const;
	void                        setRep(GsHolidayScheduleRep* rep);

	bool isCombination() const
	{
		return m_rep->isCombination();
	}

	bool isMergable( const GsHolidaySchedule& hs ) const
	{
		return m_rep->isMergable( *(hs.m_rep) );
	}

	void unionMerge( const GsHolidaySchedule& hs )
	{
		m_rep->unionMerge( *(hs.m_rep) );
	}

	void intersectionMerge( const GsHolidaySchedule& hs )
	{
		m_rep->intersectionMerge( *(hs.m_rep) );
	}

	void differenceMerge( const GsHolidaySchedule& hs )
	{
		m_rep->differenceMerge( *(hs.m_rep) );
	}

	static const GsHolidaySchedule* findSchedule( const GsString& name, bool parse=true );
	static void addSchedule( const GsString& name, const GsHolidaySchedule& hs );
	static void removeSchedule( const GsString& name );

	// Streaming support.
	void StreamStore( GsStreamOut &Stream ) const;
	void StreamRead( GsStreamIn &Stream );

private:
	static GsHsTable&        getScheduleTable();

	GsHolidayScheduleRep* m_rep;
};

GsHolidaySchedule createUnion(const GsHolidaySchedule& lop,
							  const GsHolidaySchedule& rop);

GsHolidaySchedule createIntersection(const GsHolidaySchedule& lop,
									 const GsHolidaySchedule& rop);

GsHolidaySchedule createDifference(const GsHolidaySchedule& lop,
								   const GsHolidaySchedule& rop);

CC_END_NAMESPACE

#endif

