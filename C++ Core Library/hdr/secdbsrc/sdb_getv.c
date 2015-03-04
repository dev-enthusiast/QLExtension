#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_getv.c,v 1.68 2001/11/27 23:23:37 procmon Exp $"
/****************************************************************
**
**	SDB_GETV.C	- External API for SecDbGetValue
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_getv.c,v $
**	Revision 1.68  2001/11/27 23:23:37  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.65  2001/01/19 19:15:53  shahia
**	Got rid of use of SDB_NODE_CONTEXTs Clear method, OK to PRODVER
**	
**	Revision 1.64  2000/12/05 16:48:28  singhki
**	Added Subscribe capability to nodes
**	
**	Revision 1.63  2000/05/16 10:17:28  goodfj
**	Check args in SecDbGetValue
**	
**	Revision 1.62  2000/04/24 11:04:23  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.61  2000/03/23 18:22:15  hsuf
**	Roll back SDB_BUILD_CHILDREN_BEFORE_VALUE.  Use enum for SDB_RELEASE_VALUE instead of individual bit fields.
**	Prepare for future SDB_RELEASE_DESCENDENT_VALUES implementation.
**	
**	Revision 1.60  2000/02/08 04:50:25  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.59  1999/11/17 23:42:06  davidal
**	Added two new attributes for VT as follows:
**	- SDB_RELEASE_VALUE;
**	- SDB_RELEASE_VALUE_WHEN_PARENTS_VALID.
**	Once set for the VT, the former will cause an immediate release of the node's value after it gets consumed
**	by a parent; the latter will do the same after *all* the parents are valid.
**	By default, both the attributes are set to FALSE; however, defining the env variable
**	SECDB_RELEASE_VALUE_WHEN_PARENTS_VALID as TRUE will set SDB_RELEASE_VALUE_WHEN_PARENTS_VALID for all
**	non-terminal nodes.
**	
**	Revision 1.58  1999/11/01 22:41:52  singhki
**	death to all link warnings
**	
**	Revision 1.57  1999/10/26 19:22:17  singhki
**	Allow getvalue/buildchildren to be aborted and restarted
**	
**	Revision 1.56  1999/08/07 01:40:09  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.55  1999/07/16 21:22:12  singhki
**	NodeGetValue() now is passed the Db so that it can ensure that it builds
**	the childrne in the correct Db.
**	Fixed VTI interactions with diddle scopes, VTI's now not only cause splits
**	but also are set on extended nodes.
**	
**	Revision 1.54  1999/05/20 23:20:52  singhki
**	Add nodes with 0 parents to an LRU list on the node's object
**	The last n nodes of this list get thrown away when it reaches
**	MaxUnparentedNodes which is set on a class basis
**	
**	Revision 1.53  1999/04/16 23:53:20  singhki
**	Add NodeFlags in NodeFindChild based on Item flags set by SecDbChildAdd. Fixup noeval stuff to work better with each.
**	
**	Revision 1.52  1999/03/04 20:11:06  vengrd
**	Corrected comment about who owns return of SecDbGetValueWithArgs().
**	
**	Revision 1.51  1998/11/18 17:42:52  singhki
**	Add NodeIsValid & remove default args from NodeFind... to force explicit spec of Db & Share
**	
**	Revision 1.50  1998/06/01 17:09:01  wene
**	Disable type checking for DtAny
**	
**	Revision 1.49  1998/01/02 21:33:00  rubenb
**	added GSCVSID
**	
**	Revision 1.48  1997/01/27 17:22:24  gribbg
**	DiddleVectors: Cleanup and make Dynamic Numbers/Pointers work
**	
**	Revision 1.47  1997/01/10 21:59:19  gribbg
**	Vector Diddles
**
**	Revision 1.46  1997/01/06 21:29:00  gribbg
**	New SecDbGetValueWithArgs API: Data type checking
**
**	Revision 1.45  1996/12/23 03:06:19  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
**
**	Revision 1.44  1996/10/24 18:05:24  lundek
**	Allow diddles on unsupported value types
**
**	Revision 1.43  1996/09/18 02:20:46  lundek
**	Clean up SecDb trace stuff
**
**	Revision 1.42  1996/09/11 02:53:52  gribbg
**	New Trees Merge
**
**	Revision 1.41.2.4  1996/08/09 04:07:21  lundek
**	Normalize (and fix) handling of DtPassThrough values
**
**	Revision 1.41.2.3  1996/07/22 14:21:39  lundek
**	NewTrees: finish putting argument support into SecDb api
**
**	Revision 1.41.2.2  1996/07/05 21:04:05  lundek
**	New, New Trees
**
**	Revision 1.41.2.1  1996/05/10 11:54:35  gribbg
**	NewTrees: First snapshot
**
**	Completely rewritten for new trees.
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<secnode.h>
#include	<sectrace.h>
#include	<secnodei.h>


/*
**	Variables
*/

