#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_obj.c,v 1.103 2014/09/30 21:22:10 c038571 Exp $"
// FIX - Change ~vt to simply Security or Security List cast node
/****************************************************************
**
**	SDB_OBJ.C	- Security database object helper functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_obj.c,v $
**	Revision 1.103  2014/09/30 21:22:10  c038571
**	AWR: #350874. When re-classifying a security, ensure that the security object is not removed too early.
**
**	Revision 1.102  2003/12/04 22:00:00  khodod
**	Introduced the DiddleScope Fix.
**
**	Revision 1.101  2001/09/24 22:15:00  simpsk
**	(bugfix) copy parent pred flags along with parent pred when merging.
**	
**	Revision 1.100  2001/08/16 21:01:35  simpsk
**	fixes for op!=
**	
**	Revision 1.99  2001/08/14 19:42:52  simpsk
**	fix compiler warning.
**	
**	Revision 1.98  2001/06/26 14:53:43  singhki
**	fix comment to pacify autodoc
**	
**	Revision 1.97  2001/06/25 16:28:54  singhki
**	make AsObject participate in CanShare of child component. Now
**	uniqueify intermediates used as securities after adding AsObject to
**	ensure that we don't magically convert types.
**	
**	This does mean that if a vt legitimately returning security is used as
**	both an object and as an arg we will have two child references for
**	it. but there isn't much we can do if people want this auto
**	conversion.
**	
**	Revision 1.96  2001/05/23 22:46:41  simpsk
**	fix for VC6 brain-damage.
**	
**	Revision 1.95  2001/05/23 22:32:59  simpsk
**	only merge predicates of terminals, then garbage collect intermediates.
**	
**	Revision 1.94  2001/05/21 20:05:39  simpsk
**	initial cut of intermediate garbage collection.
**	
**	Revision 1.93  2001/03/23 23:26:39  singhki
**	Make ValueTypeMap just an STL map.
**	
**	Revision 1.92  2001/03/15 16:39:11  simpsk
**	Moving around some stuff for predicates.
**	
**	Revision 1.91  2001/03/06 20:58:44  simpsk
**	Don't allow Aliases and Predicate children to play together - that would be BAD.
**	
**	Revision 1.90  2001/02/28 18:59:01  singhki
**	ignore CHECK_SUPPORTED flag in FindTerminal. SpecialVts are now
**	generated entirely via the get value of Class Value Func.
**	
**	Revision 1.89  2001/02/22 12:32:21  shahia
**	changed ValueTypeMap implementation to use STL map
**	
**	Revision 1.88  2000/12/19 23:36:26  simpsk
**	More cleanups.  Also added loop for autochildren for the dbchanged case.
**	
**	Revision 1.87  2000/12/19 17:20:44  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.86  2000/12/18 19:48:24  singhki
**	BugFix: Set of security type pointed class VTI of perserved nodes at
**	the old class. Added special arg to FindDefaultVtiNode to specify the
**	class object.
**	
**	Revision 1.85  2000/12/14 17:24:16  simpsk
**	Fixed predicate merging.
**	Fixed minor thinko for sucky compilers lacking a real bool type.
**	Lots and lots of comments.
**	
**	Revision 1.84  2000/12/12 17:39:01  simpsk
**	Predicated children.
**	
**	GOB_ARGs have been mostly reworked.
**	See example in Dev DB "KEN: predchild".
**	
**	Revision 1.83  2000/11/11 00:15:48  singhki
**	don't send the unload message if it is one of the special class
**	objects because it was never loaded
**	
**	Revision 1.82  2000/08/25 23:11:25  singhki
**	BaseDb may be NULL
**	
**	Revision 1.81  2000/08/23 01:07:18  singhki
**	Remove destroyed nodes from skiplist/literal hash when they are
**	marked. This prevents them from being found and referenced during
**	delayed destruction thus making the graph point to freed nodes.
**	
**	Also perpetuate hideous hack where literals have an implicit reference
**	to the secptr by making the secptr get freed only when its ref count
**	is -1. This should really fixed
**	
**	Revision 1.80  2000/04/24 11:04:23  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.79  2000/02/17 03:29:31  singhki
**	Destroy VTI along with Node when cleaning LRUCache.
**	
**	Revision 1.78  2000/02/11 21:48:51  singhki
**	SDB_OBJECTs now point to the SourceDb from which they were loaded. It
**	is only different if a Union/Qualified/NonPhysical Db.
**	
**	We use the new Trans APIs to get the disk info and then only load them
**	in the top database, as opposed to loading them in every level where
**	they were found.
**	
**	Now we no longer need to reload/removefromdeadpool in all members of a
**	union/qualified Db, since the load only happened at the top. Much
**	better, also no bogus failures.
**	
**	Revision 1.77  2000/02/10 22:32:50  singhki
**	Child ResultTypes are identified by a bit. Pass seperate flag to mark
**	child as terminal/intermediate
**	
**	Revision 1.76  2000/02/08 04:50:25  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.75  2000/02/01 22:54:18  singhki
**	Replace ValueTypeMap in SecPtr with the SDB_CLASS_INFO for the class
**	from which we should get VTIs. Class still points to the original
**	SDB_CLASS_INFO to be used for the ObjectFunc
**	
**	Revision 1.74  2000/01/31 21:22:13  singhki
**	Turn on LRU cache yet again. Hopefully it works now!
**	
**	Revision 1.73  1999/12/23 20:04:42  singhki
**	Add CreateIf arg to NodeFindDefaultVti so that we can respect a CreateIf spec in NodeSplitChildren
**	
**	Revision 1.72  1999/12/22 19:33:58  singhki
**	Add Object Value Func
**	
**	Revision 1.71  1999/12/17 20:53:29  singhki
**	BugFix: LRCacheDelete used slightly different logic to determine if the node was a terminal. Oops!
**	Also mark node as SetLocked in order to prevent it being destroyed while it is being inserted.
**	
**	Revision 1.70  1999/12/10 20:13:12  singhki
**	fix up reclassify to reassign VTI
**	
**	Revision 1.69  1999/12/09 17:41:18  singhki
**	Don't NULL Class until we actually free the Orphan
**	
**	Revision 1.68  1999/10/04 20:26:06  singhki
**	bugfix: specialvt must generate ref to vt
**	
**	Revision 1.67  1999/09/27 23:47:19  nauntm
**	fix DbId/SecDbRootDb stuff
**	
**	Revision 1.66  1999/09/24 19:08:21  singhki
**	remove from Db's CacheHash only when refcount goes to 0
**	
**	Revision 1.65  1999/09/24 16:58:06  singhki
**	Don't free if child list is not valid
**	
**	Revision 1.64  1999/09/22 01:10:07  singhki
**	Allocate & copy ChildList in DT_SLANG because it's lifetime is different from VTI
**	
**	Revision 1.63  1999/09/13 14:31:19  singhki
**	Use static buffers to build child list arrays and copy into real child list when done
**	
**	Revision 1.62  1999/09/09 14:19:00  singhki
**	NodeFindLiteral may fail
**	
**	Revision 1.61  1999/09/07 22:57:58  singhki
**	Fix up secptr freeing some more
**	
**	Revision 1.60  1999/09/02 22:04:22  singhki
**	Move SecPtrs which still have a ref count after Detach to the Orphans database
**	
**	Revision 1.59  1999/09/01 15:29:31  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.58  1999/09/01 01:55:13  singhki
**	Get rid of SDB_CHILD_REFs, split intermediates and terminals into seperate
**	arrays in SDB_CHILD_LIST. Rationalize NoEval stuff, auto mark args/ints as
**	NoEval if they are never evaluated.
**	Color child links as purple so that we don't have to find the VTI in order
**	to correctly unlink them.
**	Uncolor arg/int which aren't purple so that they won't invalidate the tree.
**	
**	Revision 1.57  1999/08/07 01:40:10  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.56  1999/07/29 18:21:47  singhki
**	Check vt name size
**	
**	Revision 1.55  1999/07/26 20:00:22  singhki
**	turn off LRU Cache by default
**	
**	Revision 1.54  1999/07/23 02:55:03  singhki
**	~VTs now use ellipsis in order to support arguments on the source VT
**	
**	Revision 1.53  1999/07/21 18:26:07  singhki
**	Turn on LruCache be default
**	
**	Revision 1.52  1999/06/29 18:52:50  singhki
**	oops, increment ref count of special vt otherwise it will be freed
**	
**	Revision 1.51  1999/06/28 23:19:19  singhki
**	Generate ~VTs during build children instead of in SecDbChildAdd
**	
**	Revision 1.50  1999/06/17 16:23:05  singhki
**	Add unparented nonterminals onto a NonTerminal object in order to clean them up
**	
**	Revision 1.49  1999/05/28 22:18:16  singhki
**	Must also check Node->Diddle to determine whether node is diddled
**	
**	Revision 1.48  1999/05/28 18:41:48  singhki
**	disable LRU cache by default until we get the bugs out
**	
**	Revision 1.47  1999/05/27 20:11:03  singhki
**	allow disabling of LRU cache
**	
**	Revision 1.46  1999/05/24 22:44:37  singhki
**	fix more nasty interactions between destroycommit & lrucache
**	
**	Revision 1.45  1999/05/24 16:43:59  singhki
**	Flush LRU cache before insert in order to prevent node going away
**	
**	Revision 1.44  1999/05/21 14:38:32  singhki
**	Oops, can't put non terminals on LRU cache
**	
**	Revision 1.43  1999/05/20 23:20:53  singhki
**	Add nodes with 0 parents to an LRU list on the node's object
**	The last n nodes of this list get thrown away when it reaches
**	MaxUnparentedNodes which is set on a class basis
**	
**	Revision 1.42  1999/05/17 21:05:42  singhki
**	Split CHILD_LIST_SELF into CHILD_LIST_SELF_VT & CHILD_LIST_SELF_OBJ
**	
**	Revision 1.41  1999/04/26 20:45:13  singhki
**	Strip nodeflags in an each
**	
**	Revision 1.40  1999/04/21 16:07:22  singhki
**	Stylistic changes in response to code review
**	
**	Revision 1.39  1999/04/16 23:53:20  singhki
**	Add NodeFlags in NodeFindChild based on Item flags set by SecDbChildAdd. Fixup noeval stuff to work better with each.
**	
**	Revision 1.38  1999/04/16 18:31:07  vengrd
**	Set the new type in SecPtr->SecData when reclassifying
**	
**	Revision 1.37  1999/04/16 16:33:33  vengrd
**	Completed change to Reclassify API
**	
**	Revision 1.36  1999/04/16 15:49:53  nauntm
**	change args to reclassify
**	
**	Revision 1.35  1999/04/08 20:37:11  singhki
**	Support NoEval flag to prevent evaluation of Intermediate/Cast children
**	
**	Revision 1.33  1999/04/06 22:50:36  singhki
**	Support parented literals of which SecPtrs are the only one.
**	All other literals are refcounted.
**	This allows proper cleanup of nodes using the SecPtr as args.
**	Tweak node parent hash table.
**	
**	Revision 1.32  1999/03/24 18:29:50  singhki
**	Add AllowMissingObjects only for SpecialVts
**	
**	Revision 1.31  1999/03/23 19:41:09  singhki
**	RefCount literal nodes so that we can free them
**	
**	Revision 1.30  1999/03/14 16:21:39  rubenb
**	change min/max to MIN/MAX
**	
**	Revision 1.29  1999/01/16 01:42:36  singhki
**	Add ArrayReturn hack if Db is not self
**	
**	Revision 1.28  1999/01/13 23:12:29  singhki
**	Add special DiddleScope child which implicitly depends on node's vt & args, clean up ellipsis stuff a bit
**	
**	Revision 1.27  1998/12/10 16:08:32  singhki
**	Strip NodeFlags from terminals on creation
**	
**	Revision 1.26  1998/12/09 01:17:54  singhki
**	Support Casting of VT Args
**	
**	Revision 1.25  1998/11/20 22:19:46  singhki
**	Add Size( Ellipsis() ) as a new child type
**	
**	Revision 1.24  1998/11/18 17:42:52  singhki
**	Add NodeIsValid & remove default args from NodeFind... to force explicit spec of Db & Share
**	
**	Revision 1.23  1998/11/16 22:59:58  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.22.4.2  1998/11/05 02:53:15  singhki
**	factored c++ stuff into internal header files
**	
**	Revision 1.22.4.1  1998/10/30 00:03:18  singhki
**	DiddleColors: Initial revision
**	
**	Revision 1.22  1998/06/09 12:36:45  lundek
**	Liberal (and correct) use of DTM_INIT (primarily to shush purify UMRs)
**	
**	Revision 1.21  1998/04/30 14:49:08  lundek
**	BugFix: Failure to match existing children in some cases (e.g., trades on cash settled forwards on os/2)
**	
**	Revision 1.20  1998/03/26 15:24:48  gribbg
**	Remove obsolete SecDbAutoExport()
**	
**	Revision 1.19  1998/01/02 21:33:04  rubenb
**	added GSCVSID
**	
**	Revision 1.18  1997/12/24 04:26:57  lundek
**	Preparing for new search path and qualified dbs
**	
**	Revision 1.17  1997/12/01 20:55:54  singhki
**	Added VTI to nodes and Ellipsis Arg type
**	
**	Revision 1.16  1997/07/25 19:21:05  lundek
**	revert 1.14 (solve bug in SecDbGetByName)
**	
**	Revision 1.15  1997/07/24 19:25:07  gribbg
**	BugFix: Handle objects with 0 length streams when a new InStream value
**	        is added.
**	
**	Revision 1.14  1997/07/24 16:06:53  lundek
**	BugFix: Database search paths requier that all GetByNames in service of the calc graph be gotten via SDB_ROOT_DATABASE
**
**	Revision 1.13  1997/07/15 03:19:46  lundek
**	Huge objects
**
**	Revision 1.12  1997/04/30 21:03:14  lundek
**	Allow casts in childlist
**
**	Revision 1.11  1997/04/30 03:24:07  lundek
**	Allow casts in childlists
**
**	Revision 1.10  1997/03/06 01:44:43  lundek
**	UFO enhancements and clean up
**
**	Revision 1.8  1996/09/11 02:53:55  gribbg
**	New Trees Merge
**
**	Revision 1.7.2.16  1996/08/08 01:07:53  lundek
**	Fix cvs commit comment
**
**	Revision 1.7.2.15  1996/08/08 01:02:40  lundek
**	Cleaning up
**
**	Revision 1.7.2.14  1996/08/05 20:06:42  lundek
**	Move PassErrors arg to end and make it omissible
**
**	Revision 1.7.2.13  1996/08/05 18:00:32  lundek
**	Make pass errors arg to non-terminals more descriptive
**
**	Revision 1.7.2.12  1996/08/02 13:10:07  schlae
**	Casts for NT
**
**	Revision 1.7.2.11  1996/08/01 22:16:56  lundek
**	ChildListItemRef->PassErrors; ~vt( self [, PassErrors ])
**
**	Revision 1.7.2.10  1996/07/30 21:11:00  lundek
**	BugFix: CHILD_LIST_ARG_OR_NULL
**
**	Revision 1.7.2.9  1996/07/30 04:11:17  lundek
**	BugFix: Name that argument/child
**
**	Revision 1.7.2.8  1996/07/30 00:57:31  lundek
**	Name that argument/child
**
**	Revision 1.7.2.7  1996/07/25 20:03:29  lundek
**	BugFixes: CHILD_LIST_CONSTANT
**
**	Revision 1.7.2.6  1996/07/24 22:16:25  lundek
**	BugFix: CHILD_LIST_CONSTANT
**
**	Revision 1.7.2.5  1996/07/19 16:57:03  lundek
**	Pass non-pulled childdata more explicitly
**
**	Revision 1.7.2.4  1996/07/18 15:13:38  lundek
**	Database independent childlists
**
**	Revision 1.7.2.3  1996/07/05 21:04:06  lundek
**	New, New Trees
**
**	Revision 1.7.2.2  1996/05/10 15:35:22  gribbg
**	Fix unix warnings
**
**	Revision 1.7.2.1  1996/05/10 11:54:36  gribbg
**	NewTrees: First snapshot
**
**	Revision 1.7  1996/01/02 23:25:24  ERICH
**	NT Compatibility
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>

#include	<secobj.h>
#include	<secexpr.h>

#include	<secdb.h>
#include	<secdrv.h>
#include	<secerror.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<sdb_int.h>

#include    <hash.h>
#include    <malloc.h>

#include    <vector>
#include    <ccstl.h>

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

/*
**	static member variables
*/

