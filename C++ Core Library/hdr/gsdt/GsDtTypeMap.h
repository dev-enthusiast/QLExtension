/****************************************************************
**
**	GSDTTYPEMAP.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtTypeMap.h,v 1.13 2011/12/08 20:34:01 e19351 Exp $
**
****************************************************************/

#ifndef IN_GSDT_GSDTTYPEMAP_H
#define IN_GSDT_GSDTTYPEMAP_H

#include <gsdt/base.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/types.h> // for GsString typedef

CC_BEGIN_NAMESPACE( Gs )

template<class T> struct GsDtTypeMap                   { typedef typename T::GsDtType GsDtType; };

template<>        struct GsDtTypeMap< double >         { typedef GsDtDouble           GsDtType; };
template<>        struct GsDtTypeMap< int >            { typedef GsDtDouble           GsDtType; };
template<>        struct GsDtTypeMap< unsigned int >   { typedef GsDtDouble           GsDtType; };
template<>        struct GsDtTypeMap< long int >       { typedef GsDtDouble           GsDtType; };
template<>        struct GsDtTypeMap< long unsigned int >       { typedef GsDtDouble           GsDtType; };
template<>        struct GsDtTypeMap< GsDtDictionary > { typedef GsDtDictionary       GsDtType; };
template<>        struct GsDtTypeMap< GsDtArray >      { typedef GsDtArray            GsDtType; };
template<>        struct GsDtTypeMap< GsString >       { typedef GsDtString           GsDtType; };
template<>        struct GsDtTypeMap< GsDtDouble >     { typedef GsDtDouble           GsDtType; };

template<>        struct GsDtTypeMap< const GsVector & >     { typedef GsDtVector     GsDtType; };

CC_END_NAMESPACE

#endif 
