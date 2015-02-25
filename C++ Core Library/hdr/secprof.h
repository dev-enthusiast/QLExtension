/****************************************************************
**
**	SECPROF.H	- Slang profiler
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secprof.h,v 1.14 2000/12/19 17:20:49 simpsk Exp $
**
****************************************************************/

#if !defined( IN_SECPROF_H )
#define IN_SECPROF_H

/*
**  Forward declarations
*/

struct HashStructure;

/*
**	Profile statistics
*/

struct SLANG_PROFILER_NODE_STATS
{
	char	*NodeName;				// Name of the node (type or function)

	clock_t
			Time;					// Elapsed cpu time for node

	unsigned long
			Visits,					// Number of visits/node
			Count;					// Number of nodes/node type

};

struct SLANG_PROFILER_LINE_STATS
{
	int		LineNumber;				// Line number (repeated for sorting)

	clock_t	Time;					// cpu time per line

	unsigned long
			Visits;					// Number of visits/line

};

struct SLANG_PROFILER_STATS
{
	char	*ModuleName;			// Name of module

	int		ModuleType;				// Module type

	unsigned long
			TotalNodes,				// Total number of nodes in tree
			TotalTime,				// Total number of time in tree
			TotalVisits;			// Total number of node visits

	int		LineNumbers,			// Total number of lines in expression
			LineTableSize;			// Number of lines allocated for above

	SLANG_PROFILER_LINE_STATS
			*LineStats;				// Individual line statistics

};


/*	  
**	Prototype functions
*/

EXPORT_C_SECDB void
		SlangProfilerAttach(		SLANG_NODE *Node ),
		SlangProfilerDetach(		SLANG_NODE *Node ),
		SlangProfilerReset(			SLANG_NODE *Node ),
		SlangProfilerStatsDestroy(	HashStructure *StatsHash ),
		SlangProfilerOutputStats(	SLANG_NODE *Node, FILE *fpProf );

EXPORT_C_SECDB HashStructure
		*SlangProfilerLineStats(   	SLANG_NODE *Node ),
		*SlangProfilerNodeStats(   	SLANG_NODE *Node );

EXPORT_C_SECDB int
		SlangProfilerIntercept(		SLANG_ARGS );

EXPORT_C_SECDB void
		SlangProfilerInitialize(	void );

EXPORT_C_SECDB clock_t
		SlangProfilerReduce(  		SLANG_NODE *Node );

#endif
