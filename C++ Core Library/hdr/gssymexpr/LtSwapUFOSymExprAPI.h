/****************************************************************
**
**	LTSWAPUFOSYMEXPRAPI.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/LtSwapUFOSymExprAPI.h,v 1.3 2001/11/27 22:47:49 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_LTSWAPUFOSYMEXPRAPI_H )
#define IN_GSSYMEXPR_LTSWAPUFOSYMEXPRAPI_H

#include <gssymexpr/base.h>

#include <gssymexpr/GsSymbolicExpressionVector.h>
#include <gssymexpr/GsSymbolicConstant.h>
#include <gssymexpr/GsSymbolicVariableRep.h>
#include <gssymexpr/GsSymbolicWeightedSum.h>

#include <gssymexpr/GsDtSymbolicExpression.h>

#include <gsquant/Enums.h>
#include <gsdt/GsDtArray.h>
#include <gscore/GsRDateSeries.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsStringVector.h>
#include <gscore/GsUnsignedVector.h>
#include <gsdt/GsDtGenVector.h>
#include <gsdt/GsDtDate.h>
#include <gscore/GsVector.h>
#include <gsdt/GsDtVector.h>
#include <gsdt/GsDtString.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSSYMEXPR GsDtArray
		*LtSwapCompoundPeriodExprApplicable(const GsVector& amortization,		const GsDtArray& dcfs,
									  	    const GsVector& spreads,			const GsSymbolicExpressionVector& floating,
											LT_IRM  interestMethod );

EXPORT_CPP_GSSYMEXPR GsDtArray
		*LtSwapCompoundPeriodExprFlat(const GsVector& amortization,   const GsDtArray& dcfs,
									  const GsVector& spreads,		const GsSymbolicExpressionVector& floating,
									  LT_IRM  interestMethod );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector
		LtSwapSumSubExprVectors( const GsDtArray & vec );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector 
		LtSwapFixedPriceExpression(	const GsSymbolicExpressionVector& payDates, const GsVector& payAmounts );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector 
		LtSwapFloatingPriceExpression( const GsSymbolicExpressionVector& payExpressions, const GsSymbolicExpressionVector& payAmounts );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector 
		LtSwapFloatPriceExprNotApplic(const GsVector& amortization,  const GsVector& dcfs, const GsVector& spreads,
									  const GsSymbolicExpressionVector& floating,
									  LT_IRM          interestMethod );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression 
		LtSwapLinearInterpolateExpr(const GsStringVector& prs,  const GsDateVector& endDates,
									const GsDate& fixing,   	 const GsDate&	 periodEnd );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector 
		LtSwapPaymentExpressions(const GsDateVector& payDates, const GsString& denominated );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector 
		LtSwapFloatingRateExpr(const GsDateVector& fixingDates, const GsString& pr);

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector 
		LtSwapFloatingRateNonStdExpr(const GsDateVector& fixingDates, const GsString& pr,
									 int useInitialRate,
									 double			 initialRate,     LT_INTERP	 initialPeriodInterp,
									 const GsSymbolicExpression & initialPeriodExpression, 
									 LT_INTERP		 finalPeriodInterp,
									 const GsSymbolicExpression & finalPeriodExpression );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector 
		LtSwapMaskExpressions(const GsSymbolicExpressionVector&	expr,
							  const GsVector& mask, const GsVector&	vals );

CC_END_NAMESPACE

#endif 
