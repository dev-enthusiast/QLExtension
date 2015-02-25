/* $Header: /home/cvs/src/stvol/src/stvol.h,v 1.38 2001/11/27 23:30:22 procmon Exp $ */
/****************************************************************
**
**	STVOL.H
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.38 $
**
****************************************************************/

#include <mktcalc.h>
#include <num.h>

#if !defined( IN_STVOL_H )
#define IN_STVOL_H

#ifndef IN_QTYPES_H
#include <qtypes.h>
#endif

typedef struct StVolOptParmsStruct
{
	double	Spot,
			Strike,
			ccRd,
			ccRf,
			LogFwd,
			Texp,
			InstVar,
			LongVar,
			VVar,
			Beta,
			Corr;
}
STVOL_OPTION_PARMS;

typedef struct StVolOptParmsFuzzyStruct
{
	double	Spot,
			Strike,
			ccRd,
			ccRf,
			LogFwd,
			Texp,
			InstVar,
			LongVar,
			VVar,
			Beta,
			Corr,
			stdev;
}
STVOL_OPTION_PARMS_FUZZY;

typedef struct StVolOptParmsGenStruct
{
	double  LogFwd,
			Strike,
			ccRd,
			Texp,
			InstVar,
			stdev;
			
	Q_VECTOR		
			*LongVar,
			*VVar,
			*Beta,
			*Corr,
			*Times;
}
STVOL_OPTION_PARMS_GEN;

typedef struct JDOptParmsStruct
{
	double  LogFwd,
			Strike,
			ccRd,
			Texp;

	Q_VECTOR		
			*lvol,
			*lvolT,
			*lambda,
			*jumpMean,
			*jumpVar;
}
JD_OPTION_PARMS;

int LogCharFnImag(			double Phi,	void *Context, double *Result );
int LogCharFnReal(			double Phi,	void *Context, double *Result );
int LogCharFnModImag(		double Phi,	void *Context, double *Result );
int LogCharFnModReal(		double Phi,	void *Context, double *Result );

int LogCharFnFuzzyImag(		double Phi,	void *Context, double *Result );
int LogCharFnFuzzyReal(		double Phi,	void *Context, double *Result );
int LogCharFnModFuzzyImag(	double Phi,	void *Context, double *Result );
int LogCharFnModFuzzyReal(	double Phi,	void *Context, double *Result );

int LogCharFnImagGen(		double Phi,	void *Context, double *Result );
int LogCharFnRealGen(		double Phi,	void *Context, double *Result );
int LogCharFnGenModImag(	double Phi,	void *Context, double *Result );
int LogCharFnGenModReal(	double Phi,	void *Context, double *Result );

int JDLogCharFnImag(		double Phi,	void *Context, double *Result );
int JDLogCharFnReal(		double Phi,	void *Context, double *Result );
int JDLogCharFnModImag(		double Phi,	void *Context, double *Result );
int JDLogCharFnModReal(		double Phi,	void *Context, double *Result );

DLLEXPORT int StVolDensityKernel(		double Phi,	void *Context, double *Result );
DLLEXPORT int StVolDensityKernelMod(	double Phi,	void *Context, double *Result );
DLLEXPORT int StVolOptionKernel(		double Phi,	void *Context, double *Result );
DLLEXPORT int StVolOptionKernelMod(		double Phi,	void *Context, double *Result );
DLLEXPORT int StVolOptionKernelFuzzy(	double Phi,	void *Context, double *Result );
DLLEXPORT int StVolOptionKernelModFuzzy(double Phi,	void *Context, double *Result );

DLLEXPORT int StVolLogCharFn(		double Phi,	STVOL_OPTION_PARMS *Context, DT_COMPLEX *Result );
DLLEXPORT int StVolLogCharFnMod(	double Phi,	STVOL_OPTION_PARMS *Context, DT_COMPLEX *Result );
DLLEXPORT int StVolCharFn(			double Phi,	STVOL_OPTION_PARMS *Context, DT_COMPLEX *Result );
DLLEXPORT int StVolCharFnMod(		double Phi,	STVOL_OPTION_PARMS *Context, DT_COMPLEX *Result );

