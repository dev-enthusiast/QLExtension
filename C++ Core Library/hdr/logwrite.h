/* $Header: /home/cvs/src/it_utils/src/logwrite.h,v 1.9 1998/10/09 22:14:33 procmon Exp $ */
/****************************************************************
**
**	LOGWRITE.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	Brian Weston
**	
**	$Revision: 1.9 $
**
****************************************************************/


#if !defined( IN_LOGWRITE_H )
#define IN_LOGWRITE_H

DLLEXPORT void  LogSwitch   (const char *filename);
DLLEXPORT void  LogWrite    (const char *msg, ...) ARGS_LIKE_PRINTF(1);
DLLEXPORT void  LogSet      (const char *LogFileName);
DLLEXPORT void  LogTime     (short set);

#endif

