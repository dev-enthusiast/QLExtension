/* $Header: /home/cvs/src/kool_ade/src/clex.h,v 1.18 2001/11/27 22:48:58 procmon Exp $ */
/****************************************************************
**
**	CLEX.H	- Lexical analyzer for C-like languages (and Slang)
**
**	$Revision: 1.18 $
**
****************************************************************/

#if !defined( IN_CLEX_H )
#define IN_CLEX_H

#if !defined(IN_KOOL_ADE_H)
#include <kool_ade.h>
#endif

#if !defined(IN_HASH_H)
#include <hash.h>
#endif

#include <stdio.h>

typedef enum
{
	NULL_TOKEN = 0,
	ERROR_TOKEN = NULL_TOKEN,
	EOF_TOKEN,

	NAME,
	SYMBOL = NAME,			// placed in symbol table and shown in l->symbol

	QUOTED_STRING,			// allocated copy in l->quoted_string; uses C-like 
							// rules, exceptions return ERROR_TOKEN

	QUOTED_CHAR,			// value in l->quoted_char; uses C-like
							// rules, exceptions return ERROR_TOKEN
	
	NUMBER,					// value in l->number

	TILDE,
	TILDE_EQUALS,
	TILDE_EQUAL = TILDE_EQUALS,
	TILDE_EQ = TILDE_EQUALS,

	GRAVE,
	EXCLAMATION,
	NOT = EXCLAMATION,
	NOT_EQUALS,
	NOT_EQUAL = NOT_EQUALS,
	NOT_EQ = NOT_EQUALS,

	AT,
	AT_SIGN = AT,
	POUND,
	POUND_SIGN = POUND,
	DOLLAR,
	DOLLAR_SIGN = DOLLAR,
	PERCENT,
	PERCENT_SIGN = PERCENT,
	PERCENT_EQUALS,
	PERCENT_EQUAL = PERCENT_EQUALS,
	PERCENT_EQ = PERCENT_EQUALS,

	CIRCUMFLEX,
	CARAT = CIRCUMFLEX,
	CIRCUMFLEX_EQUALS,
	CIRCUMFLEX_EQUAL = CIRCUMFLEX_EQUALS,
	CIRCUMFLEX_EQ = CIRCUMFLEX_EQUALS,


	AMPERSAND,
							// but it isn't put in the symbol table
	BIT_AND = AMPERSAND,
	AND_EQUALS,
	AND_EQUAL = AND_EQUALS,
	AND_EQ = AND_EQUALS,

	DOUBLE_AMPERSAND,
	AND = DOUBLE_AMPERSAND,
	LOGICAL_AND = DOUBLE_AMPERSAND,

	ASTERISK,
	STAR = ASTERISK,
	MULTIPLY = ASTERISK,
	TIMES_EQUALS,
	TIMES_EQUAL = TIMES_EQUALS,
	TIMES_EQ = TIMES_EQUALS,
	ASTERISK_EQ = TIMES_EQUALS,

	LPAREN,
	LEFT_PAREN = LPAREN,
	OPEN_PAREN = LPAREN,
	OPAREN = LPAREN,

	RPAREN,
	RIGHT_PAREN = RPAREN,
	CLOSE_PAREN = RPAREN,
	CPAREN = RPAREN,

	LBRACE,
	LEFT_BRACE = LBRACE,
	OPEN_BRACE = LBRACE,
	OBRACE = LBRACE,

	RBRACE,
	RIGHT_BRACE = RBRACE,
	CLOSE_BRACE = RBRACE,
	CBRACE = RBRACE,

	LBRACKET,
	LEFT_BRACKET = LBRACKET,
	OPEN_BRACKET = LBRACKET,
	OBRACKET = LBRACKET,

	RBRACKET,
	RIGHT_BRACKET = RBRACKET,
	CLOSE_BRACKET = RBRACKET,
	CBRACKET = RBRACKET,

	MINUS,
	DASH = MINUS,
	SUBTRACT = MINUS,
	DOUBLE_MINUS,
	DECREMENT = DOUBLE_MINUS,
	MINUS_MINUS = DOUBLE_MINUS,
	MINUS_EQUALS,
	MINUS_EQUAL = MINUS_EQUALS,
	MINUS_EQ = MINUS_EQUALS,

	ARROW,	// i.e. '->'
	
	PLUS,
	ADD = PLUS,
	DOUBLE_PLUS,
	INCREMENT = DOUBLE_PLUS,
	PLUS_PLUS = DOUBLE_PLUS,
	PLUS_EQUALS,
	PLUS_EQUAL = PLUS_EQUALS,
	PLUS_EQ = PLUS_EQUALS,

	EQUAL_SIGN,
	EQUAL = EQUAL_SIGN,
	DOUBLE_EQUAL,
	EQUALITY = DOUBLE_EQUAL,

	SCHEFFLER,
	SCHEFFLER_STROKE = SCHEFFLER,
	BIT_OR = SCHEFFLER,
	DOUBLE_SCHEFFLER,
	OR = DOUBLE_SCHEFFLER,
	LOGICAL_OR = DOUBLE_SCHEFFLER,
	OR_EQUALS,
	OR_EQUAL = OR_EQUALS,
	OR_EQ = OR_EQUALS,


	BACKSLASH,
	COLON,
	DOUBLE_COLON,
	SEMICOLON,

	LT,
	LESS_THAN = LT,
	LT_EQUALS,
	LT_EQUAL = LT_EQUALS,
	LT_EQ = LT_EQUALS,
	LT_LT,
	DOUBLE_LESS_THAN = LT_LT,
	DOUBLE_LT = LT_LT,
	LEFT_SHIFT = LT_LT,
	LT_LT_EQ,
	LEFT_SHIFT_EQ = LT_LT_EQ,

	GT,
	GREATER_THAN = GT,
	GT_EQUALS,
	GT_EQUAL = GT_EQUALS,
	GT_EQ = GT_EQUALS,
	GT_GT,
	DOUBLE_GREATER_THAN = GT_GT,
	DOUBLE_GT = GT_GT,
	RIGHT_SHIFT = GT_GT,
	GT_GT_EQ,
	RIGHT_SHIFT_EQ = GT_GT_EQ,

	COMMA,
	PERIOD,
	DOT = PERIOD,
	DOT_DOT,
	DOT_DOT_DOT,
	ELLIPSE = DOT_DOT_DOT,

	QUESTION,
	QUESTION_MARK = QUESTION,
	QUESTION_EQUALS,
	QUESTION_EQUAL = QUESTION_EQUALS,
	QUESTION_EQ = QUESTION_EQUALS,

	SLASH,
	DIVIDE = SLASH,
	DIVIDE_EQUALS,
	DIVIDE_EQUAL = DIVIDE_EQUALS,
	DIVIDE_EQ = DIVIDE_EQUALS,
	
	COLON_EQUAL,
	
	C_COMMENT,
	
	COMPARE,

	DOUBLE_AT,

	C_IN_LINE_COMMENT,
	
	POUND_POUND,
	DOUBLE_POUND = POUND_POUND

} TOKEN;