DLLEXPORT int StVolProbDensity(		STVOL_OPTION_PARMS *Context, double *Answer );
DLLEXPORT int StVolProbDensityMod(	STVOL_OPTION_PARMS *Context, double *Answer );
DLLEXPORT int StVolProbDensityGen(  STVOL_OPTION_PARMS_GEN *Context, double *Answer);

DLLEXPORT int StVolBinaryOption(	Q_OPT_TYPE	CallPut, STVOL_OPTION_PARMS	 	  *Context, double *Answer );
DLLEXPORT int StVolOptionDelta(		Q_OPT_TYPE	CallPut, STVOL_OPTION_PARMS	 	  *Context, double *Answer );
DLLEXPORT int StVolOptionValue(		Q_OPT_TYPE	CallPut, STVOL_OPTION_PARMS	 	  *Context, double *Answer );
DLLEXPORT int StVolOptionValueFuzzy(Q_OPT_TYPE	CallPut, STVOL_OPTION_PARMS_FUZZY *Context, double *Answer );
DLLEXPORT int StVolOptionValueGen(	Q_OPT_TYPE	CallPut, STVOL_OPTION_PARMS_GEN	  *Context, double *Answer );

DLLEXPORT int StVolLogCharFnFuzzy (		double Phi,	STVOL_OPTION_PARMS_FUZZY *Context, DT_COMPLEX *Result );
DLLEXPORT int StVolLogCharFnModFuzzy (	double Phi,	STVOL_OPTION_PARMS_FUZZY *Context, DT_COMPLEX *Result );

DLLEXPORT int StVolLogCharFnGen(		double Phi,	STVOL_OPTION_PARMS_GEN *Context, DT_COMPLEX *Result );
DLLEXPORT int StVolLogCharFnGenMod(		double Phi,	STVOL_OPTION_PARMS_GEN *Context, DT_COMPLEX *Result );
DLLEXPORT int StVolCharFnGen(			double Phi,	STVOL_OPTION_PARMS_GEN *Context, DT_COMPLEX *Result );
DLLEXPORT int StVolCharFnGenMod( 		double Phi,	STVOL_OPTION_PARMS_GEN *Context, DT_COMPLEX *Result );
DLLEXPORT int StVolOptionKernelGen(		double Phi,	void *Context, double *Result );
DLLEXPORT int StVolOptionKernelGenMod(	double Phi,	void *Context, double *Result );
DLLEXPORT int StVolDensityKernelGen(	double Phi,	void *Context, double *Result );
DLLEXPORT int StVolDensityKernelGenMod(	double Phi,	void *Context, double *Result );

DLLEXPORT int JDLogCharFn(			double Phi,	JD_OPTION_PARMS *Context, DT_COMPLEX *Result );
DLLEXPORT int JDLogCharFnMod(		double Phi,	JD_OPTION_PARMS *Context, DT_COMPLEX *Result );
DLLEXPORT int JDCharFn(				double Phi,	JD_OPTION_PARMS *Context, DT_COMPLEX *Result );
DLLEXPORT int JDCharFnMod( 			double Phi,	JD_OPTION_PARMS *Context, DT_COMPLEX *Result );
DLLEXPORT int JDOptionKernel(		double Phi,	void *Context, double *Result );
DLLEXPORT int JDOptionKernelMod(	double Phi,	void *Context, double *Result );
DLLEXPORT int JDDensityKernel(		double Phi,	void *Context, double *Result );
DLLEXPORT int JDDensityKernelMod(	double Phi,	void *Context, double *Result );

DLLEXPORT int JDOptionValue(		Q_OPT_TYPE	CallPut, JD_OPTION_PARMS	 	  *Context, double *Answer );

DLLEXPORT int StVolBackOutParams   (
	double         spot,        // current spot price
	Q_VECTOR       *strikes,    // strike prices for calls
	Q_VECTOR       *vols,       // implied volatilities
	double         initvol,     // initial volatility - known
	double         time,        // time to expiration
	double         ccRd,        // domestic interest rate
	double         ccRf,        // foreign interest rate, or yield
	double         tolerance,   // tolerance for convergence purposes
	double         *beta,       // backed out beta
	double         *corr,       // backed out correlation
	double         *vbar,       // backed out limiting variance
	double         *sigma,      // backed out volatility of volatility	
	double         *Error       // resulting least squares error
	);

