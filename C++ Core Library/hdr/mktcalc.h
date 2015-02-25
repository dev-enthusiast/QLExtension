/* $Header: /home/cvs/src/mkt/src/mktcalc.h,v 1.65 2004/02/26 20:59:03 khodod Exp $ */
/****************************************************************
**
**	MKTCALC.H	
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.65 $
**
****************************************************************/

#if !defined( IN_MKTCALC_H )
#define IN_MKTCALC_H

#if !defined( IN_DATE_H )
#	include	<date.h>
#endif

#if !defined( IN_DATATYPE_H )
#	include	<datatype.h>
#endif

#if !defined( IN_QTYPES_H )
#	include	<qtypes.h>
#endif

#include <avgsamp.h>

// This is a sentinel value for arg to Volatility Vt( cross )
#define VOLWINGS_USE_PREF_WING_INTERP	-1

#define SMALL_DAY_WEIGHT 	0.01

class GsNodeValues;

enum SQ_INSTR_TYPE
{
	SQ_INSTR_FORWARD_EURO = 1,
	SQ_INSTR_FORWARD_AMER,
	SQ_INSTR_ZERO,
	SQ_INSTR_COUPON,
	SQ_INSTR_DISCOUNT,
	SQ_INSTR_DISCOUNT_FACTOR,
	SQ_INSTR_EDFUTURE,
	SQ_INSTR_LAST
};

enum Q_KNOT_CHOICE
{
	Q_KNOTS_MULTIPLIER = 1,
	Q_KNOTS_MULTIPLICAND,
	Q_KNOTS_DIVISOR,
	Q_KNOTS_DIVIDEND
};

enum DELTA_TYPE
{
	DELTA_TYPE_SPOT = 0,
	DELTA_TYPE_FORWARD
};

enum VOLWING_TYPE
{
	VOLWING_TYPE_NO_WINGS = 0,
	VOLWING_TYPE_STRIKE_WINGS,
	VOLWING_TYPE_QUICK_DELTA_WINGS
};

enum VOL_INTERP
{
	VOL_INTERP_NO_WINGS = 0,
	VOL_INTERP_FIP
};

enum ATM_STRIKE
{
	ATM_STRIKE_FORWARD = 0,
	ATM_STRIKE_DELTA_NEUTRAL
};

enum ATMVOL_INTERP
{
	ATMVOL_INTERP_FLATFWDVAR = 0,
	ATMVOL_INTERP_FLATFWDSQRTVAR,
	ATMVOL_INTERP_LINEAR,
	ATMVOL_INTERP_HALFLIFE
};

DLLEXPORT DT_ARRAY
		*SQ_ExtendWings(				DT_ARRAY *BaseWings,	DT_CURVE *AssocCurve,	DT_CURVE *NewDatesCurve );
		
DLLEXPORT DT_CURVE
		*MktRefCurveProduct( 			DT_CURVE *Multiplier,	DT_CURVE *Multiplicand,	Q_KNOT_CHOICE Choice ),
		*MktRefCurveQuotient(			DT_CURVE *Numerator,	DT_CURVE *Denominator,	Q_KNOT_CHOICE Choice ),
		*SQ_CreateVolDateCurve(			DATE BaseDate,			double WkendDayWt,		DATE Cutoff		),
		*SQ_RenormalizeCurve( 			DT_CURVE *Curve,		DATE BaseDate,			DATE ValueDate, double Value ),
		*SQ_ShiftVolCurve(				DT_CURVE *VolCurve,		DATE BaseDate, 			DATE NewDate	),
		*SQ_RefToIntRateCurve(			DT_CURVE *RefCurve, 	DATE BaseDate ),
		*SQ_IntRateToRefCurve(			DT_CURVE *RateCurve,	DATE BaseDate ),
		*SQ_IntRatePtToRefCurve(		DT_CURVE *RefCurve, 	double Rate, 			DATE From, DATE To ),
		*SQ_VarianceToVolCurve( 		DT_CURVE *VarCurve,		DATE BaseDate ),
		*SQ_BaseVolCurveToVolCurve(		DT_CURVE *BaseVolCurve,	DATE BaseDate,			DATE PricingDate,	double VolToPD ),
		*SQ_VolCurveToBaseVolCurve(		DT_CURVE *VolCurve,		DATE BaseDate,			DATE PricingDate,	double VolToPD ),
		*SQ_VolToVarianceCurve(			DT_CURVE *VolCurve,		DATE BaseDate ),
		*SQ_AddToVarianceCurveUsingWeekendWeights( DT_CURVE *VolCurve, DT_CURVE *ExtraDates, double WkendDayWt, DT_CURVE *CustomDayWeights ),
		*SQ_MakeFwdCurve(				DT_CURVE *DenomRef,		DT_CURVE *QuantUnitRef, DT_CURVE *InterpDates ),
		*SQ_MakeVolCurve(				DT_CURVE *Curve,		DATE BaseDate, 			DT_CURVE *InterpDates ),
		*SQ_MakeSpotCurve(				DT_CURVE *DenomRef,		DT_CURVE *QuantUnitRef,	DT_CURVE *InterpDates,	RDATE SpotDays, char *Holiday1, char *Holiday2 ),
		*SQ_MakeFwdSettleCurve(	  		DT_CURVE *DenomRef,		DT_CURVE *QuantUnitRef,	DT_CURVE *InterpDates,	RDATE SpotDays, char *Holiday1, char *Holiday2 ),
		*SQ_MakeDiscountCurve(			DT_CURVE *CreditCurve,	DT_CURVE *ValueDates, 	DATE DiscTo, 			double DiscountMargin, DT_CURVE *ReinvestmentCurve ),
		*SQ_AdjustedDiscountCurve(		DT_CURVE *RefCurve,		DT_CURVE *InterpDates,	DATE StartDate,	RDATE SpotDays,	char *Holiday1,	char *Holiday2 ),
		*SQ_DiscountCurveToIntRateCurve(DT_CURVE *DiscCurve,	DATE BaseDate ),
		*MktComposeCovarianceCurve(		DT_CURVE *VarCurveA,	DT_CURVE *VarCurveB,	DT_CURVE *CorrCurve,	DATE BaseDate,			int Op );
		
