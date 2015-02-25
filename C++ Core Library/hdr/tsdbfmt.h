/****************************************************************
**
**	TSDBFMT.H
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/tsdbfmt.h,v 1.45 2013/08/29 01:52:35 khodod Exp $
**
****************************************************************/

#ifndef IN_TSDBFMT_H
#define IN_TSDBFMT_H

/*
**	Define symbol types
**
**	If the order of the types is changed or if a new type is added
**	the table of functions in 'SymbolAssignFunctions' must be updated
**	as well.
*/

#define tsdb_FLAG_MASK	0xfff00
#define tsdb_FMT_MASK	0x00ff

#define	tsdb_BINARY_FLAG			0x0100
#define	tsdb_CONSTANT_FLAG			0x0200
#define	tsdb_REALTIME_FLAG			0x0400
#define tsdb_DISABLE_AUTOEXEC_FLAG	0x0800
#define tsdb_AUTOEXEC_FLAG			0x1000
#define tsdb_SS_FLAG				0x2000
#define tsdb_SQL_FLAG				0x4000
#define tsdb_RMT_FLAG				0x8000
#define tsdb_DATE_FLAG				0x10000		// indicates the values of a series are dates

#define	tsdb_UNDEFINED_FMT			(0x0000)
#define tsdb_BINARY_FMT				(0x0001 | tsdb_BINARY_FLAG)
#define tsdb_ASCII_FMT				(0x0002)
#define tsdb_TIME_FMT				(0x0003 | tsdb_BINARY_FLAG | tsdb_REALTIME_FLAG)
#define tsdb_HLOC_FMT				(0x0004 | tsdb_BINARY_FLAG | tsdb_REALTIME_FLAG)
#define tsdb_HLOCVO_FMT				(0x0005 | tsdb_BINARY_FLAG)
#define tsdb_DBASE_FMT				(0x0006)
#define tsdb_SYMTAB_FMT				(0x0007)
#define tsdb_SYMTAB_AFMT			(0x0008)
#define tsdb_CONSTANT_FMT			(0x0009 | tsdb_CONSTANT_FLAG)
#define tsdb_STRING_FMT				(0x000A | tsdb_CONSTANT_FLAG)
#define tsdb_MEMORY_FMT				(0x000B | tsdb_BINARY_FLAG)
#define tsdb_SYMTAB_BFMT			(0x000C)
#define tsdb_QSJAP_FMT				(0x000D | tsdb_BINARY_FLAG | tsdb_REALTIME_FLAG)
#define tsdb_QSSES_FMT				(0x000E | tsdb_BINARY_FLAG | tsdb_REALTIME_FLAG)
#define tsdb_HLOCDAY_FMT			(0x000F | tsdb_BINARY_FLAG)
#define tsdb_CURVE_FMT				(0x0010)
#define tsdb_EXPRESSION_FMT			(0x0011 | tsdb_AUTOEXEC_FLAG )	// this should be removed but is left so stuff doesn't die
#define tsdb_ARRAY_FMT				(0x0013 | tsdb_BINARY_FLAG)
#define tsdb_FI_FMT					(0x0014 | tsdb_BINARY_FLAG)
#define tsdb_GMTIME_FMT				(0x0015 | tsdb_BINARY_FLAG | tsdb_REALTIME_FLAG)
#define tsdb_SECDB_FMT				(0x0016 | tsdb_BINARY_FLAG)
#define tsdb_SECDB_HLOCDAY_FMT		(0x0017 | tsdb_BINARY_FLAG)
#define tsdb_SECDB_HLOCVO_FMT		(0x0018 | tsdb_BINARY_FLAG)
#define tsdb_SECDB_TIME_FMT			(0x0019 | tsdb_BINARY_FLAG | tsdb_REALTIME_FLAG)
#define tsdb_DT_FMT					(0x0020 | tsdb_BINARY_FLAG)
#define tsdb_RT_FMT					(0x0021 | tsdb_BINARY_FLAG | tsdb_REALTIME_FLAG)
#define tsdb_PLT_FMT				(0x0022 | tsdb_CONSTANT_FLAG)
#define tsdb_SS_BINARY_FMT			( tsdb_BINARY_FMT | tsdb_SS_FLAG )
#define tsdb_SS_TIME_FMT			( tsdb_TIME_FMT | tsdb_SS_FLAG )
#define tsdb_SS_GMTIME_FMT			( tsdb_GMTIME_FMT | tsdb_SS_FLAG )
#define tsdb_SS_HLOCVO_FMT			( tsdb_HLOCVO_FMT | tsdb_SS_FLAG )
#define tsdb_SQL_BINARY_FMT			( tsdb_BINARY_FMT | tsdb_SQL_FLAG )
#define tsdb_SQL_GMTIME_FMT			( tsdb_GMTIME_FMT | tsdb_SQL_FLAG )
#define tsdb_SQL_HLOC_FMT			( tsdb_HLOC_FMT   | tsdb_SQL_FLAG )
#define tsdb_SQL_HLOCDAY_FMT		( tsdb_HLOCDAY_FMT| tsdb_SQL_FLAG )
#define tsdb_RMT_BINARY_FMT			( tsdb_BINARY_FMT | tsdb_RMT_FLAG )
#define tsdb_RMT_GMTIME_FMT			( tsdb_GMTIME_FMT | tsdb_RMT_FLAG )
#define tsdb_RMT_HLOC_FMT			( tsdb_HLOC_FMT   | tsdb_RMT_FLAG )
#define tsdb_RMT_HLOCDAY_FMT		( tsdb_HLOCDAY_FMT| tsdb_RMT_FLAG )