DLLEXPORT int StVolBackOutParamsGen   (  
	double         spot,        // current spot price
	Q_MATRIX       *strikes,    // strike prices for calls
	Q_MATRIX       *vols,       // implied volatilities       
	double         initvol,     // initial volatility - known
	Q_VECTOR       *times,      // times to expiration
	Q_VECTOR       *Rd,         // domestic interest rate vector
	Q_VECTOR       *fwds,       // forward price vector
	double         tolerance,   // tolerance for convergence purposes
	Q_VECTOR       *beta,       // backed out betas
	Q_VECTOR       *corr,       // backed out correlations
	Q_VECTOR       *vbar,       // backed out limiting variances
	Q_VECTOR       *sigma,      // backed out volatility of volatility vector
	Q_VECTOR       *Error       // vector of least-squares errors
	);

DLLEXPORT double StVolRealizedVol (
    double       	spot,     // current spot price
    double          initmean, // initial mean instantaneous volatility
    double          initsd,   // initial uncertainty in instantaneous var
	double	     	lvol,     // forward-forward reverting vol
	double       	beta,     // mean reversion factor on volatility
	double       	sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      // correlation of spot and volatility
	double       	time,     // time to expiration
	double	     	fwd, 	  // forward
    int             nSteps,   // number of timesteps
    int             nPaths,   // number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType   // return error or mean sample vol
	);

DLLEXPORT double StVolFwdFwdVol (
    double       	spot,     // current spot price
    double          initmean, // initial mean instantaneous volatility
    double          initsd,   // initial uncertainty in instantaneous var
	double	     	lvol,     // forward-forward reverting vol
	double       	beta,     // mean reversion factor on volatility
	double       	sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      // correlation of spot and volatility
	double       	time,     // time to expiration
	double       	endterm,  // end of forward term
	double	     	fwd, 	  // forward to expiration
	double          endfwd,   // forward to end of forward term
    int             nSteps,   // number of timesteps
    int             nPaths,   // number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType   // return error or mean sample vol
	);

DLLEXPORT double StVolRealizedVolOpt (
	Q_OPT_TYPE      callPut,  		// option type
    double       	spot,     		// current spot price
    double          histmean, 		// historical mean return
    double          histsqmean,     // historical mean of squared returns
    int             npast,          // number of past returns
    double          strike,   		// strike price on volatility
    double          initmean, 		// initial mean instantaneous volatility
    double          initsd,   		// initial uncertainty in instantaneous var
	double	     	lvol,     		// forward-forward reverting vol
	double       	beta,     		// mean reversion factor on volatility
	double       	sigma,    		// coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      		// correlation of spot and volatility
	double       	time,     		// time to expiration
	double          annualize,      // multiply by sqrt(this) to get correct vol
	double	     	fwd, 	  		// forward
	double          ccRd,     		// discount rate
    int             nSteps,   		// number of timesteps
    int             nPaths,   		// number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType   		// return error or mean sample vol
	);

DLLEXPORT double StVolJumpOpt(
	Q_OPT_TYPE		callPut,		// option type
	double			spot,			// current spot price
	double			strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	double			InstVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			Rho,			// correlation between spot & vol
	double			JumpMean,		// mean of normally-distributed jump in ln(Spot)
	double			JumpSD,			// std dev of normally-distributed jump in ln(Spot)
	double			Lambda			// jump frequency
	);

DLLEXPORT double SVJumpStrikeFromDelta(
	double		Delta,				// The Delta of the option (-0.5 to 0.5)
	double		Spot,				// current spot level
	double		TimeToExp,			// time to expiration (years)
	double		OverRate,			// "over" ccy interest rate (abs #, not a percent)
	double		UnderRate,			// "under" ccy interest rate (abs #, not a percent)
	double		InstVol,			// current instantaneous volatility (abs #, not a percent)
	double		MeanVol,			// mean vol level (abs #, not a percent)
	double		Beta,				// mean reversion strength (units of /years)
	double		Sigma,				// vol of vol (abs #, not a percent)
	double		Rho,				// correlation between spot and vol diffusion processes
	double		JumpMean,			// mean size of (log-space) spot jumps
	double		JumpSD,				// standard deviation of (log-space) spot jumps
	double		Lambda,				// prob/unit time of a jump (units of /years)
	DELTA_TYPE	DeltaType,			// DELTA_TYPE_SPOT or DELTA_TYPE_FORWARD
	bool		IsUnderDelta		// whether the supplied Delta is an "under" Delta.
);