DLLEXPORT DT_GCURVE
		*SQ_MakeSpotDateGCurve(			DT_CURVE *InterpDates,	RDATE SpotDays, char *Holiday1, char *Holiday2 );

DLLEXPORT int		
		SQ_NormalizeCurve( DT_CURVE *Curve, double	value, DATE	date ),
		SQ_CurveShiftKnots( DT_CURVE *Curve, RDATE Tenor, char *Hol1, char *Hol2 );

DLLEXPORT double	MktVarianceToVol( 	double	Variance,	DATE From,		DATE To ),
		MktInterpVarCurve(	DT_CURVE *VolCurve, DATE BaseDate, 	DATE AsOfDate ),
		SQ_InterpLogLinear(	DATE InterpDate, 	DATE FirstDate, double FirstValue,	DATE LastDate,	double LastValue ),
		SQ_InterpVolCurve(	DT_CURVE *VolCurve, DATE BaseDate, 	DATE AsOfDate ),
		SQ_Volatility(		DT_CURVE *VolCurve, DATE From, 	DATE To, DATE Base ),
		SQ_InterpFwdPoint(	DT_CURVE *DenomRef,	DT_CURVE *QuantUnitRef,	DATE InterpDate ),
		SQ_InterestRate( 	DT_CURVE *RefCurve, DATE From, DATE To ),
		SQ_InterpSpotVal(	DT_CURVE *DenomRef,	DT_CURVE *QuantUnitRef,	DATE InterpDate, RDATE SpotDays, char *Holiday1, char *Holiday2 ),
		SQ_DiscountFactor(	DT_CURVE *ReferenceCurve, DATE DiscFrom, DATE DiscTo, double DiscountMargin	);
		
DLLEXPORT double SQ_Annuity(
	DT_CURVE		*RefCurve,		//	Any Reference Curve
	DATE			ResetDate,		//	Reset Date of Annuity
	DATE			StartDate,		//	Start of Annuity
	DATE			FinalDate,		//	Final Date of Annuity
	int				Basis,			//	Daycount Basis
	RDATE			Tenor			//	Annuity Tenor in Months
);

DLLEXPORT double SQ_SwapRate(
	DT_CURVE		*RefCurve,		//	Any Reference Curve
	DATE			ResetDate,		//	Reset Date of Annuity
	DATE			StartDate,		//	Start of Annuity
	DATE			FinalDate,		//	Final Date of Annuity
	int				Basis,			//	Daycount Basis
	RDATE			Tenor			//	Annuity Tenor in Months
);

DLLEXPORT DT_CURVE *SQ_VarianceToVolCurveGeneric(
	DT_CURVE		*VarCurve,
	DATE			BaseDate,
	double			WkendDayWt,			// Weight to be given to weekend days
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife			// Half Life of variance
);

