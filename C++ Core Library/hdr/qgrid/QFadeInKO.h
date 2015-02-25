/****************************************************************
**
**	QFadeInKO.h	- Fade In with Window Knockout
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_QFADEINKO_H )
#define IN_QFADEINKO_H

#include <qgrid/base.h>
#include <qenums.h>
#include <qgrid/Pricer.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID GsDt* QFadeInKO(
    const Pricer&	pricer,					// the Pricer
    const GsVector&	FadeTimes,				// fadeout times, MUST lie in the range (0, Expiry Time]
    double 			Lower_Fadeout,          // Level below which accumulate 0
    double 			Upper_Fadeout,          // Level above which accumulate 0
    double 			Lower_KO,				// Level below which knocks out
	double 			Upper_KO,               // Level above which knocks out
    double 			Lower_KO_Start_Time,	// start of lower ko window
	double 			Lower_KO_End_Time,		// start of upper ko window
    double 			Upper_KO_Start_Time,	// end of lower ko window
	double 			Upper_KO_End_Time,      // end of upper ko window
    double 			Lower_Rebate,			// rebate at lower ko
	double 			Upper_Rebate,           // rebate at upper ko
    Q_REBATE_TYPE 	Rebate_Type,			// Q_IMMEDIATE or Q_DELAYED
    double 			Vanilla_Rebate_Factor,	// weighted Vanilla rebate
    bool 			Return_KO_Price			// true if you want both FOKO & KO prices returned
);

CC_END_NAMESPACE

#endif

