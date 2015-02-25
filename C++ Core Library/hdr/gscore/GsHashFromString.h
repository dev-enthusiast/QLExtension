/****************************************************************
**
**	GSHASHFROMSTRING.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsHashFromString.h,v 1.3 2001/11/27 22:41:06 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSBASE_GSHASHFROMSTRING_H )
#define IN_GSBASE_GSHASHFROMSTRING_H

#include <gscore/base.h>
#include <gscore/GsString.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSBASE GsHashFromString
{
public:
	unsigned long operator()( const GsString &KeyString ) const;
};


CC_END_NAMESPACE

#endif 
