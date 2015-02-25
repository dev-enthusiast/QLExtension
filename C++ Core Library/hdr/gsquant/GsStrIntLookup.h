/****************************************************************
**
**	gsquant/GsStrIntLookup.h - 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsStrIntLookup.h,v 1.5 2001/11/27 22:46:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSSTRINTLOOKUP_H )
#define IN_GSQUANT_GSSTRINTLOOKUP_H

#include <gsquant/base.h>
#include <gscore/GsStringVector.h>
#include <gscore/GsIntegerVector.h>
#include <map>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSQUANT GsStrIntLookup
{

protected:

	GsStrIntLookup();

public:

	typedef CC_STL_MAP( GsString, int ) str_map;
	typedef CC_STL_MAP( int, GsString ) int_map;

	typedef str_map::value_type str_value_type;
	typedef int_map::value_type int_value_type;

	GsStrIntLookup( const GsStringVector& StrVec, const GsIntegerVector& IntVec );

	GsStrIntLookup( const GsStrIntLookup& Rhs );

	~GsStrIntLookup() {}

	GsStrIntLookup& operator=( const GsStrIntLookup& Rhs );

	
	const GsString& find( int Key ) const;
	int 			find( const GsString& Key ) const;

	typedef GsDtGeneric<GsStrIntLookup> GsDtType;
	GsString toString() const { return "GsStrIntLookup"; }
	inline static const char * typeName() { return "GsStrIntLookup"; }

private:

	str_map m_StringMap;

	int_map m_IntegerMap;

};


EXPORT_CPP_GSQUANT GsStrIntLookup* GsStrIntLookupNew( const GsStringVector& StrVec, const GsIntegerVector& IntVec );
EXPORT_CPP_GSQUANT GsString GsStrIntLookupFindStr( const GsStrIntLookup& Lookup, int Key );
EXPORT_CPP_GSQUANT int GsStrIntLookupFindInt( const GsStrIntLookup& Lookup, const GsString& Key );

CC_END_NAMESPACE

#endif 

