/****************************************************************
**
**	GSPTIMECALC.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsPTimeCalc.h,v 1.2 2000/04/11 13:49:57 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSPTIMECALC_H )
#define IN_GSDATE_GSPTIMECALC_H

#include <gscore/base.h>
#include <gscore/GsString.h>
#include <gscore/GsRDate.h>
#include <gscore/GsDayCount.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsPTimeCalc 
** Description : GsPTimeCalc class
****************************************************************/

class EXPORT_CLASS_GSDATE GsPTimeCalc
{
	GsPTimeCalc();

public:

	GsPTimeCalc( const GsRDate& TermBoundaries, const GsDayCount& DayCount );
	GsPTimeCalc( const GsPTimeCalc & );

	~GsPTimeCalc();

	GsPTimeCalc &operator=( const GsPTimeCalc &rhs );

	double daycountFraction( const GsDate& StartDate, const GsDate& EndDate ) const;
	double daycountFraction( const GsDate& StartDate, const GsDate& EndDate,
							 const GsDate& RefDate, const GsPeriod& Frequency ) const;
	
	typedef GsDtGeneric< GsPTimeCalc > GsDtType;

	static const char* typeName() { return "GsPTimeCalc"; }
	GsString toString() const { return "GsPTimeCalc"; }

	static GsRDate Jan01Rolling();

private:
	
	GsRDate m_TermBoundaries;
	GsDayCount m_DayCount;
};

EXPORT_CPP_GSDATE GsPTimeCalc*
		GsPTimeCalcISDA( const GsDayCount& );

EXPORT_CPP_GSDATE double
		GsPTimeCalcDCF( const GsPTimeCalc& PTime, const GsDate& StartDate, const GsDate& EndDate );

CC_END_NAMESPACE

#endif 
