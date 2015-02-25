/* $Header: /home/cvs/src/osecserv/src/secserv/srv_tran_part.h,v 1.3 2004/11/29 23:48:10 khodod Exp $ */
/****************************************************************
**
**	srv_tran_part.h - cache handles to transaction parts
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Log: srv_tran_part.h,v $
**	Revision 1.3  2004/11/29 23:48:10  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
**	Revision 1.2  2002/06/21 19:30:00  khodod
**	Changes to support a large transaction log.
**	
**	Revision 1.1  2000/04/07 17:19:03  brownb
**	initial revision
**	
****************************************************************/



int SrvTransPartHandleCacheCreate( void );
void SrvTransPartHandleCacheFlush( void );

int SrvTransPartHandleMemoize( SDB_TRANS_ID TransID, int PartNumber,
							   BIG_OFFSET PartOffset );

int SrvTransPartOffset( SDB_TRANS_ID TransID, int PartNumber,
						BIG_OFFSET *ReturnedPartOffset );
