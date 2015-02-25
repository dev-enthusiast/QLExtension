/****************************************************************
**
**	GSDTSYMBOLICEXPRESSION.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsDtSymbolicExpression.h,v 1.6 2001/11/27 22:47:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSDTSYMBOLICEXPRESSION_H )
#define IN_GSSYMEXPR_GSDTSYMBOLICEXPRESSION_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsSymbolicExpression.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsDtSymbolicExpression 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsDtSymbolicExpression : public GsDt
{
 	GSDT_DECLARE_TYPE( GsDtSymbolicExpression )		

public:
	GsDtSymbolicExpression();
	GsDtSymbolicExpression( const GsDtSymbolicExpression &Other );
	GsDtSymbolicExpression( const GsSymbolicExpression   &Data  );
	~GsDtSymbolicExpression();

	GsSymbolicExpression data() const { return m_Data; }

	// This is all we really wanted to overload.
	virtual GsStatus BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );

	// These are the rest of the GsDt virtual functions we need.

	virtual GsDt*		highLimit() const;
	virtual GsDt*		lowLimit() const;
	virtual GsDt*		errorSentinel() const;

	virtual GsDt *CopyDeep() const;
	virtual GsDt *CopyShallow() const { return CopyDeep(); }

	virtual size_t		GetSize() const;

	virtual FcHashCode	HashComplete() const;
	virtual FcHashCode  HashQuick() const;

	virtual int CompareSame( const GsDt& Other ) const;

	virtual GsBool		Truth() const;
	virtual GsBool		IsValid() const;

	virtual GsStatus	Increment();
	virtual GsStatus	Decrement();
	virtual GsDt		*Negate() const;

	virtual GsString toString() const;

private:

	GsSymbolicExpression
			m_Data;

};



CC_END_NAMESPACE

#endif 
