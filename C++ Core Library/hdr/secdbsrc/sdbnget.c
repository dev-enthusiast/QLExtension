#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbnget.c,v 1.250 2013/02/19 21:52:08 c06121 Exp $"
/****************************************************************
**
**	SDBNGET.C	- NodeGetValue and support functions
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbnget.c,v $
**	Revision 1.250  2013/02/19 21:52:08  c06121
**
**	Check size of child array to prevent indexing off end of array.
**	AWR: #280245
**
**	Revision 1.249  2012/10/10 18:58:54  e19351
**	Hold off on moving process info - it needs Err and DynaLink.
**
**	Revision 1.248  2012/10/10 18:17:40  e19351
**	Move processinfo to portable to break circular dependency w/ kool_ade.
**	AWR: #NA
**
**	Revision 1.247  2009/04/01 19:38:34  piriej
**	For nodes that are AlwaysRecompute, free existing value first before recomputing
**
**	Revision 1.246  2009/03/23 17:23:03  piriej
**	Support vt nodes that always recalculate, even when valid
**
**	Revision 1.245  2004/01/30 15:59:55  khodod
**	BugFix: Do not dereference null child pointers.
**
**	Revision 1.244  2002/04/15 19:54:51  khodod
**	BugFix: SW4.2 clobbered DataType->Name.
**	
**	Revision 1.243  2001/11/27 23:23:40  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.236  2001/10/09 20:07:06  singhki
**	revert to min/max because roguewave is gone
**	
**	Revision 1.235  2001/10/02 18:44:31  singhki
**	rw and other fixes for NT, need to define own min/max grrr, fix should remove it when rw issues are sorted
**	
**	Revision 1.234  2001/09/26 16:43:26  simpsk
**	remove use of CC_STL_EXCEPTION.
**	
**	Revision 1.233  2001/09/18 20:13:23  hsuf
**	Allow @CalcRelDesc flag to be used with other @CalcRelValue... flags.
**	Test script: Test: ufo vt sdb_release 5
**	Also use "AllReleaseValueFlags" as variable name instead of "ReleaseValueFlags",
**	since ReleaseValueFlags is also an enum.
**	
**	Revision 1.232  2001/09/18 17:03:13  hsuf
**	Fix: If a @CalcRelWhenParents node is used as both Arg and Child in the
**	parent node, then the child node will appear twice as children of parent node.
**	When the parent nodes are computed, secdb tries to release the child's value
**	twice, thus causing a red box.
**	Test script: Test: Ufo Vt sdb_release 4
**	
**	Revision 1.231  2001/09/05 21:36:59  vengrd
**	Forte needs exception to be scoped.
**	
**	Revision 1.230  2001/09/04 20:32:47  brownb
**	do not call GetArgs method when there are no arguments
**	
**	Revision 1.229  2001/08/22 20:22:39  singhki
**	initial revision of compiled graphs.
**	modified GsNodeValues to be virtual so that we can define SecDbCompiledVectorValue.
**	Created SDB_DYNAMIC_CHILDLIST which invokes the VF during BuildChildren in order
**	to get the dependencies from the compiled graph nodes.
**	
**	Revision 1.228  2001/08/07 13:52:58  singhki
**	move stats out of the node
**	
**	Revision 1.227  2001/07/19 15:08:56  goodfj
**	Use Node.StatsEnabled()
**	
**	Revision 1.226  2001/07/19 10:04:29  goodfj
**	Added MemUsed and CpuTime to SDB_NODE
**	
**	Revision 1.225  2001/07/18 19:59:13  simpsk
**	auto_ptr_array moved to extension.
**	
**	Revision 1.224  2001/07/06 20:23:10  singhki
**	always generate ~ vts even if the object is not self
**	
**	Revision 1.223  2001/07/06 18:21:46  singhki
**	if pass errors on a pass through, then return null value instead of crashing
**	
**	Revision 1.222  2001/07/05 18:30:39  hsuf
**	When we fail to cast String security name to Security, print out
**	the security name to provide more information to users.
**	
**	Revision 1.221  2001/06/26 19:36:16  hsuf
**	Added SDB_STR_TO_SEC_USED trace msg
**	Test: Ufo Bug Security Leak
**	projects: slang, secdb, secview
**	
**	Revision 1.220  2001/06/26 14:41:33  hsuf
**	Added SDB_STR_TO_SEC_CONVERSION trace msg
**	Test: Ufo Bug Security Leak
**	projects: slang, secdb, secview
**	
**	Revision 1.219  2001/06/12 23:52:19  nauntm
**	fixed(?) over-aggressive GetValueLockGet code
**	
**	Revision 1.218  2001/05/30 21:07:43  simpsk
**	Let diddle scope lists be lists of diddle scope lists and/or diddle scopes.
**	
**	Revision 1.217  2001/05/30 01:00:44  singhki
**	Stream GsNodeValues as Array in SubVfExpensive
**	
**	Revision 1.216  2001/05/29 23:12:49  hsuf
**	removed DiddleScopeList->DiddleScopeLists() which simpsk put in by mistake.
**	
**	Revision 1.215  2001/05/29 21:31:11  simpsk
**	fixed vectorization I broke. test case is "test: ufo vectorization each"
**	
**	Revision 1.214  2001/05/29 15:00:14  simpsk
**	BuildChildrenOnce should return false if CheckArgDataTypes fails. (KIS)
**	
**	Revision 1.213  2001/05/25 21:58:22  simpsk
**	small cleanups.
**	
**	Revision 1.212  2001/05/24 18:41:04  singhki
**	make sdb_async_results a pointer and initialize it in SDB_NODE::Initialize because
**	some crappy solaris platforms cannot initialize a semaphore during dll load
**	
**	Revision 1.211  2001/05/23 22:30:18  simpsk
**	simplify GetItem's intermediate case.
**	
**	Revision 1.210  2001/05/17 15:51:59  singhki
**	move sdb_async_results into async_result.cpp
**	
**	Revision 1.209  2001/05/08 18:31:09  simpsk
**	remove DYNAMIC_* -related stuff.
**	
**	Revision 1.208  2001/05/08 01:09:15  simpsk
**	add some comments.
**	
**	Revision 1.207  2001/04/30 19:52:25  singhki
**	Support SDB_PREPEND_ARGS to prepend args of node to VtApply args
**	
**	Revision 1.206  2001/04/18 01:09:42  singhki
**	oops, remove reference to cout
**	
**	Revision 1.205  2001/04/18 01:05:35  singhki
**	major update and fixes to async get value implementation.
**	
**	In order to handle getvalues off the graph which may in turn execute
**	asynchronously:
**	
**	* define the top level node as the synchronizing node for the subgraph
**	  below it.
**	* Async results are routed to the currently active synchronizing node.
**	* at every synchronization scope, allow cancellation of only the jobs
**	  requested in that subgraph if there is an error.
**	* Maintain seperate outstanding/completed request lists for each
**	  synchronization scope.
**	
**	besides this clean up the code, objectify lots of stuff. start using a
**	Scheduler function object to dispatch the requests.
**	
**	Revision 1.204  2001/04/12 18:56:10  simpsk
**	remove a "const" as SW4.2 can't handle it.
**	
**	Revision 1.203  2001/04/12 15:50:49  singhki
**	kludge for SW4.2 which does not return the type from new correctly when the type is a pointer
**	
**	Revision 1.202  2001/04/11 22:59:52  simpsk
**	change FindChild to not throw (yet).
**	
**	Revision 1.201  2001/04/11 22:25:06  simpsk
**	Add NO_DYNAMIC_PTRS switch.
**	
**	Revision 1.200  2001/04/11 15:41:07  singhki
**	comment out async stuff in dev until gsthread is on manifest
**	
**	Revision 1.199  2001/04/10 21:31:18  singhki
**	fix flushing of request queue in case of get value errors
**	
**	Revision 1.198  2001/04/10 19:12:53  singhki
**	implement async dispatching of getvalues for expensive nodes.
**	getvalue collects the results and reapplies them at the top level.
**	
**	Note that BuildChildren will block until it can compute the purple
**	children. It should try to dispatch all the purple children it can and
**	return async instead.
**	
**	Revision 1.197  2001/04/09 19:48:40  singhki
**	fixes for NT
**	
**	Revision 1.196  2001/04/09 19:23:30  singhki
**	get value cleanups. use guard classes, split getting of child values into separate function
**	
**	Revision 1.195  2001/03/20 17:18:55  singhki
**	tracing cleanups using cool trace_scope
**	
**	Revision 1.194  2001/03/16 01:57:42  singhki
**	Add SDB_TRACK_NODE_WITH_ARGS which causes Create to invalidate the
**	special node Value Type Args every time a node with args is created.
**	
**	Add Value Type Args() VT to return the list of nodes with args.
**	
**	Revision 1.193  2001/03/15 21:37:25  vengrd
**	Fixed error path.  Empty array caused a seg fault when attempting Err().
**	
**	Revision 1.192  2001/03/14 18:33:43  singhki
**	Rewrite nodes to use STL style multiset instead of skip list
**	
**	Revision 1.191  2001/01/25 16:06:43  simpsk
**	Fix return value for C interface. (SecDbInGetValue)
**	
**	Revision 1.190  2001/01/25 15:28:29  simpsk
**	Cleanups for BuildChildren.
**	
**	Revision 1.189  2001/01/23 23:52:35  singhki
**	consistent handling of CycleDetected error in getting child values in
**	order to deal with nodes which want to pass cycle errors.
**	
**	Revision 1.188  2001/01/23 21:24:50  simpsk
**	Minor cleanups.
**	
**	Revision 1.187  2001/01/23 01:02:15  nauntm
**	fixed duplicate autochildren mistest
**	
**	Revision 1.186  2001/01/19 19:15:54  shahia
**	Got rid of use of SDB_NODE_CONTEXTs Clear method, OK to PRODVER
**	
**	Revision 1.185  2001/01/09 21:47:26  singhki
**	finally dependencies within a temporary diddle scope are now detected
**	and added to AutoChildren. This is only enabled if you set
**	SDB_AUTO_CHILD_LIST. Should be enabled by DiddleScopeUse in a VF.
**	
**	Since diddle scopes are never merged when diddles are removed, we must
**	compute this information in order to determine the dependencies of a
**	node in a temporary diddle scope. Unfortunately this is never
**	cached. We should look into a way to cache this. Possibly by adding a
**	Merged member to a DbSet...
**	
**	Revision 1.184  2001/01/08 20:10:44  singhki
**	Separate out tracking the previous node from actually collecting auto
**	children (prepare to collect temporary diddle scope children).
**	
**	Revision 1.183  2001/01/04 20:45:19  simpsk
**	Fixes for autochildren (KIS).
**	
**	Revision 1.182  2001/01/03 23:26:01  simpsk
**	Fixing splitting of auto children.
**	
**	Revision 1.181  2000/12/19 23:36:27  simpsk
**	More cleanups.  Also added loop for autochildren for the dbchanged case.
**	
**	Revision 1.180  2000/12/19 20:33:18  simpsk
**	Quick casting fix.
**	
**	Revision 1.179  2000/12/19 17:20:45  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.178  2000/12/14 17:24:16  simpsk
**	Fixed predicate merging.
**	Fixed minor thinko for sucky compilers lacking a real bool type.
**	Lots and lots of comments.
**	
**	Revision 1.177  2000/12/12 21:33:45  schlae
**	Renamed DTM_FROM to DTM_FROM_ONLY
**	
**	Revision 1.176  2000/12/12 17:39:02  simpsk
**	Predicated children.
**
**	GOB_ARGs have been mostly reworked.
**	See example in Dev DB "KEN: predchild".
**
**	Revision 1.175  2000/12/05 16:48:28  singhki
**	Added Subscribe capability to nodes
**
**	Revision 1.174  2000/11/22 00:25:37  hsuf
**	when releasing descendents, also release Object of current node
**
**	Revision 1.173  2000/11/21 23:26:55  hsuf
**	remember to check that TraceFn is valid
**
**	Revision 1.172  2000/11/21 22:57:05  hsuf
**	fix SDB_RELEASE_DESCENDENT_VALUES and added more trace
**
**	Revision 1.171  2000/09/26 21:22:35  singhki
**	Make sure GetValueLock is set during buildchildren
**
**	Revision 1.170  2000/09/12 23:03:53  singhki
**	use a class to wrap the node arg list in FindChild. It will establish
**	references to literals you put inside it, and free the references when
**	it goes out of scope.
**
**	Revision 1.169  2000/09/08 10:19:59  thompcl
**	try/catch for vector mode too
**
**	Revision 1.168  2000/08/30 18:02:46  thompcl
**	make sure Value get get to DtNull if VF throws exception
**
**	Revision 1.167  2000/08/04 18:39:00  singhki
**	reset PrimaryDb even if not valid because some child may cause a build
**	children. Ideally this should be fixed by using ChildDbChanged to make
**	the node refind it's child since it is the only one which really knows
**	the Db
**
**	Revision 1.166  2000/07/20 18:48:16  huangp
**	added call to function CycleIsNotFatalSet when building children
**
**	Revision 1.165  2000/07/12 20:20:07  singhki
**	oops, set AutoNode to NULL if not AUTO_CHILD_LIST
**
**	Revision 1.164  2000/07/12 20:00:59  singhki
**	Only save/set s_CurrentAutoChildNode before calling value func. Fix bogus VTI get error
**
**	Revision 1.163  2000/07/05 23:31:29  nauntm
**	fix for passerrors: partial child lists added (KIS)
**
**	Revision 1.162  2000/06/30 09:27:13  thompcl
**	require const cast for NT
**
**	Revision 1.161  2000/06/29 17:31:46  thompcl
**	added the GsNodeValues type, to speed up Each vectorization.
**
**	Revision 1.160  2000/05/26 23:58:50  vengrd
**	added GetDepth variable
**
**	Revision 1.159  2000/05/08 20:05:36  singhki
**	remove old test code for trying to speed up vector diddles
**
**	Revision 1.158  2000/05/03 22:36:07  singhki
**	fix SW4 warning
**
**	Revision 1.157  2000/05/03 15:19:59  singhki
**	Fix ValueArray & Each interaction. Add Value Array Eval() to terminate
**	ValueArray expansion.
**
**	Revision 1.156  2000/04/28 11:02:28  vengrd
**	Use GsString in GetText. Make secdb depend upon gsbase
**
**	Revision 1.155  2000/04/27 13:20:54  vengrd
**	Set SubValue for ValueArray too in order to get alias to work
**
**	Revision 1.154  2000/04/26 18:51:17  vengrd
**	Db which evaluates to FALSE is equivalent to Self
**
**	Revision 1.153  2000/04/24 11:04:26  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**
**	Revision 1.152  2000/04/20 22:27:52  nauntm
**	first cut at implicit children
**
**	Revision 1.151  2000/04/07 17:29:38  vengrd
**	Skunk interest rate merge/checkin
**
**	Revision 1.150  2000/03/23 23:51:49  hsuf
**	We now have the correct implementation of SDB_RELEASE_DESCENDENT_VALUES
**
**	Revision 1.149  2000/03/23 18:22:16  hsuf
**	Roll back SDB_BUILD_CHILDREN_BEFORE_VALUE.  Use enum for SDB_RELEASE_VALUE instead of individual bit fields.
**	Prepare for future SDB_RELEASE_DESCENDENT_VALUES implementation.
****************************************************************/

#define BUILDING_SECDB
#define INCL_ALLOCA
#include	<portable.h>

#include	<secdb.h>
#include	<secdt.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<secerror.h>
#include	<sdb_int.h>
#include	<sdbdbset.h>
#include    <secdb/GsNodeValues.h>
#include    <secdb/async_result.h>

