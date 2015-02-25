/****************************************************************
**
**	MANYFILE.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/manyfile.h,v 1.8 2001/11/27 23:35:01 procmon Exp $
**
****************************************************************/

#if !defined( IN_MANYFILE_H )
#define IN_MANYFILE_H

/*
**	Prototype functions
*/

int		FileInit(		int MaxActive, int MaxFiles );
void	FileCloseAll(	void );

int		FileOpen(		char *filename, int oflag, int pmode );
int		FileClose(		int handle );
DLLEXPORT int		FileRead(		int handle, void *buffer, unsigned count );
int		FileWrite(		int handle, void *buffer, unsigned count );
DLLEXPORT long	FileSeek(		int handle, long offset, int origin );
DLLEXPORT long	FileTell(		int handle );

#endif

