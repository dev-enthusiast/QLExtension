/****************************************************************
**
**	SELFDESCSTREAM.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/datatype/src/datatype/SelfDescStream.h,v 1.4 2001/07/30 15:27:53 vengrd Exp $
**
****************************************************************/

#ifndef IN_DATATYPE_SELFDESCSTREAM_H
#define IN_DATATYPE_SELFDESCSTREAM_H

#include <datatype/base.h>
#include <datatype.h>
#include <bstream.h>

CC_BEGIN_NAMESPACE( Gs )

// Versions of self-describing streams:

enum GS_SELF_DESC_VERSION
{
	GS_SELF_DESC_VERSION_STRUCTURE = 1,
	GS_SELF_DESC_VERSION_GZIP  	   = 2
};


// Operations that can generate self-describing streams:

enum GS_SELF_DESC_OPERATION
{
	GS_SELF_DESC_ADD			= 1,	// New object added
	GS_SELF_DESC_UPDATE			= 2,	// Object updated
	GS_SELF_DESC_DELETE			= 3,	// Object deleted
	GS_SELF_DESC_ADD_OR_UPDATE	= 4		// Add if not present.  Update if present.  For rebuilding indices.
};


EXPORT_CPP_DATATYPE int GsSelfDescStreamToStruct(
	BSTREAM	   *BStream,
	DT_VALUE   *OutputStruct
);


CC_END_NAMESPACE

#endif 
