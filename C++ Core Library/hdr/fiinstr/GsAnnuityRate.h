/****************************************************************
**
**	GSANNUITYRATE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsAnnuityRate.h,v 1.1 2000/03/13 11:46:53 thompcl Exp $
**
****************************************************************/

#if !defined( IN_FIINSTR_GSANNUITYRATE_H )
#define IN_FIINSTR_GSANNUITYRATE_H

#include <fiinstr/base.h>
#include <gscore/types.h>
#include <gscore/GsDate.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gsrand/GsRandUniformRange.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIINSTR GsAnnuityRate 
{
	GsAnnuityRate() {};

public:

	typedef const GsDate&  domain_type;
	typedef double range_type;

	typedef GsFunc< domain_type, range_type > func_type;
	typedef GsFuncHandle< domain_type, range_type > handle_type;

	GsAnnuityRate( handle_type Func ) : m_Func( Func ) {};

	GsAnnuityRate( const GsAnnuityRate& Other ) : m_Func ( Other.m_Func ) {};
	virtual ~GsAnnuityRate() {};

	GsAnnuityRate& operator=( const GsAnnuityRate& Rhs )
	{
		if( this != &Rhs )
		{
			m_Func = Rhs.m_Func;
		}
		return *this;
	}

	range_type operator()( domain_type Arg ) const { return m_Func( Arg ); }

	typedef GsDtGeneric< GsAnnuityRate > GsDtType;

	static const char* typeName() { return "GsAnnuityRate"; }
	GsString toString() const { return "GsAnnuityRate"; }

protected:

	handle_type m_Func;
};



class GsAnnuityRateFixedFunc : public GsAnnuityRate::func_type
{
public:

	typedef GsAnnuityRate::range_type range_type;
	typedef GsAnnuityRate::domain_type domain_type;

	GsAnnuityRateFixedFunc( range_type Rate = 1.0 ) : m_Rate( Rate ) {};
	virtual range_type  operator() ( domain_type Arg ) const { return m_Rate; }

private:
	range_type m_Rate;
};

/**
 ** THIS CLASS IS FOR DEMONSTRATION OLNY
 **/

class GsAnnuityRateRandomFunc : public GsAnnuityRate::func_type
{
public:

	typedef GsAnnuityRate::range_type range_type;
	typedef GsAnnuityRate::domain_type domain_type;

	GsAnnuityRateRandomFunc( range_type From = 0.0, range_type To = 1.0 ) : m_From( From ), m_To( To ) {};
	virtual range_type  operator() ( domain_type Arg ) const 
	{
		GsRandUniformRange Rand( m_From, m_To, Arg.getJulianDayNumber() );
		return Rand();
	}


private:
	range_type
			m_From,
			m_To;
};



EXPORT_CPP_FIINSTR GsAnnuityRate
		*GsAnnuityRateFixed( double Rate ),
		*GsAnnuityRateRandom( double From , double To );

CC_END_NAMESPACE

#endif 
