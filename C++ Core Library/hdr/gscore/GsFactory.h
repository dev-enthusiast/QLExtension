/****************************************************************
**
**	GSFACTORY.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsFactory.h,v 1.4 2001/11/27 22:41:03 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSBASE_GSFACTORY_H )
#define IN_GSBASE_GSFACTORY_H

#include <gscore/base.h>
#include <ccstl.h>
#include <map>
#include <gscore/GsString.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsFactory 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSBASE GsFactory
{
	// Private 
	GsFactory();
	GsFactory( const GsFactory & ) {};

	GsFactory &operator=( const GsFactory &rhs ) { return *this; }

	static GsFactory s_AllFactory;
	
	typedef CC_STL_MAP( GsString, void* ) map_s_type;
	typedef CC_STL_MAP( int, void* ) map_i_type;

	map_s_type m_SMap;
	map_i_type m_IMap;

public:

	~GsFactory() {};

	typedef void* value_type;

	void insert( const GsString& Key, value_type );
	value_type find( const GsString& ) const;

	void insert( int Key, value_type );
	value_type find( int ) const;

	static GsFactory* findFactory( const GsString& FactoryName );
	static GsFactory* findOrCreateFactory( const GsString& FactoryName );
	static void createFactory( const GsString& FactoryName );

	// Stifle GCC, which complains that this class has only a private
	// constructor and no friends.
	friend void ShutGccUp();
};



CC_END_NAMESPACE

#endif 
