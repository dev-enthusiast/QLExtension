/****************************************************************
**
**	GSSYMBOLICEXPRESSION.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicExpression.h,v 1.22 2001/11/27 22:47:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICEXPRESSION_H )
#define IN_GSSYMEXPR_GSSYMBOLICEXPRESSION_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsSymbolicExpressionRep.h>
#include <gssymexpr/GsSymbolicVariableAssignment.h>

#include <gscore/GsHandle.h>
#include <gsdt/GsDt.h>
#include <ccstream.h>

CC_BEGIN_NAMESPACE( Gs )


class GsSymbolicExpressionVector;
class GsSymbolicVariableNameAndArgsVector;


/****************************************************************
** Class	   : GsSymbolicExpression 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpression : public GsHandleCopyOnWrite< GsSymbolicExpressionRep >
{
public:
	GsSymbolicExpression();
	GsSymbolicExpression( double Constant );
	GsSymbolicExpression( GsSymbolicExpressionRep *Rep );
	GsSymbolicExpression( const GsSymbolicExpressionRep *Rep );
	GsSymbolicExpression( const GsSymbolicExpression &Other );

	~GsSymbolicExpression();

	GsSymbolicExpression &operator=( const GsSymbolicExpression &rhs );

	double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const;

	GsSymbolicExpression ReplaceVariables( GsSymbolicExpressionCopyOperatorReplaceVariables *Oper ) const;

	GsSymbolicVariableAssignment AllVarsInExpression() const;

	// Replace an arg value with some other arg value.
	GsSymbolicExpression ReplaceInVariables( const GsDt &Target, const GsDtArray &Replacement, size_t Position ) const;

	// Replace variables with arbitrary expressions, with wildcarding of args.
	GsSymbolicExpression ReplaceVariablesWithExpressions( const GsSymbolicVariableNameAndArgsVector &Targets, const GsSymbolicExpressionVector &Replacements ) const;

	// Helper to replace the $0, $1, etc in the wildcard expressions.
	GsSymbolicExpression ReplaceWildCards( const GsDtArray &ReplacementValues ) const;

	// Simplify the expression
	GsSymbolicExpression Simplify() const;

	// Binary operators

	GsSymbolicExpression operator+( GsSymbolicExpression rhs ) const;
	GsSymbolicExpression operator-( GsSymbolicExpression rhs ) const;
	GsSymbolicExpression operator*( GsSymbolicExpression rhs ) const;
	GsSymbolicExpression operator/( GsSymbolicExpression rhs ) const;

	GsSymbolicExpression operator+=( GsSymbolicExpression rhs );
	GsSymbolicExpression operator-=( GsSymbolicExpression rhs );
	GsSymbolicExpression operator*=( GsSymbolicExpression rhs );
	GsSymbolicExpression operator/=( GsSymbolicExpression rhs );

	// Simplifying versions of the binary ops.

	GsSymbolicExpression AddSimplify( 	   	GsSymbolicExpression rhs ) const;
	GsSymbolicExpression SubtractSimplify( 	GsSymbolicExpression rhs ) const;
	GsSymbolicExpression MultiplySimplify( 	GsSymbolicExpression rhs ) const;
	GsSymbolicExpression DivideSimplify( 	GsSymbolicExpression rhs ) const;	

	bool operator==( const GsSymbolicExpression rhs ) const;
	bool operator< ( const GsSymbolicExpression rhs ) const;

	bool operator!=( const GsSymbolicExpression rhs ) const { return !( *this ==   rhs ); }

	bool operator>=( const GsSymbolicExpression rhs ) const { return !( *this <    rhs ); }
	bool operator> ( const GsSymbolicExpression rhs ) const { return  (  rhs  <  *this ); }
	bool operator<=( const GsSymbolicExpression rhs ) const { return !(  rhs  <  *this ); }

	const GsSymbolicVariableRep *AsVariable() const { return m_rep->AsVariable(); }

	// Unary operators

	GsSymbolicExpression operator-() const;

	size_t size() const { return m_rep->size(); }

	// Collect all the terms / factors

	GsSymbolicExpressionVector CollectTerms()   const;
	GsSymbolicExpressionVector CollectFactors() const;

	// GsDt stuff

	typedef class GsDtSymbolicExpression GsDtType;
	GsString toString() const;
//	static GsString typeName() { return GsString( "GsSymbolicExpression" ); }

	size_t hash() const { return m_rep->hash(); }

	friend class GsSymbolicExpressionCopyOperator;

/*
	friend class GsSymbolicExpressionRep;
	friend class GsSymbolicVariableRep;
	friend class GsSymbolicBinOpSum;
	friend class GsSymbolicBinOpDifference;
	friend class GsSymbolicBinOpProduct;
	friend class GsSymbolicBinOpQuotient;
*/

