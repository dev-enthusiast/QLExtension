/****************************************************************
**
**	TSDB_INT.H	- TSDB Internal information
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/tsdb_int.h,v 1.23 2013/08/16 21:17:51 c01713 Exp $
**
****************************************************************/

#if !defined( IN_TSDB_INT_H )
#define IN_TSDB_INT_H


/*
**	Define constants
*/

#define	MAX_WINDOW_DAYS		8000


/*
**	Global variables
*/

extern int TsdbErrorDepth;		// for TsdbError(). Not exported.

#define TOKEN_STACK_SIZE 	256
#define SYMBOL_STACK_SIZE 	128

/*
**	Define macro to convert a value into a signal
*/

#define	TSDB_SIGNAL(val)	((val) > 0.0 ? 1.0 : ((val) < 0.0 ? -1.0 : 0.0))


/*
**	Define macro to convert number of business days into calendar days
*/

#define TsdbWeekDaysToDays( d )		( (int) ( 7 * ( d ) / 5 ) )

/*
**	Prototype functions
*/

#define TSDB_DESTROY_TEMP_INTL(s)	{ if( (s)->TmpFile == TSDB_TEMP_INTL_SYM ) DestroyTmpSymbol( s );}
DLLEXPORT SYMBOL	*CreateTmpSymbol(		int );
DLLEXPORT void	DestroyTmpSymbol(		SYMBOL * );
void	DestroyAllTmpSymbols(	void );
void 	RemoveTmpSymbol(		SYMBOL	*);

int		InitMergeBuffers(		void );

int		TsdbSymbolDbInit();
void	TsdbSymbolDbClose(		void );
extern void TsdbSymtabErrMore(  void );

void 	TsdbSetErrorJmp(		void );
void	TsdbRestoreErrorJmp(	void );
void	TsdbProcessError(		void );

/* in rev.c */
extern int  rpn(		char *in, TSDBARG *t),
			is_numeric(	char *s, int *plen );

/* in eval.c */
extern SYMBOL *Evaluate(TSDBARG *t);


/*
**	Return values of lex_token_type()
*/

#define HELL_IF_I_KNOW		-1
#define L_PAREN        		 1
#define R_PAREN        		 2
#define COMMA          		 3
#define UNARY_OP       		 4
#define BINARY_OP      		 5
#define FUNCTION_TYPE  		 6
#define SYMBOL_TYPE    		 7
#define CONSTANT       		 8
#define CHARSTRING     		 9
#define UNKNOWN_SYMBOL_TYPE 10


#endif

