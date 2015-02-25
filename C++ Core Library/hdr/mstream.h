/* $Header: /home/cvs/src/kool_ade/src/mstream.h,v 1.6 2001/11/27 22:49:07 procmon Exp $ */
/****************************************************************
**
**	MSTREAM.H	- Portable memory-mapped I/O 
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_MSTREAM_H )
#define IN_MSTREAM_H


typedef struct MStreamFileStructure MFILE;

DLLEXPORT int
		MClose(		MFILE *MFile );

DLLEXPORT MFILE
		*MOpen(		const char *Name,	const char *Modes );

DLLEXPORT size_t
		MRead(		void *Buffer,		size_t Size, size_t NItems, MFILE *MFile ),
		MWrite(		const void *Buffer,	size_t Size, size_t NItems, MFILE *MFile );

DLLEXPORT int
		MSeek(		MFILE *MFile, long Offset, int Whence );

DLLEXPORT long
		MTell(		MFILE *MFile );

DLLEXPORT int
		MSetLength(	MFILE *MFile, size_t Length );

#endif /* !defined( IN_MSTREAM_H ) */

