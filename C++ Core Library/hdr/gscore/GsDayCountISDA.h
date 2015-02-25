/****************************************************************
**
**	GsDayCountISDA.H	- GsDayCountISDA class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsDayCountISDA.h,v 1.15 2011/05/17 16:23:26 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSDATCOUNTISDA_H )
#define IN_GSDATE_GSDATCOUNTISDA_H

#include <gscore/base.h>
#include <gscore/GsDayCounter.h>
#include <gscore/GsDaysPerUnitTerm.h>
#include <gscore/GsDate.h>
#include <gscore/GsTermCalc.h>
// #include	<gsquant/Enums.h> // Taking this out due to circular build dep, putting enums into gsdateenums.h
#include	<gscore/GsDateEnums.h>
#include <hash.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsDayCountISDA 
{	
   	GsDayCounter
			*m_Numerator;
	
	GsDaysPerUnitTerm
			*m_Denominator;

	GsString
			m_TermFormula; // "nptf" for example

	GsString
			m_ISDAString;

	GsTermCalc
			m_TermCalc;

    void setFromString( const GsString& );

	GsDayCountISDA( 
		const GsString& ISDAString, 
		const GsDayCounter& Numerator, 
		const GsDaysPerUnitTerm& Denominator, 
		const GsString& TermFormula );

	static HASH* getDayCountFactory();
	static HASH* createDayCountFactory();

public:
	
	typedef GsDtDayCountISDA GsDtType;

	GsDayCountISDA();
	GsDayCountISDA( const GsString& DayCountString );
	GsDayCountISDA( const GsDayCountISDA& Rhs);
	
	~GsDayCountISDA();

	GsDayCountISDA& operator=( const GsDayCountISDA& Rhs);
	GsDayCountISDA& operator=( const GsString& DayCountString );

	bool operator==( const GsDayCountISDA& Rhs ) const;
	int Compare( const GsDayCountISDA& Rhs ) const;

	const GsDayCounter* 		getNumerator() 		const { return m_Numerator; }
	const GsDaysPerUnitTerm* 	getDenominator() 	const { return m_Denominator; }
	const GsString 				getTermFormula() 	const { return m_TermFormula; }

	double daycountFraction( 
		GsDate Start, 
		GsDate End 
	) const;

	double adjustedDaycountFraction( 	
		GsDate Start, 
		GsDate End,
		const GsString& Holidays,					// New York|London, for example
		LT_BUSINESS_DAY_CONVENTION BusinessDayConv	// mf, f, or p, for example
	) const;

	bool isValid() const { return m_Numerator && m_Denominator; }

	static const char* typeName() { return "GsDayCountISDA"; }
	GsString toString() const { return m_ISDAString; }

	friend bool operator < ( const GsDayCountISDA& dc1, const GsDayCountISDA& dc2 ) { return &dc1 < &dc2; }
};	


EXPORT_CPP_GSDATE CC_OSTREAM& operator << ( CC_OSTREAM& s, const GsDayCountISDA& DC );

EXPORT_CPP_GSDATE GsString 
		GsDayCountISDANumeratorShortName( const GsDayCountISDA& Dyc),
		GsDayCountISDADenominatorShortName( const GsDayCountISDA& Dyc),
		GsDayCountISDATermFormula( const GsDayCountISDA& Dyc);

EXPORT_CPP_GSDATE GsDayCountISDA
		GsDayCountISDAFromName( const GsString& Str );

EXPORT_CPP_GSDATE double 
		GsDayCountISDADCF(
			GsDate StartDate,			
			GsDate EndDate, 			
			const GsString& DayCountISDA );

EXPORT_CPP_GSDATE double 
		GsDayCountISDAAdjustedDCF(
			GsDate StartDate,				
			GsDate EndDate, 				
			const GsString& DayCountISDA,  	
			const GsString& Holidays,							
			LT_BUSINESS_DAY_CONVENTION BusinessDayConv );

CC_END_NAMESPACE

#endif 
