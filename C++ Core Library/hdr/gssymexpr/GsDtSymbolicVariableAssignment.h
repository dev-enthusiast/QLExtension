/****************************************************************
**
**	GSDTSYMBOLICVARIABLEASSIGNMENT.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsDtSymbolicVariableAssignment.h,v 1.6 2001/11/27 22:47:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSDTSYMBOLICVARIABLEASSIGNMENT_H )
#define IN_GSSYMEXPR_GSDTSYMBOLICVARIABLEASSIGNMENT_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsSymbolicVariableAssignment.h>
#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )

// FIX - inheriting from GsDtGeneric is probably a pretty bad idea.  
// But it's a pain to go implement all the VFs just because we want
// to overload a few VFs.

/****************************************************************
** Class	   : GsDtSymbolicVariableAssignment 
** Description : The GsDtType is just a GsDtGeneric with an iterator added.
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsDtSymbolicVariableAssignment : public GsDtGeneric< GsSymbolicVariableAssignment >
{
public:
	GsDtSymbolicVariableAssignment() {};
	GsDtSymbolicVariableAssignment( const GsDtSymbolicVariableAssignment &Other ) : GsDtGeneric< GsSymbolicVariableAssignment >( Other ) {}
	GsDtSymbolicVariableAssignment( const GsSymbolicVariableAssignment &Data ) : GsDtGeneric< GsSymbolicVariableAssignment >( Data ) {}
	~GsDtSymbolicVariableAssignment() {}

	class Iterator;

	virtual GsDt::Iterator*	Subscript( const GsDt &Index );
	virtual GsDt::Iterator*	SubscriptFirst( IterationFlag Flag );
	virtual GsStatus		SubscriptReplace( const GsDt &Index, GsDt *NewValue, GsCopyAction Action );

	// FIX - support these?
//	virtual GsStatus		SubscriptDelete( const GsDt &Index );
//	virtual GsStatus		SubscriptAppend( GsDt *NewValue, GsCopyAction Action );

	// The rest of this is needed to deal with the things that GsDtGeneric does not quite do properly.

	virtual GsDt *CopyDeep() const;

	static void initType();

	GSDT_DECLARE_COMMON( GsDtSymbolicVariableAssignment )		
};	


CC_END_NAMESPACE

#endif 
