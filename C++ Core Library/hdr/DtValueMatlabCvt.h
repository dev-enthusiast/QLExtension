/****************************************************************
**
**	DTVALUEMATLABCVT.H	- DT_VALUE <--> Matlab   conversion routines
**
**
**	$Header: /home/cvs/src/mtglue/src/DtValueMatlabCvt.h,v 1.3 2012/08/13 14:51:28 e19351 Exp $
**
****************************************************************/

#ifndef IN_MTGLUE_DTVALUEMATLAB_H
#define IN_MTGLUE_DTVALUEMATLAB_H

#include <datatype.h>

// Matlab2007's API does not accomodate VC10's improved but still incorrect
// C99 support.
#if defined(_MSC_VER) && _MSC_VER >= 1600
#	include <yvals.h>
#	ifndef __STDC_UTF_16__
#		define __STDC_UTF_16__
#	endif
#endif
#include <mex.h>


CC_BEGIN_NAMESPACE( Gs )

typedef void (*WarningHandler)(const char * warning);

extern "C" void DtValueFromMatlab( const mxArray *Val, DT_VALUE * DtVal, WarningHandler warnHandler = 0);
extern "C" mxArray * DtValueToMatlab( const DT_VALUE * DtVal);

CC_END_NAMESPACE

#endif 
