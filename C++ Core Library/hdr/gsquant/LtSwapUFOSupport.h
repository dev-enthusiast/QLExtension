/****************************************************************
**
**	LTSWAPUFOSUPPORT.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/LtSwapUFOSupport.h,v 1.30 2001/03/13 12:01:28 goodfj Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_LTSWAPUFOSUPPORT_H )
#define IN_GSQUANT_LTSWAPUFOSUPPORT_H

#include <gsquant/base.h>
#include <gscore/GsRDateVector.h>
#include <gscore/GsUnsignedVector.h>
#include <gscore/GsVectorVector.h>
#include <gscore/GsPTimeCalc.h>
#include <gsquant/Enums.h>
#include <gscore/GsDiscountCurve.h>
#include <gsdt/GsDtGenVector.h>
#include <gsdt/GsDtRDate.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsRDateVector 
		LtSwapCompoundRDatesVector( const GsRDate& start, const GsRDate& end, const GsRDate& ref,
									const GsRDate& enddates );
EXPORT_CPP_GSQUANT GsRDate 
		LtSwapAdjustFinal( const GsRDate & endDates, const GsDateGen& terminationCal );

EXPORT_CPP_GSQUANT GsRDateVector 
		LtSwapAdjustFinalComp( const GsRDateVector & endDates, const GsDateGen&	terminationCal );

EXPORT_CPP_GSQUANT GsRDateVector 
		LtSwapFixingRDates( const GsRDateVector& resetDates, const GsRDate& delay );

EXPORT_CPP_GSQUANT GsRDateVector 
	 LtSwapResetRDates( const GsRDateVector& resetDates );

EXPORT_CPP_GSQUANT GsRDate
		LtSwapPeriodStartNoComp( const GsRDate& effective,  const GsRDate & periodEndDates );

EXPORT_CPP_GSQUANT GsRDate 
		LtSwapAdjResetRDatesNoComp(const GsRDate & unadj,     const GsRDate & payment,
								   const GsDateGen &  stdCal, const GsDateGen & preceedingCal );
EXPORT_CPP_GSQUANT GsRDate 
		LtSwapPaymentAnnuityStart( const GsRDate & effective, const GsRDate & periodEnd );

EXPORT_CPP_GSQUANT GsRDate 
		LtSwapPaymentAnnuityStartComp( const GsRDateVector & compDates );


EXPORT_CPP_GSQUANT GsRDateVector 
		LtSwapAdjResetRDatesWithComp(const GsRDateVector & unadj, const GsRDate & payment,
									 const GsDateGen & stdCal,    const GsDateGen & preceedingCal );

EXPORT_CPP_GSQUANT GsDtArray
		*LtSwapExpandRDateVector( const GsRDateVector & vec	);

EXPORT_CPP_GSQUANT GsVector 
		LtSwapUsedFloatingRates( const GsUnsignedVector& mask, const GsVector& rates );

EXPORT_CPP_GSQUANT GsVector 
		LtSwapUsedFloatingRatesNonStd( const GsUnsignedVector&	mask, const GsVector& rates,
									   int	useInitial, double	setRate,
									   LT_INTERP  initial,	double initialRate,	
									   LT_INTERP  final,	double finalRate );

EXPORT_CPP_GSQUANT GsVectorVector
		LtSwapCompoundDCF(	const GsRDateVector & vec, const GsPTimeCalc& AccrualTermCalc );

EXPORT_CPP_GSQUANT GsRDateVector
		LtSwapAdjustRDatesVector( const GsRDateVector & vec, const GsRDate & inc );

EXPORT_CPP_GSQUANT GsDateVector
		LtSwapFlattenRDatesVector(const GsRDateVector & vec );

EXPORT_CPP_GSQUANT GsVector 
		LtSwapFlattenVector(const GsVectorVector&	vec );

EXPORT_CPP_GSQUANT int 
		LtSwapCompoundPeriodApplicableRef(
			GsVectorVector& rv,  const GsUnsignedVector& mask,
			const GsVector& amortization,		const GsVectorVector& dcfs,
			const GsVector& spreads,			const GsVector& floating,
			LT_IRM  interestMethod,				double multSpread = 1.0 );

EXPORT_CPP_GSQUANT GsVectorVector
		LtSwapCompoundPeriodApplicable( const GsUnsignedVector& mask,
										const GsVector& amortization,		const GsVectorVector& dcfs,
								  	    const GsVector& spreads,			const GsVector& floating,
										LT_IRM  interestMethod,				double multSpread = 1.0 );

EXPORT_CPP_GSQUANT GsVectorVector
		LtSwapCompoundPeriodFlat( const GsUnsignedVector& mask,
                                  const GsVector& amortization,  const GsVectorVector& dcfs,
								  const GsVector& spreads,		 const GsVector& floating,
								  LT_IRM  interestMethod,        double multSpread = 1.0 );

EXPORT_CPP_GSQUANT int 
		LtSwapSumSubVectorsRef( GsVector& rv, const GsVectorVector& vec );

EXPORT_CPP_GSQUANT GsVector
		LtSwapSumSubVectors( const GsVectorVector & vec );

EXPORT_CPP_GSQUANT GsVector 
		LtSwapPaymentDCF(const GsRDate & effective,  const GsRDate & periodEnd, const GsPTimeCalc& AccrualTermCalc );

EXPORT_CPP_GSQUANT GsRDate 
		LtSwapUnadjustedPeriodEnd( const GsRDate & period,      const GsRDate & effective,
								   const GsRDate & firstRoll,   const GsRDate & lastRoll,
								   const GsRDate & termination, 
								   LT_ROLL roll = LT_ROLL_DEFAULT );

EXPORT_CPP_GSQUANT GsStringVector
		LtSwapFindInterpolationMaturities(	const GsStringVector & ValidTenors, const GsDateVector & endDates,
											const GsDate & periodEndDate );

EXPORT_CPP_GSQUANT GsVector 
		LtSwapFloatPriceNotApplic(const GsUnsignedVector& mask, const GsVector& amortization,  const GsVector& dcfs, 
								  const GsVector& spreads, const GsVector& floating,      LT_IRM interestMethod, double multSpread = 1.0 );

EXPORT_CPP_GSQUANT GsDtArray* LtSwapFloatPriceNotApplicMC(
	const GsUnsignedVector & mask,   	
	const GsVector & amortization,		
	const GsVector & dcfs,				
	const GsVector & spreads,       	
	const GsDtArray & floating,			
	LT_IRM           interestMethod,  	
	double			 multSpread			
);

EXPORT_CPP_GSQUANT GsUnsignedVector 
		LtSwapFixingMaskCompounding(const GsRDateVector & vec,	int	 cursor );

EXPORT_CPP_GSQUANT GsDtArray *
		LtSwapExpandDateVector( const GsDateVector & vec );

EXPORT_CPP_GSQUANT GsVectorVector
		LtSwapMultipleFloatingRateArray( const GsRDateVector & vec,	const GsVector & rates );

EXPORT_CPP_GSQUANT GsVectorVector
		LtSwapFloatingRateArray( const GsVector & rates );

EXPORT_CPP_GSQUANT double 
		LtSwapParYieldAnnuity(const GsUnsignedVector& mask,	const GsVector&	amounts,
							  const GsRDate&	  payDates,	const GsVector&	dcfs,
							  const GsDate&			   ref,	const GsPTimeCalc&	termCalc,
							  double   				   irr );

EXPORT_CPP_GSQUANT double 
		LtSwapMoneyMarketPrice(
			const GsDiscountCurve&,
			const GsDate&,
			const GsDate&,
			double term
);

CC_END_NAMESPACE

#endif 
