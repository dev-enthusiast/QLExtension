/* $Header: /home/cvs/src/quant/src/q_vol.h,v 1.21 2000/03/02 10:33:28 goodfj Exp $ */
/****************************************************************
**
**	Q_VOL.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.21 $
**
****************************************************************/

#if !defined( IN_Q_VOL_H )
#define IN_Q_VOL_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_MRTermVol(
	Q_VECTOR  	*Expirations,  // Contract expirations in years
	Q_VECTOR 	*SpotVols,	   // Forward Spot Vols.
	double		Beta,		   // Mean reversion parameter
	double		LongVol,	   // Instantaneous long term vol
	double		Correlation,   // Correlation of spot to long price
	int			Contract,	   // Contract to calculate vol for
	double		Time1,		   // Times to calculate 
	double		Time2		   // forward vol for
	);

EXPORT_C_QUANT double Q_MRTotalCovar(
	Q_VECTOR  	*Expirations,  // Contract expirations in years
	Q_VECTOR 	*SpotVols,	   // Forward Spot Vols.
	double		Beta,		   // Mean reversion parameter
	double		LongVol,	   // Instantaneous long term vol
	double		Correlation,   // Correlation of spot to long price
	int			ordinal1,	   // Contracts to calculate 
	int			ordinal2,	   // covariance for
	double		Time1,		   // interval of time for 
	double		Time2		   // forward covariance
	);

EXPORT_C_QUANT double Q_MRSpotVolNew(
	double      lastFutExp,     // expiration of last futures contract
	double      optExp,         // expiration of this options contract
	double      nextFutExp,     // expiration of this futures contract
	double   	impVol,			// implied volatility for this contract
	double      lastTermVol,    // full term volatility for previous contract
	double      lastTermCov,    // short/long term covariance for previous contract
	double		Beta,			// mean reversion coefficient
	double		LongVol,		// volatility of long-term price
	double		Correlation		// correlation of spot and long-term price
	);
	  
EXPORT_C_QUANT double Q_MRTermVolNew(
	double      lastFutExp,     // expiration of last futures contract
	double      nextFutExp,     // expiration of this futures contract
	double   	spotVol,		// spot vol between contracts
	double      lastTermVol,    // full term volatility for previous contract
	double      lastTermCov,    // short/long term covariance for previous contract
	double		Beta,			// mean reversion coefficient
	double		LongVol,		// volatility of long-term price
	double		Correlation		// correlation of spot and long-term price
	);

EXPORT_C_QUANT double Q_MRTermCovNew(
	double      lastFutExp,     // expiration of last futures contract
	double      nextFutExp,     // expiration of this futures contract
	double   	spotVol,		// spot vol between contracts
	double      lastTermCov,    // short/long term covariance for previous contract
	double		Beta,			// mean reversion coefficient
	double		LongVol,		// volatility of long-term price
	double		Correlation		// instantaneous correlation of spot and long-term price
	);

EXPORT_C_QUANT double Q_MRFutCov (
	Q_VECTOR        *futexp,    // times to expirations of futures
	Q_VECTOR        *spotvol,   // spot volatilities between futures dates
	Q_VECTOR        *termvol,   // term volatilities to expirations
	Q_VECTOR        *termcov,   // term cov(spot,long) to expirations
	int             n1,         // first contract
	int             n2,         // second contract
	double          time,       // term of this covariance
	double			Beta,		// mean reversion coefficient
	double			LongVol,	// volatility of long-term price
	double			Correlation	// instantaneous correlation of spot and long-term price
	);

EXPORT_C_QUANT double Q_MRFutCov2 (
	Q_VECTOR        *futexp,    // times to expirations of futures
	Q_VECTOR        *spotvol,   // spot volatilities between futures dates
	Q_VECTOR        *termvol,   // term volatilities to expirations
	Q_VECTOR        *termcov,   // term cov(spot,long) to expirations
	double          t1,         // expiration of first futures price
	double          t2,         // expiration of second futures price
	double          time,       // term of this covariance
	double			Beta,		// mean reversion coefficient
	double			LongVol,	// volatility of long-term price
	double			Correlation	// instantaneous correlation of spot and long-term price
	);

