/****************************************************************
**
**	GSSYMDATE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsSymDate.h,v 1.7 2001/11/27 22:43:12 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSSYMDATE_H )
#define IN_GSDATE_GSSYMDATE_H

#include <gscore/base.h>
#include <gscore/GsDate.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsSymDate
{
public:
	typedef GsDtSymDate GsDtType;

	enum SYMBOL
	{
		GSD_DATE 	= 0,
		GSD_CLOSE 	= 1,
		GSD_RT		= 2
	};

private:

	SYMBOL	m_Symbol;
	GsDate	m_DateEquiv;


	void setFrom( const GsString& Str );
	void setFrom( const GsDate& Date );

public:

	GsSymDate(); // Default SYMBOL is DATE
	GsSymDate( const GsSymDate& Rhs );
	GsSymDate( const GsString& Str );
	GsSymDate( const GsDate& Date );
	
	GsSymDate& operator=( const GsSymDate& Rhs );
	GsSymDate& operator=( const GsString& Rhs );
	GsSymDate& operator=( const GsDate& Rhs );

	bool operator==( const GsSymDate& Rhs ) const;
	
	inline SYMBOL getSymbol() const { return m_Symbol; }

	static GsDate date(const GsSymDate& SymDate,  const GsString& Location = "" );
	static GsDate rtDate( const GsString& Location );
	static GsDate closeDate( const GsString& Location );

	static const char* typeName() { return "GsSymDate"; }
	GsString toString() const;

	friend bool operator < ( const GsSymDate& d1, const GsSymDate& d2 ) { return &d1 < &d2; }

};


EXPORT_CPP_GSDATE CC_OSTREAM& operator << ( CC_OSTREAM& s, const GsSymDate& Date );

CC_END_NAMESPACE

#endif 

