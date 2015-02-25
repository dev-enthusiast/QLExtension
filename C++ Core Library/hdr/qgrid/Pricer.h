/****************************************************************
**
**	Pricer.h	- definition of class Pricer
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.30 $
**
****************************************************************/

#if !defined( IN_PRICER_H )
#define IN_PRICER_H

#include <qgrid/base.h>
#include <gscore/types.h>
#include <gscore/err.h>
#include <gscore/GsException.h>
#include <qgrid/PricerRep.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_QGRID Pricer
{
public:
	// classes to replace temporary Pricer
	// objects when taking linear combs
	class LinearComb;
	class Addition;
	class Subtraction;
	class PricerFnProd;
	class PricerDblProd;
	class SingleObject;

	// GsDt support
	typedef class GsDtPricer GsDtType;
	static const char *typeName() { return "Pricer"; }

	inline Pricer();
	inline Pricer( PricerRep* Pricer );
	inline Pricer( const Pricer& rhs );
	inline ~Pricer();

	const Pricer& operator=( const Pricer& rhs );

	inline void SetPayout( const GsFuncPiecewiseLinear& f, double Time );
	inline void SetPayout( double a, double Time );
	
	inline void StepBack( double Time );

	inline double Price() const;
	inline GsDt* Value() const;

	inline void AddLowerKO( double KOLevel, double StartTime, double EndTime,
							double Rebate, double RebateTime );
	inline void AddUpperKO( double KOLevel, double StartTime, double EndTime,
							double Rebate, double RebateTime );
	inline void RemoveKnockouts();
	inline void SetRebateTime( double t );

	inline GsString ModelDescription() const;

	// exported member functions avoiding overloading by using GsDt
	void SetPayout(
		const GsDt&	f,		// the payout function
		double 		time	// (double) time to step back to
	);
	void Max(
		const GsDt& rhs		// grid/double/fn to max with
	);

	void Min(
		const GsDt& rhs		// grid/double/fn to min with
	);

	// operators
	inline const Pricer& operator+=( const  Pricer& rhs );
	inline const Pricer& operator+=( double a );
	inline const Pricer& operator+=( const  GsFuncPiecewiseLinear& f );
	inline const Pricer& operator-=( const  Pricer& rhs );
	inline const Pricer& operator-=( double a );
	inline const Pricer& operator-=( const  GsFuncPiecewiseLinear& f );
	inline const Pricer& operator*=( double a );
	inline const Pricer& operator*=( const GsFuncPiecewiseLinear& f );
	inline const Pricer& operator/=( double a );

	inline void Max( const Pricer& rhs );
	inline void Max( double a );
	inline void Max( const GsFuncPiecewiseLinear& f );
	inline void Min( const Pricer& rhs );
	inline void Min( double a );
	inline void Min( const GsFuncPiecewiseLinear& f );

	inline double Time() const;

	inline GsDtDictionary* ToGsDt() const;
	GsDt& Grid() const;

	const Pricer& operator+=( const LinearComb& rhs );
	const Pricer& operator-=( const LinearComb& rhs );
	const Pricer& operator=( const LinearComb& rhs );
	inline Pricer( const LinearComb& rhs );


private:
	PricerRep*	m_pricer;

	void construct( const LinearComb& rhs );

	friend class LinearComb;
	friend class Addition;
	friend class Subtraction;
	friend class PricerFnProd;
	friend class PricerDblProd;
	friend class SingleObject;

public:

	// enum for some possible ways of manipulating pricers
	enum AlgebraicElement
	{
		ADDITION,		 // an addition
		SUBTRACTION,	 // a subtraction
		PRICER_FN_PROD,	 // function * pricer
		PRICER_DBL_PROD, // double * pricer
		SINGLE_PRICER,	 // just a pricer
		SINGLE_FN,		 // just a function
		SINGLE_DBL,		 // just a double
		LINEAR_COMB		 // a linear combination
	};

	/**********************************************************************
	**  Pricer::LinearComb is a class representing linear combinations of
	**		Pricers and functions. Idea is that if we do something like
	**		Pricer1 = Pricer2 * f + Pricer3 * g, then expensive temporary
	**		objects are avoided by implicitly converting it to
	**		Pricer1 = Pricer2;
	**		Pricer1 *= f;
	**		Pricer1.m_pricer->AddProduct( Pricer3, g );
	**		similar behaviour for += and -=
	**********************************************************************/

	// classes to avoid temp pricer objects when taking linear combinations
	class LinearComb
	{
	protected:
		inline LinearComb( AlgebraicElement Elem ) : Element( Elem ) {}

		AlgebraicElement Element;
		friend class Pricer;
	};

	class Addition : public LinearComb
	{
	private:
		inline Addition( const LinearComb& lhs, const LinearComb& rhs )
		: LinearComb( ADDITION ), LHS( &lhs ), RHS( &rhs ) {}

		const LinearComb* LHS;
		const LinearComb* RHS;
		friend class Pricer;

		friend Addition operator+( const SingleObject& lhs, const SingleObject& rhs );
	};

	class Subtraction : public LinearComb
	{
	private:
		inline Subtraction( const LinearComb& lhs, const LinearComb& rhs )
		: LinearComb( SUBTRACTION ), LHS( &lhs ), RHS( &rhs ) {}

		const LinearComb* LHS;
		const LinearComb* RHS;
		friend class Pricer;
		friend Subtraction operator-( const SingleObject& lhs, const SingleObject& rhs );
	};

	class PricerFnProd : public LinearComb
	{
	private:
		inline PricerFnProd( const Pricer& lhs, const GsFuncPiecewiseLinear& rhs )
		: LinearComb( PRICER_FN_PROD ), LHS( lhs.m_pricer ), RHS( &rhs ) {}

		const PricerRep*         LHS;
		const GsFuncPiecewiseLinear* RHS;
		friend class Pricer;

		friend PricerFnProd operator*( const Pricer& lhs, const GsFuncPiecewiseLinear& rhs );
	};

	class PricerDblProd : public LinearComb
	{
	private:
		inline PricerDblProd( const Pricer& lhs, double rhs )
		: LinearComb( PRICER_DBL_PROD ), LHS( lhs.m_pricer ), RHS( rhs ) {}

		const PricerRep* LHS;
		double           RHS;
		friend class Pricer;
		friend PricerDblProd operator*( const Pricer& lhs, double rhs );
	};

	class SingleObject : public LinearComb
	{
	public:
		inline SingleObject( const Pricer& pricer )
		: LinearComb( SINGLE_PRICER ), Ptr( static_cast<const void*>( const_cast<PricerRep*>( pricer.m_pricer ))) {}

		inline SingleObject( const GsFuncPiecewiseLinear& f )
		: LinearComb( SINGLE_FN ), Ptr( static_cast<const void*>( &f )) {}

		inline SingleObject( const double& a )
		: LinearComb( SINGLE_DBL ), Ptr( static_cast<const void*>( &a )) {}

		inline SingleObject( const LinearComb& comb )
		: LinearComb( LINEAR_COMB ), Ptr( static_cast<const void*>( &comb )) {}

	private:
		const void* Ptr;
		friend class Pricer;
	};
};

