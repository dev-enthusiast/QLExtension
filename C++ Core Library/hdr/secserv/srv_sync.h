/* $Header: /home/cvs/src/osecserv/src/secserv/srv_sync.h,v 1.2 1999/10/15 18:24:26 brownb Exp $ */
/****************************************************************
**
**	srv_sync.h - syncpoint functions interface
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Log: srv_sync.h,v $
**	Revision 1.2  1999/10/15 18:24:26  brownb
**	poll for death of syncpointing child
**
**	Revision 1.1  1998/12/16 21:52:39  brownb
**	initial revision
**	support for async syncpoints
**	
****************************************************************/


#ifdef ASYNC_SYNC_POINT

int		SrvSyncPointSetup(			void ),
		SrvSyncPointAsyncSave(		void ),
		SrvSyncPointChildResult(	int *ChildResult ),
		SrvSyncPointAbort(			pid_t ChildProcessId );

void	SrvSyncPointPollChildExit(	void ),
		SrvSyncPointFinish(			int ChildResult );

#endif