SDB_M_GET_CHILD_LIST::StateType SDB_M_GET_CHILD_LIST::State = SDB_M_GET_CHILD_LIST::Uninitialized;

SDB_NODE
		**SDB_M_GET_CHILD_LIST::Literals;

SDB_CHILD_LIST_ITEM
		*SDB_M_GET_CHILD_LIST::Terminals,
		*SDB_M_GET_CHILD_LIST::Intermediates;

SDB_CHILD_CAST
		*SDB_M_GET_CHILD_LIST::Casts;

short	SDB_M_GET_CHILD_LIST::MaxTerminals,
		SDB_M_GET_CHILD_LIST::MaxIntermediates,
		SDB_M_GET_CHILD_LIST::MaxCasts,
		SDB_M_GET_CHILD_LIST::MaxLiterals;


/*
**	No function header for the following because auto doc doesn't understand it
*/

inline bool CanShare(
	SDB_CHILD_LIST_ITEM const& Item1,
	SDB_CHILD_LIST_ITEM const& Item2
)
{
	if( Item1.Db != Item2.Db || Item1.Security != Item2.Security || Item1.ValueType != Item2.ValueType
		|| Item1.Result.Type != Item2.Result.Type || Item1.Argc != Item2.Argc
		|| Item1.ArrayReturn != Item2.ArrayReturn || Item1.PassErrors != Item2.PassErrors || Item1.AsObject != Item2.AsObject
		|| Item1.Name != Item2.Name )
	{
		return FALSE;
	}
	return Item1.Argv == 0 || memcmp( Item1.Argv, Item2.Argv, Item1.Argc * sizeof( *Item1.Argv )) == 0;
}