/****************************************************************
**	Class:   Pricer
**	Routine: Pricer
**	Returns: 
**	Action : default c'tor
****************************************************************/

Pricer::Pricer()
:
	m_pricer( 0 )
{}

/****************************************************************
**	Class:   Pricer
**	Routine: Pricer
**	Returns: 
**	Action : c'tor from PricerRep
****************************************************************/

Pricer::Pricer( PricerRep* Pricer )
:
	m_pricer( Pricer )
{}

/****************************************************************
**	Class:   Pricer
**	Routine: Pricer
**	Returns: 
**	Action : copy c'tor
****************************************************************/

Pricer::Pricer( const Pricer& rhs )
:
	m_pricer( rhs.m_pricer->Clone() )
{}

Pricer::Pricer( const LinearComb& rhs )
{
	construct( rhs );
}

/****************************************************************
**	Class:   Pricer
**	Routine: ~Pricer
**	Returns: 
**	Action : destructor
****************************************************************/

Pricer::~Pricer()
{
	delete m_pricer;
}

/****************************************************************
**	Class:   Pricer
**	Routine: SetPayout
**	Returns: 
**	Action : sets a payout on the grid
****************************************************************/

void Pricer::SetPayout( const GsFuncPiecewiseLinear& f, double Time )
{
	m_pricer->SetPayout( f, Time );
}

