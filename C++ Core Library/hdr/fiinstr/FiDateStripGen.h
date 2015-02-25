/****************************************************************
**
**	FiDateStripGen.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/FiDateStripGen.h,v 1.3 1999/02/13 20:04:31 gribbg Exp $
**
****************************************************************/

#if !defined( IN_FiDateStripGen_H )
#define IN_FiDateStripGen_H

#include <fiinstr/base.h>

#include <gscore/GsFunc.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsDate.h>
#include <gscore/GsRDate.h>
#include <gscore/GsDateGen.h>
#include <gscore/GsTermCalc.h>
#include <gscore/GsIndexCurve.h>

#include <fiinstr/FiDateStrip.h>

CC_BEGIN_NAMESPACE( Gs )

typedef GsFuncHandle< GsIndexCurve, GsFuncHandle<GsDate, double> > 
		FiIndexToFlow;

class EXPORT_CLASS_FIINSTR FiDateStripGen
	: public GsFunc2< GsPeriod, GsPeriod, FiIndexToFlow >
{
public:

	FiDateStripGen(
		const GsDateGen&,
		const GsTermCalc&,
		const GsPeriod&,
		const GsRDate& observation,
		const GsDateGen& rdatedg,
		const GsDate& ref
	);
	FiDateStripGen( const FiDateStripGen& );
	virtual ~FiDateStripGen();

	FiIndexToFlow operator()( GsPeriod start, GsPeriod stop ) const;
	FiDateStrip* gen( GsPeriod start, GsPeriod stop ) const;

	// GsDtGeneric
	typedef GsDtGeneric<FiDateStripGen> GsDtType;
	inline static const char* typeName() { return "FiDateStripGen"; }
	GsString toString() const;

private:

	GsDateGen m_dg;
	GsTermCalc m_tc;
	GsPeriod m_period;
	GsRDate m_observation;
	GsDateGen m_rdatedg;
	GsDate m_ref;

};

EXPORT_CPP_FIINSTR
FiDateStripGen* FqDateStripGenA(
	const GsDateGen&,
	const GsTermCalc&,
	const GsPeriod&,
	const GsPeriod&,
	const GsDateGen&,
	const GsDateGen& rdatedg,
	const GsDate& ref
	);

EXPORT_CPP_FIINSTR
FiDateStrip* FqDateStripA(
	const FiDateStripGen&,
	const GsPeriod&,
	const GsPeriod&
	);


CC_END_NAMESPACE

#endif 
