/****************************************************************
**
**	gscore/err.h	- GsErr functions
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/err.h,v 1.11 2001/11/27 22:41:12 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_ERR_H )
#define IN_GSCORE_ERR_H

#include <gscore/base.h>
#include <err.h>

#include <limits.h>
#include <math.h>


/*
**	Return GS_ERROR_DOUBLE or GS_ERROR_INT to indicate errors from functions
**	that return double or int.
**	SeeAlso: GsStatus
*/

// Why aren't these const ints?
// and why is GS_ in front of them, they should be in the Gs namespace
// why don't people understand namespaces?

#define GS_ERROR_DOUBLE	HUGE_VAL
#define GS_ERROR_INT	INT_MIN


CC_BEGIN_NAMESPACE( Gs )

// Deprecated API.  Just use GSX_THROW()
EXPORT_CPP_GSBASE GsStatus
	GsErrException();

CC_END_NAMESPACE

#endif


