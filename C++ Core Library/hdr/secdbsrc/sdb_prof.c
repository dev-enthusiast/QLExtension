#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_prof.c,v 1.46 2012/10/10 18:58:54 e19351 Exp $"
/****************************************************************
**
**	SDB_PROF.C	- Slang profiler
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_prof.c,v $
**	Revision 1.46  2012/10/10 18:58:54  e19351
**	Hold off on moving process info - it needs Err and DynaLink.
**
**	Revision 1.45  2012/10/10 18:17:40  e19351
**	Move processinfo to portable to break circular dependency w/ kool_ade.
**	AWR: #NA
**
**	Revision 1.44  2011/08/01 21:37:16  khodod
**	Correct the clocks-per-second logic.
**	AWR: #177463
**
**	Revision 1.43  2011/05/16 22:02:38  khodod
**	Use the standard name for clock ticks per seconds.
**	AWR: #177463
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include    <kool_ade/processinfo.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>
#if defined( __unix )
#include	<sys/times.h>
#endif
#include	<sys/types.h>
#include	<mempool.h>
#include	<hash.h>
#include	<secdb.h>
#include	<secexpr.h>
#include	<secprof.h>

/*
**	Define constants
*/

int const LINE_TABLE_GROWTH_SIZE		= 100;


/*
**	Structure used to hold profiling information for
**	each node in the parsed slang tree
*/

struct SLANG_PROFILER_NODE
{
	SLANG_FUNC
			FuncPtr;				// Pointer to actual slang function

	unsigned long
			Visits;					// Number of times function was called

	clock_t
			MyTime,					// time in function
			ChildTime;				// time in children

	int		NestDepth,				// Recursive depth counter
			ProfilerRun;			// which profiler run created this

};


/*
**	Linked list to hold attached parse trees
*/

struct PROFILER_ATTACHED_TREES
{
	SLANG_NODE
			*Root;

	int		Detach;				// FALSE for Exec and UFO scripts because we have no
								// idea who has references to them. Sucks, doesn't it?

	PROFILER_ATTACHED_TREES
			*Next;
};


/*
**	Variables
*/

static HASH
		*ProfilerStatsHash;

static PROFILER_ATTACHED_TREES
		*ProfilerAttachedTrees = NULL;

static const char
		*SmartLinkScopes[] = {
			"SmartLinkEnable",
			"SmartLink",
			NULL
		};

static int
		ProfilerInitializeDepth = 0,
		ProfilerRun = 1;


/*
**	Prototype functions
*/

static void
		LineStatsRecurse(			SLANG_NODE *Node ),
		NodeStatsRecurse(			SLANG_NODE *Node );


/*
**	Helper macros
*/

#define FOR_ATTACHED_TREES( Ptr_ )		\
	for( Ptr_ = ProfilerAttachedTrees; Ptr_; Ptr_ = Ptr_->Next )



/****************************************************************
**	Routine: SlangProfilerAttach
**	Returns: None
**	Action : Recursively attach profiler intercepts to all nodes
**			 in a parse tree
****************************************************************/

void SlangProfilerAttach(
	SLANG_NODE	*Node
)
{
	int		ArgNum;

	SLANG_PROFILER_NODE
			*ProfilerNode;

	if( Node->FuncPtr == SlangProfilerIntercept ) // if already intercepted, then don't try again
		return;

	for( ArgNum = 0; ArgNum < Node->Argc; ArgNum++ )
		SlangProfilerAttach( Node->Argv[ ArgNum ] );

	if( SLANG_NODE_IS_EVAL_ONCE( Node ) && Node->ValueDataType == DtSlang )
		SlangProfilerAttach( ((DT_SLANG *) SLANG_NODE_GET_POINTER( Node, DtSlang, NULL ))->Expression );

	if( Node->FuncPtr == SlangXDelayLoad )
	{
		const char
				*Name = SLANG_NODE_STRING_VALUE( Node );

		SLANG_FUNCTION_INFO
				*Info = SlangFunctionLookup( Name, HashStrHash( Name ));

		if( !Info || !( Node->FuncPtr = SlangFunctionBind( Info )))
			Node->FuncPtr = SlangXDelayLoad; // Can't load. Restore FuncPtr so DelayLoad will display the error
	}

	ProfilerNode = (SLANG_PROFILER_NODE *) calloc( 1, sizeof( *ProfilerNode ));
	ProfilerNode->FuncPtr		= Node->FuncPtr;
	ProfilerNode->ProfilerRun	= ProfilerRun;
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	Node->FuncPtr				= (SLANG_FUNC) SlangProfilerIntercept;
	Node->Extra					= ProfilerNode;
}



