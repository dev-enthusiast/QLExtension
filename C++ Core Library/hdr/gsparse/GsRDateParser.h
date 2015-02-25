/****************************************************************
**
**	GSRDATEPARSER.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsparse/src/gsparse/GsRDateParser.h,v 1.1 1999/10/29 14:51:02 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSPARSE_GSRDATEPARSER_H )
#define IN_GSPARSE_GSRDATEPARSER_H

#include <gsparse/base.h>
#include <gscore/GsRDate.h>
#include <gscore/GsStringVector.h>
#include <gscore/GsIntegerVector.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSPARSE GsRDateParser
{	
	GsRDateParser() {}
public:
	static GsRDate fromString( const GsString& String );
};

EXPORT_CPP_GSPARSE GsStringVector
		GsRDateTokenize( const GsString& );

CC_END_NAMESPACE

#endif 



