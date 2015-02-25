/****************************************************************
**
**	GSSYMBOLICWEIGHTEDSUM.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicWeightedSum.h,v 1.20 2001/11/27 22:47:48 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICWEIGHTEDSUM_H )
#define IN_GSSYMEXPR_GSSYMBOLICWEIGHTEDSUM_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsSymbolicExpression.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsSymbolicWeightedSum 
** Description : Weighted sum of variables.
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicWeightedSum : public GsSymbolicExpressionRep
{
public:
	GsSymbolicWeightedSum( double Constant = 0.0 );
	GsSymbolicWeightedSum( const GsSymbolicVariableAssignment &VarsAndWeights, double Constant = 0.0 );
	GsSymbolicWeightedSum( double Coefficient, const GsSymbolicVariableNameAndArgs &Variable, double Constant = 0.0 );
	GsSymbolicWeightedSum( const GsSymbolicWeightedSum & );

	~GsSymbolicWeightedSum();

	virtual GsSymbolicVariableAssignment AllVarsInExpression() const;

	virtual double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const;

	virtual GsString toString() const;

	virtual const GsSymbolicWeightedSum *AsWeightedSum() const;

	GsSymbolicWeightedSum &operator+=( const GsSymbolicWeightedSum &rhs );
	GsSymbolicWeightedSum &operator+=( double rhs );
	GsSymbolicWeightedSum &operator*=( double rhs );

	GsSymbolicWeightedSum &Add( const GsSymbolicVariableNameAndArgs &VarNameAndArgs, double value );

	virtual size_t size() const { return m_VarsAndWeights.size(); }

	double Constant() const { return m_Constant; }

	virtual size_t hash() const;

protected:
	virtual GsSymbolicExpression ReplaceInVariables( const GsSymbolicExpressionCopyOperatorReplaceInVariables *oper ) const;

	virtual GsSymbolicExpression ReplaceVariables( const GsSymbolicExpressionCopyOperatorReplaceVariables *oper ) const;

	virtual GsSymbolicExpression ReplaceVariablesWithExpressions( const GsSymbolicExpressionCopyOperatorReplaceVariablesWithExpressions *oper ) const;

	virtual GsSymbolicExpression ReplaceWildCards( const GsSymbolicExpressionCopyOperatorReplaceWildCards *oper ) const;

private:
	// Hidden and unimplemented.
	GsSymbolicWeightedSum &operator=( const GsSymbolicWeightedSum &rhs );

	virtual bool operator==( const GsSymbolicExpressionRep &rhs ) const;

	// The representation just uses the same object we use for
	// variable assignment, but we put in weights instead of values.

	GsSymbolicVariableAssignment
			m_VarsAndWeights;		

	double
			m_Constant;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS

protected:

	virtual GsSymbolicExpressionRep *copyRep() const; 
};


CC_END_NAMESPACE

#endif 