/****************************************************************
**	Routine: SlangProfilerDetach
**	Returns: None
**	Action : Recursively remove profiler intercepts from all nodes
**			 in a parse tree
****************************************************************/

void SlangProfilerDetach(
	SLANG_NODE	*Node
)
{
	int		ArgNum;

	SLANG_PROFILER_NODE
			*ProfilerNode = (SLANG_PROFILER_NODE *) Node->Extra;


	Node->FuncPtr	= ProfilerNode->FuncPtr;
	Node->Extra		= NULL;
	free( ProfilerNode );

	for( ArgNum = Node->Argc - 1; ArgNum >= 0; ArgNum-- )
		SlangProfilerDetach( Node->Argv[ ArgNum ] );

	if( SLANG_NODE_IS_EVAL_ONCE( Node ) && Node->ValueDataType == DtSlang )
		SlangProfilerDetach( ((DT_SLANG *) SLANG_NODE_GET_POINTER( Node, DtSlang, NULL ))->Expression );
}



/****************************************************************
**	Routine: SlangProfilerReduce
**	Returns: Total number of milliseconds used by node & children
**	Action : Recursively reduce the time information in each node
**			 of the parse tree, so that each node contains the
**			 actual amount of time that it took sans the time that
**			 the children took
****************************************************************/

clock_t SlangProfilerReduce(
	SLANG_NODE	*Node
)
{
	SLANG_PROFILER_NODE
			*ProfilerNode = (SLANG_PROFILER_NODE *) Node->Extra;

	int		ArgNum;

	clock_t
			TotalTime = ProfilerNode->MyTime;

	for( ArgNum = 0; ArgNum < Node->Argc; ArgNum++ )
		ProfilerNode->ChildTime += SlangProfilerReduce( Node->Argv[ ArgNum ] );

	if( SLANG_NODE_IS_EVAL_ONCE( Node ) && Node->ValueDataType == DtSlang )
		ProfilerNode->ChildTime += SlangProfilerReduce( ((DT_SLANG *) SLANG_NODE_GET_POINTER( Node, DtSlang, NULL ))->Expression );

	if( ProfilerNode->MyTime >= ProfilerNode->ChildTime )
		ProfilerNode->MyTime -= ProfilerNode->ChildTime;

	return( TotalTime );
}



/****************************************************************
**	Routine: SlangProfilerReset
**	Returns: None
**	Action : Recursively reset profiler information in each node
**			 of a parse tree
****************************************************************/

void SlangProfilerReset(
	SLANG_NODE	*Node
)
{
	int		ArgNum;

	SLANG_PROFILER_NODE
			*ProfilerNode = (SLANG_PROFILER_NODE *) Node->Extra;


	ProfilerNode->Visits			= 0;
	ProfilerNode->MyTime			= 0;
	ProfilerNode->ChildTime			= 0;
	ProfilerNode->NestDepth			= 0;

	for( ArgNum = 0; ArgNum < Node->Argc; ArgNum++ )
		SlangProfilerReset( Node->Argv[ ArgNum ] );

	if( SLANG_NODE_IS_EVAL_ONCE( Node ) && Node->ValueDataType == DtSlang )
		SlangProfilerReset( ((DT_SLANG *) SLANG_NODE_GET_POINTER( Node, DtSlang, NULL ))->Expression );
}



/****************************************************************
**	Routine: GetUserTime
**	Returns: 
**	Action : 
****************************************************************/

inline clock_t GetUserTime(
)
{
#if defined( __unix )
	struct tms
			time;

	times( &time );
	return time.tms_utime;
#else
	return clock();
#endif
}



/****************************************************************
**	Routine: SlangProfilerIntercept
**	Returns: Whatever value the actual slang function returns
**	Action : Intercept slang function execution and collect
**			 profile data
****************************************************************/

