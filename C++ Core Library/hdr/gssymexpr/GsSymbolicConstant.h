/****************************************************************
**
**	GSSYMBOLICCONSTANT.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsSymbolicConstant.h,v 1.12 2001/11/27 22:47:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSSYMBOLICCONSTANT_H )
#define IN_GSSYMEXPR_GSSYMBOLICCONSTANT_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsSymbolicExpressionRep.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsSymbolicConstant 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicConstant : public GsSymbolicExpressionRep
{
public:
	GsSymbolicConstant( double Value );
	GsSymbolicConstant( const GsSymbolicConstant & );

	~GsSymbolicConstant();

	virtual double operator()( const GsSymbolicVariableAssignment &VariableAssignment ) const;

	virtual GsSymbolicVariableAssignment AllVarsInExpression() const;

	virtual GsString toString() const;

	double Value() const { return m_Value; }

	virtual size_t hash() const;

private:

	virtual const GsSymbolicConstant *AsConstant() const;

	virtual bool operator==( const GsSymbolicExpressionRep &rhs ) const;

 	double	m_Value;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS

protected:

	virtual GsSymbolicExpressionRep *copyRep() const; 

};


CC_END_NAMESPACE

#endif 
