/* $Header: /home/cvs/src/quant/src/q_range.h,v 1.20 2000/03/03 12:04:46 goodfj Exp $ */
/****************************************************************
**
**	Q_RANGE.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.20 $
**
****************************************************************/

#if !defined( IN_Q_RANGE_H )
#define IN_Q_RANGE_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QFADE double Q_RangeBin (
	double		spot,				// current spot price
	double		lowStrike,			// lower strike
	double		highStrike,			// higher strike
	DATE 		pricingDate,		// pricing date
	DATE 		startDate,			// start of fading
	DATE 		endDate,			// expiration date
	Q_VECTOR	*timesVol,			// gridpoints for volatility curve
	Q_VECTOR	*valuesVol,			// volatility curve
	Q_VECTOR	*timesRover,		// gridpoints for domestic rates
	Q_VECTOR    *valuesRover,		// domestic rate curve
	Q_VECTOR	*timesRunder,		// gridpoints for foreign rates
	Q_VECTOR	*valuesRunder,		// foreign rate curve
	int			period,				// number of time units
	char		*unit,				// time unit (day, month, etc)
	double		payout,				// total payout if no fading occurs
	char	    *Ccy1,				// overlying currency
	char	    *Ccy2,				// underlying currency
	int		    UnfadedPastResets	// Unfaded past resets
	);

EXPORT_C_QFADE double Q_RangeBinQ (
	double		spot,				 // current spot price
	double		lowStrike,			 // lower strike
	double		highStrike,			 // higher strike
	DATE 		pricingDate,		 // pricing date
	DATE 		startDate,			 // start of fading
	DATE 		endDate,			 // expiration date
	Q_VECTOR	*OUtimesVol,		 // gridpoints for over/under vol curve
	Q_VECTOR	*ODtimesVol,		 // gridpoints for over/denom vol curve
	Q_VECTOR	*UDtimesVol,		 // gridpoints for under/under vol curve
	Q_VECTOR	*OUvaluesVol,		 // over/under volatility curve
	Q_VECTOR	*ODvaluesVol,		 // over/denom volatility curve
	Q_VECTOR	*UDvaluesVol,		 // under/denom volatility curve
	Q_VECTOR	*timesRover,		 // gridpoints for domestic rates
	Q_VECTOR    *valuesRover,		 // domestic rate curve
	Q_VECTOR	*timesRunder,		 // gridpoints for underlying rates
	Q_VECTOR	*valuesRunder,		 // underlying rate curve
	Q_VECTOR	*timesRdenom,		 // gridpoints for denominated rates
	Q_VECTOR	*valuesRdenom,		 // denominated rate curve
	int			period,				 // number of time units
	char		*unit,				 // time unit (day, month, etc)
	double		payout,				 // total payout if no fading occurs
	char	    *Ccy1,				 // overlying currency
	char	    *Ccy2,				 // underlying currency
	int		    UnfadedPastResets	 // Unfaded past resets
	);

EXPORT_C_QFADE double Q_RangeBinDual (
	double		spot,				// current spot price
	double		spot2,				// current spot price
	double		lowStrike,			// lower strike
	double		lowStrike2,			// lower strike
	double		highStrike,			// higher strike
	double		highStrike2, 		// higher strike
	DATE 		pricingDate,		// pricing date
	DATE 		startDate,			// start of fading
	DATE 		endDate,			// expiration date
	Q_VECTOR	*timesVol,			// gridpoints for volatility curve
	Q_VECTOR	*timesVol2,			// gridpoints for 2nd volatility curve
	Q_VECTOR	*timesXVol,	    	// gridpoints for cross volatility curve
	Q_VECTOR	*valuesVol,			// volatility curve
	Q_VECTOR	*valuesVol2,  		// 2nd volatility curve
	Q_VECTOR	*valuesXVol,  		// cross volatility curve
	Q_VECTOR	*timesRover,		// gridpoints for domestic rates
	Q_VECTOR    *valuesRover,		// domestic rate curve
	Q_VECTOR	*timesRunder,		// gridpoints for foreign rates
	Q_VECTOR	*timesRunder2,		// gridpoints for foreign rates
	Q_VECTOR	*valuesRunder,		// foreign rate curve
	Q_VECTOR	*valuesRunder2,		// foreign rate curve
	int			period,				// number of time units
	char		*unit,				// time unit (day, month, etc)
	double		payout,				// total payout if no fading occurs
	char	    *Ccy1,				// overlying currency
	char	    *Ccy2,				// underlying currency
	char	    *Ccy3,				// second underlying currency
	int		    UnfadedPastResets	// Unfaded past resets
	);
	
