/****************************************************************
**
**	GSSYMBOLICBINOP.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicOp.h,v 1.14 2001/11/27 22:47:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICBINOP_H )
#define IN_GSSYMEXPR_GSSYMBOLICBINOP_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsSymbolicExpression.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsSymbolicBinOp 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicBinOp : public GsSymbolicExpressionRep
{
public:
	GsSymbolicBinOp( const GsSymbolicExpression &lhs, const GsSymbolicExpression &rhs );
	GsSymbolicBinOp( const GsSymbolicBinOp & );

	~GsSymbolicBinOp();

	virtual GsSymbolicVariableAssignment AllVarsInExpression() const;

	virtual GsString toString() const;

	virtual GsString OpString() const = 0;

	virtual size_t size() const { return m_Lhs.size() + m_Rhs.size(); }

	virtual const GsSymbolicBinOp *AsBinOp() const;

	virtual size_t hash() const;

	GsSymbolicExpression
			m_Lhs,
			m_Rhs;

protected:

	// Replace an arg value with some other arg value.
	virtual GsSymbolicExpression ReplaceInVariables( const GsSymbolicExpressionCopyOperatorReplaceInVariables *oper ) const;

	// Replace matching variables with replacement variables
	virtual GsSymbolicExpression ReplaceVariables( const GsSymbolicExpressionCopyOperatorReplaceVariables *oper ) const;

	// Replace matching variables with expressions (with wildcards). 
	virtual GsSymbolicExpression ReplaceVariablesWithExpressions( const GsSymbolicExpressionCopyOperatorReplaceVariablesWithExpressions *oper ) const;

	// Replace wildcards with values in operator. 
	virtual GsSymbolicExpression ReplaceWildCards( const GsSymbolicExpressionCopyOperatorReplaceWildCards *oper ) const;	

private:
	// Hidden and unimplemented.
	GsSymbolicBinOp &operator=( const GsSymbolicBinOp &rhs );

	virtual bool operator==( const GsSymbolicExpressionRep &rhs ) const;

	friend class GsSymbolicExpression;

	// WARNING - If data members are added to derived classes, then memory
	// management has to be moved down to derived classes also, otherwise
	// allocation will fail as we will be trying to allocate something 
	// bigger than the objects in the heap for this class.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS

};



/****************************************************************
** Class	   : GsSymbolicBinOpSum 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicBinOpSum : public GsSymbolicBinOp
{
public:
	GsSymbolicBinOpSum( const GsSymbolicExpression &lhs, const GsSymbolicExpression &rhs );
	GsSymbolicBinOpSum( const GsSymbolicBinOpSum & );

	~GsSymbolicBinOpSum();

	virtual GsSymbolicExpression Transform( const GsSymbolicExpressionCopyOperator *oper ) const;		

	virtual double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const;

	virtual GsString OpString() const;

	virtual const GsSymbolicBinOpSum		*AsBinOpSum()			const;

	// Simplify the expression
	virtual GsSymbolicExpression Simplify( const GsSymbolicExpressionCopyOperatorSimplify *oper ) const;	

private:
	// Hidden and unimplemented.
	GsSymbolicBinOpSum &operator=( const GsSymbolicBinOpSum &rhs );

protected:

	virtual GsSymbolicExpressionRep *copyRep() const; 

	virtual GsSymbolicExpressionVector CollectTerms() const;
};



/****************************************************************
** Class	   : GsSymbolicBinOpDifference 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicBinOpDifference : public GsSymbolicBinOp
{
public:
	GsSymbolicBinOpDifference( const GsSymbolicExpression &lhs, const GsSymbolicExpression &rhs );
	GsSymbolicBinOpDifference( const GsSymbolicBinOpDifference & );

	~GsSymbolicBinOpDifference();

	virtual GsSymbolicExpression Transform( const GsSymbolicExpressionCopyOperator *oper ) const;		

	virtual double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const;

	virtual GsString OpString() const;

	virtual const GsSymbolicBinOpDifference	*AsBinOpDifference()	const;

	// Simplify the expression
	virtual GsSymbolicExpression Simplify( const GsSymbolicExpressionCopyOperatorSimplify *oper ) const;	

private:
	// Hidden and unimplemented.
	GsSymbolicBinOpDifference &operator=( const GsSymbolicBinOpDifference &rhs );

protected:

	virtual GsSymbolicExpressionRep *copyRep() const; 
};


/****************************************************************
** Class	   : GsSymbolicBinOpProduct 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicBinOpProduct : public GsSymbolicBinOp
{
public:
	GsSymbolicBinOpProduct( const GsSymbolicExpression &lhs, const GsSymbolicExpression &rhs );
	GsSymbolicBinOpProduct( const GsSymbolicBinOpProduct & );

	~GsSymbolicBinOpProduct();

	virtual GsSymbolicExpression Transform( const GsSymbolicExpressionCopyOperator *oper ) const;		

	virtual double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const;

	virtual GsString OpString() const;

	virtual const GsSymbolicBinOpProduct	*AsBinOpProduct()		const;

	// Simplify the expression
	virtual GsSymbolicExpression Simplify( const GsSymbolicExpressionCopyOperatorSimplify *oper ) const;	

private:
	// Hidden and unimplemented.
	GsSymbolicBinOpProduct &operator=( const GsSymbolicBinOpProduct &rhs );

protected:

	virtual GsSymbolicExpressionRep *copyRep() const; 

	virtual GsSymbolicExpressionVector CollectFactors() const;
};


/****************************************************************
** Class	   : GsSymbolicBinOpQuotient 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicBinOpQuotient : public GsSymbolicBinOp
{
public:
	GsSymbolicBinOpQuotient( const GsSymbolicExpression &lhs, const GsSymbolicExpression &rhs );
	GsSymbolicBinOpQuotient( const GsSymbolicBinOpQuotient & );

	~GsSymbolicBinOpQuotient();

	virtual GsSymbolicExpression Transform( const GsSymbolicExpressionCopyOperator *oper ) const;		

	virtual double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const;

	virtual GsString OpString() const;

	virtual const GsSymbolicBinOpQuotient	*AsBinOpQuotient()		const;

	// Simplify the expression
	virtual GsSymbolicExpression Simplify( const GsSymbolicExpressionCopyOperatorSimplify *oper ) const;	

private:
	// Hidden and unimplemented.
	GsSymbolicBinOpQuotient &operator=( const GsSymbolicBinOpQuotient &rhs );

protected:

	virtual GsSymbolicExpressionRep *copyRep() const; 
};



CC_END_NAMESPACE

#endif 
