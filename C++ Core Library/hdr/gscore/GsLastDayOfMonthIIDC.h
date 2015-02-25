/* $Header: /home/cvs/src/gsdate/src/gscore/GsLastDayOfMonthIIDC.h,v 1.4 1999/11/04 22:13:25 francm Exp $ */
/****************************************************************
**
**	GsLastDayOfMonthIIDC.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_GSLASTDAYOFMONTHIIDC_H )
#define IN_GSLASTDAYOFMONTHIIDC_H

#include	<gscore/base.h>
#include	<gscore/GsInvalidIncrementedDateConvention.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsLastDayOfMonthIIDC : public GsInvalidIncrementedDateConvention
{
   public:
      virtual const GsString& getName() const ;
      virtual const GsString& getDescription() const ;
      
      virtual GsInvalidIncrementedDateConvention* clone() const ;
      
      virtual GsDate apply(int day, GsDate::MONTH month, int year) const
		  ;
};

CC_END_NAMESPACE

#endif










