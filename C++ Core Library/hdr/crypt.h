/* $Header: /home/cvs/src/crypt/src/crypt.h,v 1.5 2011/05/17 01:31:42 khodod Exp $ */
/****************************************************************
**
**	CRYPT.H	
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_CRYPT_H )
#define IN_CRYPT_H


/*
**	Prototype functions
*/

DLLEXPORT char	*gs_crypt( const char *key, const char *salt );

#ifdef BUILDING_CRYPT
#define crypt gs_crypt
#endif

#endif
