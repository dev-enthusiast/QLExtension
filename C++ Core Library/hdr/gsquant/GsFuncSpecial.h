/****************************************************************
**
**	GSFUNCSPECIAL.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncSpecial.h,v 1.10 2001/03/06 14:00:30 goodfj Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCSPECIAL_H )
#define IN_GSQUANT_GSFUNCSPECIAL_H

#include <gsquant/base.h>

#include <gscore/GsFuncHandle.h>
#include <gscore/GsVector.h>
#include <gscore/GsMatrix.h>
#include <gscore/GsDate.h>
#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )


// .... FIX: too much repetition - should templetize with int parameter and 
// .... convert to string (Vidya's idea)

class EXPORT_CLASS_GSQUANT GsFuncScFunc : public GsFuncHandle<double, double>
{
public:
	GsFuncScFunc() : GsFuncHandle< double, double >() {}
	GsFuncScFunc( const GsFuncHandle<double, double>& h );
	virtual ~GsFuncScFunc() {}

	GsFuncScFunc& operator=( const GsFuncScFunc& other )
    {
		GsFuncHandle<double, double>::operator=( other );
		return *this;
	}

	typedef GsDtGeneric<GsFuncScFunc> GsDtType;
	inline static const char* typeName() { return "GsFuncScFunc"; }
	GsString toString() const { return "GsFuncScFunc"; }

	GsFuncScFunc &operator+=( const GsFuncScFunc &rhs );
	GsFuncScFunc &operator-=( const GsFuncScFunc &rhs );
	GsFuncScFunc &operator*=( const GsFuncScFunc &rhs );
	GsFuncScFunc &operator/=( const GsFuncScFunc &rhs );

	GsFuncScFunc &operator+=( double rhs );
	GsFuncScFunc &operator-=( double rhs );
	GsFuncScFunc &operator*=( double rhs );
	GsFuncScFunc &operator/=( double rhs );
};

class EXPORT_CLASS_GSQUANT GsFuncScFunctional : 
	public GsFuncHandle< GsFuncHandle<double, double>, double >
{
public:
	GsFuncScFunctional() : 
	GsFuncHandle< GsFuncHandle<double, double >, double >() {}
	GsFuncScFunctional( const GsFuncHandle< GsFuncHandle<double, double>, double > & h );
	virtual ~GsFuncScFunctional() {}

	GsFuncScFunctional& operator=( const GsFuncScFunctional& other ) 
	{
		GsFuncHandle< GsFuncHandle<double, double>, double >::operator=( other );
		return *this;
	}

	typedef GsDtGeneric<GsFuncScFunctional> GsDtType;
	inline static const char* typeName() { return "GsFuncScFunctional"; }
	GsString toString() const { return "GsFuncScFunctional"; }
};

class EXPORT_CLASS_GSQUANT GsFuncVecToScFunc :
	public GsFuncHandle< const GsVector&, double >
{
public:
	GsFuncVecToScFunc() : GsFuncHandle< const GsVector&, double > () {}
	GsFuncVecToScFunc( const GsFuncHandle< const GsVector&, double >& h );
	virtual ~GsFuncVecToScFunc() {}

	GsFuncVecToScFunc& operator=( const GsFuncVecToScFunc& other ) 
	{
		GsFuncHandle< const GsVector&, double >::operator=(	other ); 
		return *this;
	}

	typedef GsDtGeneric<GsFuncVecToScFunc> GsDtType;
	inline static const char* typeName() { return "GsFuncVecToScFunc"; }
	GsString toString() const { return "GsFuncVecToScFunc"; }
};

class EXPORT_CLASS_GSQUANT GsFuncVecToScFunctional :
	public GsFuncHandle< const GsVector&, GsFuncHandle< double, double > >
{
public:
	GsFuncVecToScFunctional() : 
	GsFuncHandle< const GsVector&, GsFuncHandle< double, double > > () {}
	GsFuncVecToScFunctional( 
		const GsFuncHandle< const GsVector&, 
		                    GsFuncHandle< double, double > >& h );
	virtual ~GsFuncVecToScFunctional() {}

	GsFuncVecToScFunctional& operator=( const GsFuncVecToScFunctional& other ) 
	{
		GsFuncHandle< const GsVector&, 
		              GsFuncHandle< double, double > >::operator=(	other ); 
		return *this;
	}

	typedef GsDtGeneric<GsFuncVecToScFunctional> GsDtType;
	inline static const char* typeName() { return "GsFuncVecToScFunctional"; }
	GsString toString() const { return "GsFuncVecToScFunctional"; }
};

class EXPORT_CLASS_GSQUANT GsFuncVecToVecFunc :
	public GsFuncHandle< const GsVector&, GsVector >
{
public:
	GsFuncVecToVecFunc() : GsFuncHandle< const GsVector&, GsVector > () {}
	GsFuncVecToVecFunc( const GsFuncHandle< const GsVector&, GsVector >& h );
	virtual ~GsFuncVecToVecFunc() {}

	GsFuncVecToVecFunc& operator=( const GsFuncVecToVecFunc& other )
	{
		GsFuncHandle< const GsVector&, GsVector >::operator=( other );
		return *this;
	}

	typedef GsDtGeneric<GsFuncVecToVecFunc> GsDtType;
	inline static const char* typeName() { return "GsFuncVecToVecFunc"; }
	GsString toString() const { return "GsFuncVecToVecFunc"; }
};

class EXPORT_CLASS_GSQUANT GsFuncScDateFunc : public GsFuncHandle<GsDate, double>
{
public:
	GsFuncScDateFunc() : GsFuncHandle< GsDate, double >() {}
	GsFuncScDateFunc( const GsFuncHandle<GsDate, double>& h );
	virtual ~GsFuncScDateFunc() {}

	GsFuncScDateFunc& operator=( const GsFuncScDateFunc& other )
    {
		GsFuncHandle<GsDate, double>::operator=( other );
		return *this;
	}

	typedef GsDtGeneric<GsFuncScDateFunc> GsDtType;
	inline static const char* typeName() { return "GsFuncScDateFunc"; }
	GsString toString() const { return "GsFuncScDateFunc"; }
};


// .... Addins ....
EXPORT_CPP_GSQUANT 
double GsFuncScFuncValue( const GsFuncScFunc& h,
						  double x );
EXPORT_CPP_GSQUANT 
double GsFuncVecToScFuncValue( const GsFuncVecToScFunc& h,
							   const GsVector& x );
EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncVecToScFunctionalValue( const GsFuncVecToScFunctional& h,
										   const GsVector& x );
EXPORT_CPP_GSQUANT 
GsVector GsFuncVecToVecFuncValue( const GsFuncVecToVecFunc& h,
								  const GsVector& x );

// FIX FIX FIX HACK HACK HACK FIX FIX FIX
// FIX
// FIX Make a real GsDt out of GsFuncScFunc that supports binary operators.
// FIX Then we won't need the PoS addins below.
// FIX
// FIX FIX FIX HACK HACK HACK FIX FIX FIX

EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncScFuncConstCreate( double Const );

EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncScFuncAdd( const GsFuncScFunc &Lhs, const GsFuncScFunc &Rhs );

EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncScFuncLineCreate( double A, double B );

typedef GsFuncHandle< double, double > GsFuncDoubleDouble;

EXPORT_CPP_GSQUANT 
GsFuncDoubleDouble GsFuncFromGsFuncScFunc(
	const GsFuncScFunc &Func
);



CC_END_NAMESPACE

#endif 
