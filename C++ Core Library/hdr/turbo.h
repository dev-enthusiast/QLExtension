/****************************************************************
**
**	TURBO.H	- TSDB header file
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/turbo.h,v 1.59 2014/10/21 19:54:06 e19351 Exp $
**
****************************************************************/

#ifndef IN_TURBO_H
#define IN_TURBO_H
#include <portable.h>
#include <port_int.h>
#include <stdio.h>
#include <date.h>
#include <time.h>
#include <vector>
#include <set>
#include <string>
#include <utility>
#include <new>
#include <stdexcept>

/*
**	Define types
*/

struct SYMBOL;
struct TSDBARG;
struct TSDB_SYMBOL_DB;

typedef int				(*SYMFUNC)( int, SYMBOL *, long, int, void * );
typedef SYMBOL		*(*TSDB_FUNC)( TSDBARG *ts );
typedef DATE			(*BSFUNC)( long index, SYMBOL *sym, void *x );


/*
**	Functions
*/

DLLEXPORT char *TsdbHashUdbEntitlement( char *Entitlement );


/*
**	Define Sizes for structures
**
**	Its too bad we need these sizes but How many programs depend on the
**	symbol on the stack to have space !!!
**
*/

#define PATH_SIZE			128
#define SYMBOL_SIZE			96
#define DSCRP_SIZE			64
#define DTPROC_SIZE			64
#define	MODIFIER_SIZE		48
#define TZ_SZ				64

namespace TSDB_OBJECT_TYPE
{
	enum Value { SYMBOL, CATEGORY, USER_CATEGORY_NEW, COMMENT, ALIAS, UNKNOWN };
}

namespace TSDB_OBJECT_SEARCH_PATTERN_TYPE
{
	enum Value { SUBSTRING, GLOB, REGEX };
}

#ifndef DATE_DEFINITIONS
typedef long DATE;
#endif

/*
**	moved the formats into tsdbfmt.h
*/
#include <tsdbfmt.h>

/*
**  define structure for a data point
**  DT_CURVE_KNOT is based on KA_CURVE_KNOT. TSDB_POINT is asserted to be
**  the same in TsdbInit(). Note that TSDB_POINT accesses the members as x and y
**	while DT_CURVE_KNOT calls them Date and Value. Keeping DT_CURVE_KNOT and
**  TSDB_POINT identical in their memory structure allows us to cast from
**  one to the other.
*/

struct TSDB_POINT
{
	DATE x;					/* vector x number (date) */
	double y;				/* y value  */
};

struct HLOC
{
	DATE x;					/* vector x number (date) */
	double h;				/* y value  */
	double l;				/* y value  */
	double o;				/* y value  */
	double c;				/* y value  */
};

struct HLOCVO
{
	DATE x;					/* vector x number (date) */
	double h;				/* high  */
	double l;				/* low   */
	double o;				/* open  */
	double c;				/* close */
	long volume;
	long open_interest;
};


/*
**  Define packed structures
*/

#if !defined( _I860 )
#define	_packed
#endif

#if defined( __unix )
#define _packed
#endif

#if defined( WIN32 )
#pragma pack(1)
#endif

_packed struct TSDB_PACK_POINT_32
{
	int32_t x;				/* vector x number (date) */
	double y;				/* y value  */

};
#define SIZEOF_PACK_POINT_32	12

_packed struct PACK_HLOC_32
{
	int32_t x;				/* vector x number (date) */
	double h;				/* y value  */
	double l;				/* y value  */
	double o;				/* y value  */
	double c;				/* y value  */
};
#define SIZEOF_PACK_HLOC_32	36

_packed struct PACK_HLOCVO_32
{
	int32_t x;				/* vector x number (date) */
	double h;				/* high  */
	double l;				/* low   */
	double o;				/* open  */
	double c;				/* close */
	int32_t volume;
	int32_t open_interest;

};
#define SIZEOF_PACK_HLOCVO_32	44

#if defined( WIN32 )
#pragma pack()
#endif

/*
**  Define the structure for a symbol (atom)
*/
struct SYMBOL
{
	char	Name[SYMBOL_SIZE+1],		// Name of symbol (changed from 16 --> 32 chars)
			Fname[PATH_SIZE+1],			// File name of symbol
			Dscrp[DSCRP_SIZE+1],		// Description of symbol
			Modifier[MODIFIER_SIZE+1],	// Modifier (high/low...)
			GetDataProc[DTPROC_SIZE+1], // Stored procedure for data
			*UdbEntitlement;			// Udb Entitlement

	TSDB_OBJECT_TYPE::Value 	Type;					// Type of symbol

	char	TimeZone[TZ_SZ+1];

	int		ft,							// Symbol type (changed from Char to int)
			TmpFile;					// Flag indicates tmp file

	TSDB_SYMBOL_DB
			*SymDb;						// Symbol database

	void	*AutoExecTsdbArg;			// Used for Auto Eval Expressions

	void	*ExtraInfo,
			*Reserved;					// Reserved Pointers

	union								// Symbol function supplied data
	{
		long	Number;

		void	*Pointer;

	} SymData;

