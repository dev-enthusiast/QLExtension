/****************************************************************
**
**	fcadlib/gsexcel.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/gsexcel.h,v 1.9 2012/03/01 20:14:29 e19351 Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_GSEXCEL_H )
#define IN_FCADLIB_GSEXCEL_H

#include	<windows.h>
#include	<time.h>
#include	<fcadlib/base.h>
#include	<fcadlib/xlcall.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Typedefs for functions in Excel library
*/

typedef int far cdecl
		(*XLFUNC_EXCEL4)( int xlfn, LPXLOPER operRes, int count,... );
typedef int far cdecl
		(*XLFUNC_EXCEL4V)( int xlfn, LPXLOPER operRes, int count, LPXLOPER far opers[]);
typedef int far cdecl
		(*XLFUNC_XLCALLVER)( void );

extern EXPORT_CPP_GSEXCEL XLFUNC_EXCEL4
		GsXlExcel4Func;

extern EXPORT_CPP_GSEXCEL XLFUNC_EXCEL4V
		GsXlExcel4vFunc;

#define GS_XL_INIT()	( CC_NS(Gs,GsXlExcel4Func) || GsXlInit())
#define EXCEL4			(*CC_NS(Gs,GsXlExcel4Func))
#define EXCEL4V			(*CC_NS(Gs,GsXlExcel4vFunc))

/*
**	Exported functions
*/

EXPORT_CPP_GSEXCEL GsStatus
		GsXlInit();

EXPORT_CPP_GSEXCEL time_t
		GsXlExpiration();

EXPORT_CPP_GSEXCEL void
		GsXlOperAllocMulti(		XLOPER &Val, int Rows, int Cols ),
		GsXlOperAllocStr(		XLOPER &Val, const char *Str ),
		GsXlOperFreeContents(	XLOPER &Val );

inline void
		GsXlOperBecomeError(	XLOPER& Val );

EXPORT_CPP_GSEXCEL int
		GsXlDllLoad( 
			const char	*DllToLoad, 
			GsBool		Optional,
			const char	*CategoryName = NULL );	// (optional) name of category to override one specified by addin DLL

EXPORT_CPP_GSEXCEL char *
		GsXlDllGetName( void );



/****************************************************************
**	Routine: GsXlOperBecomeError
**	Returns: nothing
**	Action : Convert Val into an excel error type
****************************************************************/

inline void GsXlOperBecomeError(
	XLOPER&	Val
)
{
	Val.xltype  = xltypeErr;
	Val.val.err = xlerrValue;
}

CC_END_NAMESPACE
#endif

