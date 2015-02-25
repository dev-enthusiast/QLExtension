/****************************************************************
**
**	GsDayCounter.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsDayCounter.h,v 1.9 2001/11/27 22:43:09 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDAYCOUNTER_H )
#define IN_GSDAYCOUNTER_H

#include	<gscore/base.h>
#include	<gscore/types.h>
#include	<gscore/GsDate.h>
#include	<new>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsDayCounter 
** Description : An object that represents the "numerator", the number of days in 
**               a month. This abstract base class defines the interface of the 
**               the classes that derive from it.
****************************************************************/

class EXPORT_CLASS_GSDATE GsDayCounter
{
public: 
    virtual ~GsDayCounter();

    virtual const GsString&  getShortName() const = 0;
    virtual const GsString&  getName() const = 0;
    virtual const GsString&  getDescription() const = 0;

    virtual GsDayCounter* clone() const = 0;

    virtual int		getDaysInMonth( GsDate::MONTH, int year) const = 0;
    virtual int		getDaysBetweenDates( const GsDate& date1, const GsDate& date2 ) const = 0;
    virtual GsDate	addDaysToDate( const GsDate& date, int days ) const = 0;

protected:
    GsDate addDaysTo30360CalendarDate(int month, int day, int year, int days) const;
};

#define GSDATE_DC_DECLARE( _class_ )	\
	class EXPORT_CLASS_GSDATE _class_ : public GsDayCounter	\
	{	\
	public:	\
		virtual const GsString& getShortName() const;	\
		virtual const GsString& getName() const;		\
		virtual const GsString& getDescription() const;	\
		virtual GsDayCounter* clone() const;			\
		virtual int		getDaysInMonth( GsDate::MONTH, int year) const;			\
		virtual int		getDaysBetweenDates( const GsDate& date1, const GsDate& date2 ) const;	\
		virtual GsDate	addDaysToDate( const GsDate& date, int days ) const;	\
	}

GSDATE_DC_DECLARE( GsActualDayCounter );
GSDATE_DC_DECLARE( GsActualNoLeapDayCounter );
GSDATE_DC_DECLARE( GsISDA30DayCounter );
GSDATE_DC_DECLARE( GsISDA30EDayCounter );
GSDATE_DC_DECLARE( GsPSA30DayCounter );
GSDATE_DC_DECLARE( GsSIA30DayCounter );
GSDATE_DC_DECLARE( GsISDA1DayCounter );


#undef GSDATE_DC_DECLARE

CC_END_NAMESPACE

#endif