DLLEXPORT DT_CURVE *SQ_MakeVolCurveGeneric(
	DT_CURVE		*Curve,				// Volatility Curve
	DATE			BaseDate,			// Base Date
	DT_CURVE		*InterpDates,	  	// Interpolating Dates
	double			WkendDayWt,	 		// Weight given to weekends during vol interpolation
	DATE			WkendWtCutoffDate,	// Date after which weekend days are worth 1.0
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			OffsetFromCut,		// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction,	// Weight to be given to the last vol day
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife
);

DLLEXPORT DT_CURVE *SQ_MakeFwdVolCurve(
	DT_CURVE		*Curve,				// Volatility Curve
	DATE			BaseDate,			// Base Date for Volatility Curve
	DATE			PricingDate,		// From Date for interpolated vols
	double			VolToPD,			// Volatility to the From Date
	DT_CURVE		*InterpDates,	  	// Interpolating Dates
	double			WkendDayWt,	 		// Weight given to weekends during vol interpolation
	DATE			WkendWtCutoffDate,	// Date after which weekend days are worth 1.0
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			OffsetFromCut,		// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction,	// Weight to be given to the last vol day
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife
);

DLLEXPORT DT_CURVE *SQ_ShiftVolCurveGeneric(
	DT_CURVE		*BaseCurve,			// Curve to be shifted
	DATE			BaseDate,			// Current Base Date of the Curve
	DATE			NewDate,			// New Base Date of the Curve
	double			WkendDayWt,	 		// Weight given to weekends during vol interpolation
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			OffsetFromCut,		// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	ATMVOL_INTERP	InterpMethod,
	double			HalfLife
);

DLLEXPORT DT_CURVE	*SQ_CreateCouponCurve(
	DT_CURVE		*DFCurve,			//	Discount Factor Curve
	DT_ARRAY 		*Tenors,			//	Standard Tenors for Coupons
	DATE			PricingDate,		//	The Security Database Pricing Date
	RDATE			DaysToSpot,			//	Number of Days to Spot Date
	int				CashBasis,			//	Cash Rate Day Count Basis
	int				CoupBasis,			//	Coupon Day Count Basis
	Q_COMPOUNDING_TYPE
					Compounding,		//	Coupon Compounding method: only recognizes Q_ANNUAL and Q_SEMI_ANNUAL 
	char			*CurName			//	Used for date arithmetic
);

DLLEXPORT DT_CURVE	*SQ_BootStrapFromCouponCurve(
	DT_CURVE		*CouponCurve,		//	Coupon Curve
	DT_ARRAY 		*Tenors,			//	Standard Tenors for Coupons
	DATE			PricingDate,		//	The Security Database Pricing Date
	RDATE			DaysToSpot,			//	Number of Days to Spot Date
	int				CashBasis,			//	Cash Rate Day Count Basis
	int				CoupBasis,			//	Coupon Day Count Basis
	Q_COMPOUNDING_TYPE
					Compounding,		//	Coupon Compounding method: only recognizes Q_ANNUAL and Q_SEMI_ANNUAL 
	char			*CurName			//	Used for date arithmetic
);

DLLEXPORT DT_CURVE	*SQ_PerturbBaseCurve(
	DT_CURVE		*BaseCurve,			//	Curve on which Pertubations are to be imposed
	DT_CURVE		*PertCurve,			//	Perturbing Curve
	int				PertMethod,			//	Addititve or Multiplicative Pertubation
	int				AddNewPoints,		//	Flag set to add new points if necessary
	int				AnchorPDValue,		//	Flag set to anchor Pricing Date curve value
	int				EpsilonPert,		//	Absolute or Relative Pertubation
	DATE			PricingDate			//	Security Database Pricing Date
);


DLLEXPORT DT_CURVE *SQ_AssembleDiscountCurve (
	char            *Name,              // Comodity such as currency, metal, etc										  
	DATE			BaseDate,			// Current or Market Date
	int             CurveCount,         // Number of curves to compose
	DT_CURVE        **Curves,           // Array of curves to compose
	DATE			*Boundary,			// Array of ending dates for each curve component. If 0, use all data.
	int				*DayCount,			// Array of day count bases of given curves
	Q_COMPOUNDING_TYPE
					*RateType,			// Array of compounding types of given curves
	int				*IntrType,		  	// Intrument type of each curve
	DATE			*SpotDate			// Spot Date for each curve
);

DLLEXPORT DT_CURVE	*SQ_ForwardCurveByTenor(
		DT_CURVE	*RefCurve,		//	Original Curve
		DT_CURVE	*OldCurve,		//	Previously calculated curve
		RDATE		Tenor,			//	Shifting Tenor
		DATE		BaseDate,		//	Elements before this
		char		*Ccy2,			//	Holiday Calendar
		char		*Ccy1			//	Holiday Calendar
);