inline int MergePredicateParents(
    SDB_M_GET_CHILD_LIST* ChildList,
	SDB_CHILD_LIST_ITEM& ExistingNode,
	SDB_CHILD_LIST_ITEM& NewNode
)
{
    // check to see if the old predicate is a parent of the new predicate
	// ( AB + A -> A )
	SDB_CHILD_LIST_ITEM* Parent = &NewNode;
	while( Parent->Pred.Type != SDB_CHILD_LIST_NO_PREDICATE )
	{
	    if( Parent->Pred == ExistingNode.Pred ) // if one of the new node's ancestors is the same predicate
		{
		    if( Parent->PredFlags == ExistingNode.PredFlags ) // if they match ( AB + A -> A )
			{
			    NewNode.Pred      = ExistingNode.Pred;
				NewNode.PredFlags = ExistingNode.PredFlags;
				return 1;
			}
			return -1; // nothing can be done for now ( !AB + A -> !AB + A )
		}
		Parent = ChildList->FindChild( Parent->Pred );
		if( !Parent )
		    return 0;
	}
	return 0;
}

inline bool MergePredicateSiblings(
    SDB_M_GET_CHILD_LIST* ChildList,
	SDB_CHILD_LIST_ITEM& ExistingNode,
	SDB_CHILD_LIST_ITEM& NewNode
)
{
    // if both nodes use the same predicate
	if( ExistingNode.Pred == NewNode.Pred )
	{
	    // if the nodes don't want the same truth of the predicate
	    if( ExistingNode.PredFlags != NewNode.PredFlags )
		{
		    // remove the top-level predicate ( BA + B!A -> B )
		    SDB_CHILD_LIST_ITEM* PredItem = ChildList->FindChild( ExistingNode.Pred );
			if( !PredItem )
			{
			    NewNode.Pred.Type = ExistingNode.Pred.Type = SDB_CHILD_LIST_NO_PREDICATE;
			}
			else
			{
				NewNode.Pred      = ExistingNode.Pred      = PredItem->Pred;
				NewNode.PredFlags = ExistingNode.PredFlags = PredItem->PredFlags;
			}
		}
		return true; // share the node
	}
	return false;
}

/*
** Attempts to merge these two similar nodes by adjusting their predicates.
** Precondition: CanShare( ExistingNode, NewNode )
** Returns: true if the nodes can be adjusted to share the same predicate,
**          false otherwise
** Postcondition: if returning true, ExistingNode and NewNode share the same predicate
**                if returning false, ExistingNode and NewNode are unchanged
*/
inline bool IdenticalPredicates(
    SDB_CHILD_LIST_ITEM const& ExistingNode,
	SDB_CHILD_LIST_ITEM const& NewNode
)
{
    if( (ExistingNode.Pred.Type == SDB_CHILD_LIST_NO_PREDICATE) &&
		(NewNode.Pred.Type      == SDB_CHILD_LIST_NO_PREDICATE))
	    return true;

	return ( (ExistingNode.Pred.Type == NewNode.Pred.Type ) &&
			 (ExistingNode.Pred      == NewNode.Pred      ) &&
			 (ExistingNode.PredFlags == NewNode.PredFlags ));
}

inline bool MergePredicates(
	SDB_M_GET_CHILD_LIST* ChildList,
    SDB_CHILD_LIST_ITEM& ExistingNode,
	SDB_CHILD_LIST_ITEM& NewNode
)
{
    // if either node does not use predicates,
    // share the node and remove any predicates ( 1 + A -> 1 )
    if( (ExistingNode.Pred.Type == SDB_CHILD_LIST_NO_PREDICATE) ||
		(NewNode.Pred.Type      == SDB_CHILD_LIST_NO_PREDICATE))
	{
	    NewNode.Pred.Type = ExistingNode.Pred.Type = SDB_CHILD_LIST_NO_PREDICATE;
		return true;
	}

	// we only get to this point if both nodes use predicates

	// check if the nodes are siblings
	if( MergePredicateSiblings( ChildList, ExistingNode, NewNode ))
	    return true;

	// check to see if the old predicate is a parent of the new predicate
	{
	    int const ii = MergePredicateParents( ChildList, ExistingNode, NewNode );
		if( ii != 0 )
		    return (ii == 1);
	}

	// check to see if the new predicate is a parent of the old predicate
	{
	    int const ii = MergePredicateParents( ChildList, NewNode, ExistingNode );
		if( ii != 0 )
		    return (ii == 1);
	}

	// ok, we get here only if we have two nodes that use very different predicates.
	// punt - we don't know or don't care to share these nodes, they're too weird
	return false;
}

inline bool MergedPredicates(
	SDB_M_GET_CHILD_LIST* ChildList,
	SDB_CHILD_LIST_ITEM& ExistingNode,
	SDB_CHILD_LIST_ITEM& NewNode
)
{
    if( !CanShare( ExistingNode, NewNode ))
	    return false;

	if( ( ExistingNode.Pred.Type == SDB_CHILD_LIST_NO_PREDICATE ) &&
		( NewNode.Pred.Type      == SDB_CHILD_LIST_NO_PREDICATE ))
	    return false; // nothing to do

    if( ( ExistingNode.Pred      == NewNode.Pred ) &&
		( ExistingNode.PredFlags == NewNode.PredFlags ))
	    return false; // nothing to do

	return MergePredicates( ChildList, ExistingNode, NewNode );
}


/****************************************************************
**	Routine:	SecDbValueFuncGetSecurity
**	Returns:	TRUE or FALSE indicating success or failure, resp.
**
**	Value Type:	~<Child>( obj )
**
**	Children:	<Child>( obj )
**
**	Action:		GetSecurity( <Child>( obj ))		if <Child> is a String
**				GetSecurityList( <Child>( obj ))	if <Child> is an Array
**	
****************************************************************/

int SecDbValueFuncGetSecurity(
	SDB_OBJECT			*SecPtr,
	int					Msg,
	SDB_M_DATA			*MsgData,
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo
)
{
	switch( Msg )
	{
		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup(( ValueTypeInfo->ValueType->DataType == DtSecurity ) ? "GetSecurity( SecurityName )" : "GetManySecurities( SecurityNames )" );
			return TRUE;

		case SDB_MSG_GET_CHILD_LIST:
		{
			// Add arg for AllowMissingObjects
			{
			    DT_VALUE False;
				DTM_INIT( &False );
				False.DataType				= DtDouble;
				False.Data.Number			= FALSE;

				SDB_CHILD_LIST_ITEM Item;
				Item.Security.Type			= SDB_CHILD_LIST_CONSTANT;
				Item.Security.Element		= MsgData->GetChildList.LiteralAdd( &False, NULL );

				Item.Name					= "AllowMissingObjects";
				Item.Result.Type			= SDB_CHILD_LIST_ARGUMENT;
				if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				    return FALSE;
			}

			// Add ... for rest of args
			{
			    SDB_CHILD_LIST_ITEM Item;
				Item.Result.Type			= SDB_CHILD_LIST_ELLIPSIS;
				Item.Name					= "RestArgs";
				if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				    return FALSE;
			}

			// Add child for <ValueType>( Self, ... )
			{
			    DT_VALUE ValueTypeVal;
				DTM_INIT( &ValueTypeVal );
				ValueTypeVal.DataType		= DtValueType;
				ValueTypeVal.Data.Pointer	= (SDB_VALUE_TYPE) ValueTypeInfo->ValueData;

			    SDB_CHILD_COMPONENT ItemArg;
				ItemArg.Type                = SDB_CHILD_LIST_ELLIPSIS;

			    SDB_CHILD_LIST_ITEM Item;
				Item.Name					= ( ValueTypeInfo->ValueType->DataType == DtSecurity ) ? "SecurityName" : "SecurityNames";
				Item.ValueType.Type			= SDB_CHILD_LIST_LITERAL;
				Item.ValueType.Element		= MsgData->GetChildList.LiteralAdd( &ValueTypeVal, DtValueType );

				Item.Argc					= 1;
				Item.Argv					= &ItemArg;
				Item.Result.Type			= SDB_CHILD_LIST_TERMINAL;

				if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				    return FALSE;
			}
			
			return TRUE;
		}

		case SDB_MSG_GET_VALUE:
		{
		    bool AllowMissingObjects = ( FALSE != MsgData->GetValue.ChildValues[ 0 ]->Data.Number ); // be careful - some compilers don't have a real bool

			DT_VALUE* const Child = MsgData->GetValue.ChildValues[ 2 ];
			if( Child->DataType == DtString && ValueTypeInfo->ValueType->DataType == DtSecurity )
			{
				char const* const SecName = static_cast<char const*>( Child->Data.Pointer );

				if( !*SecName )
					return Err( ERR_INVALID_ARGUMENTS, "%s( %s ), SecName is ''", ValueTypeInfo->Name, SecPtr->SecData.Name );

				SDB_OBJECT* const ChildSecPtr = SecDbGetByName( SecName, SecPtr->Db, 0 );
				if( !ChildSecPtr && !AllowMissingObjects )
					return FALSE;

				MsgData->GetValue.Value->DataType = DtSecurity;
				MsgData->GetValue.Value->Data.Pointer = ChildSecPtr;

				return TRUE;
			}

			if(( Child->DataType == DtArray || Child->DataType == DtStructure ) && ValueTypeInfo->ValueType->DataType == DtSecurityList )
				return SecDbGetMany( SecPtr->Db, 0, Child, MsgData->GetValue.Value, AllowMissingObjects );

			return Err( ERR_INVALID_ARGUMENTS, "Cannot convert a %s to a %s", Child->DataType->Name, ValueTypeInfo->ValueType->DataType->Name );
		}
	}
	return TRUE;
}