DLLEXPORT int SVJumpParamBackout(
	double		Spot,			 		// current spot
	Q_VECTOR	*TimesToExp,			// vector of times to expiration
	Q_VECTOR	*OverRates,				// vector of "over" ccy interest rates for the times to exp (abs #, not a percent)
	Q_VECTOR	*UnderRates,			// vector of "under" ccy interest rates
	Q_MATRIX	*Strikes,				// matrix of strikes; there is a set of strikes for each time to exp
	Q_MATRIX	*ImpliedVols,			// matrix of implied vols corresponding to the strikes/times to exp
	Q_MATRIX	*Weights,				// weightings for each implied vol value in the deviation calculation
	double		*InstVolRet,			// calculated inst. vol is returned via this pointer.
	double		*MeanVolRet,			// calculated mean vol is returned via this pointer.
	double		*BetaRet,				// calculated mean reversion strength is returned via this pointer.
	double		*SigmaRet,				// calculated vol of vol is returned via this pointer.
	double		*RhoRet,				// calculated correlation is returned via this pointer.
	double		*JumpMeanRet,			// calculated jump mean is returned via this pointer.
	double		*JumpSDRet,				// calculated jump std dev is returned via this pointer.
	double		*LambdaRet,				// calculated prob/time of a spot jump is returned via this pointer.
	double		*AvgDevSqRet,			// average squared deviation between model vols and real vols, weighted appropriately.
	double		DefaultInstVol,			// initial value for the current instantaneous vol (abs #, not a percent)
	double		DefaultMeanVol,			// initial value for the mean vol level
	double		DefaultBeta,			// initial value for the mean reversion strength
	double		DefaultSigma,			// initial value for the vol of vol
	double		DefaultRho,				// initial value for the spot/vol correlation
	double		DefaultJumpMean,		// initial value for the mean of the (log-space) spot jumps
	double		DefaultJumpSD,			// initial value for the std dev of the spot jumps
	double		DefaultLambda,			// initial value for the prob/unit time of a spot jump
	double		InstVolMax,				// max. allowed value for the current instantaneous vol (HUGE_VAL for no constraint)
	double		InstVolMin,				// min. allowed value for the current instantaneous vol
	double		MeanVolMax,				// max. allowed value for the mean vol level
	double		MeanVolMin,				// min. allowed value for the mean vol level
	double		BetaMax,				// max. allowed value for the mean reversion strength
	double		BetaMin,				// min. allowed value for the mean reversion strength
	double		SigmaMax,				// max. allowed value for the vol of vol
	double		SigmaMin,				// min. allowed value for the vol of vol
	double		RhoMax,					// max. allowed value for the spot/vol correlation
	double		RhoMin,					// min. allowed value for the spot/vol correlation
	double		JumpMeanMax,			// max. allowed value for the spot jump mean
	double		JumpMeanMin,			// min. allowed value for the spot jump mean
	double		JumpSDMax,				// max. allowed value for the spot jump std dev
	double		JumpSDMin,				// min. allowed value for the spot jump std dev
	double		LambdaMax,				// max. allowed value for the prob/unit time of a spot jump
	double		LambdaMin,				// min. allowed value for the prob/unit time of a spot jump
	bool		CalcInstVol,			// whether to calculate the inst. vol - otherwise, the default value is used
	bool		CalcMeanVol,			// whether to calculate the mean vol - otherwise, the default value is used
	bool		CalcBeta,				// whether to calculate the mean reversion strength - otherwise, the default value is used
	bool		CalcSigma,				// whether to calculate the vol of vol - otherwise, the default value is used
	bool		CalcRho,				// whether to calculate the correlation - otherwise, the default value is used
	bool		CalcJumpMean,			// whether to calculate the jump mean - otherwise, the default value is used
	bool		CalcJumpSD,				// whether to calculate the jump std dev - otherwise, the default value is used
	bool		CalcLambda,				// whether to calculate the prob/time of a jump - otherwise, the default value is used
	double		TightnessFactor,		// controls size of simplex - use a smaller value to constrain size of parameter space in search
	bool		EnforceNonZeroVol		// whether to constrain the parameters such that the inst. vol can never touch 0.
);

