/****************************************************************
**
**	q_multfo.h	- Multisegment Fade Out	Routines
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.15 $
**
****************************************************************/

#if !defined( IN_Q_MULTFO_H )
#define IN_Q_MULTFO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QFADE double Q_MultiHDFO(
	Q_OPT_TYPE		OptionType,				// Option type (Q_OPT_CALL, Q_OPT_PUT or Q_OPT_FWD).
	Q_IN_OUT     	InOrOut, 				// appearing or disappearing (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_IN_OUT		InsideOrOutside,		// fade out inside our outside the range (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_FADEOUT_TYPE	FadeOutType,			// Q_FADEOUT_RESURRECTING / Q_FADEOUT_NON_RESURRECTING
	double			Spot,					// spot
	double			Strike,					// strike
	int				MaxFade,				// number of fadeouts we must hit to completely fade out
	Q_VECTOR		*EndSegments,			// end of each time segment
	Q_VECTOR		*LowFadeOutVals,		// low fadeout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*HighFadeOutVals,		// high fadeout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*FadeOutTimes,			// fadeout times
	Q_VECTOR		*LowerBarrierLevels,	// low knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*UpperBarrierLevels,	// high knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*LowerRebates,			// upper rebates for knocking out
	Q_VECTOR		*UpperRebates,			// upper rebates for knocking out
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
	int				VanillaRebate,			// TRUE to get a vanilla option (* notional) at knockout, FALSE otherwise
    Q_VECTOR 		*Volatilities,			// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,		// times for volatilities
    Q_VECTOR 		*Discounts,				// Domestic discount factors
    Q_VECTOR 		*DiscountTimes,			// times for discount factors
    Q_VECTOR 		*Forwards, 				// forwards
    Q_VECTOR 		*ForwardTimes,			// times for forwards
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	double			RangeVol				// volatility used to set up the grid
	);

EXPORT_C_QFADE double Q_MultisegmentFOKO(
	Q_OPT_TYPE		OptionType,				// Option type (Q_OPT_CALL, Q_OPT_PUT or Q_OPT_FWD).
	Q_IN_OUT     	InOrOut, 				// appearing or disappearing (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_IN_OUT		InsideOrOutside,		// fade out inside our outside the range (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_FADEOUT_TYPE	FadeOutType,			// Q_FADEOUT_RESURRECTING / Q_FADEOUT_NON_RESURRECTING
	double			Spot,					// spot
	double			Strike,					// strike
	Q_VECTOR		*EndSegments,			// end of each time segment
	Q_VECTOR		*LowFadeOutVals,		// low fadeout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*HighFadeOutVals,		// high fadeout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*FadeOutTimes,			// fadeout times, MUST lie in the range (0, Term] where Term=end of last segment
	Q_VECTOR		*LowerBarrierLevels,	// low knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*UpperBarrierLevels,	// high knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*LowerRebates,			// upper rebates for knocking out
	Q_VECTOR		*UpperRebates,			// upper rebates for knocking out
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
	int				VanillaRebate,			// TRUE to get a vanilla option (* notional) at knockout, FALSE otherwise
    Q_VECTOR 		*Volatilities,			// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,		// times for volatilities
    Q_VECTOR 		*Discounts,				// Domestic discount factors
    Q_VECTOR 		*DiscountTimes,			// times for discount factors
    Q_VECTOR 		*Forwards, 				// forwards
    Q_VECTOR 		*ForwardTimes,			// times for forwards
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	double			RangeVol				// volatility used to set up the grid
);

EXPORT_C_QFADE int Q_MultisegmentFOKOVec(
	Q_OPT_TYPE		OptionType,				// Option type (Q_OPT_CALL, Q_OPT_PUT or Q_OPT_FWD).
	Q_IN_OUT     	InOrOut, 				// appearing or disappearing (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_IN_OUT		InsideOrOutside,		// fade out inside our outside the range (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_FADEOUT_TYPE	FadeOutType,			// Q_FADEOUT_RESURRECTING / Q_FADEOUT_NON_RESURRECTING
	double          minSpot,        		// smallest Spot for which we need accurate value
	double          maxSpot,        		// largest Spot for which we need accurate value
	double			Strike,					// strike
	double			RangeVol,				// volatility used to set up the grid
	DATE			prcDate,
	Q_DATE_VECTOR	*EndSegmentDates,		// end of each time segment
	Q_VECTOR		*LowFadeOutVals,		// low fadeout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*HighFadeOutVals,		// high fadeout values ( if <= 0 then it doesn't exist)
	Q_DATE_VECTOR	*FadeOutDates,			// fadeout times, MUST lie in the range (0, Term] where Term=end of last segment
	Q_VECTOR		*LowerBarrierLevels,	// low knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*UpperBarrierLevels,	// high knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*LowerRebates,			// upper rebates for knocking out
	Q_VECTOR		*UpperRebates,			// upper rebates for knocking out
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
	int				VanillaRebate,			// TRUE to get a vanilla option (* notional) at knockout, FALSE otherwise
	DT_CURVE 		*Vols,					// Underlyer volatility	curve
	DT_CURVE		*FwdFactors,			// Normalized Forward Curve
	DT_CURVE		*DiscCrv,				// Discount Curve
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	Q_VECTOR		*SpotVec,				// spot grid, input and output
	Q_VECTOR		*PriceVec				// prices grid, input and output
);

