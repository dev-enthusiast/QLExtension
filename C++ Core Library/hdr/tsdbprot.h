/****************************************************************
**
**	TSDBPROT.H
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/tsdbprot.h,v 1.47 2014/10/21 19:54:06 e19351 Exp $
**
****************************************************************/

#ifndef IN_TSDBPROT_H
#define IN_TSDBPROT_H

struct MemPoolStructure;
typedef MemPoolStructure MEM_POOL;

struct TSDB_SYMBOL_DB;

/*
**	Prototype functions
*/

// in dbupdate.c
typedef int		(*COLLISION_FUNC)(SYMBOL *, void *, void *, void *);
typedef void	(*TSDB_FIND_FAIL_FUNC)( const char *Name );

DLLEXPORT int		tsdb_update_flags(SYMBOL *sym,void *data,int npoints,int flag,COLLISION_FUNC collision_func,void *arg);
DLLEXPORT int		tsdb_update(SYMBOL *sym,void *data,int npoints,COLLISION_FUNC collision_func,void *arg);
DLLEXPORT int		tsdb_update_no_sync(SYMBOL *sym,void *data,int npoints,COLLISION_FUNC collision_func,void *arg);
DLLEXPORT void		TsdbUpdateThrottle( SYMBOL *Sym );

// in newsym.c
DLLEXPORT int	SymbolAssignFunctions(	SYMBOL * );

// in inittab.c
DLLEXPORT int	tsdb_traverse_table(	SYMBOL_TABLE *tab,int (*func)(SYMBOL_TABLE *tab, SYMBOL *sym, void *arg),void *arg);
DLLEXPORT void	OldToNewSymbol(			SYMBOL *, OLD_SYMBOL * );
DLLEXPORT int	OldToNewFT(				char Oldft );
DLLEXPORT char	NewToOldFT(				int  Newft );
DLLEXPORT const char *
    TsdbTypeName(int ft);

// in merge.c
DLLEXPORT DATE 	merge(					SYMBOL_TABLE *pSymbTab, DATE value);
DLLEXPORT DATE 	_merge(					SYMBOL_TABLE *pSymbTab, DATE value);
DLLEXPORT DATE 	mergehl(				SYMBOL_TABLE *pSymbTab, DATE value, HLOCVO *ph, int size);
DLLEXPORT DATE 	_mergehl(				SYMBOL_TABLE *pSymbTab, DATE value, HLOCVO *ph, int size);
DLLEXPORT DATE 	merge_union(			SYMBOL_TABLE *pSymbTab, DATE value);
DLLEXPORT DATE 	_merge_union(			SYMBOL_TABLE *pSymbTab, DATE value);

DLLEXPORT int  	tsdb_open(				SYMBOL *s);
DLLEXPORT void 	tsdb_close(				SYMBOL *s);
DLLEXPORT int 	tsdb_remove(			SYMBOL *sym);
DLLEXPORT int  	tsdb_position(			SYMBOL *sym, DATE date);
DLLEXPORT int  	tsdb_write_ascii(		FILE *outf, TSDB_POINT *p);
DLLEXPORT DATE 	tsdb_last_date(			SYMBOL *sym);
DLLEXPORT DATE	tsdb_first_date(		SYMBOL *sym);
DLLEXPORT int	TsdbLastPoint(			SYMBOL	*Sym, DATE	StartDate, DATE	EndDate);

typedef DATE (*TSDB_LBS_FUNC)(long index, SYMBOL *sym, void *user);
DLLEXPORT int TsdbLinearBSearch(SYMBOL *sym, DATE target, long *index_result,
	DATE *date_result, long max_index, TSDB_LBS_FUNC IndexToDateFunc, void *user);

DLLEXPORT int TsdbLinearBSearchBefore(SYMBOL *sym, DATE target, long *index_result,
	DATE *date_result, long max_index, TSDB_LBS_FUNC IndexToDateFunc, void *user);

// Do these really belong here ??
DLLEXPORT void 	putSymb( SYMBOL *pSymb);

// in exptok.c
DLLEXPORT int	ExpTok( char *out, char *Expr, TSDBARG *t);

// in exptosym.c
DLLEXPORT SYMBOL
	*ExpToSymb( char *InpExpr, SYMBOL *OutSymb, SYMBOL_TABLE *SymbTab, DATE low, DATE high, bool RealTime );
DLLEXPORT SYMBOL 	*TsdbArgToSymb( TSDBARG *ts );
DLLEXPORT TSDBARG
	*ExpToTsdbArg( char *InpExpr, SYMBOL *OutSymb, SYMBOL_TABLE *SymbTab, DATE xlow, DATE xhigh, bool RealTime, MEM_POOL *MemPool );
DLLEXPORT TSDBARG	*TsdbAutoExecGetTsdbArg( SYMBOL *sym, TSDBARG *ts);

// in rev.c
/* These two are global variables */
DLLEXPORT TOKEN *TsdbPushToken( char *op, TSDBARG *t );
DLLEXPORT TOKEN *TsdbPopToken( TSDBARG *t );

