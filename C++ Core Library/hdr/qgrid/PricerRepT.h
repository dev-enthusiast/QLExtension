/****************************************************************
**
**	PricerRepT.h	- definition of templated class PricerRepT 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.13 $
**
****************************************************************/

#if !defined( IN_PRICERREPT_H )
#define IN_PRICERREPT_H

#include	<qgrid/FXHandle.h>
#include	<qgrid/PricerRep.h>
#include	<qgrid/BasePricer.h>
#include	<qgrid/KnockoutData.h>

CC_BEGIN_NAMESPACE( Gs )

template< class X > class PricerRepT : public PricerRep
{
	public:
		PricerRepT( const BasePricer< X >* pricer ) : m_pricer( pricer )
		{
			m_time = HUGE_VAL;
		}

		// constructor with initialised grid - must be careful that grid is valid
		PricerRepT( const BasePricer< X >* pricer, const X& grid, double time )
		: m_pricer( pricer ), m_grid( grid ), m_time( time ) {}

		PricerRepT( const PricerRepT< X >& rhs )
		:
			m_pricer( rhs.m_pricer ),
			m_grid( rhs.m_grid ),
			m_time( rhs.m_time ),
			m_KnockoutData( rhs.m_KnockoutData )
		{}

		~PricerRepT() {}

		void SetPayout( const GsFuncPiecewiseLinear& f, double Time );
		void SetPayout( double a, double Time );
		void StepBack( double Time );
		double Price() const;
		GsDt* Value() const;

		void AddUpperKO( double KOLevel, double StartTime, double EndTime,
                         double Rebate, double RebateTime );

		void AddLowerKO( double KOLevel, double StartTime, double EndTime,
                         double Rebate, double RebateTime );

		void RemoveKnockouts();

		void SetRebateTime( double t );

		void Max( const PricerRep& rhs );
		void Max( double rhs );
		void Max( const GsFuncPiecewiseLinear& f );
		void Min( const PricerRep& rhs );
		void Min( double rhs );
		void Min( const GsFuncPiecewiseLinear& f );

		const PricerRep& operator=( const PricerRep& rhs );
		const PricerRep& operator*=( double a );
		const PricerRep& operator*=( const GsFuncPiecewiseLinear& f );
		const PricerRep& operator+=( double a );
		const PricerRep& operator+=( const GsFuncPiecewiseLinear& f );
		const PricerRep& operator+=( const PricerRep& rhs );
		const PricerRep& operator-=( const PricerRep& rhs );
		const PricerRep& operator-=( const GsFuncPiecewiseLinear& f );
		void AddProduct( const PricerRep& rhspricer, const GsFuncPiecewiseLinear& f );

		PricerRep* Clone() const;
		GsString ModelDescription() const;
		double Time() const;
		GsDtDictionary* ToGsDt() const;
		intptr_t Type() const;

	private:
		FXHandleReadOnly< BasePricer< X > >	m_pricer;
		X									m_grid;
		double								m_time;
		KnockoutData						m_KnockoutData;

		const X& 	TestRHSValidity( const PricerRep& rhs ) const;
		void		TestValidity() const
		{
			if( m_time == HUGE_VAL )
				GSX_THROW( GsXInvalidArgument, "PricerRepT::TestValidity - grid time is not defined" );
		}
};

CC_END_NAMESPACE

#endif