/****************************************************************
**	Routine: ValueTypeSpecialVtGen
**	Returns: TRUE/Err
**	Action : 
****************************************************************/

int ValueTypeSpecialVtGen(
	SDB_NODE	*ValueTypeNode
)
{
	SDB_VALUE_TYPE ValueType = (SDB_VALUE_TYPE) ValueTypeNode->m_Value.Data.Pointer;
	if( ValueType->IsSpecialVt )
		return Err( ERR_INVALID_ARGUMENTS, "Can't turn a special VT into another special VT" );

	if( ValueType->SpecialVt )
		return TRUE;

	DT_DATA_TYPE DataType;

	if( ValueType->DataType == DtArray || ValueType->DataType == DtStructure )
		DataType = DtSecurityList;
	else if( ValueType->DataType == DtString )
		DataType = DtSecurity;
	else
		return Err( ERR_INVALID_ARGUMENTS, "Cannot turn %s into Security", ValueType->DataType->Name );

	if( strlen( ValueType->Name ) >= SDB_VALUE_NAME_SIZE - 1 )
		return Err( ERR_INVALID_ARGUMENTS, "ValueType %s is too long to generate a ~VT for", ValueType->Name );

	char Name[ SDB_VALUE_NAME_SIZE ];
	sprintf( Name, "~%s", ValueType->Name ); // hello buffer overflow!

	SDB_VALUE_TYPE SpecialVt = SecDbValueTypeFromName( Name, DataType );
	if( !SpecialVt )
		return FALSE;

	{
	    DT_VALUE TmpVal;
		TmpVal.DataType = DtValueType;
		TmpVal.Data.Pointer = SpecialVt;
		ValueType->SpecialVt = SDB_NODE::FindLiteral( &TmpVal, SecDbRootDb );
	}
	// Generate fake reference to special Vt in order to prevent it from being freed
	ValueType->SpecialVt->LiteralRefsIncrement();

	SpecialVt->IsSpecialVt = TRUE;
	SpecialVt->SpecialVt = ValueTypeNode;
	// Generate fake reference to Vt Node from Special Vt in order to prevent it being freed
	SpecialVt->SpecialVt->LiteralRefsIncrement();

	return TRUE;
}



/****************************************************************
**	Routine: ClassSpecialVtGen
**	Returns: TRUE/Err
**	Action : Ensures that the clas for the object has the ~x VT
**			 registered
****************************************************************/

int ClassSpecialVtGen(
	SDB_OBJECT		*Object,
	SDB_VALUE_TYPE	ValueType
)
{
	SDB_VALUE_TYPE_INFO
			*SpVti;

	if( Object->SdbClass->LookupVti( ValueType->ID ) )
		return TRUE;

	if( !ValueType->IsSpecialVt )
	{
		if( *ValueType->Name == '~' )
		{
			DT_VALUE TmpVal;
			TmpVal.DataType = DtValueType;
			TmpVal.Data.Pointer = SecDbValueTypeFromName( ValueType->Name + 1, NULL );
			if( !ValueTypeSpecialVtGen( SDB_NODE::FindLiteral( &TmpVal, SecDbRootDb ) ) || !ValueType->IsSpecialVt )
				return ErrMore( "ClassSpecialVtGen: Could could make value type special" );
		}
		else
			return Err( ERR_INVALID_ARGUMENTS, "ValueType %s is not a special Vt", ValueType->Name );
	}

	if( !ERR_CALLOC( SpVti, SDB_VALUE_TYPE_INFO, 1, sizeof( *SpVti )))
		return FALSE;

	SpVti->Func			= SecDbValueFuncGetSecurity;
	SpVti->ValueFlags	= SDB_CALCULATED;
	SpVti->ValueType	= ValueType;
	SpVti->Name			= ValueType->Name;
	SpVti->ValueData	= (SDB_VALUE_TYPE) ValueType->SpecialVt->m_Value.Data.Pointer;

	(*Object->SdbClass->ValueTypeMap)[ ValueType->ID ] = SpVti;

	return TRUE;
}



/****************************************************************
**	Routine: SDB_CHILD_LIST::Free
**	Returns: 
**	Action : 
****************************************************************/

void SDB_CHILD_LIST::Free(
)
{
	int		Num;

	SDB_CHILD_PULL_VALUE
			*PullVal;

	if( !ChildListValid )
		return;

	for( Num = 0; Num < LiteralCount; ++Num )
		LiteralUnlink( Literals[ Num ]);

	for( Num = 0; Num < TerminalCount; ++Num )
		free( Terminals[ Num ].Argv );

	for( Num = 0; Num < IntermediateCount; ++Num )
		free( Intermediates[ Num ].Argv );

	free( Literals );
	free( Terminals );
	free( Intermediates );
	free( Casts );

	while(( PullVal = PullValues ))
	{
		PullValues = PullVal->Next;
		free( PullVal );
	}
}



/****************************************************************
**	Routine: SDB_CHILD_LIST::LiteralLink
**	Returns: 
**	Action : 
****************************************************************/

void SDB_CHILD_LIST::LiteralLink(
	SDB_NODE	*Literal
)
{
	// Establish a reference to the literal
	if( Literal->RefCountedParents() )
		Literal->LiteralRefsIncrement();
	else if( Literal->m_Value.DataType == DtSecurity ) // else take out a ref to the secptr
		// This really is a hack because if the database is reloaded it will trash the literal anyway
		// but it's better than not doing it ;-) and it's no worse than it used to be
		// it really should be fixed though
		SecDbIncrementReference( (SDB_OBJECT *) Literal->m_Value.Data.Pointer );
}



/****************************************************************
**	Routine: SDB_CHILD_LIST::LiteralUnlink
**	Returns: 
**	Action : 
****************************************************************/

void SDB_CHILD_LIST::LiteralUnlink(
	SDB_NODE	*Literal
)
{
	if( Literal->RefCountedParents() )
		Literal->LiteralRefsDecrement();
	else if( Literal->m_Value.DataType == DtSecurity )
		SecDbFree( (SDB_OBJECT *) Literal->m_Value.Data.Pointer );	
}



/****************************************************************
**	Routine: SDB_CHILD_LIST::CopyItems
**	Returns: 
**	Action : 
****************************************************************/

SDB_CHILD_LIST_ITEM *SDB_CHILD_LIST::CopyItems(
	SDB_CHILD_LIST_ITEM	*Source,
	int					Count
)
{
	if( !Count )
		return NULL;

	SDB_CHILD_LIST_ITEM *Dest = (SDB_CHILD_LIST_ITEM *) malloc( sizeof( SDB_CHILD_LIST_ITEM ) * Count );
	for( ; Count-- > 0; )
	{
		SDB_CHILD_LIST_ITEM *Item = &Dest[ Count ];

		*Item = Source[ Count ];
		if( Item->Argc )
		{
			Item->Argv = (SDB_CHILD_COMPONENT *) malloc( sizeof( SDB_CHILD_COMPONENT ) * Item->Argc );
			memcpy( Item->Argv, Source[ Count ].Argv, sizeof( SDB_CHILD_COMPONENT ) * Item->Argc );
		}
	}
	return Dest;
}



