/****************************************************************
**
**	gsdb/GsDb.h	- Convenient database access
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdb/src/gsdb/GsDb.h,v 1.7 2001/11/27 22:43:33 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDB_GSDB_H )
#define IN_GSDB_GSDB_H

#include	<gsdb/base.h>
#include	<gscore/GsString.h>
#include	<secdb.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	GsDb - simple attachment to a SecDb database
*/

class EXPORT_CLASS_GSDB GsDb
{
public:
	GsDb( GsString DbName, bool AttachNow = false );
	~GsDb();

	SDB_OBJECT*	getByName( GsString Name ) { return SecDbGetByName( Name.c_str(), getDb(), 0 ); }

	GsStatus	attach();
	bool		isAttached() { return m_db != NULL; }
	SDB_DB		*getDb()
	{
		if( m_db )
			return m_db;
		else
			return doAttach();
	}
	const GsString&	getDbName() const { return m_dbName; }

private:
	GsDb(); 
	GsDb operator=( const GsDb &rhs );

	GsString	m_dbName;
	SDB_DB		*m_db;

	SDB_DB		*doAttach();	// Attach or throw exception
};


/*
**	A SecPtr class that does SecDbFree when the object leaves context, and
**	calls SecDbFree on the old value if assigned to.
**
**	Note: AutoSecPtr is not EXPORT_CLASS_GSDB because it is all inline.
*/

class AutoSecPtr
{
private:
	SDB_OBJECT*	m_secPtr;
public:
	explicit AutoSecPtr( SDB_OBJECT* Ptr )
	:	m_secPtr( Ptr )
	{
	}
	AutoSecPtr( AutoSecPtr& Other )
	:	m_secPtr( Other.steal() )
	{
	}
	AutoSecPtr()
	{
		SecDbFree( m_secPtr );
	}
	AutoSecPtr& operator=( SDB_OBJECT *Ptr )
	{
		SecDbFree( m_secPtr );
		m_secPtr = Ptr;
		return *this;
	}
	AutoSecPtr& operator=( AutoSecPtr& Other )
	{
		SecDbFree( m_secPtr );
		m_secPtr = Other.steal();
		return *this;
	}
	SDB_OBJECT* get() const
	{
		return m_secPtr;
	}
	SDB_OBJECT* steal()
	{
		SDB_OBJECT* ptr = m_secPtr;
		m_secPtr = NULL;
		return ptr;
	}
};

CC_END_NAMESPACE
#endif