#include	<kool_ade/processinfo.h>

#include    <extension/auto_ptr_array.h>
CC_USING( extension::auto_ptr_array );

#include	<exception>
CC_USING( std::exception );

#include	<memory>
CC_USING( std::auto_ptr );

#include    <vector>
#include    <ccstl.h>

#include    <climits>
#include    <cmath>
#include    <cstdio>
#include    <cstdlib>
#include    <cstring>

/*
**	Argument structure to SubValue functions
*/

struct NODE_SUBVF_ARGS
{
	SDB_NODE
			*Node;

	SDB_OBJECT
			*ObjPtr;

	SDB_M_DATA
			MsgData;

	int		SetErrorOnNull,
			SetTarget;

	SDB_VALUE_TYPE_INFO
			*NodeVti;

	DT_VALUE					// [ChildC] - child values extracted from
			**SubValues;		// nodes, but no Value Array values

};

class SafeNodeArray
{
public:
	SafeNodeArray()
	: m_Argv( 0 )
	{
	}
	~SafeNodeArray()
	{
	    int m_NextArg = m_Argv.size();
		while( --m_NextArg >= 0 )
		{
			SDB_NODE* const Arg = m_Argv[ m_NextArg ];

			if( Arg->IsLiteral() && Arg->RefCountedParents() )
				Arg->LiteralRefsDecrement();
		}
	}

	SDB_NODE* SetArg( int i, SDB_NODE* Arg )
	{
		m_Argv[ i ] = Arg;
		if( Arg->IsLiteral() && Arg->RefCountedParents() )
			Arg->LiteralRefsIncrement();
		return Arg;
	}

	SDB_NODE* Append( SDB_NODE* Arg )
	{
	    m_Argv.push_back( Arg );
		if( Arg && Arg->IsLiteral() && Arg->RefCountedParents() )
			Arg->LiteralRefsIncrement();
		return Arg;
	}

	// Clearly you shouldn't call this if Arg isn't a literal
	SDB_NODE* AppendLiteral( SDB_NODE* Arg )
	{
	    m_Argv.push_back( Arg );
		if( Arg->RefCountedParents() )
			Arg->LiteralRefsIncrement();
		return Arg;
	}

	SDB_NODE** GetArgs() { return &m_Argv[0]; }

	int size() const { return m_Argv.size();}

	void pop_back() { m_Argv.pop_back(); }

private:
	SafeNodeArray(SafeNodeArray const&); // undefined to prevent copying
	SafeNodeArray& operator=(SafeNodeArray const&); // undefined to prevent copying

	CC_STL_VECTOR( SDB_NODE* ) m_Argv;
};



// guard class used to set get value locks
struct in_get_value {
	in_get_value( SDB_NODE* node ) : m_node( node )
	{
		++SDB_NODE::s_GetDepth;
		m_node->GetValueLockSet( true );
		DTM_INIT( &m_node->m_Value );
		m_node->ValidSet( true );	// mark as valid in order to detect cycles
		m_node->CacheFlagsSet( SDB_CACHE_NO_FREE );	// prevent freeing half done values in case of cycles
	}
	~in_get_value()
	{
		--SDB_NODE::s_GetDepth; m_node->GetValueLockSet( false );
		if( SecDbTraceFunc )
			SecDbTraceFunc( m_node->ValidGet() ? SDB_TRACE_CALC : SDB_TRACE_EVAL_END, m_node, 0 );
	}

	SDB_NODE* m_node;
};

struct push_vec_index {
	push_vec_index  () { SDB_NODE::VectorIndexPush(); }
	~push_vec_index () { SDB_NODE::VectorIndexPop();  }
};


/*
**	Other macros
*/

#define	DTM_FROM_ONLY( v_to, v_from )																			\
		((*(v_to)->DataType->Func)( DT_MSG_FROM, NULL, (v_to), (v_from))									\
				|| ErrMore( "Can't convert %s to a %s", (v_from)->DataType->Name, (v_to)->DataType->Name )	\
				|| !DTM_INIT(v_to))


/****************************************************************
**	Routine: SecDbInGetValue
**	Returns: int (for C interface)
**	Action : Returns true if we are currently doing a get.
****************************************************************/

int SecDbInGetValue(
)
{
	return SDB_NODE::IsInGet();
}


/****************************************************************
**	Routine: SDB_NODE::AddAutoChild
**	Returns: Nothing
**	Action : Adds a discovered dependency.
****************************************************************/

void SDB_NODE::AddAutoChild(
	SDB_NODE* ChildNode
)
{
	NodeSet& Children = s_AutoChildrenMap[ this ];

	SDB_NODE* const Child = this->FindAutoChildren( ChildNode, Children );
	if( Child )
		Child->InsertAutoChild( Children, this );
}

/****************************************************************
**	Routine: SDB_NODE::InsertAutoChild
**	Returns: void
**	Action : Inserts the child into the AutoChildren NodeSet for the parent
****************************************************************/

void SDB_NODE::InsertAutoChild(
	NodeSet		&Children,
	SDB_NODE	*Parent
)
{
	if( IsLiteralQuick() )				// no point adding literals
		return;

	if( Children.insert( this ).second ) // do not reinsert if already there
		ParentLink( Parent );				// FIX should not hide this error
}

/****************************************************************
**	Routine: SDB_NODE::FindAutoChildren
**	Returns: SDB_NODE *
**	Action : recursively finds auto children which are shared
**           with the db of the parent
****************************************************************/

SDB_NODE* SDB_NODE::FindAutoChildren(
	SDB_NODE	*Child,
	NodeSet		&Children
)
{
	if( SDB_DBSET_BEST::Lookup( DbSetGet(), Child->DbSetGet()->BaseDb() )->m_Shareable
		|| ( Child->CacheFlagsGet() & SDB_CACHE_SET ) )
		return Child;

	if( Child->CacheFlagsGet() & SDB_CACHE_DIDDLE )
	{
		// Node is diddled, probably not shareable unless its a notmydiddle of a shareable node
		SDB_DIDDLE_HEADER *DiddleHeader = Child->DiddleHeaderGet();
		if( !DiddleHeader )
			return NULL;				// something went horribly wrong, punt

		if( DiddleHeader->DiddleList->NotMyDiddle )
		{
			SDB_DIDDLE_ID DiddleId = DiddleHeader->DiddleList->DiddleID;

			nodes_iterator_range range = nodes().equal_range( Child );
			for( nodes_iterator it = range.first; it != range.second; ++it )
			{
				SDB_DIDDLE *Diddle = (*it)->DiddleFind( DiddleId );
				if( Diddle && !Diddle->NotMyDiddle )
				{
					// Shareable if node which is the source of the diddle is shareable
					if( SDB_DBSET_BEST::Lookup( DbSetGet(), (*it)->DbSetGet()->BaseDb() )->m_Shareable )
						return *it;
					break;
				}
			}
			// FIX We should always find the Source of the diddle, however if we don't
			// we should report an error and not just pretend like it was unshareable
		}
		// diddles are usually not shareable
		return NULL;
	}

	if( Child->IsLiteralQuick() )
		return Child;					// Literals are always shared

	// Now the child isn't shared however its possible that the graph under it should
	// have been merged back (or some subsection of it). So walk the graph to decide that.
	// add any children which are merged if all children are merged then just add the node itself

	bool mergeable = true;

	CC_STL_VECTOR( SDB_NODE * ) merged;

	SDB_NODE *Object = FindAutoChildren( Child->ObjectGet(), Children );
	if( !Object )
		mergeable = false;
	else
		merged.push_back( Object );

	SDB_NODE *ValueType = FindAutoChildren( Child->ValueTypeGet(), Children );
	if( !ValueType )
		mergeable = false;
	else
		merged.push_back( ValueType );

	if( Child->VtiNodeGet() )
	{
		SDB_NODE *Vti = FindAutoChildren( Child->VtiNodeGet(), Children );
		if( !Vti )
			mergeable = false;
		else
			merged.push_back( Vti );
	}

	for( int i = Child->ChildcGet() - 1; i >= 0; --i )
	{
		SDB_NODE *ChildNode = Child->ChildGet( i );
		if( ChildNode )
		{
			ChildNode = FindAutoChildren( ChildNode, Children );
			if( !ChildNode )
				mergeable = false;
			else
				merged.push_back( ChildNode );
		}
	}

	const NodeSet *AutoChildren = Child->AutoChildrenGet();
	if( AutoChildren )
	{
		for( NodeSet::const_iterator iter = AutoChildren->begin(); iter != AutoChildren->end(); ++iter )
		{
			SDB_NODE *ChildNode = FindAutoChildren( (*iter), Children );
			if( !ChildNode )
				mergeable = false;
			else
				merged.push_back( ChildNode );
		}
	}
	// If the whole subgraph was not merged, then add all the children that were merged
	if( !mergeable )
	{
		for( CC_STL_VECTOR( SDB_NODE * )::iterator iter = merged.begin(); iter != merged.end(); ++iter )
			(*iter)->InsertAutoChild( Children, this );
		return NULL;
	}
	// else the whole subgraph is mergeable so return the child
	return Child->FindAgain( CREATE, DbSetGet()->BaseDb(), SHARE );
}



/****************************************************************
**	Routine: SDB_NODE::AutoChildrenGet
**	Returns: NodeSet* / NULL
**	Action :
****************************************************************/

SDB_NODE::NodeSet *SDB_NODE::AutoChildrenGet()
{
	AutoChildrenMap::iterator iter = s_AutoChildrenMap.find( this );

	return iter != s_AutoChildrenMap.end() ? &((*iter).second) : 0;
}

/****************************************************************
**	Routine: SDB_NODE::DeleteAutoChildList
**	Returns: true (always) - for use in while loops
**	Action : Frees now obsolete inplicit children hash.
****************************************************************/

bool SDB_NODE::DeleteAutoChildList()
{
	AutoChildrenMap::iterator iter = s_AutoChildrenMap.find( this );

	if( iter != s_AutoChildrenMap.end() )
		s_AutoChildrenMap.erase( iter );

	return true;
}


/****************************************************************
**	Routine: SDB_NODE::VectorSizeMismatchMessage
**	Returns: Nothing
**	Action : Sets Err text with appropriate error message
****************************************************************/

void SDB_NODE::VectorSizeMismatchMessage(
)
{
	int		OneSize	= 0,
			ChildNum;

	char    SubscriptString[ 512 ];

	Err( ERR_INVALID_ARGUMENTS, "Children vector size mismatch" );

	{
	    char	*pString = SubscriptString;
		if( s_VectorContext && s_VectorContext->InUse )
		    pString += sprintf( pString, "[%d]", s_VectorContext->Index );
		*pString = '\0';
	}

	for( ChildNum = 0; ChildNum < ChildcGet(); ++ChildNum )
	{
		SDB_NODE* const ChildNode   = ChildGet( ChildNum );
        if( !ChildNode ) 
			continue;

		DT_VALUE* const ChildValue  = ChildNode->AccessValue();

		if( ChildValue->DataType == DtValueArray )
		{
			OneSize = DtSize( ChildValue );
			ErrMore( "Size( %s )%s == %d", ChildNode->GetText( false ).c_str(), SubscriptString, OneSize );
			break;
		}
	}

	for( ++ChildNum; ChildNum < ChildcGet(); ++ChildNum )
	{
	    SDB_NODE* const ChildNode 	= ChildGet( ChildNum );
        if( !ChildNode ) 
			continue;
		
		DT_VALUE* const ChildValue	= ChildNode->AccessValue();

		if( ChildValue->DataType == DtValueArray )
		{
			int const OtherSize = DtSize( ChildValue );
			if( OneSize != OtherSize )
			{
				ErrMore( "Size( %s )%s == %d", ChildNode->GetText( false ).c_str(), SubscriptString, OtherSize );
				break;
			}
		}
	}
}



/****************************************************************
**	Routine: NodeGetValueError
**	Returns: nothing
**	Action : Report the error for a GetValue
****************************************************************/

static void NodeGetValueError(
	SDB_NODE 			*Node,		// Pointer to the desired node
	SDB_NODE_CONTEXT	*Ctx,		// Root node of the graph being calculated + enum hash, etc->
	SDB_TRACE_FUNC		TraceFunc	// TraceFunc to use
)
{
	// Only report the bottom node of the error
	if( !Ctx->ErrNoted )
	{
		Node->Error( "GetValue" );
		if( !Ctx->ErrNoted )
			Node->TraceFunc( Ctx, TraceFunc, SDB_TRACE_ERROR_GET );
		Ctx->ErrNoted = true;
	}
}



/****************************************************************
**	Routine: SDB_NODE::GetItem
**	Returns: Eval'd node for given item description
**	Action : Finds the node for one of the pieces of the child item
****************************************************************/

