/****************************************************************
**
**	gscore/GsEnumInfo.h	- Enum information structure
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsEnumInfo.h,v 1.7 2011/05/16 20:06:08 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSENUMINFO_H )
#define IN_GSCORE_GSENUMINFO_H

#include	<gscore/GsEnum.h>
#include	<gscore/GsStringVector.h>
#include	<cstring>
#include	<cstdlib>

// Forward declare so we don't need to include hash.h
typedef struct HashStructure HASH;

CC_BEGIN_NAMESPACE( Gs )


/*
**	Classes declared here
*/

class GsEnumInfo;
class GsEnumInfoElement;
class GsEnumInfoElementAlias;


/*
**	GsEnumInfo - semi-private implementation class.  Only needed by those
**				 functions that want to iterate.
*/

class EXPORT_CLASS_GSBASE GsEnumInfo
{
public:
	const char*	text( int value );

	char*	m_enumName;		// enum type name
	char*	m_enumDesc;		// enum type description
	HASH*	m_nameToElem;	// Hash of Text  -> GsEnumInfoElement
	HASH*	m_valueToElem;	// Hash of Value -> GsEnumInfoElement

	static GsEnumInfo* lookup( const char *enumName );
	static GsStringVector allNames();

	GsEnumInfoElement* valueToElem( int value );
	GsEnumInfoElement* nameToElem( const char *name );

private:
	friend void GsEnumRegister( const char *EnumName, const char *EnumDesc, ... );
	GsEnumInfo( const char *Name, const char *Desc );
	~GsEnumInfo();

	void addAlias( int Value, const char *alias );
	void addElement( int Value, const char *name, const char *desc );

	GsEnumInfo( const GsEnumInfo& );
	void operator=( const GsEnumInfo& );
};


/*
**	GsEnumInfoElementAlias - Linked list of strings
*/

class EXPORT_CLASS_GSBASE GsEnumInfoElementAlias
{
public:
	GsEnumInfoElementAlias( const char *n )
	:	m_next( 0 ),
		m_name( strdup( n ))
	{
	}
	~GsEnumInfoElementAlias()
	{
		free( m_name );
	}

	GsEnumInfoElementAlias*	m_next;
	char*					m_name;
};



/*
**	GsEnumInfoElement - contains info about each element of an enum
*/

class EXPORT_CLASS_GSBASE GsEnumInfoElement
{
public:
	GsEnumInfoElement( const char* name, int val, const char *desc );
	~GsEnumInfoElement();

	GsEnumInfoElementAlias *addAlias( const char *name );

	char*	m_name;			// C++ name
	int		m_value;		// Value of enum element
	char*	m_desc;			// Description

	GsEnumInfoElementAlias*	// List of aliases
			m_aliases;
};

CC_END_NAMESPACE
#endif

