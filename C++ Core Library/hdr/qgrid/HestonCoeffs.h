/****************************************************************
**
**	HestonCoeffs.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_HESTONCOEFFS_H )
#define IN_HESTONCOEFFS_H

#include	<qgrid/base.h>
#include	<qgrid/TimeSliceFn2D.h>
#include	<gsquant/GsFuncPiecewiseLinear.h>

CC_BEGIN_NAMESPACE( Gs )

/*****************************************************************
**  use equations
**  1)  dX = X * mu * dt + X * V * dW1
**  2)  dV = ( alpha / V - beta * V ) * dt + sigma * dW2
**
**			correlation between W1 and W2 is rho
**
**		if the equation in U = V * V satisfies
**		    dU = - beta1 * ( U - LongVol * LongVol ) * dt + sigma1 * sqrt( U ) * dW
**		then,
**          alpha = beta1 * LongVol * LongVol/2 - sigma1 * sigma1 / 8
**          beta = beta1 / 2
**          gamma = gamma1 / 2
**
******************************************************************/

class HestonVolCoeff : public TimeSliceFn2DRep
{
	public:
		virtual void operator()( double s, double t, double minx, double miny, double maxx, double maxy, int nGridx, int nGridy, GsVector& retvals ) const;
};

class DeterministicRate2D : public TimeSliceFn2DRep
{
	public:
		DeterministicRate2D( const GsFuncPiecewiseLinear& ForwardRates ) : m_ForwardRates( ForwardRates ) {}
		virtual void operator()( double s, double t, double minx, double miny, double maxx, double maxy, int nGridx, int nGridy, GsVector& retvals ) const;
	private:
		GsFuncPiecewiseLinear m_ForwardRates;
};

class HestonDrift : public DeterministicRate2D
{
	public:
		HestonDrift( const GsFuncPiecewiseLinear& ForwardRates ) : DeterministicRate2D( ForwardRates ) {}
};

class HestonDiscs : public DeterministicRate2D
{
	public:
		HestonDiscs( const GsFuncPiecewiseLinear& DiscountRates ) : DeterministicRate2D( DiscountRates ) {}
};

class HestonVolDrift : public TimeSliceFn2DRep
{
	public:
		HestonVolDrift( double LongVol, double beta, double sigma )
		:
			m_alpha( 0.5 * beta * LongVol * LongVol - 0.125 * sigma * sigma ),
			m_beta( 0.5 * beta )
		{}
		virtual void operator()( double s, double t, double minx, double miny, double maxx, double maxy, int nGridx, int nGridy, GsVector& retvals ) const;

	private:
		double	m_alpha;
		double	m_beta;
};

class HestonVolVol : public ConstantCoeff2D
{
	public:
		HestonVolVol( double sigma ) : ConstantCoeff2D( 0.25 * sigma * sigma ) {}
};

class HestonCorr : public ConstantCoeff2D
{
	public:
		HestonCorr( double rho ) : ConstantCoeff2D( rho ) {}
};

CC_END_NAMESPACE

#endif

