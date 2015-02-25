/****************************************************************
**
**	AverageRatePricerExp.h	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_AVERAGERATEPRICEREXP_H )
#define IN_AVERAGERATEPRICEREXP_H

#include <qgrid/AverageRatePricer.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID AverageRatePricer QGridAverageRatePricer(
	const BSPricer& pricer,			// the BSPricer for underlying model
	const GsVector& AveragingTimes, // the averaging times
	const GsVector& Weights,		// the averaging weights
	double InitialAverage,			// initial value for the average
	const GsVector& StepSizes		// step sizes for the layers rel to first layer
);

EXPORT_CPP_QGRID GsVector QGridAverageRatePricer_CreateLASPayout(
	const AverageRatePricer& pricer,	// the AverageRatePricer
	Q_OPT_TYPE OptionType,		// call, put or fwd
	double Time,				// time of the payout
	double Offset,				// strike offset
	double MinStrike,			// Minimum strike
	double MaxStrike			// Maximum strike
);

EXPORT_CPP_QGRID GsVector QGridAverageRatePricer_StepBack(
	const AverageRatePricer& pricer, // the AverageRatePricer
	const GsVector& Values,			 // grid values
	double s,						 // time to step back to
	double t,						 // time to step back from
	double LowBarrier,				 // low knockout (<=0 for no KO)
	double HighBarrier,              // high knockout (<=0 for no KO)
	double LowRebate,				 // rebate at low KO
	double HighRebate,				 // rebate at high KO
	double RebateTime				 // time when rebate is payed out
);

EXPORT_CPP_QGRID double QGridAverageRatePricer_Price(
	const AverageRatePricer& pricer, // the AverageRatePricer
	const GsVector& Values,			 // the grid values
	double LowerKO,					 // lower knockout level
	double UpperKO,					 // upper knockout level
	double LowerRebate,				 // rebate at lower ko
	double UpperRebate,				 // rebate at upper ko
	double RebateTime				 // time when rebates are payed
);

EXPORT_CPP_QGRID GsDt* QGridAverageRatePricer_ToGsDt(
	const AverageRatePricer& pricer, // the AverageRatePricer
	const GsVector& Values,          // the grid values
	double Time						 // time of grid
);

CC_END_NAMESPACE

#endif

