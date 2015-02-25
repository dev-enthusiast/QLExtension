/****************************************************************
**
**	fcadlib/adxlmain.h	- Implementation of Excel entry points
**
**	Excel requires that certain functions be defined in the
**	add-in DLL.  This file defines those functions and some
**	utility addin functions like AdlibVersion() and ErrGetString()
**
**	Usage:
**		#define AD_X_CATEGORY_NAME	"Excel Category for these functions"
**		#define AD_X_LIBRARY_NAME	"Used in version string"
**		#define AD_X_LONG_NAME		"Displayed in Tools/Addins menu"
**		XlFuncEntry XlFuncTable[] = { ... };
**
**		// The menu is Optional
**		#define AD_X_MENU XlMenuTable
**		XlMenuEntry XlMenuTable[] = { ... };
**
**		#include <fcadlib/adxlmain.h>
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/adxlmain.h,v 1.7 2001/02/22 21:39:45 simpsk Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADXLMAIN_H )
#define IN_FCADLIB_ADXLMAIN_H

#ifndef AD_X_CATEGORY_NAME
#error adxlmain.h: AD_X_CATEGORY_NAME must be defined
#endif

#ifndef AD_X_LIBRARY_NAME
#error adxlmain.h: AD_X_LIBRARY_NAME must be defined
#endif

#ifndef AD_X_LONG_NAME
#error adxlmain.h: AD_X_LONG_NAME must be defined
#endif

// Default to no menu
#ifndef AD_X_MENU
#define AD_X_MENU	NULL
#endif



/*
**	The main entry points for Excel
*/

DLLEXPORT int		xlAutoOpen();
DLLEXPORT LPXLOPER	xlAutoRegister(		LPXLOPER pxName);
DLLEXPORT LPXLOPER  xlAddInManagerInfo(	LPXLOPER xAction);
DLLEXPORT void xlAutoFree( LPXLOPER pxFree );


/*
**	Functions to be exported as Addins
*/

DLLEXPORT char *ExAdlibVersion(void);
DLLEXPORT char *ExErrGetString(void);


/*
**	A buffer to hold the version of this library
*/

static char
		VersionString[ 256 ];


/****************************************************************
**	Routine: ExAdlibVersion
**	Returns: static char *
**	Action : Return the addin version string
****************************************************************/

char *ExAdlibVersion()
{
	return VersionString;
}



/****************************************************************
**	Routine: ExErrGetString
**	Returns: static char *
**	Action : Return ErrGetString()
****************************************************************/

char *ExErrGetString()
{
	// const_cast<>?
	return (char *) ErrGetString();
}



/****************************************************************
**	Routine: xlAutoOpen
**	Returns: TRUE/FALSE
**	Action : Called when this XLL is loaded
****************************************************************/

int xlAutoOpen()
{
	int		Status;

	char	*xDll;	// Hold the DLL name


	if( !GS_XL_INIT() )
		return FALSE;

	xDll = GsXlDllGetName();
	Status = AdXAutoOpen( XlFuncTable,
						  AD_X_MENU,
                          xDll,
                          AD_X_CATEGORY_NAME,
                          AD_X_LIBRARY_NAME,
                          __DATE__ " " __TIME__,
                          VersionString );
	free( xDll );
	return Status;
}



/****************************************************************
**	Routine: xlAutoRegister
**	Returns: Error always
**	Action : All functions are already registered by xlAutoOpen,
**			 so this function should never be called.
****************************************************************/

LPXLOPER xlAutoRegister(
	LPXLOPER	pxName
)
{
	static XLOPER xRegId;

	xRegId.xltype = xltypeErr;
	xRegId.val.err = xlerrValue;

	return &xRegId;
}



/****************************************************************
**	Routine: xlAutoFree
**	Returns: nothing
**	Action : Free memory from this DLL with xlbitDLLFree set
****************************************************************/

void xlAutoFree(
	LPXLOPER	pxFree
)
{
	GsXlOperFreeContents( *pxFree );
	delete pxFree;
}



/****************************************************************
**	Routine: xlAddInManagerInfo
**	Returns:
**	Action : Provide add-in library name to Excel
****************************************************************/

LPXLOPER xlAddInManagerInfo(
	LPXLOPER	xAction
)
{
	return AdXAddInManagerInfo( xAction, AD_X_LONG_NAME );
}

#endif

