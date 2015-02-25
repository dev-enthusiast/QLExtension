/****************************************************************
**
**	q_fit.h	- header file for q_fit.c
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.18 $
**
****************************************************************/

#if !defined( IN_Q_FIT_H )
#define IN_Q_FIT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#include <num.h>


typedef struct {
	double		spot;
	double		initVol;
	double		meanVol;
	double		initRho;
	double		meanRho;
	double		beta;
	double		sigma;
	double		gamma;
	double		eps;
	double		rhoCS;
	double		rhoCV;
	double		rhoMin;
	double		rhoMax;
	double		phi;
	DT_CURVE	*dDiscCurve;
	DT_CURVE	*fDiscCurve;
	Q_VECTOR	*fwds;
	Q_VECTOR	*fwdT;
	int			nPaths;
	int			nSteps;
	int			seed;
	int			useLNMR;
} Q_HSC_PARAMS;


typedef struct {
	Q_OPT_TYPE	callPut;
	int			CVtype;
	double		ATMstrike;
	double		LowStrike;
	double		HighStrike;
	double		Term;
	double		ccRd;
	double		ccRf;
	double		effRho;
	double		effVol;
	DATE		prcDate;
	DATE		expDate;
	Q_VECTOR	*times;
} Q_OPT_PARAMS;


typedef struct {
	Q_MATRIX	*spotPaths;		// Regular Monte Carlo spot paths	
	Q_MATRIX	*dSpotPaths;	// Degenerate Monte Carlo spot paths	
	Q_MATRIX	*hSpotPaths;	// Heston's Monte Carlo spot paths	
} Q_MC_PATHS;


typedef struct {
	double		Premium;
	double		StdError;
	double		RawPremium;
	double		RawStdError;
	double		CtrlVariate;
	double		Benchmark;
	double		CorrCoeff;
	double		BetaTotal;
} Q_CV_OUTPUT;


typedef struct {
	double		ST_ATM;
	double		ST_RR;
	double		ST_BF;
	double		LT_ATM;
	double		LT_RR;
	double		LT_BF;
} Q_TARGET;


DLLEXPORT int Q_Fit(
	int				FitBeta,		// TRUE if fit his parameter, FALSE otherwise
	int				FitSigma,		// TRUE if fit his parameter, FALSE otherwise
	int				FitInitVol,		// TRUE if fit his parameter, FALSE otherwise
	int				FitMeanVol,		// TRUE if fit his parameter, FALSE otherwise
	int				FitInitRho,		// TRUE if fit his parameter, FALSE otherwise
	int				FitMeanRho,		// TRUE if fit his parameter, FALSE otherwise
    double			ShortTermRR,	// Target value: short-dated risk reversal
	double			ShortTermBF,	// Target value: short-dated butterfly
	double			ShortTermATM,	// Target value: short-dated ATM volatility
	double          ATMstrikeST,	// Short-dated ATM strike
	double          LowStrikeST,	// Short-dated low strike
	double          HighStrikeST,	// Short-dated high strike
	double			LongTermRR,		// Target value: long-dated risk reversal
	double			LongTermBF,		// Target value: long-dated butterfly
	double			LongTermATM,	// Target value: long-dated ATM volatility
	double          ATMstrikeLT,	// Long-dated ATM strike
	double          LowStrikeLT,	// Long-dated low strike
	double          HighStrikeLT,	// Long-dated high strike
	double          initVol,		// Instantaneous volatility
	double			meanVol,		// Forward-forward reverting vols
	double          initRho,		// Initial spot/vol correlation
	double        	meanRho,		// Mean level of spot/vol correlation
	double        	beta,			// Mean-reversion strength
	double        	sigma,			// Volatility of volatility
	double        	gamma, 			// Mean-reversion strength of Z
	double        	eps,			// Volatility of Z
	double			rhoCS,			// Correlation between Z and spot
	double			rhoCV,			// Correlation between Z and stoch vol
	double          spot,			// Underlying spot price
	DT_CURVE		*dDiscCurve,	// Domestic discount curve
    DT_CURVE		*fDiscCurve,	// Foreign discount curve
    DT_CURVE		*fwdCurve,		// Forward curve
	DATE			prcDate,		// Pricing date
	DATE			expDateST,		// Short-dated option expiration date
	DATE			expDateLT,		// Long-dated option expiration date
	int             nPaths,			// Number of Monte Carlo paths
	int             nSteps,			// Number of time steps on a Monte Carlo path
	int             maxIter,		// Maximum number of fitting iterations
	int				CVtype,			// Control variate index
	int             seed,			// Starting seed for random numbers generator
	int				useLNMR,		// TRUE if use LNMR-based model, FALSE otherwise 
	double			*finalInitRho,	// Returned final value of initRho
	double			*finalMeanRho,	// Returned final value of meanRho
	double			*finalInitVol,	// Returned final value of initVol
	double			*finalMeanVol,	// Returned final value of meanVol
	double			*finalBeta,		// Returned final value of Beta
	double			*finalSigma		// Returned final value of Sigma
	);

