#define GSCVSID "$Header: /home/cvs/src/secdb/src/diddle_scopes.cpp,v 1.7 2004/02/02 23:19:45 khodod Exp $"
/****************************************************************
**
**	diddle_scopes.cpp	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: diddle_scopes.cpp,v $
**	Revision 1.7  2004/02/02 23:19:45  khodod
**	Disabled dbset_less comparisons for now.
**
**	Revision 1.6  2004/01/30 23:38:17  khodod
**	Fixed the comparators for large pointer values.
**	
**	Revision 1.5  2001/06/29 15:22:12  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.4  2001/06/28 16:57:26  singhki
**	fixes for nested diddle scope cleanup
**	
**	Revision 1.3  2001/06/27 22:28:31  singhki
**	fixes for SW4.2
**	
**	Revision 1.2  2001/06/27 19:09:20  singhki
**	little fixes
**	
**	Revision 1.1  2001/06/27 19:02:27  singhki
**	added SecDbCleanupDiddleScopes to clean up all unused diddle scopes
**	
**
****************************************************************/

#define BUILDING_SECDB
#include <portable.h>
#include <sdbdbset.h>
#include <sdb_int.h>
#include <secnodei.h>
#include <ccstl.h>
#include <set>
#include <algorithm>
#include <iostream>
#include <secdrv.h>
#include <memory>

#define for		if(0); else for

CC_USING( std::for_each );
CC_USING( std::pair );
CC_USING( std::auto_ptr );

typedef CC_STL_SET_WITH_COMP( SDB_DB*, SDB_DB::db_less ) db_set_t;
// XXX - revisit
//typedef CC_STL_MAP_WITH_COMP( SDB_DBSET*, SDB_DBSET*, SDB_DBSET::dbset_less ) dbsets_map_t;
typedef CC_STL_MAP( SDB_DBSET*, SDB_DBSET* ) dbsets_map_t;

typedef CC_STL_VECTOR( CC_NS(std,string) ) string_vec_t;
typedef CC_STL_VECTOR( SDB_DBSET* ) dbset_vec_t;
typedef CC_STL_VECTOR( SDB_DB* ) db_vec_t;

void detach_all_dbs( pair<SDB_DB* const,CC_STL_VECTOR( SDB_DB* )>& it )
{
	for_each( it.second.begin(), it.second.end(), SecDbDetach );
}

void add_all_dbs( db_set_t& dbs, pair<SDB_DB* const,db_vec_t>& it )
{
	for( db_vec_t::iterator it1 = it.second.begin(); it1 != it.second.end(); ++it1 )
		dbs.insert( *it1 );
}

inline void TRACE( SDB_TRACE_MSG Msg, SDB_DB* db )
{
	if( SecDbTraceFunc )
		SecDbTraceFunc( Msg, db );
}

inline void TRACE( SDB_TRACE_MSG Msg, SDB_DBSET* dbset1, SDB_DBSET* dbset2 )
{
	if( SecDbTraceFunc )
		SecDbTraceFunc( Msg, dbset1, dbset2 );
}


/****************************************************************
**	Adlib Name:		SecDbCleanupDiddleScopes
**	Summary:		
**	References:		
**	Description:	Cleans up any memory associated with freed diddle scopes
**	Example:		
****************************************************************/

