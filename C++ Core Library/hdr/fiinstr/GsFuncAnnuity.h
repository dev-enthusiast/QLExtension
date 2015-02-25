/****************************************************************
**
**	GSFUNCANNUITY.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsFuncAnnuity.h,v 1.3 1998/11/23 22:20:14 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCANNUITY_H )
#define IN_GSFUNCANNUITY_H

#include <vector>
#include <fiinstr/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsDate.h>
#include <gscore/GsRDate.h>
#include <gscore/GsDateGen.h>
#include <gscore/GsTermCalc.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIINSTR GsFuncAnnuity 
	: public GsFunc< GsFuncHandle<GsDate, double>, GsFuncHandle<GsDate, double> >
{
public:

	GsFuncAnnuity(
		const GsDateGen&,
		const GsTermCalc&,
		const GsPeriod&,
		const GsRDate& observation,
		const GsDate& start,
		const GsDate& stop
	);
	GsFuncAnnuity(const GsFuncAnnuity&);
	virtual ~GsFuncAnnuity();

	// given realized path of uncertain observable, returns payment function
	GsFuncHandle<GsDate, double> operator()( GsFuncHandle<GsDate, double> index ) const;

private:

	GsDateVector m_payDates;
	GsPTimeVector m_accrualTerms;
	GsDateVector m_resetDates;

};

CC_END_NAMESPACE


#endif 
