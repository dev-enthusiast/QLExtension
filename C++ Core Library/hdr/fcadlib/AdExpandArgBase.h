/****************************************************************
**
**	fcadlib/AdExpandArgBase.h	- AdExpandArgBase class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/AdExpandArgBase.h,v 1.3 2001/11/27 21:41:45 procmon Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADEXPANDARGBASE_H )
#define IN_FCADLIB_ADEXPANDARGBASE_H

#include 	<fcadlib/base.h>

CC_BEGIN_NAMESPACE( Gs )

enum AD_EXPAND_MODE 
{
	AD_EXPAND_SCALAR		= 0,
	AD_EXPAND_VECTOR 		= 1,
	AD_EXPAND_TIMESERIES 	= 2
};

class EXPORT_CLASS_FCADLIB AdExpandArgBase
{
public:
	AdExpandArgBase();
	virtual ~AdExpandArgBase();
	virtual AD_EXPAND_MODE mode() const = 0;
	virtual size_t size() const = 0;
};

CC_END_NAMESPACE

#endif 