DLLEXPORT int Q_CalculateEffVol(
	Q_HSC_PARAMS	*mp,			// Model parameters structure
	Q_OPT_PARAMS	*op				// Option parameters structure
	);


DLLEXPORT int Q_CalculateEffRho(
	Q_HSC_PARAMS	*mp,			// Model parameters structure
	Q_OPT_PARAMS	*op				// Option parameters structure
	);

DLLEXPORT int Q_FitOneFactor(
	int				TargetType,		// Target type ( ATM vol, RR or BF )
	double			TargetValue,	// Target value
	double			*DiddleParam,	// Pointer to the parameter to diddle
	double			delta,			// Diddle step size
	int				AdjEffVol,		// TRUE if adjust effVol, FALSE otherwise
	int				AdjEffRho,		// TRUE if adjust effRho, FALSE otherwise
	Q_HSC_PARAMS	*ModelParams,	// Pointer to the model parameters structure
	Q_OPT_PARAMS	*OptionParams,	// Pointer to the option parameters structure
	Q_MC_PATHS		*MCpaths		// Pointer to the Monte Carlo paths structure
	);

DLLEXPORT int Q_LagrangePolynomial(
	double			x[ 3 ],			// Array of X values
	double			y[ 3 ],			// Array of Y values
	double			*a,				// Square term coefficient 
	double			*b,				// Linear term coefficient
	double			*c				// Constant
	);

DLLEXPORT int Q_WingSCorrMC_SL (
	double          spot,			// Underlying spot price
	double          initVol,		// Instantaneous volatility
	double			meanVol,		// Forward-forward reverting vols
	double			effVol,			// Effective constant volatility
	double          initRho,		// Initial spot/vol correlation
	double        	meanRho,		// Mean level of spot/vol correlation
	double			effRho,			// Effective spot/vol correlation
	double        	beta,			// Mean-reversion strength
	double        	sigma,			// Volatility of volatility
	double        	gamma, 			// Mean-reversion strength of Z
	double        	eps,			// Volatility of Z
	double			rhoCS,			// Correlation between Z and spot
	double			rhoCV,			// Correlation between Z and stoch vol
	Q_VECTOR        *fwds,			// Forwards vector
	Q_VECTOR        *fwdT,			// Gridpoints for forwards vector
	Q_VECTOR        *times,			// Gridpoints for Monte Carlo paths
	int             nPaths,			// Number of Monte Carlo paths
	int             seed,			// Starting seed for random numbers gen.
	Q_MATRIX        *spotPaths,		// Returned matrix containing spot price paths
	Q_MATRIX        *dSpotPaths,	// Returned matrix containing fully degenerate spot paths
	Q_MATRIX        *cSpotPaths		// Returned matrix containing spot paths with SV and jumps only
	);

