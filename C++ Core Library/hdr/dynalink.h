/****************************************************************
**
**	DYNALINK.H	- Dynamic function linking
**
**	$Header: /home/cvs/src/kool_ade/src/dynalink.h,v 1.16 2001/11/27 22:49:01 procmon Exp $
**
****************************************************************/

#if !defined( IN_DYNALINK_H )
#define IN_DYNALINK_H


#include <dllver.h>


/*
**	Dll information structure
*/

typedef struct DllAllowedVersionStruct DLL_ALLOWED_VERSION;

struct DllAllowedVersionStruct
{
	DLL_ALLOWED_VERSION
			*Next;

	char	*Area;				// NULL for any area

	long	MajorLow,			// Low and High are inclusive
			MajorHigh,
			MinorLow,
			MinorHigh,
			CompileLow,
			CompileHigh;

};

typedef struct
{
	char	*DllName,			// "kool_ade"
			*DllLoadPath;		// "u:\dll\kool_ade.dll"

	int		Ok,					// TRUE if version already checked
			Checking;			// TRUE if currently checking this DLL

	DLL_ALLOWED_VERSION
			*Allowed;			// NULL for any version

	DLL_VERSION
			*DllVersion;		// might be NULL

} DLL_VERSION_INFO;


/*
**	Hash table of DLL_VERSION_INFOs by DllName
*/

#ifdef IN_HASH_H

DLLEXPORT HASH
		*DynaLinkVersionHash;

#endif


/*
**	Prototype functions
*/

typedef void (DYNALINK_FUNC)( void );

DLLEXPORT DYNALINK_FUNC
		*DynaLink(					const char *DllPath, const char *FuncName );

DLLEXPORT void
		*DynaLinkPtr(				const char *DllPath, const char *FuncName );

DLLEXPORT int
		DynaLinkCheckAllVersions(	void ),
		DynaLinkGetLoadPath(		DLL_VERSION_INFO *Info ),
		DynaLinkIsLoaded(			const char *DllPath ),
		DynaLinkLoadConfig(			void );

DLLEXPORT DLL_VERSION
		*DynaLinkGetVersion(		const char *DllPath );

#endif
