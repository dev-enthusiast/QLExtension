/* $Header: /home/cvs/src/osecserv/src/secserv/srv_indx.h,v 1.2 2004/11/29 23:48:10 khodod Exp $ */
/****************************************************************
**
**	srv_indx.h
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: srv_indx.h,v $
**	Revision 1.2  2004/11/29 23:48:10  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
**	Revision 1.1  2001/06/06 01:12:51  brownb
**	initial revision
**	
**	
****************************************************************/



SRV_INDEX *IndexNumberToIndex( int	IndexNumber );

void *IndexGet( SRV_INDEX* Index, short GetType, void* RecBuf,
                SRV_CONNECTION* SrvConn );

void SwapIndexRecord( SDB_INDEX_DESCRIPTOR *Descriptor,
                      void *RecBuf );
