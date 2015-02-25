/* $Header: /home/cvs/src/kool_ade/src/fileapp.h,v 1.8 2001/11/27 22:49:02 procmon Exp $ */
/****************************************************************
**
**	FILEAPP.H	
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_FILEAPP_H )
#define IN_FILEAPP_H


typedef char FILE_APPEND;

/*
**	Prototypes
*/

DLLEXPORT FILE_APPEND
		*FileAppendOpen(		char *FileName );

DLLEXPORT int
		FileAppendBuffer(		FILE_APPEND *File, void *Buffer, int Count ),
		FileAppendFlush(		FILE_APPEND *File ),
		FileAppendWrite(		FILE_APPEND *File, void *Buffer, int Count );

DLLEXPORT void
		FileAppendClose(		FILE_APPEND *File );

DLLEXPORT char
		*FileAppendName(		FILE_APPEND *File );

DLLEXPORT int
		FileAppendClear(		FILE_APPEND *File );

DLLEXPORT void
		FileAppendLastWrite(	FILE_APPEND *File, long *Pos, long *Size );

#endif
