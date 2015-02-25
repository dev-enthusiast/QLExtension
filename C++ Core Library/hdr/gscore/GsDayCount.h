/****************************************************************
**
**	GsDayCount.H	- GsDayCount class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsDayCount.h,v 1.9 2001/11/27 22:43:09 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSDATCOUNT_H )
#define IN_GSDATE_GSDATCOUNT_H

#include <gscore/base.h>
#include <gscore/types.h>
#include <gscore/GsDayCounter.h>
#include <gscore/GsDaysPerUnitTerm.h>
#include <gscore/GsDate.h>
#include <gscore/GsDateEnums.h>
#include <gscore/GsPeriod.h>
#include <gscore/GsStringVector.h>

#include <hash.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsDayCount 
{	
   	const GsDayCounter
			*m_Numerator;
	
	const GsDaysPerUnitTerm
			*m_Denominator;

	GsString
			m_String;
	
    GsDayCount& setFromString( const GsString& );

	GsDayCount( const GsString& String, const GsDayCounter* Numerator, const GsDaysPerUnitTerm* Denominator );

	static HASH* getDayCountFactory();
	static HASH* createDayCountFactory();

public:
	
	typedef GsDtDayCount GsDtType;

	GsDayCount();
	GsDayCount( const GsString& DayCountString );
	GsDayCount( const GsDayCount& Rhs);
	
	~GsDayCount();

	GsDayCount& operator=( const GsDayCount& Rhs);
	GsDayCount& operator=( const GsString& DayCountString );

	bool operator==( const GsDayCount& Rhs ) const;
	int Compare( const GsDayCount& Rhs ) const;

	const GsDayCounter* 		getNumerator() 		const { return m_Numerator; }
	const GsDaysPerUnitTerm* 	getDenominator() 	const { return m_Denominator; }

	double daycountFraction( 
		const GsDate& Start, 
		const GsDate& End,
  		const GsDate& RefDate,						
  		const GsPeriod& Frequency
	) const;

	double adjustedDaycountFraction( 
		const GsDate& Start, 
		const GsDate& End,
		const GsString& Holidays,					
		LT_BUSINESS_DAY_CONVENTION BusinessDayConv,
		const GsDate& RefDate,
		const GsPeriod& Frequency
	) const;

	bool isValid() const { return m_Numerator && m_Denominator; }

	static GsStringVector availableNames();
	
	static const char* typeName() { return "GsDayCount"; }
	GsString toString() const { return m_String; }

	friend bool operator < ( const GsDayCount& dc1, const GsDayCount& dc2 ) { return &dc1 < &dc2; }
};	


EXPORT_CPP_GSDATE CC_OSTREAM& operator << ( CC_OSTREAM& s, const GsDayCount& DC );

EXPORT_CPP_GSDATE GsString 
		GsDayCountNumeratorShortName( const GsDayCount& Dyc),
		GsDayCountDenominatorShortName( const GsDayCount& Dyc);

EXPORT_CPP_GSDATE GsDayCount
		GsDayCountFromName( const GsString& Str );

EXPORT_CPP_GSDATE double 
		GsDayCountDCF(
			const GsDate& StartDate,							
			const GsDate& EndDate, 							
			const GsString& DayCount, 					
			const GsDate& ReferenceDate,						
			const GsPeriod& Frequency );

EXPORT_CPP_GSDATE double 
		GsDayCountAdjustedDCF(
			const GsDate& StartDate,
			const GsDate& EndDate, 	
			const GsString& DayCount,
			const GsString& Holidays,	
			LT_BUSINESS_DAY_CONVENTION BusinessDayConv,
			const GsDate& ReferenceDate,				
			const GsPeriod& Frequency );	

EXPORT_CPP_GSDATE GsStringVector
		GsDayCountAvailableNames();
 

CC_END_NAMESPACE

#endif 




