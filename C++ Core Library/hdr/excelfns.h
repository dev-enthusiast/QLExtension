/****************************************************************
**
**	excelfns.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_EXCELFNS_H )
#define IN_EXCELFNS_H


#ifndef _IN_DATATYPE_H
#include <datatype.h>
#endif
// RAA 26-9-2000 Using xlcall from fcadlib to prevent multiple includes
#include <windows.h>
#include <fcadlib/xlcall.h>

/*
**	Export some variables
*/

DLLEXPORT LOGFONT
		PrintFont;

DLLEXPORT char
		VersionString[ 256 ];


/*
**	Typedefs for functions in Excel library
*/

/*

typedef int far cdecl
		(*XLFUNC_EXCEL4)( int xlfn, LPXLOPER operRes, int count,... );
typedef int far cdecl
		(*XLFUNC_EXCEL4V)( int xlfn, LPXLOPER operRes, int count, LPXLOPER far opers[]);
typedef int far cdecl
		(*XLFUNC_XLCALLVER)( void );



/*
**	Export functions
*/

/*
DLLEXPORT XLFUNC_EXCEL4
		XlFuncExcel4;
*/

DLLEXPORT void
		DtValueToXloper( DT_VALUE *Value, XLOPER *xRetval );

DLLEXPORT void 
		xloper_string(XLOPER *x, char *s);

DLLEXPORT int
		ExcelInit( void );

DLLEXPORT int 
		ExcelErr( int ExcelStatus, char *FuncName );
		
DLLEXPORT LRESULT CALLBACK
		msgframe_winproc(HWND hwnd, UINT msg, WPARAM mp1, LPARAM mp2);
		
DLLEXPORT LRESULT CALLBACK
		printframe_winproc(HWND hwnd, UINT msg, WPARAM mp1, LPARAM mp2);


/*
**	Macro definitions
*/

/*
#if !defined(EXCEL4)
#define EXCEL4	(*XlFuncExcel4)
#endif
*/

//	if( !XlFuncExcel4 && !ExcelInit())							
#define EXCEL_INIT()											\
	if( !ExcelInit())							\
		{														\
			SLANG_ARG_FREE();									\
			return SlangEvalError( SLANG_ERROR_PARMS, NULL );	\
		}
		
//#define EXCEL_INIT_NO_SLANG() ( XlFuncExcel4 || ExcelInit() )
#define EXCEL_INIT_NO_SLANG() ( ExcelInit() )
				   
#define MENUID_COPY			100
#define MENUID_WIPE			101
#define MENUID_FONT			102
#define MENUID_HELP			103
#define MENUID_PRINT		104

#endif

