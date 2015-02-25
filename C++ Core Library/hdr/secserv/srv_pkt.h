/* $Header: /home/cvs/src/osecserv/src/secserv/srv_pkt.h,v 1.29 2004/11/29 23:48:10 khodod Exp $ */
/****************************************************************
**
**	SRV_PKT.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.29 $
**
****************************************************************/

#if !defined( IN_SRV_PKT_H )
#define IN_SRV_PKT_H


/*
**	Packet handling function arguments
*/

#define	SRV_FUNC_ARGS				\
		SRV_CONNECTION *SrvConn,    \
		SRV_MSG_TYPE MsgType,       \
		BSTREAM *Input,             \
		BSTREAM *Output


/*
**	Define types
*/

typedef	SRV_PKT_RESULT	(*PF_MSG_HANDLER)(	SRV_FUNC_ARGS );


/*
**	Macro to check correct stream usage
*/

#define	SRV_PKT_CHECK_STREAM(ExtraExpression)	\
		{	\
			if( Input->Data < Input->DataEnd || Input->Overflow )	\
			{	\
				ExtraExpression	\
				SrvError( "(%s - %s) - Request '%s' packet not correctly parsed",	\
						SrvConn->ApplicationName,			\
						SrvConn->UserName,					\
						StrTabSecSrvMessages[ MsgType ] );	\
				return SRV_PKT_DETACH;	\
			}	\
		}


/*
**	Prototype functions
*/

SRV_PKT_RESULT
		SrvFuncAttach(				SRV_FUNC_ARGS ),
		SrvFuncChangeInitData(		SRV_FUNC_ARGS ),
    	SrvFuncCollectClientStats(	SRV_FUNC_ARGS ),
		SrvFuncDbCheckSum(			SRV_FUNC_ARGS ),
    
		SrvFuncDbValidate(			SRV_FUNC_ARGS ),
		SrvFuncDebug(				SRV_FUNC_ARGS ),
		SrvFuncDelete(				SRV_FUNC_ARGS ),
		SrvFuncErrorLogGetPart(		SRV_FUNC_ARGS ),
		SrvFuncErrorLogGetState(	SRV_FUNC_ARGS ),
		SrvFuncFileClose(			SRV_FUNC_ARGS ),
		SrvFuncFileInfoGet(			SRV_FUNC_ARGS ),
		SrvFuncFileOpen(			SRV_FUNC_ARGS ),
		SrvFuncFileRead(			SRV_FUNC_ARGS ),
		SrvFuncGetByName(			SRV_FUNC_ARGS ),
		SrvFuncGetConnInfo(			SRV_FUNC_ARGS ),
		SrvFuncGetDiskInfo(			SRV_FUNC_ARGS ),
		SrvFuncGetFromSyncPoint(	SRV_FUNC_ARGS ),
		SrvFuncGetIndexDescriptor(	SRV_FUNC_ARGS ),
		SrvFuncGetObjCheckSum(		SRV_FUNC_ARGS ),
		SrvFuncGetStatistics(		SRV_FUNC_ARGS ),
		SrvFuncGetTime(				SRV_FUNC_ARGS ),
		SrvFuncGetUniqueID(			SRV_FUNC_ARGS ),
		SrvFuncGetUtilization(		SRV_FUNC_ARGS ),
		SrvFuncIncremental(			SRV_FUNC_ARGS ),
		SrvFuncIndexCheckSum(		SRV_FUNC_ARGS ),
		SrvFuncIndexClose(			SRV_FUNC_ARGS ),
		SrvFuncIndexCreate(			SRV_FUNC_ARGS ),
		SrvFuncIndexDelete(			SRV_FUNC_ARGS ),
		SrvFuncIndexDestroy(		SRV_FUNC_ARGS ),
		SrvFuncIndexGet(			SRV_FUNC_ARGS ),
		SrvFuncIndexGetByName(		SRV_FUNC_ARGS ),
		SrvFuncIndexGetSecurity(	SRV_FUNC_ARGS ),
		SrvFuncIndexIncremental(	SRV_FUNC_ARGS ),
		SrvFuncIndexInsert(			SRV_FUNC_ARGS ),
		SrvFuncIndexOpen(			SRV_FUNC_ARGS ),
		SrvFuncIndexRename(			SRV_FUNC_ARGS ),
		SrvFuncIndexUpdate(			SRV_FUNC_ARGS ),
		SrvFuncInsert(				SRV_FUNC_ARGS ),
		SrvFuncKillConnection(		SRV_FUNC_ARGS ),
		SrvFuncNameLookup(			SRV_FUNC_ARGS ),
		SrvFuncNameLookupByType(	SRV_FUNC_ARGS ),
		SrvFuncRename(				SRV_FUNC_ARGS ),
		SrvFuncSetTime(				SRV_FUNC_ARGS ),
		SrvFuncShutdown(			SRV_FUNC_ARGS ),
		SrvFuncSourceInfo(			SRV_FUNC_ARGS ),
		SrvFuncSyncPointCleanup(	SRV_FUNC_ARGS ),
		SrvFuncSyncPointSave(		SRV_FUNC_ARGS ),
		SrvFuncSyncPointAsyncSave(	SRV_FUNC_ARGS ),
		SrvFuncSyncPointAsyncAbort(	SRV_FUNC_ARGS ),
		SrvFuncTransBegin(			SRV_FUNC_ARGS ),
		SrvFuncTransGetHead(		SRV_FUNC_ARGS ),
		SrvFuncTransGetParts(		SRV_FUNC_ARGS ),
		SrvFuncTransGetState(		SRV_FUNC_ARGS ),
		SrvFuncTransMap(			SRV_FUNC_ARGS ),
		SrvFuncUpdate(				SRV_FUNC_ARGS ),
		SrvFuncSelfDescData( 		SRV_FUNC_ARGS );
		
#endif
