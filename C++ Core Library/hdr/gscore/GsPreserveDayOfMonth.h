/* $Header: /home/cvs/src/gsdate/src/gscore/GsPreserveDayOfMonth.h,v 1.5 1999/11/04 22:13:28 francm Exp $ */
/****************************************************************
**
**	GsPreserveDayOfMonth.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_GSPRESERVEDAYOFMONTH_H )
#define IN_GSPRESERVEDAYOFMONTH_H

#include	<gscore/base.h>
#include	<gscore/GsMonthIncrementingMethod.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsPreserveDayOfMonth : public GsMonthIncrementingMethod
{
   public:
      virtual const GsString& getName() const ;
      virtual const GsString& getDescription() const ;
      
      virtual GsMonthIncrementingMethod* clone() const ;
      
      virtual GsDate mapDateIntoMonth(const GsDate& date, 
				  GsDate::MONTH destMonth, int destYear,	
				  const GsInvalidIncrementedDateConvention& iidc) const 
		  ;
};

CC_END_NAMESPACE

#endif
