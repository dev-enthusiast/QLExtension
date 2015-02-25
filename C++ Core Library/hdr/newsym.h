/****************************************************************
**
**	NEWSYM.H
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/newsym.h,v 1.34 2014/09/26 19:10:05 e19351 Exp $
**
****************************************************************/

#if !defined( IN_NEWSYM_H )
#define	IN_NEWSYM_H

#include <string.h>	/* for strerror */
#include <errno.h> 	/* for errno */

#if !defined( IN_TURBO_H )
#include <turbo.h>	/* for SYMBOL_TABLE */
#endif

/*
**	Define constants
*/

#define	NUM_PNTS_TO_BUFFER		1000		// Pts to buffer for a symbol
#define	NUM_BYTES_MERGED_BUF	32000		// Num bytes for merged buffer
#define	NUM_BYTES_TMP_FILE_BUF	13000		// Num bytes used to buf some files

// Max pts for memory symbol
#define	NUM_PNTS_MEM_SYM	18250			// 50 years worth of data


/*
**	Define operation types sent to symbol handling functions
*/

#define	SYM_OPTYPE_INITIALIZE	1
#define	SYM_OPTYPE_OPEN			2
#define	SYM_OPTYPE_OPEN_UPDATE	3
#define	SYM_OPTYPE_CLOSE  		4
#define	SYM_OPTYPE_GETMORE		5
#define	SYM_OPTYPE_FLUSH  		6
#define	SYM_OPTYPE_POS	  		7
#define	SYM_OPTYPE_POS_BEFORE	8
#define	SYM_OPTYPE_DESTROY		9
#define	SYM_OPTYPE_CREATE		10
#define	SYM_OPTYPE_LASTPOINT	11
#define	SYM_OPTYPE_FIRSTPOINT	12
#define	SYM_OPTYPE_READ			13
#define	SYM_OPTYPE_WRITE		14
#define	SYM_OPTYPE_VALIDATE		15
#define	SYM_OPTYPE_UPDATE		16
#define	SYM_OPTYPE_MODIFIERS	17

#define	SYM_OPTYPE_USER			1000		// This number and up are for the symfuncs


/*
**	Macros to handle reading symbols
*/

#define	SYM_MSG(s,m,x,n,p)		(((s)->SymFunc)((m),(s),(x),(n),(p)))

// Note about hinted (SYM_*_HINT) operations:
//
// Hints are OPTIONAL upper or lower bounds for operations. If a hint is
// passed, symbols are free to give up when they've reached the hint,
// but aren't required to. The result of "giving up" depends on the
// operation; see the comments for each below. The design of this behaviour
// was intended to:
//   1. Preserve backwards compatibility with old symbol types, by allowing
//      them to ignore hints.
//   2. Minimize changes necessary to convert old code to the new hinted API.
//
// ************* Symbols are free to disregard the hint entirely! *************
// Think of hints as a suggestion, "feel free to stop at this boundary if it
// would be easier."
//
// Hints are always pointers to DATE fields. NULL indicates the lack of a hint,
// and will behave identically to the equivalent non-hinted operation.
//
// This is most helpful for symbols that are bad at indexed access, such as
// SQL databases. Without a hinted bound, symbols often need to hunt for points
// well beyond what the user actually wants, just to fulfil the operation
// contract (e.g. SYM_POS and SYM_GETMORE, which have to search until they
// find at least one point). That makes a single bulk load of the data nearly
// impossible.