EXPORT_C_QUANT double Q_MRSpotLongCov (
	Q_VECTOR        *futexp,    // times to expirations of futures
	Q_VECTOR        *spotvol,   // spot volatilities between futures dates
	Q_VECTOR        *termvol,   // term volatilities to expirations
	Q_VECTOR        *termcov,   // term cov(spot,long) to expirations
	double          time,       // term of this cov(spot,long)
	double			Beta,		// mean reversion coefficient
	double			LongVol,	// volatility of long-term price
	double			Correlation	// instantaneous correlation of spot and long-term price
	);

EXPORT_C_QUANT double Q_MRFutCov3F (  
	Q_VECTOR        *futexp,    // times to expirations of futures
	Q_VECTOR        *vol1,      // vector of volatilities for factor 1
	Q_VECTOR        *vol2,      // vector of volatilities for factor 2
	double          t1,         // expiration of first futures price
	double          t2,         // expiration of second futures price
	double          time,       // term of this covariance
	double          beta1,      // mean reversion of factor 1
	double          beta2,      // mean reversion of factor 2
	double          longvol,    // long volatility
	double          rho,        // correlation of factor 1 and factor 2
	double          rho1,       // correlation of factor 1 and long
	double          rho2        // correlation of factor 2 and long
	);

EXPORT_C_QUANT double Q_MRFutCov2Dim (
	Q_VECTOR        *futexp1,   // times to expirations of futures 1
	Q_VECTOR        *futexp2,   // times to expirations of futures 2
	Q_VECTOR        *spotvol1,  // spot volatilities between futures dates 1
	Q_VECTOR        *spotvol2,  // spot volatilities between futures dates 2
	double          t1,         // expiration of first futures price
	double          t2,         // expiration of second futures price
	double          time,       // term of this covariance
	double			Beta1, 		// mean reversion coefficient 1
	double			Beta2, 		// mean reversion coefficient 2
	double			LongVol1,	// volatility of long-term price 1
	double			LongVol2,	// volatility of long-term price 2
	double			CorrSS,		// correlation of spot 1 and spot 2
	double			CorrSL,		// correlation of spot 1 and long 2
	double			CorrLS,     // correlation of long 1 and spot 2
	double			CorrLL      // correlation of long 1 and long 2
	);

EXPORT_C_QUANT double Q_MRFutCov2Dim2 (
	Q_VECTOR        *futexp1,   // times to expirations of futures 1
	Q_VECTOR        *futexp2,   // times to expirations of futures 2
	Q_VECTOR        *spotvol1,  // spot volatilities between futures dates 1
	Q_VECTOR        *spotvol2,  // spot volatilities between futures dates 2
	double          t1,         // expiration of first futures price
	double          t2,         // expiration of second futures price
	double          time,       // term of this covariance
	double			Beta1, 		// mean reversion coefficient 1
	double			Beta2, 		// mean reversion coefficient 2
	double			LongVol1,	// volatility of long-term price 1
	double			LongVol2,	// volatility of long-term price 2
	Q_VECTOR		*CorrSS,    // correlation of spot 1 and spot 2
	Q_VECTOR        *CorrT,     // grid for correlation vector
	double			CorrSL,		// correlation of spot 1 and long 2
	double			CorrLS,     // correlation of long 1 and spot 2
	double			CorrLL      // correlation of long 1 and long 2
	);

EXPORT_C_QUANT double Q_MRFutCov2Dim2B (
	Q_VECTOR        *futexp1,   // times to expirations of futures 1
	Q_VECTOR        *futexp2,   // times to expirations of futures 2
	Q_VECTOR        *spotvol1,  // spot volatilities between futures dates 1
	Q_VECTOR        *spotvol2,  // spot volatilities between futures dates 2
	double          t1,         // expiration of first futures price
	double          t2,         // expiration of second futures price
	double          time,       // term of this covariance
	Q_VECTOR		*Beta1, 	// mean reversion coefficient 1
	Q_VECTOR		*Beta2, 	// mean reversion coefficient 2
	double			LongVol1,	// volatility of long-term price 1
	double			LongVol2,	// volatility of long-term price 2
	Q_VECTOR		*CorrSS,    // correlation of spot 1 and spot 2
	Q_VECTOR        *CorrT,     // grid for correlation vector
	double			CorrSL,		// correlation of spot 1 and long 2
	double			CorrLS,     // correlation of long 1 and spot 2
	double			CorrLL      // correlation of long 1 and long 2
	);

#endif