DLLEXPORT DT_CURVE *SQ_RefFromSpreadToParCurve(
	DT_CURVE 		*RefCurve,
	DT_CURVE 		*SprCurve,
	RDATE			CoupTenor,
	char			*CoupBasis,
	char			*CashBasis,
	DATE			SpotDays,
	DATE			PriceDate,
	char			*Currency
);

DLLEXPORT DT_CURVE *SQ_WingVolCurve(
	double		QuickDelta,			// Quick delta for returned vol curve
	DT_CURVE	*BaseCurve,			// Vol curve for quick delta of .5
	DT_ARRAY	*VolWings,			// Array of arrays of wing vol offsets
	DT_ARRAY	*VolWingQDeltas		// Quick deltas for each wing vol array
);

DLLEXPORT double SQ_VolQuickDelta(
	double		Strike,
	double		ForwardPoint,
	DATE		PricingDate,
	DATE		ExpirationDate,
	double		BaseVolatility
);

// Mean Reversion fns
DLLEXPORT double BM_BetaDecayFn(
	DT_CURVE	*BetaCurve,
	DATE		StartDate,
	DATE		EndDate
);

DLLEXPORT int BM_BetaDecayIntegral(
	DT_CURVE	*BetaCurve,
	DATE		FutDate,
	DATE		StartDate,
	DATE		EndDate,
	double		*Int1,
	double		*Int2		
);

DLLEXPORT DT_CURVE *BM_BackOutSpotVols(
	DT_CURVE		*MktVols,		// Market Option Implied Vols
	DT_CURVE		*FutDates,		// Market Option Settle Dates
	DT_CURVE		*BetaCurve,		// MR Param - Beta Curve
	double			Lvol,			// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	double			Svol,			// MR Param - Equilibrium Spot Volatility
	DATE			BaseDate		// Pricing Date
);

DLLEXPORT double BM_TermCovariance(
	DT_CURVE		*SpotVols,		// Implied Spot Vols
	DT_CURVE		*BetaCurve,		// MR Param - Beta Curve
	double			Lvol,			// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	DATE			FutDate1,		// Expiration date of first future/forward
	DATE			FutDate2,		// Expiration date of second future/forward
	DATE			StartDate,		// Start of observation period
	DATE			EndDate			// End of observation period
);

DLLEXPORT double BM_TermVariance(
	DT_CURVE		*SpotVols,		// Implied Spot Vols
	DT_CURVE		*BetaCurve,		// MR Param - Beta Curve
	double			Lvol,			// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	DATE			FutDate,		// Expiration date of future/forward
	DATE			StartDate,		// Start of observation period
	DATE			EndDate			// End of observation period
);

DLLEXPORT double BM_TermVolatility(
	DT_CURVE		*SpotVols,		// Implied Spot Vols
	DT_CURVE		*BetaCurve,		// MR Param - Beta Curve
	double			Lvol,			// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	DATE			FutDate,		// Expiration date of future/forward
	DATE			StartDate,		// Start of observation period
	DATE			EndDate			// End of observation period
);

DLLEXPORT int BM_TermSpreadVols(
	DT_CURVE		*SpotVols,		// Implied Spot Vols
	DT_CURVE		*BetaCurve,		// MR Param - Beta Curve
	double			LongVol,		// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	DATE			FutDate1,		// Expiration date of first future/forward
	DATE			FutDate2,		// Expiration date of second future/forward
	DATE			StartDate,		// Start of observation period
	DATE			EndDate,		// End of observation period
	double			*Vol1,			// Returned term vol of first fwd
	double			*Vol2,			// Returned term vol of second fwd
	double			*Corr			// Returned term correlation
);

DLLEXPORT double BM_AverageVolatility(
	DT_CURVE		*SpotVols,		// Implied Spot Vols
	DT_CURVE		*BetaCurve,		// MR Param - Beta Curve
	double			LongVol,		// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	DT_CURVE		*SampleCurve,	// Curve of sample dates and sample weights
	DT_CURVE		*FwdDates,		// Curve of fwd settlement dates
	DATE			StartDate,		// Start of observation period
	DATE			EndDate			// End of observation period
);

DLLEXPORT int MktBasketSpreadVols(
	DT_VALUE	*Basket1,		// Basket represented as structure of weights by ccy
	DT_VALUE	*Basket2,	   	// Basket represented as structure of weights by ccy
	DT_VALUE	*VolStruct,		// Structure of Vol curves indexed by cross
	char		*Denom,			// Denominated ccy
	DATE		BaseDate,		// Start of term
	DATE		FwdDate,		// End of Term
	double		*Volatility1,	// Returned vol of Basket 1
	double		*Volatility2,	// Returned vol of Basket 2
	double		*Correlation	// Returned corr of two baskets
);