SDB_NODE *SDB_NODE::GetItem(
	SDB_CHILD_COMPONENT	*Desc,		// Item description
	SDB_VALUE_TYPE_INFO	*Vti,		// Vti of node in context
	int					NoEval		// Evaluate child, but return the un eval'd one
)
{
	switch( Desc->Type )
	{
		case SDB_CHILD_LIST_DB_SUFFIX_SELF:
		{
			DT_VALUE
					TmpVal;

			if( !DiddleScopeSuffix( DbSetGet()->Db(), &TmpVal ))
				return NULL;

			SDB_NODE
					*Child = SDB_NODE::FindLiteral( &TmpVal, DbSetGet()->Db() );

			DTM_FREE( &TmpVal );
			return Child;
		}
		case SDB_CHILD_LIST_DB_SUFFIX_INT:
		case SDB_CHILD_LIST_INTERMEDIATE: // found elsewhere in childlist
		{
        const int limit = child_list_size();
        const int index = Vti->ChildList.TerminalCount + Desc->Element;

        if(limit <= index) return 0;

			// if already fulfilled this intermediate, return it
			// else attempt to fulfill it and add a purple parent link from Child to Node
		    SDB_NODE* Child = ChildGet( Vti->ChildList.TerminalCount + Desc->Element ); // intermediates come immediately after terminals
			if( !Child )
			{
			    Child = FindChild( Vti, Desc->Type, Desc->Element );
				if( !Child )
				    return NULL;

				if( Vti->ChildList.Intermediates[ Desc->Element ].Result.NoEval )
				{
				    *ChildGetAddr( Vti->ChildList.TerminalCount + Desc->Element ) = Child;
					Child->ParentLink( this );
				}
				else
				{
				    *ChildGetAddr( Vti->ChildList.TerminalCount + Desc->Element ) = SDB_NODE_PURPLE_CHILD( Child );
					Child->ParentLink( SDB_NODE_PURPLE_PARENT( this ));
				}
			}

			SDB_NODE_CONTEXT Ctx;
			if( !Child->ValidGet() && !Child->GetValue( &Ctx, DbSetGet()->Db() ))
			{
				if( PassErrorsGet() )
					Child->MarkValidLinks();
				return NULL;
			}
			if( !Child->IsLiteral() && !Vti->ChildList.Intermediates[ Desc->Element ].Result.NoEval && !NoEval )
			{
				if( Desc->Type == SDB_CHILD_LIST_DB_SUFFIX_INT ) // NOTE: This depends on the fact that using an Intermediate
				{							// as a DB_SUFFIX_INT child marks it as !NoEval, otherwise it just won't work
					DT_VALUE
							TmpVal;

					if( !DiddleScopeSuffix( Child->AccessValue(), &TmpVal ))
						return NULL;
					Child = FindLiteral( &TmpVal, DbSetGet()->Db() );
					DTM_FREE( &TmpVal );
				}
				else
					Child = FindLiteral( Child->AccessValue(), DbSetGet()->Db() );
			}
			return Child;
		}

		case SDB_CHILD_LIST_LITERAL:	// found in childlist literals
			return Vti->ChildList.Literals[ Desc->Element ];

		case SDB_CHILD_LIST_CAST:	// cast
		{
			SDB_CHILD_CAST
					*ChildCast = &Vti->ChildList.Casts[ Desc->Element ];

			SDB_NODE
					*Child;

			SDB_NODE_CONTEXT
					Ctx;

			if( !( Child = GetItem( &ChildCast->Desc, Vti, false )))
				return NULL;

			if( ChildCast->Desc.NoEval )
				Child = Child->FindCastTo( ChildCast->DataType, DbSetGet()->Db(), SHARE );

			if( !Child->ValidGet() && !Child->GetValue( &Ctx, DbSetGet()->Db() ))
			{
				if( PassErrorsGet() )
					Child->MarkValidLinks();
				return NULL;
			}

			if( !ChildCast->Desc.NoEval )
			{
				DT_VALUE
						CastVal;

				CastVal.DataType = ChildCast->DataType;
				if( !DTM_TO( &CastVal, Child->AccessValue() ))
				{
					Err( ERR_INVALID_ARGUMENTS, "@ - Cast from %s to %s failed", Child->AccessValue()->DataType->Name, ChildCast->DataType->Name );

					if( Child->AccessValue()->DataType == DtString )
						ErrMore( "value: %s", (char const*) DT_VALUE_TO_POINTER( Child->AccessValue() ) );

					return NULL;
				}

				// FIX - if cast is for DtSecurity or DtSecurityList then iterate root database through DbSetGet( CastNode ) and split as necessary
				Child = FindLiteral( &CastVal, DbSetGet()->Db() );
				DTM_FREE( &CastVal );
			}

			return Child;
		}

		case SDB_CHILD_LIST_SELF_OBJ:
			return ObjectGet();

		case SDB_CHILD_LIST_SELF_VT:
			return ValueTypeGet();

		//	Found in the argument list.  If not there, use DtNull.
		case SDB_CHILD_LIST_ARG_OR_NULL:
			if( Desc->Element >= ArgcGet() )
			{
				DT_VALUE
						TmpVal;

				DTM_INIT( &TmpVal );
				return FindLiteral( &TmpVal, DbSetGet()->Db() );
			}
			// else fall through and pick it up from the argument list

		// Found in the argument list.  If not there, then find the corresponding
		// terminal child which is always the default for that argument.
		case SDB_CHILD_LIST_ARGUMENT:
		{
			SDB_NODE
					*Child;

			SDB_NODE_CONTEXT
					Ctx;

			if( Desc->Element < ArgcGet() )
			{
				Child = ArgGet( Desc->Element );

				// Literal DtNull is just indication to use default arg
				if( Child->IsLiteral() && DtNull == Child->m_Value.DataType && Desc->Type != SDB_CHILD_LIST_ARG_OR_NULL
					&& !( Child = FindChild( Vti, SDB_CHILD_LIST_TERMINAL, Desc->Element )))
					return NULL;
			}
			else if( !( Child = FindChild( Vti, SDB_CHILD_LIST_TERMINAL, Desc->Element )))
				return NULL;

			if( !Child->ValidGet() && !Child->GetValue( &Ctx, DbSetGet()->Db() ))
			{
				if( PassErrorsGet() )
					Child->MarkValidLinks();
				return NULL;
			}

			if( !Vti->ChildList.Terminals[ Desc->Element ].Result.NoEval && !NoEval )
				Child = FindLiteral( Child->AccessValue(), DbSetGet()->Db() );

			return Child;
		}

		case SDB_CHILD_LIST_ELLIPSIS_SIZE:
			if( !Vti->ChildList.ArgEllipsis )
			{
				Err( ERR_INVALID_ARGUMENTS, "SDB_NODE::FindChild: Found Size( Ellipsis() ) but node does not have Ellipsis() args" );
				return NULL;
			}
			else
			{
				DT_VALUE TmpVal;

				TmpVal.DataType = DtDouble;
				TmpVal.Data.Number = ArgcGet() - Vti->ChildList.ArgChildCount;
				return FindLiteral( &TmpVal, DbSetGet()->Db() );
			}

		case SDB_CHILD_LIST_DIDDLE_SCOPE:
		{
			SDB_NODE
					*DiddleScopeNode,
					*Child,
					**Args;

			DT_VALUE
					TmpVal;

			int const
			        NodeArgc = 1 + ArgcGet();

			TmpVal.DataType = DtValueType;
			TmpVal.Data.Pointer = SecDbValueDiddleScope;
			DiddleScopeNode = FindLiteral( &TmpVal, DbSetGet()->Db() );

			if( !ERR_MALLOC( Args, SDB_NODE *, sizeof( SDB_NODE * ) * NodeArgc ))
				return NULL;

			Args[ 0 ] = ValueTypeGet();
			if( ArgcGet() )
				memcpy( Args + 1, ArgGetAddr( 0 ), sizeof( SDB_NODE * ) * ArgcGet() );

			// DiddleScope() nodes are never shared because their Eval'd result depends on the current database
			Child = Find( ObjectGet(), DiddleScopeNode, NodeArgc, Args, CREATE, DbSetGet()->Db(), DO_NOT_SHARE );
			free( Args );

			SDB_NODE_CONTEXT
			        Ctx;

			if( !Child->ValidGet() && !Child->GetValue( &Ctx, DbSetGet()->Db() ))
			{
				if( PassErrorsGet() )
					Child->MarkValidLinks();
				return 0;
			}

			return Child;
		}

		default:
			Err( ERR_INVALID_ARGUMENTS, "@ - unknown descriptor type %d", Desc->Type );
			return 0;
	}
}

SDB_NODE* SDB_NODE::FindPredicatedChild( SDB_CHILD_LIST_ITEM* Item, SDB_VALUE_TYPE_INFO* Vti )
{
    // If the Item has a predicate (ie. if this is a dependency in a then or else clause of an if() )
	// The predicate will be the conditional of the if()
    if( Item->Pred.Type == SDB_CHILD_LIST_NO_PREDICATE ) // if this has no predicate, nothing to do, move along
	    return 0;

	// Find the predicate
	SDB_NODE* const PredValue = this->GetItem( &Item->Pred, Vti, false /* NoEval */ );
	if( !PredValue )
	{
	    ErrMoreN( "Finding Predicate" );
		throw false;
	}

	// Have some notion of a Dummy Predicate placeholder,
	//   We need a dummy predicate because the current child is not really a dependency,
	//   because it is not going to be used.  We cannot return 0 here to indicate no
	//   dependency, because that would indicate an error, and this is not an error.
	// The dummy should never be invalidated.
	static SDB_NODE* const s_DUMMY_PRED_NODE = SDB_NODE::LinkLiteral( DtString, "Dummy Predicate Node" );

	// If my predicate was a Dummy Predicate node, I should also be a Dummy Predicate.
	// This allows Dummy Predicates to propagate to all affected children.
	// (eg. if( A )
	//      {
	//          if( B )
	//          {
	//              C;
	//          }
	//      }
	//   (A and B must be simple value types for now)
	//   here, if A is false, there is no dependency on either B or C.
	//   When finding B, we return a Dummy Predicate because A evaluated to false. (see next "if")
	//   C is predicated on B, which is now a Dummy Predicate, so C is predicated on a Dummy Predicate.
	//   When finding C, we return a Dummy Predicate because its predicate was a Dummy Predicate.
	// )
	if( PredValue == s_DUMMY_PRED_NODE )
	    return PredValue;

	// If the predicate does not evaluate to the truth value for the current clause,
	// (eg. I'm in the else clause of an if() that was true, or
	//      I'm in the then clause of an if() that was false)
	// then there is no real dependency, so return the Dummy Predicate.
	// (The != 0 crap is a work-around until we get compilers that have a real bool type)
	if( (DT_OP( DT_MSG_TRUTH, NULL, PredValue->AccessValue(), NULL ) != 0) != (Item->PredFlags != 0))
	    return s_DUMMY_PRED_NODE;

	// done with predication checking - it is a real dependency
	return 0;
}


/****************************************************************
**	Routine: ShuffleArgs
**	Returns: void
**	Action : shuffles last n args to front removing the last arg
****************************************************************/

static void ShuffleArgs(
	SDB_NODE** args,
	int const front,
	int const n,
	int const size
)
{
	int num_in_front = size - n - front;
	if( n > 0 && num_in_front > 0 )
	{
		typedef SDB_NODE* SDB_NODE_P;
		auto_ptr_array<SDB_NODE*> front_args( new SDB_NODE_P[ num_in_front ] );

		memcpy( front_args.get(), args + front, sizeof( SDB_NODE* ) * num_in_front );
		memmove( args + front, args + size - n, n * sizeof( SDB_NODE* ) );
		memcpy( args + front + n, front_args.get(), num_in_front * sizeof( SDB_NODE* ) );
	}
}



/****************************************************************
**	Routine: SDB_NODE::FindChild
**	Returns: Child node or NULL for error
**	Action : Finds item for desired child node and builds it
**			 recursively
****************************************************************/

SDB_NODE* SDB_NODE::FindChild(
	SDB_VALUE_TYPE_INFO	*Vti,
	SDB_CHILD_ITEM_TYPE	TargetResultType,
	int					TargetResultElement
)
{
	bool
			Status = true;

	SafeNodeArray
			Args;

	SDB_CHILD_LIST_ITEM
			*Item;

	if( TargetResultType == SDB_CHILD_LIST_INTERMEDIATE )
	{
		if( TargetResultElement < Vti->ChildList.IntermediateCount )
			Item = Vti->ChildList.Intermediates + TargetResultElement;
		else
		{
			Err( ERR_NOT_FOUND, "@: GetChild( Node, Vti, %d, %d )", TargetResultType, TargetResultElement );
			return 0;
		}
	}
	else
	{
		if( TargetResultElement < Vti->ChildList.TerminalCount )
			Item = Vti->ChildList.Terminals + TargetResultElement;
		else
		{
		    Err( ERR_NOT_FOUND, "@: GetChild( Node, Vti, %d, %d )", TargetResultType, TargetResultElement );
			return 0;
		}
	}

	switch( Item->Result.Type )
	{
		case SDB_CHILD_LIST_ELLIPSIS:
		{
		    DT_VALUE TmpVal;
			DTM_ALLOC( &TmpVal, DtEllipsis );
			SDB_NODE* const Child = FindLiteral( &TmpVal, DbSetGet()->Db() );
			DTM_FREE( &TmpVal );
			return Child;
		}

		case SDB_CHILD_LIST_ELLIPSIS_SIZE:
		case SDB_CHILD_LIST_DIDDLE_SCOPE:
			return GetItem( &Item->Result, Vti, false );

		case SDB_CHILD_LIST_ARGUMENT:
		    {
			    int const Num = Item->Result.Element;
				if( ArgcGet() > Num )
				{
				    SDB_NODE* const Child = ArgGet( Num );

					if( !Child->RefCountedParents() )
					{
					    if( Item->Result.NoEval )
						{
						    // FIX- This is inefficient. But I can't think of a better way. The hard part is that the
						    // VTI may change. So we cannot do it in NodeCreate. An option would be to make them always purple in Create
						    // and then fix them on InvalidateChildren because that is the only time the VTI changes... But that's not quite
						    // right either 'cause we don't want to do a getvalue on the VTI in invalidate children. Basically life sucks.
						    if( ArgGetPurple( Num ).IsPurple() )
							{
							    Child->ParentUnlink( SDB_NODE_PURPLE_PARENT( this ));
								Child->ParentLink( this );
								*ArgGetAddr( Num ) = Child;
							}
						}
						else
						{
						    if( !ArgGetPurple( Num ).IsPurple() )
							{
							    Child->ParentUnlink( this );
								Child->ParentLink( SDB_NODE_PURPLE_PARENT( this ));
								*ArgGetAddr( Num ) = SDB_NODE_PURPLE_CHILD( Child );
							}
						}
					}

					// Literal DtNull is just indication to use default arg
					if( !Child->IsLiteral() || DtNull != Child->m_Value.DataType )
					    return Child;
				}
			}
	}

	// Constant
	if( Item->Security.Type == SDB_CHILD_LIST_CONSTANT )
		return Vti->ChildList.Literals[ Item->Security.Element ];

	if( Item->Security.Type == SDB_CHILD_LIST_GET_ITEM_VT )
		return GetItem( &Item->ValueType, Vti, false );

	int		NodeIsTerminal = IsTerminalQuick();
	// Total up number of args we might need. It is args specified in ChildListItem + Number of ellisis expansions + 1 for AllowMissingObjects + 1 for flags
	int		NumArgs = Item->Argc + 1 + 1;

	DT_NODE_FLAGS NodeFlags = 0;
	if( Item->PassErrors )
		NodeFlags |= SDB_PASS_ERRORS;
	if( Item->ArrayReturn )
		NodeFlags |= SDB_ARRAY_RETURN;

	// Add up the number of ellipsis expansions
	if( Vti->ChildList.ArgEllipsis )
	{
		for( unsigned ArgNum = 0; ArgNum < Item->Argc; ++ArgNum )
			if( Item->Argv[ ArgNum ].Type == SDB_CHILD_LIST_ELLIPSIS )
				NumArgs += ArgcGet() - Vti->ChildList.ArgChildCount;
	}

	// Check to see if this dependency is predicated out
	try
	{
	    SDB_NODE* const PredNode = FindPredicatedChild( Item, Vti );
		if( PredNode )
		    return PredNode;
	}
	catch(...) // all callers of FindChild cannot yet deal with exceptions, so we plug this for now
	{
	    return 0;
	}

	// Object
	// This expression will set NodeObject to NULL if GetValue fails or the object fails to be a valid security
	SDB_NODE* NodeObject = GetItem( &Item->Security, Vti, true );
	bool SecurityLeak = false;
	if( NodeObject )
	{
	    DT_VALUE* const ObjectValue = NodeObject->AccessValue();

		if( ObjectValue->DataType == DtString
			|| ( ObjectValue->DataType == DtSecurity && ( ObjectValue->Data.Pointer || ( NodeObject = NULL ))))
		{
			// FIX - could handle other types of literals here should we ever want to, e.g., turn an array of strings into a DtSecurityList
			if( !NodeObject->IsLiteral()
				&& !( NodeObject = FindLiteral( NodeObject->AccessValue(), DbSetGet()->Db() )));
			else if( ObjectValue->DataType == DtString )
			{
			    DT_VALUE TmpVal;
				TmpVal.DataType		= DtSecurity;
				TmpVal.Data.Pointer = SecDbGetByName( (char *) NodeObject->m_Value.Data.Pointer, DbSetGet()->Db(), 0 );
				if( TmpVal.Data.Pointer )
				{
					// Purposefully discard SecPtr -- will live until SecDbDetach for DbID.
					// This leaks one security reference.
					// This may be useful for things like Pricing Date( "Security Databae" ),
					// where we do not care if Security Database is ever discarded (it will never be).

					NodeObject = FindLiteral( &TmpVal, DbSetGet()->Db() );

					if( SecDbTraceFunc )
					{
						SecDbTraceFunc( SDB_TRACE_STR_TO_SEC_CONVERSION, NodeObject, this );
						SecurityLeak = true;
					}
				}
				else
					NodeObject = 0;
			}
		}
		else
			NodeIsTerminal = false;
	}

	if( !NodeObject )
		return (SDB_NODE*) ErrMoreN( "Finding security" );

	// Must alloc Argv to have 1 arg more than NumArgs because we might need to append a node flag

	SDB_NODE* Child = 0;				// Fake initializer to make gcc warning to go away

	// Resolve Db Specifier (if present)
	if( Item->Db.Type == SDB_CHILD_LIST_SELF_DB )
		Child = NULL;
	else if( !( Child = GetItem( &Item->Db, Vti, false )))
	{
		return (SDB_NODE*) ErrMoreN( "Finding Db" );
	}
	else if( !DT_OP( DT_MSG_TRUTH, NULL, Child->AccessValue(), NULL )) // if DS is NULL
		Child = NULL;					// then set Child to NULL, no need to create an intermediate node
	else
	{
		NodeIsTerminal = false;

		if( !Child->IsLiteral())
			Child = FindLiteral( Child->AccessValue(), DbSetGet()->Db() );

		++NumArgs;
		Args.Append( 0 );
	}

	// ValueType
	// This expression will set NodeValueType to NULL if GetValue fails
	SDB_NODE* NodeValueType = GetItem( &Item->ValueType, Vti, true );
	if( NodeValueType )
	{
		DT_VALUE* ValueTypeValue = NodeValueType->AccessValue();

		// Resolve VtApply here because we already have the node generating it as a purple child
		// so we can save a pass through here.
		if( ValueTypeValue->DataType == DtVtApply )
		{
			DT_ARRAY* ApplyArray = (DT_ARRAY*) ValueTypeValue->Data.Pointer;

			if( ApplyArray->Size )
			{
				// Set NodeValueType to first element of array
				NodeValueType = FindLiteral( &ApplyArray->Element[ 0 ], DbSetGet()->Db() );
				ValueTypeValue = &NodeValueType->m_Value;
				NumArgs += ApplyArray->Size - 1;

				for( int Num = 1; Status && Num < ApplyArray->Size; ++Num )
				{
					if( !( Args.AppendLiteral( FindLiteral( &ApplyArray->Element[ Num ], DbSetGet()->Db() ))))
						Status = false;
				}
			}
		}
		if( ( ValueTypeValue->DataType == DtValueType || ValueTypeValue->DataType == DtString ))
		{
			if( !NodeValueType->IsLiteral()
				&& !( NodeValueType = FindLiteral( NodeValueType->AccessValue(), DbSetGet()->Db() )))
			    /* do nothing */;
			else if( ValueTypeValue->DataType == DtString )
			{
				// FIX - could handle other types of literals here should we ever want to, e.g., turn an array of strings into a DtSecurityList
			    DT_VALUE TmpVal;
				TmpVal.DataType		= DtValueType;
				TmpVal.Data.Pointer = SecDbValueTypeFromName( (char *) ValueTypeValue->Data.Pointer, NULL );
				if( !TmpVal.Data.Pointer )
				{
					return (SDB_NODE*) ErrMore( "Finding value type" );
				}
				NodeValueType = FindLiteral( &TmpVal, DbSetGet()->Db() );
				ValueTypeValue = &NodeValueType->m_Value;
			}

			SDB_VALUE_TYPE Vt = (SDB_VALUE_TYPE) ValueTypeValue->Data.Pointer;
			if( Vt )
			{
				// If we are in FindChild, then Node must be a terminal
				// so if ObjectGet() == NodeObject then NodeObject must be a literal security
				// this is kind of like VtApply in the sense that we must insert the AllowMissingObjects arg
				if( Item->AsObject && !( Vt->DataType->Flags & DT_FLAG_SDB_OBJECT ) &&
					( Vt->SpecialVt || ValueTypeSpecialVtGen( NodeValueType )))
				{
					DT_VALUE TmpVal;
					TmpVal.DataType = DtDouble;
					TmpVal.Data.Number = Item->PassErrors;

					Args.AppendLiteral( FindLiteral( &TmpVal, DbSetGet()->Db() ) );

					NodeValueType = Vt->SpecialVt;
				}
			}
			else
				NodeValueType = NULL;
		}
		else if( ValueTypeValue->DataType == DtSecDbNode )
			return (SDB_NODE*) ValueTypeValue->Data.Pointer;
		else
			NodeIsTerminal = false;
	}
	if( !NodeValueType || !NodeValueType->ValidGet() )
	{
		return (SDB_NODE*) ErrMore( "Finding value type" );
	}

	// Args
	for( unsigned Num = 0; Status && Num < Item->Argc; ++Num )
	{
	    SDB_NODE* Arg = 0; // Ugh, crappy Sun compiler can't deal with variable declarations in if's
		if( Item->Argv[ Num ].Type == SDB_CHILD_LIST_ELLIPSIS )
		{
			// special ellipsis child, replace with ellipsis arguments

			if( !Vti->ChildList.ArgEllipsis )
			{
				Status = Err( ERR_INVALID_ARGUMENTS, "SDB_NODE::FindChild: Found Ellipsis() but node does not have Ellipsis() args" );
				break;
			}

			for( int ChildNum = Vti->ChildList.ArgChildCount ; Status && ChildNum < ArgcGet(); ++ChildNum )
				Args.Append( ArgGet( ChildNum ) );
		}
		else if( ( Arg = Args.Append( GetItem( &Item->Argv[ Num ], Vti, false ) ) ) )
		{
			DT_VALUE* const ArgVal = Arg->AccessValue();
			if( ArgVal->DataType == DtEach )
				NodeIsTerminal = false;
			else if( ArgVal->DataType == DtNodeFlags && ( (DT_NODE_FLAGS)ArgVal->Data.Number & SDB_PREPEND_ARGS ) && NodeIsTerminal )
			{
				Args.pop_back();
				ShuffleArgs( Args.GetArgs(), Child ? 1 : 0, Num, Args.size() );
			}
		}
		else
			Status = ErrMore( "Argument %d", Num );
	}

	// Create (or find) child node
	if( Status )
	{
		// If we had a Db spec, then make the first arg the object
		// and set the object to the Db Node we found
		if( Child )
		{
			Args.SetArg( 0, NodeObject );
			NodeObject = Child;
		}

		if( !NodeIsTerminal && NodeFlags )
		{
			DT_VALUE TmpVal;
			TmpVal.DataType = DtNodeFlags;
			TmpVal.Data.Number = NodeFlags;

			Args.AppendLiteral( FindLiteral( &TmpVal, DbSetGet()->Db() ) );
		}

        size_t ArgsSize = Args.size();
        SDB_NODE** Argv = ArgsSize != 0 ? Args.GetArgs() : NULL;

		Child = Find( NodeObject, NodeValueType, ArgsSize, Argv, CREATE, DbSetGet()->Db(), SHARE );
	}

	if( SecurityLeak && SecDbTraceFunc )
		SecDbTraceFunc( SDB_TRACE_STR_TO_SEC_USED, Child, this );

	return Status ? Child : NULL;
}



