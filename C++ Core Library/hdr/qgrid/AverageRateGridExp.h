/****************************************************************
**
**	AverageRateGridExp.h	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_AVERAGERATEGRIDEXP_H )
#define IN_AVERAGERATEGRIDEXP_H

#include <qgrid/AverageRateGrid.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID AverageRateGrid QGridAverageRateGrid(
	double 			MinX,	 // min of log(spot)
	double 			MaxX,	 // max of log(spot)
	double 			MinA,	 // min of average
	double 			MaxA,	 // max of average
	int	   			nGrid,	 // size of first grid
	const GsVector& StepSize // size of gridsteps rel to large grid
);

EXPORT_CPP_QGRID GsDt* QGridAverageRate_ToGsDt(
	const AverageRateGrid& grid		// grid to convert to a GsDt
);

EXPORT_CPP_QGRID GsVector QGridAverageRate_CreateLAS(
	const AverageRateGrid& grid, // the AverageRateGrid
	Q_OPT_TYPE OptionType,		 // Q_OPT_PUT,Q_OPT_CALL or Q_OPT_FWD
	double	   Offset,			 // Strike offset
	double	   MinStrike,		 // minimum value for the strike
	double	   MaxStrike         // maximum value for the strike
);

EXPORT_CPP_QGRID double QGridAverageRate_Evaluate(
	const GsVector& Values,		 // grid values
	const AverageRateGrid& grid, // the AverageRateGrid
	double Spot,				 // the spot at which to interpolate
	double Average               // te average at which to interpolate
);

EXPORT_CPP_QGRID GsVector QGridAddAveragingPoint(
	const GsVector& Values,		 // grid values
	const AverageRateGrid& grid, // the AverageRateGrid
	double weight				 // weight of the averaging point
);

EXPORT_CPP_QGRID GsVector QGridAverageRateStepBack(
	const GsVector&        Values, 		// grid values
	const AverageRateGrid& grid,   		// the AverageRateGrid
	const BSPricer& 	   pricer, 		// BSPricer used to step back
	double				   s,			// time to step back to
	double				   t,			// time to step back from
	double				   LowBarrier,	// lower knockout ( <= 0 for no KO )
	double				   HighBarrier, // upper knockout ( <= 0 for no KO )
	double				   LowRebate,	// rebate at lower KO
	double				   HighRebate,	// rebate at upper KO
	double				   RebateTime	// time when rebates are payed out
);

EXPORT_CPP_QGRID GsVector QGridAverageRate_GetLayer(
	const GsVector&        Values, // grid values
	const AverageRateGrid& grid,   // the AverageRateGrid
	double average				   // average at the layer
);

CC_END_NAMESPACE

#endif