DLLEXPORT double MktBasketCovariance(
	DT_VALUE	*Basket1,		// Basket represented as structure of weights by ccy
	DT_VALUE	*Basket2,	   	// Basket represented as structure of weights by ccy
	DT_VALUE	*VolStruct,		// Structure of Vol curves indexed by cross
	char		*Denom,			// Denominated ccy
	DATE		BaseDate,		// Start of term
	DATE		FwdDate			// End of Term
);

DLLEXPORT double MktBasketVariance(
	DT_VALUE	*Basket,		// Basket represented as structure of weights by ccy
	DT_VALUE	*VolStruct,		// Structure of Vol curves indexed by cross
	char		*Denom,			// Denominated ccy
	DATE		BaseDate,		// Start of term
	DATE		FwdDate			// End of Term
);

DLLEXPORT double MktBasketVolatility(
	DT_VALUE	*Basket,		// Basket represented as structure of weights by ccy
	DT_VALUE	*VolStruct,		// Structure of Vol curves indexed by cross
	char		*Denom,			// Denominated ccy
	DATE		BaseDate,		// Start of term
	DATE		FwdDate			// End of Term
);

DLLEXPORT double MktAverageVolatility(
	DT_CURVE	*SampleCurve,	// Curve of sample dates and sample weights
	DT_CURVE	*SettleCurve,	// Curve of settle dates and forward values ( may be NULL )
	DT_CURVE	*VolCrv,		// Volatility Samples
	DATE		BaseDate,		// Start of term
	DATE		FwdDate			// End of Term
);

DLLEXPORT double MktAverageVolatilityTerm(
	DT_CURVE	*SampleCurve,	// Curve of sample dates and sample weights
	DT_CURVE	*SettleCurve,	// Curve of settle dates and forward values ( may be NULL )
	DATE		BaseDate		// Start of term
);

DLLEXPORT int MktAverageSpreadVols(
	DT_CURVE	*FwdCrvL,		// Forward Samples
	DT_CURVE	*FwdCrvS,		// Forward SAmples
	DT_CURVE	*VolCrvL,		// Volatility Samples
	DT_CURVE	*VolCrvS,		// Volatility Samples
	DT_CURVE	*VolCrvX, 		// Cross Volatilities
	DATE		BaseDate,		// Start of term
	DATE		FwdDate,		// End of Term
	double		*VolLong,		// Returned vol of long average
	double		*VolShort,		// Returned vol of short average
	double		*Correlation	// Returned corr of two averages
);



DLLEXPORT DT_CURVE *SQ_CurveInsertKnot(
	DT_CURVE 		*OldCurve,				//	Original curve
	DT_CURVE_KNOT	*NewKnot,				//	Knot to insert, or NULL
	DATE			date,					//	date of knot, or 0
	double			value					//	value of knot to be inserted
);

DLLEXPORT int SQ_RCurveToCurve( DT_CURVE *Curve, DATE Date, char *Currency1, char *Currency2 );

DLLEXPORT DT_CURVE	
		*SQ_CurveQuotient(		DT_CURVE *Dividend,		DT_CURVE *Divisor, 		Q_KNOT_CHOICE choice, int Method ),
		*SQ_CurveProduct(		DT_CURVE *Multiplier,	DT_CURVE *Multiplicand,	Q_KNOT_CHOICE choice, int Method ),
		*SQ_InvertCurve( 		DT_CURVE		*GivenCurve);

DLLEXPORT int SQ_CurveInflate( 
	DT_CURVE		*Curve,
	DATE			Date,		//	On this date, no shift occurs
	double			Shift
);

DLLEXPORT int SQ_TenorArrayFromRCurve(
	DT_VALUE	*Array,				//	DT_VALUE provided by caller
	DT_CURVE	*Curve,				//	An RCurve
	DATE		PriceDate,
	char		*Currency
);

DLLEXPORT int SQ_StrikeFromOverDelta(
	double		Delta,
	double		Spot,
	double		Vol,
	double		Texp,
	double		Rd,
	double		Rf,
	DELTA_TYPE 	DeltaType,
	double		*Strike
);

DLLEXPORT int SQ_StrikeFromUnderDelta(
	double		Delta,
	double		Spot,
	double		Vol,
	double		Texp,
	double		Rd,
	double		Rf,
	DELTA_TYPE	DeltaType,
	double		*Strike
);

DLLEXPORT int SQ_StrikeFromDelta(
	double		Delta,
	double		Spot,
	double		Vol,
	double		Texp,
	double		Rd,
	double		Rf,
	DELTA_TYPE	DeltaType,
	int			IsUnderDelta,
	double		*Strike
);


