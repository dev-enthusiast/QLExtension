/****************************************************************
**
**	fiinstr/FiDateStrip.h	- 
**
**	Copyright 1988-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/FiDateStrip.h,v 1.28 2011/05/17 18:25:15 khodod Exp $
**
****************************************************************/

#if !defined( IN_FIINSTR_FIDATESTRIP_H )
#define IN_FIINSTR_FIDATESTRIP_H

#include <vector>
#include <fiinstr/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsDate.h>
#include <gscore/GsRDate.h>
#include <gscore/GsDateGen.h>
#include <gscore/GsTermCalc.h>
#include <gscore/GsIndexCurve.h>
#include <gscore/GsVector.h>
#include <gscore/GsDoubleVector.h>
#include <gscore/GsDiscountCurve.h>
#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIINSTR FiDateStrip
	: public GsFunc< GsIndexCurve, GsFuncHandle<GsDate, double> >
{
public:

	FiDateStrip() {}
	FiDateStrip(
		const GsDateGen&,
		const GsTermCalc&,
		const GsPeriod&,
		const GsRDate& observation,
		const GsDate& start,
		const GsDate& stop,
		bool adjustAccrualDates = true
	);

	FiDateStrip(
		const GsDateGen&,
		const GsTermCalc&,
		const GsPeriod&,
		const GsRDate& observation,
		const GsDate& start,
		const GsDate& stop,
		const GsDate& ref,
		bool adjustAccrualDates = true
	);

	FiDateStrip(
		const GsDateGen&,
		const GsTermCalc&,
		const GsPeriod&,
		const GsRDate& observation,
		const GsDate& start,
		const GsDate& stop,
		const GsDate& first,
		const GsDate& ref,
		bool longLastStub = false,
		bool adjustAccrualDates = true,
		bool resetInArrears = false,
		bool payInAdvance = false
	);
	FiDateStrip(
		const GsDateGen& dg,
		const GsTermCalc& tc,
		const GsPeriod& period,
		const GsRDate& observation,
		const GsDate& start,
		const GsDate& stop,
		const GsDate& ref,
		bool longFirstStub,
		bool longLastStub,
		bool adjustAccrualDates,
		bool resetInArrears,	// reset at end of accrual period
		bool payInAdvance		// pay at beginning of accrual period
	);
  

	FiDateStrip(
		const GsPeriod& period,
		const GsDateVector& accrualDates,
		const GsDateVector& payDates,
		const GsPTimeVector& accrualTerms,
		const GsDateVector& resetDates
	);

	FiDateStrip(const FiDateStrip&);
	FiDateStrip& operator=(const FiDateStrip&);
	virtual ~FiDateStrip();

	size_t size() const { return m_accrualTerms.size(); }

	// given realized path of uncertain observable, returns payment function
	GsFuncHandle<GsDate, double> operator()( GsIndexCurve index ) const;

	const GsPeriod& period() const { return m_period; }
	const GsDateVector& payDates() const { return m_payDates; }
	const GsDateVector& resetDates() const { return m_resetDates; }
	const GsDateVector& accrualDates() const { return m_accrualDates; }
	const GsPTimeVector& accrualTerms() const { return m_accrualTerms; }
	GsDate accrualStart() const; // call only if size() > 0
	GsDate lastPayDate() const; // call only if size() > 0
	void setFirstResetDate(const GsDate& date) // call only if constructed.
	{
		m_resetDates.front() = date;
	}

	// GsDtGeneric
	typedef GsDtGeneric<FiDateStrip> GsDtType;
	inline static const char* typeName() { return "FiDateStrip"; }
	GsString toString() const;

private:

	GsPeriod m_period;
	GsDateVector m_payDates;
	GsPTimeVector m_accrualTerms;
	GsDateVector m_resetDates;
	GsDateVector m_accrualDates;

};


EXPORT_CPP_FIINSTR GsDate
		FqDateStripAccrualStart( const FiDateStrip& ),
		FqDateStripLastPayDate( const FiDateStrip& );

EXPORT_CPP_FIINSTR
GsDate FqDateStripPayDates(
	const FiDateStrip&,
	size_t	index
	);

EXPORT_CPP_FIINSTR
GsDate FqDateStripResetDates(
	const FiDateStrip&,
	size_t	index
	);

EXPORT_CPP_FIINSTR
double FqDateStripAccrualTerms(
	const FiDateStrip&,
	size_t	index
	);

