/* $Header: /home/cvs/src/it_utils/src/qstring.h,v 1.6 1998/10/09 22:14:34 procmon Exp $ */
/****************************************************************
**
**	QSTRING.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_QSTRING_H )
#define IN_QSTRING_H

#define QSTRING_MAX_STRING_SIZE 32

DLLEXPORT short StringArray_Initialize    (void);
DLLEXPORT short StringArray_AddElement    (char *element);
DLLEXPORT short StringArray_RemoveElement (char *element);
DLLEXPORT void  StringArray_QSort         (void);

#endif

