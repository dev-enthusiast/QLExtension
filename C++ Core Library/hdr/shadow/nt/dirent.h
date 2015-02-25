/* $Header: /home/cvs/src/portable/src/shadow/nt_vc10/dirent.h,v 1.1 2012/05/31 17:48:27 e19351 Exp $ */
/****************************************************************
**
**	DIRENT.H	Header for directory reading routines
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_DIRENT_H )
#define IN_DIRENT_H

#include <direct.h>

/*
**	Routines to emulate POSIX directory calls under OS/2 (sort of)
**	(plus some stuff that should be sys/stat.h)
*/

struct  dirent
{
	/* This is the only field required by POSIX, so we discard the rest */
	char            d_name[255+1];  // name (up to MAXNAMLEN + 1)
};

typedef struct
{
	struct dirent	dir_dirent;		// Last dirent returned
	void			*dir_info;		// Private information
} DIR;


DLLEXPORT DIR *opendir( const char *dirname);
DLLEXPORT int closedir( DIR  *dirp);
DLLEXPORT void rewinddir( DIR  *dirp);
DLLEXPORT struct dirent *readdir( DIR  *dirp);


/*
**	Some macros to be expected in sys/stat.h on a POSIX system
*/

#define		S_ISDIR(m)		((m) & S_IFDIR)
#define		S_ISCHR(m)		((m) & S_IFCHR)
#define		S_ISBLK(m)		0
#define		S_ISREG(m)		((m) & S_IFREG)
#define		S_ISFIFO(m)		0
	
#endif /* !defined( IN_DIRENT_H ) */
