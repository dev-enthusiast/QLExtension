/* $Header: /home/cvs/src/supermkt/src/q_br2f_d.h,v 1.6 1999/12/15 16:33:45 goodfj Exp $ */
/****************************************************************
**
**	Q_BR2F_D.H	
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/

#if !defined( IN_Q_BR2F_D_H )
#define IN_Q_BR2F_D_H

#if !defined( IN_SECDB_H )
#include <secdb.h>
#endif
#if !defined( IN_GOB_H )
#include <gob.h>
#endif
#if !defined( IN_DATE_H )
#include <date.h>
#endif
#if !defined( IN_DATATYPE_H )
#include <datatype.h>
#endif

int ValueFuncBetaInfo1(
	GOB_VALUE_FUNC_ARGS
);

int ValueFuncBetaInfo1Phys(
	GOB_VALUE_FUNC_ARGS
);

int ValueFuncBR2FCalibrate1(
	GOB_VALUE_FUNC_ARGS
);

int ValueFuncBR2FCalibrate1Phys(
	GOB_VALUE_FUNC_ARGS
);

int ValueFuncCovAppropMethod(
	GOB_VALUE_FUNC_ARGS
);

int ValueFuncBetaAppropMethod1(
	GOB_VALUE_FUNC_ARGS
);

int ValueFuncBR2FAppropMethod1(
	GOB_VALUE_FUNC_ARGS
);

DLLEXPORT int BR2f_Calibrate1Asset2(
	DATE				t0,			// today (Pricing Date)
	double				rhoSL,			// Long Short (SL) correlation
	double				sigL,			// Long volatility
	double                          beta0,                  // Initial beta
	DT_CURVE			*mvols,			// Market implied Vols (Curve with expiration dates)
	DT_VECTOR			*dvols,			// Market implied Vols 
	DT_ARRAY			*FwdCurves,		// Array of curves containing delivery dates and forward values
	DT_VECTOR                       *Strikes,               // Vector of strikes
	char                            *ExpDateRule,           // Expiration date rule
	char                            *HolidayCal,            // Holiday Calendar
	double				sigS_min,		// minimum value for interpolated short vol
//	BR2F_FLAGS			*flags,			// BR2F model flags
	BR2F_MODEL			*br2f,			// output: model pars (covariances) from interpolation
	BR2F_BETA			*beta			// output: Beta Curve and Integral
);
    
DLLEXPORT int BR2f_Covariances1Asset_SM2(
	BR2F_MODEL			*br2f,			// model pars (covariances) from interpolation
	BR2F_BETA			*beta,		 	// Beta Curve and Integral
//	BR2F_FLAGS			*flags,			// BR2F model flags
	DT_ARRAY   			*SampleDates,		// Sample Dates
	DT_ARRAY                        *DeliveryCurves,        // Curve of sample weights by delivery date
	DATE				ExpDate,		// expiration date
	void				**RetValPtr,  	        // for returning stuff
	DT_DATA_TYPE		        RetValType		// What are we returning??
);

DLLEXPORT int BR2f_Covariances2Assets_SM2(
	BR2F_MODEL_2ASSETS		*br2f,			// model pars (covariances) from interpolation
	BR2F_BETA_2ASSETS		*beta,		 	// Beta Curve and Integral
//	BR2F_FLAGS			*flags,			// BR2F model flags
	DT_ARRAY   			*SampleDatesFirst,	// Sample Dates of first asset
	DT_ARRAY   			*SampleDatesSecond,	// Sample Dates of second asset
	DT_ARRAY                        *DeliveryCurvesFirst,   // Curve of sample weights by delivery date
	DT_ARRAY                        *DeliveryCurvesSecond,  // Curve of sample weights by delivery date
	DATE				ExpDate,		// expiration date
	void				**RetValPtr,  	        // for returning stuff
	DT_DATA_TYPE		        RetValType		// What are we returning??
);

#endif
