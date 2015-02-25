/* $Header: /home/cvs/src/gsdate/src/gscore/GsTermFormula.h,v 1.8 2001/01/26 23:42:09 simpsk Exp $ */
/****************************************************************
**
**	GsTermFormula.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_GSTERMFORMULA_H )
#define IN_GSTERMFORMULA_H

#include	<gscore/base.h>
#include	<gscore/GsDate.h>
#include	<gscore/GsPTime.h>
#include    <gscore/types.h> // for GsString
#include	<new>

//class std::bad_alloc;

CC_BEGIN_NAMESPACE( Gs )

class GsDateGen;
class GsDayCounter;
class GsDaysPerUnitTerm;

class EXPORT_CLASS_GSDATE GsTermFormula
{
public:
	virtual ~GsTermFormula() ;
	
	virtual const GsString& getName() const  = 0;
	virtual const GsString& getShortName() const  = 0;
	virtual const GsString& getDescription() const  = 0;
	
	virtual GsTermFormula* clone() const  = 0;
	
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
										   const GsDate&            reference) const 
		 = 0;      
};

CC_END_NAMESPACE
		
#endif
		
		
