/****************************************************************
**
**	err_base.h	- Basic error handling moved here from kool_ade
**                so that core libraries can set errors without
**                having to link against kool_ade.
**                No API to get ErrorString because that needs
**                hash tables in order to lookup the names of
**                the error numbers.
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/kool_ade/src/err_base.h,v 1.10 2012/05/31 17:48:26 e19351 Exp $
**
****************************************************************/

#if !defined( IN_KOOL_ADE_ERR_BASE_H )
#define IN_KOOL_ADE_ERR_BASE_H

#include	<stdarg.h>		// va_list for ErrV
#include	<stddef.h>		// size_t for ERR_*ALLOC macros
#include	<malloc.h>		// not necessary, but included since we are referencing these in ERR_*ALLOC


/*
**	Standard error codes
*/

#define	ERR_DONT_CHANGE				(-1)	// Used internally to not change ErrNum
#define	ERR_NONE					0
#define	ERR_UNKNOWN					1

#define	ERR_INVALID_ARGUMENTS		100
#define	ERR_INVALID_CHARACTER		101
#define	ERR_INVALID_NUMBER			102
#define	ERR_INVALID_STRING			103
#define	ERR_INVALID_INDEX			104		// Range error on vector/matrix/array
#define	ERR_INVALID_SIZE			105		// Mismatched sizes on vector/matrix/array

#define	ERR_MEMORY					200
#define	ERR_UNSUPPORTED_OPERATION	201
#define	ERR_DIVIDE_BY_ZERO			202
#define	ERR_OVERFLOW				203
#define	ERR_UNDERFLOW				204
#define	ERR_INCOMPLETE				205
#define	ERR_NOTHING_CHANGED	  		206
#define	ERR_NOT_FOUND		 		207
#define	ERR_NOT_STARTED				208
#define	ERR_NOT_CONSISTENT	 		209
#define	ERR_VERSION_MISMATCH		210

#define	ERR_FILE_INVALID			300
#define	ERR_FILE_FAILURE  			301
#define	ERR_FILE_READ_ONLY			302
#define	ERR_FILE_NOT_FOUND			303
#define	ERR_FILE_IN_USE   			304
#define	ERR_FILE_ALREADY_EXISTS		305
#define	ERR_FILE_MODIFIED			306

#define	ERR_DATABASE_INVALID		400
#define	ERR_DATABASE_FAILURE  		401
#define	ERR_DATABASE_READ_ONLY		402
#define	ERR_DATABASE_NOT_FOUND		403
#define	ERR_DATABASE_IN_USE   		404
#define	ERR_DATABASE_ALREADY_EXISTS	405
#define	ERR_DATABASE_MODIFIED	 	406
#define	ERR_DATABASE_OBSOLETE	 	407

#define	ERR_NETWORK_FAILURE			500		// Use for send/recv errors
#define	ERR_CONNECTION_LOST			501		// Use for closed sockets
#define	ERR_CONNECTION_FAILURE		502		// Use for application errors
#define	ERR_WOULD_BLOCK				503
#define	ERR_HOST_NOT_FOUND			504
#define	ERR_TIMEOUT					505		// Sockets and others

#define ERR_JAVA_EXCEPT				506		// Java raised an exception
#define ERR_PYTHON_EXCEPT			507		// Python raised exception

/*
**	Standard error levels
*/

#define ERR_LEVEL_DEBUG	 0		// Debugging levels 0-9
#define ERR_LEVEL_INFO	10		// User messages
#define ERR_LEVEL_ERROR	20		// Errors
#define ERR_LEVEL_FATAL	30		// Fatal software bugs

/*
**	Define types
*/

// FIX-This #define can go away once all of the relevant files are found and fixed and prodverd
// FIX-Phase 1 - use ERR_C_CHAR instead of "char", prodver
// FIX-Phase 2 - change ERR_C_CHAR to "const char" for DEV
// FIX-Phase 3 - prodver phase 2 once all have been fixed
// FIX-Phase 4 - remove ERR_C_CHAR everywhere and prodver
// FIX-Phase 5 - remove ERR_C_CHAR from err.h and prodver
#define ERR_C_CHAR const char
typedef int		ERR_NUM;
typedef int		ERR_LEVEL;
typedef	void	(*ERR_HOOK_FUNC)( ERR_NUM ErrNum, ERR_C_CHAR *ErrText );
typedef	ERR_LEVEL	(*ERR_MSG_HOOK_FUNC)( ERR_LEVEL Level, ERR_C_CHAR *ErrText );
typedef	ERR_LEVEL	(*STATUS_MSG_HOOK_FUNC)( ERR_C_CHAR *ErrText );
typedef void	(*ERR_ABORT_HOOK_FUNC)( void );