DLLEXPORT double SVOptApprox(
	Q_OPT_TYPE		callPut,		// option type
	double			spot,			// current spot price
	double			strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	double			InstVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			Rho 			// correlation between spot & vol
);

DLLEXPORT double SVOptApproxVol(
	double			spot,			// current spot price
	double			strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	double			InstVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			Rho 			// correlation between spot & vol
);

DLLEXPORT double SVOptCorrMC(
	Q_OPT_TYPE		OptType,		// option type
	double			Spot,			// current spot price
	double			Strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	double			InitVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			S0,				// spot level where correlation == 0
	double			Alpha,			// parameter controlling how wide the range of spot is over which correlation varies
	long			nSteps,			// # of time steps
	long			nRuns,			// # of simulation runs
	int				Seed 			// seed for RNG
);

DLLEXPORT double SVOTCorrMC(
	bool			IsHighStrike,	// true, it's a high-strike OT; false, it's a low-strike.
	double			Spot,			// current spot price
	double			Strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	Q_REBATE_TYPE	PayType,		// Q_IMMEDIATE or Q_DELAYED
	bool			PaysUnder,		// true, pays out in the under ccy; false, pays in over ccy.
	double			InitVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			S0,				// spot level where correlation == 0
	double			Alpha,			// parameter controlling how wide the range of spot is over which correlation varies
	long			nSteps,			// # of time steps
	long			nRuns,			// # of simulation runs
	int				Seed			// seed for RNG
);

DLLEXPORT double SVOptSCorrMC(
	Q_OPT_TYPE		OptType,		// option type
	double			Spot,			// current spot price
	double			Strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	double			InitVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			Gamma,			// mean reversion strength of "z", the transformed spot/vol correlation
	double			InitCorr,		// initial spot/vol correlation
	double			MeanCorr,		// mean level for "z", the transformed spot/vol correlation
	double			Eps,			// the "volatility" of the transformed spot/vol correlation
	double			CorrCS,			// correlation between the spot/vol correlation and spot
	double			CorrCV,			// correlation between the spot/vol correlation and vol^2
	long			nSteps,			// # of time steps
	long			nRuns,			// # of simulation runs
	int				Seed			// seed for RNG
);

DLLEXPORT double SVOptLNSCorrMC(
	Q_OPT_TYPE		OptType,		// option type
	double			Spot,			// current spot price
	double			Strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	double			InitVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			Gamma,			// mean reversion strength of "z", the transformed spot/vol correlation
	double			InitCorr,		// initial spot/vol correlation
	double			MeanCorr,		// mean level for "z", the transformed spot/vol correlation
	double			Eps,			// the "volatility" of the transformed spot/vol correlation
	double			CorrCS,			// correlation between the spot/vol correlation and spot
	double			CorrCV,			// correlation between the spot/vol correlation and vol^2
	long			nSteps,			// # of time steps
	long			nRuns,			// # of simulation runs
	int				Seed			// seed for RNG
);

DLLEXPORT double SVOptPhiSCorrMC(
	Q_OPT_TYPE		OptType,		// option type
	double			Spot,			// current spot price
	double			Strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	double			InitVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			Gamma,			// mean reversion strength of "z", the transformed spot/vol correlation
	double			InitCorr,		// initial spot/vol correlation
	double			MeanCorr,		// mean level for "z", the transformed spot/vol correlation
	double			Eps,			// the "volatility" of the transformed spot/vol correlation
	double			CorrCS,			// correlation between the spot/vol correlation and spot
	double			CorrCV,			// correlation between the spot/vol correlation and vol^2
	double			Phi,			// stoch term in SDE for v (==inst vol^2) is proportional to v^Phi
	long			nSteps,			// # of time steps
	long			nRuns,			// # of simulation runs
	int				Seed			// seed for RNG
);

