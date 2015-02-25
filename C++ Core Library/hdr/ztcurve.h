/*$Header: /home/cvs/src/supermkt/src/ztcurve.h,v 1.9 2001/01/26 12:33:06 goodfj Exp $*/
/****************************************************************
**
**	ztcurve.h	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#ifndef IN_ZTCURVE_H
#define IN_ZTCURVE_H

#include "dtzdata.h"

typedef struct ZTCurveStructure
{
    int zflag;     // set to 1 if the data is compressed

    /*
     * Uncompressed representation:
     * knots - simply an array of knots.
     * numknots - number of knots.
     */

    int numknots; 
    double *knots;    
    
    /*
     * Compressed representation:
     * zlen    - length of zbuf
     * zstream - buffer of compressed data
     */
    
    int zlen;
    unsigned char *zbuf;
    
} ZTCURVE;

extern void     ZTCurveFree(ZTCURVE *);
extern ZTCURVE *ZTCurveAlloc(void);
extern int      ZTCurveSize(ZTCURVE *);
extern ZTCURVE *ZTCurveCopy(ZTCURVE *);
extern int      ZTCurveCompare(ZTCURVE *, ZTCURVE *);
extern unsigned long ZTCurveHash(ZTCURVE *, DT_HASH_CODE);

extern ZTCURVE *ZTCurveEncode(int, double *, int);
extern double  *ZTCurveDecode(ZTCURVE *, int *);

#endif // IN_ZTCURVE_H