DLLEXPORT int Q_WingLNSCorrMC_SL (
	double          spot,			// Underlying spot price
	double          initVol,		// Instantaneous volatility
	double			meanVol,		// Forward-forward reverting vols
	double			effVol,			// Effective constant volatility
	double          initRho,		// Initial spot/vol correlation
	double        	meanRho,		// Mean level of spot/vol correlation
	double			effRho,			// Effective spot/vol correlation
	double        	beta,			// Mean-reversion strength
	double        	sigma,			// Volatility of volatility
	double        	gamma, 			// Mean-reversion strength of Z
	double        	eps,			// Volatility of Z
	double			rhoCS,			// Correlation between Z and spot
	double			rhoCV,			// Correlation between Z and stoch vol
	Q_VECTOR        *fwds,			// Forwards vector
	Q_VECTOR        *fwdT,			// Gridpoints for forwards vector
	Q_VECTOR        *times,			// Gridpoints for Monte Carlo paths
	int             nPaths,			// Number of Monte Carlo paths
	int             seed,			// Starting seed for random numbers gen.
	Q_MATRIX        *spotPaths,		// Returned matrix containing spot price paths
	Q_MATRIX        *dSpotPaths,	// Returned matrix containing fully degenerate spot paths
	Q_MATRIX        *cSpotPaths		// Returned matrix containing spot paths with SV and jumps only
	);

DLLEXPORT double Q_FitRunWingMC(
	int				TargetType,		// Target type ( ATM vol, RR or BF )
	Q_HSC_PARAMS	*ModelParams,	// Pointer to the model parameters structure
	Q_OPT_PARAMS	*OptionParams,	// Pointer to the option parameters structure
	Q_MC_PATHS		*MCpaths		// Pointer to the Monte Carlo paths structure
	);

DLLEXPORT double Q_FitRunLNWingMC(
	int				TargetType,		// Target type ( ATM vol, RR or BF )
	Q_HSC_PARAMS	*ModelParams,	// Pointer to the model parameters structure
	Q_OPT_PARAMS	*OptionParams,	// Pointer to the option parameters structure
	Q_MC_PATHS		*MCpaths		// Pointer to the Monte Carlo paths structure
	);

DLLEXPORT double Q_FitRunPhiWingMC(
	int				TargetType,		// Target type ( ATM vol, RR or BF )
	Q_HSC_PARAMS	*ModelParams,	// Pointer to the model parameters structure
	Q_OPT_PARAMS	*OptionParams,	// Pointer to the option parameters structure
	Q_MC_PATHS		*MCpaths		// Pointer to the Monte Carlo paths structure
	);

DLLEXPORT double Q_FitCheckWingMC(
	int				TargetType,		// Target type ( ATM vol, RR or BF )
	Q_HSC_PARAMS	*ModelParams,	// Pointer to the model parameters structure
	Q_OPT_PARAMS	*OptionParams	// Pointer to the option parameters structure
	);

DLLEXPORT int Q_WingLNSCorrMC(
	double          spot,			// Underlying spot price
	double          initVol,		// Instantaneous volatility
	double			meanVol,		// Forward-forward reverting vols
	double			effVol,			// Effective constant volatility
	double          initRho,		// Initial spot/vol correlation
	double        	meanRho,		// Mean level of spot/vol correlation
	double			effRho,			// Effective spot/vol correlation
	double        	beta,			// Mean-reversion strength
	double        	sigma,			// Volatility of volatility
	double        	gamma, 			// Mean-reversion strength of Z
	double        	eps,			// Volatility of Z
	double			rhoCS,			// Correlation between Z and spot
	double			rhoCV,			// Correlation between Z and stoch vol
	Q_VECTOR        *fwds,			// Forwards vector
	Q_VECTOR        *fwdT,			// Gridpoints for forwards vector
	Q_VECTOR        *times,			// Gridpoints for Monte Carlo paths
	int             nPaths,			// Number of Monte Carlo paths
	int             seed,			// Starting seed for random numbers gen.
	Q_MATRIX        *spotPaths,		// Returned matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Returned matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Returned matrix containing fully degenerate spot paths
	Q_MATRIX        *dVolPaths,		// Returned matrix containing fully degenerate vol paths
	Q_MATRIX        *cSpotPaths,	// Returned matrix containing Heston's spot paths
	Q_MATRIX        *cVolPaths		// Returned matrix containing Heston's vol paths
	);
	