// Initialize SYMBOL structure (typically just sets point sizes; most actual
// initialization is done by the symbol table). Can't allocate memory here,
// as there's no SYM_FREE to release it when the symbol is no longer needed.
#define	SYM_INITIALIZE(s)	  	((s)->SymFunc)(SYM_OPTYPE_INITIALIZE,	(s),0,0,NULL)
// Open a symbol for reading, seeking (see SYM_POS) to the given initial date
#define	SYM_OPEN(s,x,n)			SYM_OPEN_HINT(s,x,n,NULL)
// OPEN with hint on upper bound, behaves like SYM_POS_HINT. By calling this
// with a good lower bound, and an appropriate upper bound hint, it's possible
// that the symbol will buffer the entire range up front.
#define	SYM_OPEN_HINT(s,x,n,h)	((s)->SymFunc)(SYM_OPTYPE_OPEN,			(s),(x),(n),(void*)static_cast<DATE *>( h ))
// Close a symbol, freeing buffers and any handles or memory allocated via SYM_OPEN
#define	SYM_CLOSE(s)			((s)->SymFunc)(SYM_OPTYPE_CLOSE,		(s),0,0,NULL)
// Attempt to retrieve data after the current buffer, returns whether available.
// Also returns FALSE on errors.. important to check error code.
#define	SYM_GETMORE(s)			SYM_GETMORE_HINT(s,NULL)
// GETMORE with hint on upper bound. May return false after hint is reached,
// regardless of data availability.
#define	SYM_GETMORE_HINT(s,h)	((s)->SymFunc)(SYM_OPTYPE_GETMORE,		(s),0,0,(void*)static_cast<DATE *>( h ))
// Seek to given date, or the point immediately after if it is missing.
#define	SYM_POS(s,x)			SYM_POS_HINT(s,x,NULL)
// POS with hint on upper bound. If hint < POS, symbol MAY stop seeking at any
// point in [hint, POS], including end points (CurPoint==EndPoint), so long as
// a subsequent GETMORE_HINT(s,hint) returns FALSE.
#define	SYM_POS_HINT(s,x,h)		((s)->SymFunc)(SYM_OPTYPE_POS,			(s),(x),0,(void*)static_cast<DATE *>( h ))
// Seek to point on or before a given date.
#define	SYM_POS_BEFORE(s,x)		SYM_POS_BEFORE_HINT(s,x,NULL)
// POS_BEFORE with hint on lower bound. If hint > POS_BEFORE, symbol MAY
// stop seeking at any point in [POS_BEFORE(s,x), POS_HINT(s,h,h)].
#define	SYM_POS_BEFORE_HINT(s,x,h) \
								((s)->SymFunc)(SYM_OPTYPE_POS_BEFORE,	(s),(x),0,(void*)static_cast<DATE *>( h ))
// Seek to last point in symbol
#define	SYM_LASTPOINT(s)		((s)->SymFunc)(SYM_OPTYPE_LASTPOINT,	(s),0,0,NULL)
// Seek to first point in symbol
#define	SYM_FIRSTPOINT(s)		((s)->SymFunc)(SYM_OPTYPE_FIRSTPOINT,	(s),0,0,NULL)

// Read a point into given point array. This will load consecutive points
// with repeated calls, and will obey a preceding POS/POS_BEFORE, but are not
// guaranteed to update the point iterators (CurPoint/EndPoint), so DO NOT use
// iterators (including calls to GET/GETMORE) after READ without an explicit
// seek in between. Why doesn't it always update the iterators? Who knows.
// I recommend using GET and considering READ/BLOCK_READ deprecated.
#define	SYM_READ(s,p)			SYM_READ_HINT(s,p,NULL)
// READ with hint on upper bound
#define	SYM_READ_HINT(s,p,h)	((s)->SymFunc)(SYM_OPTYPE_READ,			(s),(h ? *static_cast<DATE *>( h ) : 0),1,(p))
// Read multiple points into given point array (same caveats as READ)
#define	SYM_BLOCK_READ(s,n,p)	SYM_BLOCK_READ_HINT(s,n,p,NULL)
// BLOCK_READ with hint on upper bound
#define	SYM_BLOCK_READ_HINT(s,n,p,h) \
								((s)->SymFunc)(SYM_OPTYPE_READ,			(s),(h ? *static_cast<DATE *>( h ) : 0),(n),(p))
								
// Reserved for symbol-dependent magic operations
#define	SYM_USER(s)				((s)->SymFunc)(SYM_OPTYPE_USER,			(s),0,0,NULL)
// Retrieve a point while incrementing iterator; will call GETMORE if needed.
#define	SYM_GET(s,p) SYM_GET_HINT(s,p,NULL)
// GET with hint on upper bound (passed to GETMORE)
#define SYM_GET_HINT(s,p,g)	if( (s)->CurPoint< (s)->EndPoint) (p)=(s)->CurPoint++;\
							else if(SYM_GETMORE_HINT(s,g))(p)=(s)->CurPoint++;\
							else (p)=NULL;

#define SYM_STRING(s) ((s)->ft == tsdb_STRING_FMT ? ((s)->PointBuffer == NULL ? (s)->Dscrp : (char *)((s)->PointBuffer)) : NULL)


/*
**	Macros to handle writing symbols
*/

