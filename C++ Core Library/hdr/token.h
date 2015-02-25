/* $Header: /home/cvs/src/it_utils/src/token.h,v 1.16 1998/10/09 22:14:35 procmon Exp $ */
/****************************************************************
**
**	TOKEN.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**	
**	Brian A. Weston
**
**	$Revision: 1.16 $
**
****************************************************************/

#if !defined( IN_TOKEN_H )
#define IN_TOKEN_H

DLLEXPORT char *StrToken (char **x, char token);
DLLEXPORT char *StrCpy   (char *x,  char *y);
DLLEXPORT char *StrNCpy  (char *x,  char *y, unsigned int i);
DLLEXPORT int   StrICmp  (char *x,  char *y);
DLLEXPORT char *StrUpr   (char *x);
DLLEXPORT int   StrINCmp (char *x,  char *y, unsigned int n);
DLLEXPORT char *StrCat   (char *x,  char *y);
DLLEXPORT char *StrLwr   (char *x);


#endif