/****************************************************************
**	Routine: CheckArgDataTypes
**	Returns: true if args datatypes are correct, false ow
**	Action : Finds the value types of the corresponding children
**			 and compares each arg datatype (after evaluating, if
**			 necessary) with the datatype of the value type of
**			 the corresponding child
****************************************************************/

bool SDB_NODE::CheckArgDataTypes(
	SDB_VALUE_TYPE_INFO	*Vti
)
{
	SDB_NODE_CONTEXT
			Ctx;

/*
**	Check datatypes
*/

	if( ArgcGet() > Vti->ChildList.ArgChildCount && !Vti->ChildList.ArgEllipsis )
		return Err( ERR_INVALID_ARGUMENTS, "Too many arguments, %d (max. %d)", ArgcGet(), Vti->ChildList.ArgChildCount );

	for( int Num = MIN( Vti->ChildList.ArgChildCount, ArgcGet() ); --Num >= 0; )
	{
	    SDB_CHILD_LIST_ITEM* const Item = &Vti->ChildList.Terminals[ Num ];

		if( Item->Result.Type != SDB_CHILD_LIST_ARGUMENT )
			return Err( ERR_UNKNOWN, "Expected Child to be an argument but it is %d!", Item->Result.Type );

		DT_DATA_TYPE DataType = DtNull;

		if( Item->Security.Type == SDB_CHILD_LIST_CONSTANT )
			DataType = Vti->ChildList.Literals[ Item->Security.Element ]->m_Value.DataType;
		else
		{
		    SDB_NODE* const ValueTypeNode = GetItem( &Item->ValueType, Vti, false );

			if( !ValueTypeNode )
				return( false );

			if( !ValueTypeNode->ValidGet() && !ValueTypeNode->GetValue( &Ctx, DbSetGet()->Db() ))
				return( ErrMore( "Checking datatype of arg #%d", Num ));

			DT_VALUE const* const ValueTypeData = ValueTypeNode->AccessValue();
			if( ValueTypeData->DataType == DtValueType )
				DataType = ((SDB_VALUE_TYPE) ValueTypeData->Data.Pointer)->DataType;
			else if( ValueTypeData->DataType == DtString )
			{
				SDB_VALUE_TYPE const ValueType = SecDbValueTypeFromName( (char *) ValueTypeData->Data.Pointer, NULL );
				DataType = ValueType->DataType;
			}
			else
				DataType = DtNull;
		}

		// IF value type is not instantiated yet or is DtAny, cannot check
		if( DataType == DtNull || DataType == DtAny )
			continue;

		SDB_NODE* const ArgNode = ArgGet( Num );
		// DtNull is just indication to use default arg
		if( ArgNode->IsLiteral() && DtNull == ArgNode->m_Value.DataType )
			continue;

		if( !ArgNode->ValidGet() && !ArgNode->GetValue( &Ctx, DbSetGet()->Db() ))
			return ErrMore( "Checking datatype of arg #%d", Num );

		// Finally do the test
		DT_VALUE* const NodeValue = ArgNode->AccessValue();
		if( NodeValue->DataType != DataType )
		{
			if(	NodeValue->DataType != DtValueArray && NodeValue->DataType != DtEach )
				return Err( ERR_INVALID_ARGUMENTS,
							"Arg #%d is a '%s', but should be a '%s'",
							Num, NodeValue->DataType->Name, DataType->Name );

			// Check the first element for datatype match
			DT_VALUE const* const NodeValue1 = DtSubscriptGet( NodeValue, 0, NULL );
			if( !NodeValue1 )
				return Err( ERR_INVALID_ARGUMENTS,
							"Arg #%d is a an empty Value Array, but should be a Value Array of '%s'",
							Num, DataType->Name );
			if(	NodeValue1->DataType != DataType )
				return Err( ERR_INVALID_ARGUMENTS,
							"Arg #%d is a Value Array of '%s', but should be a Value Array of '%s'",
							Num, NodeValue1->DataType->Name, DataType->Name );
		}
	}
	return true;
}



/****************************************************************
**	Routine: SDB_NODE::ArgumentUsage
**	Returns: true or false
**	Action : Fills in array with argument descriptions
****************************************************************/

bool SDB_NODE::ArgumentUsage(
	SDB_OBJECT				*SecPtr,
	SDB_VALUE_TYPE			SrcValueType,
	SDB_VALUE_TYPE_INFO		*Vti,
	DT_VALUE				*RetArray
)
{
	if( !DTM_ALLOC( RetArray, DtArray ))
		return false;

	for( int ArgNum = 0; ArgNum < Vti->ChildList.ArgChildCount; ++ArgNum )
	{
	    SDB_CHILD_LIST_ITEM* const Item = &Vti->ChildList.Terminals[ ArgNum ];

		if( Item->Result.Type != SDB_CHILD_LIST_ARGUMENT )
			return Err( ERR_UNKNOWN, "Expected ArgChild for Arg %d, but found %d!", ArgNum, Item->Result.Type );

		DT_DATA_TYPE DataType;

		if( Item->Security.Type == SDB_CHILD_LIST_CONSTANT )
			DataType = Vti->ChildList.Literals[ Item->Security.Element ]->m_Value.DataType;
		else
		{
			SDB_VALUE_TYPE ValueType = NULL;
			SDB_NODE* const Node = FindTerminal( SecPtr, SrcValueType, 0, NULL, SecDbRootDb, SHARE );
			if( !Node )
				return false;

			SDB_NODE* const ValueTypeNode = Node->GetItem( &Item->ValueType, Vti, false );
			if( ValueTypeNode )
			{
			    SDB_NODE_CONTEXT Ctx;
				if( !ValueTypeNode->ValidGet() && !ValueTypeNode->GetValue( &Ctx, Node->DbSetGet()->Db() ))
					return ErrMore( "Getting argument usage" );

				DT_VALUE* const ValueTypeData = ValueTypeNode->AccessValue();
				if( ValueTypeData->DataType == DtValueType )
					ValueType = (SDB_VALUE_TYPE) ValueTypeData->Data.Pointer;
				else if( ValueTypeData->DataType == DtString )
					ValueType = SecDbValueTypeFromName( (char *) ValueTypeData->Data.Pointer, NULL );
			}
			DataType = ValueType ? ValueType->DataType : DtNull;
		}

		char Buffer[256];
		sprintf( Buffer, "(%s) %s",
				 ( !DataType || DataType == DtNull ) ? "" : DataType->Name,
				 Item->Name ? Item->Name : "" );

		if( !DtAppendPointer( RetArray, DtString, Buffer ))
			return false;
	}
	if( Vti->ChildList.ArgEllipsis && !DtAppendPointer( RetArray, DtString, "..." ))
		return false;

	return true;
}



/****************************************************************
**	Routine: NodeGeneralizeDb
**	Returns: void
**	Action : Chooses the most general Db such that the children
**			 remain the same
****************************************************************/

inline void NodeGeneralizeDb(
	SDB_NODE	*Node
)
{
	// Node is explicitly finding child in a Db, pointless to attempt generalization
	if( Node->ObjectGet()->AccessValue()->DataType == DtDiddleScope )
		return;

	SDB_DB	*Db = Node->DbSetGet()->Db();

	while( Db->Right && Db->DbPhysical != Db )
	{
		Db = Db->Right;

		if( Node->FindAgain( SDB_NODE::DO_NOT_CREATE, Db, SDB_NODE::SHARE ) != Node )
			break;						// this suffix is too general

		// This suffix satisfies the requirements that the children remain the same
		// so try again. Find should have correctly extended the DbSet of the node
		// to make the suffix a member
	}
}



/****************************************************************
**	Routine: SDB_NODE::BuildChildren
**	Returns: true for success/false for error
**	Action : Helper function to rebuild children if the child db
**           changed while building them the first time.
****************************************************************/

bool SDB_NODE::BuildChildren(
    SDB_NODE_CONTEXT* Ctx
)
{
    bool built_children = false; // dummy initial value - we should always get assigned

	bool first_loop = true;

	do
	{
	    // Revisit - do we NEED a new context for all but the first try, or can we just always Ctx->Clear() ?
	    SDB_NODE_CONTEXT NewCtx;
		first_loop = false;
		if( !first_loop )
		    Ctx = &NewCtx;

		// attempt to build children
		TraceFunc( Ctx, SecDbTraceFunc, SDB_TRACE_BUILD_CHILDREN, DbSetGet()->Db() );
		built_children = BuildChildrenOnce( Ctx );
		if( !built_children ) // if we failed
		{
		    ChildrenValidSet( false );
			MakeFirstChildNonNull();
		    TraceFunc( Ctx, SecDbTraceFunc, SDB_TRACE_ERROR ); // report failure if we failed
		}
		TraceFunc( Ctx, SecDbTraceFunc, SDB_TRACE_DONE_BUILD_CHILDREN );


		// if we succeeded in building our children and the Db has not changed while we were getting the children,
		// we might need to split the children
		if( built_children && !ChildDbChangedGet() )
		{
		    // BuiltChildren:
		    // free NodeArgs which we may have allocated for an Apply
		    ChildrenValidSet( true );

			// Db may be too restrictive for this Node, find the least restrictive Db such that the children remain the same
			NodeGeneralizeDb( this );

			// Make sure that the children we just built are OK with all the other Dbs sharing this node
			// too. Remove all Dbs which generate splits.
			for( int Num = 0; Num < DbSetGet()->Size(); ++Num )
			{
			    SDB_DB *Db = DbSetGet()->Elem( Num );
				if( Db == DbSetGet()->Db() )
				    continue;

				// FIX- I think this works even if the DbSet shrink, but I should revisit it.
				if( SplitChildren( Db, CREATE ) != this )
				  --Num;
			}
		}
	}
	while( ChildDbChangedGet() && // if the Db changed while we were getting the children, we need to do it again

		   // reset the child list before looping again (Note: this expression must Always evaluate to true)
		   ( built_children ? ( ChildrenValidSet( false ),
								MakeFirstChildNonNull(), true )
			 : true ) );

	return built_children;
}

