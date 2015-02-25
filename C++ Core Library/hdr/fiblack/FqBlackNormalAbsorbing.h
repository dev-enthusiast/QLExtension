/****************************************************************
**
**	FqBlackNORMALABSORBING.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqBlackNormalAbsorbing.h,v 1.5 1999/09/24 21:46:14 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FqBlackNORMALABSORBING_H )
#define IN_FqBlackNORMALABSORBING_H

#include <fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqBlackNormalAbsorbing : public FqBlack
{
public:
	FqBlackNormalAbsorbing( double boundary = 0.0 )
	:	m_boundary( boundary )
	{
	}
	virtual ~FqBlackNormalAbsorbing();
	virtual FqBlack* copy() const;
	virtual FqBlackType type() const;
	virtual GsString toString() const;

	void setBoundary(double b)
	{	
		m_boundary = b;
	}
	double getBoundary() const
	{
		return m_boundary;
	}

protected:
	virtual double cvalue(double strike, double term, double fwd, double sigma) const;
	virtual double cdelta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cgamma(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double ctheta(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvanna(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvega(double strike, double term, double fwd, double sigma, double bump=0) const;
	virtual double cvolga(double strike, double term, double fwd, double sigma, double bump=0) const;
	
	// binary
	virtual double cbinaryValue(double strike, double term, double fwd, double sigma) const;
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

private:
	double m_boundary;

};

EXPORT_CPP_FIBLACK FqBlack*
		FqBsCreateNormalAbsorbing( double barrier );

CC_END_NAMESPACE

#endif 
