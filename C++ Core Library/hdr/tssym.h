/****************************************************************
**
**	TSSYM.H	- Definitions for new SecDb/TSDB Symbol table interface
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tssym/src/tssym.h,v 1.17 2005/12/21 21:01:18 khodod Exp $
**
****************************************************************/

#if !defined( IN_TSSYM_H )
#define IN_TSSYM_H

#if !defined( IN_DATE_H )
#	include	<date.h>
#endif

#if !defined( IN_HASH_H )
#	include	<hash.h>
#endif

#if !defined( IN_TSSYMFMT_H )
#	include <tssymfmt.h>
#endif


/*
**	Typedefs
*/

typedef struct _tsdb_symbol 		TSDB_SYMBOL;		// Information about the symbol
typedef struct _tsdb_iter_struct	*TSDB_SYMITER;		// Symbol category iterator
typedef struct _tsdb_symbol_table	TSDB_SYMBOL_TABLE;	// Table of TSDB Symbols


/*
**	Macros
*/

#define		TSDB_VALUE_NAME(tvi)				((tvi)->ValueType->Name)
#define		TSDB_VALUE_TYPE(tvi)				((tvi)->ValueType->DataType)
#define		TSDB_VALUE_TYPENAME(tvi) 			((tvi)->ValueType->DataType->Name)

#define		TsdbSymbolIsCategory( Sym ) 		( (Sym)->SymList != NULL 	)
#define		TsdbCategoryNumberOfSymbols(SymCat)	( (SymCat)->NumSymbols		)

/*
**	Beware that the Obj parameter gets evaluated twice
*/

#define	TsdbSymbolGetString( Obj, Type ) \
			( ((Obj) == NULL ) ? NULL : GobGetString(Obj->SecPtr, Type))

#define	TsdbSymbolGetDouble( Obj, Type ) \
			( ((Obj) == NULL ) ? HUGE_VAL : GobGetDouble(Obj->SecPtr, Type))

#define	TsdbSymbolSetString( Obj, Type, Val ) \
			( ((Obj) == NULL ) ? Err(ERR_INVALID_ARGUMENTS, "@: In TsdbSymbolSetString") : GobSetString(Obj->SecPtr, Type, Val))

#define	TsdbSymbolSetDouble( Obj, Type, Val ) \
			( ((Obj) == NULL ) ? Err(ERR_INVALID_ARGUMENTS, "@: In TsdbSymbolSetDouble") : GobSetDouble(Obj->SecPtr, Type, Val))

#define TsdbSymbolValueTypeEnumFirst( Obj, EnumPtr ) \
			( SecDbValueTypeEnumFirst( Obj->SecPtr, EnumPtr ))

#define TsdbSymbolValueTypeEnumNext( EnumPtr ) \
			( SecDbValueTypeEnumNext( EnumPtr ))

#define TsdbSymbolValueTypeEnumClose( EnumPtr ) \
			( SecDbValueTypeEnumClose( EnumPtr ))

#define TsdbSymIterNameFirst( Iter )		TsdbSymbolNameByNumber( Iter->Symbol, (Iter->Current = 1) - 1 )

#define TsdbSymIterNameLast( Iter )			TsdbSymbolNameByNumber( Iter->Symbol, (Iter->Current = Iter->Symbol->NumSymbols) - 1 )

#define TsdbSymIterNameOffset( Iter, Off )	TsdbSymbolNameByNumber( Iter->Symbol, Off)

#define TsdbSymIterNameNext( Iter )			TsdbSymbolNameByNumber( Iter->Symbol, Iter->Current++)

#define TsdbSymIterNamePrev( Iter )			\
	((Iter->Current == 0) ? NULL : TsdbSymbolNameByNumber( Iter, Iter->Current++))


#define TsdbSymIterFirst( Iter )		TsdbSymbolByNumber( Iter->Symbol, (Iter->Current = 1) - 1 )

#define TsdbSymIterLast( Iter )			TsdbSymbolByNumber( Iter->Symbol, (Iter->Current = Iter->Symbol->NumSymbols) - 1 )

#define TsdbSymIterOffset( Iter, Off )	TsdbSymbolByNumber( Iter->Symbol, Off)

#define TsdbSymIterNext( Iter )			TsdbSymbolByNumber( Iter->Symbol, Iter->Current++)

#define TsdbSymIterPrev( Iter )			\
	((Iter->Current == 0) ? NULL : TsdbSymbolByNumber( Iter, Iter->Current++))

#define TsdbCategoryAppendSymbolName( SymCat, Name) \
	DtSubscriptSetPointer( SymCat->SymList, SymCat->NumSymbols++, DtString, Name );

#define		TsdbSymbolIsRealTime( Sym )		( TsdbSymbolGetDouble( Sym, "Real Time" ) == 1.0 )
/*
**	Structure used to represent a TSDB symbol
*/

