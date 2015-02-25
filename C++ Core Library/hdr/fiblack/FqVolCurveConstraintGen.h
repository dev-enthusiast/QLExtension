/****************************************************************
**
**	FQVOLCURVECONSTRAINTGEN.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqVolCurveConstraintGen.h,v 1.4 1999/11/20 23:04:05 gribbg Exp $
**
****************************************************************/

#if !defined( IN_FQVOLCURVECONSTRAINTGEN_H )
#define IN_FQVOLCURVECONSTRAINTGEN_H

#include <fiblack/base.h>

#include <gscore/GsIndexCurve.h>
#include <gscore/GsDiscountCurve.h>
#include <gscore/GsVolatilityCurve.h>
#include <gsdt/GsDtGeneric.h>

#include <fiinstr/FiDateStrip.h>

#include <fiblack/FqBlack.h>
#include <fiblack/FqVolCurveConstraint.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqVolCurveConstraintGen
{
public:

	FqVolCurveConstraintGen() {};
	virtual ~FqVolCurveConstraintGen();
	virtual FqVolCurveConstraintGen* copy() const = 0;
	
	double getMaturity() const { return m_mat; }
	GsDate getRefDate() const { return m_refDate; }
	virtual FqVolCurveConstraint generate( const FqBlack& dist, const GsTermCalc& voltc ) const = 0;
	

	//// GsDt ////
	typedef GsDtGeneric< FqVolCurveConstraintGen > GsDtType;
	inline static const char* typeName() { return "FqVolCurveConstraintGen"; }
	virtual	GsString toString() const = 0;

protected:
	double m_mat;
	GsDate m_refDate;

};

EXPORT_CPP_FIBLACK
FqVolCurveConstraintGen* FqBsVolCurveConstraintCapGen(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	double strike,
	double price,
	const GsDiscountCurve& zerocurve
);

EXPORT_CPP_FIBLACK
FqVolCurveConstraintGen* FqBsVolCurveConstraintEuroOptionGen(
	const GsDate& delivery,		   
	const GsPeriod& tenor,		   
	const GsDateGen& dg,		   
	const GsTermCalc& tc,		   
	const GsPeriod& delay,		   
	const GsDateGen& delaydg,	   
	Q_OPT_TYPE opttype,		   
	double strike,				   
	double price,				   
	const GsDiscountCurve& zerocurve
);

EXPORT_CPP_FIBLACK
FqVolCurveConstraintGen* FqBsVolCurveConstraintOptionOnFwdGen(
	Q_OPT_TYPE opttype,
	double strike,
	const GsDate& expDate,
    double fwd,
	double value,
	const GsDate& refDate
	);

EXPORT_CPP_FIBLACK
FqVolCurveConstraint FqBsVolCurveConstraintCreateFromGenerator(
	const FqVolCurveConstraintGen&,
	const FqBlack&,
	const GsTermCalc&
);

CC_END_NAMESPACE

#endif 
