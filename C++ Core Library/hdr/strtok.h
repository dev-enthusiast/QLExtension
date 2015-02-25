/* $Header: /home/cvs/src/kool_ade/src/strtok.h,v 1.6 2001/11/27 22:49:10 procmon Exp $ */
/****************************************************************
**
**	STRTOK.H	
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_STRTOK_H )
#define IN_STRTOK_H



typedef struct StringTokenStructure
{
	char	*String,
			*Cur;

} *STRING_TOKEN_HANDLE;



DLLEXPORT STRING_TOKEN_HANDLE
		StringTokenStart(	char *String );

DLLEXPORT char
		*StringToken(		STRING_TOKEN_HANDLE Handle, char *Delimiters );

DLLEXPORT void
		StringTokenEnd(		STRING_TOKEN_HANDLE Handle );

#endif