DLLEXPORT double SVOTPhiSCorrMC(
	bool			IsHighStrike,	// true, it's a high-strike OT; false, it's a low-strike.
	double			Spot,			// current spot price
	double			Strike,			// option strike price
	double			Texp,			// time to expiration
	double			ccRd,			// "over" currency risk-free rate to expiration
	double			ccRf,			// "under" currency risk-free rate to expiration
	Q_REBATE_TYPE	PayType,		// Q_IMMEDIATE or Q_DELAYED
	bool			PaysUnder,		// true, pays out in the under ccy; false, pays in over ccy.
	double			InitVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			MeanVol,		// mean volatility (sqrt of mean vol^2)
	double			Beta,			// mean reversion strength
	double			Sigma,			// vol of vol
	double			Gamma,			// mean reversion strength of "z", the transformed spot/vol correlation
	double			InitCorr,		// initial spot/vol correlation
	double			MeanCorr,		// mean level for "z", the transformed spot/vol correlation
	double			Eps,			// the "volatility" of the transformed spot/vol correlation
	double			CorrCS,			// correlation between the spot/vol correlation and spot
	double			CorrCV,			// correlation between the spot/vol correlation and vol^2
	double			Phi,			// stoch term in v=vol^2 is proportional to v^Phi
	long			nSteps,			// # of time steps
	long			nRuns,			// # of simulation runs
	int				Seed			// seed for RNG
);

DLLEXPORT double SVBondOpt2 (
    Q_OPT_TYPE   	      callPut,       // option type                                        
    double       	      spot,          // current bond yield                                 
    double       	      strike,        // strike price                                       
	N_GENERIC_FUNC_2D     BondPrice,     // bond pricing function with yield and time as inputs
	void                  *context,      // structure containing coupon, period, maturity data for bond
	Q_VECTOR      	      *lvol,         // vector of mean volatilities                        
	Q_VECTOR 		      *lvolT,        // time vector for lvol                               
	double                initvol,       // initial short-term vol                             
	Q_VECTOR       	      *beta,         // mean reversion factor on volatility                
	Q_VECTOR       	      *sigma,        // coeff of sqrt(vol) in stochastic term for vol      
	Q_VECTOR       	      *corr,         // correlation of spot and volatility                 
	double       	      time,          // time to expiration                                 
	Q_VECTOR     	      *fwds,         // forward vector for bond yields
	Q_VECTOR     	      *fwdT,         // forward time vector                                
	double                ccRd,          // rate used for discounting                          
	int                   nGrid1         // number of grid points in spot dimension
	);

DLLEXPORT double SVBondOpt (
    Q_OPT_TYPE   	      callPut,       // option type                                        
    double       	      spot,          // current bond yield                                 
    double       	      strike,        // strike price                                       
    Q_VECTOR        	  *coupon,       // coupon vector for bond
    Q_VECTOR              *couponT,      // times when coupons are paid
    double                period,        // period used for calculating bond price
	Q_VECTOR      	      *lvol,         // vector of mean volatilities                        
	Q_VECTOR 		      *lvolT,        // time vector for lvol                               
	double                initvol,       // initial short-term vol                             
	Q_VECTOR       	      *beta,         // mean reversion factor on volatility                
	Q_VECTOR       	      *sigma,        // coeff of sqrt(vol) in stochastic term for vol      
	Q_VECTOR       	      *corr,         // correlation of spot and volatility                 
	double       	      time,          // time to expiration                                 
	Q_VECTOR     	      *fwds,         // forward vector for bond yields
	Q_VECTOR     	      *fwdT,         // forward time vector                                
	double                ccRd,          // rate used for discounting                          
	int                   nGrid1         // number of grid points in spot dimension
	);

typedef struct {
	Q_OPT_TYPE   	      callPut;   // option type                                       
	double       	      spot;      // current spot yield                                
	Q_VECTOR       	      *strikes;  // array of strikes for expiration T                 
	Q_VECTOR              *prices;   // market option prices, used for objective function 
	N_GENERIC_FUNC_2D     BondPrice; // bond pricing function with yield and time as inputs
	void                  *context;  // structure containing coupon, period, maturity data for bond
	double                initvol;   // initial instantaneous volatility       
	Q_VECTOR       	      *times;    // times to expiration of options         
	Q_VECTOR     	      *fwds;     // forward yield curve                    
	Q_VECTOR     	      *fwdT;     // nodes for forward yield curve          
	Q_VECTOR              *ccRd;     // rates used for discounting the options 
	int                   nGrid1;    // size of spot grid                      
	double                extra;     // extra parameter that is fixed
	int                   fixed;     // 1 fixes beta, 2 corr, 3 lvol, 4 sigma, 0 none 
	double                *options;  // returned option values, if desired
	int                   fittype;   // 0 means price-based, 1 means volatility-based
	} BONDOPT_PARMS_SIMPLE2;		

