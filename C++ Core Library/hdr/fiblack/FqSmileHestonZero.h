/****************************************************************
**
**	FqSmileHeston.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqSmileHestonZero.h,v 1.1 2001/06/22 19:08:41 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FIBLACK_FQSMILEHESTON_ZERO_H )
#define IN_FIBLACK_FQSMILEHESTON_ZERO_H

#include <fiblack/base.h>
#include <gscore/GsFuncLine.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsCTimeCurve.h>
#include <fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqSmileHestonZero : public FqBlack
{
public:
	FqSmileHestonZero(double shift=0.0, double beta=0.0, double longvol=0.0, double volvol=0.0 )
	: 	m_shift(new GsFuncLine(shift)), 
		m_beta(new GsFuncLine(beta)), 
		m_longvol(new GsFuncLine(longvol)), 
		m_volvol(new GsFuncLine(volvol))
	{
    }

	FqSmileHestonZero(const GsFuncHandle<double,double>& shift,
				      const GsFuncHandle<double,double>& beta,
				      const GsFuncHandle<double,double>& longvol,
				      const GsFuncHandle<double,double>& volvol)
	:	m_shift(shift),
		m_beta(beta),
		m_longvol(longvol),
		m_volvol(volvol)
	{
    }

    virtual ~FqSmileHestonZero();
	virtual FqBlack* copy() const;
	virtual GsString toString() const;
	virtual FqBlackType type() const;

	void setShift (const GsFuncHandle<double,double>& a)
	{
		m_shift=a;
	}
	GsFuncHandle<double,double> getShift() const
	{
		return m_shift;
	}
    void setBeta (const GsFuncHandle<double,double>& b)
    {
		m_beta=b;
    }
    GsFuncHandle<double,double> getBeta() const
    {
		return m_beta;
    }
    void setLongvol (const GsFuncHandle<double,double>& c)
    {
		m_longvol=c;
    }
    GsFuncHandle<double,double> getLongvol() const
    {
		return m_longvol;
    }
    void setVolvol (const GsFuncHandle<double,double>& d)
    {
		m_volvol=d;
    }
    GsFuncHandle<double,double> getVolvol() const
    {
		return m_volvol;
    }

protected:

	virtual double cvalue(double strike, double term, double fwd, double sigma) const;
	virtual double cdelta(double strike, double term, double fwd, double sigma, double bump=0) const;
    //binary
    virtual double cbinaryValue(double strike, double term, double fwd, double sigma) const;

	virtual double fwdVar(double term, double fwd, double sigma) const;

	// for fitter
	virtual GsStringVector getParameterNames() const;
	virtual GsStatus setParameter( const GsString& paramName,  const GsFuncHandle<double,double>& param );
	virtual GsFuncHandle<double,double> getParameter( const GsString& paramName ) const;
	virtual bool hasParameter( const GsString& name ) const;

	double volGuess() const;  	


private:

	GsFuncHandle<double, double> m_shift;
	GsFuncHandle<double, double> m_beta;
	GsFuncHandle<double, double> m_longvol;
	GsFuncHandle<double, double> m_volvol;
};

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateHestonZero(double shift, double beta, double longvol, double volvol);

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateHestonZero2(
    const GsFuncScFunc& shift, 
	const GsFuncScFunc& beta, 
	const GsFuncScFunc& longvol, 
	const GsFuncScFunc& volvol
);

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateHestonZeroFromCurves(
	const GsCTimeCurve& shift, 
	const GsCTimeCurve& beta, 
	const GsCTimeCurve& longvol, 
	const GsCTimeCurve& volvol
);

CC_END_NAMESPACE

#endif 

