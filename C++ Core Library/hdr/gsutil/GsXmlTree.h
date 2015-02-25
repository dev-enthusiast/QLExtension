/****************************************************************
**
**	GSXMLTREE.H	- A simple implementation of an XML tree which
**  is accesible by slang. The basics are that the tree acts
**  like a vector of children (containment), a vector of
**  string/value pairs (attribution), a named element (tag name)
**  and a value-bearing element (has a value). Warning - it is
**  currently not thread safe.
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsutil/src/gsutil/GsXmlTree.h,v 1.24 2001/07/18 19:46:02 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSUTIL_GSXMLTREE_H
#define IN_GSUTIL_GSXMLTREE_H

#include <gsutil/base.h>

#include <memory>

#include <gsdt/GsDtGeneric.h>

#include <gsquant/Enums.h>

#if defined(XML_IMPL_XERCES)
#include <gsutil/Xerces.h>
#endif

#include <sstream> // for std::ostringstream
#include <ostream> // for std::endl
#include <string>  // for std::string

#include <gsutil/GsXmlUtils.h>

CC_BEGIN_NAMESPACE( Gs )


#if defined(XML_IMPL_XERCES)

// A little forward declaration
template<class _TDom> class GsXmlDOMWrapper;


#endif

/****************************************************************
** Class	   : GsXmlTree 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSXMLDT GsXmlTree
{
public:
	typedef GsDtGeneric<GsXmlTree> GsDtType;
	
	GsXmlTree();
	GsXmlTree(const GsXmlTree &x) { }
	GsXmlTree &operator=( const GsXmlTree &rhs );

	static bool initializeXml( );

	static void shutdownXml();

	virtual ~GsXmlTree() { 
		//CC_NS( std, cout ) << "Destroying GsXmlTree" << CC_NS( std, endl );
#if defined(XML_IMPL_XERCES)
		// delete m_parser; -> not now it is static!
#endif
	}

	virtual void setWhitespaceTrim(const bool trim) {
		m_trim = trim;
	}

	virtual bool parse(const CC_NS(std, string)& xml);

	static const char* typeName() { return "GsXmlTree"; }
	GsString toString() { return GsString( "GsXmlTree for now" ); }



	// NB: since the parser is persistent it is impmortant to always set
	// a value in any case
	inline void applyFlags(int xmlFlags) 
	{
		// Factor...
		if ( ( xmlFlags & GSXML_STRIP_FIELD_WHITESPACE ) != 0 )
		{
			setWhitespaceTrim( true );
		}
		else
		{
			setWhitespaceTrim( false );
		}
#if defined(XML_IMPL_XERCES)
		if ( ( xmlFlags & GSXML_NONVALIDATING ) != 0 )
		{
			m_parser->setValidationScheme(DOMParser::Val_Never);
		}
		else if ( ( xmlFlags & GSXML_VALIDATING ) != 0 )
		{
			m_parser->setValidationScheme(DOMParser::Val_Always);
		}
		else if ( ( xmlFlags & GSXML_VALIDATING ) != 0 &&
				  ( xmlFlags & GSXML_NONVALIDATING ) != 0 )
		{
			GSX_THROW(GsXUnsupported,
					  "While, semantically, by specifying both validating and non-validating simultaneously you could have meant automatic validation detection (the default setting, BTW) we have chosen to make the combo throw an error, as it just looks wierd.");
		}
		else
		{
			m_parser->setValidationScheme(DOMParser::Val_Auto);
		}
#endif
	}


	inline GsDtDictionary *parseIntoDictionary() {
#if defined(XML_IMPL_XERCES)
		DOM_Document doc = getRootDocument();
		DOM_Element  el  = doc.getDocumentElement();
		return parseNodeIntoDictionary((DOM_Node &)el);
#else
		return NULL;
#endif
	}

public:
	void replaceSecDBDTDURIWithActualDTDBody( CC_NS ( std, string ) &xmlIn);	// grin
protected:
	static void registerConverters();	// Implemented in templates.cpp

	CC_NS( std, string ) getDTDFromSecDbURI( CC_NS( std, string ) uri );

#if defined(XML_IMPL_XERCES)
public:
	inline DOM_Document getRootDocument() const {
		return m_doc;
	}

	inline void setRootDocument( DOM_Document d )
	{
		m_doc = d;
	}
private:
	GsDtDictionary *parseNodeIntoDictionary(DOM_Node &node);
#endif

private:

	static bool m_xmlInitialized;
	bool m_trim;
#if defined(XML_IMPL_XERCES)
	DOMParser::ValSchemes m_valScheme;
	CC_NS( std, auto_ptr )<DOMParser> m_parser;

	friend class GsXmlDOMWrapper<DOM_Node>;

	DOM_Document m_doc;
#endif

#if defined(XML_IMPL_XERCES)
	class GsXmlErrorHandler : public ErrorHandler 
	{

	public:
		GsXmlErrorHandler() { }
		~GsXmlErrorHandler() { }

		
		void warning(const SAXParseException &e) {
			CC_NS(std,ostringstream) error;
			error << "Warning at " << xsMgr(e.getSystemId()).get()
				  << ", line " << e.getLineNumber()
				  << ", char " << e.getColumnNumber()
				  << "\n  Message: " << xsMgr(e.getMessage()).get() << CC_NS(std, endl);
			GSX_THROW( GsXStreamRead, error.str());
		}
		
		void error(const SAXParseException &e) {
			CC_NS(std,ostringstream) error;
			error << "\nError at " << xsMgr(e.getSystemId()).get()
				  << ", line " << e.getLineNumber()
				  << ", char " << e.getColumnNumber()
				  << "\n  Message: " << xsMgr(e.getMessage()).get() << CC_NS(std, endl);
			GSX_THROW( GsXStreamRead, error.str());
		}
		
		void fatalError(const SAXParseException &e) {
			CC_NS(std,ostringstream) error;
			error << "\nFatal error at " << xsMgr(e.getSystemId()).get()
				  << ", line " << e.getLineNumber()
				  << ", char " << e.getColumnNumber()
				  << "\n  Message: " << xsMgr(e.getMessage()).get() << CC_NS(std, endl);
			GSX_THROW( GsXStreamRead, error.str());
		}
		
		void resetErrors() {
			GSX_THROW( GsXNotImplemented, "Reset error not implemented");
		}
		
	};


	GsXmlErrorHandler m_gxeh;

	EntityResolver *m_entityResolver;
#endif

};

CC_END_NAMESPACE

#endif 
