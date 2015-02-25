/* $Header: /home/cvs/src/gsdate/src/gscore/GsMonthIncrementingMethod.h,v 1.8 2001/01/26 23:42:09 simpsk Exp $ */
/****************************************************************
**
**	GsMonthIncrementingMethod.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_GSMONTHINCREMENTINGMETHOD_H )
#define IN_GSMONTHINCREMENTINGMETHOD_H

#include	<gscore/base.h>
#include	<gscore/GsDate.h>
#include	<gscore/GsStream.h>
#include    <gscore/types.h>
#include	<new>

//class std::bad_alloc;

CC_BEGIN_NAMESPACE( Gs )

class GsInvalidIncrementedDateConvention;
class GsXInvalidArgument;

class EXPORT_CLASS_GSDATE GsMonthIncrementingMethod
{
public:
      virtual ~GsMonthIncrementingMethod() ;

      virtual const GsString& getName() const  = 0;
      virtual const GsString& getDescription() const  = 0;

      virtual GsMonthIncrementingMethod* clone() const = 0;

      virtual GsDate mapDateIntoMonth(const GsDate& originalDate, 
									  GsDate::MONTH destinationMonth,	
									  int destinationYear,
									  const GsInvalidIncrementedDateConvention& iidc) const 
		   = 0;

      virtual void addMonths(GsDate::MONTH inputMonth, int inputYear, int monthsToAdd,
							 GsDate::MONTH& outputMonth, int& outputYear ) const 
		  ;


      virtual GsDate addMonths(const GsDate& date, int months,
							   const GsInvalidIncrementedDateConvention& iidc) const 
		  ;

	// Streaming support.
	virtual void StreamStore( GsStreamOut &Stream ) const;
	virtual void StreamRead( GsStreamIn &Stream );

};

CC_END_NAMESPACE

#endif
