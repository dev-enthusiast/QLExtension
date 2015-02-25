/* $Header: /home/cvs/src/quant/src/q_swap2.h,v 1.14 2000/04/07 10:08:50 goodfj Exp $ */
/****************************************************************
**
**	Q_SWAP2.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.14 $
**
****************************************************************/

#if !defined( IN_Q_SWAP2_H )
#define IN_Q_SWAP2_H

#if !defined( IN_DATE_H )
#include <date.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#if !defined( IN_DATATYPE_H )
#include <datatype.h>
#endif
#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif


EXPORT_C_QUANT double Q_AnnuityCalc	(
	DT_CURVE		*DiscCurve,	//  discount curve
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT		basis,		//	daycountbasis
	int				frequency	//	payment frequency
	);


EXPORT_C_QUANT double Q_IntSwapCalc (
	DT_CURVE		*DiscCurve,	//  discount curve
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT		basis,		//	daycountbasis
	int				frequency	//	payment frequency
	);
	

EXPORT_C_QUANT double Q_IntSwapPvCalc (
	DT_CURVE		*DiscCurve,	//  discount curve
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT		basis,		//	daycountbasis
	int				frequency,	//	payment frequency
	double			fixed		//	fixed rate
	);
	
EXPORT_C_QUANT double Q_CapCalc(
	DT_CURVE		*DiscCurve, //  Discount Curve
	Q_DATE_VECTOR	*v_dates,	//	vector of dates for volatilities
	Q_VECTOR		*vols,		//	vector of volatilities
	DATE			today,		//	today
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT		basis,		//	daycountbasis
	int				frequency,	//	payment frequency
	double			strike		//	strike (- for Floor + Cap)
	);

EXPORT_C_QUANT double Q_LibCapCalc	(
	Q_OPT_TYPE  	callPut,    //  call (cap) or put (floor)
	DT_CURVE		*DiscCurve,	//  discount curve
	Q_DATE_VECTOR	*v_dates,	//	vector of dates for volatilities
	Q_VECTOR		*vols,		//	vector of volatilities
	DATE			today,		//	today
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT 	basis,   	//	daycountbasis
	int				frequency,	//	payment frequency
	double			strike		//	strike rate
	); 

EXPORT_C_QUANT double Q_LibAnnuity	(
	Q_DATE_VECTOR	*dates,		//	vector of dates
	Q_VECTOR		*dscfs,		//	vector of discount factors
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT		basis,		//	daycountbasis
	int				frequency	//	payment frequency
	);

EXPORT_C_QUANT double Q_IntSwap (
	Q_DATE_VECTOR	*dates,		//	vector of dates
	Q_VECTOR		*discf,		//	vector of discount factors
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT		basis,		//	daycountbasis
	int				frequency	//	payment frequency
	);
	
EXPORT_C_QUANT double Q_IntSwapPv	(
	Q_DATE_VECTOR	*dates,		//	vector of dates
	Q_VECTOR		*discf,		//	vector of discount factors
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT		basis,		//	daycountbasis
	int				frequency,	//	payment frequency
	double			fixed		//	fixed rate
	);
	
EXPORT_C_QUANT double Q_LibCap	(
	Q_OPT_TYPE  	callPut,    //  call (cap) or put (floor)
	Q_DATE_VECTOR	*d_dates,	//	vector of dates for discount factors
	Q_VECTOR		*dscfs,		//	vector of discount factors
	Q_DATE_VECTOR	*v_dates,	//	vector of dates for volatilities
	Q_VECTOR		*vols,		//	vector of volatilities
	DATE			today,		//	today
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT 	basis,   	//	daycountbasis
	int				frequency,	//	payment frequency
	double			strike		//	strike rate
	); 

		
EXPORT_C_QUANT double Q_ParRateAnnuity (
	DATE		SpotDate,	//	Spot date
	DATE		SwapTenor,	//	Tenor of Swap eg. 2yrs or 3m
	DATE		CoupTenor,	//	Tenor of Coupon if Swap Tenor in years
	double		ParRate,	//	Rate used for discounting
	Q_DAY_COUNT	Basis,		//	daycountbasis
	char		*Calendar1,
	char		*Calendar2
	);

EXPORT_C_QUANT double Q_SwapRate (
	char            *CCY,        //  Currency						 
	DT_CURVE		*DiscCurve,  //  Discount Curve
	DATE			spotdate,	 //	 spot date
	int			    yn,	         //	 year from spot date
	Q_COMPOUNDING_TYPE
					rateType,    //  interest rate type
	int             dayCount     //  day count convention
	);

EXPORT_C_QUANT double Q_Cap(
	Q_DATE_VECTOR	*d_dates,	//	vector of dates for discount factors
	Q_VECTOR		*dscfs,		//	vector of discount factors
	Q_DATE_VECTOR	*v_dates,	//	vector of dates for volatilities
	Q_VECTOR		*vols,		//	vector of volatilities
	DATE			today,		//	today
	DATE			effective,	//	start date
	DATE			maturity,	//	effective date
	Q_DAY_COUNT		basis,		//	daycountbasis
	int				frequency,	//	payment frequency
	double			strike		//	strike (- for Floor + Cap)
	);

#endif

