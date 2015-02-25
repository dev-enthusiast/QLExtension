/****************************************************************
**
**	fcadlib/adlib.h	- Common adlib header
**
**	#define ADLIB_SLANG or ADLIB_EXCEL before including this file
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/adlib.h,v 1.33 2012/02/27 13:47:00 e19351 Exp $
**
****************************************************************/

#ifndef IN_FCADLIB_ADLIB_H
#define IN_FCADLIB_ADLIB_H

#include <gscore/gsdt_fwd.h>

#include <fcadlib/base.h>
#include <fcadlib/autoarg.h>
#include <fcadlib/AdExpandArg.h>
#include <fcadlib/AdMissingArg.h>
#include <fcadlib/adrdate.h>
#include <fcadlib/adhandle.h>


/*
**	These gscore/gsdt includes are just here temporarily. They should not be here. They will be removed
**	when all the ensuing downstream breakage is resolved.
*/

#include <gsquant/Enums.h>
#include <gscore/err.h>
#include <gscore/GsString.h>
#include <gscore/GsSymbol.h>
#include <gscore/GsVector.h>
#include <gscore/GsMatrix.h>
#include <gscore/GsDate.h>
#include <gscore/GsTimeSeries.h>
#include <gsdt/GsDt.h>
#include <gsdt/GsDtDouble.h>
#include <gsdt/GsDtGeneric.h>
#include <gsdt/GsDtGenVector.h>
#include <gsdt/GsType.h>
#include <gsdt/GsDtVector.h>
#include <gsdt/GsDtArray.h>
#include <gsdt/GsDtMatrix.h>
#include <gsdt/GsDtTensor.h>
#include <gsdt/GsDtString.h>
#include <gsdt/GsDtDate.h>
#include <gsdt/GsDtSymDate.h>
#include <gsdt/GsDtRDate.h>
#include <gsdt/GsDtDayCountISDA.h>
#include <gsdt/GsDtDayCount.h>
#include <gsdt/GsDtTypedVector.h>

// Template type parameter if desired
//#define TT(Type) <Type>
#define TT(Type)

//#define ADLIB_SLANG
#if defined( ADLIB_SLANG )
#include <fcadlib/adslang.h>
#endif


//#define ADLIB_EXCEL
#if defined( ADLIB_EXCEL )
#include <fcadlib/adexcel.h>
#include <fcadlib/adexcellog.h>
#endif

#endif

