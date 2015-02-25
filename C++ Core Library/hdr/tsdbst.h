/****************************************************************
**
**	TSDBST.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/tsdbst.h,v 1.22 2014/09/12 19:29:11 khodod Exp $
**
****************************************************************/

#ifndef IN_TSDBST_H
#define IN_TSDBST_H

#include <string>

// For DT_VALUE
#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif

// For SYMBOL
#ifndef IN_TURBO_H
#include <turbo.h>
#endif

/*
**	Define constants
*/

#define SYMBOL_MODIFIER_CHAR		'@'


/*
**	Define types
*/

struct TSDB_SYMBOL_DB_DRIVER;
struct TSDB_SYMBOL_DB;

typedef TsdbObjectContextNamePairList* ( *TSDB_PF_DB_GET_CATEGORY_MEMBERS ) ( TSDB_SYMBOL_DB *SymbolDb, const std::string& MemberId );
typedef TSDB_OBJECT* ( *TSDB_PF_DB_GET_OBJECT ) ( TSDB_SYMBOL_DB *SymbolDb, const std::string& ObjectId );

typedef int		(*TSDB_PF_DB_INIT)						( TSDB_SYMBOL_DB_DRIVER *SymbolDbDriver );
typedef int 	(*TSDB_PF_DB_OPEN)						( TSDB_SYMBOL_DB *SymbolDb );
typedef void	(*TSDB_PF_DB_CLOSE)						( TSDB_SYMBOL_DB *SymbolDb );
typedef SYMBOL	*(*TSDB_PF_DB_FIND_SYMBOL)				( TSDB_SYMBOL_DB *SymbolDb, const char *SymName, const char *Modifier );

typedef int		(*TSDB_PF_DB_FIND_ALL_SYMBOL)			( TSDB_SYMBOL_DB *SymbolDb, const char *Pattern, DT_VALUE *Results );
typedef int		(*TSDB_PF_DB_FIND_ALL_SYMBOL_METADATA)	( TSDB_SYMBOL_DB *SymbolDb, const std::string& Pattern, TsdbObjectList *List );

typedef int		(*TSDB_PF_DB_ATTRIBUTES)				( TSDB_SYMBOL_DB *SymbolDb, char *SymName, DT_VALUE *Results );
typedef int		(*TSDB_PF_DB_MEMBERS)					( TSDB_SYMBOL_DB *SymbolDb, char *CategoryName,  DT_VALUE *Results );
typedef int		(*TSDB_PF_DB_GET_MANY)					( TSDB_SYMBOL_DB *SymbolDb,  DT_VALUE *SymNames, SYMBOL **OutPutArray );

/*
**	Define structure for symbol db drivers
*/
struct TSDB_SYMBOL_DB_DRIVER
{
	std::string	Name,
				SymbolTableDll,
				SymbolFunctionDll,
				SymbolTableInitFunc;

	void	*DriverData;

	TSDB_PF_DB_OPEN
			pfOpen;

	TSDB_PF_DB_CLOSE
			pfClose;

	TSDB_PF_DB_FIND_SYMBOL
			pfFindSymbol;

	TSDB_PF_DB_GET_OBJECT
			pfGetObject;

	TSDB_PF_DB_FIND_ALL_SYMBOL
			pfFindAllSymbol;

	TSDB_PF_DB_FIND_ALL_SYMBOL_METADATA
			pfFindAllSymbolMetadata;

	TSDB_PF_DB_GET_CATEGORY_MEMBERS
			pfGetCategoryMembers;

	TSDB_PF_DB_ATTRIBUTES
			pfAttributes;

	TSDB_PF_DB_MEMBERS
			pfMembers;

	TSDB_PF_DB_GET_MANY
			pfGetMany;

	int		ErrorStatus;

	TSDB_SYMBOL_DB_DRIVER() : Name(), SymbolTableDll(), SymbolFunctionDll(), SymbolTableInitFunc(), DriverData( 0 ),
							  pfOpen( 0 ), pfClose( 0 ), pfFindSymbol( 0 ), pfGetObject( 0 ), pfFindAllSymbol( 0 ),
							  pfFindAllSymbolMetadata( 0 ), pfGetCategoryMembers( 0 ), pfAttributes( 0 ),
							  pfMembers( 0 ), pfGetMany( 0 ), ErrorStatus( 0 ) {}
	
};	// TSDB_SYMBOL_DB_DRIVER

/*
**	Define structure for a symbol db instance
*/
struct TSDB_SYMBOL_DB
{
	std::string Identifier,
				Prefix,
				Username,
				Password,
				DatabaseName,
				DatabaseSchema,
				DatabaseDescription,
				OdbcDriver,
				OdbcConnectionString,
				ErrorText;

	TSDB_SYMBOL_DB_DRIVER
			*Driver;

	void	*DriverData;

	TSDB_SYMBOL_DB
		*Next;

	unsigned Open : 1;

	unsigned ErrorStatus : 1;

	TSDB_SYMBOL_DB() : Identifier(), Prefix(), Username(), Password(), DatabaseName(), DatabaseSchema(),
					   DatabaseDescription(), OdbcDriver(), OdbcConnectionString(), ErrorText(),
					   Driver( 0 ), DriverData( 0 ), Next( 0 ), Open( 0 ), ErrorStatus( 0 ) {}
};

DLLEXPORT TSDB_SYMBOL_DB
		*TsdbSymDb;

#endif
