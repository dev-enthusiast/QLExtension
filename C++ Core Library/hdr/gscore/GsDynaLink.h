/****************************************************************
**
**	gscore/GsDynaLink.h	- Dynamic linking from C++ (w/ exceptions)
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsDynaLink.h,v 1.3 2001/11/27 22:41:03 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSDYNALINK_H )
#define IN_GSCORE_GSDYNALINK_H

#include	<gscore/base.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**  Type of function for GsDynaLinkCall
*/

typedef void (*GsDynaLinkFunc)( void );



EXPORT_CPP_GSBASE void
	*GsDynaLink(	const char *DllPath, const char *FuncName ),
	GsDynaLinkCall(	const char *DllPath, const char *FuncName );

CC_END_NAMESPACE
#endif