EXPORT_C_QFADE double Q_RangeBinDualQ (
	Q_VECTOR	*spot,				// current spot prices
	Q_VECTOR	*lowStrike,			// lower strikes
	Q_VECTOR	*highStrike,		// higher strikes
	DATE 		pricingDate,		// pricing date
	DATE 		startDate,			// start of fading
	DATE 		endDate,			// expiration date
	Q_MATRIX	*Vol01,				// vol curve - denom/first currency
	Q_MATRIX	*Vol02,				// vol curve - denom/second currency
	Q_MATRIX	*Vol03,				// vol curve - denom/third currency
	Q_MATRIX	*Vol04,				// vol curve - denom/fourth currency
	Q_MATRIX	*Vol12,				// vol curve - first/second currency
	Q_MATRIX	*Vol13,				// vol curve - first/third currency
	Q_MATRIX	*Vol14,				// vol curve - first/fourth currency
	Q_MATRIX	*Vol23,				// vol curve - second/third currency
	Q_MATRIX	*Vol24,				// vol curve - second/fourth currency
	Q_MATRIX	*Vol34,				// vol curve - third/fourth currency
	Q_MATRIX	*R0,				// rate curve - denom currency
	Q_MATRIX	*R1,				// rate curve - first currency
	Q_MATRIX	*R2,				// rate curve - second currency
	Q_MATRIX	*R3,				// rate curve - third currency
	Q_MATRIX	*R4,				// rate curve - fourth currency
	int			period,				// number of time units
	char		*unit,				// time unit (day, month, etc)
	double		payout,				// total payout if no fading occurs
	char	    *Ccy0,				// denom currency
	char	    *Ccy1,				// first currency
	char	    *Ccy2,				// second currency
	char	    *Ccy3,				// third currency
	char	    *Ccy4,				// fourth currency
	int		    UnfadedPastResets	// Unfaded past resets
	);

EXPORT_C_QFADE double Q_RangeBinary (
    double      	spot,
	double			lowStrike,			 // lower strike
	double			highStrike,			 // higher strike
	Q_VECTOR		*OUvaluesVol,		 // over/under volatility curve
	Q_VECTOR		*ODvaluesVol,		 // over/denom volatility curve
	Q_VECTOR		*UDvaluesVol,		 // under/denom volatility curve
	Q_VECTOR    	*SampleTimes,        // times in the future when price is read
	double      	settle,              // time from settle date to exp settle date
	double      	ccRd,                // discount rate
	Q_VECTOR    	*SampleFwds,         // forward prices at sampling points
	double			payout,				 // total payout if no fading occurs
	int		    	UnfadedPastResets,	 // Unfaded past resets
	Q_RETURN_TYPE   retType              // return premium or delta
	);

EXPORT_C_QFADE double Q_RangeMulti (
	Q_VECTOR     *Fwds,			 // forward prices
	Q_VECTOR     *Lower,		 // lower levels of ranges
	Q_VECTOR     *Upper,         // upper levels of ranges
	Q_MATRIX     *Corr,          // correlation matrix (vols on diagonal)
	double       Time,			 // time to expiration
	double       ccRd,			 // domestic interest rate
	int          R,              // number of prices required within range
	int          N,              // fineness of grid in each dimension
	double       payoff          // payoff if within range
	);

EXPORT_C_QFADE double Q_MaxMinBinMC (
	Q_OPT_TYPE      callPut,      // call means above wins
	double          spot,         // current spot price
	double          strike,       // strike on difference between max and min
	double          vol,          // volatility
	double          time,         // time to expiration
	double          ccRd,         // interest rate
	double          ccRf,         // foreign interest rate
	int             nSteps,       // number of timesteps
	int             nPaths        // number of paths in Monte Carlo run
	);

EXPORT_C_QFADE double Q_MaxMinBin (
	Q_OPT_TYPE      callPut,      // call means above wins
	double          spot,         // current spot price
	double          strike,       // strike on difference between max and min
	double          vol,          // volatility
	double          time,         // time to expiration
	double          ccRd,         // interest rate
	double          ccRf,         // foreign interest rate
	int             nSteps        // number of steps in integral
	);

EXPORT_C_QFADE double Q_RangeBinaryWtd(
    double      	spot,				 // current spot price
	double			lowStrike,			 // lower strike
	double			highStrike,			 // higher strike
	Q_VECTOR		*OUvaluesVol,		 // over/under volatility curve
	Q_VECTOR		*ODvaluesVol,		 // over/denom volatility curve
	Q_VECTOR		*UDvaluesVol,		 // under/denom volatility curve
	Q_VECTOR    	*SampleTimes,        // times in the future when price is read
	Q_VECTOR		*SampleWeights,		 // weights applied to the times 
	Q_VECTOR    	*SampleFwds,         // forward prices at sampling points
	double      	discount,            // discount factor from exp settle date to settle date
	double	    	UnfadedPastWeight,	 // weight of previous unfaded points
	double			TotalPastWeight,	 // weight of all previous points, faded and unfaded
	Q_RETURN_TYPE   retType              // return premium or delta
	);
	
					
#endif

