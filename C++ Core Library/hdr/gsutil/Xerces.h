/****************************************************************
**
**	XERCES.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsutil/src/gsutil/Xerces.h,v 1.4 2001/06/15 15:49:01 walkerpa Exp $
**
****************************************************************/

#ifndef IN_GSUTIL_XERCES_H
#define IN_GSUTIL_XERCES_H

#include <gsutil/base.h>

#if defined(XML_IMPL_XERCES)
// workaround for XERCES
#ifdef linux

#if defined(toupper)
#undef toupper
#endif

#if defined(tolower)
#undef tolower
#endif

#endif

#include <util/PlatformUtils.hpp>
#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/EntityResolver.hpp>


#include <parsers/DOMParser.hpp>

#include <framework/MemBufInputSource.hpp>
#include <framework/XMLErrorReporter.hpp>

#include <validators/DTD/DTDValidator.hpp>

#include <dom/DOM.hpp>

#include <internal/ReaderMgr.hpp>

#if defined(XSLT_IMPL_XALAN)
#include <XalanTransformer/XalanTransformer.hpp>
#endif
#endif
#endif 
