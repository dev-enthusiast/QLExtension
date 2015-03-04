/****************************************************************
**
**	SDBNPROF.H	- SecDb Node profiling functions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdbnprof.h,v 1.3 1999/11/01 22:41:53 singhki Exp $
**
****************************************************************/

#if !defined( IN_SECDB_SDBNPROF_H )
#define IN_SECDB_SDBNPROF_H

EXPORT_C_SECDB int
		SecDbProfileStart( const char *ProfileName ),
		SecDbProfileEnd();

#endif 
