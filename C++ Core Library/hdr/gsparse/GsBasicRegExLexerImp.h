/****************************************************************
**
**	GSBASICREGEXLEXERIMP.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsparse/src/gsparse/GsBasicRegExLexerImp.h,v 1.2 1999/12/07 01:44:33 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSPARSE_GSBASICREGEXLEXERIMP_H )
#define IN_GSPARSE_GSBASICREGEXLEXERIMP_H

#include <gsparse/base.h>
#include <gsparse/GsLexerImp.h>
#include <gscore/GsString.h>
#include <utility>
#include <map>
#include <vector>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

class GsRegEx;
class EXPORT_CLASS_GSPARSE GsBasicRegExLexerImp : public GsLexerImp 
{
public:
	GsBasicRegExLexerImp();
	~GsBasicRegExLexerImp();
	
	/*
	** From GsLexerImp.h
	*/
	virtual int nextToken();
	virtual int peekToken();
	virtual void unPeek();

	virtual GsString tokenText( int Token ) const;

	virtual GsLexerImp* copy() const { return new GsBasicRegExLexerImp( *this ); }

	void addToken( int Token, const GsString& TokenStr, const GsString& RegExpression );
	virtual void setInput( const GsString& Input );

private:

	GsBasicRegExLexerImp( const GsBasicRegExLexerImp& Other );
	GsBasicRegExLexerImp& operator=( const GsBasicRegExLexerImp& Rhs );
	
	void clear();
	
	int longestMatch( const GsString& String, int& SizeOfMatch );

	typedef CC_NS(std,pair)< int, GsRegEx* > int_regex_pair;
	CC_STL_VECTOR( int_regex_pair )
			m_RegExList;

	// FIX should be GsString but breaks MSDEV cannot operator= on pair<int, GsString>
 	typedef CC_STL_MAP( int, char* ) map_type;
 	map_type
 			m_TokenMap;

	typedef CC_NS(std,pair)< int, GsString > int_string_pair;
	CC_STL_VECTOR( int_string_pair )
			m_PeekList;

	CC_STL_VECTOR( int_string_pair )::iterator
			m_NextIter,
			m_PeekIter;
};

CC_END_NAMESPACE

#endif

