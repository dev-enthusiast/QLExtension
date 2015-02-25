/****************************************************************
**
**	GSFUNCPIECEWISEPOLY.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncPiecewisePoly.h,v 1.8 2001/02/16 00:08:10 tsoucp Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCPIECEWISEPOLY_H )
#define IN_GSQUANT_GSFUNCPIECEWISEPOLY_H

#include <gsquant/base.h>

#include <gsquant/GsFuncSpecial.h>

#include <gscore/GsFunc.h>
#include <gscore/GsVector.h>
#include <gscore/GsMatrix.h>
#include <gscore/GsStreamMap.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsFuncPiecewisePoly 
** Description : If knots := t[0], t[1], ..., t[N],
**                  d     := degree of the polynomial pieces,
**                  coeffs:= c[0],  ... ,  c[d],
** 					         c[d+1], ... , c[d+1+d], 
**                           c[2*(d+1)], ... , c[2*(d+1)+d],
**                           ...
**                           c[i*(d+1)], ... , c[i*(d+1)+d], 
**                           ...
**                           c[(N-1)*(d+1)], ..., c[(N-1)*(d+1)+d],
**               then f(x):= c[i*(d+1)] + 
**                           c[i*(d+1)+1]*(t-t[i])+
**                           ...
**                           c[i*(d+1)+d]*(t-t[i])^d,
**                           for t[i]<=t<t[i+1], i = 0, ..., N-1.
****************************************************************/

class EXPORT_CLASS_GSQUANT GsFuncPiecewisePoly : public GsFunc<double, double>
{
public:
	GsFuncPiecewisePoly() {};
	GsFuncPiecewisePoly( const GsVector& knots,
						 const GsVector& coefficients,
						 bool isRightClosed = FALSE);

	// .... FIX: get rid of this when () is properly implemented on GsFunc ...
	GsFuncPiecewisePoly( const GsFuncPiecewisePoly& other );
	virtual ~GsFuncPiecewisePoly() {}
	
	virtual double operator() ( double x ) const;

	unsigned getDegree() const { return m_degree; }
	unsigned getOrder() const { return m_order; }
	GsVector getCoefficients() const { return m_coefficients; }
	GsVector getKnots() const { return m_knots; }
	bool isRightClosed() const { return m_isRightClosed; }

	GsFuncPiecewisePoly integral( double initialValue ) const;
	GsFunc< double, double >* Integrate( double initialValue ) const;

	virtual GsFunc<double, double> *Square(  ) const;

  	typedef GsStreamMapT< GsFuncPiecewisePoly > GsStType;

  	virtual GsStreamMap* buildStreamMapper() const;

	// FIX: can speed this up some more ....
	static double horner( const GsVector& coefficients,
						  double x, 
						  long intervalIndex, 
						  unsigned order );
protected:
	long findKnotIntervalIndex( double x ) const;
	static long coefficientIndex( long intervalIndex, 
								  unsigned order,
								  unsigned degreeOfCoeff )
	{ return intervalIndex * order + degreeOfCoeff; }

	GsVector integralCoefficients( double initialValue ) const;
	GsVector squaredCoefficients ( ) const;

	// .... data members ....
protected:
	const GsVector m_knots;
	GsVector m_coefficients;
	unsigned m_degree;
	unsigned m_order;
	mutable long m_mruKnotIndex;

	const static double m_tolerance; // general purpose small number 
	bool m_isRightClosed;
public: // .... 
	inline void setCoefficient( unsigned interval, unsigned i, double p );
};

// .... Inline ....
void GsFuncPiecewisePoly::setCoefficient( unsigned interval, 
										  unsigned i, 
										  double p )
{
	m_coefficients[ interval * m_order + i ] = p;
}

class EXPORT_CLASS_GSQUANT GsFuncPiecewisePolyGen : public  
		GsFunc< const GsVector&, GsFuncPiecewisePoly >
{
public:
	GsFuncPiecewisePolyGen( const GsVector& knots ) : m_knots( knots ) {}
	virtual ~GsFuncPiecewisePolyGen() {}

	virtual GsFuncPiecewisePoly operator() ( const GsVector&  p ) const
	{ 
		return GsFuncPiecewisePoly( m_knots, p );
	}

protected:
	const GsVector m_knots;
};

class EXPORT_CLASS_GSQUANT GsFuncPiecewisePolyGenConverter :
 	public GsFunc< const GsVector&, GsFuncHandle< double, double > >
{
public:
	GsFuncPiecewisePolyGenConverter( 
		const GsFuncHandle< const GsVector&, GsFuncPiecewisePoly >& h ): 
	m_gen( h ) {}
	virtual ~GsFuncPiecewisePolyGenConverter() {}

	virtual GsFuncHandle< double, double >
			operator()( const GsVector& c ) const
	{
		return 
				GsFuncHandle< double, double >(
					new GsFuncPiecewisePoly( m_gen( c ) ) );
	}

protected:
	GsFuncHandle< const GsVector&, GsFuncPiecewisePoly > m_gen;
};

// ... Addins ....
// .... FIX: Unfortunate choice of name here. Use the one below instead ....
EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncScFuncCreatePiecewisePoly( const GsVector& knots, 
											  const GsMatrix& coeffs );
// .........................................................................

EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncPiecewisePolyCreate( const GsVector& knots, 
										const GsMatrix& coeffs );
EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncPiecewisePolyCreateFromVec( const GsVector& knots, 
											   const GsVector& coeffs );

CC_END_NAMESPACE

#endif 











