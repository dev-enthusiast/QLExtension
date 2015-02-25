/****************************************************************
**
**	GSSYMBOLICVARIABLEREP.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicVariableRep.h,v 1.16 2001/11/27 22:47:48 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICVARIABLEREP_H )
#define IN_GSSYMEXPR_GSSYMBOLICVARIABLEREP_H

#include <gssymexpr/base.h>
#include <functional>
#include <gscore/GsSymbol.h>
#include <gscore/GsRefCount.h>
#include <gsdt/GsDtArray.h>
#include <gssymexpr/GsSymbolicExpressionRep.h>
#include <gssymexpr/GsSymbolicVariableNameAndArgs.h>
#include <gscore/GsMemPoolPerClass.h>


CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsSymbolicVariableRep 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicVariableRep : public GsSymbolicExpressionRep
{
public:
	GsSymbolicVariableRep( GsSymbol Name, const GsDtArray &Args );
	GsSymbolicVariableRep( const GsSymbolicVariableNameAndArgs &NameAndArgs ) : m_NameAndArgs( NameAndArgs )
	{
	}

	~GsSymbolicVariableRep();

	GsSymbol  Name() const;

	const GsDtArray &Args() const;
		  GsDtArray &Args();

	const GsSymbolicVariableNameAndArgs &NameAndArgs() const { return m_NameAndArgs; }
	      GsSymbolicVariableNameAndArgs &NameAndArgs() 		 { return m_NameAndArgs; }

	virtual GsString toString() const;

	virtual const GsSymbolicVariableRep *AsVariable() const;

	virtual double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const;

	virtual GsSymbolicVariableAssignment AllVarsInExpression() const;

	virtual size_t hash() const;

protected:
	virtual GsSymbolicExpression ReplaceInVariables( const GsSymbolicExpressionCopyOperatorReplaceInVariables *oper ) const;

	virtual GsSymbolicExpression ReplaceVariables( const GsSymbolicExpressionCopyOperatorReplaceVariables *oper ) const;

	virtual GsSymbolicExpression ReplaceVariablesWithExpressions( const GsSymbolicExpressionCopyOperatorReplaceVariablesWithExpressions *oper ) const;

	virtual GsSymbolicExpression ReplaceWildCards( const GsSymbolicExpressionCopyOperatorReplaceWildCards *oper ) const;

private:

	// Hidden from public view, but not implemented.
	GsSymbolicVariableRep( const GsSymbolicVariableRep & );
	GsSymbolicVariableRep &operator=( const GsSymbolicVariableRep &rhs );

	virtual bool operator==( const GsSymbolicExpressionRep &rhs ) const;

	GsSymbolicVariableNameAndArgs
			m_NameAndArgs;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS

protected:

	virtual GsSymbolicExpressionRep *copyRep() const; 

};



CC_END_NAMESPACE

#endif 