int SlangProfilerIntercept( SLANG_ARGS )
{
	SLANG_PROFILER_NODE
			*ProfilerNode = (SLANG_PROFILER_NODE *) Root->Extra;

	int		RetCode;

	clock_t
			StartTime,
			EndTime;


	if( ProfilerNode->ProfilerRun != ProfilerRun )
	{
		if( !SlangAttachProfiler )
			return SlangEvalError( SLANG_ERROR_PARMS, "Profiler instrumented code can only be executed while profiling.\nYou may need to reload/relink your script/ufo." );
		else
			return SlangEvalError( SLANG_ERROR_PARMS, "Attempting to profile code which was instrumented on a previous profiler run.\nYou may beed to reload/relink some of your scripts/ufos.\n");
	}

	ProfilerNode->Visits++;

	StartTime = GetUserTime();

	ProfilerNode->NestDepth++;	// FIX - Special handling of recursion is necessary
	RetCode = (*ProfilerNode->FuncPtr)( Root, EvalFlag, Ret, Scope );
	ProfilerNode->NestDepth--;

	EndTime = GetUserTime();

	ProfilerNode->MyTime += EndTime - StartTime;
	return( RetCode );
}



/****************************************************************
**	Routine: SlangProfilerLineStats
**	Returns: Hash table of statistics
**	Action : Collect line statistics for each module
****************************************************************/

HASH *SlangProfilerLineStats(
	SLANG_NODE	*Node
)
{
	PROFILER_ATTACHED_TREES
			*Ptr;


	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	ProfilerStatsHash = HashCreate( "Profiler Line Stats", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, MemPoolCreate());
	if( ProfilerStatsHash )
	{
		LineStatsRecurse( Node );
		FOR_ATTACHED_TREES( Ptr )
			LineStatsRecurse( Ptr->Root );
	}

	return( ProfilerStatsHash );
}



/****************************************************************
**	Routine: LineStatsRecurse
**	Returns: None
**	Action : Recursively
****************************************************************/

static void LineStatsRecurse(
	SLANG_NODE	*Node
)
{
	SLANG_PROFILER_NODE
			*ProfilerNode = (SLANG_PROFILER_NODE *) Node->Extra;

	SLANG_PROFILER_STATS
			*Stats;

	int		ArgNum;


	if( !(Stats = (SLANG_PROFILER_STATS *) HashLookup( ProfilerStatsHash, Node->ErrorInfo.ModuleName )))
	{
		Stats = (SLANG_PROFILER_STATS *) MemPoolCalloc( ProfilerStatsHash->MemPool, 1, sizeof( *Stats ));
		Stats->ModuleName	= MemPoolStrDup( ProfilerStatsHash->MemPool, Node->ErrorInfo.ModuleName );
		Stats->ModuleType	= Node->ErrorInfo.ModuleType;
		Stats->LineNumbers	= -1;
		HashInsert( ProfilerStatsHash, Stats->ModuleName, Stats );
	}

	if( Node->ErrorInfo.BeginningLine > Stats->LineNumbers )
	{
		Stats->LineNumbers = Node->ErrorInfo.BeginningLine;
		if( Stats->LineNumbers >= Stats->LineTableSize )
		{
			int		NewTableSize = LINE_TABLE_GROWTH_SIZE * ((Stats->LineNumbers / LINE_TABLE_GROWTH_SIZE) + 1);


			if( Stats->LineStats )
			{
				Stats->LineStats = (SLANG_PROFILER_LINE_STATS *) MemPoolRealloc( ProfilerStatsHash->MemPool, Stats->LineStats, NewTableSize * sizeof( SLANG_PROFILER_LINE_STATS ));
				memset( &Stats->LineStats[ Stats->LineTableSize ], 0, (NewTableSize - Stats->LineTableSize) * sizeof( SLANG_PROFILER_LINE_STATS ));
			}
			else
				Stats->LineStats = (SLANG_PROFILER_LINE_STATS *) MemPoolCalloc( ProfilerStatsHash->MemPool, 1, NewTableSize * sizeof( SLANG_PROFILER_LINE_STATS ));

			for( ;Stats->LineTableSize < NewTableSize; Stats->LineTableSize++ )
				Stats->LineStats[ Stats->LineTableSize ].LineNumber = Stats->LineTableSize;
		}
	}

	Stats->LineStats[ Node->ErrorInfo.BeginningLine ].Time		+= ProfilerNode->MyTime;
	Stats->LineStats[ Node->ErrorInfo.BeginningLine ].Visits	+= ProfilerNode->Visits;

	for( ArgNum = 0; ArgNum < Node->Argc; ArgNum++ )
		LineStatsRecurse( Node->Argv[ ArgNum ] );

	if( SLANG_NODE_IS_EVAL_ONCE( Node ) && Node->ValueDataType ==  DtSlang )
		LineStatsRecurse( ((DT_SLANG *) SLANG_NODE_GET_POINTER( Node, DtSlang, NULL ))->Expression );
}



