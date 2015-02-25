/****************************************************************
**
**	DOMOUTPUT.H	- See DOMOutput.cpp comments. This assumes you
**  are in a XERCES C++ block.
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsutil/src/gsutil/DOMOutput.h,v 1.5 2001/08/25 01:03:59 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSUTIL_DOMOUTPUT_H
#define IN_GSUTIL_DOMOUTPUT_H

#include <gsutil/base.h>
#include <dom/DOM_Node.hpp>

#include <iosfwd> // for std::ostream

EXPORT_CPP_GSXMLUTIL 
        void DOMOutputInitialize();

EXPORT_CPP_GSXMLUTIL
		CC_NS( std, ostream ) &operator<<( CC_NS( std, ostream ) &, DOM_Node & );

#endif 
