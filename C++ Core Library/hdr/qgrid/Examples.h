/****************************************************************
**
**	qgrid/Examples.h	- example grid pricing routines
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/qgrid/src/qgrid/Examples.h,v 1.4 2000/10/26 18:21:58 lowthg Exp $
**
****************************************************************/

#if !defined( IN_QGRID_EXAMPLES_H )
#define IN_QGRID_EXAMPLES_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<qgrid/GsDtPricer.h>


CC_BEGIN_NAMESPACE( Gs )
                 
EXPORT_CPP_QGRID double QGridMultisegmentDKO(
	const	Pricer&			Model,				// the grid pricing model
	Q_OPT_TYPE				OptionType,			// the option type
	double					Strike,				// the strike
	double					Term,				// time to expiry
	const	GsVector&		EndSegments,		// end time of each segment
	const	GsVector&		LowerKnockouts,		// the lower knockout levels
	const	GsVector&		UpperKnockouts,		// the upper knockout levels
	const	GsVector&		LowerRebates,		// the rebates at the lower knockouts
	const	GsVector&		UpperRebates		// the rebates at the upper knockouts
);

EXPORT_CPP_QGRID double QGridVarQuantityWindowDKO(
	const	Pricer&			Model,			// the grid pricing model
	Q_OPT_TYPE				OptionType,		// the option type
	double					Strike,			// the strike
	double					Term,			// the time to expiry
	double					VarQtyTime,		// time to quantity determination date
	double					LowPrice,		// low price for quantity determination fix
	double					HighPrice,		// high price for quantity determination fix
	double					LowAmount,		// amount of notional at LowPrice
	double					HighAmount		// amount of notional at HighPrice
);

EXPORT_CPP_QGRID double QGridFadeout(
	const Pricer&			Model,			// the Pricer
	Q_OPT_TYPE				OptionType,		// the option type
	double					Strike,			// the strike
	double					Term,			// the time to expiry
	double					LowerFadeout,	// lower fadeout level
	double					UpperFadeout,	// upper fadeout level 
	const GsVector&			FadeTimes		// fadeout times
);

CC_END_NAMESPACE
#endif