/****************************************************************
**	Routine: SDB_CHILD_LIST::Copy
**	Returns: 
**	Action : 
****************************************************************/

int SDB_CHILD_LIST::Copy(
	SDB_CHILD_LIST	&Source
)
{
	int	Num;

	memset( this, 0, sizeof( *this ));

	TerminalCount		= Source.TerminalCount;
	IntermediateCount	= Source.IntermediateCount;
	ArgChildCount		= Source.ArgChildCount;
	LiteralCount		= Source.LiteralCount;
	CastCount			= Source.CastCount;
	PassErrors			= Source.PassErrors;
	ForceExport			= Source.ForceExport;
	ArgEllipsis			= Source.ArgEllipsis;
	ChildListValid		= Source.ChildListValid;

	Terminals = CopyItems( Source.Terminals, TerminalCount );
	Intermediates = CopyItems( Source.Intermediates, IntermediateCount );

	if(  LiteralCount )
	{
		Literals = (SDB_NODE **) malloc( sizeof( SDB_NODE * ) * LiteralCount );
		for( Num = 0; Num < LiteralCount; ++Num )
			LiteralLink( Literals[ Num ] = Source.Literals[ Num ]);
	}

	if( CastCount )
	{
		Casts = (SDB_CHILD_CAST *) malloc( sizeof( *Casts ) * CastCount );
		for( Num = 0; Num < CastCount; ++Num )
			Casts[ Num ] = Source.Casts[ Num ];
	}
	// FIX- Should also copy the pull values here

	return TRUE;
}



//**************************************************************
//	Routine: SDB_CHILD_LIST::Copy
//	Returns: 
//	Action : 
//**************************************************************

int SDB_CHILD_LIST::Copy(
	SDB_M_GET_CHILD_LIST	*ChildList
)
{
	int	Num;

	memset( this, 0, sizeof( *this ));

	TerminalCount		= ChildList->Vti->ChildList.TerminalCount;
	IntermediateCount	= ChildList->Vti->ChildList.IntermediateCount;
	ArgChildCount		= ChildList->Vti->ChildList.ArgChildCount;
	LiteralCount		= ChildList->Vti->ChildList.LiteralCount;
	CastCount			= ChildList->Vti->ChildList.CastCount;
	PassErrors			= ChildList->Vti->ChildList.PassErrors;
	ForceExport			= ChildList->Vti->ChildList.ForceExport;
	ArgEllipsis			= ChildList->Vti->ChildList.ArgEllipsis;
	ChildListValid		= ChildList->Vti->ChildList.ChildListValid;

	Terminals = CopyItems( SDB_M_GET_CHILD_LIST::Terminals, TerminalCount );
	Intermediates = CopyItems( SDB_M_GET_CHILD_LIST::Intermediates, IntermediateCount );

	if(  LiteralCount )
	{
		Literals = (SDB_NODE **) malloc( sizeof( SDB_NODE * ) * LiteralCount );
		for( Num = 0; Num < LiteralCount; ++Num )
			LiteralLink( Literals[ Num ] = SDB_M_GET_CHILD_LIST::Literals[ Num ]);
	}

	if( CastCount )
	{
		Casts = (SDB_CHILD_CAST *) malloc( sizeof( *Casts ) * CastCount );
		for( Num = 0; Num < CastCount; ++Num )
			Casts[ Num ] = SDB_M_GET_CHILD_LIST::Casts[ Num ];
	}
	// FIX- Should also copy the pull values here

	return TRUE;
}



/****************************************************************
**	Routine: SDB_M_GET_CHILD_LIST::CastAdd
**	Returns: Index into Casts Array or -1 for error
**	Action : Adds a cast to the child list structure
****************************************************************/

short SDB_M_GET_CHILD_LIST::CastAdd(
	SDB_CHILD_CAST	*Cast
)
{
	int	CastNum;

	for( CastNum = 0; CastNum < Vti->ChildList.CastCount; ++CastNum )
		if( Casts[ CastNum ] == *Cast )
			return CastNum;

	if( CastNum >= MaxCasts )
	{
		MaxCasts *= 2;
		if( !ERR_REALLOC( Casts, SDB_CHILD_CAST, MaxCasts * sizeof( *Cast )))
			return -1;
	}
	++Vti->ChildList.CastCount;
	Casts[ CastNum ] = *Cast;

	SDB_CHILD_LIST_ITEM *Items = Cast->Desc.Type == SDB_CHILD_LIST_ARGUMENT ? Terminals
			: ( Cast->Desc.Type == SDB_CHILD_LIST_INTERMEDIATE ? Intermediates : NULL );

	if( Items && !Cast->Desc.NoEval )
		Items[ Cast->Desc.Element ].Result.NoEval = FALSE;

	return CastNum;
}



/****************************************************************
**	Routine: SDB_M_GET_CHILD_LIST::LiteralAdd
**	Returns: Index into Literal Array or -1 for error
**	Action : Adds a Literal to the child list structure
****************************************************************/

short SDB_M_GET_CHILD_LIST::LiteralAdd(
	DT_VALUE		*LiteralValue,
	DT_DATA_TYPE	ExpectedType
)
{
	SDB_NODE
			*Literal = NULL;

	int		LitNum;


	// Convert the LiteralValue to a Literal Node

	// If the expected type is a VT, then convert strings to value types in order to save time during build children
	if( ExpectedType == DtValueType )
	{
		if( LiteralValue->DataType == DtString )
		{
			DT_VALUE
					VtValue;

			SDB_VALUE_TYPE
					Vt;

			if( !( Vt = SecDbValueTypeFromName( (char *) LiteralValue->Data.Pointer, NULL )))
				return -1;

			VtValue.Data.Pointer	= Vt;
			VtValue.DataType		= DtValueType;

			Literal = SDB_NODE::FindLiteral( &VtValue, SecDbRootDb );
		}
		else if( !( LiteralValue->DataType->Flags & DT_FLAG_VALUE_TYPE ))
		{
			Err( ERR_INVALID_ARGUMENTS, "SecDb cannot turn datatype %s into Value Type", LiteralValue->DataType->Name );
			return -1;
		}
	}
	if( !Literal && !( Literal = SDB_NODE::FindLiteral( LiteralValue, SecDbRootDb )))
		return -1;

	// See if we already have this literal
	for( LitNum = 0; LitNum < Vti->ChildList.LiteralCount; LitNum++ )
		if( Literal == Literals[ LitNum ] )
			return LitNum;

	++Vti->ChildList.LiteralCount;
	if( LitNum >= MaxLiterals )
	{
		MaxLiterals *= 2;
		if( !ERR_REALLOC( Literals, SDB_NODE *, MaxLiterals * sizeof( *Literal )))
			return -1;
	}

	Literals[ LitNum ] = Literal;
	Vti->ChildList.LiteralLink( Literal );
	return LitNum;
}



/****************************************************************
**	Routine: SDB_M_GET_CHILD_LIST::CheckDesc
**	Returns: TRUE/Err
**	Action : Checks if Desc is a valid Child Component
****************************************************************/

bool SDB_M_GET_CHILD_LIST::CheckDesc(
	SDB_CHILD_COMPONENT	*Desc,
	bool		   		NoEval
)
{
	switch( Desc->Type )
	{
		case SDB_CHILD_LIST_ARGUMENT:
			if( !NoEval && Desc->Element >= 0 )
				Terminals[ Desc->Element ].Result.NoEval = false;
			if( Desc->Element == -1 )
				return Err( ERR_INVALID_ARGUMENTS, "ChildAdd(): ChildComponent::Elemement is invalid1" );
			break;

		case SDB_CHILD_LIST_INTERMEDIATE:
			if( !NoEval && Desc->Element >= 0 )
				Intermediates[ Desc->Element ].Result.NoEval = false;
			if( Desc->Element == -1 )
				return Err( ERR_INVALID_ARGUMENTS, "ChildAdd(): ChildComponent::Elemement is invalid2" );
			break;

		case SDB_CHILD_LIST_LITERAL:
		case SDB_CHILD_LIST_TERMINAL:
		case SDB_CHILD_LIST_CONSTANT:
			if( Desc->Element == -1 )
				return Err( ERR_INVALID_ARGUMENTS, "ChildAdd(): ChildComponent::Elemement is invalid3" );
			break;
	}
	return true;
}



