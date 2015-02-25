/****************************************************************
**
**	FqBlackNORMAL.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqBlackNormal.h,v 1.6 1999/11/10 20:36:56 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FqBlackNORMAL_H )
#define IN_FqBlackNORMAL_H

#include <fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqBlackNormal : public FqBlack
{
public:
	FqBlackNormal() {}
	virtual ~FqBlackNormal();
	virtual FqBlack* copy() const;
	virtual FqBlackType type() const;
	virtual GsString toString() const;

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

	// for implied vol calculation	
	virtual double volGuess() const;

};

EXPORT_CPP_FIBLACK
FqBlack* FqBsCreateNormal();

CC_END_NAMESPACE

#endif 