DLLEXPORT int Q_PDRunWingMC(
	Q_HSC_PARAMS	*ModelParams,	// Pointer to the model parameters structure
	Q_OPT_PARAMS	*OptionParams,	// Pointer to the option parameters structure
	Q_MC_PATHS		*MCpaths,		// Pointer to the Monte Carlo paths structure
	double			*ATM,			// Returned value of the ATM volatility
	double			*RR,			// Returned value of the risk reversal
	double			*BF				// Returned value of the butterfly
	);
	
DLLEXPORT int Q_PDRunLNWingMC(
	Q_HSC_PARAMS	*ModelParams,	// Pointer to the model parameters structure
	Q_OPT_PARAMS	*OptionParams,	// Pointer to the option parameters structure
	Q_MC_PATHS		*MCpaths,		// Pointer to the Monte Carlo paths structure
	double			*ATM,			// Returned value of the ATM volatility
	double			*RR,			// Returned value of the risk reversal
	double			*BF				// Returned value of the butterfly
	);
	
DLLEXPORT int Q_GetPDOneFactor(
	double			*DiddleParam,		// Pointer to the parameter to diddle
	double			delta,				// Diddle step size
	int				AdjEffVol,			// TRUE if adjust effVol, FALSE otherwise
	int				AdjEffRho,			// TRUE if adjust effRho, FALSE otherwise
	Q_HSC_PARAMS	*ModelParams,		// Pointer to the model parameters structure
	Q_OPT_PARAMS	*OptionParamsST,	// Pointer to the ST option parameters structure
	Q_OPT_PARAMS	*OptionParamsLT,	// Pointer to the LT option parameters structure
	Q_MC_PATHS		*MCpaths,			// Pointer to the Monte Carlo paths structure
	Q_TARGET		*Center,			// Pointer to the structure with results from central point
	double			*PD					// Pointer to where to record the calculated values
	);

DLLEXPORT int Q_PartialDerivatives(
	int				InitVolPD,		// TRUE if get PD w.r.t. InitVol, FALSE otherwise
	int				MeanVolPD,		// TRUE if get PD w.r.t. MeanVol, FALSE otherwise
	int				InitRhoPD,		// TRUE if get PD w.r.t. InitRho, FALSE otherwise
	int				MeanRhoPD,		// TRUE if get PD w.r.t. MeanRho, FALSE otherwise
	int				BetaPD,			// TRUE if get PD w.r.t. Beta,    FALSE otherwise
	int				SigmaPD,		// TRUE if get PD w.r.t. Sigma,   FALSE otherwise
	int				GammaPD,		// TRUE if get PD w.r.t. Gamma,   FALSE otherwise
	int				EpsPD,			// TRUE if get PD w.r.t. Eps,     FALSE otherwise
	int				RhoCSPD,		// TRUE if get PD w.r.t. RhoCS,   FALSE otherwise
	int				RhoCVPD,		// TRUE if get PD w.r.t. RhoCV,   FALSE otherwise
	double          ATMstrikeST,	// Short-dated ATM strike
	double          LowStrikeST,	// Short-dated low strike
	double          HighStrikeST,	// Short-dated high strike
	double          ATMstrikeLT,	// Long-dated ATM strike
	double          LowStrikeLT,	// Long-dated low strike
	double          HighStrikeLT,	// Long-dated high strike
	double          initVol,		// Instantaneous volatility
	double			meanVol,		// Forward-forward reverting vols
	double          initRho,		// Initial spot/vol correlation
	double        	meanRho,		// Mean level of spot/vol correlation
	double        	beta,			// Mean-reversion strength
	double        	sigma,			// Volatility of volatility
	double        	gamma, 			// Mean-reversion strength of Z
	double        	eps,			// Volatility of Z
	double			rhoCS,			// Correlation between Z and spot
	double			rhoCV,			// Correlation between Z and stoch vol
	double          spot,			// Underlying spot price
	DT_CURVE		*dDiscCurve,	// Domestic discount curve
    DT_CURVE		*fDiscCurve,	// Foreign discount curve
    DT_CURVE		*fwdCurve,		// Forward curve
	DATE			prcDate,		// Pricing date
	DATE			expDateST,		// Short-dated option expiration date
	DATE			expDateLT,		// Long-dated option expiration date
	int             nPaths,			// Number of Monte Carlo paths
	int             nSteps,			// Number of time steps on a Monte Carlo path
	int             maxIter,		// Maximum number of fitting iterations
	int				CVtype,			// Control variate index
	int             seed,			// Starting seed for random numbers generator
	int				useLNMR,		// TRUE if use LNMR-based model, FALSE otherwise
	Q_MATRIX        *PD				// Returned matrix containing partial derivatives wrt. model params
	);