/****************************************************************
**	Class:   Pricer
**	Routine: SetPayout
**	Returns: 
**	Action : sets a payout on the grid
****************************************************************/

void Pricer::SetPayout( double a, double Time )
{
	m_pricer->SetPayout( a, Time );
}

/****************************************************************
**	Adlib Name: Pricer::StepBack
**	Summary:		
**	References:		
**	Description: Steps grid back to time 'time'
**	Example:		
****************************************************************/

void Pricer::StepBack(
	double Time		// time to step back to
)
{
	m_pricer->StepBack( Time );
}

/****************************************************************
**	Class:   Pricer
**	Routine: AddLowerKO
**	Returns: 
**	Action : sets time of rebate payments
****************************************************************/

void Pricer::SetRebateTime( double t )
{
	m_pricer->SetRebateTime( t );
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator+=
**	Returns: 
**	Action :
****************************************************************/

const Pricer& Pricer::operator+=( const Pricer& rhs )
{
	*m_pricer += *rhs.m_pricer;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator+=
**	Returns: 
**	Action : 
****************************************************************/

const Pricer& Pricer::operator+=( double a )
{
	*m_pricer += a;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator+=
**	Returns: 
**	Action : 
****************************************************************/

const Pricer& Pricer::operator+=( const GsFuncPiecewiseLinear& f )
{
	*m_pricer += f;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator-=
**	Returns: 
**	Action : 
****************************************************************/

const Pricer& Pricer::operator-=( const Pricer& rhs )
{
	*m_pricer -= *rhs.m_pricer;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator-=
**	Returns: 
**	Action : 
****************************************************************/

const Pricer& Pricer::operator-=( double a )
{
	*m_pricer += -a;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator-=
**	Returns: 
**	Action : 
****************************************************************/

const Pricer& Pricer::operator-=( const GsFuncPiecewiseLinear& f )
{
	*m_pricer -= f;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator*=
**	Returns: 
**	Action : 
****************************************************************/

const Pricer& Pricer::operator*=( double a )
{
	*m_pricer *= a;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator*=
**	Returns: 
**	Action : 
****************************************************************/

const Pricer& Pricer::operator*=( const GsFuncPiecewiseLinear& f )
{
	*m_pricer *= f;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator/=
**	Returns: 
**	Action : 
****************************************************************/

const Pricer& Pricer::operator/=( double a )
{
	*m_pricer *= 1./a;
	return *this;
}

/****************************************************************
**	Class:   Pricer
**	Routine: Max
**	Returns: 
**	Action : gets max of 2 pricers
****************************************************************/

void Pricer::Max( const Pricer& rhs )
{
	m_pricer->Max( *rhs.m_pricer );
}

/****************************************************************
**	Class:   Pricer
**	Routine: Max
**	Returns: 
**	Action : gets max of pricer with double
****************************************************************/

void Pricer::Max( double a )
{
	m_pricer->Max( a );
}

/****************************************************************
**	Class:   Pricer
**	Routine: Max
**	Returns: 
**	Action : gets max of pricer with function
****************************************************************/

void Pricer::Max( const GsFuncPiecewiseLinear& f )
{
	m_pricer->Max( f );
}

/****************************************************************
**	Class:   Pricer
**	Routine: Min
**	Returns: 
**	Action : gets min of 2 pricers
****************************************************************/

void Pricer::Min( const Pricer& rhs )
{
	m_pricer->Min( *rhs.m_pricer );
}

/****************************************************************
**	Class:   Pricer
**	Routine: Min
**	Returns: 
**	Action : gets min of pricer with double
****************************************************************/

void Pricer::Min( double a )
{
	m_pricer->Min( a );
}

/****************************************************************
**	Class:   Pricer
**	Routine: Min
**	Returns: 
**	Action : gets min of pricer with function
****************************************************************/

void Pricer::Min( const GsFuncPiecewiseLinear& f )
{
	m_pricer->Min( f );
}

/****************************************************************
**	Adlib Name: Pricer::ToGsDt
**	Summary:
**	References:		
**	Description: converts pricer to structure
**	Example:		
****************************************************************/

GsDtDictionary* Pricer::ToGsDt(
)
	const
{
	return m_pricer->ToGsDt();
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator*
**	Returns: 
**	Action : pricer * fn converted to PricerFnProd
****************************************************************/

inline Pricer::PricerFnProd operator*( const Pricer& lhs, const GsFuncPiecewiseLinear& rhs )
{
	return Pricer::PricerFnProd( lhs, rhs );
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator*
**	Returns: 
**	Action : pricer * double converted to Pricer::PricerDblProd
****************************************************************/

inline Pricer::PricerDblProd operator*( const Pricer& lhs, double rhs )
{
	return Pricer::PricerDblProd( lhs, rhs );
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator+
**	Returns: 
**	Action : a + b converted to Pricer::Addition
****************************************************************/

inline Pricer::Addition operator+( const Pricer::SingleObject& lhs, const Pricer::SingleObject& rhs )
{
	return Pricer::Addition( lhs, rhs );
}

/****************************************************************
**	Class:   Pricer
**	Routine: operator-
**	Returns: 
**	Action : a - b converted to Pricer::Subtraction
****************************************************************/

inline Pricer::Subtraction operator-( const Pricer::SingleObject& lhs, const Pricer::SingleObject& rhs )
{
	return Pricer::Subtraction( lhs, rhs );
}

/****************************************************************
**	Adlib Name: Pricer::Price
**	Summary:
**	References:		
**	Description: gets price from the grid
**	Example:		
****************************************************************/

double Pricer::Price(
)
	const
{
	return m_pricer->Price();
}

/****************************************************************
**	Adlib Name: Pricer::Value
**	Summary:
**	References:		
**	Description: gets price/value function from the grid
**	Example:		
****************************************************************/

GsDt* Pricer::Value(
)
	const
{
	return m_pricer->Value();
}


/****************************************************************
**	Adlib Name: Pricer::AddLowerKO
**	Summary:
**	References:		
**	Description: adds a lower knockout
**	Example:
****************************************************************/

void Pricer::AddLowerKO(
	double KOLevel,		// knockout price
	double StartTime,	// time to start of KO period
	double EndTime,     // time to end of KO period
    double Rebate,      // rebate
	double RebateTime   // time when rebate is payed
)
{
	m_pricer->AddLowerKO( KOLevel, StartTime, EndTime, Rebate, RebateTime );
}

/****************************************************************
**	Adlib Name: Pricer::AddUpperKO
**	Summary:
**	References:		
**	Description: adds a upper knockout
**	Example:
****************************************************************/

void Pricer::AddUpperKO(
	double KOLevel,		// knockout price
	double StartTime,	// time to start of KO period
	double EndTime,     // time to end of KO period
	double Rebate,      // rebate
	double RebateTime   // time when rebate is payed
)
{
	m_pricer->AddUpperKO( KOLevel, StartTime, EndTime, Rebate, RebateTime );
}

/****************************************************************
**	Adlib Name: Pricer::RemoveKnockouts
**	Summary:
**	References:		
**	Description: removes all knockouts
**	Example:
****************************************************************/

void Pricer::RemoveKnockouts(
)
{
	m_pricer->RemoveKnockouts();
}

/****************************************************************
**	Adlib Name: Pricer::ModelDescription
**	Summary:
**	References:		
**	Description: describes underlying model
**	Example:
****************************************************************/

GsString Pricer::ModelDescription(
)
	const
{
	return m_pricer->ModelDescription();
}

/****************************************************************
**	Adlib Name: Pricer::Time
**	Summary:
**	References:		
**	Description: gets time of pricer
**	Example:
****************************************************************/

double Pricer::Time(
)
	const
{
	return m_pricer->Time();
}

CC_END_NAMESPACE

#endif

