/***********************************************************************
**
**	AdUsageReporting.h - Reports use of adlib functions via UDP 
**                             packets.
**
**	Copyright (c) 2011 - Constellation Energy Group, Baltimore, MD
**	
***********************************************************************/
#ifndef IN_FCADLIB_ADUSAGEREPORTING_H
#define IN_FCADLIB_ADUSAGEREPORTING_H

#include <fcadlib/base.h>

namespace SecDb {

// Check whether usage reporting is active for this session.
EXPORT_CPP_FCADLIB bool AdIsUsageReportingActive();

// Report adlib function usage via UDP to server specified in config.
EXPORT_CPP_FCADLIB void AdReportUsage(
	const char* functionName, 
	const char* platform,
	const char* process,
	const char* script
);

}

#endif