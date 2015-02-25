/****************************************************************
**
**	GSFORMATUTILS.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFormatUtils.h,v 1.1 1999/12/05 11:21:14 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFORMATUTILS_H )
#define IN_GSQUANT_GSFORMATUTILS_H

#include <gsquant/base.h>
#include <gscore/types.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsDtArray * GsArrayTruncate( const GsDtArray &InputArray, unsigned MaxOutputSize, const GsString &ContinuedMessage );

CC_END_NAMESPACE

#endif 
