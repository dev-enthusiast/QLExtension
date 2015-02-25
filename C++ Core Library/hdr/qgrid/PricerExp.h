/****************************************************************
**
**	FXGridExp.h	- FXGrid functions which are exported to Slang
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_PRICEREXP_H )
#define IN_PRICEREXP_H

#include <qgrid/base.h>
#include <gscore/types.h>
#include <qgrid/enums.h>
#include <qgrid/GsDtPricer.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID Pricer QGrid_SetPayout(
	const Pricer&				grid,		// the grid pricer
	const GsDt&					f,			// the payout function
	double						t			// time at which the payout occurs
);

EXPORT_CPP_QGRID Pricer QGrid_SetPayoutDbl(
	const Pricer&	grid,		// the grid pricer
	double		 	a,			// the payout
	double		 	t			// time at which the payout occurs
);

EXPORT_CPP_QGRID Pricer QGrid_StepBack(
	const Pricer&	grid,	// the grid pricer
	double			t		// time at which we step back to
);

EXPORT_CPP_QGRID double QGrid_Price(
	const Pricer&	grid	// the grid pricer
);

EXPORT_CPP_QGRID Pricer QGrid_SetLowerKOWindow(
	const Pricer&	model,		// the Pricer
	double			KOLevel,	// the knockout level
	double			StartTime,	// start of window
	double			EndTime,	// end of window
	double			Rebate,		// the rebate
	double			RebateTime	// time when rebate is payed
);

EXPORT_CPP_QGRID Pricer QGrid_SetUpperKOWindow(
	const Pricer&	model,		// the Pricer
	double			KOLevel,	// the knockout level
	double			StartTime,	// start of window
	double			EndTime,	// end of window
	double			Rebate,		// the rebate
	double			RebateTime	// time when rebate is payed
);

EXPORT_CPP_QGRID Pricer QGrid_SetLowerKO(
	const Pricer&	model,		// the Pricer
	double			LowerKO,	// the lower knockout
	double			LowerRebate	// the lower rebate
);

EXPORT_CPP_QGRID Pricer QGrid_SetUpperKO(
	const Pricer&	model,		// the Pricer
	double			UpperKO,	// the lower knockout
	double			UpperRebate	// the lower rebate
);

EXPORT_CPP_QGRID Pricer QGrid_SetRebateTime(
	const Pricer&	model,		// the Pricer
	double			RebateTime	// the rebate time
);

EXPORT_CPP_QGRID Pricer QGrid_RemoveKnockouts(
	const Pricer&	model		// the Pricer
);

EXPORT_CPP_QGRID GsString QGrid_ModelDescription(
	const Pricer&	grid	// the grid pricer
);

EXPORT_CPP_QGRID double QGrid_Time(
	const Pricer&	grid	// the grid pricer
);

EXPORT_CPP_QGRID Pricer QGrid_Add(
	const Pricer&	grid1,	// the first grid pricer
	const Pricer&	grid2	// the second grid pricer
);

EXPORT_CPP_QGRID Pricer QGrid_AddDbl(
	const Pricer&	grid,	// the first grid pricer
	double			a		// the double to add
);

EXPORT_CPP_QGRID Pricer QGrid_AddPwl(
	const Pricer&			 grid,	// the first grid pricer
	const GsFuncPiecewiseLinear& f		// the function to add
);

EXPORT_CPP_QGRID Pricer QGrid_Subtract(
	const Pricer&	grid1,	// the first grid pricer
	const Pricer&	grid2	// the grid pricer to subtract
);

EXPORT_CPP_QGRID Pricer QGrid_SubtractDbl(
	const Pricer&	grid,	// the first grid pricer
	double			a		// the double to subtract
);

EXPORT_CPP_QGRID Pricer QGrid_SubtractPwl(
	const Pricer&			 grid,	// the first grid pricer
	const GsFuncPiecewiseLinear& f		// the function to subtract
);

EXPORT_CPP_QGRID Pricer QGrid_MultiplyDbl(
	const Pricer&	grid,	// the grid pricer
	double			a		// the double to multiply by
);

EXPORT_CPP_QGRID Pricer QGrid_MultiplyPwlf(
	const Pricer&				grid,	// the grid pricer
	const GsFuncPiecewiseLinear&	f		// the function to multiply by
);

EXPORT_CPP_QGRID Pricer QGrid_Max(
	const Pricer&	grid1,	// the first grid pricer
	const Pricer&	grid2	// the second grid pricer
);

EXPORT_CPP_QGRID Pricer QGrid_MaxDbl(
	const Pricer&	grid1,	// the first grid pricer
	double			a		// double to max with
);

EXPORT_CPP_QGRID Pricer QGrid_MaxPwl(
	const Pricer&			 grid1,	// the first grid pricer
	const GsFuncPiecewiseLinear& f		// function to max with
);

EXPORT_CPP_QGRID Pricer QGrid_Min(
	const Pricer&	grid1,	// the first grid pricer
	const Pricer&	grid2	// the second grid pricer
);

EXPORT_CPP_QGRID Pricer QGrid_MinDbl(
	const Pricer&	grid1,	// the first grid pricer
	double			a		// double to min with
);

EXPORT_CPP_QGRID Pricer QGrid_MinPwl(
	const Pricer&			 grid1,	// the first grid pricer
	const GsFuncPiecewiseLinear& f		// function to min with
);

EXPORT_CPP_QGRID GsDt* QGrid_ToGsDt(
	const Pricer&			 grid	// the grid pricer
);

EXPORT_CPP_QGRID GsDt* QGrid_CreatePricer(
	const GsDt&		model	 	// the non-generic grid pricer
);

CC_END_NAMESPACE

#endif

