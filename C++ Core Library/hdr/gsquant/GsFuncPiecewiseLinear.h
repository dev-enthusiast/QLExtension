/****************************************************************
**
**	GsFuncPiecewiseLinear.h	- Piecewise linear function
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.16 $
**
****************************************************************/

#ifndef IN_GSFUNCPIECEWISELINEAR_H
#define IN_GSFUNCPIECEWISELINEAR_H

#include <gsquant/base.h>
#include <qenums.h>
#include <gscore/GsFunc.h>
#include <gscore/types.h>
#include <gscore/GsVector.h>
#include <gsdt/GsDtVector.h>
#include <gsdt/GsDtArray.h>
#include <gsdt/GsDtDouble.h>
#include <gsquant/GsFuncSpecial.h>
#include <gsdt/GsDtDictionary.h>
#include <n_consts.h>

#include <vector>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSQUANT GsFuncPiecewiseLinear : public GsFunc<double, double>
{
public:
	typedef GsDtGeneric<GsFuncPiecewiseLinear> GsDtType;
	GsString toString() const;
	inline static const char* typeName() { return "GsFuncPiecewiseLinear"; }

	typedef CC_STL_VECTOR( double ) dvector_t;

	inline GsFuncPiecewiseLinear();
	inline GsFuncPiecewiseLinear( double LowGradient, double HighGradient,
							      const double* XValues, const double* YValues,
                                  int nValues );

	inline GsFuncPiecewiseLinear( double LowGradient, double HighGradient,
                                  const dvector_t& XValues, const dvector_t& YValues );

	inline GsFuncPiecewiseLinear( double LowGradient, double HighGradient,
                                  const GsVector& XValues, const GsVector& YValues );

	inline GsFuncPiecewiseLinear( const GsFuncPiecewiseLinear& rhs );

	GsFuncPiecewiseLinear( Q_OPT_TYPE OptionType, double Strike );

	GsFuncPiecewiseLinear( double LowPrice, double HighPrice, double LowAmount, double HighAmount );

	GsFuncPiecewiseLinear( const GsDtDictionary& dict ) { FromDictionary( dict ); }
	GsFuncPiecewiseLinear( const GsDtArray& Arr ) { FromArray( Arr ); }

	virtual ~GsFuncPiecewiseLinear() {}

	inline const GsFuncPiecewiseLinear&	operator=( const GsFuncPiecewiseLinear& f );
	const GsFuncPiecewiseLinear& operator=( double x );
	double 			operator()( double x ) const;
	void			operator()( const double* x, double* y, int nValues ) const;
	GsVector		operator()( const GsVector& XVals) const;
    double 			Evaluate( double x, int& k ) const;

	const GsFuncPiecewiseLinear&	operator+=( double rhs );
	const GsFuncPiecewiseLinear&	operator*=( double rhs );
	inline const GsFuncPiecewiseLinear	operator+( double rhs ) const;
	inline const GsFuncPiecewiseLinear	operator*( double rhs ) const;
	inline const GsFuncPiecewiseLinear&	operator-=( double rhs );
	inline const GsFuncPiecewiseLinear&	operator/=( double rhs );
	inline const GsFuncPiecewiseLinear	operator-( double rhs ) const; 
	inline const GsFuncPiecewiseLinear	operator/( double rhs ) const; 

	inline const dvector_t& XValues() const { return m_XValues; }
	inline const dvector_t& YValues() const { return m_YValues; }
	inline dvector_t& XValues() { return m_XValues; }
	inline dvector_t& YValues() { return m_YValues; }
	inline double LowGradient() const { return m_LowGradient; }
	inline double HighGradient() const { return m_HighGradient; }

	void Resize( int n );

	bool		Increasing() const;			// returns true/false
	bool		Positive() const;			// returns true/false
	bool		StrictlyPositive() const;	// returns true/false
	bool		Linear() const;   			// returns true/false
	bool		Constant() const;   		// returns true/false

	inline int				Size() const { return m_XValues.size(); }
	inline double*			BeginX() { return &m_XValues[0]; }
	inline const double*	BeginX() const { return &m_XValues[0]; }
	inline double*			BeginY() { return &m_YValues[0]; }
	inline const double*	BeginY() const { return &m_YValues[0]; }

	double			LastX() const { return *(m_XValues.end()-1); }

	void			SetLowGradient( double LowGradient ) { m_LowGradient = LowGradient; }
	void			SetHighGradient( double HighGradient ) { m_HighGradient = HighGradient; }

	double LinearBasisIntegrate( double x0, double x1, double x2 ) const;
	double LinearBasisIntegrate( double x0, double x1, double x2, int LeftKnot, int& NewLeftKnot ) const;
	double ConstantBasisIntegrate( double x0, double x1 ) const;
	double ConstantBasisIntegrate( double x0, double x1, int LeftKnot, int& NewLeftKnot ) const;

	GsDtDictionary* ToDictionary() const;

/*
**	FromGsDtDictionary: converts from a GsDtDictionary. The Dictionary must contain either of
**	the following sets of tags
**
**	i) 
**		X Values      (GsDtVector)
**		Y Values      (GsDtVector)
**		Low Gradient  (GsDtDouble) optional arg, 0 by default
**		High Gradient (GsDtDouble) optional arg, 0 by default
**		Quantity	  (GsDtDouble) optional arg, 1 by default 
**
**  or ii)
**		Option Type   (Q_OPT_TYPE)
**		Strike		  (GsDtDouble) only if Option Type != Q_OPT_CONSTANT
**		Quantity	  (GsDtDouble) optional arg, 1 by default 
**
**  or iii)
**		Option Type   (GsDtString) "call", "put", "forward", "forward buy", "forward sell",
**									"digital call", "digital put", "constant"
**		Strike		  (GsDtDouble)  only if Option Type != "constant"
**		Quantity	  (GsDtDouble) optional arg, 1 by default 
**
**  or iv)
**      Option Type   (GsDtString) "spread", "bull spread"
**		Low Strike	  (GsDtDouble)
**		High Strike	  (GsDtDouble)
**		Low Amount	  (GsDtDouble) optional arg, 0 by default 
**		High Amount	  (GsDtDouble) optional arg, 1 by default 
**		Quantity	  (GsDtDouble) optional arg, 1 by default 
**
**  or v)
**      Option Type   (GsDtString) "bear spread"
**		Low Strike	  (GsDtDouble)
**		High Strike	  (GsDtDouble)
**		Low Amount	  (GsDtDouble) optional arg, 1 by default 
**		High Amount	  (GsDtDouble) optional arg, 0 by default 
**		Quantity	  (GsDtDouble) optional arg, 1 by default
**
*/
	void FromDictionary( const GsDtDictionary& dict );

	GsDtArray* ToArray() const;
/*
**	FromArray: converts from a GsDtArray. The array must contain the same elements as
**  FromDictionary requires of a GsDtDictionary. Any elements of the same type must
**	occur in the same order as in the comments for FromDictionary
**
*/
	void FromArray( const GsDtArray& Arr );

protected:
	double		m_LowGradient;
	double		m_HighGradient;
	dvector_t		m_XValues;
	dvector_t 		m_YValues;

	void Construct( double LowGradient, double HighGradient,
					const double* XValues, const double* YValues,
					int nValues );

private:
	// general constructor, if OptionType doesn't exist then first elt of Dbls
	// must be the option type (when cast to Q_OPT_TYPE)
	void Construct( const double* Dbls, int nDbls, const char* OptionType );
	void Construct( const GsVector& XValues, const GsVector& YValues );
};

