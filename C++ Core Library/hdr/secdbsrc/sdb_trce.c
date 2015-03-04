#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_trce.c,v 1.9 2001/01/25 14:49:11 simpsk Exp $"
/****************************************************************
**
**	SDB_TRCE.C	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_trce.c,v $
**	Revision 1.9  2001/01/25 14:49:11  simpsk
**	Only abort from a getvalue.
**
**	Revision 1.8  2000/06/07 14:05:28  goodfj
**	Fix includes
**	
**	Revision 1.7  1999/10/26 19:22:17  singhki
**	Allow getvalue/buildchildren to be aborted and restarted
**	
**	Revision 1.6  1999/09/01 15:29:32  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<secerror.h>
#include	<err.h>


/*
** Global variables
*/

SDB_TRACE_FUNC
		SecDbTraceAbortFunc;

int		SecDbTraceAbortInit;



/****************************************************************
**	Routine: SecDbMessageTrace
**	Returns: TRUE or FALSE
**	Action : Calls the SecDbTraceFunc if present
****************************************************************/
int SecDbMessageTrace(
	SDB_OBJECT	*SecPtr,
	int			Msg,
	SDB_M_DATA	*MsgData
)
{
	int		Status;


	if( !SecDbTraceFunc )
		return( (*SecPtr->Class->FuncPtr)( SecPtr, Msg, MsgData ));

	if( !(*SecDbTraceFunc)( SDB_TRACE_OBJECT_MSG, SecPtr, Msg, MsgData ))
		return( FALSE );
		
	Status = (*SecPtr->Class->FuncPtr)( SecPtr, Msg, MsgData );
	if( !Status && SecDbTraceFunc )
		(*SecDbTraceFunc)( SDB_TRACE_ERROR_OBJECT, SecPtr, Msg, MsgData );
	
	if( SecDbTraceFunc )
		(*SecDbTraceFunc)( SDB_TRACE_DONE_OBJECT_MSG, SecPtr, Msg, MsgData );
	return( Status );
}



/****************************************************************
**	Routine: SecDbMessageTraceVt
**	Returns: TRUE or FALSE
**	Action : Calls the SecDbTraceFunc if present
****************************************************************/
int SecDbMessageTraceVt(
	SDB_OBJECT			*SecPtr,
	int					Msg,
	SDB_M_DATA			*MsgData,
	SDB_VALUE_TYPE_INFO	*Vti
)
{
	int		Status;


	if( !SecDbTraceFunc )
		return( (*Vti->Func)( SecPtr, Msg, MsgData, Vti ));

	if( !(*SecDbTraceFunc)( SDB_TRACE_VALUE_MSG, SecPtr, Msg, MsgData, Vti ))
		return( FALSE );
		
	Status = (*Vti->Func)( SecPtr, Msg, MsgData, Vti );
	if( !Status && SecDbTraceFunc )
		(*SecDbTraceFunc)( SDB_TRACE_ERROR_VALUE, SecPtr, Msg, MsgData, Vti );
	
	if( SecDbTraceFunc )
		(*SecDbTraceFunc)( SDB_TRACE_DONE_VALUE_MSG, SecPtr, Msg, MsgData, Vti );
	return( Status );
}



/****************************************************************
**	Routine: SecDbTraceAbortGet
**	Returns: FALSE
**	Action : Used to abort getvalue. Always returns FALSE
****************************************************************/

int SecDbTraceAbortGet(
	SDB_TRACE_MSG	Msg,
	...
)
{
	switch( Msg )
	{
		case SDB_TRACE_GET:
			if( SecDbTraceAbortInit )
			{
				Err( SDB_ERR_GET_VALUE_ABORTED, "@" );
				SecDbTraceAbortInit = FALSE;
			}
			return FALSE;

		default:
			return TRUE;
	}
}