DLLEXPORT double SQ_DeltaNeutralStrike(
	double		Spot,
	double		Vol,
	double		Texp,
	double		Rd,
	double		Rf,
	int			IsUnderDelta
);

DLLEXPORT int SQ_VolCubicSpline(
	DATE			Expiry,
	double			*Strikes,
	double			*Vols,
	int				NumStrikes,
	DT_STRUCTURE	*StrikeWings,
	DT_ARRAY		*WingParametersCubicSpline,
	RDATE			SpotDays,
	char			*Denominated,
	char			*QuantityUnit,
	char			*OptionPremiumCcy,
	ATM_STRIKE		ATMStrikeType,
	double			WeekendWeight,
	DT_CURVE		*CustomDayWeights,
	double			FinalDayFraction,
	ATMVOL_INTERP	InterpMethod,
	double			HalfLife
);

DLLEXPORT int SQ_StrikeCarpetFromBSDeltaCarpet(
	DT_ARRAY		*DeltaWings,
	DT_ARRAY		*Deltas,
	DT_CURVE		*ATMVolCurve,
	DATE			PricingDate,
	RDATE			SpotDays,
	GsNodeValues		*DeltaTypes,
	ATM_STRIKE		ATMType,
	DT_CURVE		*RdRefCurve,
	DT_CURVE		*RfRefCurve,
	char			*Denominated,
	char			*QuantityUnit,
	char			*PremCcy,
	double			Spot,
	DT_VALUE		*StrikeCarpet
);

DLLEXPORT DT_STRUCTURE *SQ_StrikeCarpetFromDeltaCarpet(
	DT_STRUCTURE 	*WingOffsets,
	DT_CURVE		*BaseVolCurve,
	double			Spot,
	DT_CURVE		*RdCurve,
	DT_CURVE		*RfCurve,
	DATE			VolBaseDate,
	DATE			PricingDate,
	double			VarToPD,
	DATE			SpotDate,
	RDATE			SpotDays,
	char			*Denominated,
	char			*QuantityUnit,
	char			*PremCcy,
	GsNodeValues		*DeltaTypes,
	ATM_STRIKE		ATMType,
	double			WkendWt,
	DATE			CutoffDate,
	DT_CURVE		*CustomDayWeights,
	double			VolMarkOffset,
	double			VolCutPct,
	ATMVOL_INTERP	InterpMethod,
	double			HalfLife
);


DLLEXPORT double SQ_InterpolateFromStrikeSurface(
	DT_STRUCTURE 	*StrikeCarpet,
	DATE			PricingDate,
	DATE			Expiry,
	double			Strike,
	double			WkendDayWt,
	DT_CURVE		*CustomDayWeights,
	double			VolMarkOffset,
	double			VolCutPct,
	double			FinalDayFraction,
	ATMVOL_INTERP	InterpMethod,
	double			HalfLife
);

DLLEXPORT int SQ_ReverseStrikeCarpet(
	DT_VALUE		*NewCarpet,
	DT_VALUE		*OldCarpet
);

DLLEXPORT DT_STRUCTURE *SQ_ReverseStrikeCarpetNew(
	DT_STRUCTURE	*OldCarpet
);

DLLEXPORT DT_STRUCTURE *SQ_ReverseWingOffsets(
	DT_STRUCTURE	*OldOffsets
);

DLLEXPORT DT_CURVE *SQ_ATMStrikeCurve(
	DT_CURVE		*TermVolCrv,
	double			Spot,
	DATE			PricingDate,
	RDATE			SpotDays,
	char			*Denominated,
	char			*QtyUnit,
	char			*PremiumCcy,
	ATM_STRIKE		ATMStrikeType,
	DT_CURVE		*RdDiscountCrv,
	DT_CURVE		*RfDiscountCrv
);


DLLEXPORT DATE SQ_VolResolveRDate(
	RDATE			RDate,
	DATE			MktDate,
	char			*Denominated,
	char			*QuantityUnit,
	RDATE			SpotDays,
	int				Offset
);

DLLEXPORT double SQ_AdjustedNumberofDays(
	double		    WkendDayWt,
	DATE		    From,
	DATE		    To,
	DT_CURVE	    *CustomDayWeights
);

DLLEXPORT double SQ_VolatilityDays(
	DATE		    Start,
	DATE		    End,
	double		    WkendDayWt,
	DT_CURVE	    *CustomDayWeights,
	double		    InitialOffset,
	double		    FinalOffset
);

