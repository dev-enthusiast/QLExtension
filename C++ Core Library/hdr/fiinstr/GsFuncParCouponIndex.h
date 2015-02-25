/****************************************************************
**
**	GSFUNCPARCOUPONINDEX.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsFuncParCouponIndex.h,v 1.6 2001/11/27 21:58:26 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCPARCOUPONINDEX_H )
#define IN_GSFUNCPARCOUPONINDEX_H

#include <fiinstr/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDate.h>
#include <gscore/GsRDate.h>
#include <gscore/GsTermCalc.h>
#include <gscore/GsDateGen.h>
#include <gscore/GsPeriod.h>

#include <gscore/GsDiscountCurve.h>
#include <gscore/GsIndexCurve.h>
#include <gscore/GsDoubleVector.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIINSTR GsFuncParCouponIndex
	: public GsFunc<GsDate, double>
{
public:

	GsFuncParCouponIndex(
		const GsRDate& settle,
		const GsRDate& tenor,
		const GsPeriod& period,
		const GsDateGen& cal,
		const GsTermCalc& tc,
		const GsDiscountCurve& df
	);
	virtual ~GsFuncParCouponIndex();

	virtual double operator()( GsDate ) const;

private:

	//// index description ////
	GsRDate m_tenor;
	GsPeriod m_period;

	// common to indices of particular currency
	GsDateGen m_cal;
	GsTermCalc m_tc;
	GsRDate m_settle;

	//// modeling ////
	GsDiscountCurve m_df;

};


EXPORT_CPP_FIINSTR
GsIndexCurve* FqIndexRawParCoupon(
	const GsPeriod& tenor,	
	const GsPeriod& period, 
	const GsDateGen& tenorDG,
	const GsTermCalc& tc,	 
	const GsPeriod& daysToSettle,  
	const GsDateGen& daysToSettleDG,
	const GsDiscountCurve& df		
);


EXPORT_CPP_FIINSTR
GsIndexCurve* FqIndexCreateFromSamples(
	const GsDateVector& dates,	
	const GsDoubleVector& rates		
);

EXPORT_CPP_FIINSTR
GsIndexCurve* FqIndexAlgebra(
	const GsIndexCurve& index1,	
	const GsIndexCurve& index2,
	const GsString& operation
);


CC_END_NAMESPACE

#endif 
