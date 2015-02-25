/****************************************************************
**
**	GsDaysPerUnitTerm.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsDaysPerUnitTerm.h,v 1.9 2001/08/30 21:30:51 coulsd Exp $
**
****************************************************************/

#ifndef IN_GSDAYSPERUNITTERM_H
#define IN_GSDAYSPERUNITTERM_H

#include	<gscore/base.h>
#include	<gscore/GsDate.h>
#include	<gscore/GsString.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsDaysPerUnitTerm
{
public:
	virtual ~GsDaysPerUnitTerm();
      
	virtual const GsString& getShortName() const = 0;
	virtual const GsString& getName() const = 0;
	virtual const GsString& getDescription() const = 0;
      
	virtual GsDaysPerUnitTerm* clone() const = 0;
      
	virtual double getDaysPerUnitTerm( int year ) const = 0;
	virtual bool   isConstantDaysPerUnitTerm() const = 0;
      
	virtual GsDate addYears( const GsDate& date, int years ) const;
};


#define GSDATE_DECLARE_DPT(_class_)		\
	class EXPORT_CLASS_GSDATE _class_ : public GsDaysPerUnitTerm	\
	{	\
	public:	\
		virtual ~_class_();								\
		virtual const GsString& getShortName() const;	\
		virtual const GsString& getName() const;		\
		virtual const GsString& getDescription() const;	\
		virtual GsDaysPerUnitTerm* clone() const;		\
		virtual double getDaysPerUnitTerm( int year ) const;	\
		virtual bool   isConstantDaysPerUnitTerm() const;		\
	}

/*
**	Declare all of the DaysPerTerm classes here.
*/

GSDATE_DECLARE_DPT( Gs360DaysPerTerm );
GSDATE_DECLARE_DPT( Gs365DaysPerTerm );
GSDATE_DECLARE_DPT( Gs365_25DaysPerTerm );
GSDATE_DECLARE_DPT( GsActualYearPerTerm );
GSDATE_DECLARE_DPT( GsGregorianYearPerTerm );
GSDATE_DECLARE_DPT( GsSolarYearPerTerm );
GSDATE_DECLARE_DPT( Gs1DayPerTerm );
GSDATE_DECLARE_DPT( Gs2DayPerTerm );

CC_END_NAMESPACE

#endif
