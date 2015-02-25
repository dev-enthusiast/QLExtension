/****************************************************************
**
**	gsdt/GsDtuBinaryFunc.h	- GsDt::GsDtuBinaryFunc class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtuBinaryFunc.h,v 1.2 2001/01/22 13:04:31 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTUBINARYFUNC_H )
#define IN_GSDT_GSDTUBINARYFUNC_H

#include	<gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	The types of various functions
*/

typedef GsStatus	(*GSDT_ASSIGN_OP_FUNC)( GsDt &Lhs, GSDT_BINOP Op, const GsDt &Rhs );
typedef GsDt		*(*GSDT_BINOP_FUNC)( const GsDt &Lhs, GSDT_BINOP Op, const GsDt &Rhs );


/*
**	GsDtuBinaryFunc is the handle for a binary operations
*/

class EXPORT_CLASS_GSDTLITE GsDtuBinaryFunc
{
private:
	GsType*	m_typeLhs;
	GsType*	m_typeRhs;

	GSDT_ASSIGN_OP_FUNC
			m_funcAssign;

	GSDT_BINOP_FUNC
			m_func;

	GsDtuBinaryFunc
			*m_next;

	void operator=( const GsDtuBinaryFunc& );
	GsDtuBinaryFunc( const GsDtuBinaryFunc& );
	GsDtuBinaryFunc( GSDT_ASSIGN_OP_FUNC AssignFunc, GSDT_BINOP_FUNC BinFunc = NULL );
public:
	GsDtuBinaryFunc( GsType& Lhs, GsType& Rhs, GSDT_ASSIGN_OP_FUNC AssignFunc, GSDT_BINOP_FUNC BinFunc = NULL );
	~GsDtuBinaryFunc();

	GsStatus	doOpAssign( GsDt &Lhs, GSDT_BINOP Op, const GsDt &Rhs ) const;
	GsDt		*doOp( const GsDt &Lhs, GSDT_BINOP Op, const GsDt &Rhs ) const;

	void			nextSet( GsDtuBinaryFunc *Next )	{ m_next = Next; }
	GsDtuBinaryFunc	*next() const						{ return m_next; }

	GsType&		typeLhs() const	{ return *m_typeLhs; }
	GsType&		typeRhs() const	{ return *m_typeRhs; }

	static GsDtuBinaryFunc &sameFunc();	// Call when Lhs & Rhs are ==
	static GsDtuBinaryFunc &errorFunc();	// Call when no matching pair found
};


CC_END_NAMESPACE
#endif