DLLEXPORT SYMBOL *(*parse_sym_find_func)(char *name, SYMBOL_TABLE *SymbTab);

DLLEXPORT const char *
	syntax( char *pExprsn, SYMBOL_TABLE *SymbTab );
DLLEXPORT int 	lex_token_type(			char *op, TSDBARG *t);

// in eval.c
DLLEXPORT SYMBOL	*eval(					TSDBARG *t);
DLLEXPORT SYMBOL	*PushE(					SYMBOL *pSymb, TSDBARG *t);
DLLEXPORT SYMBOL	*PopE(					TSDBARG *t);
DLLEXPORT SYMBOL	*eval_unknown_shell(	TSDBARG *t);

// in funcsini.c
DLLEXPORT void	initfunc(				FUNC_TABLE *fTab );		// function is no longer needed
DLLEXPORT int 	TsdbWalkFunctionTable( TSDB_ITERATE_FUNC IterateFunc, void *Param );


DLLEXPORT SYMBOL	*GetArgs(				SYMBOL_TABLE *pSymbTab, TSDBARG *t, DATE xlooff, DATE xhioff);
DLLEXPORT SYMBOL	*CleanUp(				SYMBOL_TABLE *pSymbTab, SYMBOL *OutSymb, TSDBARG *t);

DLLEXPORT int		TsdbFunctionTableLoad(	char *TableName );
DLLEXPORT int 
    TsdbFunctionRegister(
        const char *Name, 
        const char *DllPath,
        const char *FuncName, 
        TSDB_FUNC pfFunc, 
        int NumArg, 
        int OpPrec, 
        const char *Description );

DLLEXPORT TSDB_FUNCTION_INFO
		*TsdbFunctionLookup( const char *Name );
DLLEXPORT void (*TsdbFunctionLookupHook)( const char *Name );
DLLEXPORT const TSDB_HELP *TsdbFunctionHelp( char *Name );


// in init.c
DLLEXPORT int		TsdbInit( void );
DLLEXPORT void 		TsdbClose(void );

// in tsdbfind.c
DLLEXPORT SYMBOL*		TsdbCopySymbol( SYMBOL *, SYMBOL * );
DLLEXPORT SYMBOL*		TsdbFindSymbol( const char *Name );

DLLEXPORT TSDB_OBJECT*	TsdbGetObject( const std::string& ObjectId );

// Used to call from python using ctypes
DLLEXPORT TSDB_OBJECT*	TsdbGetObject_Py( const char* ObjectId );

EXPORT_CPP_EXPORT TsdbDbList TsdbGetDbListFromPrefix( const std::vector<std::string>& List );
 
EXPORT_CPP_EXPORT TsdbObjectList* TsdbSearchSymbols( const std::string& Pattern, TSDB_OBJECT_SEARCH_PATTERN_TYPE::Value Type, TsdbDbList & DbList);

DLLEXPORT int TsdbSearchSymbolsFrom( TSDB_SYMBOL_DB *SymDb, const std::string& Pattern, TsdbObjectList *List );

EXPORT_CPP_EXPORT TsdbObjectContextNamePairList* TsdbGetCategoryMembers( const std::string& CategoryId );
EXPORT_CPP_EXPORT TsdbObjectContextNamePairList* TsdbGetCategoryMembersFrom( TSDB_SYMBOL_DB *SymDb, const std::string& CategoryId );

DLLEXPORT void TsdbFreeObjectContextNamePairList( TsdbObjectContextNamePairList *List );
DLLEXPORT void TsdbFreeObjectList( TsdbObjectList *List );

DLLEXPORT TSDB_FIND_FAIL_FUNC TsdbFindFailFunc;
DLLEXPORT int		TsdbSymbolDbPrepend( const std::string& DatabaseSpec );
DLLEXPORT int		TsdbSymbolDbAppend(	const std::string& DatabaseSpec );
DLLEXPORT int TsdbSymbolDbOpen( TSDB_SYMBOL_DB *SymDb );

#if defined( IN_DATATYPE_H )
DLLEXPORT int		TsdbSymbolGetAttributes(	SYMBOL *Sym, DT_VALUE *Attributes );
DLLEXPORT int TsdbCategoryMembers(	char *CategoryName,	DT_VALUE *Results);
DLLEXPORT int TsdbGetManySymbols( TSDB_SYMBOL_DB *SymDb, DT_VALUE *SymNames, SYMBOL **OutPutArray);
DLLEXPORT int TsdbGetModifiers( SYMBOL *Sym, DT_VALUE *Modifiers );

// Used to call from python using ctypes
DLLEXPORT void TsdbGetCategoryMembers_Py( const char* CategoryId, DT_VALUE *RetVal );
DLLEXPORT int TsdbSearchSymbols_Py( const char *Pattern, unsigned C_PatternType, DT_VALUE *PrefixList, DT_VALUE *RetVal );
#endif

// in manyfile.c
DLLEXPORT void TsdbFileStats(long	*OpenCount, long *OpenFailCount, long *CloseCount,
	long *ReadCount, long *WriteCount, double *BytesRead, double *BytesWritten);

#endif