/****************************************************************
**	Routine: SlangProfilerNodeStats
**	Returns: Hash table of statistics
**	Action : Collect statistics for each node type
****************************************************************/

HASH *SlangProfilerNodeStats(
	SLANG_NODE	*Node
)
{
	PROFILER_ATTACHED_TREES
			*Ptr;


	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	ProfilerStatsHash = HashCreate( "Profiler Node Stats", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, MemPoolCreate());
	if( ProfilerStatsHash )
	{
		NodeStatsRecurse( Node );
		FOR_ATTACHED_TREES( Ptr )
			NodeStatsRecurse( Ptr->Root );
	}

	return( ProfilerStatsHash );
}



/****************************************************************
**	Routine: NodeStatsRecurse
**	Returns: None
**	Action : Recursively
****************************************************************/

static void NodeStatsRecurse(
	SLANG_NODE	*Node
)
{
	SLANG_PROFILER_NODE
			*ProfilerNode = (SLANG_PROFILER_NODE *) Node->Extra;

	SLANG_PROFILER_NODE_STATS
			*Stats;

	int		ArgNum;

	char	Buffer[ 80 ];

	const char
			*NodeName;


	NodeName = Buffer;
	if( Node->Type == SLANG_RESERVED_FUNC )
		sprintf( Buffer, "%.79s", Node->StringValue );
	else if( Node->Type == SLANG_TYPE_VARIABLE || Node->Type == SLANG_TYPE_LOCAL_VARIABLE )
		sprintf( Buffer, "VARIABLE %.70s", Node->StringValue );
	else
		NodeName = SlangNodeNameFromType( Node->Type );

	Stats = (SLANG_PROFILER_NODE_STATS *) HashLookup( ProfilerStatsHash, NodeName );
	if( Stats )
	{
		Stats->Time		+= ProfilerNode->MyTime;
		Stats->Visits	+= ProfilerNode->Visits;
		Stats->Count++;
	}
	else
	{
		Stats = (SLANG_PROFILER_NODE_STATS *) MemPoolCalloc( ProfilerStatsHash->MemPool, 1, sizeof( *Stats ));
		Stats->NodeName		= MemPoolStrDup( ProfilerStatsHash->MemPool, NodeName );
		Stats->Time			= ProfilerNode->MyTime;
		Stats->Visits		= ProfilerNode->Visits;
		Stats->Count		= 1;
		HashInsert( ProfilerStatsHash, Stats->NodeName, Stats );
	}

	for( ArgNum = 0; ArgNum < Node->Argc; ArgNum++ )
		NodeStatsRecurse( Node->Argv[ ArgNum ] );

	if( SLANG_NODE_IS_EVAL_ONCE( Node ) && Node->ValueDataType == DtSlang )
		NodeStatsRecurse( ((DT_SLANG *) SLANG_NODE_GET_POINTER( Node, DtSlang, NULL ))->Expression );
}



/****************************************************************
**	Routine: SlangProfilerStatsDestroy
**	Returns: None
**	Action : Free statistics
****************************************************************/

void SlangProfilerStatsDestroy(
	HASH	*StatsHash
)
{
	MemPoolDestroy( StatsHash->MemPool );
}


	
/****************************************************************
**	Routine: SlangProfilerDetachTrees
**	Returns: void
**	Action : Detach from list of attached trees
****************************************************************/