SDB_TRACE_FUNC
		SecDbTraceFunc = NULL;


/*
**	Local class to update SecDbGetValueDepth
*/

class UPDATE_GET_VALUE_DEPTH
{
public:
	UPDATE_GET_VALUE_DEPTH()
	{
		++SecDbGetValueDepth;
	}
	~UPDATE_GET_VALUE_DEPTH()
	{
		if( --SecDbGetValueDepth == 0 && SecDbTraceFunc == SecDbTraceAbortFunc )
			SecDbTraceFunc = NULL;
	}
};



/****************************************************************
**	Routine: SecDbGetValue
**	Returns: Pointer to DT_VALUE (owned by SecPtr) if successful
**			 NULL if error getting value
**	Summary: Retrieve a value from a security
****************************************************************/

DT_VALUE *SecDbGetValue(
	SDB_OBJECT 		*SecPtr,		// Pointer to security
	SDB_VALUE_TYPE	ValueType		// Value type to get
)
{
	if( NULL == ValueType )
		return (DT_VALUE *) ErrN( ERR_INVALID_ARGUMENTS, "SecDbGetValue: NULL ValueType" );

	return SecDbGetValueWithArgs( SecPtr, ValueType, 0, NULL, ValueType->DataType );
}



/****************************************************************
**	Routine: SecDbSubscribeValue
**	Returns: Pointer to DT_VALUE (owned by SecPtr) if successful
**			 NULL if error getting value
**	Action : Evaluates the given node
****************************************************************/

DT_VALUE *SecDbSubscribeValue(
	SDB_OBJECT 		*SecPtr,		// Pointer to security
	SDB_VALUE_TYPE	ValueType,		// Value type to get
	int				Argc,			// Count of arguments
	DT_VALUE		**Argv,			// Array of pointers to arguments
	DT_DATA_TYPE	DataType,		// Required datatype (NULL for any)
	bool			DoNotSubscribe	// dont subscribe
)
{
	SDB_NODE
			*pNode;

	SDB_NODE_CONTEXT
			Ctx;

	DT_VALUE
			*Result;

	UPDATE_GET_VALUE_DEPTH
			UpdateGetValueDepth;

	SDB_DB	*Db = SecDbRootDb;


	pNode = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, Argc, Argv, SDB_NODE::CHECK_SUPPORTED, Db, SDB_NODE::SHARE );
	if( !pNode )
		return (DT_VALUE *) ErrMoreN( "SecDbGetValueWithArgs" );

	if( !DoNotSubscribe )
		pNode->SubscribedSet( true );

	if( !pNode->GetValue( &Ctx, Db ))
		return (DT_VALUE *) ErrMoreN( "SecDbGetValueWithArgs" );

    pNode->ReleaseValueFlagSet( SDB_NODE::ReleaseValueNoneFlag );             // make sure ReleaseValue flag is reset!

	Result = pNode->AccessValue();

	// Disable type checking for DtAny
	if( DataType && Result && Result->DataType != DataType && DataType != DtAny )
	{
		Err( ERR_NOT_CONSISTENT, "%s( %s ) expected a '%s', but got a '%s'",
				ValueType->Name, SecPtr->SecData.Name,
				DataType->Name, Result->DataType->Name );
		Result = NULL;
	}

	if( !pNode->OnLRUCacheGet() && pNode->ParentIndexGet() == 0 )
		// node never got any parent link/unlink operations
		// therefore it is a good candidate to put on the LRU list
		pNode->LRUCacheByObjectInsert();

	return Result;
}




/****************************************************************
**	Routine: SecDbGetValueWithArgs
**	Returns: Pointer to DT_VALUE (owned by SecPtr) if successful
**			 NULL if error getting value
**	Action : Evaluates the given node
****************************************************************/

DT_VALUE *SecDbGetValueWithArgs(
	SDB_OBJECT 		*SecPtr,		// Pointer to security
	SDB_VALUE_TYPE	ValueType,		// Value type to get
	int				Argc,			// Count of arguments
	DT_VALUE		**Argv,			// Array of pointers to arguments
	DT_DATA_TYPE	DataType		// Required datatype (NULL for any)
)
{
	return SecDbSubscribeValue( SecPtr, ValueType, Argc, Argv, DataType, true );
}
