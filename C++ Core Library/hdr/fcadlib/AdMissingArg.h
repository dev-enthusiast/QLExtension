/****************************************************************
**
**	ADMISSINGARG.H	- AdMissingArg class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/AdMissingArg.h,v 1.5 2001/11/27 21:41:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADMISSINGARG_H )
#define IN_FCADLIB_ADMISSINGARG_H

#include <fcadlib/base.h>
#include <vector>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )


class EXPORT_CLASS_FCADLIB AdMissingArg
{
	CC_STL_VECTOR(bool)
			m_Missing;

public:
	
	AdMissingArg( size_t NumberOfArgs = 0);
	~AdMissingArg();

	inline bool operator[]( size_t Index ) const { return m_Missing[ Index ]; }

	void setMissing( size_t Index, const char* Name, bool Missing, bool MissingAllowed );
};

CC_END_NAMESPACE

#endif 
