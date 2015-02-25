/****************************************************************
**
**	GSREGEX.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsparse/src/gsparse/GsRegEx.h,v 1.1 1999/10/29 14:51:02 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSPARSE_GSREGEX_H )
#define IN_GSPARSE_GSREGEX_H

#include <gsparse/base.h>
#include <gscore/types.h>
#include <gscore/GsString.h>
#include <regex.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSPARSE GsRegEx
{
	GsString
			m_ExpressionString;

	regex_t	m_Expression;

	GsRegEx();

	void init( const GsString& );
	void clear();
public:

	GsRegEx( const GsString& Expression );
	GsRegEx( const GsRegEx& Other );
	~GsRegEx();

	GsRegEx& operator=( const GsRegEx& Rhs );

	bool match( const GsString& Pattern ) const;
	bool exactMatch( const GsString& Pattern ) const;
	int sizeOfMatch( const GsString& Pattern ) const;
};

CC_END_NAMESPACE

#endif 
