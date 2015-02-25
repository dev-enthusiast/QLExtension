/****************************************************************
**
**	GSFUNCSIMPLEINDEX.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsFuncSimpleIndex.h,v 1.8 2001/11/27 21:58:26 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCSIMPLEINDEX_H )
#define IN_GSFUNCSIMPLEINDEX_H

#include <vector>
#include <fiinstr/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDate.h>
#include <gscore/GsRDate.h>
#include <gscore/GsDateGen.h>
#include <gscore/GsTermCalc.h>
#include <gscore/GsPeriod.h>

#include <gscore/GsIndexCurve.h>
#include <gscore/GsDiscountCurve.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIINSTR GsFuncSimpleIndex : public GsFunc<double, double>
{
public:

	GsFuncSimpleIndex(
		const GsRDate& settle,
		const GsRDate& tenor,
		const GsTermCalc& cal,
		const GsDate& ref,
		GsFuncHandle<double, double> df
	);

	GsFuncSimpleIndex(
		const GsRDate& settle,
		const GsRDate& tenor,
		double spread,
		const GsTermCalc& cal,
		const GsDate& ref,
		GsFuncHandle<double, double> df
	);

	virtual ~GsFuncSimpleIndex();

	double operator()( double ) const;

	GsFuncHandle<double, double> getFunc() const { return m_df; }
	GsDate getRef() { return m_ref; }

private:

	//// index description ////
	GsRDate m_tenor;
	GsDate m_ref;
	double m_spread;

	// common to many indices
	GsTermCalc m_cal;
	GsRDate m_settle;

	//// modeling ////
	GsFuncHandle<double, double> m_df;

};

EXPORT_CPP_FIINSTR
GsIndexCurve* FqIndexSimple(
	const GsPeriod& tenor,
	const GsDateGen& tenorDG,
	const GsTermCalc& tc,
	const GsPeriod& daysToSettle,
	const GsDateGen& daysToSettleDG,
	const GsDiscountCurve& df
	);

EXPORT_CPP_FIINSTR
GsIndexCurve* FqIndexSpread(
	const GsPeriod& tenor,
	double spread,
	const GsDateGen& tenorDG,
	const GsTermCalc& tc,
	const GsPeriod& daysToSettle,
	const GsDateGen& daysToSettleDG,
	const GsDiscountCurve& df
	);

EXPORT_CPP_FIINSTR
double GsIndexApply(
	const GsIndexCurve&,
	const GsDate&
	);

CC_END_NAMESPACE

#endif 
