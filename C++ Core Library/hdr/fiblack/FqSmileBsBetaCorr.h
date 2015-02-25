/****************************************************************
**
**	FqSmileBsBetaCorr.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqSmileBsBetaCorr.h,v 1.1 2001/06/22 19:08:37 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FIBLACK_FQSMILEBSBETACORR_H )
#define IN_FIBLACK_FQSMILEBSBETACORR_H

#include <fiblack/base.h>
#include <gscore/GsFuncLine.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsCTimeCurve.h>
#include <fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIBLACK FqSmileBsBetaCorr : public FqBlack
{
public:
	FqSmileBsBetaCorr( double beta=0.0, double corr=0.0, double volvol=0.0, double shift = 0.0 )
	: 	m_beta(		new GsFuncLine( beta	)), 
		m_corr(		new GsFuncLine( corr	)),
		m_volvol(	new GsFuncLine( volvol	)),
		m_shift(	new GsFuncLine( shift	))
	{
    }

	FqSmileBsBetaCorr(
				const GsFuncHandle<double,double>& beta,
				const GsFuncHandle<double,double>& corr,
				const GsFuncHandle<double,double>& volvol,
				const GsFuncHandle<double,double>& shift )
	:	m_beta(beta),
		m_corr(corr),
		m_volvol(volvol),
		m_shift(shift)
	{
	}

    virtual ~FqSmileBsBetaCorr();
	virtual FqBlack* copy() const;
	virtual GsString toString() const;
	virtual FqBlackType type() const;

	// overwrite the eqLognormVol from baseclass, because here we calc the eq lognorm vol explicitly
	virtual double eqLognormVol(Q_OPT_TYPE opttype, double strike, double term, 
						 double fwd, double sigma) const;

    void setBeta 		( const GsFuncHandle<double,double>& x ) 	{	m_beta 	= x;   	}
    GsFuncHandle<double,double> 	getBeta() const    				{	return m_beta;  }

	void setCorrelation	( const GsFuncHandle<double,double>& x )	{	m_corr	= x;   	}
	GsFuncHandle<double,double> 	getCorrelation() const			{	return m_corr;	}

    void setVolVol 		( const GsFuncHandle<double,double>& x )	{	m_volvol= x;	}
    GsFuncHandle<double,double> 	getVolVol() const				{	return m_volvol;}

    void setShift		( const GsFuncHandle<double,double>& x )	{	m_shift = x;	}
    GsFuncHandle<double,double> 	getShift() const				{	return m_shift; }

protected:

	virtual double cvalue(double strike, double term, double fwd, double sigma) const;
	// FIX: don't calc these things analytically yet
	//	virtual double cdelta(double strike, double term, double fwd, double sigma, double bump=0) const;
    
	//binary
	// FIX: this currently does nothing but throw an exception (is pure virtual, has to be implemented)
	virtual double cbinaryValue(double strike, double term, double fwd, double sigma) const;

	// for fitter
	virtual GsStringVector getParameterNames() const;
	virtual GsStatus setParameter( const GsString& paramName,  const GsFuncHandle<double,double>& param );
	virtual GsFuncHandle<double,double> getParameter( const GsString& paramName ) const;
	virtual bool hasParameter( const GsString& name ) const;

	double volGuess() const;  	

private:
	GsFuncHandle<double, double> m_beta;
	GsFuncHandle<double, double> m_corr;
	GsFuncHandle<double, double> m_volvol;
	GsFuncHandle<double, double> m_shift;
};

EXPORT_CPP_FIBLACK
	FqBlack* FqSmCreateBsBetaCorr(		double beta, double correlation, double volvol );

EXPORT_CPP_FIBLACK
	FqBlack* FqSmCreateBsBetaCorrShift(	double beta, double correlation, double volvol, double shift );

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateBsBetaCorrShiftFromCurves(
	const GsCTimeCurve& beta, 
	const GsCTimeCurve& correlation, 
	const GsCTimeCurve& volvol, 
	const GsCTimeCurve& shift
);

EXPORT_CPP_FIBLACK
FqBlack* FqSmCreateBsBetaCorrShift2(
	const GsFuncScFunc& beta, 
	const GsFuncScFunc& correlation, 
	const GsFuncScFunc& volvol, 
	const GsFuncScFunc& shift
);


CC_END_NAMESPACE

#endif
