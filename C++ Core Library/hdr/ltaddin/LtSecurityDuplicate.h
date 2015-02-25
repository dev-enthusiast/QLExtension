/****************************************************************
**
**	LTSECURITYDUPLICATE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltaddin/src/ltaddin/LtSecurityDuplicate.h,v 1.5 2001/01/26 19:54:28 johnsoc Exp $
**
****************************************************************/

#if !defined( IN_LTADDIN_LTSECURITYDUPLICATE_H )
#define IN_LTADDIN_LTSECURITYDUPLICATE_H

#include <ltaddin/base.h>
#include <secdb/LtSecDbObj.h>
#include <gsdt/GsDtDictionary.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_LTADDIN
LtSecDbObj * LtSecurityDuplicate(
	const LtSecDbObj &Security,				
	unsigned ValueFlags,					
	unsigned ValueFlagsReplace,
	const GsDtDictionary &ReplacementValues,
	const GsDtDictionary &ExcludedValues,			
	const GsDtDictionary &RecurisveReplacementValues
);

CC_END_NAMESPACE

#endif 
