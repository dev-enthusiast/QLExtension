/****************************************************************
**
**	arg.h --- Utilities for command line arguments
**
**	$Header: /home/cvs/src/kool_ade/src/arg.h,v 1.9 2001/11/27 22:48:57 procmon Exp $
**	
****************************************************************/

#if !defined( IN_ARG_H )
#define IN_ARG_H

#include <kool_ade.h>

typedef enum ArgTypeEnum
{
	ARG_TOGGLE,					// Simple toggle flag,	Buf is (int *)
	ARG_INT,					// Arg is an int,		Buf is (int *)
	ARG_STRING,					// Arg is a string,		Buf is (char **)
	ARG_DATE,					// Arg is a date,		Buf is (DATE *)
	ARG_DOUBLE,					// Arg is a double,		Buf is (double *)
	ARG_FUNCTION				// Arg is a date,		Buf is ARG_CALLBACK

} ARG_TYPE;

typedef struct ArgEntryStruct
{
	char    ShortName;			// Single letter name

	const char
			*LongName;			// Long-form name

	ARG_TYPE
			Type;				// Type of argument

	void	*Buf;				// Pointer to result buffer

	const char
			*Desc;				// Descriptive string

} ARG_ENTRY;

typedef		int (*ARG_CALLBACK)( ARG_ENTRY *Entry, const char *ArgStr );


DLLEXPORT void
		ArgPrintUsage(	ARG_ENTRY *List, STRING_TABLE *Usage );

DLLEXPORT int
		ArgParse(		int argc, char **argv, ARG_ENTRY *Args);

#endif // IN_ARG_H