/****************************************************************
**	Routine: SDB_NODE::BuildChildrenOnce
**	Returns: true for success/false for error
**	Action : Build all the child nodes
****************************************************************/

bool SDB_NODE::BuildChildrenOnce(
	SDB_NODE_CONTEXT* Ctx
)
{
	// set a GetValueLock during build children because diddle color logic needs
	// to fixup the node if a getvalue on an intermediate removes a child which has already been determined
	class GetValueLock
	{
	public:
	    explicit GetValueLock( SDB_NODE* node )
		  : m_node    ( node ),
			m_OldValue( node->GetValueLockGet() )
	    {
		    m_node->GetValueLockSet( true );
		}
		~GetValueLock()
		{
			m_node->GetValueLockSet( m_OldValue );
		}

	private:
		SDB_NODE* const m_node;
		bool      const m_OldValue;

	} getValueLock( this );

	class ArrayGuard
	{
	public:
	    explicit ArrayGuard( SDB_NODE** Ptr = 0 ) throw() : m_Ptr( Ptr ) {}
	    ~ArrayGuard() throw() { delete [] m_Ptr; }

	    ArrayGuard& operator=( SDB_NODE** Ptr ) throw()
	    {
		    delete [] m_Ptr; m_Ptr = Ptr; return *this;
		}

	    SDB_NODE*& operator[]( int index ) throw() { return m_Ptr[ index ] ;}

	    SDB_NODE** val( SDB_NODE** Default = 0 ) throw() { return m_Ptr ? m_Ptr : Default; }

	private:
	    SDB_NODE** m_Ptr;

	    ArrayGuard( ArrayGuard const& ) {}; // undefined to prevent copying
	    ArrayGuard& operator=( ArrayGuard const& ) { return *this;}; // undefined to prevent copying
		// should remove the bodies of the above functions.
		// They exist only to pacify SW4.2 and should be removed (Both SW4.2 and the function bodies)
	};

	ArrayGuard NodeArgs;

	// Clear out any children we might have left in order to get pass errors invalidation
	if( ChildcGet() > 0 && ChildGet( 0 ) )
		InvalidateChildrenInt();

/*
**	Determine type of node
*/

	DT_VALUE* ObjectValue    = ObjectGet()->AccessValue();
	DT_VALUE* ValueTypeValue = ValueTypeGet()->AccessValue();
	int ChildArgc      = ArgcGet();
	int DecompSize     = 1;

	ChildDbChangedSet         ( false );
	NodeTypeSet               ( SDB_NODE_TYPE_UNKNOWN );
	ReleaseDescendentValuesSet( false );
	ReleaseValueFlagSet       ( SDB_NODE::s_ReleaseValueWhenParentsValidDefault
								? SDB_NODE::ReleaseValueWhenParentsValidFlag
								: SDB_NODE::ReleaseValueNoneFlag );

	// If vt or obj are Value Arrays then node is decomposed into one node for each element
	if( ObjectValue->DataType == DtValueArray )
	{
		NodeTypeSet( SDB_NODE_TYPE_VALUE_ARRAY );
		DecompSize         = DtSize( ObjectValue );
	}
	else if( ValueTypeValue->DataType == DtValueArray )
	{
		NodeTypeSet( SDB_NODE_TYPE_VALUE_ARRAY );
		DecompSize         = DtSize( ValueTypeValue );
	}

	// If vt, obj, or any arg are Eaches then node is decomposed into one node for each element
	else if( ObjectValue->DataType == DtEach )
	{
		NodeTypeSet( SDB_NODE_TYPE_EACH );
		DecompSize         = DtSize( ObjectValue );
	}
	else if( ValueTypeValue->DataType == DtEach )
	{
		NodeTypeSet( SDB_NODE_TYPE_EACH );
		DecompSize         = DtSize( ValueTypeValue );
	}
	if( ObjectGet() != SubscriptObject ) // FIX- nasty hack to prevent cycles on Subscripts of Eaches or ValueArrays
	{
		for( int ArgNum1 = 0; ArgNum1 < ArgcGet(); ++ArgNum1 )
		{
		    DT_VALUE* ArgValue = ArgGet( ArgNum1 )->AccessValue();
			if( ArgValue->DataType == DtValueArray && ValueTypeGet() != s_ValueArrayEvalVT )
			{
				NodeTypeSet( SDB_NODE_TYPE_VALUE_ARRAY );
				DecompSize = DtSize( ArgValue );
				break;					// ValueArray takes precedence over each
			}
			else if( ArgValue->DataType == DtEach )
			{
				NodeTypeSet( SDB_NODE_TYPE_EACH );
				DecompSize = DtSize( ArgValue );
			}
		}
	}

	if( NodeTypeGet() == SDB_NODE_TYPE_UNKNOWN )
	{
		if( ComputeIsTerminal() )
		{
			NodeTypeSet( SDB_NODE_TYPE_TERMINAL );
		}
		else
		{
			DT_NODE_FLAGS
					NodeFlags = 0;

			PassErrorsSet( false );
			if( ChildArgc > 0 )
			{
				SDB_NODE* LastArg = ArgGet( ChildArgc - 1 );
				if( LastArg && LastArg->ValidGet() && LastArg->m_Value.DataType == DtNodeFlags )
				{
					NodeFlags = (DT_NODE_FLAGS) LastArg->m_Value.Data.Number;

					if( NodeFlags & SDB_PASS_ERRORS )
						PassErrorsSet( true );

					if( NodeFlags & SDB_ARRAY_RETURN )
						NodeTypeSet( SDB_NODE_TYPE_ARRAY );

					--ChildArgc;
				}
			}

			// FIX- VtApply doesn't work if Vt is an Each
			if( ValueTypeValue->DataType == DtVtApply )
			{
				DT_ARRAY* ApplyArray = (DT_ARRAY *) ValueTypeValue->Data.Pointer;

				if( ApplyArray->Size == 0 )
				    return Err( ERR_UNSUPPORTED_OPERATION, "VtApply( Vt, Args )( obj ): Vt must not be an empty array." );

				if( ApplyArray->Element->DataType != DtValueType )
				    return Err( ERR_UNSUPPORTED_OPERATION, "VtApply( Vt, Args )( obj ): Vt must be of type %s, not %s", DtValueType->Name, ApplyArray->Element->DataType->Name );

				{
				    int Num1 = ChildArgc + ApplyArray->Size - 1;

					if( Num1 > 0 )
					{
					    typedef SDB_NODE* SDB_NODE_PTR; // SW4.2 kludge - what a sucky piece of ((inaudible))
						NodeArgs = new SDB_NODE_PTR[ Num1 ];
					}
				}

				int Num = 0;

				// First add rest of VtApply elements as args
				for( int ArgNum2 = 1; ArgNum2 < ApplyArray->Size; ++ArgNum2 )
					if( !( NodeArgs[ Num++ ] = FindLiteral( &ApplyArray->Element[ ArgNum2 ], DbSetGet()->Db() )))
					    return false;

				// Now add node's own arguments as rest of args
				for( int ArgNum3 = 0; ArgNum3 < ChildArgc; ++ArgNum3, ++Num )
					if( !( NodeArgs[ Num ] = ArgGet( ArgNum3 )->IsLiteral()
						   ? ArgGet( ArgNum3 ) : FindLiteral( &ArgGet( ArgNum3 )->m_Value, DbSetGet()->Db() )))
					    return false;

				if( NodeFlags & SDB_PREPEND_ARGS )
					ShuffleArgs( NodeArgs.val(), 0, ChildArgc, Num );

				ChildArgc = Num;
			}
			else if( ValueTypeValue->DataType != DtValueType )
			{
			    return Err( ERR_UNSUPPORTED_OPERATION, "vt( obj ) - vt must be of type %s, not %s", DtValueType->Name, ValueTypeValue->DataType->Name );
			}

			if( ObjectValue->DataType == DtSecurity )
			{
				NodeTypeSet( SDB_NODE_TYPE_PASSTHRU ); // Purposely ignores Array Return since it is irrelevant
			}
			else if( ObjectValue->DataType == DtSecurityList || ObjectValue->DataType == DtDiddleScopeList )
			{
			    NodeTypeSet( SDB_NODE_TYPE_ARRAY );
			}
			else if( ObjectValue->DataType == DtDiddleScope )
			{
				if( ArgcGet() == 0 )
				{
				    SDB_VALUE_TYPE NodeValueType = (SDB_VALUE_TYPE) ValueTypeValue->Data.Pointer;

					if( ValueTypeValue->DataType == DtVtApply )
					{
					    DT_ARRAY* ApplyArray = (DT_ARRAY *) ValueTypeValue->Data.Pointer;
					    NodeValueType = (SDB_VALUE_TYPE) ApplyArray->Element->Data.Pointer;
					}

				    return Err( ERR_INVALID_ARGUMENTS, "%s( %s ): Expected Object argument", NodeValueType->Name, ObjectValue->DataType->Name );
				}

				NodeTypeSet( SDB_NODE_TYPE_PASSTHRU );

				if( !AdjustChildc( 1 ))
				    return false;

				SDB_NODE* Child = Find( ArgGet( 0 ), ValueTypeGet(), ChildArgc - 1, ArgGetAddr( 1 ), CREATE,
										((DT_DIDDLE_SCOPE *) ObjectValue->Data.Pointer)->Db, SHARE );

				bool Status = Child && Child->ParentLink( this );

				if( !Status )
				{
					*ChildGetAddr( 0 ) = NULL;
					return false;
				}
				*ChildGetAddr( 0 ) = Child;
				return true;
			}
			else
			{
			    return Err( ERR_UNSUPPORTED_OPERATION, "vt( obj ) - obj must be of type %s or %s, not %s", DtSecurity->Name, DtSecurityList->Name, ObjectValue->DataType->Name );
			}
		}
	} // if( NodeTypeGet() == SDB_NODE_TYPE_UNKNOWN )


/*
**	Build the child graph
*/

	switch( NodeTypeGet() )
	{
		case SDB_NODE_TYPE_TERMINAL:
		{
			int AllReleaseValueFlags;

			SDB_VALUE_TYPE_INFO* Vti = VtiGet();
			if( !Vti )
			    return Err( ERR_UNSUPPORTED_OPERATION, "Unsupported value type %s( %s )",
							((SDB_VALUE_TYPE) ValueTypeGet()->m_Value.Data.Pointer)->Name,
							((SDB_OBJECT *) ObjectGet()->m_Value.Data.Pointer)->SecData.Name );

			if( ( AllReleaseValueFlags = ( Vti->ValueFlags & ( SDB_RELEASE_VALUE_MASK | SDB_RELEASE_DESCENDENT_VALUES ) )) != 0 )
			{
				if( ( ( AllReleaseValueFlags & SDB_RELEASE_VALUE_MASK ) & 
					  ( ( AllReleaseValueFlags & SDB_RELEASE_VALUE_MASK ) - 1 ) ) != 0 )
				    return Err( ERR_UNSUPPORTED_OPERATION, "VT %s has more than one SDB_RELEASE_VALUE... flag",
								((SDB_VALUE_TYPE) ValueTypeGet()->m_Value.Data.Pointer)->Name );

				if( AllReleaseValueFlags & SDB_RELEASE_VALUE )
					ReleaseValueFlagSet( SDB_NODE::ReleaseValueFlag );
				else if( AllReleaseValueFlags & SDB_RELEASE_VALUE_WHEN_PARENTS_VALID )
					ReleaseValueFlagSet( SDB_NODE::ReleaseValueWhenParentsValidFlag );
				else if( AllReleaseValueFlags & SDB_RELEASE_VALUE_BY_ANCESTOR )
					ReleaseValueFlagSet( SDB_NODE::ReleaseValueByAncestorFlag );

				if( AllReleaseValueFlags & SDB_RELEASE_DESCENDENT_VALUES )
					ReleaseDescendentValuesSet( true );
			}

			AlwaysRecomputeSet( Vti->ValueFlags & SDB_ALWAYS_RECOMPUTE );

			if( !SecDbMsgGetChildList( (SDB_OBJECT *) ObjectValue->Data.Pointer, Vti ))
			    return false;

			PassErrorsSet( Vti->ChildList.PassErrors );
			CycleIsNotFatalSet( Vti->ChildList.CycleIsNotFatal );

			if( Vti->ChildList.DynamicChildList )
			{
				CC_STL_VECTOR(SDB_NODE*) children;
				SDB_M_DATA MsgData;
				MsgData.DynamicChildList.node = this;
				MsgData.DynamicChildList.children = &children;

				if( !SDB_MESSAGE_VT_TRACE( (SDB_OBJECT*) ObjectValue->Data.Pointer, SDB_MSG_GET_DYNAMIC_CHILD_LIST, &MsgData, Vti ) )
					return false;

				if( !AdjustChildc( children.size() ) )
					return false;

				CC_STL_VECTOR(SDB_NODE*)::const_iterator it;
				for( it = children.begin(); it < children.end(); ++it )
				{
					SDB_NODE_CHILD_PTR child = *it;
					if( !child->ParentLink( child.IsPurple() ? SDB_NODE_PURPLE_PARENT( this ) : this ) )
						break;
				}
				if( it != children.end() )
				{
					AdjustChildc( it - children.begin() );
					return false;
				}
				return true;
			}

			if( !AdjustChildc( Vti->ChildList.TerminalCount + Vti->ChildList.IntermediateCount ))
				return false;

			// Zero out intermediates
			memset( ChildGetAddr( Vti->ChildList.TerminalCount ), 0,
					sizeof( SDB_NODE * ) * Vti->ChildList.IntermediateCount );

			for( int Num = 0; Num < Vti->ChildList.TerminalCount; ++Num )
			{
				SDB_NODE* Child = FindChild( Vti, SDB_CHILD_LIST_TERMINAL, Num );
				if( Child )
				{
					if( Num >= Vti->ChildList.ArgChildCount || Vti->ChildList.Terminals[ Num ].Result.NoEval )
					{
						if( !Child->ParentLink( this ))
							Child = NULL;
					}
					else
					{
						if( !Child->ParentLink( SDB_NODE_PURPLE_PARENT( this )))
							Child = NULL;
						else
							Child = SDB_NODE_PURPLE_CHILD( Child );
					}
				}

				if( !( *ChildGetAddr( Num ) = Child ) && ( !PassErrorsGet() || IsErrorFatal( ErrNum )))
				{
					memset( ChildGetAddr( Num ), 0, sizeof( SDB_NODE * ) * ( Vti->ChildList.TerminalCount - Num ));
					return false;
				}
			}

			if( ArgcGet() && !CheckArgDataTypes( Vti ) )
			    return false;

			break;
		}

		case SDB_NODE_TYPE_VALUE_ARRAY:
		case SDB_NODE_TYPE_EACH:
		{
			SDB_NODE
					**DecompArgv;

			DT_VALUE
					**DecompVals,
					*SubVal,
					NullVal;

			DT_DATA_TYPE
					CheckDt;

			// need to strip off node flags because we will be finding terminals as children
			// we can ignore arrayreturn because Each/ValueArray always return arrays by default
			if( ChildArgc > 0 )
			{
			    SDB_NODE* LastArg = ArgGet( ChildArgc - 1 );
				if( LastArg && LastArg->ValidGet() && LastArg->m_Value.DataType == DtNodeFlags )
				{
					DT_NODE_FLAGS NodeFlags = (DT_NODE_FLAGS) LastArg->m_Value.Data.Number;

					if( NodeFlags & SDB_PASS_ERRORS )
						PassErrorsSet( true );
					--ChildArgc;
				}
			}

			DTM_INIT( &NullVal );

			bool Status = true;
			CheckDt = NodeTypeGet() == SDB_NODE_TYPE_VALUE_ARRAY ? DtValueArray : DtEach;

			// This should really be Argc + 2, but KLL can't remember why it is 3 so we just leave it.
			Status = ERR_CALLOC( DecompArgv, SDB_NODE *, ArgcGet() + 3, sizeof( SDB_NODE * ));
			Status = ERR_CALLOC( DecompVals, DT_VALUE *, ArgcGet() + 3, sizeof( DT_VALUE * )) && Status;

			if( CheckDt == ObjectValue->DataType )
				DecompVals[ 0 ] = ObjectValue;
			else
				DecompArgv[ 0 ] = ObjectGet();

			if( CheckDt == ValueTypeValue->DataType )
				DecompVals[ 1 ] = ValueTypeValue;
			else
				DecompArgv[ 1 ] = ValueTypeGet();

			for( int ArgNum4 = 0; ArgNum4 < ChildArgc; ++ArgNum4 )
			{
			    DT_VALUE* ArgValue = ArgGet( ArgNum4 )->AccessValue();
				if( CheckDt == ArgValue->DataType )
					DecompVals[ ArgNum4 + 2 ] = ArgValue;
				else
					DecompArgv[ ArgNum4 + 2 ] = ArgGet( ArgNum4 );
			}

			if( Status && ( Status = AdjustChildc( DecompSize )))
			{
				for( int Num = 0; Num < DecompSize; ++Num )
				{
				    SDB_NODE* Child = 0;

					// Get literal nodes for object[Num] and valuetype[Num]
					for( int ArgNum5 = ChildArgc + 1; Status && ArgNum5 >= 0; --ArgNum5 )
					{
						if( !DecompVals[ ArgNum5 ] )
						    continue;

						if( ArgNum5 > 1 && !ArgGet( ArgNum5 - 2 )->IsLiteral())
						{
							// Don't evaluate this argument because it was passed in unevaluated

						    DT_VALUE
							        SubscriptVal;

							SubscriptVal.DataType = DtDouble;
							SubscriptVal.Data.Number = Num;
							SubVal = DtSubscriptGet( DecompVals[ ArgNum5 ], Num, &NullVal );

							Status = Status && ( DecompArgv[ ArgNum5 ] = ArgGet( ArgNum5 - 2 )->FindSubscriptOf( SubVal->DataType, &SubscriptVal, DbSetGet()->Db(), SHARE ));
							if( ArgGetPurple( ArgNum5 - 2 ).IsPurple() )
							{
							    // Arg is purple, since we are not evaluating it, there is no need to be purple, so whack it
							    // FIX- This is kinda inefficient, but I'm not sure how to do it better
							    // also currently this is wrong 'cause if the size of the each changes, we are ****ed!
							    ArgGet( ArgNum5 - 2 )->ParentUnlink( SDB_NODE_PURPLE_PARENT( this ));
								ArgGet( ArgNum5 - 2 )->ParentLink( this );
								*ArgGetAddr( ArgNum5 - 2 ) = ArgGet( ArgNum5 - 2 );
							}
						}
						else
						{
						    SubVal = DtSubscriptGet( DecompVals[ ArgNum5 ], Num, NULL );
							if( SubVal )
							{
							    if( SubVal->DataType == DtSecDbNode )
								    Status = ( Status &&
											   ( ( Child = (SDB_NODE*) SubVal->Data.Pointer ) ||
												 Err( ERR_UNSUPPORTED_OPERATION, "Element in Each is a SecDb Node with a NULL node reference" ) ));
								else
								    Status = Status && ( DecompArgv[ ArgNum5 ] = FindLiteral( SubVal, DbSetGet()->Db() ));
							}
							else
							    Status = FALSE;
						}
					} // for
					Status = Status && ( Child ||
										 ( Child = Find( DecompArgv[ 0 ], DecompArgv[ 1 ], ChildArgc, &DecompArgv[ 2 ], CREATE, DbSetGet()->Db(), SHARE )));
					Status = Status && Child->ParentLink( this );
					if( !Status )
					{
						memset( ChildGetAddr( Num ), 0, sizeof( SDB_NODE * ) * ( ChildcGet() - Num ));
						return false;
					}
					*ChildGetAddr( Num ) = Child;

				}
			}
			free( DecompVals );
			free( DecompArgv );

			if( !Status )
			    return false;

			break;
		}

		case SDB_NODE_TYPE_PASSTHRU:
		{
			SDB_OBJECT* SecPtr = (SDB_OBJECT*) ObjectValue->Data.Pointer;
			if( !AdjustChildc( 1 ))
			    return false;

			SDB_VALUE_TYPE NodeValueType = (SDB_VALUE_TYPE) ValueTypeValue->Data.Pointer;

			if( ValueTypeValue->DataType == DtVtApply )
			{
			    DT_ARRAY* ApplyArray = (DT_ARRAY *) ValueTypeValue->Data.Pointer;
			    NodeValueType = (SDB_VALUE_TYPE) ApplyArray->Element->Data.Pointer;
			}

			SDB_NODE* Child = FindTerminal( SecPtr, NodeValueType, ChildArgc, NodeArgs.val( ArgGetAddr( 0 )), DbSetGet()->Db(), SHARE );
			bool Status = Child && Child->ParentLink( this );

			if( !Status )
			{
				*ChildGetAddr( 0 ) = NULL;
				return false;
			}
			*ChildGetAddr( 0 ) = Child;
			break;
		}
		case SDB_NODE_TYPE_ARRAY:
		{
		    DT_VALUE TmpVal;

			if( ObjectValue->DataType == DtSecurityList )
			{
				DT_SECURITY_LIST
						*SecPtrList = (DT_SECURITY_LIST *) ObjectValue->Data.Pointer;

				DecompSize	= SecPtrList->Size;

				SDB_NODE* ValueTypeNode = ValueTypeValue->DataType == DtVtApply
						? FindLiteral( ((DT_ARRAY *) ValueTypeValue->Data.Pointer)->Element, DbSetGet()->Db() ) : ValueTypeGet();

				if( !AdjustChildc( DecompSize ))
				    return false;

				bool Status = true;
				for( int Num = 0; Num < DecompSize; ++Num )
				{
					DTM_INIT( &TmpVal );
					TmpVal.DataType		= DtSecurity;
					TmpVal.Data.Pointer	= SecPtrList->SecPtrs[ Num ];
					if( TmpVal.Data.Pointer == 0 )
					{
						if( !PassErrorsGet() )
							Status = false;
						else
							DTM_INIT( &TmpVal );
					}

					SDB_NODE* ObjectNode = 0;
					SDB_NODE* Child      = 0;
					Status = Status && ( ObjectNode	= FindLiteral( &TmpVal, DbSetGet()->Db() ))
							&&         ( Child		= Find( ObjectNode, ValueTypeNode, ChildArgc, NodeArgs.val( ArgGetAddr( 0 )), CREATE, DbSetGet()->Db(), SHARE ));
					Status = Status && Child->ParentLink( this );

					if( !Status )
					{
						memset( ChildGetAddr( Num ), 0, sizeof( SDB_NODE * ) * ( ChildcGet() - Num ));
						return false;
					}
					*ChildGetAddr( Num ) = Child;
				}
			}
			else						// DtDiddleScopeList
										// FIX- Can make more efficient by directly finding the Children in the DiddleScopes
										// given by the elements of the list, instead of going through yet another passthru
			{
				DT_DIDDLE_SCOPE_LIST* DiddleScopeList = (DT_DIDDLE_SCOPE_LIST *) ObjectValue->Data.Pointer;

				DecompSize	= DiddleScopeList->Size;

				SDB_NODE* ValueTypeNode = ValueTypeValue->DataType == DtVtApply
						? FindLiteral( ((DT_ARRAY *) ValueTypeValue->Data.Pointer)->Element, DbSetGet()->Db() ) : ValueTypeGet();

				if( !AdjustChildc( DecompSize )) // only should happen if we ran out of memory!
				    return false;

				bool Status = true;
				for( int Num = 0; Num < DecompSize; ++Num )
				{
				    TmpVal = DiddleScopeList->DiddleScopeLists[ Num ];

					if( TmpVal.Data.Pointer == NULL )
					{
						if( !PassErrorsGet() )
							Status = false;
						else
							DTM_INIT( &TmpVal );
					}

					SDB_NODE* ObjectNode;
					SDB_NODE* Child = 0;
					Status = Status && ( ObjectNode	= FindLiteral( &TmpVal, DbSetGet()->Db() ))
							&&         ( Child		= Find( ObjectNode, ValueTypeNode, ChildArgc, NodeArgs.val( ArgGetAddr( 0 )), CREATE, DbSetGet()->Db(), SHARE ));
					Status = Status && Child->ParentLink( this );

					if( !Status )
					{
						memset( ChildGetAddr( Num ), 0, sizeof( SDB_NODE * ) * ( ChildcGet() - Num ));
						return false;
					}
					*ChildGetAddr( Num ) = Child;
				}
			}
			break;
		}
	} // switch( NodeTypeGet() )

	return true;
}



