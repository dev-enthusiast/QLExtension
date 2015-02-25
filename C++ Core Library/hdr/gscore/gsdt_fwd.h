/****************************************************************
**
**	gscore/gsdt_fwd.h	- Forward declares for gsdt types
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/gsdt_fwd.h,v 1.16 2001/11/27 22:41:12 procmon Exp $
**
****************************************************************/

#ifndef IN_GSCORE_GSDT_FWD_H
#define IN_GSCORE_GSDT_FWD_H

CC_BEGIN_NAMESPACE( Gs )


/*
**	Classes someone might want to use in other headers
*/

class GsDt;
class GsDtDouble;
typedef GsDtDouble GsDtInteger;
class GsDtString;
class GsDtSymbol;
class GsDtVector;
class GsDtMatrix;
class GsDtArray;
class GsDtTime;
class GsDtDate;
class GsDtPTime;
class GsDtCTime;
class GsDtRDate;
class GsDtSymDate;
class GsDtDayCountISDA;
class GsDtDayCount;
class GsDtDictionary;
class GsDtFuncHandle;
template< class Domain, class Range > class GsDtFuncRepT;
template< class Domain1, class Domain2, class Range > class GsDtFunc2RepT;
template<class T>   class GsDtGeneric;
template<class T>   class GsDtGenVector;

template< class T, class R, class S, class I > class GsDtTraited;

template< class T > class GsDtDefaultIterator;
template< class T > struct default_rtti_traits;
template< class T > struct default_subscript_traits;

template< class T > class GsDtBasicStlIterator;
template< class T > struct stl_rtti_traits;
template< class T > struct stl_subscript_traits;

template< class T > class GsTypedVector;
template< class To, class From, class Container > class GsDtTypedVector;

CC_END_NAMESPACE
#endif

