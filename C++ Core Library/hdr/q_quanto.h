/* $Header: /home/cvs/src/quant/src/q_quanto.h,v 1.11 2000/03/23 13:48:20 goodfj Exp $ */
/****************************************************************
**
**	Q_QUANTO.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.11 $
**
****************************************************************/

#if !defined( IN_Q_QUANTO_H )
#define IN_Q_QUANTO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_QtosOpt (
	Q_OPT_TYPE		callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE		style,				// Option style (Q_OPT_EURO or Q_OPT_AMER)
	double			Spot,				// Spot of Over/Under (<0 => American)
	double			Strike,				// Option strike price in Over/Under (<0 => Put)
	double			OverUnderVol,		// Cross volatility of Over/Under
	double			OverDenomVol,	   	// Cross volatility of Over/Denom
	double			UnderDenomVol,   	// Cross volatility of Under/Denom
	double			timeToExp,			// Time to option expiration, actual 365
	double			OverRate,			// Interest rate of Over currency 
	double			UnderRate,			// Interest rate of Under currency
	double			DenomRate,			// Interest rate of Denom currency
	Q_RETURN_TYPE	retType             // return premium or delta
	);

EXPORT_C_QKNOCK void Q_QuantosBinary(
	Q_OPT_TYPE	OptionType,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double		Spot,				// Spot of Over/Under
	double		Strike,				// Option strike price in Over/Under
	double		timeToExp,			// Time to option expiration, actual 365
	double		timeToSettle,		// Time to settlement of underlying 
	double		OverRate,			// Interest rate of Over currency (continuous)
	double		UnderRate,			// Interest rate of Under currency (continuous)
	double		DenomRate,			// Interest rate of Denom currency (continuous)
	double		OverUnderVol,		// Cross volatility of Over/Under
	double		OverDenomVol,	   	// Cross volatility of Over/Denom
	double		UnderDenomVol,	   	// Cross volatility of Under/Denom
	double		*Premium,			// Returned option premium in Denom/Denom
	double		*Delta				// Returned option delta
	);

EXPORT_C_QKNOCK void Q_QuantosBinaryKO(
	Q_OPT_TYPE		OptionType,	   // Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown,      // direction of knockout
	Q_ALIVE_DEAD    knockedOut,    // Already knocked out or not
	Q_IN_OUT     	inOrOut,       // knockin or knockout
	double			Spot,		   // Spot of Over/Under
	double			Strike,		   // Option strike price in Over/Under
	double  		outVal,        // knockout level in over/under
	double			timeToExp,	   // Time to option expiration, actual 365
	double			timeToSettle,  // Time to settlement of underlying
	double			OverRate,	   // Interest rate of Over currency (continuous)
	double			UnderRate,	   // Interest rate of Under currency (continuous)
	double			DenomRate,	   // Interest rate of Denom currency (continuous)
	double			OverUnderVol,  // Cross volatility of Over/Under
	double			OverDenomVol,  // Cross volatility of Over/Denom
	double			UnderDenomVol, // Cross volatility of Under/Denom
	double  		payoff,		   // payoff for winning
	double			*Premium,	   // Returned option premium in Denom/Denom
	double			*Delta		   // Returned option delta
	);

EXPORT_C_QKNOCK double Q_QtosBinaryDKO(
	Q_OPT_TYPE		OptionType,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_ALIVE_DEAD	knockedOut,         // Already knocked out or not
	Q_IN_OUT     	inOrOut,            // knockin or knockout   
	double			Spot,				// Spot of Over/Under
	double			Strike,				// Option strike price in Over/Under
	double  		lowknockout,        // lower knockout level 
	double  		highknockout,       // upper knockout level
	double			timeToExp,			// Time to option expiration, actual 365
	double			timeToSettle,		// Time to settlement of underlying 
	double			OverRate,			// Interest rate of Over currency (continuous)
	double			UnderRate,			// Interest rate of Under currency (continuous)
	double			DenomRate,			// Interest rate of Denom currency (continuous)
	double			OverUnderVol,		// Cross volatility of Over/Under
	double			OverDenomVol,	   	// Cross volatility of Over/Denom
	double			UnderDenomVol,	   	// Cross volatility of Under/Denom
	double  		payoff,				// payoff for winning
	Q_RETURN_TYPE	retVal				// Q_PREMIUM or Q_DELTA	
);

