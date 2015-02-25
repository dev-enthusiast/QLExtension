/****************************************************************
**
**	gscore/GsString.h	- GsString class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsString.h,v 1.45 2001/11/27 22:41:08 procmon Exp $
**
****************************************************************/

#ifndef IN_GSCORE_GSSTRING_H
#define IN_GSCORE_GSSTRING_H

#include <gscore/base.h>
#include <gscore/types.h>     // for GsString typedef

#include <functional> // for std::unary_function/std::binary_function
#include <string>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSBASE GsString
	StrDefault( GsString const& Str, GsString const& DefaultValue ),
	StrLower( GsString const& Str ),
	StrUpper( GsString const& Str );

EXPORT_CPP_GSBASE GsStringVector
	StrSplit( GsString const& Str, char SepChar, bool IncludeBlanks = true );

EXPORT_CPP_GSBASE int
	StrICmp( GsString const& StrA, GsString const& StrB );

/****************************************************************
** Class	   : StrICmpPred 
** Description : A useful predicate for comparing strings
**				 to a target, e.g. in std::find_if
****************************************************************/

class EXPORT_CLASS_GSBASE StrICmpPred : public CC_NS(std, unary_function)<GsString, bool>
{
public:
	StrICmpPred( GsString const& Target ) : m_Target( Target ) {}

	// use default copy constructor
	// use default destructor

	bool operator()( GsString const& Test ) const { return ( StrICmp( m_Target, Test ) == 0 ); }

private:
	GsString const m_Target;
};

/****************************************************************
** Class	   : StrICmpFunctor 
** Description : Functor to compare two strings for case-insenesistive equality.
****************************************************************/

struct EXPORT_CLASS_GSBASE StrICmpFunctor : public CC_NS(std, binary_function)<GsString, GsString, bool>
{
	bool operator()( GsString const& a, GsString const& b ) const
	{
		return ( StrICmp( a, b ) == 0 );
	}
};

CC_END_NAMESPACE

#endif

