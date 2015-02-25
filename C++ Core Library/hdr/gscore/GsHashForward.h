/****************************************************************
**
**	GSHASHFORWARD.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsHashForward.h,v 1.8 2001/11/27 22:41:06 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSBASE_GSHASHFORWARD_H )
#define IN_GSBASE_GSHASHFORWARD_H

////////////////////////////////////////////////////////////////////////
//
// HOW TO USE THIS FILE:
//
// This file forward declares the GsHashMap templated class.  It
// does this so that we can typedef instantiations of the class
// without causing the compiler to generate code for inline functions
// and then refusing to fully instantiate in Templates.cpp.
//
// To do this, include this file, then do the typedef, then include
// GsHash.h.  Here is an example from the javaglue project:
//
// #include	<gscore/GsHashForward.h>
//
// class 	JavaMethodSignature;
// class  	JavaMethodInfo;
// struct 	JavaMethodSignatureToIntFunctor;
// struct 	JavaMethodSignatureComparator;
//
// typedef Gs::GsHashMap< 
// 		JavaMethodSignature*, JavaMethodInfo*, JavaMethodSignatureToIntFunctor, JavaMethodSignatureComparator > 
// 		SigToMethodHash;
//
// #include	<gscore/GsHash.h>
//
////////////////////////////////////////////////////////////////////////

#include <gscore/base.h>
#include <functional>
#include <gscore/GsString.h>
#include <gscore/GsHashFromString.h>

CC_BEGIN_NAMESPACE( Gs )

// Forward declaration of the templated hash class.  

template<
	class KeyT,
	class ValueT,
	class HashFunctor,
#if defined( CC_NO_TEMPLATE_DEFAULTS )
	class Compare
#else
	class Compare = CC_NS(std,equal_to)<KeyT>
#endif
> 
class GsHashMap;

// An instantiation we will very commonly use.  Note that in order to
// get the MSVC5.0 compiler to not attempt to instantiate at this
// point, we have to do this typedef at a point where we have only the
// forward declatation of the GsHashMap class.  Not sure what SW4.2 does.	

typedef GsHashMap< GsString, void *, GsHashFromString, CC_NS(std,equal_to)<GsString> > GsHashMapStringToVoid;
typedef GsHashMap< GsString, void *, GsHashFromString, StrICmpFunctor > GsHashMapStringToVoidNoCase;
typedef GsHashMap< GsString, const void *, GsHashFromString, CC_NS(std,equal_to)<GsString> > GsHashMapStringToConstVoid;
typedef GsHashMap< GsString, const void *, GsHashFromString, StrICmpFunctor > GsHashMapStringToConstVoidNoCase;
typedef GsHashMap< GsString, int, GsHashFromString, CC_NS(std,equal_to)<GsString> > GsHashMapStringToInt;


/*
**	Helper functions
*/

EXPORT_CPP_GSBASE size_t
		GsHashNextSize( size_t InitialSize );



CC_END_NAMESPACE

#endif 
