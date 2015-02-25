/****************************************************************
**
**	FQVOLCURVECONSTRAINT.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqVolCurveConstraint.h,v 1.7 2011/08/11 03:06:01 khodod Exp $
**
****************************************************************/

#if !defined( IN_FQVOLCURVECONSTRAINT_H )
#define IN_FQVOLCURVECONSTRAINT_H

#include <fiblack/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsIndexCurve.h>
#include <gscore/GsDiscountCurve.h>
#include <gscore/GsVolatilityCurve.h>
#include <gsdt/GsDtGeneric.h>

#include <fiinstr/FiDateStrip.h>

#include <fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqVolCurveConstraint
{
public:

	FqVolCurveConstraint();
	FqVolCurveConstraint(const FqVolCurveConstraint&);
	FqVolCurveConstraint(GsFuncHandle<GsFuncHandle<double, double>, double>, double);
	FqVolCurveConstraint& operator=(const FqVolCurveConstraint&);
	virtual ~FqVolCurveConstraint();

	// mathematical rep
	virtual GsFuncHandle<GsFuncHandle<double, double>, double> getMathRep() const
	{
		return m_rep;
	}
		
	virtual double getMaturity() const 
	{ 
		return m_mat; 
	}

	//// GsDt ////
	typedef GsDtGeneric< FqVolCurveConstraint > GsDtType;
	inline static const char* typeName() { return "FqVolCurveConstraint"; }
	GsString toString() const {	return "FqVolCurveConstraint"; }

private:

	GsFuncHandle<GsFuncHandle<double, double>, double> m_rep;
	double m_mat;

};

EXPORT_CPP_FIBLACK
FqVolCurveConstraint FqBsVolCurveConstraintCreateFromCap(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double strike,
	double price,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
FqVolCurveConstraint FqBsVolCurveConstraintCreateFromEuroOption(
	const GsDate& delivery,		   
	const GsPeriod& tenor,		   
	const GsDateGen& dg,		   
	const GsTermCalc& tc,		   
	const GsPeriod& delay,		   
	const GsDateGen& delaydg,	   
	Q_OPT_TYPE opttype,		   
	const GsTermCalc& voltermcalc, 
	double strike,				   
	double price,				   
	const FqBlack& dist,			
	const GsDiscountCurve& zerocurve
);

EXPORT_CPP_FIBLACK
FqVolCurveConstraint FqBsVolCurveConstraintCreateFromOptionOnFwd(
	Q_OPT_TYPE opttype,
	double strike,
	const GsDate& expDate,
	double fwd,
	double value,
	const FqBlack& dist,
	const GsTermCalc& voltc,
	const GsDate& ref
);

EXPORT_CPP_FIBLACK
FqVolCurveConstraint FqBsVolCurveConstraintPlusConst(
	const FqVolCurveConstraint &Constr,	
	double AdditiveFactor				
);

CC_END_NAMESPACE

#endif 