typedef struct lexical_context
{
	FILE	*file;				// internal use

	const char
			*filename;			// if NULL and string NULL, using stdin
	
	const char
			*string;			// if filename NULL and string non-NULL,
								// take input from string

	TOKEN	token;				// token just returned by clex_gettok

	int		end_line;			// line number of token just returned
	
	int		end_char;			// in line (first char after \n is 1) 
								// of token just returned

	char	*symbol;			// last symbol,  (look but don't touch)

	HASH	*symbol_table;		// hash table of all the symbols seen
								// It's legal to put stuff in here
								// yourself, e.g. reserved words.  The
								// only string reserved by clex is
								// "~peek".  Also note that clex_stop
								// free's all the values in the table.

	char	*quoted_string;		// last quoted string seen

	double	number;				// last numberic constant seen

	char	quoted_char;		// last quoted char seen

	unsigned int
			ws_sym : 1,			// set if symbols can contain spaces
			c_comment : 1,		// set to return C_COMMENT tokens
			il_comment : 1,		// set to return C_IN_LINE_COMMENT tokens
			case_insensitive:1,	// set if  clex_set_case_insensitive() has been called
			dollar_op : 1,		// set if $ should only stringize if it is followed by a non alphabetic character
			have_symtab : 1,	// set to make CLex put tokens in the symbol_table
			tab_size : 8;		// set to tab_size if tabs should be counted

	int		start_line;
	int		start_char;
	
	struct lexical_context
			*peek;

} CLEXCTX;


/*
**	Prototypes
*/

DLLEXPORT CLEXCTX
		*clex_start( 				const char *filename, const char *or_string ),
		// preferred way of starting clex
		*CLexStart(					const char *filename, const char *or_string, int WantSymbolTable );

DLLEXPORT void
		clex_stop(					CLEXCTX	*l ),
		clex_set_case_insensitive(	CLEXCTX *l, int OnOrOff );

DLLEXPORT int
		clex_reset(					CLEXCTX *l, const char *filename, const char *or_string );
		
DLLEXPORT TOKEN
		clex_peektok(				CLEXCTX *l ),
		clex_gettok(				CLEXCTX *l );

DLLEXPORT const char *
		clex_tokenname(				TOKEN token );


#endif
