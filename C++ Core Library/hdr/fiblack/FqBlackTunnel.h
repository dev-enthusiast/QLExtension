/****************************************************************
**
**	FQBLACKTUNNEL.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqBlackTunnel.h,v 1.5 1999/09/24 21:46:14 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FQBLACKTUNNEL_H )
#define IN_FQBLACKTUNNEL_H

#include <fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqBlackTunnel : public FqBlack
{
public:
	FqBlackTunnel( double yl=0, double yu=1 ) :	m_yl( yl ), m_yu( yu ) {}
	virtual ~FqBlackTunnel();
	virtual FqBlack* copy() const;
	virtual FqBlackType type() const;
	virtual GsString toString() const;

protected:
	
	virtual double cvalue(double strike, double term, double fwd, double sigma) const;
	virtual double cdelta(double strike, double term, double fwd, double sigma, double bump) const;
	virtual double cgamma(double strike, double term, double fwd, double sigma, double bump) const;
	virtual double ctheta(double strike, double term, double fwd, double sigma, double bump) const;
	virtual double cvega(double strike, double term, double fwd, double sigma, double bump) const;
	
	// binary
	virtual double cbinaryValue(double strike, double term, double fwd, double sigma) const;
	virtual double cbinaryDelta(double strike, double term, double fwd, double sigma, double bump) const;
	virtual double cbinaryGamma(double strike, double term, double fwd, double sigma, double bump) const;
	virtual double cbinaryTheta(double strike, double term, double fwd, double sigma, double bump) const;
	virtual double cbinaryVega(double strike, double term, double fwd, double sigma, double bump) const;

	// fwd variance
	virtual double fwdVar(double term, double fwd, double sigma) const;

	// for implied vol calculation	
	virtual double volGuess() const;

private:
	double	m_yl,
			m_yu;
};

EXPORT_CPP_FIBLACK FqBlack*
		FqBsCreateTunnel( double yl, double yu );

CC_END_NAMESPACE

#endif 