/****************************************************************
**	Routine: SDB_M_GET_CHILD_LIST::handle_string_objects
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_M_GET_CHILD_LIST::handle_string_objects(
	SDB_CHILD_LIST_ITEM* child
)
{
	if( child->Security.Type != SDB_CHILD_LIST_INTERMEDIATE )
		return true;

	SDB_CHILD_LIST_ITEM sec_item = Intermediates[ child->Security.Element ];
	sec_item.AsObject = TRUE;

	short sec_elem = ChildAdd( &sec_item, SDB_CHILD_LIST_INTERMEDIATE );
	if( sec_elem == -1 )
		return ErrMore( "Cannot find security element as object" );

	child->Security.Element = sec_elem;	// change childs security to point to the new one we found
	// if the old one is not used it will be garbage collected at the end

	return true;
}



/****************************************************************
**	Routine: SDB_M_GET_CHILD_LIST::ChildAdd
**	Returns: Index of item in ItemArray or -1 for error
**	Action : Add child to childlist
****************************************************************/

short SDB_M_GET_CHILD_LIST::ChildAdd(
	SDB_CHILD_LIST_ITEM	*Child,
	int					TerminalOrIntermediate // SDB_CHILD_LIST_TERMINAL/SDB_CHILD_LIST_INTERMEDIATE
)
{
	// Only allow arguments if there have been no other terminals.
	if( ( Child->Result.Type == SDB_CHILD_LIST_ARGUMENT || Child->Result.Type == SDB_CHILD_LIST_ELLIPSIS )
		&&	( Vti->ChildList.TerminalCount != Vti->ChildList.ArgChildCount || Vti->ChildList.ArgEllipsis ))
	{
		Err( ERR_INVALID_ARGUMENTS,
				"GOB_CHILD_A illegal after other GOB_CHILDren\n"
				"Terminal count is %d, ArgChildCount is %d, ArgEllipsis is %d",
				Vti->ChildList.TerminalCount, Vti->ChildList.ArgChildCount, Vti->ChildList.ArgEllipsis );
		return -1;
	}

	if( Child->Result.Type == SDB_CHILD_LIST_ELLIPSIS_SIZE && !Vti->ChildList.ArgEllipsis )
	{
		Err( ERR_INVALID_ARGUMENTS,	"Size( Ellipsis() ) illegal unless Ellipsis() is present" );
		return -1;
	}

	if( !( Child->Result.Type & SDB_CHILD_LIST_RESULT_TYPES ))
	{
		Err( ERR_INVALID_ARGUMENTS, "ChildAdd: Unknown result type %d", Child->Result.Type );
		return -1;
	}

	if(
		// check that all the items are correct
		!CheckDesc( &Child->Db,        FALSE ) ||
		!CheckDesc( &Child->Security,  FALSE ) ||
		!CheckDesc( &Child->ValueType, FALSE ) ||
		!CheckDesc( &Child->Pred,      FALSE ) ||

		// if intermediate is used as security then find a unique intermediate of the same characteristics but with the AsObject flag
		!handle_string_objects( Child )
	  )
		return -1;

	// now check all args
	for( short unsigned int Numc = 0; Numc < Child->Argc; ++Numc )
		if( !CheckDesc( &Child->Argv[ Numc ], Child->Result.NoEval ))
			return -1;

	// Guard against Aliases with predicates - these don't make sense
	if( ( TerminalOrIntermediate == SDB_CHILD_LIST_TERMINAL ) &&
		( Vti->ValueFlags & SDB_ALIAS ) &&
		( Child->Pred.Type != SDB_CHILD_LIST_NO_PREDICATE ) )
	{
	    Err( ERR_INVALID_ARGUMENTS, "ChildAdd: May not use an Alias with a Predicate" );
	    return -1;
	}

	short int
	        *Max,
	        *Count;

	SDB_CHILD_LIST_ITEM**
	        Items;

	if( TerminalOrIntermediate == SDB_CHILD_LIST_INTERMEDIATE )
	{
		Max   = &MaxIntermediates;
		Count = &Vti->ChildList.IntermediateCount;
		Items = &Intermediates;
	}
	else
	{
		Max   = &MaxTerminals;
		Count = &Vti->ChildList.TerminalCount;
		Items = &Terminals;
	}

	// See if we already have this item
	if( Child->Result.Type != SDB_CHILD_LIST_ARGUMENT )// never share arguments
	{
		for( short int Num = 0; Num < *Count; ++Num )
		{
		    SDB_CHILD_LIST_ITEM& ExistingNode = Items[0][ Num ];
			SDB_CHILD_LIST_ITEM& NewNode      = *Child;

			if( CanShare( ExistingNode, NewNode ) &&
				IdenticalPredicates( ExistingNode, NewNode ))
			{
			    return Num;
			}
		}
	}

	// New child item

	if( TerminalOrIntermediate == SDB_CHILD_LIST_TERMINAL )
	{
		if( Child->Result.Type == SDB_CHILD_LIST_ARGUMENT )
			++Vti->ChildList.ArgChildCount;
		else if( Child->Result.Type == SDB_CHILD_LIST_ELLIPSIS )
			Vti->ChildList.ArgEllipsis = true;
	}

	// See if we need to alloc more space for the new child
	if( *Count >= *Max )
	{
		*Max *= 2;
		if( !ERR_REALLOC( *Items, SDB_CHILD_LIST_ITEM, *Max * sizeof( **Items )))
			return -1;
	}

	// Get a reference to the new child
	SDB_CHILD_LIST_ITEM& Item = *(*Items + *Count);

	// Copy the child and assign it it's number
	Item = *Child;
	Item.Result.Element = *Count;

	if( Item.Result.Type == SDB_CHILD_LIST_ARGUMENT )	// Args are NoEval by default until someone uses them as !NoEval
		Item.Result.NoEval = true;

	// Copy the args
	Item.Argv = 0;
	if( Item.Argc )
	{
		if( !ERR_MALLOC( Item.Argv, SDB_CHILD_COMPONENT, Item.Argc * sizeof( *Item.Argv )))
			return -1;
		memcpy( Item.Argv, Child->Argv, sizeof( *Item.Argv ) * Item.Argc );
	}
	
	return (*Count)++;
}



/****************************************************************
**	Routine: SecDbChildPullValue
**	Returns: TRUE or FALSE
**	Action : Puts instruction for pulling Value.Data from ChildData
**			 into the childlist.
****************************************************************/

bool SecDbChildPullValue(
	SDB_VALUE_TYPE_INFO	*Vti,
	SDB_CHILD_COMPONENT	*Child,
	int					Offset,
	DT_DATA_TYPE		*DataType
)
{
	SDB_CHILD_PULL_VALUE
			*PullVal;


	if(		Child->Type != SDB_CHILD_LIST_TERMINAL
		&&	Child->Type != SDB_CHILD_LIST_ARGUMENT
		&&	Child->Type != SDB_CHILD_LIST_ELLIPSIS )
		return Err( ERR_INVALID_ARGUMENTS, "SecDbChildPullValue, @: Type must be TERMINAL or ARGUMENT" );

	if( !ERR_MALLOC( PullVal, SDB_CHILD_PULL_VALUE, sizeof( *PullVal )))
		return false;

	PullVal->Offset			= Offset;
	PullVal->ChildNumber	= Child->Element;
	PullVal->DataType		= DataType;
	
	// Prepend to list
	PullVal->Next				= Vti->ChildList.PullValues;
	Vti->ChildList.PullValues	= PullVal;
	
	// Check size
	Vti->ChildList.ChildDataSize = MAX( (unsigned) Vti->ChildList.ChildDataSize, Offset + sizeof( DT_VALUE_RESULTS ));

	return true;
}



/****************************************************************
**	Routine: SecDbBStreamCreateDefault
**	Returns: BSTREAM * or NULL
**	Action : Create a standard sized BStream.
****************************************************************/

BSTREAM *SecDbBStreamCreateDefault(
	void
)
{
	BSTREAM	*Stream = (BSTREAM *) malloc( sizeof( *Stream ));


	if( !Stream )
		Err( ERR_MEMORY, "SecDbBStreamCreateDefault, @" );

	BStreamAutoGrowInit( Stream );
	return Stream;
}



/****************************************************************
**	Routine: SecDbBStreamCreate
**	Returns: BSTREAM * or NULL
**	Action : Create a BStream from passed arguments
****************************************************************/

BSTREAM *SecDbBStreamCreate(
	void	*Mem,
	size_t	MemSize
)
{
	BSTREAM	*Stream = (BSTREAM *) malloc( sizeof( *Stream ));


	if( !Stream )
		Err( ERR_MEMORY, "SecDbBStreamCreate, @" );

	BStreamInit( Stream, Mem, MemSize );
	// Don't allow empty streams to start in overflow (error) state
	if( !Mem )
		Stream->Overflow = FALSE;

	Stream->UserData = (unsigned long) SecDbRootDb;
	return Stream;
}