	int 	PackSize,					// Size of packed data element
			UnpackSize,		 			// Size of unpacked data element
			offset,			 			// Var used with field contains byte offset
							 			// into data structures that have more fields
							 			// than the time,price format
			Native,			 			// Flag to indicate native mode (don't
							 			// translate data into points
			field,						// Field(s) within symbol to work with
			Open;						// TRUE if symbol is 'open'

	void
			*pTmpSym;					// Pointer to private temporary symbol information

	SYMFUNC	SymFunc;					// Symbol type related function

	TSDB_POINT	*PointBuffer,				// In-memory buffer of points
			*CurPoint,					// Pointer to current point in buffer
			*EndPoint,					// Pointer to last actual point in buffer
			*MaxPoint;					// Pointer to last possible point in buffer

	TSDB_POINT	v;							// Vector point

	time_t	LastUpdateTime,				// Time of last TsdbUpdate. Used to throttle updates
			UpdateInterval;				// Minimum time between updates
};

typedef SYMBOL TSDB_OBJECT;
typedef std::vector<TSDB_SYMBOL_DB*> TsdbDbList;
typedef std::vector< TSDB_OBJECT* > TsdbObjectList;
typedef std::pair< TSDB_OBJECT*, std::string > TsdbObjectContextNamePair;
typedef std::vector< TsdbObjectContextNamePair > TsdbObjectContextNamePairList;

struct TSDB_OBJECT_COMPARE
{
	enum Type { COMPARE_BY_NAME, COMPARE_BY_FNAME, COMPARE_BY_DSCRP, COMPARE_BY_CONTEXT_NAME };

 	TSDB_OBJECT_COMPARE (): CompareType( TSDB_OBJECT_COMPARE::COMPARE_BY_NAME ), CheckLess( true ) {}

 	bool operator () ( const TSDB_OBJECT *t1, const TSDB_OBJECT *t2 )
 	{
 		if( this->CompareType == TSDB_OBJECT_COMPARE::COMPARE_BY_NAME )
 		{
 			if( this->CheckLess == true )
 				return  std::string(t1->Name) < std::string(t2->Name);
 			else
 				return  std::string(t1->Name) > std::string(t2->Name);
 		}
 		else if( this->CompareType == TSDB_OBJECT_COMPARE::COMPARE_BY_DSCRP )
 		{
 			if( this->CheckLess == true )
 				return  std::string(t1->Dscrp) < std::string(t2->Dscrp);
 			else
 				return  std::string(t1->Dscrp) > std::string(t2->Dscrp);
 		}

 		return true;
 	}

 	bool operator () ( const TsdbObjectContextNamePair& p1, const TsdbObjectContextNamePair& p2 )
 	{
 		if( this->CompareType == TSDB_OBJECT_COMPARE::COMPARE_BY_NAME )
 		{
 			if( this->CheckLess == true )
 				return  std::string(p1.first->Name) < std::string(p2.first->Name);
 			else
 				return  std::string(p1.first->Name) > std::string(p2.first->Name);
 		}
 		else if( this->CompareType == TSDB_OBJECT_COMPARE::COMPARE_BY_DSCRP )
 		{
 			if( this->CheckLess == true )
 				return  std::string(p1.first->Dscrp) < std::string(p2.first->Dscrp);
 			else
 				return  std::string(p1.first->Dscrp) > std::string(p2.first->Dscrp);
 		}
 		else if( this->CompareType == TSDB_OBJECT_COMPARE::COMPARE_BY_FNAME )
 		{
 			if( this->CheckLess == true )
 				return  std::string(p1.first->Fname) < std::string(p2.first->Fname);
 			else
 				return  std::string(p1.first->Fname) > std::string(p2.first->Fname);
 		}
 		else if( this->CompareType == TSDB_OBJECT_COMPARE::COMPARE_BY_CONTEXT_NAME )
 		{
 			if( this->CheckLess == true )
 				return  p1.second < p2.second;
 			else
 				return  p1.second > p2.second;
 		}

 		return true;
 	}

	TSDB_OBJECT_COMPARE::Type CompareType;
	bool CheckLess;
};

time_t const MAX_TSDB_UPDATE_INTERVAL = 60;

int const TSDB_UPDATE_NO_SYNC			= 0x0001;
int const TSDB_UPDATE_NO_OVERWRITE		= 0x0002;
int const TSDB_UPDATE_MERGE				= 0x0004;
int const TSDB_UPDATE_NO_INSERT			= 0x0008;
int const TSDB_UPDATE_OVERWRITE			= 0x00016;

/*
**  Define the structure used by old symbols to maintain compatibilty
**	with .tbb files
*/

#define	OLD_SYMBOL_SIZE		16
#define OLD_DSCRP_SIZE		32

#if !defined( sunos )
#pragma pack(2)
#endif

struct OLD_SYMBOL
{
	char	Name[OLD_SYMBOL_SIZE],		// Name of symbol
			Fname[PATH_SIZE],		// File name of symbol
			Dscrp[OLD_DSCRP_SIZE],	// English description of symbol
			ft,						// Symbol type
			TmpFile;				// Flag indicates tmp file

	FILE	*pFile;					// Stream - NULL if not open

