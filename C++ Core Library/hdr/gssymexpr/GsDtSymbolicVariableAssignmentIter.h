/****************************************************************
**
**	GSDTSYMBOLICVARIABLEASSIGNMENTITER.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsDtSymbolicVariableAssignmentIter.h,v 1.5 2001/11/27 22:47:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSDTSYMBOLICVARIABLEASSIGNMENTITER_H )
#define IN_GSSYMEXPR_GSDTSYMBOLICVARIABLEASSIGNMENTITER_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsDtSymbolicVariableAssignment.h>
#include <gsdt/GsDtDouble.h>

CC_BEGIN_NAMESPACE( Gs )


class GsDtSymbolicVariableAssignment::Iterator : public GsDt::Iterator
{
public:
	Iterator( GsDtSymbolicVariableAssignment *VariableAssignment, const GsSymbolicVariableNameAndArgs::GsDtType &Start );
	Iterator( const Iterator &Other );
	virtual ~Iterator();

	virtual GsDt::Iterator	*Copy();
	virtual GsDt			*BaseValue();
	virtual GsDt			*CurrentKey();
	virtual const GsDt		*CurrentValue();
	virtual GsDt			*ModifyValueBegin();
	virtual GsStatus		ModifyValueEnd();
	virtual GsStatus		Replace( GsDt *Val, GsCopyAction Action );

	virtual GsBool			More();
	virtual void			Next();

private:
	Iterator &operator=( const Iterator &Rhs );

	GsDtSymbolicVariableAssignment	
			*m_VariableAssignment;

	GsSymbolicVariableNameAndArgs::GsDtType
			*m_Key;

	GsDtDouble
			m_GsDtValue;
};


CC_END_NAMESPACE

#endif 
