/****************************************************************
**
**	adgetmap.h		- Current mapping checker
**					- Determines whether running prod, pre, dev etc...
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**	
****************************************************************/


#if !defined( IN_ADGETMAP_H )
#define IN_ADGETMAP_H

#include <fcadlib/base.h>
#include <gscore/GsString.h>


CC_BEGIN_NAMESPACE( Gs )


EXPORT_CPP_FCADLIB int GetCurrentMapping(GsString &mapping);


CC_END_NAMESPACE


#endif	// IN_ADGETMAP_H