EXPORT_C_QKNOCK void Q_QuantosBinaryDKO(
	Q_OPT_TYPE		OptionType,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_ALIVE_DEAD    knockedOut,         // Already knocked out or not
	Q_IN_OUT     	inOrOut,            // knockin or knockout   
	double			Spot,				// Spot of Over/Under
	double			Strike,				// Option strike price in Over/Under
	double  		lowknockout,        // lower knockout level 
	double  		highknockout,       // upper knockout level
	double			timeToExp,			// Time to option expiration, actual 365
	double			timeToSettle,		// Time to settlement of underlying 
	double			OverRate,			// Interest rate of Over currency (continuous)
	double			UnderRate,			// Interest rate of Under currency (continuous)
	double			DenomRate,			// Interest rate of Denom currency (continuous)
	double			OverUnderVol,		// Cross volatility of Over/Under
	double			OverDenomVol,	   	// Cross volatility of Over/Denom
	double			UnderDenomVol,	   	// Cross volatility of Under/Denom
	double  		payoff,				// payoff for winning
	double			*Premium,			// Returned option premium in Denom/Denom
	double			*Delta				// Returned option delta
	);
		
EXPORT_C_QKNOCK int Q_QtosFwd(
	double	Spot,				// Spot of Over/Under
	double  Strike,             // strike price of forward contract
	double	TimeToExpiration,	// Number of days to expiration in Actual/365
	double  timeToSettle,       // time between premium settlement and payoff settlement
	double	OverRate,			// Interest rate of Over currency (continuous)
	double	UnderRate,			// Interest rate of Under currency (continuous)
	double	DenomRate,			// Interest rate of Denom currency (continuous)
	double	OverUnderVol,		// Cross volatility of Over/Under
	double	OverDenomVol,	   	// Cross volatility of Over/Denom
	double	UnderDenomVol,	   	// Cross volatility of Under/Denom
	double  *premRet,           // returned premium
	double  *deltaRet           // returned delta
	);

EXPORT_C_QKNOCK int Q_QuantosAdjust ( 
	DATE              prcDate,   // pricing date
	DT_CURVE          *fwds,     // over/under forward curve
	DT_CURVE          *OUVol,    // over/under volatility curve
	DT_CURVE          *ODVol,    // over/denom volatility curve
	DT_CURVE          *UDVol,    // under/denom volatility curve
	DT_CURVE          *adjfwds   // returned quantos adjusted forward curve
	);
			
EXPORT_C_QKNOCK void Q_Quantos(
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE		euroAmer,		// European or american style
	Q_OPT_ON		underType,		// Option on spot or futures
	Q_MARGIN		markToMkt,		// Daily mark to market or not
	double			Spot,			// Spot of Over/Under
	double			Strike,			// Option strike price in Over/Under
	double			timeToExp,		// Time to option expiration, actual 365
	double			timeToSettle,	// Time to settlement of underlying
	double			OverRate,		// Interest rate of Over currency (continuous)
	double			UnderRate,		// Interest rate of Under currency (continuous)
	double			DenomRate,		// Interest rate of Denom currency (continuous)
	double			OverUnderVol,	// Cross volatility of Over/Under
	double			OverDenomVol,	// Cross volatility of Over/Denom
	double			UnderDenomVol,	// Cross volatility of Under/Denom
	double			*Premium,		// Returned option premium in Denom
	double			*Delta			// Returned option delta
);

EXPORT_C_QKNOCK double Q_CalcFwdQuantos(
	double	Spot,				// Spot of Over/Under
	double	TimeToExpiration,	// Number of days to expiration in Actual/365
	double	OverRate,			// Interest rate of Over currency (continuous)
	double	UnderRate,			// Interest rate of Under currency (continuous)
	double	DenomRate,			// Interest rate of Denom currency (continuous)
	double	OverUnderVol,		// Cross volatility of Over/Under
	double	OverDenomVol,	   	// Cross volatility of Over/Denom
	double	UnderDenomVol	   	// Cross volatility of Under/Denom
);

EXPORT_C_QKNOCK void Q_QuantosKO(
	Q_OPT_TYPE		OptionType,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown,           // direction of knockout
	Q_ALIVE_DEAD    knockedOut,         // Already knocked out or not
	Q_IN_OUT     	inOrOut,            // knockin or knockout
	double			Spot,				// Spot of Over/Under
	double			Strike,				// Option strike price in Over/Under
	double  		outVal,             // knockout level in over/under
	double  		rebate,             // rebate upon knockout
	Q_REBATE_TYPE   rebType,            // delayed or immediate rebate
	double			timeToExp,			// Time to option expiration, actual 365
	double			timeToSettle,		// Time to settlement of underlying
	double			OverRate,			// Interest rate of Over currency (continuous)
	double			UnderRate,			// Interest rate of Under currency (continuous)
	double			DenomRate,			// Interest rate of Denom currency (continuous)
	double			OverUnderVol,		// Cross volatility of Over/Under
	double			OverDenomVol,	   	// Cross volatility of Over/Denom
	double			UnderDenomVol,	   	// Cross volatility of Under/Denom
	double			*Premium,			// Returned option premium in Denom/Denom
	double			*Delta				// Returned option delta
	);

#endif
