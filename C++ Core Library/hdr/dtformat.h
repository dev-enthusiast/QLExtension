/****************************************************************
**
**	DTFORMAT.H	- DT_MSG_FORMAT support
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/datatype/src/dtformat.h,v 1.5 2001/08/14 18:38:13 simpsk Exp $
**
****************************************************************/

#ifndef IN_DTFORMAT_H
#define IN_DTFORMAT_H

#include	<outform.h>


/*
**	Data type format control structure
*/

struct DT_MSG_FORMAT_INFO
{
	OUTPUT_FORMAT
			OutForm;	// Width, Decimal, Flags, ScaleAmt

	int		Code;		// Letter code from printf

	int		BufferSize;	// Allocated size of Buffer

	char	*Buffer;	// Pre-allocated buffer, can be realloc'd if needed
						// using DTM_FORMAT_ALLOC.  Can also be reset
						// to allocated string or whatever, but Buffer must
						// be free()d first.

};


/*
**	Define macro to allocate FormatInfo->Buffer
*/

#define DTM_FORMAT_ALLOC( fi, size )				\
		(((fi)->BufferSize >= (size))					\
				|| ((fi)->BufferSize = (size), (fi)->Buffer = (char *)realloc( (fi)->Buffer, (size) )) \
				|| Err( ERR_MEMORY, "DTM_FORMAT_ALLOC, @" ))

#endif