struct _tsdb_symbol
{
	// Public values.  These may not be modified directly by application code.

	const char			*Name,
						*Description,
						*FileName;

	int					SeriesType;

	// Private values.  These should not be used or modified by application code.

	int					RefCount,		// Number of open pointers to symbol
						NumSymbols,		// Number of symbols in SymList
						TempFlag;		// Temporary symbol

	SDB_OBJECT			*SecPtr;		// Underlying SecDb object

	DT_VALUE			*SymList;		// NULL for symbols, pointer
										// to Symbols array for categories

	TSDB_SYMBOL_TABLE	*SymTab;		// Symbol table for this symbol

};


/*
**	Structure holding state information for a TSDB symbol table
**	None of this is public.
*/

struct _tsdb_symbol_table
{
	char				DbName[SDB_DB_NAME_SIZE + 1];

	SDB_DB				*Db;			// SecDb database pointer

	int					RefCount;		// Reference count
};


/*
**	Structure used for enumerating the symbols in a category
*/

struct _tsdb_iter_struct
{
	int					Current;		// Current offset

	TSDB_SYMBOL			*Symbol;		// Pointer to symbol
};






/*
**	Functions operating on TSDB Symbol Tables
*/

DLLEXPORT TSDB_SYMBOL_TABLE
		*TsdbTableOpen(			const char *TableName, int Mode);

DLLEXPORT int		TsdbTableClose(			TSDB_SYMBOL_TABLE *Table ),
		TsdbTableRenameObject(	TSDB_SYMBOL_TABLE *Table, const char *New, const char *old ),
		TsdbTableDeleteObject(	TSDB_SYMBOL_TABLE *Table, const char *item );


/*
**	Public functions operating on TSDB Symbols
*/

DLLEXPORT TSDB_SYMBOL
		*TsdbSymbolByName(		TSDB_SYMBOL_TABLE *Table, const char *name ),
		*TsdbSymbolCreate(		TSDB_SYMBOL_TABLE *Table, const char *name,
								const char *desc, const char *path,
								int SerType, int TempFlag),
		*TsdbSymbolCreateCategory(	TSDB_SYMBOL_TABLE *Table, const char *name,
									const char *desc, int TempFlag );

DLLEXPORT int		TsdbSymbolFree(			TSDB_SYMBOL *Sym),
		TsdbSymbolUpdate(		TSDB_SYMBOL *Sym),
		TsdbSymbolSetDesc(		TSDB_SYMBOL *Sym, const char *desc),
		TsdbSymbolSetPath(		TSDB_SYMBOL *Sym, const char *path),
		TsdbSymbolSetType(		TSDB_SYMBOL *Sym, int type),
		TsdbSymbolGetAttribute(	DT_VALUE	*Result, TSDB_SYMBOL *Sym, const char *Name );

DLLEXPORT HASH	*TsdbSymbolAttributeTable( TSDB_SYMBOL *Sym );

DLLEXPORT char	*TsdbSymbolSeriesTypeName (TSDB_SYMBOL *Sym );

DLLEXPORT int	TsdbSymbolGetGreater(TSDB_SYMBOL_TABLE *Table, const char *Name, TSDB_SYMBOL **out_syms, int nsyms);
DLLEXPORT int	TsdbNameGreater(TSDB_SYMBOL_TABLE *Table, char *Name);

/*
**	Symbol list manipulation
*/

DLLEXPORT TSDB_SYMBOL	*TsdbSymbolByNumber(		TSDB_SYMBOL *SymCat, int Offset);
DLLEXPORT const char	*TsdbSymbolNameByNumber( 	TSDB_SYMBOL *SymCat, int Offset);
DLLEXPORT TSDB_SYMBOL	*TsdbSymbolNameSearch(		TSDB_SYMBOL_TABLE *Table, const char *Class, const char *Name, int Max);

DLLEXPORT int			TsdbCategoryAppendSymbol(	TSDB_SYMBOL *SymCat, TSDB_SYMBOL *Sym);
DLLEXPORT int			TsdbCategoryAddAtOffset(	TSDB_SYMBOL *SymCat, TSDB_SYMBOL *Sym, int Offset);
DLLEXPORT int			TsdbCategoryAddAfter(		TSDB_SYMBOL *SymCat, TSDB_SYMBOL *Sym, const char *name);


DLLEXPORT TSDB_SYMITER
			TsdbSymIterOpen(			TSDB_SYMBOL *SymCat);
DLLEXPORT int			TsdbSymIterClose(			TSDB_SYMITER Iter);

/*
**	Statistics gathering
*/

DLLEXPORT void		TsdbSymbolStats(  			int *nAlloc, int *nFree, int *nRealFree);


/*
**	Miscellaneous
*/


DLLEXPORT HASH		*TsdbLoadCategoryNames(		TSDB_SYMBOL_TABLE *Table);

#endif // IN_TSSYM_H
