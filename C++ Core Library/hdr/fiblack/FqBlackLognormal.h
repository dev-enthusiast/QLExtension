/****************************************************************
**
**	FQBLACKLOGNORMAL.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqBlackLognormal.h,v 1.9 2000/04/12 20:05:54 lischf Exp $
**
****************************************************************/

#if !defined( IN_FQBLACKLOGNORMAL_H )
#define IN_FQBLACKLOGNORMAL_H

#include <fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqBlackLognormal : public FqBlack
{
public:
	FqBlackLognormal() {}
	virtual ~FqBlackLognormal();
	virtual FqBlack* copy() const;
	virtual GsString toString() const;
	virtual FqBlackType type() const;
	virtual double eqLognormVol(Q_OPT_TYPE opttype, double strike, double term, double fwd, double sigma) const
	    { return sigma; } 

protected:

	virtual double cvalue(double strike, double term, double fwd, double sigma) const;
	virtual double ckappa(double delta, double term, double fwd, double sigma) const;
	virtual double cdelta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cgamma(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double ctheta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvanna(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvega(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvolga(double strike, double term, double fwd, double sigma, double bump=0) const;
	
	// binary
	virtual double cbinaryValue(double strike, double term, double fwd, double sigma) const;
	virtual double cbinaryKappa(double delta, double term, double fwd, double sigma) const;
	virtual double cbinaryDelta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryGamma(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryTheta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryVanna(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryVega(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cbinaryVolga(double strike, double term, double fwd, double sigma, double bump=0) const;

	// fwd variance
	virtual double fwdVar(double term, double fwd, double sigma) const;

	// trading tools

	virtual double cGammaGain(double strike, double term, double horizon, double fwd, double mktVol, double myVol) const;
	virtual double cGammaGainBounds(double strike, double term, double horizon, double mktVol, double myVol, int whichBound) const;


	// for implied vol calculation	
	virtual double volGuess() const;

};

EXPORT_CPP_FIBLACK
FqBlack* FqBsCreateLognormal();

CC_END_NAMESPACE

#endif 
