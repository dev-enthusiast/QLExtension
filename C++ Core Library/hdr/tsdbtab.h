/****************************************************************
**
**	TSDBTAB.H
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/tsdbtab.h,v 1.16 2001/11/27 23:35:07 procmon Exp $
**
****************************************************************/

#if !defined( IN_TSDBTAB_H )
#define IN_TSDBTAB_H

#include	<hash.h>

#define		SYMTAB_XATTR_PARENT_CODE		((XATTR_CODE) 20)

#define		SYMTAB_XATTR_EXPDATE			((XATTR_CODE) 302)
#define		SYMTAB_XATTR_OPTEXPDATE	  		((XATTR_CODE) 303)
#define		SYMTAB_XATTR_STRIKE   			((XATTR_CODE) 307)
#define		SYMTAB_XATTR_PUTCALL			((XATTR_CODE) 311)

#define		SYMTAB_XATTR_STRIKE_START 		((XATTR_CODE) 351)
#define		SYMTAB_XATTR_STRIKE_STOP 		((XATTR_CODE) 352)
#define		SYMTAB_XATTR_STRIKE_INCR 		((XATTR_CODE) 353)

#define		SYMTAB_XATTR_EXPRESSION 		((XATTR_CODE) 10)
#define		SYMTAB_XATTR_EXCH_HOLIDAY		((XATTR_CODE) 53)

#define		SYMTAB_XATTR_FI_MATURITY		((XATTR_CODE) 710)
#define		SYMTAB_XATTR_FI_COUPON			((XATTR_CODE) 711)
#define		SYMTAB_XATTR_FI_ISSCURR			((XATTR_CODE) 716)

#define		SYMTAB_XATTR_SECDB_VALUETYPE	((XATTR_CODE) 1005)

#define		SYMTAB_XATTR_FUT_COMMOD			((XATTR_CODE) 1101)
#define		SYMTAB_XATTR_FUT_OWNER			((XATTR_CODE) 1102)
#define		SYMTAB_XATTR_INTRATE_NAME		((XATTR_CODE) 1103)
#define		SYMTAB_XATTR_FUT_NUMKNOTS		((XATTR_CODE) 1104)

#define		SYMTAB_XATTR_TERM 				((XATTR_CODE) 1201 )
#define		SYMTAB_XATTR_CURRENCY1			((XATTR_CODE) 1202 )
#define		SYMTAB_XATTR_CURRENCY2			((XATTR_CODE) 1203 )
#define		SYMTAB_XATTR_INTRATE_NSIMP		((XATTR_CODE) 1204)
#define		SYMTAB_XATTR_INTRATE_DAYCNT		((XATTR_CODE) 1205)
#define		SYMTAB_XATTR_INTRATE_OWNER		((XATTR_CODE) 1206)
#define		SYMTAB_XATTR_INTRATE_COUPON_PER	((XATTR_CODE) 1207)

/*
**	Define structure used to pass symbol arguments for searching
*/

typedef struct sym_arg
{
	char	*name;
	SYMBOL	out_sym;

} FINDSYMBOL_ARG ;


/*
**	Define constants
*/

#define		MIN_SYMBOL_HASH_SIZE	500


/*
**	Variables
*/

extern HASH
		*TsdbSymCacheHash,
		*UdbEntitlementHash;


/*
**	Define tsdb_traverse_table function
*/

typedef int	(*TRAVERSE_FUNC)(SYMBOL_TABLE *tab, SYMBOL *sym, void *arg);


/* in hashtab.c */
HASH 	*SymHashCreate( void );
void 	SymHashDestroy(  HASH *SymHash);
SYMBOL 	*InsertSymInHash(HASH *SymHash,SYMBOL *Sym);

DLLEXPORT void		*tsdb_get_symbol_hash( void );
DLLEXPORT void		*tsdb_clear_symbol_hash( void );
DLLEXPORT int		tsdb_dump_symbol_hash(char *fn,int type);
DLLEXPORT void		tsdb_delete_symbol_entry(char *Name);
DLLEXPORT SYMBOL	*tsdb_add_symbol_entry(SYMBOL *SymToAdd);
DLLEXPORT void		tsdb_remove_assigned_symbols(void);
void 				ResetAutoExecEntries(void);

#endif

