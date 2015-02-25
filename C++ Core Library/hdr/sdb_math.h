/************************************************************************
**
** $Header: /home/cvs/src/portable/src/sdb_math.h,v 1.9 2012/03/28 14:05:14 khodod Exp $
** 
** Common math functions.
**
** Copyright (c) 2012 - Constellation Energy Group, Inc.
**
************************************************************************/

#ifndef __SDB_MATH
#define __SDB_MATH

#include <port_int.h>
#include <math.h>     // Needed for the exception struct.
#ifdef WIN32
#include <float.h>
#include <stdlib.h>   // for atoi().
#endif


// Unfortunately, there's no standard c++ name for math exceptions.
#if defined( WIN32 )
typedef struct _exception SDB_MATHEXCEPT;
#elif defined( __cplusplus )
#	if defined( linux )
typedef struct __exception SDB_MATHEXCEPT;
#	else
typedef struct __math_exception SDB_MATHEXCEPT;
#	endif
#else
typedef struct exception SDB_MATHEXCEPT;
#endif


#if BYTE_ORDER == LITTLE_ENDIAN

#define HIWORD 1
#define LOWORD 0

#else

#define HIWORD 0
#define LOWORD 1

#endif

static union {
    int    i[2];
    double d;
} __u_d;

inline int   __HI(double x) 
{ 
    __u_d.d = x; return __u_d.i[HIWORD]; 
}

inline int   __LO(double x) 
{ 
    __u_d.d = x; return __u_d.i[LOWORD]; 
}

inline void  __HIset(double& x, int hx) 
{ 
    __u_d.d = x; __u_d.i[HIWORD] = hx; x = __u_d.d;  
}

inline void  __LOset(double& x, int lx) 
{ 
    __u_d.d = x; __u_d.i[LOWORD] = lx; x =__u_d.d; 
}

#ifdef WIN32

#define copysign      _copysign
#define finite        _finite

inline bool signbit(double x) 
{
    return _copysign(1.0, x) < 0; 
} 

/* replacement of Unix rint() for Windows */

static int rint (double x)
{
    char *buf;
    int i,dec,sig;

    buf = _fcvt(x, 0, &dec, &sig);
    i = atoi(buf);
    if(sig == 1) {
        i = i * -1;
    }
    return(i);
}

#endif

#ifdef sparc
#undef signbit
#define signbit( x ) __builtin_signbit( x )
#endif

extern double sdb_SVID_libm_err( double x, double y, int type );

#endif // __SDB_MATH