/****************************************************************
**	Routine: SDB_NODE::SubVfExpensive
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_NODE::SubVfExpensive(
	NODE_SUBVF_ARGS* args,
	bool&            res
)
{
	if( !sdb_async_results->async_servers_available() )
		return false;

	dt_value_var converted_vals;		// list of child values converted to arrays

	DT_ARRAY array;

	DT_VALUE array_val;
	array_val.DataType = DtArray;
	array_val.Data.Pointer = &array;


	array.Size = args->MsgData.GetValue.Childc;

	array.Element = (DT_VALUE*) malloc( sizeof( DT_VALUE ) * array.Size );

	for( int child = 0; child < args->MsgData.GetValue.Childc; ++child )
	{
		DT_VALUE* child_value = args->SubValues[ child ];
		if( child_value )
		{
			if( child_value->DataType == DtGsNodeValues ) // vector values, convert to array else it will be unstreamable
			{
				if( converted_vals->DataType != DtArray )
					converted_vals.reset( DtArray );

				// convert GsNodeValues to DtArray
				DT_VALUE tmp_val;
				tmp_val.DataType = DtArray;
				if( !DTM_TO( &tmp_val, child_value ) )
					return ErrMore( "SubVfExpensive: Error while streaming childvalues" );

				// Squirrel away in values to be freed
				DtAppendNoCopy( converted_vals.get(), &tmp_val );

				// now that we have a copy, whack it into the array
				array.Element[child] = tmp_val;
			}
			else
				array.Element[child] = *child_value;
		}
		else
			DTM_INIT( &array.Element[child] );
	}

	if( SecDbTraceFunc )
		SecDbTraceFunc( SDB_TRACE_SCHEDULE_ASYNC_GET, args->Node );

	//cout << "Scheduling request for { obj:" << args->ObjPtr->SecData.Name << ", vt:" << args->NodeVti->Name << " }: req_id:" << ((long) args->Node) << endl;

	res = sdb_async_results->schedule( args->Node, args->ObjPtr->Class->Type, args->NodeVti->Name, &array_val );

	free( array.Element );				// not exception safe
	return true;
}



/****************************************************************
**	Routine: SDB_NODE::SubVfChildValues
**	Returns: true/Err()
**	Action : Handle value functions that need only ChildValues
**			 Args->NodeVti is assumed to be valid
****************************************************************/

bool SDB_NODE::SubVfChildValues(
	NODE_SUBVF_ARGS* Args
)
{
	static SDB_VALUE_TYPE
		SecurityNameVt = SecDbValueTypeFromName( "Security Name", NULL ),
		SecurityTypeVt = SecDbValueTypeFromName( "Security Type", NULL );


	// In this case, the value function just wants a list of child values

    if( !Args->SetTarget )
	    return true;

	SDB_VALUE_TYPE_INFO* const
	        Vti = Args->NodeVti;

	SDB_NODE& Node = *(Args->Node);

	if( Vti )
	{
		bool res = true;
		if( ( Vti->ValueFlags & SDB_EXPENSIVE ) && SubVfExpensive( Args, res ) )
			return res;

	    Args->MsgData.GetValue.Value->DataType = Vti->ValueType->DataType;

		if( Vti->ValueFlags & SDB_AUTO_CHILD_LIST )
		{
		    s_CollectAutoChildren = true;
			Args->Node->DeleteAutoChildList();
		}
	}

	double  MemBefore,
			MemAfter,
			CpuBefore,
			CpuAfter;

	NODE_STATS* const stats = Node.StatsGet();

	if( stats )
	{
		MemBefore	= ProcessWorkingSetSizeGet();
		CpuBefore	= ProcessCpuTimeGet();
	}

	do
	{
	    Node.ChildDbChangedSet( false );

		// Call object function
		if( !SDB_MESSAGE_VT_TRACE( Args->ObjPtr, SDB_MSG_GET_VALUE, &Args->MsgData, Vti ))
		{
		    DTM_INIT( &Node.m_Value );
			return false;
		}
	}
	while( Node.ChildDbChangedGet() &&
		   Args->Node->DeleteAutoChildList() ); // We should only get a ChildDbChanged when we have autochildren.
	                                            // the autochildren must be reset each time we repeat

	if( stats )
	{
		DT_VALUE
				ResultMemSize;


		MemAfter	= ProcessWorkingSetSizeGet();
		CpuAfter	= ProcessCpuTimeGet();
/*
**	Now subtract out the size of the returned value. This may or may not be accurate for GsDts...
**	Can't do DT_MSG_MEMSIZE with Security Type/Name, as they aren't allocated off the heap in the normal way.
*/
		if( Args->NodeVti->ValueType != SecurityNameVt && Args->NodeVti->ValueType != SecurityTypeVt )
			if( DT_OP( DT_MSG_MEMSIZE, &ResultMemSize, Args->MsgData.GetValue.Value, NULL ))
				MemAfter -= DT_VALUE_TO_NUMBER( &ResultMemSize );

		stats->Add( max( MemAfter - MemBefore, 0. ), CpuAfter - CpuBefore );
	}

	// FIX-GEG Should this happen on each Vf call?
	Node.CacheFlagsSet( Args->MsgData.GetValue.CacheFlags & SDB_NODE_CACHE_FLAGS_MASK );

	return true;
}



/****************************************************************
**	Routine: SDB_NODE::SubVfChildData
**	Returns: true/Err()
**	Action : Handle value functions that need ChildData filled in
****************************************************************/

bool SDB_NODE::SubVfChildData(
	NODE_SUBVF_ARGS* Args
)
{
	char* const ChildData = (char *) Args->MsgData.GetValue.ChildData;


	// In this case, the value function wants all the child
	// values stuffed into a custom-made structure
	for( SDB_CHILD_PULL_VALUE* PullVal = Args->NodeVti->ChildList.PullValues; PullVal; PullVal = PullVal->Next )
	{
		DT_VALUE* const SubValue = Args->SubValues[ PullVal->ChildNumber ];
		if( SubValue )
		{
			if( SubValue->DataType->Size == sizeof( double ))
				*(double *)( ChildData + PullVal->Offset ) = SubValue->Data.Number;
			else
				*(void const **) ( ChildData + PullVal->Offset ) = SubValue->Data.Pointer;
		}
		else if( Args->SetErrorOnNull )
		{
			if( (*PullVal->DataType)->Flags & DT_FLAG_NUMERIC )
				*(double *)( ChildData + PullVal->Offset ) = HUGE_VAL;
			else
				*(void **)( ChildData + PullVal->Offset ) = NULL;
		}
	}

	return SDB_NODE::SubVfChildValues( Args );
}


