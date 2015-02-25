/* $Header: /home/cvs/src/slang/src/x_file.h,v 1.5 2014/07/14 15:51:31 c01713 Exp $ */
/****************************************************************
**
**	X_FILE.H	- File open flags
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_X_FILE_H )
#define IN_X_FILE_H

// Do not use 0x0001 value!  It must not be used for compatibility reasons.
#define	FILE_OPEN_READ			0x0002
#define	FILE_OPEN_WRITE			0x0004
#define	FILE_OPEN_BINARY		0x0008
#define	FILE_OPEN_APPEND		0x0010
#define	FILE_OPEN_TRUNCATE		0x0020
#define	FILE_OPEN_DONT_CREATE	0x0040
#define	FILE_OPEN_MUST_CREATE	0x0080

#define	FILE_OPTION_NON_BLOCK	0x0001

#endif

