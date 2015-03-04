/****************************************************************
**
**	SDB_DBUG.H	- Optional debugging information
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdb_dbug.h,v 1.10 2000/12/19 17:20:46 simpsk Exp $
**
****************************************************************/

#if !defined( IN_SDB_DBUG_H )
#define IN_SDB_DBUG_H


#if defined( DBUG )

#include	<stdio.h>
#include	<sys/timeb.h>
#include	<hash.h>

extern FILE	*DbugFile;
extern HASH *DbugNames;
extern int	DbugNumber;
extern int	DbugRepeat;
extern char	*DbugRepeatEnv;

enum DBUG_NODE_TYPE
{
	DbugTypeSecName,
	DbugTypeValueName,
	DbugTypeStart,
	
	DbugTypeGetDiddle,
	
	DbugTypeOldCount,
	DbugTypeOldClipSet,
	DbugTypeOldClipVisited,
	DbugTypeOldClipLatent,
	DbugTypeOldCache,
	DbugTypeOldCalc,

	DbugTypeNewCount,
	DbugTypeNewClipSet,
	DbugTypeNewClipVisited,
	DbugTypeNewClipLatent,
	DbugTypeNewCache,
	DbugTypeNewCalc,
	DbugTypeNoChildChange,
	DbugTypeNoChange,
	
	DbugTypeAbortNotSupported,
	DbugTypeAbortNoChildList,
	DbugTypeAbortSecTableElem,
	DbugTypeAbortDynamicElem,
	DbugTypeAbortDynamicValue,
	DbugTypeAbortNoChildValue,
	DbugTypeAbortValueFailed,
	
	DbugTypeSetDiddle,
	DbugTypeChangeDiddle,
	DbugTypeRemoveDiddle,
	DbugTypeSetValue

};
	

struct DBUG_TRACE
{
	DBUG_NODE_TYPE
			NodeType;	

	time_t	Time;		// in milliseconds
	
	int		ValueName,
			SecName;
	
	long	TimeStamp;	// SecDb time stamp

};
	

/*
**	Macros for SecDbGetValue
*/

#define	DBUG_GLOBAL_VARS													\
	FILE	*DbugFile = NULL;												\
	HASH	*DbugNames = NULL;												\
	int		DbugNumber = 0;													\
	int		DbugRepeat = 1;													\
	char	*DbugRepeatEnv = NULL;


#define	DBUG_OPEN()															\
{																			\
	if( !DbugFile ) 														\
		DbugFile = fopen( "sdb_dbug.dat", "wb" );							\
	if( !DbugNames )														\
		DbugNames = HashCreate( "DbugNames", HashStrHash, HashStricmp, 200, NULL );	\
	if( !DbugRepeatEnv && (DbugRepeatEnv = getenv( "SECDB_DEBUG_REPEAT" )))	\
		DbugRepeat = atoi( DbugRepeatEnv );									\
}
	

#define	DBUG_VARIABLES														\
	struct timeb															\
			DbugTime;														\
	DBUG_TRACE																\
			DbugTrace;

#define	DBUG_GET_VALUE_VARIABLES											\
	DBUG_VARIABLES															\
	int		DbugRepeatCount;
			
			
#define	DBUG_WRITE_TRACE(T,When)											\
	ftime( &DbugTime );														\
	DbugTrace.Time		= DbugTime.time * 1000 + DbugTime.millitm;			\
	DbugTrace.TimeStamp	= (When);											\
	if( !( DbugTrace.ValueName = (int) (long) HashLookup( DbugNames, ValueParams->Type->Name )))	\
	{																		\
		DbugTrace.ValueName = ++DbugNumber;									\
		HashInsert( DbugNames, strdup( ValueParams->Type->Name ), (HASH_VALUE) (long) DbugTrace.ValueName );	\
		DbugTrace.NodeType = DbugTypeValueName;								\
		DbugTrace.SecName = strlen( ValueParams->Type->Name );				\
		fwrite( &DbugTrace, sizeof( DbugTrace ), 1, DbugFile );				\
		fwrite( ValueParams->Type->Name, 1, DbugTrace.SecName, DbugFile );	\
	}																		\
	if( !( DbugTrace.SecName = (int) (long) HashLookup( DbugNames, SecPtr->SecData.Name )))	\
	{																		\
		DbugTrace.SecName = ++DbugNumber;									\
		HashInsert( DbugNames, strdup( SecPtr->SecData.Name ), (HASH_VALUE) (long) DbugTrace.SecName );	\
		DbugTrace.NodeType = DbugTypeSecName;								\
		DbugTrace.ValueName = strlen( SecPtr->SecData.Name );				\
		fwrite( &DbugTrace, sizeof( DbugTrace ), 1, DbugFile );				\
		fwrite( SecPtr->SecData.Name, 1, DbugTrace.ValueName, DbugFile );	\
		DbugTrace.ValueName = (int) (long) HashLookup( DbugNames, ValueParams->Type->Name );	\
	}																		\
	DbugTrace.NodeType	= T;												\
	fwrite( &DbugTrace, sizeof( DbugTrace ), 1, DbugFile )
		

#define	DBUG_GET_VALUE_START()					DBUG_WRITE_TRACE( DbugTypeStart, 0 );
#define	DBUG_GET_VALUE_RETURN(why,when,what)	{ STATS_INC( why ); DBUG_WRITE_TRACE( DbugType##why, (when) ); }
#define	DBUG_GET_VALUE_ABORT(why)				{ DBUG_WRITE_TRACE( DbugTypeAbort##why, 0 ); }

#define DBUG_GET_VALUE_REPEAT					for( DbugRepeatCount = 0; DbugRepeatCount < DbugRepeat; DbugRepeatCount++ )

#else

#define	DBUG_GLOBAL_VARS
#define DBUG_INIT()
#define	DBUG_OPEN()
#define	DBUG_VARIABLES
#define	DBUG_GET_VALUE_VARIABLES
#define	DBUG_GET_VALUE_START()					
#define	DBUG_GET_VALUE_RETURN(why,when,what)	STATS_INC(why)
#define	DBUG_GET_VALUE_ABORT(why)
#define DBUG_GET_VALUE_REPEAT
#endif


/*
**	Macros for SetValue
*/

#if defined( DBUG )
#define	DBUG_SET_VALUE(what)					DBUG_WRITE_TRACE( DbugTypeSetValue, 0 );
#else
#define	DBUG_SET_VALUE(what)
#endif


/*
**	Macros for Diddles
*/

#if defined( DBUG )
#define	DBUG_SET_DIDDLE(what)					DBUG_WRITE_TRACE( DbugTypeSetDiddle, 0 );
#define	DBUG_CHANGE_DIDDLE(what)				DBUG_WRITE_TRACE( DbugTypeChangeDiddle, 0 );
#define	DBUG_REMOVE_DIDDLE()					DBUG_WRITE_TRACE( DbugTypeRemoveDiddle, 0 );
#else
#define	DBUG_SET_DIDDLE(what)
#define	DBUG_CHANGE_DIDDLE(what)
#define	DBUG_REMOVE_DIDDLE()
#endif

#endif
