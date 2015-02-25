/**************************************************************************
 * File:  OCIENGINE.H
 *
 * File Description: 
 *		Declaration of user callable OCI functions for MATLAB.
 *
 * $Log: ociengine.h,v $
 * Revision 1.1  2011/07/07 21:38:41  c07426
 * Initial commit for Matlab OCI Integration.
 * AWR#: 236533
 *
 *
 * $Revision: 1.1 $ $Date: 2011/07/07 21:38:41 $
 **************************************************************************/

#ifndef _OCIENGINE_H_
#define _OCIENGINE_H_

#include "ociutil.h"
#include "cdtable.h"

#define OPTION          (mxArray *)prhs[ 0 ]

/* For "open" */

#define DBNAME          (mxArray *)prhs[ 1 ]
#define UNAME           (mxArray *)prhs[ 2 ]
#define PWORD           (mxArray *)prhs[ 3 ]
#define DAT_TIM			(mxArray *)prhs[ 4 ]
#define CLIENT_INFO		(mxArray *)prhs[ 5 ] 

/* For "exec" */

#define CONN            (mxArray *)prhs[ 1 ]	/* For "close" , "rpc" */
#define QUERY           (mxArray *)prhs[ 2 ]

/* For "rpc" */

#define RPCNAME			(mxArray *)prhs[ 2 ]
#define RPCARGS			(mxArray *)prhs[ 3 ]


const int VALID_NUMERIC_RANGE   = 1477;
const int VALID_CHARACTER_RANGE = 32512;
const int VALID_DATE_RANGE      = 4644;

mxArray *OpenConnection( mxArray *, mxArray *, mxArray *, mxArray *, mxArray * );
void     CloseConnection( mxArray * );
mxArray *ExecuteSQL( mxArray *, mxArray * );
mxArray *ExecuteRPC( mxArray *, mxArray *, mxArray * );


#endif
