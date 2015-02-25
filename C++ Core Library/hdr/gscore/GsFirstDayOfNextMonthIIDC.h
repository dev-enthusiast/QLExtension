/* $Header: /home/cvs/src/gsdate/src/gscore/GsFirstDayOfNextMonthIIDC.h,v 1.4 1999/11/04 22:13:23 francm Exp $ */
/****************************************************************
**
**	GsFirstDayOfNextMonthIIDC.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_GSFIRSTDAYOFNEXTMONTHIIDC_H )
#define IN_GSFIRSTDAYOFNEXTMONTHIIDC_H

#include	<gscore/base.h>
#include	<gscore/GsInvalidIncrementedDateConvention.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsFirstDayOfNextMonthIIDC : public GsInvalidIncrementedDateConvention
{
   public:
      virtual const GsString& getName() const ;
      virtual const GsString& getDescription() const ;
      
      virtual GsInvalidIncrementedDateConvention* clone() const ;

      virtual GsDate apply(int day, GsDate::MONTH month, int year) const ;
};

CC_END_NAMESPACE

#endif