static void SlangProfilerDetachTrees(
	FILE	*fpProf
)
{
	PROFILER_ATTACHED_TREES
			*Ptr;

	SDB_ENUM_PTR
			EnumPtr;

	SLANG_VARIABLE_SCOPE
			*Scope;

	SLANG_VARIABLE
			*Var;

	const char
			**ScopeName;

	int		NumUnDetachedTrees = 0;


	for( Ptr = ProfilerAttachedTrees; Ptr; Ptr = ProfilerAttachedTrees )
	{
		ProfilerAttachedTrees = Ptr->Next;
		if( Ptr->Detach )
		{
			SlangProfilerDetach( Ptr->Root );
			++NumUnDetachedTrees;
		}
		SlangFree( Ptr->Root );
		free( Ptr );
	}

	// Iterate over SmartLinkEnable scopes and flip all the states of the variables
	// this has the effect of forcing the scripts which got profiled to relink
	for( ScopeName = SmartLinkScopes; *ScopeName; ++ScopeName )
	{
		Scope = SlangVariableScopeGet( *ScopeName, TRUE );
		for( Var = SlangEnumFirstVar( &EnumPtr, Scope->VariableHash ); Var; Var = SlangEnumNextVar( EnumPtr ))
			if( Var->Value && Var->Value->DataType == DtDouble )
				Var->Value->Data.Number = ! (int) Var->Value->Data.Number;
	}

	SlangAttachProfiler = NULL;
	++ProfilerRun;

	if( NumUnDetachedTrees )
		fprintf( fpProf, "\n\nProfiler could not detach from %d trees which were probably generated\n"
				 "as a result of an exec or were UFO Value Functions.\nThis will result in a memory leak,\n"
				 "so you might wish to restart your secview session when you are done profiling.\n", NumUnDetachedTrees );
}



/****************************************************************
**	Routine: SlangProfilerOutputStats
**	Returns: None
**	Action : Print profiler statistics to a file
****************************************************************/

