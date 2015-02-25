/* $Header: /home/cvs/src/kool_ade/src/glob.h,v 1.9 2001/11/27 22:49:03 procmon Exp $ */
/****************************************************************
**
**	GLOB.H		Header for globbing routines
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_GLOB_H )
#define IN_GLOB_H


#ifndef IN_HASH_H
#include	<hash.h>
#endif

/*
**	Types
*/

typedef struct
{
	DIR		*g_dir;

	char	*g_pattern;

} GLOB;


DLLEXPORT GLOB
		*GlobOpen(			const char *Dir, const char *Pattern );

DLLEXPORT const char
		*GlobNext(			GLOB *Glob );

DLLEXPORT int
		GlobClose(			GLOB *Glob );

DLLEXPORT HASH
		*GlobHashBuild(		const char *Dir, const char *Pattern, int StatFlag );

DLLEXPORT void
		GlobHashDestroy(	HASH *Hash );
	
DLLEXPORT int GlobFindFirst(
	void *phandle,
	char *pattern,
	char *outbuf,
	int outsize,
	struct stat *st
);

DLLEXPORT int GlobFindNext(
	void *phandle,
	char *outbuf,
	int outsize,
	struct stat *st
);

DLLEXPORT int GlobFindClose(
	void *phandle
);

#endif /* !defined( IN_GLOB_H ) */