private:

};


// Global arith operator
EXPORT_CPP_GSSYMEXPR GsSymbolicExpression operator+( double lhs, GsSymbolicExpression rhs );
EXPORT_CPP_GSSYMEXPR GsSymbolicExpression operator-( double lhs, GsSymbolicExpression rhs );
EXPORT_CPP_GSSYMEXPR GsSymbolicExpression operator*( double lhs, GsSymbolicExpression rhs );
EXPORT_CPP_GSSYMEXPR GsSymbolicExpression operator/( double lhs, GsSymbolicExpression rhs );


class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionCopyOperator
{
public:
	virtual GsSymbolicExpression operator()( const GsSymbolicExpressionRep *Src ) const = 0;

	static const GsSymbolicExpressionRep
			*GetRep( const GsSymbolicExpression &Src )
	{
		return Src.m_rep;
	}
};

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionCopyOperatorIdentity : public GsSymbolicExpressionCopyOperator
{
public:
	virtual GsSymbolicExpression operator()( const GsSymbolicExpressionRep *Src ) const;
};

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionCopyOperatorReplaceInVariables : public GsSymbolicExpressionCopyOperator
{
public:
	GsSymbolicExpressionCopyOperatorReplaceInVariables( const GsDt &Target, const GsDtArray &Replacement, size_t Position )
	: m_Target( Target ), m_Replacement( Replacement ), m_Position( Position )
	{
	}
	virtual GsSymbolicExpression operator()( const GsSymbolicExpressionRep *Src ) const;

	const GsDt
			&m_Target;

	const GsDtArray
			&m_Replacement;

	size_t
			m_Position;
};

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionCopyOperatorReplaceVariables : public GsSymbolicExpressionCopyOperator
{
public:
	GsSymbolicExpressionCopyOperatorReplaceVariables( const GsSymbolicVariableAssignment &Variables )
	: m_Variables( Variables )
	{
	}
	virtual GsSymbolicExpression operator()( const GsSymbolicExpressionRep *Src ) const;

	virtual GsSymbolicVariableNameAndArgs ReplacementFor( const GsSymbolicVariableNameAndArgs &NameAndArgs, double Val ) const = 0;

	const GsSymbolicVariableAssignment
			&m_Variables;
};

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionCopyOperatorReplaceVariablesWithExpressions : public GsSymbolicExpressionCopyOperator
{
public:
	GsSymbolicExpressionCopyOperatorReplaceVariablesWithExpressions( const GsSymbolicVariableNameAndArgsVector &Targets, const GsSymbolicExpressionVector &Replacements )
	: 
		m_Targets( Targets ),
		m_Replacements( Replacements )
	{
	}

	virtual GsSymbolicExpression operator()( const GsSymbolicExpressionRep *Src ) const;

	const GsSymbolicVariableNameAndArgsVector
			&m_Targets;

	const GsSymbolicExpressionVector
			&m_Replacements;
};


class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionCopyOperatorReplaceWildCards : public GsSymbolicExpressionCopyOperator
{
public:
	GsSymbolicExpressionCopyOperatorReplaceWildCards( const GsDtArray &ReplacementValues )
	: 
		m_ReplacementValues( ReplacementValues )
	{
	}

	virtual GsSymbolicExpression operator()( const GsSymbolicExpressionRep *Src ) const;

	// The things to replace $0, $1, ... with.
	const GsDtArray
			&m_ReplacementValues;
};


class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionCopyOperatorSimplify : public GsSymbolicExpressionCopyOperator
{
public:
	GsSymbolicExpressionCopyOperatorSimplify( )
	{
	}

	virtual GsSymbolicExpression operator()( const GsSymbolicExpressionRep *Src ) const;

private:
	// FIX - might want to store a flag about how much simplification to attempt.
};


EXPORT_CPP_GSSYMEXPR CC_OSTREAM &operator<<( CC_OSTREAM& s, const GsSymbolicExpression &SymExpr );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicExpressionVectorSum(  		const GsSymbolicExpressionVector &Vec, size_t StartPos, size_t EndPos );
EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicExpressionVectorProduct(	const GsSymbolicExpressionVector &Vec );

CC_END_NAMESPACE

#endif 
