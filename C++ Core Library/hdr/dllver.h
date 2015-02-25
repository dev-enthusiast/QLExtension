/****************************************************************
**
**	dllver.h - DLL version structure
**
**	Copyright 1993-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/dllver.h,v 1.4 2002/01/21 21:44:49 khodod Exp $
**
****************************************************************/

#ifndef IN_VERSION_H
#define IN_VERSION_H

/*
**	Structure used to hold DLL version information
*/

struct DllVersionStructure 
{
	char	*Version,		// Contains string after VeRsIoN=...
			*Message,		// Contains string after CoPyRiGhT=...
			*Name;			// Contains string after NaMe SeRvIcE=...

	long	Major,
			Minor,
			CompileTime,
			CompileNumber;

	char	*Area;			// "core", "strat", "version/v13Dec93", ...

	DLL_VERSION
			*Next;

};

#endif

