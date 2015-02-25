/****************************************************************
**
**	FadeOutDiscKO.h	- Fade Out Discrete Knockout
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_FADEOUTDISCKO_H )
#define IN_FADEOUTDISCKO_H

#include <qgrid/base.h>
#include <gscore/types.h>
#include <qgrid/Pricer.h>
#include <gsdt/GsDt.h>
#include <qenums.h>
#include <qgrid/GsDtPricer.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID void CreateInnerMask(
	double LowKO,
	double HighKO,
	double Amount,
	GsFuncPiecewiseLinear& f
);

EXPORT_CPP_QGRID void CreateOuterMask(
	double LowKO,
	double HighKO,
	double LowAmount,
	double HighAmount,
	GsFuncPiecewiseLinear& f
);

EXPORT_CPP_QGRID GsDt* QFadeOutDiscKO(
	const Pricer& 	model,				// the Pricer object
	double 		  	ExpiryTime,			// time to expiration
	Q_OPT_TYPE	  	OptionType,			// option type
	double		  	Strike,				// the strike
	const GsVector&	FadeTimes,			// times at which fadeouts occur
	const GsVector& LowerFadeouts,		// levels below which we don't accumulate
	const GsVector& UpperFadeouts,		// levels above which we don't accumulate
	const GsVector& KnockoutTimes,		// must lie in the range (0,Expiry Time]
	const GsVector& LowerKnockouts,		// levels below which the option knocks out
	const GsVector& UpperKnockouts,		// levels above which the option knocks out
	double			LowerVanillaRebate,	// If non-zero, gives weighted vanilla rebate
	double			UpperVanillaRebate,	// If non-zero, gives weighted vanilla rebate
	const GsVector& LowerRebates,		// lower rebates for knocking out
	const GsVector& UpperRebates,		// upper rebates for knocking out
	Q_REBATE_TYPE	RebateType,			// Q_DELAYED or Q_IMMEDIATE
	bool			DoKO				// return KO price as well?
);

CC_END_NAMESPACE

#endif