#define	tsdb_UNDEFINED_AUTOEXEC_FMT	(tsdb_UNDEFINED_FMT | tsdb_AUTOEXEC_FLAG )
#define	tsdb_BINARY_AUTOEXEC_FMT	(tsdb_BINARY_FMT  	| tsdb_AUTOEXEC_FLAG )
#define	tsdb_HLOC_AUTOEXEC_FMT		(tsdb_HLOC_FMT    	| tsdb_AUTOEXEC_FLAG )
#define	tsdb_HLOCVO_AUTOEXEC_FMT	(tsdb_HLOCVO_FMT  	| tsdb_AUTOEXEC_FLAG )
#define	tsdb_HLOCDAY_AUTOEXEC_FMT	(tsdb_HLOCDAY_FMT 	| tsdb_AUTOEXEC_FLAG )
#define	tsdb_TIME_AUTOEXEC_FMT		(tsdb_TIME_FMT 		| tsdb_AUTOEXEC_FLAG )

#define is_binary(S)			((S)->ft & tsdb_BINARY_FLAG)
#define	is_constant(S)			((S)->ft & tsdb_CONSTANT_FLAG)
#define	is_realtime(S)			((S)->ft & tsdb_REALTIME_FLAG)
#define is_autoexec(S)			((S)->ft & tsdb_AUTOEXEC_FLAG)
#define is_autoexec_disabled(S)	((S)->ft & tsdb_DISABLE_AUTOEXEC_FLAG)

#define TSDB_IS_BINARY_FMT(S)	( (((S)->ft&tsdb_FMT_MASK) == (tsdb_BINARY_FMT & tsdb_FMT_MASK)) || ((S)->ft == tsdb_TIME_FMT)  \
								|| ((S)->ft == tsdb_GMTIME_FMT) || ((S)->ft == tsdb_MEMORY_FMT)	\
								|| ((S)->ft == tsdb_SECDB_FMT) || ((S)->ft == tsdb_SECDB_TIME_FMT) )
#define TSDB_IS_HLOC_FMT(S)		( (((S)->ft&tsdb_FMT_MASK) == (tsdb_HLOCDAY_FMT & tsdb_FMT_MASK)) || ((S)->ft == tsdb_HLOC_FMT) || ((S)->ft == tsdb_SECDB_HLOCDAY_FMT))
#define TSDB_IS_HLOCVO_FMT(S)	( (((S)->ft&tsdb_FMT_MASK) == (tsdb_HLOCVO_FMT & tsdb_FMT_MASK)) || ((S)->ft == tsdb_SECDB_HLOCVO_FMT) )
#define is_tsdb_bar(S)	   		(TSDB_IS_HLOC_FMT((S))||TSDB_IS_HLOCVO_FMT((S)))

/*
**	Define field types
*/
#define	tsdb_DEFAULT_FLD		0x0000
#define	tsdb_HIGH_FLD			0x0001
#define	tsdb_LOW_FLD			0x0002
#define	tsdb_OPEN_FLD			0x0004
#define	tsdb_CLOSE_FLD			0x0008
#define	tsdb_VOLUME_FLD			0x0010
#define	tsdb_OPENINT_FLD		0x0020
#define	tsdb_MORNING_FLD		0x0040
#define	tsdb_AFTERNOON_FLD		0x0080
#define	tsdb_PREVCLOSE_FLD		0x0100
#define	tsdb_DAY_FLD			(tsdb_MORNING_FLD | tsdb_AFTERNOON_FLD)
#define	tsdb_CURVE_START_FLD	0x0200


/*
**	Define merge types
**
**	Note that for backward compatibility, we want the default
**	(zero) to be intersection.    Having the zero value be
**	something else will break any code that creates a SYMBOL_TABLE
**	by a mechanism other than SetUpTable()
*/

#define	tsdb_INTERSECT_MERGE	0x0000
#define tsdb_REALTIME_MERGE		0x0001
#define tsdb_UNION_MERGE		0x0002
#define tsdb_ERROR_MERGE		0x0003

#define tsdb_union_merge(S)	    ((S)->MergeType!=tsdb_INTERSECT_MERGE)


/*
**	Define TmpFile flags
**
**	These are used internally by tsdb to determine when and whether to delete files
**	associated with temporary symbols. The letters were a mystery to everyone, so
**	I came up with the best definitions I could based on how they were used.
**
**	[Pp] are used for symbols with permanent files which should not be deleted when no longer used locally.
**	[Tt] are used for symbols with temporary files which should be deleted when no longer used locally.
**
**	[PT] are used for symbols assigned by the user.
**	[pt] are used for symbols created internally by tsdb.
**
**	Thus:
**	'P': "mysymbol=dem"; a straight assignment of one symbol to another
**	'T': "mysymbol=dem/jpy"; the assignment of a calculated symbol to a named symbol
**	't': "dem/jpy"; used internally for calculated interim values
**	'p': "3"; used internally for calculated constant symbols, strings, and array symbols.
**
**	Example 1: User evaluates "mysym=dem".
**	Tsdb copies dem to mysym and sets mysym.TmpFile == 'P'.
**
**	Example 2: User evaluates "mysem=dem*100".
**	1. Tsdb evaulates each token on the right side of the '='.
**		Token "dem" evaluates to symbol dem.
**		Token "100" is evaluated. Tsdb creates an internal constant symbol with TmpFile == 'p'.
**	2. Tsdb evaluates the expression "dem*100".
**		Tsdb creates an internal temporary symbol with the resulting values of dem*100 and sets TmpFile == 't'.
**	3. The result of (2) is assigned to "mysym".
**		"mysym" is now a user symbol with TmpFile == 'T'.
*/

#define TSDB_PERM_USER_SYM	'P'
#define	TSDB_TEMP_USER_SYM	'T'
#define	TSDB_TEMP_INTL_SYM	't'
#define	TSDB_PERM_INTL_SYM	'p'

#endif
