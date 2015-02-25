/**************************************************************** -*-c++-*-
**
**	secdb/slang_util.h - Utility functions for slang trees
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/slang_util.h,v 1.10 2012/10/12 13:21:35 e19351 Exp $
**
****************************************************************/

#ifndef IN_SLANG_UTIL_H
#define IN_SLANG_UTIL_H

#include <secdb/base.h>
#include <secexpr.h>

#include <clex.h>

#include <iosfwd>

// forward declarations
struct SLANG_NODE;

// moved here from sdb_expr.h
EXPORT_CPP_SECDB SLANG_FUNC SlangNodeTypeLookup( SLANG_NODE_TYPE NodeType, int* ParseFlags );

// moved here from sdb_expr.c
EXPORT_CPP_SECDB SLANG_NODE_TYPE TokenToOperator( bool Unary, int PrevToken, TOKEN Token, bool& LeftAssociative, int& Precedence );

// moved here from secexpr.h
EXPORT_C_SECDB void SlangParseTreeToString     ( SLANG_NODE const* Root, int IndentLevel, std::ostream& stream );
EXPORT_C_SECDB void SlangParseTreeToBriefString( SLANG_NODE const* Root, int IndentLevel, std::ostream& stream );

template <class F_>
class Slang_Parse_Walker // depth-first walk of the slang parse tree
{
public:
    explicit Slang_Parse_Walker( SLANG_NODE* root ) : m_root( root ) {}
    void operator()( F_& func ) const
    {
	    SLANG_NODE** const begin = m_root->Argv[0];
		SLANG_NODE** const end = m_root->Argv[ m_root->Argc ];

		func( *m_root );

		for( SLANG_NODE** iter = begin; iter != end; ++iter )
		{
		    Slang_Parse_Walker<F_> child_walker( iter );
			child_walker( func );
		}
	}
private:
    SLANG_NODE* const m_root;
};

template <class F_>
class Slang_Parse_Searcher // depth-first search of the slang parse tree
{
public:
    explicit Slang_Parse_Searcher( SLANG_NODE const* root ) : m_root( root ) {}
    SLANG_NODE const* operator()( F_ const& func ) const
    {
	    SLANG_NODE const* const* const begin = m_root->Argv;
		SLANG_NODE const* const* const end = m_root->Argv + m_root->Argc;
		for( SLANG_NODE const* const* iter = begin; iter != end; ++iter )
		{
		    Slang_Parse_Searcher<F_> child_searcher( *iter );
			SLANG_NODE const* const res = child_searcher( func );
		    if( res )
			    return res;
		}

		return func( *m_root ) ? m_root : 0;
	}
private:
    SLANG_NODE const* const m_root;
};

template <class F_>
inline SLANG_NODE const* SlangParseSearch( SLANG_NODE const* root, F_ const& func )
{
    return Slang_Parse_Searcher<F_>(root)( func );
}

template <class F_>
inline void SlangParseWalk( SLANG_NODE* root, F_& func )
{
    return Slang_Parse_Walker<F_>(root)( func );
}


#endif