/****************************************************************
**	Routine: SecDbBStreamDestroy
**	Returns: nothing
**	Action : Destroy a BStream created with SecDbBStreamCreate.
**			 If FreeData is TRUE, also free BStream->DataStart
**			 if it was allocated by SecDbBStreamCreate.  If Mem
**			 was passed to SecDbBStreamCreate, SecDbBStreamDestroy
**			 will never free that memory.
****************************************************************/

void SecDbBStreamDestroy(
	BSTREAM	*Stream,
	int		FreeData
)
{
	if( !Stream )
		return;

	if( !FreeData )
		Stream->FreeData = FALSE;
	BStreamDestroy( Stream );
	free( Stream );
}



/****************************************************************
**	Routine: SecDbReclassify
**	Returns: True or False
**	Action : Changes an object to be a new class.
****************************************************************/

int SecDbReclassify(
	SDB_OBJECT	*SecPtr,
	SDB_SEC_TYPE Type
)
{
	SDB_NODE
			*Node;

	SDB_ENUM_PTR
			EnumPtr;

	int		Token = 0;

	SDB_CLASS_INFO
			*ClassInfo;

	if( !(ClassInfo = SecDbClassInfoFromType( Type )))
		return FALSE;

	if( SecDbTraceFunc )
		(SecDbTraceFunc)( SDB_TRACE_DESTROY_CACHE, SecPtr );

	// Pull it out of the old class
	SecDbClassObjectRemove( SecPtr );

	// Set the Class so that NodeFindClassVti will find the correct VTI
	SecPtr->Class = ClassInfo;

	SDB_NODE::Destroy( NULL, &Token );
	
	// AWR 350874. 
	// Increase SecPtr's reference counter to ensure that SecPtr will not be removed "too early".
	// If ReferenceCount = 0 then security is already in the dead pool.
	bool ref_counter_increased = false;
	if( SecPtr->ReferenceCount > 0 )
	{
		SecDbIncrementReference( SecPtr );
		ref_counter_increased = true;
	}

	for( Node = SecDbNodeEnumFirst( SecPtr, SDB_NODE_ENUM_TERMINALS, &EnumPtr ); Node; Node = SecDbNodeEnumNext( EnumPtr ))
	{
		// Preserve the node if it is Diddled or Set and 
		// the node exists in the new security as retained

		SDB_VALUE_TYPE_INFO
				*NodeVti = Node->VtiGet();

		int Preserve = Node->CacheFlagsGet() & ( SDB_CACHE_DIDDLE | SDB_CACHE_SET );

		if( !NodeVti )
			Preserve = 0;
		else 
		{
			const SDB_VALUE_TYPE_INFO* Vti = ClassInfo->LookupVti( NodeVti->ValueType->ID );

			Preserve = Preserve && ( Vti && 
									 Vti->ValueFlags & SDB_SET_RETAINED );

		}
		if ( !Preserve )
			SDB_NODE::Destroy( Node, &Token );
		else if( Node->DbSetGet()->BaseDb() && Node->VtiNodeGet() && Node->VtiNodeGet()->ObjectGet() != Node->ObjectGet() ) // Reassign VTI
			Node->VtiNodeSet( SDB_NODE::FindDefaultVtiNode( Node->DbSetGet()->BaseDb(), Node->ObjectGet(), Node->ValueTypeGet(), SDB_NODE::CREATE, SecPtr->Class->ClassSecNode ));
	}
	SecDbNodeEnumClose( EnumPtr );
	SDB_NODE::Destroy( NULL, &Token );

	if( SecDbTraceFunc )
		(SecDbTraceFunc)( SDB_TRACE_DONE_DESTROY_CACHE, SecPtr );

	// Pop it into the new class.
	SecDbClassObjectInsert( SecPtr );
	SecPtr->SdbClass = SecPtr->Class;
	SecPtr->SecData.Type = Type;

	// Decrease the SecPtr's reference count that we bumped up above. 
	// Remove SecPtr if no longer needed.
	if( SecPtr->ReferenceCount > 0 && ref_counter_increased )
		SecDbFree( SecPtr );
	
	return TRUE;
}



/****************************************************************
**	Routine: SDB_OBJECT::Free
**	Returns: 
**	Action : 
****************************************************************/

void SDB_OBJECT::Free(
)
{
	if( ReferenceCount != -1 )				// don't free until ref count goes to zero
		return;

	if( Class )
	{
		// Don't bother sending the unload message if it is one of the class objects
		// because it never got a NEW message
		if( Db != SecDbClassObjectsDb )
		{
			SDB_MESSAGE( this, SDB_MSG_UNLOAD, NULL );
		}

		SecDbClassObjectRemove( this );
		Class = NULL;
	}
    
	SlangUpdateScopeDiddles( this );

	if( Db )
	{
		HashDelete( Db->CacheHash, SecData.Name );
		Db = NULL;
	}
	free( this );
}



/****************************************************************
**	Routine: SDB_M_GET_CHILD_LIST::Initialize
**	Returns: TRUE/FALSE
**	Action : initializes static arrays used to build child lists
****************************************************************/

int SDB_M_GET_CHILD_LIST::Initialize(
)
{
	MaxTerminals = MaxIntermediates = MaxCasts = MaxLiterals = 32;

	Terminals = (SDB_CHILD_LIST_ITEM *) malloc( sizeof( SDB_CHILD_LIST_ITEM ) * MaxTerminals );
	Intermediates = (SDB_CHILD_LIST_ITEM *) malloc( sizeof( SDB_CHILD_LIST_ITEM ) * MaxIntermediates );
	Literals = (SDB_NODE **) malloc( sizeof( SDB_NODE * ) * MaxLiterals );
	Casts = (SDB_CHILD_CAST *) malloc( sizeof( SDB_CHILD_CAST ) * MaxCasts );

	State = Initialized;
	return TRUE;
}



/****************************************************************
**	Routine: SDB_M_GET_CHILD_LIST::FindChild
**	Returns: reference to the child
**	Action : locates the child corresponding to the component
****************************************************************/

SDB_CHILD_LIST_ITEM* SDB_M_GET_CHILD_LIST::FindChild(
    SDB_CHILD_COMPONENT const& Component
)
{
    if( Component.Type == SDB_CHILD_LIST_INTERMEDIATE )
	    return &Intermediates[ Component.Element ];

	if( Component.Type == SDB_CHILD_LIST_TERMINAL )
	    return &Terminals[ Component.Element ];

	return 0;
}


void SecDbMergeTerminalPredicates( SDB_M_GET_CHILD_LIST& GetChildList, SDB_CHILD_LIST& ChildList )
{
    int const total_terminals = ChildList.TerminalCount;

	bool merge = true;
	while( merge ) // N^3 worst case, N^2 best case - revisit - KEN
	{
	    merge = false;
	    for( int ii = 0; ii < total_terminals; ++ii )
		    for( int jj = ii + 1; jj < total_terminals; ++jj )
			    if( MergedPredicates( &GetChildList, ChildList.Terminals[ii], ChildList.Terminals[jj] ))
				    merge = true;
	}
}

// simple helper function because component references are scattered around the child list items
template <class T>
void apply_over_all_components( SDB_CHILD_LIST_ITEM& child_list, T& obj )
{
    obj( child_list.Db );
	obj( child_list.Security );
	obj( child_list.ValueType );
	obj( child_list.Result );
	obj( child_list.Pred );
	// don't want to do: for_each( child_list.Argv, child_list.Argv + child_list.Argc, obj );
	// because we want to use obj by reference, not value
	for( unsigned ii = 0; ii < child_list.Argc; ++ii )
	    obj( child_list.Argv[ii] );
}

// like a for_each, but takes the function object by reference, not by value
template <class Iter, class T>
void apply_over( Iter begin, Iter end, T& obj )
{
    for( ; begin != end; ++begin )
	    obj( *begin );
}

// Objects of this class perform the mark phase of a simple mark-and-sweep garbage collector for the intermediate children
class NonTerminalMarker
{
public:
    explicit NonTerminalMarker( int inter_size, int casts_size )
	  : m_inter_colors( inter_size ),
		m_casts_colors( casts_size ),
		m_fresh( false ) {}
    // use default copy constructor
    // use default destructor
    // use default assignment operator

    enum Mark_State { UNMARKED = 0, MARKED, FRESH_MARKED };

    typedef CC_STL_VECTOR( Mark_State ) Mark_Record;

    void operator()( SDB_CHILD_COMPONENT const& child )
    {
	    if( child.Type == SDB_CHILD_LIST_INTERMEDIATE ) { color( m_inter_colors[ child.Element ] ); return; }
		if( child.Type == SDB_CHILD_LIST_CAST         ) { color( m_casts_colors[ child.Element ] ); return; }
	}

