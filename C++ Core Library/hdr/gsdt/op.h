/****************************************************************
**
**	gsdt/op.h	- Just the Operator stuff (Binary & Unary)
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/op.h,v 1.2 2001/01/22 13:04:31 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDT_OP_H )
#define IN_GSDT_OP_H

#include <gsdt/base.h>
#include <gsdt/dttypes.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Just in case you need to print one of these enums
*/

EXPORT_CPP_GSDTLITE const char
		*GsDtuBinOpToString(	GSDT_BINOP   Op ),
    	*GsDtuUnaryOpToString(	GSDT_UNARYOP Op );


/*
**	The types of various functions
*/

typedef GsDt	*(*GSDT_CONVERT_FUNC)( const GsType& NewType, const GsDt &Val );


/*
**	GsDtuConvFunc is the handle for a conversion function
*/

class EXPORT_CLASS_GSDTLITE GsDtuConvFunc
{
private:
	GsType&	m_TypeLhs;
	GsType&	m_TypeRhs;

	GSDT_CONVERT_FUNC
			m_Func;

	GsDtuConvFunc
			*m_Next;

public:
	GsDtuConvFunc( GsType& Lhs, GsType& Rhs, GSDT_CONVERT_FUNC Func );
	~GsDtuConvFunc();

	GsDt			*DoConvert( const GsType& NewType, const GsDt &Val ) const;

	void			SetNext( GsDtuConvFunc *Next )	{ m_Next = Next; }
	GsDtuConvFunc	*Next() const					{ return m_Next; }

	const GsType&	TypeLhs() const	{ return m_TypeLhs; }
	const GsType&	TypeRhs() const	{ return m_TypeRhs; }
};

inline GsDt *GsDtuConvFunc::DoConvert( const GsType& NewType, const GsDt& Val ) const
{
	return (*m_Func)( NewType, Val );
}

CC_END_NAMESPACE

#endif

