/****************************************************************
**
**	TSSYMFMT.H
**
**	$Header: /home/cvs/src/tssym/src/tssymfmt.h,v 1.11 2000/06/13 19:54:28 schlae Exp $
**
****************************************************************/

#if !defined( IN_TSSYMFMT_H )

#define IN_TSSYMFMT_H

/*
**	Define symbol types
**
**	If the order of the types is changed or if a new type is added
**	the table of functions in 'SymbolAssignFunctions' must be updated
**	as well.
*/

#define	TSDB_BINARY_FLAG			0x0100
#define	TSDB_CONSTANT_FLAG			0x0200
#define	TSDB_REALTIME_FLAG			0x0400
#define TSDB_DISABLE_AUTOEXEC_FLAG	0x0800
#define TSDB_AUTOEXEC_FLAG			0x1000

#define	TSDB_UNDEFINED_FMT			(0x0000)
#define TSDB_BINARY_FMT				(0x0001)
#define TSDB_ASCII_FMT				(0x0002)
#define TSDB_TIME_FMT				(0x0003)
#define TSDB_HLOC_FMT				(0x0004)
#define TSDB_HLOCVO_FMT				(0x0005)
#define TSDB_DBASE_FMT				(0x0006)
#define TSDB_SYMTAB_FMT				(0x0007)
#define TSDB_SYMTAB_AFMT			(0x0008)
#define TSDB_CONSTANT_FMT			(0x0009)
#define TSDB_STRING_FMT				(0x000A)
#define TSDB_MEMORY_FMT				(0x000B)
#define	TSDB_SYMTAB_BFMT			(0x000C)
#define	TSDB_QSJAP_FMT				(0x000D)
#define	TSDB_QSSES_FMT				(0x000E)
#define	TSDB_HLOCDAY_FMT			(0x000F)
#define	TSDB_CURVE_FMT				(0x0010)
#define	TSDB_EXPRESSION_FMT			(0x0011)
#define	TSDB_FAME_FMT				(0x0012)
#define TSDB_FI_FMT					(0x0013)
#define TSDB_GMTIME_FMT				(0x0014)
#define TSDB_SECDB_FMT				(0x0015)
#define TSDB_SECDB_HLOCDAY_FMT		(0x0016)
#define TSDB_SECDB_HLOCVO_FMT		(0x0017)
#define TSDB_SECDB_TIME_FMT			(0x0018)
#define TSDB_RT_FMT					(0x0019)


/*
**	Define field types
*/
#define	TSDB_DEFAULT_FLD		0x0000
#define	TSDB_HIGH_FLD			0x0001
#define	TSDB_LOW_FLD			0x0002
#define	TSDB_OPEN_FLD			0x0004
#define	TSDB_CLOSE_FLD			0x0008
#define	TSDB_VOLUME_FLD			0x0010
#define	TSDB_OPENINT_FLD		0x0020
#define	TSDB_MORNING_FLD		0x0040
#define	TSDB_AFTERNOON_FLD		0x0080
#define	TSDB_PREVCLOSE_FLD		0x0100
#define	TSDB_DAY_FLD			(TSDB_MORNING_FLD | TSDB_AFTERNOON_FLD)
#define	tsdb_CURVE_START_FLD	0x0200


#endif