CC_STL_VECTOR( CC_NS(std,string) ) SecDbCleanupDiddleScopes(
)
{
	string_vec_t removed_dbs;

	HASH_ENTRY_PTR Ptr;

	db_set_t garbage_dbs;

	for( db_to_db_vec_t::iterator it = sdb_reusable_diddle_scopes.begin(); it != sdb_reusable_diddle_scopes.end(); ++it )
	{
		for( db_vec_t::iterator it1 = it->second.begin(); it1 != it->second.end(); ++it1 )
		{
			garbage_dbs.insert( *it1 );
			SecDbDetach( *it1 );
		}
	}
	sdb_reusable_diddle_scopes = db_to_db_vec_t();

	HASH_FOR( Ptr, SecDbDatabaseHash )
	{
		SDB_DB* db = (SDB_DB*) HashValue( Ptr );

		if( db->OpenCount == 0 )
		{
			if( db->DbOperator == SDB_DB_UNION && db->Left->DbOperator == SDB_DB_DIDDLE_SCOPE )
				garbage_dbs.insert( db );
		}
	}

	for( db_set_t::iterator it = garbage_dbs.begin(); it != garbage_dbs.end(); ++it )
	{
		TRACE( SDB_TRACE_DESTROY_DIDDLE_SCOPE, (*it) );
		removed_dbs.push_back( (*it)->FullDbName.c_str() );
	}

	dbsets_map_t dbsets;

	dbset_vec_t all_dbsets;
	HASH_FOR( Ptr, SecDbDbSetHash )
	{
		all_dbsets.push_back( (SDB_DBSET*) HashValue( Ptr ) );
	}

	for( dbset_vec_t::iterator it = all_dbsets.begin(); it != all_dbsets.end(); ++it )
	{
		SDB_DBSET* dbset = *it;
		SDB_DBSET* new_dbsetp;
		SDB_DBSET  new_dbset;

		// if the base db is garbage then the whole dbset is garbage
		if( garbage_dbs.find( dbset->BaseDb() ) != garbage_dbs.end() )
		{
			new_dbsetp = (SDB_DBSET*) new_dbset;
			TRACE( SDB_TRACE_TRANSFORM_DBSET, dbset, new_dbsetp );
			dbsets[ dbset ] = new_dbsetp;
			continue;
		}

		// see what the new dbset is going to be
		for( SDB_DBSET::iterator it1 = dbset->begin(); it1 != dbset->end(); ++it1 )
			if( garbage_dbs.find( *it1 ) == garbage_dbs.end() )
				new_dbset.Union( *it1 );

		// if its the same then no change
		if( new_dbset.size() == dbset->size() )
		{
			dbsets[ dbset ] = dbset;
		}
		else							// else find what the new dbset will be and insert it in
		{
			if( garbage_dbs.find( dbset->Db() ) != garbage_dbs.end() )
				new_dbset.DbPrimarySet( new_dbset.BaseDb() );
			else
				new_dbset.DbPrimarySet( dbset->Db() );

			new_dbsetp = (SDB_DBSET*) new_dbset;
			dbsets[ dbset ] = new_dbsetp;
			dbsets[ new_dbsetp ] = new_dbsetp;

			TRACE( SDB_TRACE_TRANSFORM_DBSET, dbset, new_dbsetp );
		}
	}

	int token = 0;
	SDB_NODE::Destroy( 0, &token );
	for( SDB_NODE::nodes_map::iterator it = SDB_NODE::nodes().begin(); it != SDB_NODE::nodes().end(); ++it )
	{
		SDB_DBSET* new_dbset = dbsets[ (*it)->DbSetGet() ];
		if( new_dbset == (*it)->DbSetGet() ) continue;

		if( new_dbset->size() == 0 )
		{
			SDB_NODE::Destroy( *it, 0 );
			continue;
		}

		(*it)->DbSetSet( new_dbset );
	}
	SDB_NODE::Destroy( 0, &token );

	HASH_FOR( Ptr, SDB_NODE::LiteralHash() )
	{
		SDB_NODE* literal = (SDB_NODE*) HashValue( Ptr );
		SDB_DBSET* new_dbset = dbsets[ literal->DbSetGet() ];
		literal->DbSetSet( new_dbset );
	}

	HashDestroy( SecDbDbSetHash );
	SecDbDbSetHash = HashCreate( "Db Set Hash", SDB_DBSET::HashFunc, SDB_DBSET::Compare, 0, NULL );
	for( dbsets_map_t::iterator it = dbsets.begin(); it != dbsets.end(); )
	{
		if( it->first == it->second )
		{
			HashInsert( SecDbDbSetHash, it->first, it->first );
			++it;
		}
		else
		{
			SDB_DBSET *dbset = it->first;
			dbsets.erase( it++ );
			delete dbset;
		}
	}

	HASH_FOR( Ptr, SecDbDbSetOpsCache )
	{
		SDB_DBSET_OPERATION* op = (SDB_DBSET_OPERATION*) HashValue( Ptr );
		delete op;
	}
	HashDestroy( SecDbDbSetOpsCache );
	SecDbDbSetOpsCache = HashCreate( "Db Set Ops Hash", SDB_DBSET_OPERATION::HashFunc, SDB_DBSET_OPERATION::Compare, 0, NULL );

	for( db_set_t::iterator it = garbage_dbs.begin(); it != garbage_dbs.end(); )
	{
		if( (*it)->OpenCount == 0 )
		{
			auto_ptr<SDB_DB> db( *it );	// free up SDB_DB since its virtual and will have been removed from the hash
			HashDelete( SecDbDatabaseHash, db->FullDbName.c_str() );

			garbage_dbs.erase( it++ );
		}
		else
			++it;
	}

	return removed_dbs;
}