/****************************************************************
**	Routine: SubVfArray
**	Returns: true/Err()
**	Action : Handle non-terminals of DtArrays
****************************************************************/

static bool SubVfArray(
	NODE_SUBVF_ARGS* Args
)
{
	DT_DATA_TYPE
	        Dt = ( Args->Node->NodeTypeGet() == SDB_NODE_TYPE_VALUE_ARRAY ? DtValueArray : DtArray );

	DT_VALUE
	        ArrayValue;

	if( !DTM_ALLOC( &ArrayValue, Dt ))
		return false;

	int const Size = Args->Node->ChildcGet();
	for( int Index = 0; Index < Size; ++Index )
	{
		DT_VALUE* SubValue = Args->SubValues[ Index ];
		if( SubValue )
		{
			// FIX - kludge, node should somehow know to only calculate the ith part of the value array
			if( SubValue->DataType == DtValueArray && Dt == DtValueArray )
			{
				SubValue = DtSubscriptGet( SubValue, Index, NULL );
				if( !SubValue )
					return false;
			}
			if( !DtAppend( &ArrayValue, SubValue ))
				return false;
		}
		else if( Args->SetErrorOnNull )
		{
			if( !DtAppendNumber( &ArrayValue, DtNull, 0 ))
				return false;
		}
		else
		{
			ErrMsg( ERR_LEVEL_ERROR, "SubVfArray, not done yet" );
			if( !DtAppendNumber( &ArrayValue, DtNull, 0 ))
				return false;
		}
	}

	if( Args->SetTarget )
		*Args->MsgData.GetValue.Value = ArrayValue;
	return true;
}


/****************************************************************
**	Routine: SubVfGsNodeValues
**	Returns: true/Err()
**	Action : Handle non-terminals of GsNodeValues
****************************************************************/

static bool SubVfGsNodeValues(
	NODE_SUBVF_ARGS* Args
)
{

	if( !Args->SetTarget)
		return true;

	DT_VALUE* vec_val = Args->MsgData.GetValue.Value;
	vec_val->DataType = DtGsNodeValues;
	vec_val->Data.Pointer = new SecDbVectorValue( Args->Node );

	return true;
}



/****************************************************************
**	Routine: MarkValidLinks
**	Returns: void
**	Action : Set valid link on node an all children/object/vt
**           recursively
****************************************************************/

void SDB_NODE::MarkValidLinks(
)
{
    if( ValidLinkGet() )
	    return;

	sdb_node_trace_scope_ctx< SDB_TRACE_VALID_LINK, SDB_TRACE_DONE_VALID_LINK > tracer( this, 0 );

	ValidLinkSet( true );

	if( !ObjectGet()->ValidGet() )
	    ObjectGet()->MarkValidLinks();

	if( !ValueTypeGet()->ValidGet() )
	    ValueTypeGet()->MarkValidLinks();

	if( VtiNodeGet() && !VtiNodeGet()->ValidGet() )
	    VtiNodeGet()->MarkValidLinks();

	if( ChildcGet() > 0 )
	{
		SDB_NODE** const SubNodeBound = ChildGetAddr( ChildcGet() - 1 );
		for( SDB_NODE** pSubNode = ChildGetAddr( 0 ); pSubNode <= SubNodeBound; ++pSubNode )
		{
		    SDB_NODE* const Child = SDB_NODE_FROM_PURPLE_CHILD( *pSubNode );
			if( Child && !Child->ValidGet())
			    Child->MarkValidLinks();
		}
	}
}


/****************************************************************
**	Routine: SDB_NODE::ReleaseChildValue
**	Returns: true/Err
**	Action : updates childs count of invalid parents. Releases
**			 value if necessary
****************************************************************/

bool SDB_NODE::ReleaseChildValue(
	SDB_NODE* Child
)
{
	int const NumLockedParents = InvParentsHashDec( Child );
	if( NumLockedParents == -1 )
	{
		Err( SDB_ERR_OBJECT_IN_USE, "ReleaseValue%s Child not found in the InvParentsHash",
			 Child->ReleaseValueNeedParentHashGet() ? "ParentHash" : "");
		return false;
	}

	if( !NumLockedParents )
	{
		Child->FreeValue();
		Child->m_Value.DataType = ReservedValidValue();
		Child->ValidLinkSet( true );	// make sure we can still invalidate its parents!
		if( SecDbTraceFunc )
			SecDbTraceFunc( SDB_TRACE_RELEASE_VALUE, Child, this );
	}
	return true;
}

/****************************************************************
**	Routine: SDB_NODE::ReleaseOneDescendent
**	Returns: true/Err
**	Action :
****************************************************************/

bool SDB_NODE::ReleaseOneDescendent(
	SDB_NODE* ChildNode
)
{
	// We will stop traversal if we find an invalid node.
	// The invalid node may be a node whose valid we have just released earlier
	// (a node that is shared by multiple parents).
	// The invalide node may also be SDB_RELEASE_VALUE node placed strategically
	// by the user to stop the recursion earlier.

	if( !ChildNode || !ChildNode->ValidGet() )
		return true;

	if( ChildNode->ReleaseValueByAncestorGet())
		if( !ReleaseChildValue( ChildNode ))
			return false;

	if( !ChildNode->ReleaseDescendentValues())
		return false;

	return true;
}

/****************************************************************
**	Routine: SDB_NODE::ReleaseDescendentValues
**	Returns: TRUE/Err
**	Action : Descend to and release values of children marked
**			 as release by ancestor
****************************************************************/

bool SDB_NODE::ReleaseDescendentValues(
)
{
	if( SecDbTraceFunc )
		SecDbTraceFunc( SDB_TRACE_RELEASE_DESCENDENT_VALUES, this );

	if( !ChildrenValidGet())
		return true;

	for( int Child = 0; Child < ChildcGet(); ++Child )
		if ( !ReleaseOneDescendent( ChildGet( Child )))
			return false;

	if ( !ReleaseOneDescendent( ObjectGet()))
		return false;

// FIX
// Also need to work on Arguments, etc
//	if ( ! ReleaseOneDescendent( ValueTypeGet() ) )
//		return FALSE;
//	if ( ! ReleaseOneDescendent( VtiNodeGet() ) )
//		return FALSE;

	if( SecDbTraceFunc )
		SecDbTraceFunc( SDB_TRACE_DONE_RELEASE_DESCENDENT_VALUES, this );

	return true;
}

/****************************************************************
**	Routine: SDB_NODE::collect_child_values
**	Returns: true/Err
**	Action : collects child values into an array
****************************************************************/

bool SDB_NODE::collect_child_values(
    SDB_NODE** const                first_child_addr, // first child
    SDB_NODE** const                last_child_addr,  // last child
    DT_VALUE** const                SubValues,        // array in which to fill in child values (allocated)
    auto_ptr_array<DT_VALUE_ARRAY*> &vec_values,      // array in which to fill in vec values
    DT_VALUE* const                 ellipsis,         // array in which to fill ellipsis args
    DT_VALUE* const                 vec_result        // array in which vector results will be collected
)
{
	DT_VALUE** pSubValue = SubValues;
	for( SDB_NODE** pSubNode = first_child_addr; pSubNode < last_child_addr; ++pSubNode, ++pSubValue )
	{
		SDB_NODE* Child;
		if( ( Child = SDB_NODE_FROM_PURPLE_CHILD( *pSubNode )) && Child->ValidGet() )
		{
			DT_VALUE* SubValue = &Child->m_Value;
			while( SubValue->DataType == DtPassThrough )
				SubValue = (DT_VALUE *) SubValue->Data.Pointer;
			
			if( SubValue->DataType == DtValueArray && ObjectGet() != SubscriptObject && ValueTypeGet() != s_ValueArrayEvalVT ) // Nasty hack to prevent recursion on subscripts of value arrays
			{
				// Do something special for vectors
				//		- check sizes
				//		- record exact position

				// If already in 'check size of each vector child'
				if( vec_result->DataType != DtNull )
				{
					if( DtSize( SubValue ) != DtSize( vec_result ) )
					{
						VectorSizeMismatchMessage();
						return false;
					}
				}

				// Else change to that state
				else
				{
					DT_VALUE tmp;
					tmp.DataType = DtDouble;
					tmp.Data.Number = DtSize( SubValue );
					vec_result->DataType = DtValueArray;
					if( !DTM_TO( vec_result, &tmp ) )
						return ErrMore( "SDB_NODE::collect_child_values: could not allocate a ValueArray for the results" );

					const int childc = ( last_child_addr - first_child_addr );

					typedef DT_VALUE_ARRAY* DT_VALUE_ARRAY_P; // kludge for inaudible pos SW4.2
					vec_values.reset( new DT_VALUE_ARRAY_P[ childc ] );
					memset( vec_values.get(), 0,  childc * sizeof( DT_VALUE_ARRAY * ));
				}

				// This is a Value Array, so store it in VecValues
				vec_values[ pSubValue - SubValues ] = (DT_VALUE_ARRAY *) SubValue->Data.Pointer;

				*pSubValue = SubValue; // Set this in case node is an alias so we can return the right type
			}
			else if( SubValue->DataType == DtEllipsis )
			{
				// make a DtArray and stuff all the extra arguments into it

				if( ellipsis->DataType != DtNull )
					return Err( ERR_UNKNOWN, "NodeGetValue: Already have ellipsis" );

				DTM_ALLOC( ellipsis, DtArray );

				int cur_arg = pSubNode - ChildGetAddr( 0 );
				if( cur_arg < ArgcGet() )
				{
					DtSubscriptSetNumber( ellipsis, ArgcGet() - cur_arg - 1, DtDouble, 0 ); // allocate elements
					for( int i = 0; cur_arg < ArgcGet(); ++cur_arg )
						DtSubscriptSet( ellipsis, i++, ArgGet( cur_arg )->AccessValue());
				}
				*pSubValue = ellipsis;
			}
			else
				*pSubValue = SubValue;
		}
		else
			*pSubValue = NULL;
	}
	return true;
}



/****************************************************************
**	Routine: SDB_NODE::GetValue
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_NODE::GetValue(
	SDB_NODE_CONTEXT	*Ctx,	// Random bullshit, should be scrapped
	SDB_DB				*Db		// Db in which we want the result
)
{
	// another problem is that if you have a pass errors above an async node and the async node fails, the pass error
	// is not respected. we need to fix this too.

	// Also this waits forever. if a client gets hosed, and the ORB never throws an exception back to us
	// we will hang. need to timeout, or ping the calc server or something....

	// we are synchronizing off this node
	Sdb_Async_Results::Synchronize_Guard guard = sdb_async_results->push_synchronizer( this );

	get_value_result res;
	while( is_res_async( res = GetValueInternal( Ctx, Db ) ) && !is_res_error( res ))
	{
		for( auto_ptr<Sdb_Async_Result> a_res( sdb_async_results->retrieve_result() );
			 a_res.get();
			 a_res.reset( sdb_async_results->try_retrieve_result() ) )
		{
			if( a_res->is_failure() )	// setup the initial error
				Err( ERR_UNKNOWN, "Async GetValue failed" );

			if( !a_res->apply_result() )
				return false;
		}
	}

	return !is_res_error( res );
	// The Synchronize_Guard will guarantee that unretreived results for this subgraph will be applied into
	// the graph
	// and that outstanding ones for this subgraph will be cancelled
}



/****************************************************************
**	Routine: SDB_NODE::GetValueInternal
**	Returns: true or false
**	Action : Makes the given node valid
****************************************************************/

