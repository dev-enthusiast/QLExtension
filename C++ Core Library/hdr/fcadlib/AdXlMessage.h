/****************************************************************
**
**	ADXLMESSAGE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/AdXlMessage.h,v 1.4 2001/11/27 21:41:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADXLMESSAGE_H )
#define IN_FCADLIB_ADXLMESSAGE_H

#include <fcadlib/base.h>
#include <windows.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_FCADLIBX int AdXMessageBox( 
	HWND Window, 
	LPCSTR MessageText,
	LPCSTR Title,
	UINT Flags
);						 

CC_END_NAMESPACE

#endif 
