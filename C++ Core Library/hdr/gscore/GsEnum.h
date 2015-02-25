/****************************************************************
**
**	gscore/GsEnum.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsEnum.h,v 1.8 2011/05/16 20:06:08 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSENUM_H )
#define IN_GSCORE_GSENUM_H

#include	<gscore/base.h>
#include	<gscore/GsString.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Functions
*/

//EXPORT_CPP_GSBASE void GsEnumRegisterAlias( const char *EnumName, const char *RootEnum );

EXPORT_CPP_GSBASE void GsEnumRegister( const char *EnumName, const char *EnumDesc, ... );
#define GS_ENUM_MEMBER(V,D)	#V, (int)V, D
#define GS_ENUM_ALIAS(V,A)	 A, (int)V, NULL
#define GS_ENUM_LAST		 NULL,  -1, NULL


class GsEnumInfo;
EXPORT_CPP_GSBASE const GsEnumInfo *GsEnumInfoLookup( const char *EnumName );

EXPORT_CPP_GSBASE int GsEnumLookupByName( const char *EnumName, const char *ElemName );
EXPORT_CPP_GSBASE int GsEnumLookup( const char *EnumName, const char *AnyName );

EXPORT_CPP_GSBASE const char *GsEnumToName( const char *EnumName, int EnumValue );
EXPORT_CPP_GSBASE const char *GsEnumToText( const char *EnumName, int EnumValue );

enum GsEnumHelpFlag
{
    GS_ENUM_HELP_ALIAS = 0x0001,	// Display the friendly strings
    GS_ENUM_HELP_NAME  = 0x0002,	// Display the constant names
    GS_ENUM_HELP_VALUE = 0x0004,	// Display the enum value
	GS_ENUM_HELP_ALL   = 0x0007		// _ALIAS | _NAME
};

EXPORT_CPP_GSBASE GsString GsEnumHelpText( const char *EnumName, int HelpFlags = GS_ENUM_HELP_ALL );


/*
**	Templated attributes
**
**	Methods:
**		const char *enumName();	- Name of enum (C++ type name: "Q_OPT_TYPE" )
**		E lookupByName( valName ) - Lookup by name of enum (C++ enum element name: "Q_OPT_PUT" )
**		E lookup( anyName ) - Lookup by name or random short text ("put" or "Put" or "Q_OPT_PUT")
**		const char *toName( E val ) - Convert enum to name ("Q_OPT_PUT")
**		const char *toText( E val ) - Convert to prefered text ("Put")
**
*/

template <class E> class GsEnumAttributes;

#define GS_ENUM_DECLARE(E)	\
	template<> class GsEnumAttributes<E>	\
	{							\
		public:					\
			static const char *enumName() { return #E; }	\
			static E lookupByName( const char *n )	{ return (E) GsEnumLookupByName( enumName(), n ); }	\
			static E lookup( const char *n )		{ return (E) GsEnumLookup( enumName(), n ); }		\
			static const char *toName( E v )		{ return GsEnumToName( enumName(), (int) v ); }		\
			static const char *toText( E v )		{ return GsEnumToText( enumName(), (int) v ); }		\
	}


CC_END_NAMESPACE
#endif