/*
**	Define maximum error length
*/

#define	ERR_MAX_STR		4096


/*
**	Variables
*/

DLLEXPORT int
		ErrOffLevel,
		ErrStrOffLevel;

DLLEXPORT ERR_NUM
		ErrNum;				// Error number

DLLEXPORT ERR_HOOK_FUNC
		ErrHookFunc;		// Called from Err & ErrMore & ErrAppend

DLLEXPORT ERR_LEVEL
		ErrLevel;			// Controls calls to ErrMsgHookFunc from ErrMsg

DLLEXPORT ERR_MSG_HOOK_FUNC	// Called if Level >= ErrLevel
		ErrMsgHookFunc;		// Returns adjusted error level 

DLLEXPORT STATUS_MSG_HOOK_FUNC
		StatusMsgHookFunc;		// Returns adjusted error level 

DLLEXPORT char
		ErrStrDontChange;	// Sentinel used to tell Err not to change ErrStr instead of passing NULL

DLLEXPORT ERR_ABORT_HOOK_FUNC
		ErrAbortHookFunc;	// Will be called when abort is request. MUST be threadsafe


/*
**	Error-checking allocation macros
*/

#define ERR_CKALLOC(expr)					( (expr) ? TRUE : Err( ERR_MEMORY, "@ %s:%d", __FILE__, __LINE__ ))
#define ERR_MALLOC(var,type,size)			ERR_CKALLOC( ((var) = (type *)malloc( size )) || (size) == 0 )
#define ERR_CALLOC(var,type,num,size)		ERR_CKALLOC( ((var) = (type *)calloc( (num), (size) )) || (num) == 0 || (size) == 0 )
#define ERR_STRDUP(tgt,src)					ERR_CKALLOC( (tgt) = strdup(src) )
#define ERR_REALLOC(var,type,size)			ERR_CKALLOC( ((var) = (type *)ErrReallocWrapper((var),(size))) || (size) == 0 )
#define ERR_REALLOC_WOF(var,type,size,ofvar,tmp)	( (tmp) = (ofvar) - (var), (var) = (type *)ErrReallocWrapper( (var), (size) ), (ofvar) = ( (var) ? (var) + (tmp) : NULL ), ERR_CKALLOC( var ))

#define ERR_OFF()							( (void) ++ErrOffLevel )
#define ERR_ON()							( (void) ( ErrOffLevel = ErrOffLevel > 0 ? ErrOffLevel - 1 : 0 ))

#define ERR_STR_OFF()						( (void) ++ErrStrOffLevel )
#define ERR_STR_ON()						( (void) ( ErrStrOffLevel = ErrStrOffLevel > 0 ? ErrStrOffLevel - 1 : 0 ))

#define ERR_STR_DONT_CHANGE					( (const char *) &ErrStrDontChange )

#define ERR_MORE_SENTINEL_CHAR				'\255'

/*
**	Prototype functions
*/

DLLEXPORT int
		Err(		ERR_NUM ErrNum, const char *Format, ... ) ARGS_LIKE_PRINTF(2),
		ErrMore(	const char *Format, ... ) ARGS_LIKE_PRINTF(1),
		ErrMoreV(	const char *Format, va_list Marker ),
		ErrAppend(	const char *Format, ... ) ARGS_LIKE_PRINTF(1),
		ErrV(		const char *Format, va_list Marker ),
		ErrVN(		ERR_NUM ErrNum, const char *Format, va_list Marker ),
		ErrMsg(		ERR_LEVEL Level, const char *Format, ... ) ARGS_LIKE_PRINTF(2),
		StatusMessage(	 const char *Format, ... ) ARGS_LIKE_PRINTF(1),
		StatusMessageVN( const char *Format, va_list Marker );

/*
** Double (HUGE_VAL) returning versions of the above.
*/

DLLEXPORT double
		ErrD(		ERR_NUM ErrNum, const char *Format, ... ) ARGS_LIKE_PRINTF(2),
		ErrMoreD(	const char *Format, ... ) ARGS_LIKE_PRINTF(1);


/*
** (void *) NULL returning versions of the above.
*/

DLLEXPORT void
		*ErrN(		ERR_NUM ErrNum, const char *Format, ... ) ARGS_LIKE_PRINTF(2),
		*ErrMoreN(	const char *Format, ... ) ARGS_LIKE_PRINTF(1);


DLLEXPORT void
		ErrClear(			void );

DLLEXPORT void
		ErrPush(			void ),
		ErrPop(				void );

DLLEXPORT const char
		*ErrStrRawGet(),
		*ErrStrEndGet();

#endif 
