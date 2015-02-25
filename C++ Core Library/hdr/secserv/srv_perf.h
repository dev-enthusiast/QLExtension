/* $Header: /home/cvs/src/osecserv/src/secserv/srv_perf.h,v 1.1 1999/01/13 18:31:16 brownb Exp $ */
/****************************************************************
**
**	srv_perf.h - server utilization gathering
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_SRV_PERF_H )
#define IN_SRV_PERF_H


void	SrvWorkInitialize(	void ),
		SrvWorkEnd(			double TimeBeforeSelect ),
		SrvWorkStart(		double TimeAfterSelect );


#endif
