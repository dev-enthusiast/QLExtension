/****************************************************************
**
**	GSXMLDOMWRAPPER.H	- A wrapper for an arbitrary DOM
**  element which we use for GsDtGeneric specializations
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsutil/src/gsutil/GsXmlDOMWrapper.h,v 1.12 2001/07/18 19:46:00 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSUTIL_GSXMLDOMWRAPPER_H
#define IN_GSUTIL_GSXMLDOMWRAPPER_H

#include <gsutil/base.h>
#include <gsutil/GsXmlTree.h>

#include <gsdt/GsDt.h>
#include <gsdt/GsDtGeneric.h>
#include <gsdt/GsDtToDtValue.h>
#include <datatype.h>

#include <extension/GsRefCountedPtr.h>

CC_BEGIN_NAMESPACE( Gs )

#if defined(XML_IMPL_XERCES)
/****************************************************************
** Class	   : GsXmlUtils 
** Description : A fully static class (non-constructable)
**               which contains various silly utils I need
**               like node value extraction and string trminnig
****************************************************************/
template<class _TDom>
class GsXmlDOMWrapper 
{
public:
	typedef GsDtGeneric< GsXmlDOMWrapper<_TDom> > GsDtType;

	GsXmlDOMWrapper( GsXmlTree *tree ); // take ownership of the tree
	GsXmlDOMWrapper(_TDom *dom, CC_NS(extension,GsRefCountedPtr)<GsXmlTree, int> const& tree);		// take ownership of the dom
	
	GsXmlDOMWrapper( const GsXmlDOMWrapper<_TDom> &c );

	const GsXmlDOMWrapper<_TDom>& operator=( const GsXmlDOMWrapper<_TDom> &c );
	
	virtual ~GsXmlDOMWrapper()
	{
		//CC_NS( std, cout ) << "Destroying a DOM Wrapper" << CC_NS( std, endl );
	}
	
	GsString toString();
	
	static const char* typeName();
	
	friend class GsDtGeneric< GsXmlDOMWrapper<_TDom> >;
	
	CC_NS(extension,GsRefCountedPtr)<_TDom, int> m_dom; 
	CC_NS(extension,GsRefCountedPtr)<GsXmlTree, int> m_tree; 

	DOM_Node getCreatorNode( );
	void setCreatorNode( DOM_Node &n ) { m_creatorNode = n; }

protected:
	DOM_Node m_creatorNode;

	virtual void initialize();
	
};

// typedefs
typedef GsXmlDOMWrapper<DOM_Node>          GsXmlTreeNode;
typedef GsXmlDOMWrapper<DOM_NodeList>      GsXmlTreeNodeList;
typedef GsXmlDOMWrapper<DOM_NamedNodeMap>  GsXmlTreeNamedNodeMap;

typedef GsDtGeneric<GsXmlTreeNode>         GsDtXmlTreeNode;
typedef GsDtGeneric<GsXmlTreeNodeList>     GsDtXmlTreeNodeList;
typedef GsDtGeneric<GsXmlTreeNamedNodeMap> GsDtXmlTreeNamedNodeMap;

#else

// No DOM support. Still need this symbol.
typedef GsString GsXmlTreeNode;
#endif

CC_END_NAMESPACE

#endif 
