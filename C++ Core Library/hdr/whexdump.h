/* $Header: /home/cvs/src/secview/src/whexdump.h,v 1.2 1998/10/09 22:26:41 procmon Exp $ */
/****************************************************************
**
**	WHEXDUMP.H	-  Window hex dump function
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_WHEXDUMP_H )
#define IN_WHEXDUMP_H


/*
**	Prototype functions
*/

DLLEXPORT void	WindowHexDump(			WINDOW *w, void *MemToDump, int Size );

#endif


