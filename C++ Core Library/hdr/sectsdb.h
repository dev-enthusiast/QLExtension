/****************************************************************
**
**	SECTSDB.H	- SecDb / Tsdb interface
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/sectsdb/src/sectsdb.h,v 1.21 2014/10/21 19:54:05 e19351 Exp $
**
****************************************************************/

#if !defined( IN_SECTSDB_H )
#define IN_SECTSDB_H

#ifndef IN_DATE_H
#include <date.h>
#endif
#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif
#ifndef IN_TURBO_H
#include <turbo.h>
#endif

/*
**	Structure used to contain SecDb Tsdb symbol data type instances
*/

struct SECTSDB_SYMBOL
{
	int		UseCount;

	SYMBOL	TsdbSymbol;

	char	*Expression;

	DATE	StartDate,
			EndDate;
};


/*
**	Variables
*/

DLLEXPORT int
		SecTsdbInitialized;

DLLEXPORT SYMBOL_TABLE
		SecTsdbSymbolTable;

DLLEXPORT FUNC_TABLE
		SecTsdbFunctionTable;

DLLEXPORT DT_DATA_TYPE
		DtTsdb;


/*
**	Helper functions
*/

// KLUGE for TimeGmtToTsdb
#define TimeGmtToTsdb(x)  ((time_t)((2*((long)(x)))-(long)TimeTsdbToGmt((x))))


/*
**	Prototype functions
*/

DLLEXPORT int
		SecTsdbStart(					void ),
		SecTsdbValueFromExpression(		DT_VALUE *Value, const char *Expression, DATE StartDate, DATE EndDate, bool RealTime );

DLLEXPORT DT_CURVE *DtExpressionToCurve( const char *Symbol, DATE Start, DATE Finish );

DLLEXPORT int SecTsdbSymInfo( SECTSDB_SYMBOL *Symbol, DT_VALUE *r );

#endif
