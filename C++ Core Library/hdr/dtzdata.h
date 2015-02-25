/*$Header: /home/cvs/src/supermkt/src/dtzdata.h,v 1.9 2001/01/30 18:16:21 paints Exp $*/
/****************************************************************
**
**	dtzdata.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#ifndef IN_DTZDATA_H
#define IN_DTZDATA_H

#include <time.h>
#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif
#ifndef IN_ZTCURVE_H
#include <ztcurve.h>
#endif

#define DEFAULT_ZLEVEL 6

typedef struct DtZDataStructure
{
    time_t start,       // start time
           step;        // time differential
    int    numknots;    // number of knots (samples)

    
    ZTCURVE *ZCurve;    // compressed curve representation
    DT_TCURVE *TCurve;  // the above curve uncompressed
    
} DT_ZDATA;

DLLEXPORT DT_DATA_TYPE DtZData;
DLLEXPORT int DtFuncZData(int, DT_VALUE *, DT_VALUE *, DT_VALUE *);


DLLEXPORT DT_BINARY
		*GsGZip( DT_BINARY *input, int zlevel ),
		*GsGUnzip( DT_BINARY *input );


#endif // IN_DTZDATA_H
