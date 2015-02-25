/****************************************************************
**
**	GSHASHFUNCSYMBOLICVARIABLE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gssymexpr/src/gssymexpr/GsHashFuncSymbolicVariable.h,v 1.3 2001/11/27 22:47:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMEXPR_GSHASHFUNCSYMBOLICVARIABLE_H )
#define IN_GSSYMEXPR_GSHASHFUNCSYMBOLICVARIABLE_H

#include <gssymexpr/base.h>
#include <gssymexpr/GsSymbolicVariableNameAndArgs.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsHashFuncSymbolicVariable 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSSYMEXPR GsHashFuncSymbolicVariable
{
public:
	unsigned long operator()( const GsSymbolicVariableNameAndArgs &NameAndArgs ) const;

private:
		
};

CC_END_NAMESPACE

#endif 
