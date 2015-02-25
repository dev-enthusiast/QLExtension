/****************************************************************
**
**	PricerRep.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_PRICERREP_H )
#define IN_PRICERREP_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gsquant/GsFuncPiecewiseLinear.h>
#include	<gsdt/GsDtDictionary.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_QGRID PricerRep
{
	public:
		virtual void SetPayout( const GsFuncPiecewiseLinear& f, double Time ) = 0;
		virtual void SetPayout( double a, double Time ) = 0;
		virtual void StepBack( double Time ) = 0;
		virtual double Price() const = 0;
		virtual GsDt* Value() const = 0;
		virtual void AddLowerKO( double KOLevel, double StartTime, double EndTime,
								 double Rebate, double RebateTime ) = 0;
		virtual void AddUpperKO( double KOLevel, double StartTime, double EndTime,
								 double Rebate, double RebateTime ) = 0;
		virtual void RemoveKnockouts() = 0;
		virtual void SetRebateTime( double t ) = 0;
		virtual const PricerRep& operator=( const PricerRep& rhs ) = 0;
		virtual const PricerRep& operator*=( double a ) = 0;
		virtual const PricerRep& operator*=( const GsFuncPiecewiseLinear& f ) = 0;
		virtual const PricerRep& operator+=( const PricerRep& rhs ) = 0;
		virtual const PricerRep& operator+=( double a ) = 0;
		virtual const PricerRep& operator+=( const GsFuncPiecewiseLinear& f ) = 0;
		virtual const PricerRep& operator-=( const PricerRep& rhs ) = 0;
		virtual const PricerRep& operator-=( const GsFuncPiecewiseLinear& f ) = 0;
		virtual void AddProduct( const PricerRep& rhspricer, const GsFuncPiecewiseLinear& f ) = 0;
		virtual void Max( const PricerRep& rhs ) = 0;
		virtual void Max( double rhs ) = 0;
		virtual void Max( const GsFuncPiecewiseLinear& f ) = 0;
		virtual void Min( const PricerRep& rhs ) = 0;
		virtual void Min( double rhs ) = 0;
		virtual void Min( const GsFuncPiecewiseLinear& f ) = 0;
		virtual GsString ModelDescription() const { return GsString( "No description" ); }
		virtual double Time() const = 0;
		virtual GsDtDictionary* ToGsDt() const = 0;
		virtual PricerRep* Clone() const = 0;
		virtual intptr_t Type() const = 0;
		virtual ~PricerRep() {};
};

CC_END_NAMESPACE

#endif

