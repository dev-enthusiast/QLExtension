/****************************************************************
**
**	GSXMLUTILS.H	- A class with various static methods
**  for utilities I execute often.
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsutil/src/gsutil/GsXmlUtils.h,v 1.4 2001/06/15 22:28:56 walkerpa Exp $
**
****************************************************************/

#ifndef IN_GSUTIL_GSXMLUTILS_H
#define IN_GSUTIL_GSXMLUTILS_H

#include <gsutil/base.h>

#include <string>

#if defined(XML_IMPL_XERCES)
#include <gsutil/Xerces.h>
#endif

CC_BEGIN_NAMESPACE( Gs )

#if defined(XML_IMPL_XERCES)
// A pair of little memory manager classes for xml and dom strings
class dsMgr {
public:
	dsMgr(const DOMString &ds) {
		m_local = ds.transcode();
	}
	char *get() { return m_local; }
	~dsMgr() { delete[] m_local; }
private:
	char* m_local;
};

class xsMgr {
public:
	xsMgr(const XMLCh* const toTranscode) {
		m_local = XMLString::transcode(toTranscode);
	}
	char *get() { return m_local; }
	~xsMgr() { delete[] m_local; }
private:
	char* m_local;
};
#endif



CC_END_NAMESPACE

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsXmlUtils 
** Description : A fully static class (non-constructable)
**               which contains various silly utils I need
**               like node value extraction and string trminnig
****************************************************************/

class EXPORT_CLASS_GSXMLUTIL GsXmlUtils {
private:
	GsXmlUtils() {}
public:
	static CC_NS(std, string) trimString(CC_NS(std,string) &instr); 

#if defined(XML_IMPL_XERCES)

	/****************************************************************
	 **	Class  : GsXmlValue 
	 **	Routine: fullNodeValue 
	 **	Returns: All the text and cdata sub-nodes
	 **	Action : 
	 ****************************************************************/
	static CC_NS(std, string) fullNodeValue(DOM_Node &node,
											bool trim=true); 
#endif

};


CC_END_NAMESPACE

#endif 
