/****************************************************************
**
**	FQBLACK.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqBlack.h,v 1.36 2011/08/11 03:06:01 khodod Exp $
**
****************************************************************/

#if !defined( IN_FQBLACK_H )
#define IN_FQBLACK_H

#include <fiblack/base.h>
#include <gsdt/GsDtGeneric.h>
#include <gsdt/GsDtString.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDoubleVector.h>
#include <gscore/GsVector.h>
#include <gscore/GsDiscountCurve.h>
#include <gscore/GsIntegerVector.h>
#include <gscore/GsStringVector.h>
#include <gscore/GsCTimeCurve.h>
#include <gscore/GsDateVector.h>
#include <gsquant/Enums.h>
#include <gsquant/GsFuncSpecial.h>
#include <gsquant/GsFuncDateToContinuous.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	All FqBlack types are enumerated here, since the world is small.
*/

enum FqBlackType
{
	FQBS_LOGNORMAL,
	FQBS_NORMAL,
	FQBS_NORMAL_ABSORBING,
	FQBS_SHIFTED_LOGNORMAL,
	FQBS_TUNNEL,
	FQBS_SMILE_LOGNORMAL,
	FQBS_SMILE_NORMAL,
	FQBS_SMILE_HESTON,
	FQBS_SMILE_BS_BETA_CORR,
	FQBS_SMILE_HESTON_ZERO,
	FQBS_SMILE_RANDOM_VOL,
	FQBS_SMILE_MERTON,
	FQBS_CEV,
	FQBS_SMILE_CEV_LOGNORMAL
};

class EXPORT_CLASS_FIBLACK FqBlack
{
public:
	//// GsDt ////
	typedef GsDtGeneric< FqBlack > GsDtType;
	inline static const char* typeName() { return "FqBlack"; }

	FqBlack() {}
	virtual ~FqBlack() = 0;
	virtual FqBlack* copy() const = 0;
	virtual GsString toString() const = 0;
	virtual FqBlackType type() const = 0;

	virtual double value(Q_OPT_TYPE opttype, double strike, double term, 
						 double fwd, double sigma) const;
	virtual double delta(Q_OPT_TYPE opttype, double strike, double term, 
						 double fwd, double sigma, double bump=0) const;
	virtual double gamma(Q_OPT_TYPE opttype, double strike, double term, 
						 double fwd, double sigma, double bump=0) const;
	virtual double theta(Q_OPT_TYPE opttype, double strike, double term, 
						 double fwd, double sigma, double bump=0) const;
	virtual double vanna(Q_OPT_TYPE opttype, double strike, double term, 
						double fwd, double sigma, double bump=0) const;
	virtual double vega(Q_OPT_TYPE opttype, double strike, double term, 
						 double fwd, double sigma, double bump=0) const;
	virtual double volga(Q_OPT_TYPE opttype, double strike, double term, 
						double fwd, double sigma, double bump=0) const;
	virtual double kappa(Q_OPT_TYPE opttype, double delta, double term,
						 double fwd, double sigma) const;
	virtual double impvol(Q_OPT_TYPE opttype, double strike, double term, 
						  double fwd, double pv) const;
	virtual double impStrike(Q_OPT_TYPE opttype, double term, double fwd, 
						  double sigma, double pv) const;

	virtual double eqLognormVol(Q_OPT_TYPE opttype, double strike, double term, 
						 double fwd, double sigma) const;
 
	
	// binary
	virtual double binaryValue(Q_OPT_TYPE opttype, double strike, double term, 
							   double fwd, double sigma) const;
	virtual double binaryDelta(Q_OPT_TYPE opttype, double strike, double term, 
							   double fwd, double sigma, double bump=0) const;
	virtual double binaryKappa(Q_OPT_TYPE opttype, double delta, double term,
							   double fwd, double sigma) const;
	virtual double binaryGamma(Q_OPT_TYPE opttype, double strike, double term, 
							   double fwd, double sigma, double bump=0) const;
	virtual double binaryTheta(Q_OPT_TYPE opttype, double strike, double term, 
							   double fwd, double sigma, double bump=0) const;
	virtual double binaryVanna(Q_OPT_TYPE opttype, double strike, double term, 
							   double fwd, double sigma, double bump=0) const;
	virtual double binaryVega(Q_OPT_TYPE opttype, double strike, double term, 
							  double fwd, double sigma, double bump=0) const;
	virtual double binaryVolga(Q_OPT_TYPE opttype, double strike, double term, 
							  double fwd, double sigma, double bump=0) const;
	//gains from gamma trade

