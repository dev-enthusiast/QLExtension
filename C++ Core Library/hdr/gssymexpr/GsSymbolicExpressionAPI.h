/****************************************************************
**
**	GSSYMBOLICEXPRESSIONAPI.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicExpressionAPI.h,v 1.15 2001/11/27 22:47:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICEXPRESSIONAPI_H )
#define IN_GSSYMEXPR_GSSYMBOLICEXPRESSIONAPI_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsSymbolicExpression.h>
#include <gssymexpr/GsSymbolicExpressionVector.h>
#include <gssymexpr/GsSymbolicVariableNameAndArgsVector.h>

CC_BEGIN_NAMESPACE( Gs )


EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicConstantCreate( double Value );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicVariableCreate( const GsString &Name, const GsDtArray &Args );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicWeightedSumCreate( GsSymbolicVariableAssignment VarsAndWeights, double Constant );

EXPORT_CPP_GSSYMEXPR GsSymbolicVariableAssignment GsSymbolicExpressionAllVars( GsSymbolicExpression Epxr );

EXPORT_CPP_GSSYMEXPR double GsSymbolicExpressionEvaluate( GsSymbolicExpression Expr, GsSymbolicVariableAssignment VarAssignment );

EXPORT_CPP_GSSYMEXPR GsDtArray *GsSymbolicExpressionArrayAllVars(   GsSymbolicExpression Expr );

EXPORT_CPP_GSSYMEXPR GsSymbolicVariableNameAndArgsVector *GsSymbolicExpressionAllVarsVector( 		GsSymbolicExpression Expr );
EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector 		 *GsSymbolicExpressionAllVarsExpressions( 	GsSymbolicExpression Expr );

EXPORT_CPP_GSSYMEXPR GsDtArray *GsSymbolicExpressionVtApplyAllVars( GsSymbolicExpression Expr );

EXPORT_CPP_GSSYMEXPR double GsSymbolicExpressionEvaluateArray( GsSymbolicExpression Expr, const GsVector &Values );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicExpressionReplaceInVariables( GsSymbolicExpression Expr, const GsDt &Target, const GsDt &Replacement, size_t Position );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicExpressionReplaceMultiple( GsSymbolicExpression Expr, const GsDt &Target, const GsDtArray &Replacement, size_t Position );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicExpressionReplaceVariable( GsSymbolicExpression Expr, GsSymbolicExpression Target, GsSymbolicExpression Replacement );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicExpressionReplaceVariables( GsSymbolicExpression Expr, const GsSymbolicExpressionVector& Targets, const GsSymbolicExpressionVector& Replacements	);

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicExpressionSumVector( const GsSymbolicExpressionVector& Vec, size_t Position = 0 );

EXPORT_CPP_GSSYMEXPR size_t GsSymbolicExpressionSize( GsSymbolicExpression Expr );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpression GsSymbolicExpressionSimplify( GsSymbolicExpression Expr );

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector GsSymbolicExpressionCollectTerms(   GsSymbolicExpression Expr	);

EXPORT_CPP_GSSYMEXPR GsSymbolicExpressionVector GsSymbolicExpressionCollectFactors( GsSymbolicExpression Expr	);

CC_END_NAMESPACE

#endif 