	char	sFlag;					// 0->vector; 1->scalar

	TSDB_POINT	v;						// Vector point

};

#if !defined(sunos)
#pragma pack()
#endif

/*
**  Define the structure for symbol table
*/

struct SYMBOL_TABLE
{
	SYMBOL	*s,						// array of symbols
			**st;					// Array of pointers to only time-series

	unsigned int
			size,					// size of array
			StSize;					// Number of elements in st array

	FILE	*f;						// pointer to single file of 1st fsize symbols

	char	Fname[PATH_SIZE];		// file name

	unsigned int
			fsize;					// number of symbols in file f

	char 	symtab_name[PATH_SIZE];

	int		ft;

	DATE	xLow,
			xHigh,
			xTarget,
			FreqDays;

	int		FreqMonths,
			Observation,
			MergedPointSize,
			NoMorePoints,
			MergeType,
			TSTarget;

	unsigned char
			*PointBuffer;

	void	*extra;
};


/*
**  Define structure for functions & operators
*/

struct TSDB_OLD_FUNC_INFO
{
	int     NumArg,					// # of arguments taken
			OpPrec;					// relative precedence or 0 if not operator

	char    *Name;					// name of function

	SYMBOL  *(*pSymFunc)();			// pointer to function (takes TSDBARG)
	char	*Description;			// character description

};


/*
**	Structure to hold function information
*/

struct TSDB_FUNCTION_INFO
{
	char	*Name,
			*DllPath,
			*FuncName,
			*Description;

	int		NumArg,
			OpPrec;

	TSDB_FUNC
			pfFunc;

};

typedef int (*TSDB_ITERATE_FUNC)( TSDB_FUNCTION_INFO *TsdbFuncInfo, void *Param);

/*
**  Define structure for functions/operators table
*/

struct FUNC_TABLE
{
	TSDB_OLD_FUNC_INFO
			*pFunc;					// pointer to function

	int		size;					// size of table

};


/*
**  define structure for token
*/

struct TOKEN
{
	char	Type;		 			/* type: Constant, symbol, or function */

	union
	{
		TSDB_FUNCTION_INFO			/* pointer to function information */
				*pFunc;

		struct _sym_and_field
		{
			SYMBOL	*pSymb;			// symbol

			char	field;			// field
		} p;

		double	Const;				/* constant value */

		char	*pChar;				/* char string  */

	} t;

};


/*
** define structure for symbol(operator) stack
*/

struct TOKEN_STACK
{
	TOKEN	*Token;					/* array of symbols */
	TOKEN	*pStk;					/* stack pointer  */
	int		size;		 			/* added by A4 */
	TOKEN	*Top;					// added SRW for TsdbArgToSymb

};


/*
** define structure for evaluation stack
** modified 29jun88 - Symbol placed on heap
*/

struct SYMBOL_STACK
{
	SYMBOL *Symbol;			/* array of symbols (needs initialization)*/
	SYMBOL *pStk;				/* stack pointer  */
	int size;					/* added by EPS September 5, 1990  6:22 pm */

};


/*
**	Define internal tsdb struct
**	used to pass state
*/

struct TSDB_HELP
{
	const char *TsdbName;
	const char *Usage;
	const char *Parameters;
	const char *Summary;
	const char *References;
	const char *Description;
	const char *Example;
	const char *SourceFile;
};

struct TSDBARG
{
	SYMBOL			*OutSymb;		// address for output
	SYMBOL_TABLE	*SymbTab;		// symbol table
	TSDB_FUNCTION_INFO *CurrentFunc;// calling function
	TOKEN_STACK		token_stack;	// symbol stack
	SYMBOL_STACK	SymbSt;			// symbol stack pointer
	const TSDB_HELP	*Help;			// Filled in when asking for help
	DATE			xlow;			// Low date of range
	DATE			xhigh;			// High date of range
	bool			RealTime;		// True if xlow/xhigh are times, else dates
	int				lags;			// # of lags
	char			*OutTokList;	// parsed string that generated tokens
	int				RetrieveHelp;   // TRUE to retrieve help, FALSE to execute
};

#define TSDB_PARSE_HELP( TS, FUNCHELP ) if ((TS)->RetrieveHelp) { (TS)->Help = FUNCHELP; return ( NULL ); }


#if defined( IN_MEMPOOL_H )
DLLEXPORT MEM_POOL
	*MemPoolTsdb;                   // Pointer to TSDB memory pool
#endif


/*
**	Configuration structure
*/

#if defined( IN_KOOL_ADE_H )

DLLEXPORT CONFIGURATION
		*TsdbConfig;

#endif


/*
**	Structure used to assign symbol functions to symbols
*/

struct SYM_ASSIGN
{
	int		Type;

	SYMFUNC	SymFunc;

	const char	
		*DllPath,
		*FuncName;

};

#if defined( IN_SYMFUNC_C )
    EXPORT_C_EXPORT 
#else
    EXPORT_C_IMPORT 
#endif
SYM_ASSIGN SymAssignments[];

/*
**	Prototypes moved in tsdbprot.h
*/

#include <tsdbprot.h>

#endif