	// fwd variance & covariance
	virtual double fwdVar(double term, double fwd, double sigma) const;
	static double fwdCov(const FqBlack& dist1, const FqBlack& dist2, double rho, double term, double fwd1, double fwd2, double sigma1, double sigma2);
	static double fwdCC(double t, int n, double delta, double t0, int k, double epsilon, double s0, double delta0, double epsilon0, double r1, double r2, double r3, double r4, double var, double cov12, double cov13, double cov14);

	// name of parameters
	virtual GsStringVector getParameterNames() const;
	virtual GsStatus setParameter( const GsString& paramName,  const GsFuncHandle<double,double>& param );
	virtual GsFuncHandle<double,double> getParameter( const GsString& paramName ) const;
	virtual bool hasParameter( const GsString& name ) const;

    virtual double GammaGain(Q_OPT_TYPE opttype, double strike, double term, double horizon, double fwd, double mktVol, double myVol) const;
    virtual double GammaGainBounds(Q_OPT_TYPE opttype, double strike, double term, double horizon, double mktVol, double myVol, int whichBound) const;

protected:
	
	// call versions
	virtual double cvalue(double strike, double term, double fwd, double sigma) const = 0;
	virtual double ckappa(double delta, double term, double fwd, double sigma) const;
	virtual double cdelta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cgamma(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double ctheta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvanna(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvega(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvolga(double strike, double term, double fwd, double sigma, double bump=0) const;
	
	// binary
	virtual double cbinaryValue(double strike, double term, double fwd, double sigma) const = 0;
	virtual double cbinaryKappa(double delta, double term, double fwd, double sigma) const;
	virtual double cbinaryDelta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryGamma(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryTheta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryVanna(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryVega(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryVolga(double strike, double term, double fwd, double sigma, double bump=0) const;

	// for implied vol calculation	
	virtual double volGuess() const = 0;

	// covariance routines
	static double fwd_cov_ln_ln(double fwd1, double fwd2, double term, double sigma1, double sigma2, double rho);
	static double fwd_cov_ln_n(double fwd1, double fwd2, double term, double sigma1, double sigma2, double rho);
	static double fwd_cov_ln_sln(double fwd1, double fwd2, double term, double sigma1, double sigma2, double rho, 
                                 double boundary2, double shift2);
	static double fwd_cov_n_n(double fwd1, double fwd2, double term, double sigma1, double sigma2, double rho);
	static double fwd_cov_n_sln(double fwd1, double fwd2, double term, double sigma1, double sigma2, double rho, 
                                double boundary2, double shift2);
	static double fwd_cov_sln_sln(double fwd1, double fwd2, double term, double sigma1, double sigma2, double rho, 
								  double boundary1, double boundary2, double shift1, double shift2);


	//gains from gamma trade
	// FIX - Make this =0 when we have implemented it in all derived classes.
    virtual double cGammaGain(double strike, double term, double horizon, double fwd, double mktVol, double myVol) const;
    virtual double cGammaGainBounds(double strike, double term, double horizon, double mktVol, double myVol, int whichBound ) const;

 
                               
};

EXPORT_CPP_FIBLACK
GsCTimeCurve GsCTimeCurveCreatePiecewise(
	const GsDateVector& dates,
	const GsDoubleVector& values,
	const GsDate& refDate
);

EXPORT_CPP_FIBLACK
GsCTimeCurve GsCTimeCurveCreateLinear(
	const GsDateVector& dates,		
	const GsDoubleVector& values,	
	const GsDate& refDate			
);

EXPORT_CPP_FIBLACK
GsDiscountCurve GsDiscountCurveCreateLogLinear(
	const GsDateVector& dates,		
	const GsDoubleVector& values,	
	const GsDate& refDate			
);

EXPORT_CPP_FIBLACK
double GsCTimeCurveLookupByDate(
	const GsCTimeCurve&,
	const GsDate&
);

EXPORT_CPP_FIBLACK
double GsCTimeCurveLookupByCTime(
	const GsCTimeCurve&,
	double
);

EXPORT_CPP_FIBLACK
GsStringVector FqBsGetParameterNames(
	const FqBlack&
);

EXPORT_CPP_FIBLACK
GsFuncScFunc FqBsGetParameter(
	const FqBlack&,
	const GsString&
);

EXPORT_CPP_FIBLACK
double FqBsGetParameterValue(
	const GsFuncScFunc& Func,
	double Term
);

EXPORT_CPP_FIBLACK
double FqBlackRawImpVol(
	const FqBlack &Dist,
	Q_OPT_TYPE OptType, 
	double Strike, 		
	double Term, 		
	double Fwd, 		
	double PV			
);


EXPORT_CPP_FIBLACK
double FqBlackFwdVar(
	const FqBlack&	distribution,
	double	term,
	double  fwd,
	double	sigma
);

EXPORT_CPP_FIBLACK
GsVector FqBlackFwdVarVector(
	const FqBlack&	distribution,
	const GsDateVector&	term,			
	const GsFuncDateToDouble& ctime,
	const GsVector&  fwd,			
	const GsVector&	sigma			
);

EXPORT_CPP_FIBLACK
double FqBlackFwdCov(
	const FqBlack& dist1,
	const FqBlack& dist2,
	double rho,				
	double term,			
	double		fwd1,		
	double		fwd2,		
	double		sigma1,		
	double		sigma2		
);

EXPORT_CPP_FIBLACK
GsVector FqBlackFwdCovVector(
	const FqBlack& dist1,		
	const FqBlack& dist2,	 	
	const GsVector& rho,		
	const GsDateVector& term,		
	const GsFuncDateToDouble& ctime,
	const GsVector&	fwd1,		
	const GsVector&	fwd2,		
	const GsVector&	sigma1,		
	const GsVector&	sigma2		
);

EXPORT_CPP_FIBLACK
double FqBlackFwdCC(
	double t,		
	int n,			
	double delta,	
	double t0,		
	int k,			
	double epsilon,	
	double s0,		
	double delta0,	
	double epsilon0,
	double r1,		
	double r2,		
	double r3,		
	double r4,		
	double var,		
	double cov12,	
	double cov13,	
	double cov14	
);

EXPORT_CPP_FIBLACK
GsVector FqBlackFwdCCVec(
	const GsDateVector&  t,				
	const GsFuncDateToDouble& ctime,
	const GsIntegerVector& n,		
	const GsVector&  delta, 		
	const GsIntegerVector&  k,		
	const GsVector&  epsilon,		
	const GsVector&  r1,			
	const GsVector&  r2,			
	const GsVector&  var,			
	const GsVector&  cov12			
);

EXPORT_CPP_FIBLACK
GsVector FqBlackFwdQCVec(
	const FqBlack&		 rateDist,		
	const GsDateVector&  t,				
	const GsFuncDateToDouble& ctime,	
	const GsVector&  rate,				
	const GsVector&  rateVol,			
	const GsVector&  fxVol,				
	const GsVector&  corr				
);

EXPORT_CPP_FIBLACK
GsVector FqBlackFwdQCVec(
	const FqBlack&		 rateDist,		
	const GsDateVector&  t,				
	const GsFuncDateToDouble& ctime,	
	const GsVector&  rate,				
	const GsVector&  rateVol,			
	const GsVector&  fxVol,				
	const GsVector&  corr				
);
	
CC_END_NAMESPACE

#endif
