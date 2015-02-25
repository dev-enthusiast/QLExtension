/****************************************************************
**
**	QTYPES.H		- Typedefs, data manipulation fns for use in quant routines
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/num/src/qtypes.h,v 1.9 2001/11/27 23:02:39 procmon Exp $
**
****************************************************************/

#if !defined( IN_QTYPES_H )
#define IN_QTYPES_H

#include <qenums.h>

#ifndef IN_DATE_H
#include <date.h>
#endif
#ifndef IN_DTCORE_H
#include <dtcore.h>
#endif


#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif

#undef Q_VECTOR_DEBUG

/*
** Forward curve structure
*/
typedef struct q_fwd_rate
{
	DATE	fwdDate;
	double	fwdPt;
	double	fwdRate;
} Q_FWD_RATE;

typedef struct q_cash_flow
{
	DATE 	fwdDate;
	double	termRate;
	double	cashFlow;
} Q_CASH_FLOW;

typedef struct q_mth_curv
{
	double	time;
	double	vol;
	double	rd;
	double	rf;
} Q_MTH_CURV;

/* This structure is used in average rate option routines */

typedef struct q_vol_fwd
{
	DATE      avgdate;
	double    vol;
	double    fwd;
}   Q_VOL_FWD;

/*  structures used by basket option routines  */

typedef struct q_option
{
	double		Quantity;	/* Option Parameters */
	double		Strike;
	DATE		Expiration;
	int			Type;
	double 		Discount;
	double 		Price;
} Q_OPTION;


typedef struct q_vars
{
	double		*Forward		;	 /* forwards of vars */
	double		*Mean			;	 /* calculated means */
	double 		*Initial		;	 /* initial values */
	double		*Weight	    	;    /*weight in basket */
	double		*Prob			;
	double		*ProbVar	    ;
	int			Dimension   	;
	double      *Exp			;    /*expiration value*/
} Q_VARS;

typedef struct q_prob
{
	double		*Dist	;    /* binomial dist.*/
	double		*Var	;    /* binomial var */
	double		Range;	     /* +/- range of var */
	int			Grid;		 /* gridsize */
} Q_PROB;


typedef struct ICPParametersStructure
{
	DATE 	IssueDate;			// Date on which coupon began to accrue
	DATE 	SettlementDate;		// Date that transaction will settle
	DATE 	MaturityDate;		// Date on which redemption is paid

	double	Yield;				// Discount rate for present value (in absolute; e.g. 0.0858)
	Q_COMPOUNDING_TYPE
			YieldInterestMethod;
	int		YieldDayCount;

	double	Coupon;	 			// Coupon (in percent; e.g. 7.5)
	int		CouponDayCount;

	double	Price;				// Price (in percent; e.g. 100)

	int		NumberOfCurrencies;	// Number of items in following lists
	double	*Weights;			// List of weights of each currency in redemption formula
	double	*Spots;				// List of beginning spot of each currency
	double	*Forwards;			// List of forward price from SettlementDate to MaturityDate for each currency
	int		*InverseQuoted;		// List of whether given currency's spot and forward price is inverted

	double	PrincipalIntercept;	// Amount of principal paid if all the currencies remain unchanged

	double	AnnualCosts;		// Other per annum costs (in percent)
	double	OneTimeCosts;		// Other initial costs (in percent)

} Q_ICP_PARAMETERS;

typedef struct Q_CSCPParametersStructure
{
	DATE 	IssueDate;			// Date on which coupon began to accrue
	DATE 	SettlementDate;		// Date that transaction will settle
	DATE 	MaturityDate;		// Date on which redemption is paid
	DATE	FirstCouponDate;	// Date of first coupon payment
	DATE	LastCouponDate;		// Date of last coupon prior to maturity date

	Q_COMPOUNDING_TYPE	
			Period;				// Q_ANNUAL or Q_SEMI_ANNUAL

	int     NumCouponDates;		// Number of remaining coupon dates (incl. redemption)
	DATE    *CouponDates;		// Remaining Coupon dates (incl. redemption)
	double	FixedCoupon;		// Last fixed coupon value .
	int		CouponDayCount;		//

	double	*Forwards;			// List of forward price from SettlementDate to MaturityDate in FX/DENOM

	double	*Yield;				// Discount rate for DENOM (in absolute; e.g. 0.0858, for each coupon date.)
	Q_COMPOUNDING_TYPE
			YieldInterestMethod;
	int		YieldDayCount;

	double	*VolBid;			// Array of bid vols per coupon date
	double	*VolAsk;			// Array of ask vols per coupon date

	int     BullBear;			// +/-1 for BULL or Bear

	double  FXQuantity;			// Quantity of FX per coupon option.

	double  Strike1;			// First strike	in FX/DENOM	 ( < Strike2 )
	double  Strike2;			// Second strike in FX/DENOM ( > Strike1 )


	double	PrincipalIntercept;	// Minimum Redemption

	double	AnnualCosts;		// Other per annum costs (in percent)
	double	OneTimeCosts;		// Other initial costs (in percent)

}		Q_CSCP_PARAMETERS;

typedef struct lbresults
{
	double	premRet;
	double	pErr;
	double	coarseLbkPErr;
	double	benchprem;
	double	rho;
	double	delta;
} LBRESULTS;

typedef struct MultiDkoParametersList
{
	int				isKOUp;
	int				isKODown;
	double			Texp;
	Q_OPT_TYPE    	isCall;
	double			spot;	 		
	double			strike;	 		
	double			lowOutVal;		
	double			highOutVal;		
	double			lowRebate;
	double			highRebate;
	Q_REBATE_TYPE   rebType;	 	
} PARAMETERS_LIST_MULTI_DKO;



#define A_M_PACKED				0x8000		/* 1000 0000 0000 0000 */
#define A_M_SYMETTRIC			0x4000		/* 0100 0000 0000 0000 */
#define A_M_UPPERTRIANGULAR	0x2000		/* 0010 0000 0000 0000 */

/*
**	Vector of dates necessary for adlib/slang use.
*/

typedef Q_VECTOR Q_DATE_VECTOR;

typedef struct q_ifrn
{
	double      InitCoupon;
	DATE        *Grid;
	Q_VECTOR	*DiscountRate;
	double      Spot;
	double      InitSpot;
	double      Weight;
	int         DayCount;
} Q_IFRN_INPUT;

typedef struct
{
	double      InitCoupon;
	DATE        *Grid;
	Q_VECTOR	*DiscountRate;
	double      Weight;
	int         DayCount;
} Q_IFRN_DENOM;

typedef enum 
{
		SWAP, 
		CALL,
		PUT 
} ROL_OPT_TYPE;

typedef struct DT_curvemat
{
	int			Rows,
				Cols;  
	
	DT_CURVE	**M;	    	 // The data block

} Q_CURVE_MATRIX;


#endif

