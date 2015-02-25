/****************************************************************
**
**	FqSmileHeston.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqSmileHeston.h,v 1.1 2001/06/22 19:08:40 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FIBLACK_FQSMILEHESTON_H )
#define IN_FIBLACK_FQSMILEHESTON_H

#include <fiblack/base.h>
#include <gscore/GsFuncLine.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsCTimeCurve.h>
#include <fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqSmileHeston : public FqBlack
{
public:
	FqSmileHeston(double shift=0.0, double beta=0.0, double longvol=0.0, double volvol=0.0, double corr=0.0 )
	: 	m_shift(new GsFuncLine(shift)), 
		m_beta(new GsFuncLine(beta)), 
		m_longvol(new GsFuncLine(longvol)), 
		m_volvol(new GsFuncLine(volvol)),
		m_corr(new GsFuncLine(corr))
	{
    }

	FqSmileHeston(const GsFuncHandle<double,double>& shift,
				  const GsFuncHandle<double,double>& beta,
				  const GsFuncHandle<double,double>& longvol,
				  const GsFuncHandle<double,double>& volvol,
				  const GsFuncHandle<double,double>& corr )
	:	m_shift(shift),
		m_beta(beta),
		m_longvol(longvol),
		m_volvol(volvol),
		m_corr(corr)
	{
    }

    virtual ~FqSmileHeston();
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
	void setCorrelation( const GsFuncHandle<double,double>& e)
	{
		m_corr=e;
	}
	GsFuncHandle<double,double> getCorrelation() const
	{
		return m_corr;
	}


protected:

	virtual double cvalue(double strike, double term, double fwd, double sigma) const;
	virtual double cdelta(double strike, double term, double fwd, double sigma, double bump=0) const;
    //binary
    virtual double cbinaryValue(double strike, double term, double fwd, double sigma) const;

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
	GsFuncHandle<double, double> m_corr;

};

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateHeston(double shift, double beta, double longvol, double volvol, double correlation);

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateHeston2(
	const GsFuncScFunc& shift, 
	const GsFuncScFunc& beta, 
	const GsFuncScFunc& longvol, 
	const GsFuncScFunc& volvol,
	const GsFuncScFunc& correlation
);

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateHestonFromCurves(
	const GsCTimeCurve& shift, 
	const GsCTimeCurve& beta, 
	const GsCTimeCurve& longvol, 
	const GsCTimeCurve& volvol,
	const GsCTimeCurve& correlation
);

CC_END_NAMESPACE

#endif 
