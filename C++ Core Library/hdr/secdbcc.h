/****************************************************************
**
**	SECDBCC.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdbcc.h,v 1.17 2001/11/27 23:23:44 procmon Exp $
**
****************************************************************/

#ifndef IN_SECDB_SECDBCC_H
#define IN_SECDB_SECDBCC_H

#include <sdb_base.h>
#include <secdb.h>
#include <secnode.h>
#include <skiplist.h>
#include <gscore/GsException.h>
#include <secdt.h>

#include <string>

/*
**  Helper NoCopy class
**  Privately inherit from this to prevent instances of a class from being replicated
*/

class NoCopy
{
protected:
  NoCopy() throw() {}
  ~NoCopy() throw() {}

  NoCopy( NoCopy const& ) throw(); // undefined
  NoCopy& operator=( NoCopy const& ) throw(); // undefined
};

/*
**	Smart pointer class wrapping an SDB_OBJECT *
*/

class SdbObjectPtr
{
public:
    SdbObjectPtr() : m_SecPtr( 0 ) {}

	SdbObjectPtr( char const* Name, SDB_DB* Db, unsigned Flags = 0 )
	  : m_SecPtr( SecDbGetByName( Name, Db, Flags ))
	{}
	SdbObjectPtr( char const* Name, SDB_SEC_TYPE Type, SDB_DB* Db )
	  : m_SecPtr( SecDbNew( Name, Type, Db ))
	{}
	explicit SdbObjectPtr( SDB_OBJECT* SecPtr )
	  : m_SecPtr( SecPtr )
	{
		if( m_SecPtr )
			SecDbIncrementReference( m_SecPtr );
	}
	SdbObjectPtr( SdbObjectPtr const& Src )
	  : m_SecPtr( Src.m_SecPtr )
	{
	    if( m_SecPtr )
		    SecDbIncrementReference( m_SecPtr );
	}
	~SdbObjectPtr()
	{
		if( m_SecPtr )
		    SecDbFree( m_SecPtr ); // aka SecDbDecrementReference
	}

	SdbObjectPtr& operator=( SdbObjectPtr const& Src )
	{
	    // do this first to guard against self-assignment
	    if( Src.m_SecPtr )
		    SecDbIncrementReference( Src.m_SecPtr );
		if( m_SecPtr )
		    SecDbFree( m_SecPtr ); // aka SecDbDecrementReference
		m_SecPtr = Src.m_SecPtr;
		return *this;
	}
	SdbObjectPtr& operator=( SDB_OBJECT* sec )
	{
	    // do this first to guard against self-assignment
		if( sec )
			SecDbIncrementReference( sec );
		if( m_SecPtr )
		    SecDbFree( m_SecPtr ); // aka SecDbDecrementReference
		m_SecPtr = sec;
		return *this;
	}

	SDB_OBJECT*       val()       throw() { return m_SecPtr; }
    SDB_OBJECT const* val() const throw() { return m_SecPtr; }

	SDB_OBJECT*       get()       throw() { return m_SecPtr; }
    SDB_OBJECT const* get() const throw() { return m_SecPtr; }

	SDB_OBJECT*       release()   throw() { SDB_OBJECT* tmp = m_SecPtr; m_SecPtr = 0; return tmp; }

	SDB_OBJECT*&      ref()     throw() { return m_SecPtr; }

	SDB_OBJECT&       operator*()       throw() { return *m_SecPtr; }
    SDB_OBJECT const& operator*() const throw() { return *m_SecPtr; }

	SDB_OBJECT*       operator->()       throw() { return m_SecPtr; }
    SDB_OBJECT const* operator->() const throw() { return m_SecPtr; }

    // operator bool() const { return val(); } // uncomment this when all compilers have a true bool type
    bool operator!() const { return val() == 0;}

private:
	SDB_OBJECT
			*m_SecPtr;
};


/*
** Guard class to use a Db as the RootDb and restore it upon exit from the scope
*/

class SdbDiddleScopeUse
  : private NoCopy
{
public:
    explicit SdbDiddleScopeUse( DT_DIDDLE_SCOPE* DS )
	  : m_OldDb( SecDbSetRootDatabase( DS->Db ))
	{
	}

	explicit SdbDiddleScopeUse( SDB_DB* Db )
	  :	m_OldDb( SecDbSetRootDatabase( Db ))
	{
	}

	~SdbDiddleScopeUse()
	{
		if( m_OldDb )
			SecDbSetRootDatabase( m_OldDb );
	}

protected:
	SDB_DB* const m_OldDb;
};


/*
**  Guard class to detach from a Db
*/

class SecDbDbDetachGuard
  : private NoCopy
{
public:
    explicit SecDbDbDetachGuard( SDB_DB* Db ) : m_Db( Db ) {} // takes ownership
	~SecDbDbDetachGuard() { if( m_Db ) SecDbDetach( m_Db ); }
  
private:
	SDB_DB* const m_Db;
};

CC_BEGIN_NAMESPACE( Gs )


/*
**  Guard class to apply a diddle and remove it when the scope exits (kind of like an Eval)
*/

template< class T >
class SecDbDiddleApply
  : private NoCopy
{
public:
	SecDbDiddleApply( T const& t, DT_VALUE* DiddleValue, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleId )
	  : m_t( t ),
	    m_diddleId( SecDbSetDiddleWithArgs( m_t.DiddleObjectGet(),
											m_t.DiddleValueTypeGet(),
											m_t.DiddleArgcGet(),
											m_t.DiddleArgvGet(),
											DiddleValue, Flags, DiddleId ))
    {
		if( m_diddleId == SDB_DIDDLE_ID_ERROR )
			GSX_THROW( GsXSecDbError, CC_NS(std,string)( "SecDbDiddleApply: diddle failed\n" ) + CC_NS(std,string)( ErrGetString() ) );
	}
	~SecDbDiddleApply()
	{
		if( !SecDbRemoveDiddleWithArgs( m_t.DiddleObjectGet(), m_t.DiddleValueTypeGet(), m_t.DiddleArgcGet(), m_t.DiddleArgvGet(), m_diddleId ) )
		    GSX_THROW( GsXSecDbError, CC_NS(std,string)( "SecDbDiddleApply: diddle remove failed\n" ) + CC_NS(std,string)( ErrGetString() ) ); // XXX Rethink this
	}

private:
	T const&		    m_t;
	SDB_DIDDLE_ID const m_diddleId;
};

CC_END_NAMESPACE
#endif 
