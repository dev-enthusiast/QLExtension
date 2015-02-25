/****************************************************************
**
**	GSXML.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsutil/src/gsutil/gsxml.h,v 1.19 2001/08/29 16:28:50 walkerpa Exp $
**
****************************************************************/

#if !defined( IN_GSUTIL_GSXML_H )
#define IN_GSUTIL_GSXML_H

#include <gscore/GsString.h>
#include <gsdt/GsDtDictionary.h>

#include <gsutil/GsXmlTree.h>
#include <gsutil/GsXmlDOMWrapper.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_EXPORT
		GsDtDictionary *GsXmlParseXmlToStructure(const GsString &xmlBlob,
												 int xmlFlags);

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlParseXml(const GsString &xmlBlob,
									 int xmlFlags);

EXPORT_CPP_EXPORT
		int GsXmlSetupXRefs( const GsXmlTreeNode &node,
							 const GsString      &idtag,
							 const GsString      &reftag );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlResolveXRefs( const GsXmlTreeNode &node,
										  bool resolveChildren );


EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlGetParent( const GsXmlTreeNode &node );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlGetRoot( const GsXmlTreeNode &node );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlGetParentWithName( const GsXmlTreeNode &node,
											   const GsString &name );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlGetParentWithAttribute( const GsXmlTreeNode &node,
													const GsString &name );

EXPORT_CPP_EXPORT
		GsString GsXmlGetPath( const GsXmlTreeNode &node );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlGetChildByPath( const GsXmlTreeNode &node,
											const GsString &path );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlGetNodeByPath( const GsXmlTreeNode &node,
										   const GsString &path );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlGetBestNodeByPath( const GsXmlTreeNode &node,
											   const GsString &path );

EXPORT_CPP_EXPORT
		GsString GsXmlGetXmlRep( const GsXmlTreeNode &node );

EXPORT_CPP_EXPORT
		GsString GsXmlApplyXSLT( const GsString &xml,
								 const GsString &xsl );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlCreateDoc( const GsString &nodeName );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlCreateDocument( const GsString &nodeName,
											const GsString &publicDTD,
											const GsString &systemDTD
											);

EXPORT_CPP_EXPORT
		bool GsXmlInsertNodeAfter( const GsXmlTreeNode &inThisNode,
								   int afterThisChild,
								   const GsXmlTreeNode &insertMe );

EXPORT_CPP_EXPORT
		bool GsXmlReplaceNode( const GsXmlTreeNode &inThisNode,
							   int replaceThisNode,
							   const GsXmlTreeNode &withThis );

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlCreateNode( const GsString &nodeName,
										const GsXmlTreeNode &inThisDoc );

EXPORT_CPP_EXPORT
		bool GsXmlSetXmlDecl( const GsXmlTreeNode &forThisDoc,
							  const GsString &version,
							  const GsString &encoding,
							  const GsString &standalone );
							  

EXPORT_CPP_EXPORT
		GsXmlTreeNode *GsXmlTabbifyDocument( const GsXmlTreeNode &inThisDoc );



CC_END_NAMESPACE

#endif 
