/****************************************************************
**
**	ERR.H	- Error handling functions and codes
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/kool_ade/src/err.h,v 1.47 2001/11/27 22:49:01 procmon Exp $
**
****************************************************************/

#if !defined( IN_ERR_H )
#define IN_ERR_H

// basic error handling stuff is in here
#include    <err_base.h>


/*
**	Prototype functions
*/

DLLEXPORT const char	
		*ErrGetString(		void ),
		*ErrStringFromNum(	ERR_NUM ErrNum );

DLLEXPORT ERR_NUM
		ErrRegister(		ERR_NUM ErrNum, const char *ErrText );

DLLEXPORT void
		*ErrReallocWrapper(	void *Var, size_t Size );

DLLEXPORT int
		ErrWindows( int ErrNo );


/*
**	Error-checking allocation macros
*/

#define ERR_CKALLOC(expr)					( (expr) ? TRUE : Err( ERR_MEMORY, "@ %s:%d", __FILE__, __LINE__ ))
#define ERR_MALLOC(var,type,size)			ERR_CKALLOC( ((var) = (type *)malloc( size )) || (size) == 0 )
#define ERR_CALLOC(var,type,num,size)		ERR_CKALLOC( ((var) = (type *)calloc( (num), (size) )) || (num) == 0 || (size) == 0 )
#define ERR_STRDUP(tgt,src)					ERR_CKALLOC( (tgt) = strdup(src) )
#define ERR_REALLOC(var,type,size)			ERR_CKALLOC( ((var) = (type *)ErrReallocWrapper((var),(size))) || (size) == 0 )
#define ERR_REALLOC_WOF(var,type,size,ofvar,tmp)	( (tmp) = (ofvar) - (var), (var) = (type *)ErrReallocWrapper( (var), (size) ), (ofvar) = ( (var) ? (var) + (tmp) : NULL ), ERR_CKALLOC( var ))

/*
**	Alternate GsErr... API which calls Err..., but returns GsStatus enum
*/

DLLEXPORT GsStatus
		GsErr(		ERR_NUM GsErrNum, const char *Fmt, ... ),
		GsErrMore(	const char *Fmt, ... );

DLLEXPORT const char
		*GsErrGetString();


#endif
