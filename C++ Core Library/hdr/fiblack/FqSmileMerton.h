/****************************************************************
**
**	FqSmileMerton.H	- 
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqSmileMerton.h,v 1.1 2001/06/22 19:08:43 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FIBLACK_FQSMILEMERTON_H )
#define IN_FIBLACK_FQSMILEMERTON_H

#include <fiblack/base.h>
#include <gscore/GsFuncLine.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsCTimeCurve.h>
#include <fiblack/FqBlack.h>
#include <gsquant/transdistribution.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqSmileMerton : public FqBlack
{
public:
	FqSmileMerton( double intensity=0.0, double gamma=0.0, double deltasq=0.0 )
	: 	m_intensity(new GsFuncLine(intensity)), 
		m_gamma(new GsFuncLine(gamma)), 
		m_deltasq(new GsFuncLine(deltasq))
	{
	}
	
	FqSmileMerton(const GsFuncHandle<double,double>& intensity,
				  const GsFuncHandle<double,double>& gamma,
				  const GsFuncHandle<double,double>& deltasq )
	:	m_intensity(intensity),
		m_gamma(gamma),
		m_deltasq(deltasq)
	{
    }
    
	virtual ~FqSmileMerton();
	virtual FqBlack* copy() const;
	virtual GsString toString() const;
	virtual FqBlackType type() const;

	void setintensity (const GsFuncHandle<double,double>& a)
	{
		m_intensity=a;
	}
	GsFuncHandle<double,double> getIntensity() const
	{
		return m_intensity;
	}
    void setgamma (const GsFuncHandle<double,double>& c)
    {
		m_gamma=c;
    }
    GsFuncHandle<double,double> getGamma() const
    {
		return m_gamma;
    }
    void setdeltasq (const GsFuncHandle<double,double>& d)
    {
		m_deltasq=d;
    }
    GsFuncHandle<double,double> getDeltasq() const
    {
		return m_deltasq;
    }


protected:
	virtual double CalcBsTermMerton( double strike, double term, double fwd, double vol, double intensity, double gamma, double deltasq, int N) const;
	virtual double cvalue(double strike, double term, double fwd, double sigma) const;
//	virtual double cdelta(double strike, double term, double fwd, double sigma, double bump=0) const;
    //binary
    virtual double cbinaryValue(double strike, double term, double fwd, double sigma) const;

	// for fitter
	virtual GsStringVector getParameterNames() const;
	virtual GsStatus setParameter( const GsString& paramName,  const GsFuncHandle<double,double>& param );
	virtual GsFuncHandle<double,double> getParameter( const GsString& paramName ) const;
	virtual bool hasParameter( const GsString& name ) const;

	double volGuess() const;  	


private:

	GsFuncHandle<double, double> m_intensity;
	GsFuncHandle<double, double> m_gamma;
	GsFuncHandle<double, double> m_deltasq;

};

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateMerton(double intensity, double gamma, double deltasq );

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateMerton2(
	const GsFuncScFunc& intensity, 
	const GsFuncScFunc& gamma, 
	const GsFuncScFunc& deltasq
);

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateMertonFromCurves(
	const GsCTimeCurve& intensity, 
	const GsCTimeCurve& gamma, 
	const GsCTimeCurve& deltasq
);

CC_END_NAMESPACE

#endif 
