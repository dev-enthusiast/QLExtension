/****************************************************************
**
**	GSLEXER.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsparse/src/gsparse/GsLexer.h,v 1.2 2000/07/20 13:33:36 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSPARSE_GSLEXER_H )
#define IN_GSPARSE_GSLEXER_H

#include <gsparse/base.h>
#include <gscore/GsString.h>

CC_BEGIN_NAMESPACE( Gs )

class GsLexerImp;
class EXPORT_CLASS_GSPARSE GsLexer
{
	GsLexer();

	GsLexerImp* 
			m_Imp;

public:

	GsLexer( GsLexerImp* Imp ); // This pointer now belongs to GsLexer
	GsLexer( const GsLexer& Other );
	
	~GsLexer();

	GsLexer& operator=( const GsLexer& Rhs );

	int nextToken();
	int peekToken();
	void unPeek();

	GsString text() const;
	GsString tokenText( int ) const;

	int textToInt() const;

	void setInput( const GsString& Input );

	enum GsLexerToken {
		EOT 	= 0,
		SPACE 	= 1,
		ERR 	= 2
	};

	static GsString CaseInsensitive( const GsString &String );
};

CC_END_NAMESPACE

#endif 
