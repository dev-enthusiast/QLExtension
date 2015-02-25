/****************************************************************
**
**	DICTIONARYUTIL.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/DictionaryUtil.h,v 1.8 2000/02/02 18:49:45 kongl Exp $
**
****************************************************************/

#if !defined( IN_FIQUANT_DICTIONARYUTIL_H )
#define IN_FIQUANT_DICTIONARYUTIL_H

#include <gsquant/base.h>
#include <gsdt/GsDtDictionary.h>
#include <gsdt/GsDtArray.h>
#include <gscore/GsStringVector.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsDtDictionary *GsDictionaryFromTagsAndValues( const GsStringVector &Tags, const GsDtArray &Values );

EXPORT_CPP_GSQUANT GsDtDictionary *GsDictionaryCreate( const GsDtArray &TagsAndValues );

EXPORT_CPP_GSQUANT GsStringVector *GsDictionaryTags( const GsDtDictionary &Dict );

EXPORT_CPP_GSQUANT GsDtArray 	  *GsTableInit( const GsDtArray &TagsAndValues );

EXPORT_CPP_GSQUANT double	   GsDictionaryElemDouble( const GsDtDictionary &Dict, const GsString &Tag );
EXPORT_CPP_GSQUANT GsString	   GsDictionaryElemString( const GsDtDictionary &Dict, const GsString &Tag );
EXPORT_CPP_GSQUANT GsVector	  		*GsDictionaryElemVector( const GsDtDictionary &Dict, const GsString &Tag );
EXPORT_CPP_GSQUANT GsMatrix	  		*GsDictionaryElemMatrix( const GsDtDictionary &Dict, const GsString &Tag );
EXPORT_CPP_GSQUANT GsStringVector	*GsDictionaryElemStringVector( const GsDtDictionary &Dict, const GsString &Tag );

EXPORT_CPP_GSQUANT GsDtDictionary *GsDictionaryUnion( const GsDtDictionary &Dict0, const GsDtDictionary &Dict1 );

EXPORT_CPP_GSQUANT GsDtDictionary *GsDictionaryReplaceComponents( const GsDtDictionary &Dict0, const GsDtDictionary &Dict1 );

CC_END_NAMESPACE

#endif 
