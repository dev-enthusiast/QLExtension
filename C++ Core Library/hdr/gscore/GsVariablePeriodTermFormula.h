/****************************************************************
**
**	GsVariablePeriodTermFormula.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsVariablePeriodTermFormula.h,v 1.8 1999/11/04 22:13:31 francm Exp $
**
****************************************************************/

#if !defined( IN_GSVARIABLEPERIODTERMFORMULA_H )
#define IN_GSVARIABLEPERIODTERMFORMULA_H

#include	<gscore/base.h>
#include	<gscore/GsTermFormula.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsVariablePeriodTermFormula: public GsTermFormula
{
   public:
      virtual const GsString& getName() const ;
      virtual const GsString& getShortName() const ;
      virtual const GsString& getDescription() const ;  

      virtual GsTermFormula* clone() const ;
      
      // Compute terms assuming regular periodic dates
      // that can be calculated using the frequency and
      // reference dates.

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