/*
EXPORT_C_QFADE int Q_DiscretizeFadeOutRange(
		double		MinGrid,				// lowest grid point
		double		MaxGrid,				// highest grid point
		int			nGrid,					// number of grid points
		double		LowFade,				// the lower fade out level ( =-1 if there isn't one )
		double		HighFade,				// the upper fade out level ( =nGrid if there isn't one )
		double		LowKnockOut,			// Low Knockout Barrier (HUGE if there isn't one)
		double		HighKnockOut,			// High Knockout Barrier (HUGE if there isn't one)
		int			*LowFadeIndex,			// the grid point number of the low fade level (output only)
		int			*HighFadeIndex,			// the grid point number of the high fade level (output only)
		double		*LowEdgeFactor,			// the probability of being in the range at LowFadeIndex (output only)
		double		*HighEdgeFactor			// the probability of being in the range at HighFadeIndex (output only)
);

EXPORT_C_QFADE void Q_CalculateFadeOut(
		int			nGrid,					// number of grid points
		double		*OptionPrices,			// pointer to an array of pointers to the option price grid layers, of size nLayers (input + output)
		double		*KnockOutPrices,		// pointer to the knockout price grid (input + output)
		int			Resurrecting,			// True = Resurrecting, False = Non-Resurrecting
		double		FractionLost,			// fraction which fades out
		Q_IN_OUT	InsideOrOutside,		// fade inside or outside the range
		int			LowFadeOutIndex,		// output of Q_DiscretizeFadeOutRange
		int			HighFadeOutIndex,		// as above
		double		LowEdgeFactor,			// as above
		double		HighEdgeFactor			// as above
);
*/
  
EXPORT_C_QFADE int Q_MultisegmentFOKOMatrix(
	Q_OPT_TYPE		OptionType,				// Option type (Q_OPT_CALL, Q_OPT_PUT or Q_OPT_FWD).
	Q_IN_OUT     	InOrOut, 				// appearing or disappearing (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_IN_OUT		InsideOrOutside,		// fade out inside our outside the range (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_FADEOUT_TYPE	FadeOutType,			// Q_FADEOUT_RESURRECTING / Q_FADEOUT_NON_RESURRECTING
	double          minSpot,        		// smallest Spot for which we need accurate value
	double          maxSpot,        		// largest Spot for which we need accurate value
	double			Strike,					// strike
	double			RangeVol,				// volatility used to set up the grid
	Q_DATE_VECTOR	*EndSegmentDates,		// end of each time segment
	Q_VECTOR		*LowFadeOutVals,		// low fadeout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*HighFadeOutVals,		// high fadeout values ( if <= 0 then it doesn't exist)
	Q_DATE_VECTOR	*FadeOutDates,			// fadeout times, MUST lie in the range (0, Term] where Term=end of last segment
	Q_VECTOR		*LowerBarrierLevels,	// low knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*UpperBarrierLevels,	// high knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*LowerRebates,			// upper rebates for knocking out
	Q_VECTOR		*UpperRebates,			// upper rebates for knocking out
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
	int				VanillaRebate,			// TRUE to get a vanilla option (* notional) at knockout, FALSE otherwise
	DT_CURVE 		*Vols,					// Underlyer volatility	curve
	DT_CURVE		*DiscCrv,				// Discount Curve
	DT_CURVE		*ForeignDisc, 			// Foreign currency discount curve
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	Q_DATE_VECTOR	*OutDates,				// dates when we want the output
	Q_VECTOR		*SpotVec,				// spot grid, input and output
	Q_VECTOR		**PriceMat				// prices grid, input and output
);


#endif


