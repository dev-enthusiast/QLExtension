/****************************************************************
**
**	GsSymbol.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsSymbol.h,v 1.25 2001/11/27 22:41:08 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSSYMBOL_H )
#define IN_GSSYMBOL_H

#include <gscore/base.h>
#include <gscore/GsString.h>
#include <list>
#include <utility>
#include <ccstl.h>


CC_BEGIN_NAMESPACE(Gs)

class EXPORT_CLASS_GSBASE GsSymbol
{
public:
	typedef unsigned long Hash;
	typedef CC_NS(std,pair)< GsString, int > symbol_pair;
	typedef CC_STL_LIST(symbol_pair) SymbolList;

	typedef GsString::size_type					size_type;
	typedef GsString::difference_type			difference_type;
	typedef GsString::const_pointer				const_pointer;
	typedef GsString::const_reference			const_reference;
	typedef GsString::value_type				value_type;
	typedef GsString::const_iterator			const_iterator;
	typedef GsString::const_reverse_iterator	const_reverse_iterator;
	typedef GsString::traits_type				traits_type;

	explicit GsSymbol() CC_NOTHROW : m_rep( nullsym() ) {}
	explicit GsSymbol( const GsString& s ) : m_rep( 0 ) { attach( s ); }
	GsSymbol( const GsSymbol& s ) : m_rep( s.m_rep ) { incrRef(); }

	~GsSymbol() { decrRef(); }
  
	operator GsString() const { return m_rep->m_str; }

	GsSymbol& operator = ( const GsSymbol& x ) { return assign( x ); }
	GsSymbol& operator = ( const GsString& s ) { return assign( s ); }

	GsSymbol& assign( const GsSymbol& x )
	{
		if( m_rep != x.m_rep )
		{
			decrRef();
			m_rep = x.m_rep;
			incrRef();
	   }
	   return *this;
	}

	GsSymbol& assign( const GsString& s )
	{
		decrRef();
		attach( s );
		return *this;
	}

	const_iterator begin() const CC_NOTHROW	{ return m_rep->m_str.begin(); }
	const_iterator end() const CC_NOTHROW	{ return m_rep->m_str.end(); }
	const_reverse_iterator rbegin() const CC_NOTHROW
	{
		// We need the const version of the rbegin() function here.
		return ((const GsString&)m_rep->m_str).rbegin(); 
	}
	const_reverse_iterator rend() const CC_NOTHROW
	{ 
		// We need the const version of the rend() function here.
		return ((const GsString&)m_rep->m_str).rend(); 
	}

	const_reference at( size_type i ) const	{ return m_rep->m_str.at( i ); }
	const_reference operator[]( size_type i ) const CC_NOTHROW { return m_rep->m_str[i]; }
	const char	*c_str()		const CC_NOTHROW { return m_rep->m_str.c_str(); }
	const char	*data()			const CC_NOTHROW { return m_rep->m_str.data(); }
	size_type	length()		const CC_NOTHROW { return m_rep->m_str.length(); }
	size_type	size()			const CC_NOTHROW { return m_rep->m_str.size(); }
	bool		empty()			const CC_NOTHROW { return m_rep == nullsym(); }

	void swap( GsSymbol& x ) CC_NOTHROW
	{
		GsSymbol::Rep* tmp_rep = m_rep;
		m_rep = x.m_rep;
		x.m_rep = tmp_rep;
	}
	friend void swap( GsSymbol& x, GsSymbol& y ) CC_NOTHROW { x.swap( y ); }
  
	int compare( const GsSymbol& x ) const CC_NOTHROW 	{ return compare( x.c_str() ); }
	int compare( const GsString& s ) const CC_NOTHROW 	{ return compare( s.c_str() ); }
	int compare( const char *c ) const CC_NOTHROW		{ return stricmp( c_str(), c ); }

	bool is_equal( const GsSymbol& x ) const CC_NOTHROW { return m_rep == x.m_rep; }

	Hash 	hash() const CC_NOTHROW { return m_rep->m_hash; }
	int		getReferenceCount() const CC_NOTHROW { return m_rep->m_count; }

	static	int			eraseUnreferencedSymbols();
	static	SymbolList*	getAllSymbols();

	typedef class GsDtSymbol GsDtType;


private:
	struct Rep
	{
		Rep() : m_count( 0 ) {}
	
		GsString  m_str;
		Hash	  m_hash;
		int       m_count;
	};
	void decrRef()
	{
		--m_rep->m_count;
	}

	void incrRef()
	{
		m_rep->m_count++;
	}
  
	void attach( const GsString& s )
	{
		m_rep = lookup( s );
		incrRef();
	}

	static Rep* lookup( const GsString& s );

	static Rep* nullsym();	

	Rep* m_rep;
};

inline bool  operator == ( const GsSymbol& lop, const GsSymbol& rop )	{ return lop.is_equal( rop ); }
inline bool  operator == ( const GsSymbol& lop, const char* rop )		{ return lop.compare( rop ) == 0; }
inline bool  operator == ( const char* lop, const GsSymbol& rop )		{ return rop.compare( lop ) == 0; }
inline bool  operator != ( const GsSymbol& lop, const GsSymbol& rop )	{ return !lop.is_equal( rop ); }
inline bool  operator != ( const GsSymbol& lop, const char* rop )		{ return lop.compare( rop ) != 0; }
inline bool  operator != ( const char* lop, const GsSymbol& rop )		{ return rop.compare( lop ) != 0; }
inline bool  operator < ( const GsSymbol& lop, const GsSymbol& rop )	{ return lop.compare( rop ) < 0; }
inline bool  operator < ( const GsSymbol& lop, const char* rop )		{ return lop.compare( rop ) < 0; }
inline bool  operator < ( const char* lop, const GsSymbol& rop )		{ return rop.compare( lop ) < 0; }
inline bool  operator <= ( const GsSymbol& lop, const GsSymbol& rop )	{ return lop.compare( rop ) <= 0; }
inline bool  operator <= ( const GsSymbol& lop, const char* rop )		{ return lop.compare( rop ) <= 0; }
inline bool  operator <= ( const char* lop, const GsSymbol& rop )		{ return rop.compare( lop ) <= 0; }
inline bool  operator > ( const GsSymbol& lop, const GsSymbol& rop )	{ return lop.compare( rop ) > 0; }
inline bool  operator > ( const GsSymbol& lop, const char* rop )		{ return lop.compare( rop ) > 0; }
inline bool  operator > ( const char* lop, const GsSymbol& rop )		{ return rop.compare( lop ) > 0; }
inline bool  operator >= ( const GsSymbol& lop, const GsSymbol& rop )	{ return lop.compare( rop ) >= 0; }
inline bool  operator >= ( const GsSymbol& lop, const char* rop )		{ return lop.compare( rop ) >= 0; }
inline bool  operator >= ( const char* lop, const GsSymbol& rop )		{ return rop.compare( lop ) >= 0; }

EXPORT_CPP_GSBASE CC_OSTREAM& operator << ( CC_OSTREAM& o, const GsSymbol& s );

CC_END_NAMESPACE  		  
				  
#endif


