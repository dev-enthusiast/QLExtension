/****************************************************************
**
**	LTVOLCONSTRAINTINGRAPH.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltaddin/src/ltaddin/LtVolConstraintInGraph.h,v 1.6 2001/01/26 19:54:28 johnsoc Exp $
**
****************************************************************/

#if !defined( IN_LTADDIN_LTVOLCONSTRAINTINGRAPH_H )
#define IN_LTADDIN_LTVOLCONSTRAINTINGRAPH_H

#include <ltaddin/base.h>
#include <fiblack/FqVolCurveConstraint.h>
#include <secdb/LtSecDbObj.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : LtVolCurveConstraintInGraph 
** Description : 
****************************************************************/

class EXPORT_CLASS_LTADDIN LtVolCurveConstraintInGraph : public FqVolCurveConstraint
{
public:
	LtVolCurveConstraintInGraph(
		const GsString &DidVtName, 
		const GsDtArray &DidVtArgs, 
		LtSecDbObj &DidDbObj,
		const GsString &EvalVtName, 
		const GsDtArray &EvalVtArgs,
		LtSecDbObj &EvalDbObj,
		const GsDate &Initial,
		const GsDate &Maturity	
	);

	LtVolCurveConstraintInGraph( const LtVolCurveConstraintInGraph & );

	~LtVolCurveConstraintInGraph();

	// GsDt support
	
	virtual GsString toString() const {	return "LtDiscountFuncConstraintInGraph"; }

private:

	LtVolCurveConstraintInGraph &operator=( const LtVolCurveConstraintInGraph &rhs );		
};


EXPORT_CPP_LTADDIN
FqVolCurveConstraint *LtVolCurveConstraintInGraphCreate(
	const GsString &DidVtName, 	
	const GsDtArray &DidVtArgs, 
	const LtSecDbObj &DidDbObj,	
	const GsString &EvalVtName, 
	const GsDtArray &EvalVtArgs,
	const LtSecDbObj &EvalDbObj,
	const GsDate &Initial,		
	const GsDate &Maturity		
);


CC_END_NAMESPACE

#endif 