GsFuncPiecewiseLinear::GsFuncPiecewiseLinear()
:
	m_LowGradient( 0. ),
	m_HighGradient( 0. )
{
}

GsFuncPiecewiseLinear::GsFuncPiecewiseLinear(
	double		  LowGradient,
	double 		  HighGradient,
	const double* XValues,
	const double* YValues,
	int 		  nValues
)
{
	Construct( LowGradient, HighGradient, XValues, YValues, nValues );
}

GsFuncPiecewiseLinear::GsFuncPiecewiseLinear(
	const GsFuncPiecewiseLinear& rhs
)
:
	m_LowGradient( rhs.m_LowGradient ),
	m_HighGradient( rhs.m_HighGradient ),
	m_XValues( rhs.m_XValues ),
	m_YValues( rhs.m_YValues )
{
}

GsFuncPiecewiseLinear::GsFuncPiecewiseLinear(
	double LowGradient,
	double HighGradient,
	const dvector_t& XValues,
	const dvector_t& YValues
)
{
	if( XValues.size() != YValues.size() )
		GSX_THROW( GsXInvalidArgument, "XValues and YValues are not the same size" );
	Construct( LowGradient, HighGradient, &XValues[0], &YValues[0],
			   XValues.size());
}

GsFuncPiecewiseLinear::GsFuncPiecewiseLinear(
	double LowGradient,
	double HighGradient,
	const GsVector& XValues,
	const GsVector& YValues
)
{
	if( XValues.size() != YValues.size() )
		GSX_THROW( GsXInvalidArgument, "XValues and YValues are not the same size" );
	Construct( LowGradient, HighGradient, &XValues[0], &YValues[0],
			   XValues.size());
}

const GsFuncPiecewiseLinear& GsFuncPiecewiseLinear::operator=(
	const GsFuncPiecewiseLinear& rhs
)
{
	m_LowGradient = rhs.m_LowGradient;
	m_HighGradient = rhs.m_HighGradient;
	m_XValues = rhs.m_XValues;
	m_YValues = rhs.m_YValues;
	return *this;
}


const GsFuncPiecewiseLinear	GsFuncPiecewiseLinear::operator+(
	double rhs
) const
{
	GsFuncPiecewiseLinear f( *this );
	f += rhs;
	return f;
}

const GsFuncPiecewiseLinear	GsFuncPiecewiseLinear::operator*(
	double rhs
) const
{
	GsFuncPiecewiseLinear f( *this );
	f *= rhs;
	return f;
}

const GsFuncPiecewiseLinear& GsFuncPiecewiseLinear::operator-=(
	double rhs
)
{
	return operator+=( -rhs );
}

const GsFuncPiecewiseLinear& GsFuncPiecewiseLinear::operator/=(
	double rhs
)
{
	return operator*=( 1. / rhs );
}

const GsFuncPiecewiseLinear GsFuncPiecewiseLinear::operator-(
	double rhs
) const
{
	return operator+( -rhs );
}

const GsFuncPiecewiseLinear	GsFuncPiecewiseLinear::operator/(
	double rhs
) const
{
	return operator*( 1. / rhs );
}

EXPORT_CPP_GSQUANT GsDtVector* GsDtVectorFromDict(
	const GsDtDictionary* Dict,				 // the GsDtDictionary
	const char*			  Tag,				 // tag to find
	bool				  ThrowIfNone = true // throw exception if missing
);

EXPORT_CPP_GSQUANT GsDt* GsFuncPiecewiseLinearCtor(
	const GsDt &Data	// Construct from <DEFAULT>GsFuncPiecewiseLinear::GsDtType( GsFuncPiecewiseLinear() )</DEFAULT>
);

EXPORT_CPP_GSQUANT const double* DoubleFromDict(
	const GsDtDictionary* Dict,				 // the GsDtDictionary
	const char*			  Tag,				 // tag to find
	bool				  ThrowIfNone = true // throw exception if missing
);

CC_END_NAMESPACE

#endif

