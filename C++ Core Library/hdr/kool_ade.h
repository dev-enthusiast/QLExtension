/* $Header: /home/cvs/src/kool_ade/src/kool_ade.h,v 1.67 2013/06/18 00:53:01 khodod Exp $ */
/****************************************************************
**
**	KOOL_ADE.H
**
**	$Header: /home/cvs/src/kool_ade/src/kool_ade.h,v 1.67 2013/06/18 00:53:01 khodod Exp $
**
****************************************************************/

#if !defined( IN_KOOL_ADE_H )
#define IN_KOOL_ADE_H

#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <port_int.h>
#include <vector>
#include <string>

// hack to include sys/param.h on linux in order to fix warning about MIN/MAX redefinition
#if defined( INCLUDE_SYS_PARAM ) && defined( linux )
#include <sys/param.h>
#endif

/*
**	StrReplace constants
*/

#define REPL_GLOBAL	0x0001		// Global replace within string
#define REPL_FREE	0x0002		// Free Input argument
#define REPL_CASE	0x0004		// Case sensitive replacement


/*
**	Trivia
*/

#define	NULLSTR(m_s)				((m_s)?(m_s):"(null)")
#define	EMPTY_STRING(m_s)			((m_s)?(m_s):"")
#define SWAP(a,b)					((a) ^= ((b) ^= ((a) ^= (b))))

// We undefine MIN and MAX here because they tend to appear in system headers,
// which are included before kool_ade.h.

#undef MIN
#define MIN( a, b )		            (((a) < (b)) ? (a) : (b))
#undef MAX
#define MAX( a, b )			        (((a) > (b)) ? (a) : (b))

#define MIN_STABLE(a,b,exp)			((a) < ((b)=(exp)) ? (a) : (b))
#define MAX_STABLE(a,b,exp)			((a) > ((b)=(exp)) ? (a) : (b))
#define BETWEEN(x,min,max)			((min) <= (x) && (x) <= (max))
#define STRICTLY_BETWEEN(x,min,max)	((min) < (x) && (x) < (max))
#define CONFINE(x,min,max)			((x) < (min) ? ((x) = (min)) : ((x) > (max) ? ((x) = (max)) : (x)))

#if defined( IBM ) || defined( IBM_CPP )
#	define STRUCT_OFFSET(t,c)		(size_t)&(((t *)0)->c)
#else
#	define STRUCT_OFFSET(t,c)  		((char *)&(((t *)NULL)->c) - (char *)NULL)
#endif


/*
**  DT_CURVE_KNOT is based on KA_CURVE_KNOT. TSDB_POINT is asserted to be
**  the same. Note that TSDB_POINT accesses the members as x and y while
**	DT_CURVE_KNOT calls them Date and Value. Keeping DT_CURVE_KNOT and
**	TSDB_POINT identical in their memory structure allows us to cast from
**	one to the other.
*/

typedef struct KACurveKnotStructure
{
	long	Date;	// Date or time or rdate

	double	Value;	// Corresponding value

} KA_CURVE_KNOT;


/*
**	Macros used to add/extract data to and from streams
*/

typedef char *STREAM;

#define	STREAM_TO_STR(Stream,Mem,Size)		{(Stream) += sprintf((Mem),"%.*s",(Size)-1,(Stream)); (Stream) += strlen( Stream ) + 1;}
#define	STREAM_TO_MEM(Stream,Mem,Size)		{memcpy((Mem),(Stream),(Size));(Stream) += Size;}
#define	STR_TO_STREAM(Stream,Mem)			((Stream) += sprintf((Stream),"%s",(Mem))+1)
#define	MEM_TO_STREAM(Stream,Mem,Size)		{memcpy((Stream),(Mem),(Size));(Stream) += Size;}
#define	STREAM_SKIP(Stream,Size)			((Stream) += (Size))


/*
**	Range checking
*/

#define	VALID_DOUBLE(dbl)			( !IS_NAN_DOUBLE( dbl ) && (dbl) != HUGE_VAL && (dbl) != -HUGE_VAL )
#define VALID_FLOAT(flt)			( !IS_NAN_FLOAT( flt ))
#define IN_RANGE_DOUBLE(dbl,low,hi)	( VALID_DOUBLE( dbl ) && BETWEEN( dbl, low, hi ))
#define IN_RANGE_FLOAT(flt,low,hi)	( VALID_FLOAT( flt )  && BETWEEN( flt, low, hi ))



#if !defined(TRUE)
#define TRUE	1
#define FALSE	0
#endif


/****************************************************************
**
**	High resolution time routine
**
****************************************************************/

DLLEXPORT double
		Now( void );


/****************************************************************
**
**	Crc routines
**
****************************************************************/

