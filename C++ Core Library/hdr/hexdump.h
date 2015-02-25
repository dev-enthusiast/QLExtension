/* $Header: /home/cvs/src/kool_ade/src/hexdump.h,v 1.7 2001/11/27 22:49:04 procmon Exp $ */
/****************************************************************
**
**	HEXDUMP.H	
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_HEXDUMP_H )
#define IN_HEXDUMP_H

typedef int (*HEX_DUMP_PRINT_FUNC)( void *PrintFuncArg, char *String );

DLLEXPORT int
		HexDump( 			void *MemToDump, int Size, HEX_DUMP_PRINT_FUNC PrintFunc, void *PrintFuncArg ),
		HexDumpToBuffer(	void *MemToDump, int Size, char *Buffer );

#endif
