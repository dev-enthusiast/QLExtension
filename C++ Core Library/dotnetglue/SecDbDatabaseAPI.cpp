#define GSCVSID "$Header: /home/cvs/src/dotnetglue/src/SecDbDatabaseAPI.cpp,v 1.15 2013/09/26 19:43:05 c038571 Exp $"
/**************************************************************************
**
** Date:        2013-09-20
** Copyright:   Copyright (C) 2012-2013, Constellation, an Exelon Company
** Author:      Tom Krzeczek
**
** SecDbDatabaseAPI.cpp: Defines database manipulation functions for the 
** SecDbAPI C# DLL. This code is based on a version by Randall Maddox.
** 
***************************************************************************
**
** $Log: SecDbDatabaseAPI.cpp,v $
** Revision 1.15  2013/09/26 19:43:05  c038571
** Fixed typo.
**
** Revision 1.14  2013/09/26 19:23:21  c038571
** Added SecDbTransactionGetCurrent().
**
** Revision 1.13  2013/09/24 18:36:46  c038571
** Minor update.
**
** Revision 1.12  2013/09/20 23:21:07  khodod
** Added standard header.
** AWR: #NA
**
**
**************************************************************************/

#include <portable.h>
#include <secdrv.h>
#include <sectrans.h>
#include "dotnetglue.h"

namespace {
	//Custom SecDb message output handler - supplied by a .NET client 
	custom_secdb_msg_handler secdb_msg_handler = NULL;

	//Custom SecDb error message output handler - supplied by a .NET client 
	custom_secdb_msg_handler secdb_error_msg_handler = NULL;
}

extern "C" void custom_global_msg_handler(char const* msg)
{
	if(secdb_msg_handler)
		(*secdb_msg_handler)(msg);
}

extern "C" void custom_global_error_msg_handler(char const* msg)
{
	if(secdb_error_msg_handler)
		(*secdb_error_msg_handler)(msg);
}

/**
* Initialize the SecDb system and gets a handle for specified security database.
* @param db_name the database name.
* @param app_name the client application name.
* @param user_name the user name.
* @see ShutdownDb().
* @return 0 on failure, or valid database handle otherwise.
*/
EXPORT_C_EXPORT SDB_DB* SecDbStartupDb(
	const char* db_name,
	const char* app_name,
	const char* user_name,
	custom_secdb_msg_handler msg_handler,
	custom_secdb_msg_handler error_msg_handler
	)
{
	//see if we need to get database name
	if(!db_name || *db_name == '\0')
		db_name = getenv("SECDB_DATABASE");
	
	if( !user_name || *user_name == '\0' )
		user_name = getenv("USER_NAME");

	if( !user_name || *user_name == '\0' )
		user_name = getenv("USERNAME");

	secdb_msg_handler = msg_handler;
	secdb_error_msg_handler = error_msg_handler;

	return SecDbStartup(
		db_name,
		SDB_DEFAULT_DEADPOOL_SIZE,
		secdb_msg_handler == 0 
		? 0 
		: custom_global_msg_handler,
		secdb_error_msg_handler == 0 
		? 0 
		: custom_global_error_msg_handler,
		app_name,
		user_name);
}


/**
* Gets a handle for a specified security database.
* @param db_name the database name
* @see ReleaseDb()
* @return 0 on failure, or valid database handle otherwise.
*/
EXPORT_C_EXPORT SDB_DB* SecDbAttachDb(
	const char* db_name)
{
	return SecDbAttach(
		db_name,
		SDB_DEFAULT_DEADPOOL_SIZE,
		0);
}


/**
* Gets a name of the current Source database.
* @return FALSE on failure, TRUE otherwise.
*/
EXPORT_C_EXPORT int SecDbSourceDatabaseGet(char** name)
{
	if( SDB_DB *db_ptr = SecDbSourceDbGet() )
	{
		*name = (char*)co_task_mem_alloc( strlen(db_ptr->DbName) + 1);
		if(!*name) return FALSE;
		strcpy( *name, db_ptr->DbName );
		return TRUE;
	}
	return FALSE;
}


/**
* Sets the current Source database.
* @return FALSE on failure, TRUE otherwise.
*/
EXPORT_C_EXPORT int SecDbSourceDatabaseSet(const char* sourcedb_name)
{
	return SecDbSourceDbSet(sourcedb_name) == 0 ? FALSE : TRUE;
}

/**
* Gets a search path for a given database.
* Callee allocates memory, caller is responsible for cleanup.
* @param db_name the database handler.
* @param current search_path the new search path(list of semi colon- separated database names).
* @return FALSE on failure, TRUE otherwise.
*/
EXPORT_C_EXPORT int SecDbSearchDbPathGet(
	SDB_DB* db_handle,
	char** search_path)
{
	SDB_DB **dbs_list = SecDbSearchPathGet(db_handle);
	if(!dbs_list) return FALSE;

	int idx = 0;
	std::string path;
	//iterate through the list of databases to pick up all the names
	SDB_DB *db_ptr = 0;
 	while( db_ptr = dbs_list[idx]){
		std::string tmp = db_ptr->DbName;
		path += tmp + ';';
		++idx;
	}
	*search_path = (char*)co_task_mem_alloc(path.length() + 1);
	if(!*search_path) return FALSE;

	strcpy( *search_path, path.c_str() );
	return TRUE;
}


