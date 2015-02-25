/****************************************************************
**
**	FqBlackSHIFTEDLOGNORMAL.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqBlackShiftedLognormal.h,v 1.7 1999/11/05 23:36:26 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FqBlackSHIFTEDLOGNORMAL_H )
#define IN_FqBlackSHIFTEDLOGNORMAL_H

#include <fiblack/FqBlack.h>
#include <gscore/GsFuncLine.h>
#include <gsquant/GsFuncSpecial.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqBlackShiftedLognormal : public FqBlack
{
public:

	FqBlackShiftedLognormal( double boundary = 0.0, double shift = 0.0 )
	:	m_boundary( new GsFuncLine(boundary) ), m_shift( new GsFuncLine(shift) )
	{
	}
	FqBlackShiftedLognormal( GsFuncHandle<double,double> boundary, GsFuncHandle<double,double> shift )
	:	m_boundary( boundary ), m_shift( shift )
	{
    }
	virtual ~FqBlackShiftedLognormal();
	virtual FqBlack* copy() const;
	virtual FqBlackType type() const;
	virtual GsString toString() const;
	
	
	void setBoundary( double b )
	{	
		m_boundary = new GsFuncLine(b);
	}
	double getBoundary() const
	{
		return m_boundary(0);
	}

	void setShift( double s )
	{	
		m_shift = new GsFuncLine(s);
	}
	double getShift() const
	{
		return m_shift(0);
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

	// for fitter
	virtual GsStringVector getParameterNames() const;
	virtual GsStatus setParameter( const GsString& paramName,  const GsFuncHandle<double,double>& param );
	virtual GsFuncHandle<double,double> getParameter( const GsString& paramName ) const;
	virtual bool hasParameter( const GsString& name ) const;

private:

	GsFuncHandle<double,double> m_boundary;
	GsFuncHandle<double,double> m_shift;

};

EXPORT_CPP_FIBLACK
FqBlack* FqBsCreateShiftedLognormalAbsorbing(double barrier, double shift);

EXPORT_CPP_FIBLACK
FqBlack* FqBsCreateShiftedLognormalAbsorbing2(const GsFuncScFunc& barrier, const GsFuncScFunc& shift);

CC_END_NAMESPACE

#endif 