SDB_NODE::get_value_result SDB_NODE::GetValueInternal(
	SDB_NODE_CONTEXT	*Ctx,	// Root node of the graph being calculated + enum hash, etc->
	SDB_DB				*Db		// Db in which we want the result
)
{
	SDB_TRACE_FUNC
			LocalTraceFunc;

	SDB_OBJECT
			*ObjPtr = NULL;				// Also in Args

	SDB_VALUE_TYPE_INFO
			*NodeVti = NULL;			// Also in Args

	NODE_SUBVF_ARGS
			Args;

	int		Childc;

	char	*ChildData = NULL;			// Also in Args

	DT_VALUE
			**ChildValues = NULL,		// Also in Args
			*TargetValue;

	dt_value_var
			Ellipsis,					// Variable arguments if any
			vec_result;					// Vector mode results

	SDB_NODE
			*SubNode,
			**pSubNode,
			**SubNodeBound = NULL;

	auto_ptr_array<DT_VALUE_ARRAY*>
			VecValues;

	DT_VALUE
			**SubValues = NULL;		// Also in Args

	int		Index;

	bool	HasChildrenWithReleaseValue = false,
			NodeHasReservedValue = false;

	bool (*SubValueFunc)( NODE_SUBVF_ARGS* ) = 0;

/*
**	Check for already locked node which would indicate a cycle.
*/

	if( this == SDB_NODE_INVALID_PTR )
		return RES_SYNC_ERROR;

	LocalTraceFunc = SecDbTraceFunc;
	if( !TraceFuncTest( Ctx, LocalTraceFunc, SDB_TRACE_GET, Db ))
		return RES_SYNC_ERROR;

	if( GetValueLockGet() )
	{
		TraceFunc( Ctx, LocalTraceFunc, SDB_TRACE_EVAL_END );
		Err( SDB_ERR_CYCLE_DETECTED, "@ - Fatal error during get value" );
		TraceFunc( Ctx, LocalTraceFunc, SDB_TRACE_ERROR );
		return RES_SYNC_ERROR;
	}
	else if( m_AsyncResult )
	{
		TraceFunc( Ctx, LocalTraceFunc, SDB_TRACE_EVAL_END );
		return RES_ASYNC_OK;
	}

#if defined( CHECK_FOR_CHANGE_WHILE_SET )		// Many value funcs diddle underlying values and re-get -- while not encouraged, still necessary
	else if( SetOrDiddleLock )
	{
		TraceFunc( Ctx, LocalTraceFunc, SDB_TRACE_EVAL_END );
		Err( SDB_ERR_OBJECT_IN_USE, "Using value while it is being set or diddled" );
		TraceFunc( Ctd, LocalTraceFunc, SDB_TRACE_ERROR );
		return RES_SYNC_ERROR;
	}
#endif

	if( m_AlwaysRecompute )
		FreeValue();

	/*
	**	All done if the node is valid
	*/
	if( ValidGet() )
	{
		if( CollectAutoChildren() )
		    s_CurrentAutoChildListNode->AddAutoChild( this );

		TraceFunc( Ctx, LocalTraceFunc, SDB_TRACE_VALID );
		return RES_SYNC_OK;
	}
	else if( DbSetGet()->Db() != Db )
	{
		// BuildChildren (or some nested buildchildren) could remove Db from the DbSet, so we force the primary Db to be Db
		// in case some other locked node depended on the old PrimaryDb, it will be fixed up
		// by RemoveDbFromParents
		DbSetSet( SDB_DBSET_SET_DB_PRIMARY( DbSetGet(), Db ));
	}

	// Clear the AutoGetNode state before we potentially recurse
	SDB_NODE::PushAutoGetNode // this is a nested type
	       Save( this );

	{
		// setup locks to guard against nested getvalues
		in_get_value __in_get_value( this );

		// Evaluate the node definition first (Object, VT and args)

		get_value_result res;

		if( is_res_error( res = ObjectGet()->GetValueInternalQuick( Ctx, Db ) ) ||
			is_res_error( res = ValueTypeGet()->GetValueInternalQuick( Ctx, Db ) & res ) ||
			( VtiNodeGet() && is_res_error( res = VtiNodeGet()->GetValueInternalQuick( Ctx, Db ) & res ) ) )
			goto ERROR_EXIT;

		if( ArgcGet() > 0 )
		{
			SubNodeBound = ArgGetAddr( ArgcGet() );
			for( pSubNode = ArgGetAddr( 0 ); pSubNode < SubNodeBound; ++pSubNode )
			{
				SDB_NODE *Child = SDB_NODE_FROM_PURPLE_CHILD( *pSubNode );
				if( is_res_error( res = Child->GetValueInternalQuick( Ctx, Db ) & res ) )
					goto ERROR_EXIT;
			}
		}

		// Check for modification of children while in use
		if( !ValidGet() )
		{
			Err( SDB_ERR_VALUE_CHANGED, "@ - Fatal error after getting arg values" );
			goto ERROR_EXIT;
		}

		if( is_res_async( res ))
		{
			ValidSet( false );
			return res;
		}


		// Build the childlist if not up-to-date
		// FIX- BuildChildren is not async
		if( !ChildrenValidGet() && !BuildChildren( Ctx ))
			goto ERROR_EXIT;
		else
			ChildDbChangedSet( false );
		
		
/*
**	Evaluate and collect the children
*/
		
		Childc = ChildrenNum( NodeVti );
		
		NodeHasReservedValue = IsReservedValidValue();
		
		if( Childc > 0 )
		{
			SDB_NODE
					*Child;
			
			SDB_DB	*ChildDb = ExpectedChildDb( Db );
			
			SubNodeBound = ChildGetAddr( Childc );
			do
			{
				ChildDbChangedSet( false ); // extra assignment. Unroll loop??
				for( pSubNode = ChildGetAddr( 0 ); pSubNode < SubNodeBound; ++pSubNode )
				{
					if(!( Child = SDB_NODE_FROM_PURPLE_CHILD( *pSubNode )))
						continue;
					
					if( is_res_error( res = Child->GetValueInternalQuick( Ctx, ChildDb ) & res ) )
					{
						if( PassErrorsGet() && IsErrorNonFatal( ErrNum ))
						{
							Child->MarkValidLinks();
							Ctx->ErrNoted = false;
							res = res | RES_ASYNC_OK;
							continue;
						}
						else
							break;			// error, only continue if ChildDbChanged
					}
					
					if( Child->ReleaseValueAnyFlagGet() )
					{
						if( Child->ReleaseValueGet() || Child->ReleaseValueWhenParentsValidGet() )
							HasChildrenWithReleaseValue = true;
						
						if( Child->ReleaseValueGet() && !NodeHasReservedValue )
							SDB_NODE::InvParentsHashInc( Child );
					}
				}
			}
			while( ChildDbChangedGet() );
			
			if( pSubNode < SubNodeBound )	// didn't finish validating all children
				goto ERROR_EXIT;			// and ChildDbChanged is false, so abort
			
			if( is_res_async( res ))
			{
				ValidSet( false );
				return res;
			}
			
			// Check for modification of children while in use
			if( !ValidGet() )
			{
				Err( SDB_ERR_CYCLE_DETECTED, "@ - Fatal Error while getting child values" );
				goto ERROR_EXIT;
			}
			
			// FIX- no need to do this loop unless some children are vectors
			// we should just be able to pass the node itself to the valuefunc which
			// can use accessors to pull out the child values
			
			// Do this as a separate loop, since the above is the major source
			// of recursion.  And we don't want to allocate SubValues until after
			// the recursion.
			SubValues = (DT_VALUE **) alloca( Childc * sizeof( DT_VALUE * ));
			if( !collect_child_values( ChildGetAddr(0), SubNodeBound, SubValues, VecValues, Ellipsis.get(), vec_result.get() ) )
				goto ERROR_EXIT;
		}
		
		
/*
**	Figure out which child function to call
*/
		
		//bool (*SubValueFunc)( NODE_SUBVF_ARGS* ) = 0;
		
		switch( NodeTypeGet() )
		{
			case SDB_NODE_TYPE_TERMINAL:
			{
				ObjPtr = (SDB_OBJECT *) ObjectGet()->AccessValue()->Data.Pointer;
				
				// NodeVti has already been computed if we are a terminal
				if( !NodeVti )
				{
					SDB_VALUE_TYPE
							Vt = (SDB_VALUE_TYPE) ValueTypeGet()->AccessValue()->Data.Pointer;
					
					
					Err( ERR_UNSUPPORTED_OPERATION, "Unsupported value type %s( %s )",
						 Vt->Name, ObjPtr->SecData.Name );
					goto ERROR_EXIT;
				}
				
				// If terminal is an Alias, then make it's value a pass thru to it's last child
				if( NodeVti->ValueFlags & SDB_ALIAS )
				{
					DT_VALUE* src_val = SubValues[ NodeVti->ChildList.TerminalCount - 1 ];
					if( src_val )
					{
						m_Value.DataType = DtPassThrough;
						if( !DTM_FROM_ONLY( &m_Value, src_val ))
							goto ERROR_EXIT;
					}
					else
						DTM_ALLOC( &m_Value, NodeVti->ValueType->DataType );
					goto OK_EXIT;
				}
				
				// Make space for passing children values to value function
				if( NodeVti->ChildList.ChildDataSize > 0 )
				{
					ChildData = (char *) alloca( NodeVti->ChildList.ChildDataSize );
					ChildValues  = NULL;
					SubValueFunc = SubVfChildData;
				}
				else if( Childc > 0 )
				{
					ChildData    = NULL;
					ChildValues  = SubValues;
					SubValueFunc = SubVfChildValues;
				}
				else
				{
					ChildData    = NULL;
					ChildValues  = NULL;
					SubValueFunc = SubVfChildValues;
				}
				break;
			}
			
			case SDB_NODE_TYPE_PASSTHRU:
			{
				SubNode = ChildGet( 0 );
				if( SubNode && SubNode->ValidGet() )
				{
					m_Value.DataType = DtPassThrough;
					if( !DTM_FROM_ONLY( &m_Value, &SubNode->m_Value ))
						goto ERROR_EXIT;
				}
				else
					DTM_INIT( &m_Value );
				
				goto OK_EXIT;
			}
			case SDB_NODE_TYPE_VALUE_ARRAY:
				SubValueFunc = SubVfArray;
				break;
			case SDB_NODE_TYPE_ARRAY:
			case SDB_NODE_TYPE_EACH:
				SubValueFunc = SubVfGsNodeValues;
//			SubValueFunc = SubVfArray;
				break;
				
			default:
				Err( SDB_ERR_OBJECT_INVALID, "@, NodeType[%d] unknown",
					 (int) NodeTypeGet() );
				goto ERROR_EXIT;
		}
		
		
/*
**	Collect the children and args and call the object function
*/
		
		// Figure out where result should go
		TargetValue = AccessTargetValue();
		if( !TargetValue )
		{
			ErrMore( "Software Bug: Could not find target value pointer" );
			goto ERROR_EXIT;
		}
		
		CacheFlagsSet( 0 );
		
		// Initialize the Args structure
		Args.Node			= this;
		Args.SubValues		= SubValues;
		Args.ObjPtr			= ObjPtr;
		Args.NodeVti		= NodeVti;
		Args.SetTarget		= true;
		Args.SetErrorOnNull	= true;
		
		// Set up data for object function
		Args.MsgData.GetValue.ChildData		= ChildData;
		Args.MsgData.GetValue.Argc			= ArgcGet();
		Args.MsgData.GetValue.CacheFlags	= 0;
		Args.MsgData.GetValue.Childc		= Childc;
		Args.MsgData.GetValue.ChildValues	= ChildValues;
		Args.MsgData.GetValue.Value			= TargetValue;
		Args.MsgData.GetValue.Db			= DbSetGet()->Db();
		Args.MsgData.GetValue.Node			= this;
		
		if( !VecValues.get() )				// not vector mode
		{
			DTM_FREE( TargetValue );	// FIX-GEG Probably not needed
			DTM_INIT( TargetValue );	// FIX-GEG Probably not needed
			
			try
			{
				if( !(*SubValueFunc)( &Args ))
					goto ERROR_EXIT;
			}
			catch( exception& e )
			{
				DTM_INIT( TargetValue );	// Value func fail so lets DTM_INIT just in case the VF changes the datatype
				Err( ERR_UNSUPPORTED_OPERATION, "@ NodeGetValue: caught exception '%s'", e.what() );
				goto ERROR_EXIT;
			}
			goto OK_EXIT;
		}
		else
		{
			//	Handle vector mode
			
			// FIX-GEG What happens to CacheFlags in vector mode?
			
			if( SubValueFunc == SubVfChildData )
			{
				// If in ChildData mode, do some special things
				
				// First, fill in all of the non-changing values
				Args.SetTarget = false;
				try
				{
					if( !(*SubValueFunc)( &Args ))
						goto ERROR_EXIT;
				}
				catch( exception&e )
				{
					Err( ERR_UNSUPPORTED_OPERATION, "@ NodeGetValue: caught exception '%s'", e.what() );
					goto ERROR_EXIT;
				}
				Args.SetTarget = true;
				Args.SetErrorOnNull	= false;
				
				// Next, erase all value pointers in SubValues, so they can
				// be reused to hold only the elements of the VecValues.
				memset( SubValues, 0, Childc * sizeof( DT_VALUE * ));
			}
			
			int const ChildVectorSize = DtSize( vec_result.get() );
			
			// TargetValue is now a Value Array
			TargetValue = vec_result.get();
			
			push_vec_index __push_vec_index;
			
			s_VectorContext->Index = ChildVectorSize - 1;
			s_VectorContext->Size  = ChildVectorSize;
			
			//	Now call the SubValue function for each vector element
			
			TargetValue = DtSubscriptGet( TargetValue, s_VectorContext->Size-1, NULL );
			
			for(	;
					s_VectorContext->Index >= 0;
					--(s_VectorContext->Index), --TargetValue )
			{
				DT_VALUE_ARRAY
						*VecValue;
				
				
				// Free Target value if needed
				if( TargetValue->DataType != DtNull )
				{
					DTM_FREE( TargetValue );
					DTM_INIT( TargetValue );
				}
				
				// Fix Adjust vector values
				// Set the values in SubValues to be VecValues[ NVC->Index ]>
				for( Index = 0; Index < Childc; ++Index )
					if( (VecValue = VecValues[ Index ] ))
						SubValues[ Index ] = &VecValue->Element[ s_VectorContext->Index ];
				
				Args.MsgData.GetValue.Value = TargetValue;
				try
				{
					if( !(*SubValueFunc)( &Args ))
						goto ERROR_EXIT;
				}
				catch( exception& e )
				{
					DTM_INIT( TargetValue );	// Value func fail so lets DTM_INIT just in case the VF changes the datatype
					Err( ERR_UNSUPPORTED_OPERATION, "@ NodeGetValue: caught exception '%s'", e.what() );
					goto ERROR_EXIT;
				}
			}
		}
		
/*
**	Clean up and unlock
*/
		
	  OK_EXIT:
		
		// Check for modification of children while in use
		if( !ValidGet() )
		{
			Err( SDB_ERR_OBJECT_IN_USE, "A value changed while using it as a child" );
			goto ERROR_EXIT;
		}
	}

	// FIX this when there are no gotos in this function (Save should be in its own scope)
	// and we should just be able to test s_CurrentAutoChildListNode at this point
	if( Save.CollectAutoChildren() )
	    Save.SavedGet()->AddAutoChild( this );

	if( m_AsyncResult )					// ValueFunc invoked async calculation
	{
		ValidSet( false );
		return RES_ASYNC_OK;
	}

	// Turn off the ValidLink flag if it is set, since the node is now valid and
	// therefore will pass invalidation up anyway
	ValidLinkSet( false );

	// If this node's value needs to be released when all parents nodes are computed,
	// remember the number of parents this node has.
	if( ReleaseValueNeedParentHashGet() )
	{
		int NumInvalidParents = CalcInvOrLockedParentsNum();
		SDB_NODE::InvParentsHashSet( this, NumInvalidParents );
	}

	if( HasChildrenWithReleaseValue && !NodeHasReservedValue )
	{
		// If Node has any child whose value needs to be released after Node recomputes,
		// then we must perform this clean up.  First check that all parents of the child
		// are valid, then we release child's value.

		for( pSubNode = ChildGetAddr( 0 ); pSubNode < SubNodeBound; ++pSubNode )
		{
			SDB_NODE *Child = SDB_NODE_FROM_PURPLE_CHILD( *pSubNode );
			if( !Child )
				continue;

			// Notice that if Child is used as both Arg and Child in the parent node,
			// The Child node will appear twice as children of parent node.
			// We can only release its value once.

			if( Child->ReleaseValueGet() || Child->ReleaseValueWhenParentsValidGet() )
				if( Child->ValidGet() )
					if( ! ReleaseChildValue( Child ))
						goto ERROR_EXIT;
		}
	}

	if( ReleaseDescendentValuesGet() )
	{
		// When we see the SDB_RELEASE_DESCENDENT_VALUES flag, we will go back to
		// children (and their descendents) that we have just computed, find descendents with
		// SDB_RELEASE_VALUE_BY_ANCESTOR nodes and release their values.
		// Without any intervention, a complete branch traversal is done.
		// You can, however, stop the traversal at arbitrary descendent node by marking it
		// with SDB_RELEASE_VALUE or SDB_RELEASE_VALUE_WHEN_PARENTS_VALID

		if ( ! ReleaseDescendentValues() )
			goto ERROR_EXIT;
	}

/*
**	Return
*/

	return RES_SYNC_OK;


ERROR_EXIT:

	NodeGetValueError( this, Ctx, LocalTraceFunc );

	{
		sdb_node_trace_scope_ctx< SDB_TRACE_INVALIDATE, SDB_TRACE_DONE_INVALIDATE > tracer( this, Ctx );
		ValidSet( false );
		if( !( CacheFlagsGet() & SDB_CACHE_NO_FREE ))
		    DTM_FREE( &m_Value );
		DTM_INIT( &m_Value );
	}

	if ( Save.CollectAutoChildren() )
	{
		MarkValidLinks();     // FIX?
		Save.SavedGet()->AddAutoChild( this );
	}
	if( SubscribedGet() )
		MarkValidLinks();

	return RES_SYNC_ERROR;
}


/****************************************************************
**	Routine: SDB_NODE::AccessTargetValue
**	Returns: &Node->Value or if DtPassThrough, the ultimate value
**	Action : Just like AccessValue, but don't check for DtNull
**			 because we are looking for an LValue.
****************************************************************/

DT_VALUE* SDB_NODE::AccessTargetValue(
)
{
	DT_VALUE* SubValue = &m_Value;

	while( SubValue->DataType == DtPassThrough )
		SubValue = static_cast< DT_VALUE* >( const_cast< void* >( SubValue->Data.Pointer ));

	if( !s_VectorContext || !SubValue || SubValue->DataType != DtValueArray )
	    return SubValue;

	DT_VALUE_ARRAY* const ValueArray = static_cast<DT_VALUE_ARRAY*>( const_cast< void* >( SubValue->Data.Pointer ));

	int const Index = s_VectorContext->Index;

	if( Index >= 0 && Index <= ValueArray->Size )
	    return &ValueArray->Element[ Index ];

	Err( ERR_INVALID_ARGUMENTS, "NodeAccessTargetValue, vector index[%d] out of range [0..%d]", Index, ValueArray->Size-1 );
	return 0;
}