/**
* Sets a search path for a given database.
* @param db_name the database handler.
* @param search_path the new search path(list of semi colon- separated database names).
* @see ReleaseDb()
* @return FALSE on failure, TRUE otherwise.
*/
EXPORT_C_EXPORT int SecDbSearchDbPathSet(
	SDB_DB* db_handle,
	const char* search_path)
{
	if( !search_path || *search_path == '\0' )
		return FALSE;
	
	const char *item_sep = ";";
	int dbs_count = 0;
	int last_del_idx = -1;
	int i = 0;
	//find out the number of dbs
	for( ; search_path[i] != '\0'; ++i )
		if( search_path[i] == item_sep[0] ){
			last_del_idx = i;
			++dbs_count;
		}

	if( last_del_idx < i-1 )
		++dbs_count;

	const char **items = (const char**)malloc( sizeof(char*) * (dbs_count+1) );
	if(!items) return FALSE;

	char *s_path = strdup(search_path);
	if(!s_path){
		free(items);
		return FALSE;
	}

	char *token = strtok(s_path, item_sep);
	i = 0;
	while(token){
		items[i++] = token;
		token = strtok(NULL, item_sep);
	}
	items[i] = 0;

	SDB_DB **db_ids = SecDbSearchPathSet(db_handle, items);
	free(items);
	free(s_path);

	return db_ids ? TRUE : FALSE;
}


/**
* Releases a security database specified by handle.
* Handle shall have been returned by GetDb(), otherwise results are undefined.
* @param db_name a database handler.
* @see SecDbAttachDb()
* @return FALSE on failure, TRUE otherwise.
*/
EXPORT_C_EXPORT int ReleaseDb(SDB_DB* handle)
{
	int ret_code = TRUE;
	if(handle)
		ret_code = SecDbDetach(handle) ? TRUE : FALSE;
	return ret_code;
}

/**
* Releases a SecDb system from memory.
* @see SecDbInitializeDb()
*/
EXPORT_C_EXPORT void SecDbShutdownDb()
{
	SecDbShutdown(TRUE);
}


/**
* Choose new database for SDB_ROOT_DATABASE.
* @param db_name a new root database
* @return Previous root database if successful, NULL otherwise. If returns NULL, root remains unchanged.
*/
EXPORT_C_EXPORT SDB_DB* SecDbSetRootDb(SDB_DB* new_db)
{
	return SecDbSetRootDatabase(new_db);
}

/**
* @return Current root security database.
*/
EXPORT_C_EXPORT SDB_DB* SecDbGetRootDb()
{
	return SecDbRootDb;
}

/**
* Gets the name of the database associated with a given handle.
* Handle shall have been returned by SecDbInitializeDb()/SecDbAttachDb(),
* otherwise results are undefined.
* @param db_name a database handler.
* @return 0 on failure, or the database name otherwise.
*/
EXPORT_C_EXPORT const char * GetDbName(SDB_DB * handle)
{
  if(!handle) return 0;
  SDB_DB_INFO info = {0};
  return SecDbDbInfoFromDb(handle, &info) 
	  ? info.DbName 
	  : 0;
}

/**
* Discards all of the operations that have been done since a matching SecDbTransBegin() was executed.
* @param db Database to operate on.
* @return TRUE if transaction aborted without error, FALSE otherwise.
*/
EXPORT_C_EXPORT int SecDbTransactionAbort(SDB_DB* db)
{
	return SecDbTransAbort(db);
}

/**
* Starts a transaction.
* @param db Database to operate on.
* @param desc Transaction's name.
* @return TRUE if transaction has began without error, FALSE otherwise.
*/
EXPORT_C_EXPORT int SecDbTransactionBegin(
	SDB_DB *db,
	char   *desc)
{
	return SecDbTransBegin(db, desc);
}


/**
* Commits a transaction.
* param db Database to work on.
* return TRUE if transaction has committed without error, FALSE otherwise.
*/
EXPORT_C_EXPORT int SecDbTransactionCommit(SDB_DB* db)
{
	return SecDbTransCommit(db);
}

/**
* Returns a depth of transaction tree for a given database.
* param db Database to work on.
*/
EXPORT_C_EXPORT int SecDbTransactionDepth(SDB_DB* db)
{
	return SecDbTransDepth(db);
}

/**
* Returns a current transaction list for a given database.
* @param db Database to work on.
* @return NULL if error or no un-committed transaction, 
* a pointer to uncommitt transaction list otherwise.
*/
EXPORT_C_EXPORT const SDB_TRANS_PART* SecDbTransactionGetCurrent(SDB_DB* db)
{
	return SecDbTransGetCurrent(db);
}

/**
* Returns the total size of all transaction parts.
* @param db Database to work on.
*/
EXPORT_C_EXPORT long SecDbTransactionSize(SDB_DB* db)
{
	return SecDbTransSize(db);
}

/**
* Returns a string of un-commited transactions for a given database.
* The names of transactions are semicolon-separated.
* @param db_handle Database to work on.
* @return TRUE if data is retrieved successfully, FALSE otherwise.
*/
EXPORT_C_EXPORT int SecDbTransactionCurrent(
	SDB_DB* db_handle,
	char** transactions)
{
	*transactions = 0;
	if(!db_handle) return FALSE;

	std::string trans;
	const SDB_TRANS_PART *tr_part = SecDbTransGetCurrent(db_handle);
	for( ; tr_part; tr_part = tr_part->Next )
		if( SDB_TRAN_BEGIN == tr_part->Type )
			trans += std::string(tr_part->Data.Begin.SecName) + ';';
	*transactions = (char*)co_task_mem_alloc( trans.length() + 1);
	if(!*transactions) return FALSE;

	strcpy( *transactions, trans.c_str() );
	return TRUE;
}
