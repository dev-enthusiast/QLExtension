/* $Header: /home/cvs/src/fcadlib/src/fcadlib/adexcellog.h,v 1.10 2001/03/13 13:03:11 goodfj Exp $ */
/****************************************************************
**
**	fcadlib/adexcellog.h	- Excel addin audit trail features
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/


#if !defined( IN_FCADLIB_ADEXCELLOG_H )
#define IN_FCADLIB_ADEXCELLOG_H

#define ADLIB_EXCEL

#include	<kool_ade.h>
#include	<fcadlib/base.h>
#include	<fcadlib/xlcall.h>
#include    <gscore/GsString.h>


enum ADXLLOG_CONVERSIONFLAG {
	ADXLLOG_NOCONVERSION = 0,
	ADXLLOG_DATE,
	ADXLLOG_HANDLE,
	ADXLLOG_STRUCTURE
};


EXPORT_CPP_FCADLIBX void AdXLLogGetCellAddress(
	long		row,
	long		col,
	CC_NS(Gs,GsString)	&retAddress,
	bool		bRowAbsolute = false,
	bool		bColAbsolute = false
);


EXPORT_CPP_FCADLIBX  bool AdXLLogGetExternalRefAddress(
	LPXLOPER		xlRef,
	CC_NS(Gs,GsString)		&retAddr
);


EXPORT_CPP_FCADLIBX  bool AdXLLogGetCallerInformation(
	CC_NS(Gs,GsString)	&callerInfo
);


EXPORT_CPP_FCADLIBX  void AdXLLogGetArgDesc(
	LPXLOPER		pxlArg,
	CC_NS(Gs,GsString)		&argDesc,
	ADXLLOG_CONVERSIONFLAG			// Underlying type information.
					argConvFlag = ADXLLOG_NOCONVERSION,
	bool			bDefineAssignment = false	// Declare new variables for an assignment
);


// Same as GetArgDesc but can handle matrices.
EXPORT_CPP_FCADLIBX  void AdXLLogGetArgDescMatrix(
	LPXLOPER		pxlArg,
	CC_NS(Gs,GsString)		&argDesc,
	ADXLLOG_CONVERSIONFLAG			// Underlying type information.
					argConvFlag = ADXLLOG_NOCONVERSION,
	bool			bDefineAssignment = false	// Declare new variables for an assignment
);


EXPORT_CPP_FCADLIBX  bool AdXLLogvGetFunctionSig(
	CC_NS(Gs,GsString)	&outputStr,
	CC_NS(Gs,GsString)	fnName,
	va_list		vl,					// va_start and va_end must be done by the caller of this function.
	ADXLLOG_CONVERSIONFLAG			// Array of underlying type information.
				*argConvList = NULL// Pass in as NULL if not interested.
);


EXPORT_CPP_FCADLIBX  bool AdXLLogGetFunctionSig(
	CC_NS(Gs,GsString)	&outputStr,
	CC_NS(Gs,GsString)	fnName,
	ADXLLOG_CONVERSIONFLAG			// Array of underlying type information.
				*argConvList,		// Pass in as NULL if not interested.
	...								// LPXLOPER inputs, terminated by NULL
);


EXPORT_CPP_FCADLIBX  void AdXLLogWriteText(
	CC_NS(Gs,GsString)	&msgText
);


EXPORT_CPP_FCADLIBX void AdXLLogTraceSlangExpr(
	CC_NS(Gs,GsString)	slangExpr
);


EXPORT_CPP_FCADLIBX  void AdXLLogFunctionTraceCall(
	CC_NS(Gs,GsString)	fnName,
	ADXLLOG_CONVERSIONFLAG		// Array of underlying type information.
				*argConvList,	// Pass in as NULL if not interested.
	...							// LPXLOPER inputs, terminated by NULL
);


EXPORT_CPP_FCADLIBX  void AdXLLogFunctionTraceResult(
	LPXLOPER	xlResult,
	ADXLLOG_CONVERSIONFLAG
				resultConversion = ADXLLOG_NOCONVERSION
);


EXPORT_CPP_FCADLIBX  void AdXLSetErrorLogStatus(
	bool		bLogToFile
);


EXPORT_CPP_FCADLIBX  bool AdXLGetErrorLogStatus();


EXPORT_CPP_FCADLIBX  void AdXLSetErrorLogFileName(
	CC_NS(Gs,GsString)	FileName
);


EXPORT_CPP_FCADLIBX  void AdXLGetErrorLogFileName(
	CC_NS(Gs,GsString)	&FileName
);

EXPORT_CPP_FCADLIBX  bool AdXLEvaluateFormula(
	CC_NS(Gs,GsString)	&command,
	LPXLOPER	pxlResult
);

EXPORT_CPP_FCADLIBX  bool GetSerialProcessTime(
	double		&cpuTime
);

#endif // IN_FCADLIB_ADEXCELLOG_H