#define	SYM_OPEN_UPDATE(s,x,n) 	((s)->SymFunc)(SYM_OPTYPE_OPEN_UPDATE,	(s),(x),(n),NULL)
#define	SYM_WRITE(s,p)			((s)->SymFunc)(SYM_OPTYPE_WRITE,		(s),0,1,(p))
#define	SYM_BLOCK_WRITE(s,n,p)	((s)->SymFunc)(SYM_OPTYPE_WRITE,		(s),0,(n),(p))
#define	SYM_FLUSH(s)			((s)->SymFunc)(SYM_OPTYPE_FLUSH,		(s),0,0,NULL)
#define SYM_UPDATE_EXISTS(s)	((s)->SymFunc)(SYM_OPTYPE_UPDATE,		(s),0,0,NULL)
#define SYM_UPDATE(s,n,p)		((s)->SymFunc)(SYM_OPTYPE_UPDATE,		(s),0,(n),(p))
// Create backing store for symbol (e.g. external file), if required
#define	SYM_CREATE(s,n)			((s)->SymFunc)(SYM_OPTYPE_CREATE,		(s),0,(n),NULL)
// Destroy backing store for symbol (e.g. external file), if required
#define	SYM_DESTROY(s)			((s)->SymFunc)(SYM_OPTYPE_DESTROY,		(s),0,0,NULL)

#define	SYM_PUT(s,p)	{*(s)->CurPoint++=(p);\
						if((s)->CurPoint>=(s)->MaxPoint && !SYM_FLUSH(s)) {\
							Err( ERR_UNKNOWN, TsdbErrMsgCantWrite,(s)->Fname,strerror(errno));\
							DestroyOutSym(&s); break;\
						}}
#define	SYM_PUT0(s,p)	{*(s)->CurPoint++=(p);\
						if((s)->CurPoint>=(s)->MaxPoint && !SYM_FLUSH(s)) {\
							Err( ERR_UNKNOWN, TsdbErrMsgCantWrite,(s)->Fname,strerror(errno));\
							DestroyOutSym(&s);\
						}}


/*
**	Buffer used by symbol functions as a temporary buffer for data
**	read from the file.
*/

extern unsigned char
		*TmpFileBuffer;

DLLEXPORT const char
		*TsdbErrMsgCantWrite;


/*
**	Prototype functions
*/

DLLEXPORT void	RemoveArgument(				SYMBOL_TABLE *, int );

DLLEXPORT void 	SetUpTable(					SYMBOL_TABLE *, int, TSDBARG *);

DLLEXPORT SYMBOL
				*GetArguments(				SYMBOL_TABLE *, int, TSDBARG *,int, int, int );
DLLEXPORT SYMBOL
				*CleanUpArguments(			SYMBOL_TABLE *, SYMBOL *, TSDBARG * );
DLLEXPORT void	DestroyOutSym(SYMBOL **pOutSym);

DLLEXPORT int	GetNumArgs(					TSDBARG *t );
DLLEXPORT int	TsdbGetInitialPoints(		SYMBOL_TABLE *, unsigned char **, unsigned char **, int * );
DLLEXPORT int	TsdbGetMorePoints(			SYMBOL_TABLE *, unsigned char **, unsigned char ** );
DLLEXPORT int	TsdbGetInitialSampledPoints(SYMBOL_TABLE *, unsigned char **, unsigned char **, int *, int );
DLLEXPORT int	TsdbGetMoreSampledPoints( 	SYMBOL_TABLE *, unsigned char **, unsigned char ** );
DLLEXPORT int 	ExpandArgumentWithModifiers(SYMBOL_TABLE *, int, int, const char **, int, int ** );


DLLEXPORT double
				GetConstant(				TSDBARG *t );

DLLEXPORT char	*GetString(					char *,	TSDBARG * );

DLLEXPORT int	SymFuncUndefined(			int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncBinary(				int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncConstant(			int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncString(				int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncMemory(				int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncHloc(				int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncHlocvo(				int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncQSJapan(				int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncQSSession(			int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncDbaseSparks(			int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncCurve(				int, SYMBOL *, long, int, void * );
DLLEXPORT int	SymFuncArray(				int, SYMBOL *, long, int, void * );

DLLEXPORT void SymPointsToExternalFormat(	SYMBOL	*Sym,	TSDB_POINT	*Array,	char	*Output,	int		Total);
#endif
