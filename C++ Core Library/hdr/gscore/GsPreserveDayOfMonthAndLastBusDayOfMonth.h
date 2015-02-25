/* $Header: /home/cvs/src/gsdate/src/gscore/GsPreserveDayOfMonthAndLastBusDayOfMonth.h,v 1.1 2000/10/13 18:38:13 benhae Exp $ */
/****************************************************************
**
**	GsPreserveDayOfMonthAndLastBusDayOfMonth.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$log$
**
****************************************************************/

#if !defined( IN_GSPRESERVEDAYOFMONTHANDLASTBUSDAYOFMONTH_H )
#define IN_GSPRESERVEDAYOFMONTHANDLASTBUSDAYOFMONTH_H

#include	<gscore/base.h>
#include	<gscore/GsMonthIncrementingMethod.h>

CC_BEGIN_NAMESPACE( Gs )


EXPORT_CPP_GSDATE GsDate GsDateLastBusDayofMonth(
	const GsString& Holidays,			// eg London or New York|London<DEFAULT>GsString("")</DEFAULT>
	const GsDate& date 					// Date
);

EXPORT_CPP_GSDATE bool GsDateIsLastBusDayofMonth(
	const GsString& Holidays,			// eg London or New York|London<DEFAULT>GsString("")</DEFAULT>
	const GsDate& date 					// Date
);

class EXPORT_CLASS_GSDATE GsPreserveDayOfMonthAndLastBusDayOfMonth 
      : public GsMonthIncrementingMethod
{
public:
	GsPreserveDayOfMonthAndLastBusDayOfMonth( GsString holidaySchedule ):  
	  m_holidaySchedule( holidaySchedule ) {;};
	virtual const GsString& getName() const ;
	virtual const GsString& getDescription() const ;
      
	virtual GsMonthIncrementingMethod* clone() const ;
      
	virtual GsDate mapDateIntoMonth(const GsDate& date, 
									GsDate::MONTH destMonth, int destYear,
									const GsInvalidIncrementedDateConvention& iidc) const ;
private:
	// constructor private so that only construction with a holiday
	GsPreserveDayOfMonthAndLastBusDayOfMonth( ) {;} ;
	GsString m_holidaySchedule;



};

CC_END_NAMESPACE

#endif