EXPORT_CPP_FIINSTR
GsVector FqDateStripFlows(
	const FiDateStrip&,
	const GsIndexCurve&
	);

EXPORT_CPP_FIINSTR
double FqDateStripFlowsPV(
	const FiDateStrip&,
	const GsIndexCurve&,
	const GsDiscountCurve&
	);

EXPORT_CPP_FIINSTR
double FqDateStripAmortFlowsPV(
	const FiDateStrip&,
	const GsDoubleVector&,
	const GsIndexCurve&,
	const GsDiscountCurve&,
	bool
	);

EXPORT_CPP_FIINSTR
double FqDateStripParRate(
	const FiDateStrip&,
	const GsDiscountCurve&
	);

EXPORT_CPP_FIINSTR
double FqDateStripDV01(
	const FiDateStrip&,
	const GsDiscountCurve&
	);

EXPORT_CPP_FIINSTR
double FqDateStripAmortDV01(
	const FiDateStrip&,
	const GsDoubleVector&,
	const GsDiscountCurve&
	);

EXPORT_CPP_FIINSTR
double FqDateStripDV01IRR(
	const FiDateStrip&,
	const GsDiscountCurve&
	);

EXPORT_CPP_FIINSTR
FiDateStrip* FqDateStrip(
	const GsDateGen&,
	const GsTermCalc&,
	const GsPeriod&,
	const GsPeriod& resetdelay,
	const GsDateGen& resetdelaydg,
	const GsDate& start,
	const GsDate& stop
	);

EXPORT_CPP_FIINSTR
FiDateStrip* FqDateStripRaw(
	const GsPeriod&,
	const GsDateVector&,
	const GsDateVector&,
	const GsVector&,
	const GsDateVector&
	);

EXPORT_CPP_FIINSTR
FiDateStrip* FqDateStripWithRefDate(
	const GsDateGen&,
	const GsTermCalc&,
	const GsPeriod&,
	const GsPeriod& resetdelay,
	const GsDateGen& resetdelaydg,
	const GsDate& start,
	const GsDate& stop,
	const GsDate& ref
	);

EXPORT_CPP_FIINSTR
FiDateStrip* FqDateStripStub(
	const GsDateGen&,
	const GsTermCalc&,
	const GsPeriod&,
	const GsPeriod& resetdelay,
	const GsDateGen& resetdelaydg,
	const GsDate& start,
	const GsDate& first,
	const GsDate& stop
	);

EXPORT_CPP_FIINSTR
FiDateStrip* FqDateStripDumbStub(
	const GsDateGen& dg,		
	const GsTermCalc& tc,		
	const GsPeriod& period,		
	const GsPeriod& resetdelay,	
	const GsDateGen& resetdelaydg,	
	const GsDate& start,		
	const GsDate& stop,			
	const GsDate& ref,			
	bool longFirstStub,			
	bool longLastStub,			
	bool adjustAccrualDates,	
	bool resetInArrears,		
	bool payInAdvance			
	);
EXPORT_CPP_FIINSTR
FiDateStrip* FqDateStripCustom(
	const GsDateGen& dg,		
	const GsTermCalc& tc,		
	const GsPeriod& period,		
	const GsPeriod& resetdelay,	
	const GsDateGen& resetdelaydg,	
	const GsDate& start,		
	const GsDate& stop,			
	const GsDate& first,		
	const GsDate& ref,			
	bool longLastStub,			
	bool adjustAccrualDates,	
	bool resetInArrears,		
	bool payInAdvance			
	);

EXPORT_CPP_FIINSTR
GsDateVector FqDateStripGetPayDates(
	const FiDateStrip&
	);

EXPORT_CPP_FIINSTR
GsDateVector FqDateStripGetResetDates(
	const FiDateStrip&
	);

EXPORT_CPP_FIINSTR
GsDateVector FqDateStripGetAccrualDates(
	const FiDateStrip&
	);

EXPORT_CPP_FIINSTR
GsVector FqDateStripGetAccrualTerms(
	const FiDateStrip&
	);

EXPORT_CPP_FIINSTR
int FqDateStripGetNumberOfPeriods(
	const FiDateStrip&
	);

EXPORT_CPP_FIINSTR
double FqDateStripCapATMStrike(
	const FiDateStrip&,
	const GsIndexCurve&,
	const GsDiscountCurve&
);

CC_END_NAMESPACE

#endif 

