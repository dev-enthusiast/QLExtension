/****************************************************************
**
**	port_int.h	- Goldman version of <stdint.h>
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/port_int.h,v 1.15 2004/11/12 16:09:42 khodod Exp $
**
****************************************************************/

#if !defined( IN_PORT_INT_H )
#define IN_PORT_INT_H

/*
**	These are the new integer types
*/

#if defined( CC_STDINT_HAVE )

#include <stdint.h>

#elif defined( CC_STDINT_USE_INTTYPES )

#include <inttypes.h>

#else	// no header file support

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

#ifdef WIN32
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
typedef long long int64_t;
typedef unsigned long long uint64_t;
#endif

typedef long intptr_t;
typedef unsigned long uintptr_t;

#endif

/*
**	These are the old integer types.
**
**	Don't use them for new code.  Eliminate them from old code.
*/

#if defined( WIN32 )
typedef int				INT32;
typedef unsigned int	UINT32;
#else
typedef long			INT32;
typedef unsigned long	UINT32;
#endif

typedef short			INT16;
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;

#ifndef UINT32_MAX
#define UINT32_MAX	(4294967295U)
#endif

#endif
