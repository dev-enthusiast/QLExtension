/* $Header: /home/cvs/src/gsdate/src/gscore/GsSIATermFormula.h,v 1.7 1999/11/04 22:13:30 francm Exp $ */
/****************************************************************
**
**	GsSIATermFormula.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_GSSIATERMFORMULA_H )
#define IN_GSSIATERMFORMULA_H

#include	<gscore/GsFixedPeriodTermFormula.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsSIATermFormula : public GsFixedPeriodTermFormula
{
   public:
      virtual const GsString& getName() const ;
      virtual const GsString& getShortName() const ;
      virtual const GsString&  getDescription() const ;  
  
      virtual GsTermFormula* clone() const ;
      
      // Compute terms assuming regular periodic dates
      // that can be calculated using the frequency and
      // reference dates and the SIA rule that the days
      // remaining in a coupon period is equal to the
      // number of days since the previous coupon subtracted
      // from the days in one coupon period.

      virtual GsPTime getTermsBetweenDatesNC(const GsDateGen&         generator,
											const GsDayCounter&      dayCounter,
											const GsDaysPerUnitTerm& daysPerTerm,
											int                      frequency,
											const GsDate&            beginDate,
											const GsDate&            endDate,
											const GsDate&            reference) const ;
};

CC_END_NAMESPACE

#endif