    void operator()( SDB_CHILD_LIST_ITEM& children ) { apply_over_all_components( children, *this );}

    bool is_fresh() const { return m_fresh; }
    void refresh() { m_fresh = false; }

    bool process_inter( int ii ) { if( m_inter_colors[ii] != FRESH_MARKED ) return false; m_inter_colors[ii] = MARKED; return true; }
    bool process_casts( int ii ) { if( m_casts_colors[ii] != FRESH_MARKED ) return false; m_casts_colors[ii] = MARKED; return true; }

    Mark_Record&       inter_colors()       { return m_inter_colors; }
    Mark_Record const& inter_colors() const { return m_inter_colors; }

    Mark_Record&       casts_colors()       { return m_casts_colors; }
    Mark_Record const& casts_colors() const { return m_casts_colors; }

private:
    void color( Mark_State& mark ) { if( mark == UNMARKED ) { mark = FRESH_MARKED; m_fresh = true; } }
    Mark_Record m_inter_colors; // coloring for intermediate children
    Mark_Record m_casts_colors; // coloring for cast children

    bool m_fresh;
};

template <class Iter, class T>
int sweep_helper( Iter begin, Iter end, CC_STL_VECTOR( int )& mapping, T* array )
{
    int new_count = 0;
	for( int old_count = 0; begin != end; ++begin, ++old_count )
	{
	    if( *begin == NonTerminalMarker::UNMARKED ) // nuke node as it is unused
		    continue;

		mapping[ old_count ] = new_count;
		if( new_count != old_count ) // performance hack only
		    array[ new_count ] = array[ old_count ];
		++new_count;
	}
	return new_count;
}

// Objects of this class do the sweep phase of a simple mark-and-sweep garbage collector for the intermediate children
// They also fixup any references to the intermediates.
class NonTerminalRenamer
{
public:
    explicit NonTerminalRenamer( NonTerminalMarker const& marker, SDB_CHILD_LIST& ChildList )
	  : m_inter_mapping( marker.inter_colors().size()),
		m_casts_mapping( marker.casts_colors().size())
    {
	    // sweep away any unreference intermediates, and make a table to record the needed fixups
	    ChildList.IntermediateCount = sweep_helper( marker.inter_colors().begin(), marker.inter_colors().end(),
													m_inter_mapping, ChildList.Intermediates );

		// sweep away any unreference casts, and make a table to record the needed fixups
		ChildList.CastCount = sweep_helper( marker.casts_colors().begin(), marker.casts_colors().end(),
											m_casts_mapping, ChildList.Casts );
	}

    // fixup references to the intermediates
    void operator()( SDB_CHILD_COMPONENT& child ) const
    {
	    if( child.Type == SDB_CHILD_LIST_INTERMEDIATE ) { child.Element = m_inter_mapping[ child.Element ]; return; }
		if( child.Type == SDB_CHILD_LIST_CAST         ) { child.Element = m_casts_mapping[ child.Element ]; return; }
	}

    void operator()( SDB_CHILD_LIST_ITEM& children ) { apply_over_all_components( children, *this );}
    void operator()( SDB_CHILD_CAST& child )         { this->operator()( child.Desc );}

private:
    CC_STL_VECTOR( int ) m_inter_mapping;
    CC_STL_VECTOR( int ) m_casts_mapping;
};

void SecDbGarbageCollectIntermediates( SDB_CHILD_LIST& ChildList )
{
	int const orig_intermediates = ChildList.IntermediateCount;
	int const orig_casts         = ChildList.CastCount;

	// simple mark and sweep garbage collector

	//
	// Mark
	//

	NonTerminalMarker marker( orig_intermediates, orig_casts );

	// mark all terminals, as these must always be referenced
	apply_over( ChildList.Terminals, ChildList.Terminals + ChildList.TerminalCount, marker );

	// recursively mark referenced nodes
	while( marker.is_fresh() )
	{
	    marker.refresh();
		for( int ii = 0; ii < orig_intermediates; ++ii )
		{
		    if( marker.process_inter( ii ))
			    marker( ChildList.Intermediates[ ii ] );
		}
		for( int jj = 0; jj < orig_casts; ++jj )
		{
		    if( marker.process_casts( jj ))
			    marker( ChildList.Casts[ jj ].Desc );
		}
	}

	//
	// Sweep
	//

	NonTerminalRenamer renamer( marker, ChildList ); // collapse unused intermediates and casts

	// fixup all references to the intermediates and casts
	apply_over( ChildList.Terminals,     ChildList.Terminals     + ChildList.TerminalCount,     renamer );
	apply_over( ChildList.Intermediates, ChildList.Intermediates + ChildList.IntermediateCount, renamer );
	apply_over( ChildList.Casts,         ChildList.Casts         + ChildList.CastCount,         renamer );
}

/****************************************************************
**	Routine: SecDbSendGetChildList
**	Returns: TRUE/FALSE
**	Action : Sends the get child list message
****************************************************************/

int SecDbSendGetChildList(
	SDB_OBJECT			*Object,
	SDB_VALUE_TYPE_INFO	*Vti
)
{
	static SDB_M_DATA
			MsgData;

	SDB_M_GET_CHILD_LIST
	        NewChildList;

	if( NewChildList.State == SDB_M_GET_CHILD_LIST::Uninitialized )
		NewChildList.Initialize();
	else if( NewChildList.State == SDB_M_GET_CHILD_LIST::GettingChildList )
		return Err( ERR_UNSUPPORTED_OPERATION, "Already getting child list for %s, cannot recursively invoke for %s", MsgData.GetChildList.Vti->Name, Vti->Name );

	NewChildList.State = SDB_M_GET_CHILD_LIST::GettingChildList;

	MsgData.GetChildList.Vti		= Vti;

	int Status = SDB_MESSAGE_VT_TRACE( Object, SDB_MSG_GET_CHILD_LIST, &MsgData, Vti ); // sends the message AND prints a trace

	NewChildList.State = SDB_M_GET_CHILD_LIST::Initialized;

	if( !Status )
		return ErrMore( "Failed to get child list for %s( %s )", Vti->Name, Object->Class->Name );

	// already valid, probably because someone did a Copy
	if( Vti->ChildList.ChildListValid )
		return TRUE;

	// share the child lists for now
	SDB_CHILD_LIST_ITEM* old_terminals     = Vti->ChildList.Terminals;
	SDB_CHILD_LIST_ITEM* old_intermediates = Vti->ChildList.Intermediates;
	SDB_CHILD_CAST*      old_casts         = Vti->ChildList.Casts;
	Vti->ChildList.Terminals     = NewChildList.Terminals;
	Vti->ChildList.Intermediates = NewChildList.Intermediates;
	Vti->ChildList.Casts         = NewChildList.Casts;

	SecDbMergeTerminalPredicates( NewChildList, Vti->ChildList ); // do final walk to merge predicates
	SecDbGarbageCollectIntermediates( Vti->ChildList );// do a final garbage collect on the intermediates as predicates may have been merged

	Vti->ChildList.Terminals     = old_terminals;
	Vti->ChildList.Intermediates = old_intermediates;
	Vti->ChildList.Casts         = old_casts;

	//

	if( Vti->ChildList.TerminalCount )
	{
	    int Size = sizeof( SDB_CHILD_LIST_ITEM ) * Vti->ChildList.TerminalCount;
		Vti->ChildList.Terminals = (SDB_CHILD_LIST_ITEM *) malloc( Size );
		memcpy( Vti->ChildList.Terminals, NewChildList.Terminals, Size );
	}

	if( Vti->ChildList.IntermediateCount )
	{
	    int Size = sizeof( SDB_CHILD_LIST_ITEM ) * Vti->ChildList.IntermediateCount;
		Vti->ChildList.Intermediates = (SDB_CHILD_LIST_ITEM *) malloc( Size );
		memcpy( Vti->ChildList.Intermediates, NewChildList.Intermediates, Size );
	}

	if( Vti->ChildList.CastCount )
	{
	    int Size = sizeof( SDB_CHILD_CAST ) * Vti->ChildList.CastCount;
		Vti->ChildList.Casts = (SDB_CHILD_CAST *) malloc( Size );
		memcpy( Vti->ChildList.Casts, NewChildList.Casts, Size );
	}

	if( Vti->ChildList.LiteralCount )
	{
	    int Size = sizeof( SDB_NODE * ) * Vti->ChildList.LiteralCount;
		Vti->ChildList.Literals = (SDB_NODE **) malloc( Size );
		memcpy( Vti->ChildList.Literals, NewChildList.Literals, Size );
	}
	Vti->ChildList.ChildListValid = true;

	return TRUE;
}