DLLEXPORT int Q_PhiWingMC (
	double          spot,			// Underlying spot price
	double          initVol,		// Instantaneous volatility
	double			meanVol,		// Forward-forward reverting vols
	double			effVol,			// Effective constant volatility
	double          initRho,		// Initial spot/vol correlation
	double        	meanRho,		// Mean level of spot/vol correlation
	double			effRho,			// Effective spot/vol correlation
	double        	beta,			// Mean-reversion strength
	double        	sigma,			// Volatility of volatility
	double        	gamma, 			// Mean-reversion strength of Z
	double        	eps,			// Volatility of Z
	double			rhoCS,			// Correlation between Z and spot
	double			rhoCV,			// Correlation between Z and stoch vol
	double			phi,			// Exponent in the stoch. vol term
	Q_VECTOR        *fwds,			// Forwards vector
	Q_VECTOR        *fwdT,			// Gridpoints for forwards vector
	Q_VECTOR        *times,			// Gridpoints for Monte Carlo paths
	int             nPaths,			// Number of Monte Carlo paths
	int             seed,			// Starting seed for random numbers gen.
	Q_MATRIX        *spotPaths,		// Returned matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Returned matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Returned matrix containing fully degenerate spot paths
	Q_MATRIX        *dVolPaths,		// Returned matrix containing fully degenerate vol paths
	Q_MATRIX        *cSpotPaths,	// Returned matrix containing Heston's spot paths
	Q_MATRIX        *cVolPaths		// Returned matrix containing Heston's vol paths
	);

DLLEXPORT int Q_PhiWingMC_SL (
	double          spot,			// Underlying spot price
	double          initVol,		// Instantaneous volatility
	double			meanVol,		// Forward-forward reverting vols
	double			effVol,			// Effective constant volatility
	double          initRho,		// Initial spot/vol correlation
	double        	meanRho,		// Mean level of spot/vol correlation
	double			effRho,			// Effective spot/vol correlation
	double        	beta,			// Mean-reversion strength
	double        	sigma,			// Volatility of volatility
	double        	gamma, 			// Mean-reversion strength of Z
	double        	eps,			// Volatility of Z
	double			rhoCS,			// Correlation between Z and spot
	double			rhoCV,			// Correlation between Z and stoch vol
	double			phi,			// Exponent in the stoch. vol term
	Q_VECTOR        *fwds,			// Forwards vector
	Q_VECTOR        *fwdT,			// Gridpoints for forwards vector
	Q_VECTOR        *times,			// Gridpoints for Monte Carlo paths
	int             nPaths,			// Number of Monte Carlo paths
	int             seed,			// Starting seed for random numbers gen.
	Q_MATRIX        *spotPaths,		// Returned matrix containing spot price paths
	Q_MATRIX        *dSpotPaths,	// Returned matrix containing fully degenerate spot paths
	Q_MATRIX        *cSpotPaths		// Returned matrix containing Heston's spot paths
	);
				   
#endif

