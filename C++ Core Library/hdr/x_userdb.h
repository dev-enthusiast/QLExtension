/* $Header: /home/cvs/src/userdb/src/x_userdb.h,v 1.4 1998/10/09 22:28:05 procmon Exp $ */
/****************************************************************
**
**	X_USERDB.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_X_USERDB_H )
#define IN_X_USERDB_H


/*
**	Structure used to contain UdbConnection datatype information
*/

typedef struct SecUdbConnectionStructure
{
	int		UseCount;

	UDB_CONTEXT
			*hudb;

} SEC_UDB_CONNECTION;


/*
**	Variables
*/

DLLEXPORT DT_DATA_TYPE
		DtUdbConnection;


/*
**	Macros
*/

#define	ARG0_UDB_CONNECTION		(((SEC_UDB_CONNECTION *) SLANG_PTR( 0 ))->hudb)

#endif

