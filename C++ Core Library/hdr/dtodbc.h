/* $Header: /home/cvs/src/datatype/src/dtodbc.h,v 1.16 2013/08/31 15:48:15 khodod Exp $ */
/****************************************************************
**
**   DTODBC.H	
**
**   Copyright 2000 - Goldman, Sachs & Co. - New York
**
**   $Revision: 1.16 $
**
****************************************************************/

#ifndef IN_DTODBC_H
#define IN_DTODBC_H

#ifdef WIN32
#	include <windows.h>
#endif

#include <hash.h>
#include <dtdb.h>



struct DT_ODBC_CONNECTION;

typedef int DT_ODBC_ROW_CB(
	DT_VALUE			*RowValues, 
	int					ResultNumber,
	void				*CallbackArg
);

DLLEXPORT DT_DATA_TYPE DtOdbcConnection;

DLLEXPORT int DtFuncOdbcConnection(
	int	                  Msg,
	DT_VALUE			*r,
	DT_VALUE			*a,
	DT_VALUE			*b
);

DLLEXPORT DT_ODBC_CONNECTION *DtOdbcConnect(
	const char			*connString, 
	const char			*timeZone
);
        
DLLEXPORT int DtOdbcQuery(
	DT_ODBC_CONNECTION	*connection,
	const char			*query, 
	DT_ODBC_ROW_CB		*rowCallback, 
	void				*callbackArg, 
	int					*retCount,
	DB_DATE_FMT			dateFormat = DB_TIME 
);

DLLEXPORT int DtOdbcRPC(
	DT_ODBC_CONNECTION	*connection, 
	const char			*rpcName,
	DT_VALUE			*rpcArgs,
	DT_ODBC_ROW_CB		*rowCallback,
	void				*callbackArg,
	DT_VALUE			*procRetStat,
	int					*retCount
);

#endif
