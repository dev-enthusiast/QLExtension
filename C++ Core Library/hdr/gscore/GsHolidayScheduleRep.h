/* $Header: /home/cvs/src/gsdate/src/gscore/GsHolidayScheduleRep.h,v 1.3 1999/11/04 22:13:24 francm Exp $ */
/****************************************************************
**
**	GsHolidayScheduleRep.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_GSHOLIDAYSCHEDULEREP_H )
#define IN_GSHOLIDAYSCHEDULEREP_H

#include	<gscore/base.h>
#include	<gscore/GsString.h>
#include	<gscore/GsStream.h>
#include	<gscore/GsDate.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsHolidayScheduleRep
{
public:
	GsHolidayScheduleRep();
	GsHolidayScheduleRep( const GsHolidayScheduleRep& other );
	virtual ~GsHolidayScheduleRep() ;

	virtual GsHolidayScheduleRep* clone() const = 0;

	virtual GsString getName() const  = 0;
	virtual void setName(const GsString& name) = 0;

	virtual bool isABusinessDate( const GsDate& date ) const;
	virtual bool isANonBusinessDate( const GsDate& date ) const;

	virtual bool isAHolidayDate( const GsDate& date ) const = 0;
	virtual bool isAWeekendDate( const GsDate& date ) const;

	virtual GsString getDateDescription( const GsDate& date ) const;

	virtual GsDate getNthBusinessDate( const GsDate& date, int n ) const;
	virtual GsDate getNthNonBusinessDate( const GsDate& date, int n ) const;

	virtual int getNumberOfBusinessDaysBetweenDates( const GsDate& beginDate,
												     const GsDate& endDate ) const;

	virtual int getNumberOfNonBusinessDaysBetweenDates( const GsDate& beginDate,
													    const GsDate& endDate ) const;
      
	virtual GsDate getNextBusinessDate( const GsDate& date ) const;
	virtual GsDate getPreviousBusinessDate( const GsDate& date ) const;

	virtual GsDate getNextNonBusinessDate( const GsDate& date ) const;
	virtual GsDate getPreviousNonBusinessDate( const GsDate& date ) const;

	virtual GsDate getNextHolidayDate( const GsDate& date ) const;
	virtual GsDate getPreviousHolidayDate( const GsDate& date ) const;

	virtual void optimize();

	friend class GsHolidaySchedule;

//protected:
	virtual bool isCombination() const;
	virtual bool isMergable( const GsHolidayScheduleRep& x ) const; 
	virtual void unionMerge( const GsHolidayScheduleRep& x ) = 0;
	virtual void intersectionMerge( const GsHolidayScheduleRep& x ) = 0;
	virtual void differenceMerge( const GsHolidayScheduleRep& x ) = 0;

	// Streaming support.
	virtual GsLocalTypeID LocalTypeID() const = 0;
	virtual void StreamStore( GsStreamOut &Stream ) const;
	virtual void StreamRead( GsStreamIn &Stream );
	static GsHolidayScheduleRep *LocalEmptyInstance( GsLocalTypeID LocalTypeID );

private:
	void incCount() { ++m_count; }
	void decCount() { --m_count; }
	bool isUniqueRep() const { return m_count == 1; }
	bool hasReferences() const { return m_count != 0; }

	unsigned m_count;
};

CC_END_NAMESPACE

#endif