typedef struct {
	Q_OPT_TYPE   	      callPut;   // option type                                       
	double       	      spot;      // current spot yield                                
	Q_VECTOR       	      *strikes;  // array of strikes for expiration T                 
	Q_VECTOR              *prices;   // market option prices, used for objective function 
	Q_VECTOR              *coupon;   // array of coupons
	Q_VECTOR              *couponT;  // array of coupon payment times
	double                period;    // period used in calculating price from yield
	double                initvol;   // initial instantaneous volatility       
	Q_VECTOR       	      *times;    // times to expiration of options         
	Q_VECTOR     	      *fwds;     // forward yield curve                    
	Q_VECTOR     	      *fwdT;     // nodes for forward yield curve          
	Q_VECTOR              *ccRd;     // rates used for discounting the options 
	int                   nGrid1;    // size of spot grid                      
	double                extra;     // extra parameter that is fixed
	int                   fixed;     // 1 fixes beta, 2 corr, 3 lvol, 4 sigma, 0 none 
	double                *options;  // returned option values, if desired
	int                   fittype;   // 0 means price-based, 1 means volatility-based
	} BONDOPT_PARMS_SIMPLE;		

DLLEXPORT int SVBondOptFitOne2 (
    double       	      spot,      // current bond yield     
	Q_VECTOR              *strikes,  // strike prices for calls
    Q_VECTOR       	      *prices,   // prices of calls        
	N_GENERIC_FUNC_2D     BondPrice, // bond pricing function with yield and time as inputs
	void                  *context,  // structure containing coupon, period, maturity data for bond
	double                initvol,   // initial short-term vol                                   
	Q_VECTOR              *times,    // times to expiration of the options                       
	Q_VECTOR     	      *fwds,     // forward vector                                           
	Q_VECTOR     	      *fwdT,     // forward time vector                                      
	Q_VECTOR              *ccRd,     // one discount rate per option expiration                  
	int                   nGrid1,    // gridsize in spot dimension                               
	int             	  fixed,     // 1 fixes beta, 2 corr, 3 lvol, 4 sigma, 0 none
	double                tolerance, // parameter determining when to stop amoeba                
	double       	      *lvol,     // backed out limiting volatility                           
	double       	      *beta,     // backed out mean reversion factor on volatility           
	double       	      *sigma,    // backed out coeff of sqrt(vol) in stochastic term for vol 
	double       	      *corr,     // backed out correlation of spot and volatility            
	double                *Error,    // vector of least-squares error                            
	double                *options,  // returned array of option values
	int                   fittype    // 0 means price-based, 1 means volatility-based
	);

DLLEXPORT int SVBondOptFitOne (
    double       	      spot,      // current bond yield     
	Q_VECTOR              *strikes,  // strike prices for calls
    Q_VECTOR       	      *prices,   // prices of calls        
    Q_VECTOR        	  *coupon,   // coupon vector for bond
    Q_VECTOR              *couponT,  // times when coupons are paid
    double                period,    // period used for calculating bond price
	double                initvol,   // initial short-term vol                                   
	Q_VECTOR              *times,    // times to expiration of the options                       
	Q_VECTOR     	      *fwds,     // forward vector                                           
	Q_VECTOR     	      *fwdT,     // forward time vector                                      
	Q_VECTOR              *ccRd,     // one discount rate per option expiration                  
	int                   nGrid1,    // gridsize in spot dimension                               
	int             	  fixed,     // 1 fixes beta, 2 corr, 3 lvol, 4 sigma, 0 none
	double                tolerance, // parameter determining when to stop amoeba                
	double       	      *lvol,     // backed out limiting volatility                           
	double       	      *beta,     // backed out mean reversion factor on volatility           
	double       	      *sigma,    // backed out coeff of sqrt(vol) in stochastic term for vol 
	double       	      *corr,     // backed out correlation of spot and volatility            
	double                *Error,    // vector of least-squares error                            
	double                *options,  // returned array of option values
	int                   fittype    // 0 means price-based, 1 means volatility-based
	);

#endif

