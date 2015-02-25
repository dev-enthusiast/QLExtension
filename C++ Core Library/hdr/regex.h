/* $Header: /home/cvs/src/regex/src/regex.h,v 1.13 2013/06/18 12:37:44 khodod Exp $ */
/****************************************************************
**
**	REGEX.H		- Regular expressions
**
**	$Revision: 1.13 $
**
****************************************************************/

#include <portable.h>

// Avoid DLL export conflicts.

#define regfree GnuRegFree 
#define regexec GnuRegExec
#define regcomp GnuRegComp
#define regerror GnuRegError
#define regoff_t GnuRegOff_t
#define re_set_registers gnu_re_set_registers
#define re_match_2 gnu_re_match_2
#define re_match gnu_re_match
#define re_search gnu_re_search
#define re_compile_pattern gnu_re_compile_pattern
#define re_set_syntax gnu_re_set_syntax
#define re_search_2 gnu_re_search_2
#define re_compile_fastmap gnu_re_compile_fastmap
#define re_syntax_options gnu_re_syntax_options
#define re_max_failures gnu_re_max_failures

#define _REGEX_RE_COMP
#define re_comp gnu_re_comp
#define re_exec gnu_re_exec

#include "xregex2.h"

/*
** GsStuff
*/

DLLEXPORT int GsReMatch (regex_t* preg, const char *string, int len, int start );

/*
**	DUBNIX compatibility
*/

DLLEXPORT void GnuRegSub (char *String, regmatch_t MatchTable[], size_t MatchTableSize,
					char *Template, char *Output, size_t OutputSize );

DLLEXPORT void GnuRegPrint(regex_t *RegEx);


