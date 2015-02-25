/****************************************************************
**
**	GSSYMBOLICEXPRESSIONREP.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicExpressionRep.h,v 1.15 2001/11/27 22:47:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICEXPRESSIONREP_H )
#define IN_GSSYMEXPR_GSSYMBOLICEXPRESSIONREP_H

#include <gssymexpr/base.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsString.h>
#include <gssymexpr/GsSymbolicVariableAssignment.h>

CC_BEGIN_NAMESPACE( Gs )


// Fwd declarations of derived rep classes.

class GsSymbolicConstant;
class GsSymbolicVariableRep;
class GsSymbolicWeightedSum;
class GsSymbolicBinOp;
class GsSymbolicBinOpSum;
class GsSymbolicBinOpDifference;
class GsSymbolicBinOpProduct;
class GsSymbolicBinOpQuotient;

class GsSymbolicExpression;
class GsSymbolicExpressionCopyOperator;
class GsSymbolicExpressionCopyOperatorReplaceInVariables;
class GsSymbolicExpressionCopyOperatorReplaceVariables;
class GsSymbolicExpressionCopyOperatorReplaceVariablesWithExpressions;
class GsSymbolicExpressionCopyOperatorReplaceWildCards;
class GsSymbolicExpressionCopyOperatorSimplify;

class GsSymbolicExpressionVector;

/****************************************************************
** Class	   : GsSymbolicExpressionRep 
** Description : rep of a symbolic expression
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsSymbolicExpressionRep *(*ReplacementFunc)( const GsSymbolicVariableNameAndArgs &NameAndArgs, double Val );

	GsSymbolicExpressionRep() {}
	GsSymbolicExpressionRep( const GsSymbolicExpressionRep &Other ) /* : GsRefCountCopyOnWrite( Other ) */ {}

	virtual ~GsSymbolicExpressionRep() {}

	GsSymbolicExpressionRep *copy() const;

	// Transform the rep in some way, as specified by the given
	// operator.  We return an expression rather than a rep, since the
	// operator returns one.  The operator returns an expression
	// rather than a rep so that it can construct its return value
	// using whaterver combination of expression operators it sees
	// fit.
	virtual GsSymbolicExpression Transform( const GsSymbolicExpressionCopyOperator *oper ) const;

	virtual double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const = 0;

	virtual GsSymbolicVariableAssignment AllVarsInExpression() const = 0;

	virtual GsString toString() const = 0;

	virtual size_t size() const { return 1; }

	// Quick run-time type checking for known derived classes.  This helps us optimize representations.

	virtual const GsSymbolicConstant 	*AsConstant() 		const;
	virtual const GsSymbolicVariableRep *AsVariable() 		const;
	virtual const GsSymbolicWeightedSum	*AsWeightedSum()	const;
	virtual const GsSymbolicBinOp		*AsBinOp()			const;

	virtual const GsSymbolicBinOpSum		*AsBinOpSum()			const;
	virtual const GsSymbolicBinOpDifference	*AsBinOpDifference()	const;
	virtual const GsSymbolicBinOpProduct	*AsBinOpProduct()		const;
	virtual const GsSymbolicBinOpQuotient	*AsBinOpQuotient()		const;

	// Hashing.  Default is lame string hashing, but derived classes should override.

	virtual size_t hash() const;
	
protected:

	// Replace an arg value with some other arg value.
	virtual GsSymbolicExpression ReplaceInVariables( const GsSymbolicExpressionCopyOperatorReplaceInVariables *oper ) const;

	// Replace matching variables with replacement variables
	virtual GsSymbolicExpression ReplaceVariables( const GsSymbolicExpressionCopyOperatorReplaceVariables *oper ) const;

	// Replace matching variables with expressions (with wildcards). 
	virtual GsSymbolicExpression ReplaceVariablesWithExpressions( const GsSymbolicExpressionCopyOperatorReplaceVariablesWithExpressions *oper ) const;

	// Replace wildcards with values in operator. 
	virtual GsSymbolicExpression ReplaceWildCards( const GsSymbolicExpressionCopyOperatorReplaceWildCards *oper ) const;	

	// Simplify the expression
	virtual GsSymbolicExpression Simplify( const GsSymbolicExpressionCopyOperatorSimplify *oper ) const;	

private:

	// Equality testing is left to derived classes.

	virtual bool operator==( const GsSymbolicExpressionRep &rhs ) const = 0;

	friend class GsSymbolicExpression;
	friend class GsSymbolicExpressionCopyOperatorReplaceInVariables;
	friend class GsSymbolicExpressionCopyOperatorReplaceVariables;
	friend class GsSymbolicExpressionCopyOperatorReplaceVariablesWithExpressions;
	friend class GsSymbolicExpressionCopyOperatorReplaceWildCards;
	friend class GsSymbolicExpressionCopyOperatorSimplify;

protected:

	// This function copies the rep without any transformations of any
	// kind.  In particular, each derived class should implment this
	// simply by using new and the copy constructor of the class.  If
	// the rep happens to contain handles, like BinOps do, the reps
	// they point to are not copied.

	virtual GsSymbolicExpressionRep *copyRep() const = 0; 

	// Collect all the terms in the expression.  If the expression is a sum,
	// this will be called recursively, otherwise it will just return the
	// expression itself.

	virtual GsSymbolicExpressionVector CollectTerms()   const;
	virtual GsSymbolicExpressionVector CollectFactors() const;
};






CC_END_NAMESPACE

#endif 