DLLEXPORT UINT16
		Crc16( const void *Stream, size_t StreamLength, UINT16 InitialCrc );

DLLEXPORT UINT32
		Crc32( const void *Stream, size_t StreamLength, UINT32 InitialCrc );


/****************************************************************
**
**	Copy files
**
****************************************************************/

DLLEXPORT int
		FileCopy( const char *Old, const char *New );


/****************************************************************
**
**	Flush all I/O
**
****************************************************************/

DLLEXPORT void
		FlushToDevice(	FILE *File );


/****************************************************************
**
**	Remap OS/2 filenames to portable filenames.
**
****************************************************************/

DLLEXPORT const char
		* NormalizeFilename( const char *FileName );
DLLEXPORT const char
		* UNCFilename(const char *FileName, char *OutBuf, int OutSize);
DLLEXPORT const char
		* NoUNCFilename(const char *FileName, char *OutBuf, int OutSize, int Reset, int Remap);


/****************************************************************
**
**	Pipe I/O
**
****************************************************************/

DLLEXPORT FILE
		* Popen(	const char *Command, const char *Mode );

DLLEXPORT int
		PopenRW(	const char *Command, FILE **Pipes ),
		Pclose(		FILE *Pipe );


/****************************************************************
**
**	Execute a program
**
****************************************************************/

DLLEXPORT int
		Shell(		const char *Command ),
		ShellEditor(const char *FileName ),
		ShellPS(	const char *Flags, const char *FileName );


/****************************************************************
**
**	String support routines
**
****************************************************************/

DLLEXPORT int
		StrBegins(		const char *Str, const char *BegPart ),
		StrEnds(		const char *Str, const char *EndPart ),
		StrIBegins(		const char *Str, const char *BegPart ),
		StrIEnds(		const char *Str, const char *EndPart ),
		StrTokenize(	char *String, int MaxTokens, char **Tokens ),
		StrWildMatch(	const char *Pattern, const char *String ),
		StrChrCount(	const char *String, const char *CharSet, int Start, int End );

DLLEXPORT char
		*StrFromUrl(	const char *Str ),
		*StrIStr(		char *String1, const char *String2 ),
		*StrPaste(		const char *First, ... ),
		*StrQuoteHtml(	const char *Str ),
		*StrQuoteUrl(	const char *Str ),
		*StrReplace(	char *Input, const char *SubStr, const char *Replace, int Flags ),
		*StrLTrim(		char *Str ),
		*StrRTrim(		char *Str );


DLLEXPORT const char *StrBaseName(   const char *Path );

/****************************************************************
**
**	Configuration file routines
**
****************************************************************/

typedef struct HashStructure		CONFIGURATION;

DLLEXPORT CONFIGURATION
		* ConfigurationLoad(		const char *FileName, const char *Path, char *ErrorBuffer );

DLLEXPORT void
		 ConfigurationUnload(		CONFIGURATION *Configuration );

DLLEXPORT char
		* ConfigurationGet(			CONFIGURATION *Configuration, const char *VariableName );

DLLEXPORT int
		ConfigurationStuff(			CONFIGURATION *Configuration );

DLLEXPORT int
		ConfigurationLoadNStuff(	const char *FileName, const char *Path );

DLLEXPORT void
		ConfigurationSet(			CONFIGURATION *Configuration, const char *VariableName, const char *VariableValue );


/****************************************************************
**
**	Searching and Sorting
**
****************************************************************/

typedef int
		(*HUGE_COMPARE)( void _huge *Elem1, void _huge *Elem2 );

DLLEXPORT void
		HugeQSort(	void _huge *Base, long Num, size_t Width, HUGE_COMPARE Compare );

DLLEXPORT int
		KASearchPath(	const char *Path, const char *FileName, char *Buf, int BufSize );

typedef int
		(*BSearchCompare)( const void *, const void * );

// does a bsearch and returns the closest match
// (unlike bsearch which just fails if it couldn't find the key)
DLLEXPORT const void
		*KABinarySearchNearest( const void *Key, const void *Base, int Num, int Size, BSearchCompare Compare );


/****************************************************************
**
**	String tables
**
****************************************************************/

typedef const char * const STRING_TABLE;	// The constant version
typedef char *STRING_TABLE_MOD;				// Non-constant version

DLLEXPORT int
		LFind(	const char *String, STRING_TABLE* StringTable );

/****************************************************************
**
**	User/Network Functions
**
****************************************************************/

DLLEXPORT int
		LoginName( char *OutputBuffer, unsigned int OutputBufferSize );

/****************************************************************
**
**	Path Configuration functions
**
****************************************************************/

DLLEXPORT int
		WhackPathFromConfig( );

#endif