DLLEXPORT double SQ_VolatilityDaysByCutPct(
	DATE		    Start,
	DATE		    End,
	double		    WkendDayWt,
	DT_CURVE	    *CustomDayWeights,
	double		    OffsetFromCut,
	double		    VolCutFraction,
	double		    FinalDayFraction
);


DLLEXPORT DT_STRUCTURE *SQ_VariancesFromVolCurve(
	DT_CURVE	    *VolCurve,		
	DATE		    BaseDate,
	double		    WkendDayWeight,
	DATE		    CutoffDate,
	DT_CURVE	    *VolDayWeights,
	double		    InitialOffset,
	double		    FinalOffset
);

DLLEXPORT double SQ_InterpVolsFlatFwdVar(
	DATE			BaseDate,			// Base Date for Vols
	DATE			InterpDate,			// Interpolating Date
	DATE			FirstDate,			// Vol Date before Interpolating Date
	double			FirstVol,			// Vol on First Date
	DATE			LastDate,			// Vol Date after Interpolating Date
	double			LastVol,			// Vol on Last Date
	double			WkendDayWt,			// Weight to be given to weekend days
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction	// Fraction of the final day before option expiration
);

DLLEXPORT double SQ_InterpVolsFlatFwdSqrtVar(
	DATE			BaseDate,			// Base Date for Vols
	DATE			InterpDate,			// Interpolating Date
	DATE			FirstDate,			// Vol Date before Interpolating Date
	double			FirstVol,			// Vol on First Date
	DATE			LastDate,			// Vol Date after Interpolating Date
	double			LastVol,			// Vol on Last Date
	double			WkendDayWt,			// Weight to be given to weekend days
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction	// Fraction of the final day before option expiration
);

DLLEXPORT double SQ_InterpVolsLinear(
	DATE			BaseDate,			// Base Date for Vols
	DATE			InterpDate,			// Interpolating Date
	DATE			FirstDate,			// Vol Date before Interpolating Date
	double			FirstVol,			// Vol on First Date
	DATE			LastDate,			// Vol Date after Interpolating Date
	double			LastVol				// Vol on Last Date
);

DLLEXPORT double SQ_InterpVolsHalfLife(
	DATE			BaseDate,			// Base Date for Vols
	DATE			InterpDate,			// Interpolating Date
	DATE			FirstDate,			// Vol Date before Interpolating Date
	double			FirstVol,			// Vol on First Date
	DATE			LastDate,			// Vol Date after Interpolating Date
	double			LastVol,			// Vol on Last Date
	double			WkendDayWt,			// Weight to be given to weekend days     
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction,	// Fraction of the final day before option expiration
	double			HalfLife			// Half Life of variance
);

DLLEXPORT double SQ_InterpVolsGeneric(
	DATE			BaseDate,			// Base Date for Vols
	DATE			InterpDate,			// Interpolating Date
	DATE			FirstDate,			// Vol Date before Interpolating Date
	double			FirstVol,			// Vol on First Date
	DATE			LastDate,			// Vol Date after Interpolating Date
	double			LastVol,			// Vol on Last Date
	double			WkendDayWt,			// Weight to be given to weekend days
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction,	// Fraction of the final day before option expiration
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife			// Half Life of variance expressed as a NUMBER OF DAYS
);

DLLEXPORT double SQ_InterpVolCurveFlatFwdVar(
	DT_CURVE		*VolCurve,			// Volatility Curve
	DATE			BaseDate,			// Base Date for Volatility Curve
	DATE			InterpDate,			// Interpolating Date
	double			WkendDayWt,			// Weight to be given to weekend days
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction	// Fraction of the final day before option expiration
);

DLLEXPORT double SQ_InterpVolCurveFlatFwdSqrtVar(
	DT_CURVE		*VolCurve,			// Volatility Curve
	DATE			BaseDate,			// Base Date for Volatility Curve
	DATE			InterpDate,			// Interpolating Date
	double			WkendDayWt,			// Weight to be given to weekend days
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction	// Fraction of the final day before option expiration
);

DLLEXPORT double SQ_InterpVolCurveLinear(
	DT_CURVE		*VolCurve,			// Volatility Curve
	DATE			BaseDate,			// Base Date for Volatility Curve
	DATE			InterpDate			// Interpolating Date
);

DLLEXPORT double SQ_InterpVolCurveHalfLife(
	DT_CURVE		*VolCurve,			// Volatility Curve
	DATE			BaseDate,			// Base Date for Volatility Curve
	DATE			InterpDate,			// Interpolating Date
	double			WkendDayWt,			// not used in this routine
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction,	// Fraction of the final day before option expiration
	double			HalfLife			// Half Life of variance
);

