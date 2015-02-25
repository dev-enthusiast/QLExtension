/****************************************************************
**
**	GsDtPricer.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_GSDTPRICER_H )
#define IN_GSDTPRICER_H

#if !defined( IN_PRICER_H )
#include <qgrid/Pricer.h>
#endif
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_QGRID GsDtPricer : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtPricer )

	public:
		GsDtPricer() : m_data( 0 ) {}
		GsDtPricer( Pricer* data ) : m_data( data ) {}
		GsDtPricer( const Pricer& rhs ) : m_data( new Pricer( rhs )) {}
		GsDtPricer( const GsDtPricer& rhs ) : m_data( new Pricer( *rhs.m_data )) {}
		~GsDtPricer() { delete m_data; }

		inline const GsDtPricer& operator=( const GsDtPricer& rhs );

		virtual GsDt*		CopyDeep() const;
		virtual GsDt*		CopyShallow() const;

		GsString toString() const;
		inline static const char* typeName() { return "GsDtPricer"; }

		const Pricer& data() const { return *m_data; }
		Pricer& data() { return *m_data; }
		GsDtPricer( Pricer* NewData, GsCopyAction Action );
		inline Pricer* stealData();

		virtual GsStatus	BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );

		// the prefix used for generated member function wrappers
		virtual CC_NS(std,string) memberFunctionWrapperPrefix() const;

		inline const GsDtPricer& operator+=( double a );
		inline const GsDtPricer& operator-=( double a );
		inline const GsDtPricer& operator*=( double a );
		inline const GsDtPricer& operator/=( double a );

		inline const GsDtPricer& operator+=( const GsFuncPiecewiseLinear& f );
		inline const GsDtPricer& operator-=( const GsFuncPiecewiseLinear& f );
		inline const GsDtPricer& operator*=( const GsFuncPiecewiseLinear& f );
		inline const GsDtPricer& operator/=( const GsFuncPiecewiseLinear& f );

		GsDt* Min( const GsDt& rhs ) const;
		GsDt* Max( const GsDt& rhs ) const;

	private:
		Pricer*		m_data;
};

const GsDtPricer& GsDtPricer::operator=( const GsDtPricer& rhs )
{
	if( this != &rhs )
	{
		if( m_data )
			*m_data = *rhs.m_data;
		else
			m_data = new Pricer( *rhs.m_data );
	}
	return *this;
}

Pricer* GsDtPricer::stealData()
{
	Pricer* tmp = m_data;
	m_data = 0;
	return tmp;
}

const GsDtPricer& GsDtPricer::operator+=( double a )
{
	*m_data += a;
	return *this;
}

const GsDtPricer& GsDtPricer::operator-=( double a )
{
	*m_data -= a;
	return *this;
}

const GsDtPricer& GsDtPricer::operator*=( double a )
{
	*m_data *= a;
	return *this;
}

const GsDtPricer& GsDtPricer::operator/=( double a )
{
	*m_data /= a;
	return *this;
}

const GsDtPricer& GsDtPricer::operator+=( const GsFuncPiecewiseLinear& f )
{
	*m_data += f;
	return *this;
}

const GsDtPricer& GsDtPricer::operator-=( const GsFuncPiecewiseLinear& f )
{
	*m_data -= f;
	return *this;
}

const GsDtPricer& GsDtPricer::operator*=( const GsFuncPiecewiseLinear& f )
{
	*m_data *= f;
	return *this;
}

const GsDtPricer& GsDtPricer::operator/=( const GsFuncPiecewiseLinear& f )
{
	GSX_THROW( GsXInvalidOperation, "Cannot divide a pricer by a GsFuncPiecewiseLinear" );
	return *this;
}

CC_END_NAMESPACE

#endif