void SlangProfilerOutputStats(
	SLANG_NODE	*Expression,
	FILE		*fpProf
)
{
	SLANG_PROFILER_STATS
			*Stats;

	SLANG_PROFILER_NODE_STATS
			*NodeStats;

	SDB_OBJECT
			*SecPtr;

	HASH	*StatsHash;

	HASHPAIR
			*HashPair;

	FILE	*fpScript	= NULL;

	time_t	Time;

	char	
			TimeBuf[26],
			Buffer[ 1024 ],
			*StrScript	= NULL,
			*Script = NULL,
			*Str,
			c;

	HASH_ENTRY_PTR
			Ptr;

	int		Line;

	DT_VALUE
			*ScriptVal;

	PROFILER_ATTACHED_TREES
			*AttachedTrees;


	if( --ProfilerInitializeDepth )
		return;

/*
**	Print the header
*/

	time( &Time );
	fprintf( fpProf, "Profile ran on %s\n", ( gs_ctime_r( &Time, TimeBuf ) ? TimeBuf : "<bad time>" ) );


/*
**	Reduce the time information into the profile nodes
*/

	SlangProfilerReduce( Expression );
	FOR_ATTACHED_TREES( AttachedTrees )
		SlangProfilerReduce( AttachedTrees->Root );


/*
**	Print line statistics
*/

	StatsHash = SlangProfilerLineStats( Expression );
	HASH_FOR( Ptr, StatsHash )
	{
		Stats = (SLANG_PROFILER_STATS *) HashValue( Ptr );

		if( SLANG_MODULE_FILE == Stats->ModuleType )
			fpScript = fopen( Stats->ModuleName, "r" );
		else if( SLANG_MODULE_OBJECT == Stats->ModuleType )
		{
			SDB_DB	*SourceDb = SecDbSourceDbGet();
		
			
			if( SourceDb )
				SecPtr = SecDbGetByName( Stats->ModuleName, SourceDb, 0 );
			if( SourceDb && SecPtr )
			{
				ScriptVal = SecDbGetValue( SecPtr, SecDbValueTypeFromName( "Expression", NULL ));
				if( ScriptVal )
					StrScript = strdup( (char *) ScriptVal->Data.Pointer );
				Script = StrScript;
				SecDbFree( SecPtr );
			}
		}
		else if( SLANG_MODULE_STRING == Stats->ModuleType )
			Script = StrScript = strdup( Stats->ModuleName );

		fprintf( fpProf, "Module: %s\n\n", Stats->ModuleName );
		fprintf( fpProf, "   Seconds NodeHits   Line\n" );
		fprintf( fpProf, "---------- -------- ------\n" );
		for( Line = 1; Line <= Stats->LineNumbers; Line++ )
		{
			fprintf( fpProf, "%10.3f %8ld %6d | ",
					 (double) Stats->LineStats[ Line ].Time / KA_CLOCKS_PER_SEC,
					 Stats->LineStats[ Line ].Visits,
					 Line );
			if( fpScript )
			{
				if( fgets( Buffer, sizeof( Buffer ), fpScript ))
					fprintf( fpProf, "%s", Buffer );
				else
				{
					fclose( fpScript );
					fpScript = NULL;
					fprintf( fpProf, "\n" );
				}
			}
			else if( StrScript )
			{
				Str = strchr( Script, '\n' );
				if( Str )
				{
					c = Str[ 1 ];
					Str[ 1 ] = '\0';
					fputs( Script, fpProf );
					Str[ 1 ] = c;
					Script = Str + 1;
				}
				else
					fprintf( fpProf, "\n" );
			}
			else
				fprintf( fpProf, "\n" );

		}

		if( fpScript )
			fclose( fpScript );
		else if( StrScript )
			free( StrScript );

		fprintf( fpProf, "\f\n\n" );
	}
	SlangProfilerStatsDestroy( StatsHash );


/*
**	Print node statistics
*/

	fprintf( fpProf,	"Node Statistics\n\n"
						"Node Type                   Seconds NodeHits NodeCount\n"
						"------------------------ ---------- -------- ---------\n" );

	StatsHash = SlangProfilerNodeStats( Expression );
	for( HashPair = HashSortedArray( StatsHash, NULL ); HashPair->Key; HashPair++ )
	{
		NodeStats = (SLANG_PROFILER_NODE_STATS *) HashPair->Value;
		fprintf( fpProf, "%-24.24s %10.3f %8ld %9ld\n",
				 NodeStats->NodeName,
				 (double) NodeStats->Time / KA_CLOCKS_PER_SEC,
				 NodeStats->Visits,
				 NodeStats->Count );
	}
	SlangProfilerStatsDestroy( StatsHash );
	SlangProfilerDetachTrees( fpProf );
}



/****************************************************************
**	Routine: SlangProfilerParseAttach
**	Returns: TRUE/False
**	Action : Attach profiler to Root and add to list of
**			 attached trees
****************************************************************/

int SlangProfilerParseAttach(
	SLANG_NODE	*Root,
	int			Detach
)
{
	PROFILER_ATTACHED_TREES
			*Ptr = (PROFILER_ATTACHED_TREES *) malloc( sizeof( *Ptr ));

	if( !Ptr )
		return FALSE;

	Ptr->Root = Root;
	Ptr->Detach = Detach;
	Ptr->Next = ProfilerAttachedTrees;
	ProfilerAttachedTrees = Ptr;

	SlangProfilerAttach( Root );
	
	return TRUE;
}



/****************************************************************
**	Routine: SlangProfilerInitialize
**	Returns: void
**	Action : Initialize profiler to begin profiling
****************************************************************/

void SlangProfilerInitialize(
	void
)
{
	SDB_ENUM_PTR
			EnumPtr;

	SLANG_VARIABLE_SCOPE
			*Scope;

	const char
			**ScopeName;

	SLANG_VARIABLE
			*Var;


	++ProfilerInitializeDepth;
	if( SlangAttachProfiler )
		return;

	// Iterate over SmartLinkEnable scope and reset all variables
	for( ScopeName = SmartLinkScopes; *ScopeName; ++ScopeName )
	{
		Scope = SlangVariableScopeGet( *ScopeName, TRUE );
		for( Var = SlangEnumFirstVar( &EnumPtr, Scope->VariableHash ); Var; Var = SlangEnumNextVar( EnumPtr ))
			if( Var->Value && Var->Value->DataType == DtDouble )
				Var->Value->Data.Number = 0;
	}

	SlangAttachProfiler = SlangProfilerParseAttach;
}
