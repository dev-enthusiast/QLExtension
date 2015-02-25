/****************************************************************
**
**	GSLEXERIMP.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsparse/src/gsparse/GsLexerImp.h,v 1.1 1999/10/29 14:51:01 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSPARSE_GSLEXERIMP_H )
#define IN_GSPARSE_GSLEXERIMP_H

#include <gsparse/base.h>
#include <gscore/GsString.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSPARSE GsLexerImp
{
protected:

	GsLexerImp();

public:

	virtual ~GsLexerImp();

	virtual int nextToken() = 0;
	virtual int peekToken() = 0;
	virtual void unPeek() = 0;

	virtual GsString text() const { return m_Text; }
	virtual GsString tokenText( int Token ) const = 0;
	virtual void setInput( const GsString& Input );
	
	virtual GsLexerImp* copy() const = 0;

protected:

	GsString
			m_Buffer,
			m_Text;

	GsString::iterator
			m_Next;

};

CC_END_NAMESPACE

#endif 
