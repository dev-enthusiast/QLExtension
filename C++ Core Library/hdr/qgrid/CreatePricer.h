/****************************************************************
**
**	CreatePricer.h	- Creates Pricer from model dependent pricers
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.16 $
**
****************************************************************/

#if !defined( IN_CREATEPRICER_H )
#define IN_CREATEPRICER_H

#include <math.h>
#include <qgrid/base.h>
#include <qgrid/BSPricer.h>
#include <qgrid/DVPricer.h>
#include <qgrid/SVPricer.h>
#include <qgrid/BSPricerFourier.h>
#include <qgrid/AndersenPricer.h>
#include <qgrid/GsDtPricer.h>
#include <gscore/types.h>
#include <qenums.h>

CC_BEGIN_NAMESPACE( Gs )

// constructor for generic pricer
EXPORT_CPP_QGRID GsDt* GsDtPricerCtor(
	const GsDt&		SpecialPricer	// specialised pricer
);

// constructor for specialised pricers
EXPORT_CPP_QGRID BSPricer 		BSPricerCtor( const GsDt &  );
EXPORT_CPP_QGRID DVPricer 		DVPricerCtor( const GsDt &  );
EXPORT_CPP_QGRID AndersenPricer AndersenPricerCtor( const GsDt &  );

EXPORT_CPP_QGRID Pricer* QGridBSPricer_CreatePricer(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID Pricer* QGridDVPricer_CreatePricer(
	const DVPricer&	model	// the DVPricer
);

EXPORT_CPP_QGRID Pricer* QGridSVPricer_CreatePricer(
	const SVPricer&	model	// the SVPricer
);

EXPORT_CPP_QGRID Pricer* QGridBSPricerFourier_CreatePricer(
	const BSPricerFourier&	model	// the BSPricerFourier
);

EXPORT_CPP_QGRID Pricer QGridBSPricer_CreateLASPricer(
	const BSPricer& model,	        // the BSPricer
	Q_OPT_TYPE      OptionType,	    // call/put/fwd
	double          StrikeOffset,   // offset for strike
	double          MinStrike,	    // strike floor
	double          MaxStrike,	    // strike cap
	double          Expiration,	    // time to payout
	const GsVector& AveragingTimes, // times to do averaging
	const GsVector& Weights,		// averaging weights
	double          InitialAverage, // weighted initial value of average
	const GsVector& StepSizes,	    // step sizes of layers rel to first layer
	double			MaxSkewFactor   // restricts amount of skew of spot-average range, <= 1 to ignore
);

EXPORT_CPP_QGRID Pricer QGridBSPricer_CreateAverageRatePricer(
	const BSPricer& model,	        // the BSPricer
	Q_OPT_TYPE      OptionType,	    // call/put/fwd
	double          Strike,   		// strike
	double          Expiration,	    // time to payout
	const GsVector& AveragingTimes, // times to do averaging
	const GsVector& Weights,		// averaging weights
	double          InitialAverage, // weighted initial value of average
	const GsVector& StepSizes,	    // step sizes of layers rel to first layer
	double			MaxSkewFactor   // restricts amount of skew of spot-average range, <= 1 to ignore
);

EXPORT_CPP_QGRID Pricer QGridBSPricer_CreateInvAvgStrikePricer(
	const BSPricer& model,	        // the BSPricer
	Q_OPT_TYPE      OptionType,	    // call/put/fwd
	double          StrikeOffset,   // offset for strike (of inverted cross)
	double          Expiration,	    // time to payout
	const GsVector& AveragingTimes, // times to do averaging
	const GsVector& Weights,		// averaging weights
	double          InitialAverage, // weighted initial value of average
	const GsVector& StepSizes,	    // step sizes of layers rel to first layer
	double			MaxSkewFactor   // restricts amount of skew of spot-average range, <= 1 to ignore
);

CC_END_NAMESPACE

#endif