DLLEXPORT double SQ_InterpVolCurveGeneric(
	DT_CURVE		*VolCurve,			// Volatility Curve
	DATE			BaseDate,			// Base Date for Volatility Curve
	DATE			InterpDate,			// Interpolating Date
	double			WkendDayWt,			// Weight to be given to weekend days
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction,	// Fraction of the final day before option expiration
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife			// Half Life of variance
);

DLLEXPORT double SQ_VarianceByTimeFlatFwdVar(
	DT_VECTOR		*Variances,			// Vector of variances
	DT_VECTOR   	*Times,				// Vector of corresponding times
	double			InterpTime,			// Time at which to interpolate variance
	double			*InstVar			// limit as t->InterpTime of dVariance/dT
);

DLLEXPORT double SQ_VarianceByTimeFlatFwdSqrtVar(
	DT_VECTOR		*Variances,			// Vector of variances
	DT_VECTOR   	*Times,				// Vector of corresponding times
	double			InterpTime,			// Time at which to interpolate variance
	double			*InstVar			// limit as T->InterpTime of dVariance/dT
);

DLLEXPORT double SQ_VarianceByTimeHalfLife(
	DT_VECTOR		*Variances,			// Vector of variances
	DT_VECTOR   	*Times,				// Vector of corresponding times
	double			InterpTime,			// Time at which to interpolate variance
	double			HalfLife,			// Input parameter
	double			*A,					// Model parameter, "long-term instantaneous variance"
	double			*B,					// Model parameter, "short-term instantaneous variance shock"
	double			*InstVar			// limit as T->InterpTime of dVariance/dT
);

DLLEXPORT double SQ_VarianceByTimeHLByParameters(
	double			A,					// Model parameter, "long-term instantaneous variance"
	double			B,					// Model parameter, "short-term instantaneous variance shock"
	double			HalfLife,			// Input parameter
	double			InterpTime,			// Time at which to interpolate variance
	double			*InstVar			// limit as T->InterpTime of dVariance/dT
);

DLLEXPORT double SQ_VarianceByTime(
	DT_VECTOR		*Variances,			// Vector of variances
	DT_VECTOR   	*Times,				// Vector of corresponding times
	double			InterpTime,			// Time at which to interpolate variance
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife,			// Input parameter for Half Life interp method
	double			*InstVar			// limit as T->InterpTime of dVariance/dT
);

DLLEXPORT double SQ_VolatilityByTime(
	DT_VECTOR		*Variances,			// Vector of variances
	DT_VECTOR   	*Times,				// Vector of corresponding times
	double			StartTime,			// Start of volatility period
	double			EndTime,			// End of volatility period
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife,			// Input parameter for Half Life interp method
	double			CalendarTime		// Time that variance should be divided by to get volatility
);

DLLEXPORT double SQ_VolatilityGeneric(
	DT_CURVE		*VolCurve,
	DATE			Base,				// Base Date of Vol Curve
	DATE			From,
	DATE			To,
	double			WkendDayWtFrom,		
	double			WkendDayWtTo,
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFractionFrom,	// Fraction of the day on which to start the vol measurement
	double			FinalDayFractionTo,	// Fraction of the final day before option expiration
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife			// Half Life of variance
);

DLLEXPORT double SQ_VolatilitySV(
	DT_CURVE		*VolCurve,
	DATE			Base,				// Base Date of Vol Curve
	DATE			From,
	DATE			To,
	double      	vvol,
	double			WkendDayWtFrom,		
	double			WkendDayWtTo,
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFractionFrom,	// Fraction of the day on which to start the vol measurement
	double			FinalDayFractionTo,	// Fraction of the final day before option expiration
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife			// Half Life of variance
);

DLLEXPORT double SQ_VolatilityFromBaseCurve(
	DT_CURVE		*BaseCurve,			// Volatility Curve based at BaseDate
	DATE			BaseDate,			// Base Date for Volatility Curve
	DATE			FromDate,			// Vol is measured From this date
	DATE			InterpDate,			// Interpolating Date
	double			VolToFrom,			// Volatility from BaseDate to FromDate
	double			WkendDayWt,			// Weight to be given to weekend days
	DT_CURVE		*CustomDayWeights,	// Custom weights for special vol days
	double			BaseOffsetFromCut,	// Difference, in pct of a day, between when vols were marked and the cut
	double			VolCutFraction,		// Fraction of a day until the relevant cut
	double			FinalDayFraction,	// Fraction of the final day before option expiration
	ATMVOL_INTERP	InterpMethod,		// Interpolation Method
	double			HalfLife			// Half Life of variance
);




#endif

