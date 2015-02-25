/* $Header: /home/cvs/src/gsdate/src/gscore/GsFixedPeriodTermFormula.h,v 1.6 1999/11/04 22:13:24 francm Exp $ */
/****************************************************************
**
**	GsFixedPeriodTermFormula.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_GSFIXEDPERIODTERMFORMULA_H )
#define IN_GSFIXEDPERIODTERMFORMULA_H

#include	<gscore/GsTermFormula.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsFixedPeriodTermFormula : public GsTermFormula
{
public:      
      virtual const GsString& getName() const ;
      virtual const GsString& getShortName() const ;
      virtual const GsString& getDescription() const ;

      virtual GsTermFormula* clone() const ;
      
      // Compute terms assuming regular periodic dates
      // that can be calculated using the frequency and
      // reference dates.

      virtual GsPTime getTermsBetweenDates(const GsDateGen&         generator,
										  const GsDayCounter&      dayCounter,
										  const GsDaysPerUnitTerm& daysPerTerm,
										  int                      frequency,
										  const GsDate&            begin,
										  const GsDate&            end,
										  const GsDate&            reference) const
		  ;

      virtual GsPTime getTermsBetweenDatesNC(const GsDateGen&         generator,
											const GsDayCounter&      dayCounter,
											const GsDaysPerUnitTerm& daysPerTerm,
											int                      frequency,
											const GsDate&            begin,
											const GsDate&            end,
											const GsDate&            reference) const ;
};

CC_END_NAMESPACE

#endif



