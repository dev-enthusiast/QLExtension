#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_errd.c,v 1.5 2000/06/07 14:05:28 goodfj Exp $"
/****************************************************************
**
**	SDB_ERR.C	- Security database error handling functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_errd.c,v $
**	Revision 1.5  2000/06/07 14:05:28  goodfj
**	Fix includes
**
**	Revision 1.4  1999/11/01 21:58:58  singhki
**	oops, no BUILDING_SECDB in executables which link with secdb
**	
**	Revision 1.3  1999/09/01 15:29:29  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
****************************************************************/

#include	<portable.h>
#include	<err.h>
#include	<secdrive.h>


/****************************************************************
**	Routine: SecDbDriverErrStartup
**	Returns: Nothing
**	Action : Hooks up Err API
****************************************************************/

void SecDbDriverErrStartup( void )
{
#define REGISTER_SDB(Name,Desc)		ErrRegister( SDB_##Name, Desc )
#define REGISTER_ERR(Name)			
#include	<sdb_err.h>	
#undef	REGISTER_SDB
#undef	REGISTER_ERR
}


