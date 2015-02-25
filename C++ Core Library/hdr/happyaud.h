/* $Header: /home/cvs/src/portable/src/happyaud.h,v 1.3 2001/11/27 23:09:57 procmon Exp $ */
/****************************************************************
**
**	HAPPYAUD.H	
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_HAPPYAUD_H )
#define IN_HAPPYAUD_H

typedef	enum
{
	HAPPY_AUDIT_MALLOC,
	HAPPY_AUDIT_CALLOC,
	HAPPY_AUDIT_REALLOC,
	HAPPY_AUDIT_FREE
	
} HAPPY_AUDIT_MSG;

typedef size_t *HAPPY_PTR;
typedef void (HAPPY_AUDIT_FUNC)( HAPPY_AUDIT_MSG Msg, HAPPY_PTR Ptr );

DLLEXPORT HAPPY_AUDIT_FUNC
		*HappyAuditFunc;

#endif

