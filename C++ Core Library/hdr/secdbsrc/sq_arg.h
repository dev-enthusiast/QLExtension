/****************************************************************
**
**	SQ_ARG.H	- Argument parsing support
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sq_arg.h,v 1.1 2000/09/01 10:03:58 goodfj Exp $
**
****************************************************************/

#if !defined( IN_SQ_ARG_H )
#define IN_SQ_ARG_H

#ifndef IN_QTYPES_H
#include <qtypes.h>
#endif
#ifndef IN_SECEXPR_H
#include <secexpr.h>
#endif

typedef char *String;

DLLEXPORT int		SqArg_String( DT_VALUE *Value, String *String, SLANG_NODE *Root, SLANG_SCOPE *Scope, const char *ArgName, int ArgNum );


#define SQ_INIT_Q_VECTOR(Vec) 		memset(&(Vec),0,sizeof(Vec))
#define SQ_INIT_Q_DATE_VECTOR(Vec)	memset(&(Vec),0,sizeof(Vec))
#define SQ_FREE_Q_VECTOR(Vec)		free( Vec.V )
#define SQ_FREE_Q_DATE_VECTOR(Vec)	free( Vec.V )


DLLEXPORT int		SqArg_Q_VECTOR( DT_VALUE *Value, Q_VECTOR *Vector, SLANG_NODE *Root, SLANG_SCOPE *Scope, const char *ArgName, int ArgNum );
DLLEXPORT int		SqArg_Q_DATE_VECTOR( DT_VALUE *Value, Q_DATE_VECTOR *Vector, SLANG_NODE *Root, SLANG_SCOPE *Scope, const char *ArgName, int ArgNum );
DLLEXPORT int		SqArg_DT_CURVE ( DT_VALUE *Value, DT_CURVE *Curve, SLANG_NODE *Root, SLANG_SCOPE *Scope, const char *ArgName, int ArgNum );


#define SQ_INIT_Q_MATRIX(Matrix) memset(&(Matrix),0,sizeof(Matrix))
#define SQ_INIT_Q_CURVE_MATRIX(Matrix) memset(&(Matrix),0,sizeof(Matrix))
#define SQ_INIT_DT_CURVE(Curve) memset(&(Curve),0,sizeof(Curve))
#define SQ_FREE_Q_MATRIX(Matrix) free( Matrix.M )
#define SQ_FREE_DT_CURVE(Curve) 


DLLEXPORT int		SqArg_Q_MATRIX( DT_VALUE *Value, Q_MATRIX *Matrix, SLANG_NODE *Root, SLANG_SCOPE *Scope, const char *ArgName, int ArgNum );
DLLEXPORT int		SqArg_Q_CURVE_MATRIX( DT_VALUE *Value, Q_CURVE_MATRIX *Matrix, SLANG_NODE *Root, SLANG_SCOPE *Scope, const char *ArgName, int ArgNum );

#endif

