/**************************************************************** -*-c++-*-
**
**	SecDbGenericTraceFunc.h	: generic trace functions
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/SecDbGenericTraceFunc.h,v 1.2 2004/11/29 23:48:13 khodod Exp $
**
****************************************************************/

#if !defined( IN_SECDB_GENERIC_TRACE_FUNC_H )
#define IN_SECDB_GENERIC_TRACE_FUNC_H

#include    <secdb/base.h>
#include	<sectrace.h>

struct DT_ARRAY;
class SecDbTraceStack;
struct SDB_OBJECT;

EXPORT_C_SECDB SecDbTraceStack* SecDbSetTraceStack( SecDbTraceStack	*pTraceStack );
EXPORT_C_SECDB SecDbTraceStack* SecDbGetTraceStack( );
EXPORT_C_SECDB int  SecDbTraceBreak(
	SDB_OBJECT		*Object,
	DT_ARRAY		*VtApply,
	int				Message,
	int				On,
	bool			exclude				// if we should exclude nodes which match this breakpoint
);

EXPORT_C_SECDB int SecDbGenericTraceFunc(
	SDB_TRACE_MSG	Msg
	...
);

EXPORT_C_SECDB int SecDbGenericTraceOnErrFunc(
	SDB_TRACE_MSG	Msg
	...
);



#endif




