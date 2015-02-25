/****************************************************************
**
**	dynafns.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_DYNAFNS_H )
#define IN_DYNAFNS_H

#include <excelfns.h>

#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif
#ifndef IN_SECEXPR_H
#include <secexpr.h>
#endif


/*
**	Typedefs used for dynalinking functions. If function declarations change, these must be changed too
*/

typedef LPXLOPER (*DO_SLANG_FUNC)( const char *slang_string, XLOPER *xRetval );
typedef LPXLOPER (*GET_TSDB_FUNC)( LPXLOPER pxExpr, LPXLOPER pxStart, LPXLOPER pxEnd );
typedef LPXLOPER (*TSDB_UPDATE_FUNC)(LPXLOPER pxExpr, LPXLOPER pxDates, LPXLOPER pxValues);
typedef int (*SET_FLAG_FUNC)( int );
typedef LPXLOPER (*TSDB_VALUES_AT_DATES_FUNC)( LPXLOPER pxExpr, LPXLOPER pxDateRange );

/*
**	SlangScope setup by dynax_excel.dll
*/

DLLEXPORT SLANG_SCOPE
		*XlSlangScope;

DLLEXPORT SLANG_SCOPE
		*XlSlangSetup( void );		// Use instead of XlSlangScope

DLLEXPORT void
		slang_errlog( const char *format, ... );

DLLEXPORT LPXLOPER
		do_slang( const char *slang_string, XLOPER *xRetval ),					// DO_SLANG_FUNC
		TsdbForExcel(LPXLOPER pxExpr, LPXLOPER pxStart, LPXLOPER pxEnd),		// GET_TSDB_FUNC
		TsdbUpdateFunc(LPXLOPER pxExpr, LPXLOPER pxDates, LPXLOPER pxValues),	// TSDB_UPDATE_FUNC
		TsdbValuesAtDates(LPXLOPER pxExpr, LPXLOPER pxDateRange );				// TSDB_VALUES_AT_DATES_FUNC

		
DLLEXPORT int
		DoSlang( const char	*SlangString, DT_VALUE	*Result ),
		SetSlangDebugFlag( int Flag ),											// SET_FLAG_FUNC
		SetSlangErrorRetFlag( int Flag );										// SET_FLAG_FUNC

#endif

