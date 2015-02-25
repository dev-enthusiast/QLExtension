/****************************************************************
**
**	ADVECTOR.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/AdVector.h,v 1.4 2011/05/17 16:08:04 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_ADVECTOR_H )
#define IN_GSDATE_ADVECTOR_H

#include <gscore/base.h>
#include <vector>
#include <ccstl.h>
#include <gscore/GsVector.h>
#include <gscore/GsIntegerVector.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsStringVector.h>
#include <gscore/GsDayCountISDAVector.h>
#include <gscore/GsDayCountVector.h>
#include <gscore/GsRDateVector.h>
#include <gscore/GsSymDateVector.h>

CC_BEGIN_NAMESPACE( Gs )

/*
** A Generic vector class. 
** Used by 'autofunc <EXPAND>' code to store vectors of
** GsDt'able (NeedAutoRef) classes during <EXPAND> mode. We need this class since
** GsDtArray does not have the same interface as GsXXXXVector. 
** By having this class we can have commen code generated in the adlib_XXXXXX.c
** file regardless of argument type.
*/

// FIX is a tempory Name and Location for this class, inorder to remove a dependency on fcadlib for core projects.

template <class _T>
class AdExpandVector : public CC_STL_VECTOR(_T)
{

public:
	typedef typename CC_STL_VECTOR(_T)	base_type;
	typedef typename base_type::size_type size_type;

	static const char* typeName();
	GsString toString() const { return typeName(); }

	typedef GsDtGeneric< AdExpandVector< _T > > GsDtType;

	AdExpandVector() : base_type() {}
	AdExpandVector( size_type N ) : base_type( N ) {}
	AdExpandVector( const AdExpandVector& Rhs ) : base_type( Rhs ) {}
	AdExpandVector& operator=( const AdExpandVector& Rhs ) 
	{
		if( &Rhs != this )
			base_type::operator=( Rhs );

		return *this;
	}
};



template <class _T>
const char* AdExpandVector<_T>::typeName()
{
	static GsString* TypeName = 0;

	if( !TypeName ) 
	{
		TypeName = new GsString();
		*TypeName += "AdExpandVector<";
		*TypeName += _T::typeName();
		*TypeName += ">";
	}

	return TypeName->c_str();
}



template <class ArgType>
class AdExpandVectorType
{
public:
	typedef AdExpandVector<ArgType> vector_type;
	typedef typename ArgType::GsDtType ElementGsDtType;
};

template<> class AdExpandVectorType<double>
{
public:
	typedef GsVector vector_type;
	typedef GsDtDouble ElementGsDtType;
};

template<> class AdExpandVectorType<int>
{
public:
	typedef GsIntegerVector vector_type;
	typedef GsDtDouble ElementGsDtType;
};

template<> class AdExpandVectorType<GsDate>
{
public:
	typedef GsDateVector vector_type;
	typedef GsDtDate ElementGsDtType;
};

template<> class AdExpandVectorType<GsString>
{
public:
	typedef GsStringVector vector_type;
	typedef GsDtString ElementGsDtType;
};

template<> class AdExpandVectorType<GsDayCountISDA>
{
public:
	typedef GsDayCountISDAVector vector_type;
	typedef GsDtDayCountISDA ElementGsDtType;
};

template<> class AdExpandVectorType<GsDayCount>
{
public:
	typedef GsDayCountVector vector_type;
	typedef GsDtDayCount ElementGsDtType;
};

template<> class AdExpandVectorType<GsRDate>	
{													
public:												
	typedef GsRDateVector vector_type;				
	typedef GsDtRDate ElementGsDtType;				
};													

template<> class AdExpandVectorType<GsSymDate>
{
public:
	typedef GsSymDateVector vector_type;
	typedef GsDtSymDate ElementGsDtType;
};


CC_END_NAMESPACE

#endif 
