#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbnfind.c,v 1.205 2014/09/23 01:32:09 khodod Exp $"
/****************************************************************
**
**	SDBNFIND.C - Find, create, and delete SDB_NODEs
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbnfind.c,v $
**	Revision 1.205  2014/09/23 01:32:09  khodod
**	Make sure SDB_NODE::Find returns NULL on error. Previously a call to
**	SplitChildren returned SDB_NODE_INVALID_PTR, causing ValueTypeChildInfo
**	to crash when that pointer was dereferenced.
**	AWR: #350317
**
**	Revision 1.204  2012/06/14 18:24:05  e19351
**	Backout cast object changes; seems this file was left out accidentally.
**	AWR: #229190
**
****************************************************************/

#define BUILDING_SECDB
#define INCL_ALLOCA
#include <portable.h>
#include <secdb.h>
#include <secnode.h>
#include <secnodei.h>
#include <secobj.h>
#include <sectrace.h>
#include <secdrv.h>
#include <sdbdbset.h>
#include <secdt.h>
#include <sdb_int.h>

#include <datatype.h>

#include <kool_ade.h>
#include <hash.h>

#include <algorithm>

#include <stdlib.h>
#include <stdio.h>

/*
**	Bring in TotalRefCount from sdb_api.c
*/

extern int
		TotalRefCount;


/*
** List of nodes to split
*/

struct SDB_NODE_SPLIT
{
	SDB_NODE
			*Node;

	SDB_DBSET_BEST
			*DbSetBest;

	SDB_NODE_SPLIT
			*Next;
};

static SDB_NODE *NodeFindVtiNode( SDB_DB *Db, SDB_NODE *Object, SDB_NODE *ValueType, int Argc, SDB_NODE **Argv, SDB_NODE::EnumCreate CreateNode = SDB_NODE::CREATE );


/*
**	~Cast and ~Subscript objects
*/

SDB_NODE
		*CastObject = NULL,
		*SubscriptObject = NULL;



/****************************************************************
**	Routine: SDB_NODE::LinkLiteral
**	Returns: SDB_NODE *
**	Action : Finds a literal node and establishes a ref to it
****************************************************************/

SDB_NODE *SDB_NODE::LinkLiteral(
	DT_DATA_TYPE	DataType,
	const void		*Pointer
)
{
	DT_VALUE
			TmpVal;

	TmpVal.DataType = DataType;
	TmpVal.Data.Pointer = Pointer;

	SDB_NODE *Node = FindLiteral( &TmpVal, SecDbRootDb );
	if( Node->RefCountedParents() )
		Node->LiteralRefsIncrement();

	return Node;
}



/****************************************************************
**	Routine: NodeFindVtiNode
**	Returns: SDB_NODE * / Null
**	Action : Finds VTI node corresponding to node
****************************************************************/

static SDB_NODE *NodeFindVtiNode(
	SDB_DB					*Db,
	SDB_NODE				*Object,
	SDB_NODE				*ValueType,
	int						Argc,
	SDB_NODE				**Argv,
	SDB_NODE::EnumCreate	CreateNode
)
{
	static SDB_NODE *ValueFuncNode = SDB_NODE::LinkLiteral( DtValueType, SecDbValueValueFunc );
	SDB_NODE** NodeArgs = (SDB_NODE**) alloca( sizeof( SDB_NODE* ) * ( Argc + 1 ) );

	NodeArgs[ 0 ] = ValueType;
	memcpy( NodeArgs + 1, Argv, sizeof( SDB_NODE * ) * Argc );

	return SDB_NODE::Find( Object, ValueFuncNode, Argc + 1, NodeArgs, CreateNode, Db, SDB_NODE::SHARE );
}



/****************************************************************
**	Routine: SDB_NODE::FindDefaultVtiNode
**	Returns: SDB_NODE * / Null
**	Action : Finds Default VTI node for Class & VT
****************************************************************/

SDB_NODE *SDB_NODE::FindDefaultVtiNode(
	SDB_DB			*Db,				// Db in which to find VTI
	SDB_NODE		*ObjectNode,		// Object whose VTI we are looking for
	SDB_NODE		*ValueTypeNode,		// Value Type to find VTI for
	EnumCreate		CreateNode,			// Create if not found
	SDB_NODE		*ClassObjNode		// Class Object Node (defaults to ObjectNode)
)
{
	static SDB_NODE
			*ClassValueFuncNode = LinkLiteral( DtValueType, SecDbValueClassValueFunc ),
			*ObjectValueFuncNode = LinkLiteral( DtValueType, SecDbValueObjectValueFunc );

	DT_VALUE
			*ObjectValue;

	if( ObjectNode->ValidGet() && ( ObjectValue = ObjectNode->AccessValue())->DataType == DtSecurity && ((SDB_OBJECT *) ObjectValue->Data.Pointer)->HaveObjectVti )
	{
		SDB_NODE
				TmpNode;

		SDB_NODE* tmpnode_children[4];
		tmpnode_children[0] = ObjectNode;
		tmpnode_children[1] = ObjectValueFuncNode;
		tmpnode_children[2] = 0;
		tmpnode_children[3] = ValueTypeNode;

		TmpNode.m_children = tmpnode_children;
		TmpNode.m_Argc = 1;

		for( nodes_iterator it = nodes().lower_bound( &TmpNode ); it != nodes().end() && equal( &TmpNode, *it ); ++it )
		{
			if( (*it)->DbSetGet()->BaseDb() && SDB_DBSET_BEST::Lookup( Db->DbSingleton, (*it)->DbSetGet()->BaseDb() )->m_Shareable )
				// Have Object VTI for this Object/VF pair such that Db contains the BaseDb of the DbSet
				// so go find an Object VTI node in this DB
				return Find( ObjectNode, ObjectValueFuncNode, 1, &ValueTypeNode, CreateNode, Db, SHARE );
		}
		// Otherwise we don't have an VTI for this Object/VF pair such that Db contains it's BaseDb, return the class one
	}
	return Find( ClassObjNode ? ClassObjNode : ObjectNode, ClassValueFuncNode, 1, &ValueTypeNode, CreateNode, Db, SHARE );
}



/****************************************************************
**	Routine: NodeRedirectValueFunc
**	Returns: true/false
**	Action : Redirects the VTI for the nodes referenced by a
**			 Value Func() node
****************************************************************/

bool NodeRedirectValueFunc(
	SDB_NODE		*VtiNode,
	SDB_NODE		*Object,
	SDB_NODE		*ValueType,
	int				Argc,
	SDB_NODE		**Argv,
	SDB_DB			*Db
)
{
	// Find one matching node which should be referenced by VtiNode
	SDB_NODE *DestNode = SDB_NODE::Find( Object, ValueType, Argc, Argv, SDB_NODE::CREATE, Db, SDB_NODE::DO_NOT_SHARE );
	if( !DestNode )
		return false;					// something bad happened

	for( SDB_NODE::nodes_iterator it = SDB_NODE::nodes().lower_bound( DestNode ); it != SDB_NODE::nodes().end() && equal( DestNode, *it ); ++it )
	{
		if( (*it)->VtiNodeGet() != VtiNode && SDB_DBSET_BEST::Lookup( (*it)->DbSetGet(), Db )->m_Shareable )
			// Refind the VTI node is it is possible that it might have changed.
			// note that we don't whack the VtiNode to VtiNode because clearly the existing Vti node might be better
			(*it)->VtiNodeSet( VtiNode->FindAgain( SDB_NODE::CREATE, (*it)->DbSetGet()->BaseDb(), SDB_NODE::SHARE ));
	}

	return true;
}



/****************************************************************
**	Routine: SDB_NODE::RedirectObjectValueFunc
**	Returns: true/false
**	Action : Redirects the VTI for the nodes referenced by a
**			 Object Value Func() node
****************************************************************/

bool SDB_NODE::RedirectObjectValueFunc(
	SDB_NODE		*VtiNode,
	SDB_NODE		*Object,
	SDB_NODE		*ValueType,
	SDB_DB			*Db
)
{
	static SDB_NODE
			*ValueFuncNode = SDB_NODE::LinkLiteral( DtValueType, SecDbValueValueFunc );

	DT_VALUE
			*ObjectValue = Object->AccessValue();

	if( ObjectValue->DataType != DtSecurity )
	{
		Err( ERR_UNKNOWN, "@ NodeCreate(): Object Value Func, the object must be a security" );
		return false;
	}

	SDB_OBJECT *SecPtr = (SDB_OBJECT *) ObjectValue->Data.Pointer;
	SecPtr->HaveObjectVti = TRUE;

	SDB_NODE
			**ParentMax = Object->ParentGetAddr( Object->ParentsMax() );

	for( SDB_NODE **ParentPtr = Object->ParentGetAddr( 0 ); ParentPtr < ParentMax; ++ParentPtr )
	{
		if( IsNode( *ParentPtr ))
		{
			SDB_NODE *Node = SDB_NODE_PARENT_PTR( *ParentPtr ).Node();

			if( Node->ValueTypeGet() == ValueType && Node->VtiNodeGet()
				&& Node->VtiNodeGet()->ValueTypeGet() != ValueFuncNode
				&& SDB_DBSET_BEST::Lookup( Node->DbSetGet(), Db )->m_Shareable )
				Node->VtiNodeSet( VtiNode );
		}
	}
	return true;
}



/****************************************************************
**	Routine: InvalidateArgNode
**	Returns: true/err
**	Action : 
****************************************************************/

static bool InvalidateArgNode(
	SDB_DB		*Db,
	SDB_NODE	*Object,
	SDB_NODE	*ValueType
)
{
	static SDB_NODE *VTNode = SDB_NODE::LinkLiteral( DtValueType, SecDbValueValueTypeArgs );
	SDB_NODE* NodeArgs[1];

	NodeArgs[0] = ValueType;

	SDB_NODE* node = SDB_NODE::Find( Object, VTNode, 1, NodeArgs, SDB_NODE::CREATE, Db, SDB_NODE::SHARE );
	if( !node || !node->Invalidate() )
		return ErrMore( "Could not create and/or invalidate node which references args" );
	return true;
}



/****************************************************************
**	Routine: SDB_NODE::Create
**	Returns: SDB_NODE* / NULL
**	Action : creates a new node
****************************************************************/

SDB_NODE *SDB_NODE::Create(
	SDB_DB			 		 *Db,
	SDB_NODE		 		 *Object,
	SDB_NODE		 		 *ValueType,
	int				 		 Argc,
	SDB_NODE		 		 **Argv,
	SDB_NODE		 		 *BestNode,
	SDB_NODE::nodes_iterator position
)
{
	SDB_NODE *Node;

	if( !ERR_CKALLOC( Node = SDB_NODE::Alloc()))
		return NULL;

	SDB_NODE** children = (SDB_NODE**) malloc( ( 3 + Argc ) * sizeof( SDB_NODE* ) );
	children[0] = SDB_NODE_PURPLE_CHILD( Object );
	children[1] = SDB_NODE_PURPLE_CHILD( ValueType );
	children[2] = 0;

	Node->DbSetSet( Db->DbSingleton );
	Node->m_children = children;

	if( SecDbTraceFunc )
		(SecDbTraceFunc)( SDB_TRACE_NEW, Node );

	// link Object & ValueType nodes as parents
	int Status = Node->ObjectGet()->ParentLink( SDB_NODE_PURPLE_PARENT( Node ))
			&& Node->ValueTypeGet()->ParentLink( SDB_NODE_PURPLE_PARENT( Node ));

	Node->m_Argc = Argc;
	Node->NodeTypeSet( Node->ComputeIsTerminal() ? SDB_NODE_TYPE_TERMINAL : SDB_NODE_TYPE_UNKNOWN );

	// Link Arguments as parents
	for( int Num = 0; Status && Num < Argc; ++Num )
	{
		SDB_NODE *Arg = SDB_NODE_FROM_PURPLE_CHILD( Argv[ Num ]);
		// Check if the terminal really is an each
		if( Node->NodeTypeGet() == SDB_NODE_TYPE_TERMINAL && Arg->IsLiteral() && Arg->m_Value.DataType == DtEach )
			Node->NodeTypeSet( SDB_NODE_TYPE_EACH );

		Status = Arg->ParentLink( SDB_NODE_PURPLE_PARENT( Node ));
		*Node->ArgGetAddr( Num ) = SDB_NODE_PURPLE_CHILD( Arg );
	}

	if( !Status )
	{
		if( SecDbTraceFunc )
		{
			(SecDbTraceFunc)( SDB_TRACE_DONE_NEW, Node );
			(SecDbTraceFunc)( SDB_TRACE_ERROR_NODE );
		}
		Free( Node );
		return( NULL );
	}
	if( position != nodes().end() )
		nodes().insert( position, Node );
	else
		nodes().insert( Node );

	if( BestNode && BestNode->DbSetGet()->Member( Db ) )
		BestNode->RemoveDbFromParents( Db );

	// Compute Node flags
	if( Node->IsTerminalQuick() )
	{
		SDB_VALUE_TYPE
				Vt = (SDB_VALUE_TYPE) ValueType->m_Value.Data.Pointer;
		
		SDB_VALUE_TYPE_INFO
				*NodeVti;

		// New node. Cases:
		//	1. Have our own special VTI
		//		In this case creating that VTI node has called us and it will fix the node after we are done
		//	2. Best node has a special VTI
		//		Refind it's VTI node
		//	3. Best node has class VTI
		//		Refind it's VTI node
		//		unless the Object has an ObjectVTI in which case we must find the Default VTI node again
		//	3. No best node
		//		Use Default VTI node

		SDB_NODE* Vti;
		if( BestNode )
		{
			if( ((SDB_OBJECT *) Node->ObjectGet()->m_Value.Data.Pointer)->HaveObjectVti
				&& ((SDB_VALUE_TYPE) BestNode->VtiNodeGet()->ValueTypeGet()->m_Value.Data.Pointer)->ID != SecDbValueValueFunc->ID )
				// all bets are off. We must refind the VTI entirely if it is not the node's own VTI
				Vti = FindDefaultVtiNode( Db, Object, ValueType, SDB_NODE::CREATE );
			else
				// We would like to not create the node if it wasn't found, but we can't do that
				// correctly because we don't have enough information. For example, if there is a node
				// in 1;sim (which is diddled) and we ask for it in 2;1;sim, we cannot share it if it's children
				// are invalid, so we must create it.
				Vti = BestNode->VtiNodeGet()->FindAgain( CREATE, Db, ((SDB_OBJECT *) BestNode->VtiNodeGet()->ObjectGet()->m_Value.Data.Pointer )->Db == SecDbClassObjectsDb ? SHARE : DO_NOT_SHARE );
		}
		else
			Vti = FindDefaultVtiNode( Db, Object, ValueType, SDB_NODE::CREATE );

		if( !Vti )
		{
			ErrMore( "Could not create VTI node while creating '%s'", Node->GetText(0).c_str() );
			goto DestroyAndDie;
		}
		Node->VtiNodeSet( Vti );

		// If Vti node is the same as Node then no point trying to get it as it will fail.
		// this case only happens for creation of Class Value Func( Class, Class Value Func ) during class startup
		if( Node->VtiNodeGet() != Node && ( NodeVti = Node->VtiGet() ) )
		{
			if( ( NodeVti->ValueFlags & SDB_VALUE_FUNC_PER_INSTANCE ) && !( NodeFindVtiNode( Db, Object, ValueType, Argc, Argv )))
				goto DestroyAndDie;
			if( ( NodeVti->ValueFlags & SDB_TRACK_NODES_WITH_ARGS ) && !InvalidateArgNode( Db, Object, ValueType ) )
				goto DestroyAndDie;
		}

		if( Argc >= 1 )
		{
			if( Vt->ID == SecDbValueValueFunc->ID )
			{
				if( !NodeRedirectValueFunc( Node, Object, SDB_NODE_FROM_PURPLE_CHILD( Argv[ 0 ]), Argc - 1, Argv + 1, Db ))
					goto DestroyAndDie;
			}
			else if( Vt->ID == SecDbValueObjectValueFunc->ID )
			{
				if( !RedirectObjectValueFunc( Node, Object, SDB_NODE_FROM_PURPLE_CHILD( Argv[ 0 ]), Db ))
					goto DestroyAndDie;
			}
		}
	}

	// If Best then if Best is diddled or set make sure it makes it across here
	if( BestNode )
	{
		if( BestNode->DiddleGet() )
		{
			if( SDB_DBSET_BEST::Lookup( Node->DbSetGet(), BestNode->DbSetGet()->BaseDb() )->m_Shareable )
			{
				// If we are moving the BaseDb from the best node then we have to move the whole diddle
				// stack over to the new node and put a notmydiddle on the bestnode
				if( Db == BestNode->DbSetGet()->BaseDb() )
				{
					if( !BestNode->DiddleStackMoveTo( Node ))
						goto DestroyAndDie;
				}
				// else if we were moving any but the base db from the node, then we simply put a notmydiddle
				// on the new node
				else if( !Node->PlaceNotMyDiddleFrom( BestNode ))
					goto DestroyAndDie;
			}
		}
		else if( BestNode->ValidGet() && BestNode->CacheFlagsGet() & SDB_CACHE_SET ) // keep the cached value
		{
			Node->CacheFlagsSet( SDB_CACHE_SET );
			Node->ValidSet( true );
			DTM_ASSIGN( &Node->m_Value, &BestNode->m_Value );
		}
	}

  DoneNew:
	if( SecDbTraceFunc )
		(SecDbTraceFunc)( SDB_TRACE_DONE_NEW, Node );

	return Node;

  DestroyAndDie:
	Destroy( Node, NULL );
	Node = NULL;
	goto DoneNew;
}



/****************************************************************
**	Routine: SDB_NODE::FindTerminalInt
**	Returns: Found or created node, NULL for error
**	Action : Looks for current identical node; if not found,
**			 creates it
****************************************************************/

SDB_NODE *SDB_NODE::FindTerminalInt(
	SDB_OBJECT		*SecPtr,
	SDB_VALUE_TYPE	ValueType,
	int				Argc,
	SDB_NODE		**Argv,
	EnumCreate		CreateNode,
	SDB_DB			*Db,
	EnumShare		Share
)
{
	DT_VALUE
			TmpVal;

	SDB_NODE
			*ObjectNode,
			*ValueTypeNode;


	if( !Db )
	{
		Err( ERR_INVALID_ARGUMENTS, "FindTerminalInt(): Null Db" );
		return NULL;
	}
	// Pretend that ChildDb of a SearchPath Db are the same as the ParentDb
	Db = Db->ParentDb;

	DTM_INIT( &TmpVal );

	TmpVal.DataType		= DtSecurity;
	TmpVal.Data.Pointer	= (void *) SecPtr;
	ObjectNode = FindLiteral( &TmpVal, Db );
	if( !ObjectNode )
		return( NULL );

	TmpVal.DataType		= DtValueType;
	TmpVal.Data.Pointer	= (void *) ValueType;
	ValueTypeNode = FindLiteral( &TmpVal, Db );
	if( !ValueTypeNode )
		return( NULL );

	return( Find( ObjectNode, ValueTypeNode, Argc, Argv, CreateNode, Db, Share ));
}



/****************************************************************
**	Routine: SDB_NODE::FindTerminal
**	Returns: Found or created node, NULL for error
**	Action : Looks for current identical node; if not found,
**			 creates it.  First checks that vt( secptr ) is
**			 supported.
****************************************************************/

SDB_NODE *SDB_NODE::FindTerminal(
	SDB_OBJECT		*SecPtr,
	SDB_VALUE_TYPE	ValueType,
	int				Argc,
	SDB_NODE		**Argv,
	SDB_DB			*Db,
	EnumShare		Share,
	EnumSupported	CheckSupported
)
{
	if( !SecPtr || !ValueType )
	{
		Err( ERR_INVALID_ARGUMENTS, "FindTerminal: NULL SecPtr or ValueType" );
		return( NULL );
	}
	return FindTerminalInt( SecPtr, ValueType, Argc, Argv, CREATE, Db, Share );
}



/****************************************************************
**	Routine: SDB_NODE::FindTerminalAndArgs
**	Returns: Found or created node, NULL for error
**	Action : First finds all the args with FindLiteral, then
**			 calls FindTerminal
****************************************************************/

SDB_NODE *SDB_NODE::FindTerminalAndArgs(
	SDB_OBJECT		*SecPtr,
	SDB_VALUE_TYPE	ValueType,
	int				Argc,
	DT_VALUE		**Argv,
	EnumSupported	CheckSupported,
	SDB_DB			*Db,
	EnumShare		Share
)
{
	int		ArgNum;

	SDB_NODE
			**NodeArgv = NULL;

	if( !SecPtr )
		return (SDB_NODE *) Err( ERR_INVALID_ARGUMENTS, "Security Pointer is null" );

	if( !ValueType )
		return (SDB_NODE *) Err( ERR_INVALID_ARGUMENTS, "ValueType Pointer is null" );

	if( Argc > 0 )
	{
		NodeArgv = (SDB_NODE **) alloca( sizeof( SDB_NODE * ) * Argc );

		for( ArgNum = 0; ArgNum < Argc; ArgNum++ )
		{
			NodeArgv[ ArgNum ] = FindLiteral( Argv[ ArgNum ], Db );
			if( !NodeArgv[ ArgNum ] )
				return( NULL );
		}
	}

	return( FindTerminal( SecPtr, ValueType, Argc, NodeArgv, Db, Share, CheckSupported ));
}



/****************************************************************
**	Routine: SDB_NODE::LiteralExtend
**	Returns: void
**	Action : Extends the DbSet of a literal to include the Db
**			 It will not extend the DbSet for securities
****************************************************************/

inline void SDB_NODE::LiteralExtend(
	SDB_DB		*Db
)
{
	SDB_OBJECT
			*SecPtr;

	// if it's not a security or it's the CastObject/SubscriptObject/ClassObject
	// then it's DbSet is extensible
	if(
		// if not a security then OK to extend
		m_Value.DataType != DtSecurity

		// Else if it is one of the special global objects CastObject/SubscriptObject/ClassObject
		// then ok to extend too
		|| this == CastObject || this == SubscriptObject
		|| ( SecPtr = (SDB_OBJECT *) m_Value.Data.Pointer )->Db == SecDbClassObjectsDb

		// or if the physical Db of the SecPtr is the same then extend it too
		|| SecPtr->Db->ParentDb == Db->DbPhysical )
	{
		if( SecDbTraceFunc && !DbSetGet()->Member( Db ) )
			(SecDbTraceFunc)( SDB_TRACE_NODE_SDB_DBSET_EXTEND, this, Db );
		
		DbSetSet( SDB_DBSET_PLUS( DbSetGet(), Db ));
	}
}



/****************************************************************
**	Routine: SDB_NODE::FindLiteral
**	Returns: Found or created literal node, NULL for error
**	Action : Looks for current identical literal node; if not
**		     found, creates it
****************************************************************/

SDB_NODE *SDB_NODE::FindLiteral(
	DT_VALUE	*Value,
	SDB_DB		*Db
)
{
	SDB_NODE
			*Literal;

	if( !Db )
	{
		Err( ERR_DATABASE_INVALID, "FindLiteral(): Db is NULL" );
		return NULL;
	}
	Db = Db->ParentDb;

	Literal = (SDB_NODE *) HashLookup( s_LiteralHash, (HASH_KEY) Value );
	if( Literal )
	{
		Literal->LiteralExtend( Db );
		return( Literal );
	}

	if( !( Literal = Alloc()) )
		return( FALSE );

    Literal->NodeTypeSet      ( SDB_NODE_TYPE_REF_COUNTED_LITERAL );
    Literal->ChildrenValidSet ( true                              );
    Literal->ValidSet         ( true                              );
    Literal->DbSetSet         ( Db->DbSingleton                   );

	// Special case DtSecurity so the literal node does not look like another reference.
	if( Value->DataType == DtSecurity )
	{
		SDB_OBJECT *SecPtr = (SDB_OBJECT *) Value->Data.Pointer;

		if( !SecPtr )
		{
			Err( ERR_UNSUPPORTED_OPERATION, "Cannot construct literal for an Undefined Security" );
			goto ReturnErr;
		}
		if( !DbToPhysicalDb( SecPtr->Db ))
		{
			Err( ERR_UNSUPPORTED_OPERATION, "Cannot construct literal of security %s from non physical database %s", SecPtr->SecData.Name, SecPtr->Db ? SecPtr->Db->FullDbName.c_str() : "<long closed>" );
			goto ReturnErr;
		}

        Literal->NodeTypeSet ( SDB_NODE_TYPE_PARENTED_LITERAL    );
        Literal->DbSetSet    ( SecPtr->Db->ParentDb->DbSingleton );
        Literal->m_Value     = *Value                            ;
	}
	else if( Value->DataType == DtDiddleScope )
	{
		// Do not make a reference for a diddle scope. The diddle scope takes care of destroying the literal node
		// when it is freed
        Literal->NodeTypeSet   ( SDB_NODE_TYPE_PARENTED_LITERAL );
        Literal->m_Value	   = *Value;
        Literal->CacheFlagsSet ( SDB_CACHE_NO_FREE );
	}
	else if( LiteralHashCmp( Value, Value ) != 0 )
	{
		Err( ERR_UNSUPPORTED_OPERATION, "Cannot construct literal when Value != Value for datatype %s", Value->DataType->Name );
		goto ReturnErr;
	}
	else if( !DTM_ASSIGN( &Literal->m_Value, Value ))
		goto ReturnErr;
	HashInsert( s_LiteralHash, (HASH_KEY) &Literal->m_Value, (HASH_VALUE) Literal );

	if( SecDbTraceFunc )
	{
		(SecDbTraceFunc)( SDB_TRACE_NEW, Literal );
		(SecDbTraceFunc)( SDB_TRACE_DONE_NEW, Literal );
	}

	return( Literal );

ReturnErr:
	Free( Literal );
	return NULL;
}



/****************************************************************
**	Routine: SDB_NODE::FindAgain
**	Returns: SDB_NODE *
**	Action : finds Node in the Db requested
****************************************************************/

SDB_NODE *SDB_NODE::FindAgain(
	EnumCreate	CreateNode,
	SDB_DB		*Db,
	EnumShare	Share
)
{
	if( IsLiteralQuick() )
	{
		if( !Db )
		{
			Err( ERR_DATABASE_INVALID, "FindLiteral(): Db is NULL" );
			return SDB_NODE_INVALID_PTR;
		}
		// FIX- This is unsafe for security literals because it will not extend the DbSet
		// however the code used to call FindLiteral which would have done the same. But I don't really
		// what it means to extend the DbSet of a Security Literal to another database.
		// This should probably error if it is a security and Db is not a member, but I'm not sure it's worth
		// the expense
		LiteralExtend( Db->ParentDb );
		return this;
	}
	else
		return Find( ObjectGet(), ValueTypeGet(), ArgcGet(), ArgGetAddr(0), CreateNode, Db, Share );
}



/****************************************************************
**	Routine: SDB_NODE::SplitChildren
**	Returns: SDB_NODE *
**	Action : Returns a Node whose children all support Db within
**			 their DbSets
****************************************************************/

SDB_NODE *SDB_NODE::SplitChildren(
	SDB_DB		*Db,
	EnumCreate	CreateNode
)
{
    // guard classes
    class TraceFuncGuard
	{
	public:
	    TraceFuncGuard( SDB_NODE* Node, SDB_DB* Db ) : m_Node( Node ), m_Db( Db )
	    { if( SecDbTraceFunc ) SecDbTraceFunc( SDB_TRACE_NODE_SPLIT, m_Node, m_Db );}

	    ~TraceFuncGuard()
	    { if( SecDbTraceFunc ) SecDbTraceFunc( SDB_TRACE_NODE_SPLIT_DONE, m_Node, m_Db ); }

	private:
	    SDB_NODE* m_Node;
	    SDB_DB*   m_Db;
	};

	class SplitChildrenLockGuard
	{
	public:
	    SplitChildrenLockGuard( SDB_NODE* Node ) : m_Node( Node )
	    { m_Node->SplitChildrenLockSet( true ); }

	    ~SplitChildrenLockGuard()
	    { m_Node->SplitChildrenLockSet( false ); }

	private:
	    SDB_NODE* m_Node;
	};

	// SplitChildren cycle, simply return the node
	if( SplitChildrenLockGet() )
		return this;

	TraceFuncGuard         TraceFunc( this, Db );
	SplitChildrenLockGuard LockGuard( this );

	if( !IsLiteralQuick() )
	{
		// if we are trying to span databases in the graph, then switch dbs
		if( ObjectGet()->DbSetGet()->BaseDb()->ParentDb->DbPhysical != Db->DbPhysical )
			Db = ObjectGet()->DbSetGet()->BaseDb()->ParentDb->DbPhysical;

		SDB_NODE* const NodeObject    = ObjectGet()->FindAgain( CreateNode, Db, SHARE );

		if( !IsNode( NodeObject ))
		    return SDB_NODE_INVALID_PTR;

		SDB_NODE* const NodeValueType = ValueTypeGet()->FindAgain( CreateNode, Db, SHARE );

		if( !IsNode( NodeValueType ))
		    return SDB_NODE_INVALID_PTR;

		DT_VALUE* const ObjectValue = ObjectGet()->ValidGet() ? ObjectGet()->AccessValue() : 0;
		SDB_DB*   const ChildDb     = (( ObjectValue && ObjectValue->DataType == DtDiddleScope ) ?
									   (static_cast<DT_DIDDLE_SCOPE const*>( ObjectValue->Data.Pointer ))->Db :
									   Db);

		// Cases:
		// 1. Have Class VTI:
		//		This clearly means we do not have any node VTI in any Db for this node. Because if we did then creating that
		//		Vti node would have pointed this node's VTI away from the class VTI.
		//	So in this case it is sufficient to refind the VTI node in Db.
		//		Unless the object has an Object VTI. In this case we must find the Default VTI node again in this Db to
		//		see if it is different from the class VTI
		// 2. Have Node VTI:
		//		Node VTIs always take precedence over class VTIs. So even if we have diddled the Class VTI in Db we still
		//		want the node's VTI.
		//  So again in this case it is sufficient to refine the VTI node in Db.

		bool SplitVti      = false;

		if( VtiNodeGet() && VtiNodeGet() != this )
		{
			static SDB_NODE* const
			        VFNode = LinkLiteral( DtValueType, SecDbValueValueFunc );

			if( VtiNodeGet()->ValueTypeGet() != VFNode &&
				ObjectValue && ObjectValue->DataType == DtSecurity &&
				(static_cast<SDB_OBJECT const*>( ObjectValue->Data.Pointer ))->HaveObjectVti )
				SplitVti = FindDefaultVtiNode( Db, ObjectGet(), ValueTypeGet(), CreateNode ) != VtiNodeGet();
			else
				SplitVti = VtiNodeGet()->FindAgain( CreateNode, Db, SHARE ) != VtiNodeGet();
		}

		bool SplitChildren = false;

		if( ChildrenValidGet() )
		{
			if( !SplitVti )
			{
				// if node is diddled and the Db will be the new base db of the DbSet, then we cannot
				// share because it will mean that the diddle will belong to the new Db which is wrong
				// FIX- this can be more efficient because if the diddle is a notmydiddle, then it is ok
				// for the the new Db to own the diddle since it is from belowwhere
 				if( DiddleGet() )
 				{
 					SDB_DBSET_BEST* BestDbSet = SDB_DBSET_BEST::Lookup( DbSetGet(), Db );

 					if( BestDbSet->m_Shareable && !BestDbSet->m_Member )
 						SplitChildren = true;
 				}
				if( !SplitChildren )
				{
					// We need to be split if any of our children are split

					// search for a split child
					// Efficiency hack: we want to go backwards because the purple children are more likely to be split
					for( int i = ChildcGet() - 1; i >= 0; --i )
					{
						SDB_NODE* const Child = ChildGet( i );
						if( Child && Child->FindAgain( CreateNode, ChildDb, SHARE ) != Child )
						{
							if( SecDbTraceFunc )
								SecDbTraceFunc(SDB_TRACE_NODE_SPLIT_CHILD, Child );
							SplitChildren = true;
							break;
						}
					}

					// check auto children (mostly exclusive of normal children, but not guaranteed)
					if( !SplitChildren )
					{
						const NodeSet *AutoChildren = AutoChildrenGet();
						if( AutoChildren )
						{
							for( NodeSet::iterator iter = AutoChildren->begin(); iter != AutoChildren->end(); ++iter )
							{
								if( (*iter)->FindAgain( CreateNode, ChildDb, SHARE ) != (*iter) )
								{
									if( SecDbTraceFunc )
										SecDbTraceFunc( SDB_TRACE_NODE_SPLIT_CHILD, (*iter) );
									SplitChildren = true;
									break;
								}
							}
						}
					}
				}
			}
		}
		else if( ( CacheFlagsGet() & SDB_CACHE_DIDDLE ) )
		{
			SDB_DBSET_BEST* BestDbSet = SDB_DBSET_BEST::Lookup( DbSetGet(), Db );

			if( BestDbSet->m_Shareable && !BestDbSet->m_Member )
										// force split if node is diddled and children are invalid
				SplitChildren = true;		// because there may be a child which will cause us to split
		}

		if( SplitChildren || ObjectGet() != NodeObject || ValueTypeGet() != NodeValueType || SplitVti )
		{
			if( !CreateNode )
			    return SDB_NODE_INVALID_PTR;

			if( SecDbTraceFunc )
			{
				if( NodeObject != ObjectGet() )
					SecDbTraceFunc( SDB_TRACE_NODE_SPLIT_CHILD, NodeObject );
				else if( NodeValueType != ValueTypeGet() )
					SecDbTraceFunc( SDB_TRACE_NODE_SPLIT_CHILD, NodeValueType );
				else if( !SplitChildren )
					SecDbTraceFunc( SDB_TRACE_NODE_SPLIT_CHILD, VtiNodeGet() );
			}

			SDB_NODE* const NewNode = Create( Db, NodeObject, NodeValueType, ArgcGet(), ArgGetAddr(0), this, nodes().end() );

			RemoveDbFromParents( Db );

			return NewNode;
		}
	}

	// Extend DbSet of Node with Db
	if( SecDbTraceFunc )
	{
		if( !DbSetGet()->Member( Db ))
			(SecDbTraceFunc)( SDB_TRACE_NODE_SPLIT_EXTEND, this, Db );
	}

	DbSetSet( SDB_DBSET_PLUS( DbSetGet(), Db ));
	return this;
}



/****************************************************************
**	Routine: SDB_NODE::RemoveDbFromParents
**	Returns: TRUE/Err
**	Action : removes Db from DbSet of Node 
****************************************************************/

bool SDB_NODE::RemoveDbFromParents(
	SDB_DB		*Db
)
{
    // if I'm not a node - nothing to do
	if( !IsNode( this ))
		return true;

	if( SecDbTraceFunc )
		(SecDbTraceFunc)( SDB_TRACE_NODE_REMOVE_DB, this, Db );

	SDB_DBSET* const NewDbSet = SDB_DBSET_MINUS( DbSetGet(), Db );
	if( NewDbSet == DbSetGet() )
		return true;

	if( DiddleGet() && SetOrDiddleLockGet() ) // Node is diddled and being diddled, we can't possible remove the Db
	{
		if( !GetValueLockGet() )		// not locked, can invalidate children
			return InvalidateChildren();          // simple invalidate it's children
		// else doing getvalue/buildchildren, set ChildDbChanged to rebuild
		ChildDbChangedSet( true );
		DbSetSet( NewDbSet );
		return true;
	}

	DbSetSet( NewDbSet );

	if( DiddleGet() && DbSetGet()->Size() && Db->DbDepth < DbSetGet()->Db()->DbDepth )
	{
		SDB_DBSET_BEST* const BestDbSet = SDB_DBSET_BEST::Lookup( DbSetGet(), Db );

		// If the whole DbSet is shareable by the Db, then it is not enough to simply remove the Db, we must create
		// the new node and move the diddle stack
		if( BestDbSet->m_Shareable )
		{
			SDB_NODE* const NewNode = FindAgain( CREATE, Db, DO_NOT_SHARE );
			if( IsNode( NewNode ) && !DiddleStackMoveTo( NewNode ))
				return false;
		}
	}

	// FIX- We really need better error reporting out of this
	// function. Right now it just fails silently
	for( int Num = 0; Num < ParentsMax(); ++Num )
	{
		SDB_NODE* const Parent = ParentGet( Num );
		// No need to remove Db from parent if Parent is a literal
		// Literals support all Dbs by definition. Except for security literals which support all Db's extended
		// from their physical Db. And of course they have only one physical Db
		if( IsNode( Parent ) && !Parent->IsLiteralQuick() )
		{
			DT_VALUE* const ParentObjValue = Parent->ObjectGet()->ValidGet() ? Parent->ObjectGet()->AccessValue() : 0;
			SDB_DB*         ExpectedChildDb2; // used as a temp in an if()

			if( Parent->GetValueLockGet() )
			{
				SDB_DB* const ExpectedChildDb = Parent->ExpectedChildDb();
				// If Db != Parent->DbSetGet()->Db() then we can simply recurse because the children of the Parent
				// are not invalid

				// However if this is not the case, this particular child is invalid for the parent
				// and we must refind the child

				// Check to see if it is safe to recurse (i.e., we are removing a Db of no consequence to the current GetValue operation)
				if( ( !ParentObjValue || ParentObjValue->DataType != DtDiddleScope )
					&& DbSetGet()->Member( ExpectedChildDb ))
				{
					if( !Parent->RemoveDbFromParents( Db ))
						return false;
					continue; // go on to next parent
				}

				// special case: If Parent's object is a Db, then it's child is constrained to be in that Db
				// so we must be very careful to maintain that relationship.

				// otherwise refind the child and whack it in
				bool FoundAtLeastOne = false;

				for( int ii = 0; ii < Parent->ChildcGet(); ++ii )
				{
				    // search for yourself in the parent's child list
				    if( Parent->ChildGet( ii ) != this )
					    continue;

					FoundAtLeastOne = true;

					SDB_NODE* Child = FindAgain( CREATE, ExpectedChildDb, SHARE );

					if( ParentObjValue && ParentObjValue->DataType == DtDiddleScope )
					{
						if( Child == this )
						    break;	// no change necessary, child is correct
						if( !IsNode( Child ))
						    return ErrMore( "Tried to fix up child %d of %s but could not find it again", ii, Parent->GetText(0).c_str() );
					}
					else
					{
						if( !IsNode( Child ) )
						{
							if( !Parent->PassErrorsGet() )
							    return ErrMore( "Tried to fix up child %d of %s but could not find it again", ii, Parent->GetText(0).c_str() );
							Child = 0;
						}

						if( Child == this )	// Ouch, something wierd has happened
						    return ErrMore( "Tried to fix up child %d of %s but found the same incorrect one", ii, Parent->GetText(0).c_str() );
					}

					// Inform the Parent that one of its children's Db's has changed
					// so that it will repeat the current Build/GetValue operation
					Parent->ChildDbChangedSet( true );

					SDB_NODE* ParentLink;
					if( ParentGet( Num ).IsPurple() )
					{
						ParentLink = SDB_NODE_PURPLE_PARENT( Parent );
						if( !Parent->ChildGet( ii )->ParentUnlink( ParentLink ))
						    return ErrMore( "ParentUnlink failed while fixing up child %d of %s", ii, Parent->GetText(0).c_str() );
						*Parent->ChildGetAddr( ii ) = SDB_NODE_PURPLE_CHILD( Child );
					}
					else
					{
						ParentLink = Parent;
						if( !Parent->ChildGet( ii )->ParentUnlink( ParentLink ))
						    return ErrMore( "ParentUnlink failed while fixing up child %d of %s", ii, Parent->GetText(0).c_str() );
						*Parent->ChildGetAddr( ii ) = Child;
					}
					if( Child )
					{
						if( !Child->ParentLink( ParentLink ))
						    return ErrMore( "ParentLink failed while fixing up child %d of %s", ii, Parent->GetText(0).c_str() );

						// Check to see if all the Db's in the Parent's DbSet are members of the Child's DbSet
						// remove the ones which aren't down with it.
						for( int DbNum = 0; DbNum < Parent->DbSetGet()->Size(); ++DbNum )
								if( !Child->DbSetGet()->Member( Parent->DbSetGet()->Elem( DbNum )))
									Parent->RemoveDbFromParents( Parent->DbSetGet()->Elem( DbNum-- ));	// DbSet has shrunk
					}
					if( !Parent->DuplicateChildrenGet() )
					    break; // if this is true, we should have exactly one entry in our parent's child list - no need to look further
				} // for

				if( Parent->VtiNodeGet() == this )
				{
					SDB_NODE* Vti = FindAgain( CREATE, ExpectedChildDb, SHARE );
					if( !Vti )
						return ErrMore( "Could not refind VTI for node %s", Parent->GetText(0).c_str() );
					Parent->VtiNodeSet( Vti );
				}
				else if( !FoundAtLeastOne )
				{
					const NodeSet* auto_children = Parent->AutoChildrenGet();
					if( auto_children->find( this ) == auto_children->end() )
						return ErrMore( "graph inconsistent, child %s had db %s but parent %s did not.", GetText(0).c_str(), Db->FullDbName.c_str(), Parent->GetText(0).c_str() );
				}

				// no need to fix up autochildren, the getvalue will be repeated because ChildDbChanged

				Parent->ChildDbChangedSet( true );
			}
			// If the ExpectedChildDb is not in DbSet of Node, then we invalidate the parent and it's children
			else if( ( ExpectedChildDb2 = Parent->ExpectedChildDb( Parent->DbSetGet()->BaseDb() )) && !DbSetGet()->Member( ExpectedChildDb2 ))
			{
				if( !Parent->InvalidateSafe() || !Parent->InvalidateChildren() )
					return ErrMore( "%s lost its base db but invalidation failed", Parent->GetText(0).c_str() );
			}
			// need to think more carefully about what happens when Db is removed from a parent thus creating an empty DbSet for it
			// I think it works because it will just not be found as a valid node, and will get recreated
			// might want to consider destroying it in this case. Need delayed destruction feature.
			else if( !Parent->RemoveDbFromParents( Db ))
				return false;
		}
	}
	return true;
}



/****************************************************************
**	Routine: FindClassObject
**	Returns: SDB_NODE * or NULL for error
**	Action : Finds the class object
****************************************************************/

static SDB_NODE *FindClassObject(
	SDB_NODE	*ObjectNode,
	SDB_DB		*Db
)
{
	SDB_NODE_CONTEXT
			Ctx;

	DT_VALUE
			*ObjectValue;

	if( !ObjectNode->ValidGet() && !ObjectNode->GetValue( &Ctx, Db ))
	{
		ErrMore( "FindClassObject: Cannot get Object value" );
		return NULL;
	}
	ObjectValue = ObjectNode->AccessValue();
	if( ObjectValue	&& !( ObjectValue->DataType->Flags & DT_FLAG_SDB_OBJECT ))
	{
		if( !( ObjectNode = ObjectNode->FindCastTo( ObjectValue->DataType == DtString ? DtSecurity : DtSecurityList, Db, SDB_NODE::SHARE )))
			return NULL;
		ObjectValue = ObjectNode->AccessValue();
	}
	if( !ObjectValue )
	{
		Err( ERR_INVALID_ARGUMENTS, "@ Find: Cannot get Object value" );
		return NULL;
	}
	if( ObjectValue->DataType != DtSecurity )
		return ObjectNode;

	return ((SDB_OBJECT *) ObjectValue->Data.Pointer )->SdbClass->ClassSecNode;
}



/****************************************************************
**	Routine: SDB_NODE::Find
**	Returns: Found or created node, NULL for error
**	Action : Looks for current identical node; if not found,
**			 creates it.
****************************************************************/

// helper class to convert a DtArray to a more efficient representation
// Like a DtSecurityList or DtDiddleScopeList
struct Convert_DtArray
{
    typedef bool     result_type;
    typedef DT_VALUE argument_type;

    Convert_DtArray( bool& security, bool& diddle_scope ) : m_sec(security), m_diddle(diddle_scope) {}
    bool operator()( DT_VALUE const& val ) const
    {
	    m_sec    = m_sec    && ( val.DataType == DtSecurity    || val.DataType == DtString          );
		m_diddle = m_diddle && ( val.DataType == DtDiddleScope || val.DataType == DtDiddleScopeList );
		return !(m_sec || m_diddle ); // stop looking if we can be neither
	}

    bool& m_sec;
    bool& m_diddle;
};

SDB_NODE *SDB_NODE::Find(
	SDB_NODE	*Object,
	SDB_NODE	*ValueType,
	int			Argc,
	SDB_NODE	**Argv,
	EnumCreate	CreateNode,
	SDB_DB		*Db,
	EnumShare	Share
)
{
	SDB_NODE
			*Node;

	SDB_NODE_CONTEXT
			Ctx;

	DT_VALUE
			*ObjectValue = NULL,
			*ValueTypeValue;

	EnumShare
			OkToShare = SHARE;


	if( !Object || !ValueType )
		return( NULL );

	if( Argc > SDB_NODE_MAX_ARGS )
	{
		Err( ERR_INVALID_NUMBER, "Find, @, Argc > %d (was %d)", Argc, SDB_NODE_MAX_ARGS );
		return NULL;
	}

	// if we are trying to span databases in the graph, then switch dbs
	if( Object->DbSetGet()->BaseDb()->ParentDb->DbPhysical != Db->DbPhysical )
		Db = Object->DbSetGet()->BaseDb()->ParentDb->DbPhysical;


/*
**	Resolve any type conversion for Object and/or ValueType
*/

	if( !ValueType->ValidGet() )
	{
		memset( &Ctx, 0, sizeof( Ctx ));
		(void) ValueType->GetValue( &Ctx, Db );
	}
	if( ValueType->ValidGet() )
	{
		ValueTypeValue = ValueType->AccessValue();
		if( ValueTypeValue && !( ValueTypeValue->DataType->Flags & DT_FLAG_VALUE_TYPE ))
		{
			ValueType = ValueType->FindCastTo( DtValueType, Db, SHARE );
			if( !ValueType )
				return NULL;

			ValueTypeValue = ValueType->AccessValue();
		}

		if( ValueTypeValue && ValueTypeValue->DataType == DtValueType )
		{
			SDB_VALUE_TYPE
					ValueType = (SDB_VALUE_TYPE) ValueTypeValue->Data.Pointer;

			if( !ValueType )
			{
				Err( ERR_INVALID_ARGUMENTS, "@: ValueType is Null in Find." );
				return NULL;
			}

			// can never share Diddle Scope( ... ) nodes because their eval result depends on the database in which they exist
			if(	ValueType == SecDbValueDiddleScope )
				Share = OkToShare = DO_NOT_SHARE;
			else if( ValueType->ClassStaticVt && !( Object = FindClassObject( Object, Db )))
				return NULL;
		}
	}

	if( !Object->ValidGet() )
	{
		memset( &Ctx, 0, sizeof( Ctx ));
		(void) Object->GetValue( &Ctx, Db );
	}
	if( Object->ValidGet() )
	{
		ObjectValue = Object->AccessValue();
		if( ObjectValue
				&& !( ObjectValue->DataType->Flags & DT_FLAG_SDB_OBJECT ))
        {
		    DT_DATA_TYPE target_type = DtEach; // default to converting this into an each

		    // convert all scalar values to DtSecurities
		    if( ObjectValue->DataType == DtString )
			{
			    target_type = DtSecurity;
			}
			else if( ObjectValue->DataType == DtArray ) // efficiency hack only: 
			     // try to convert this array into something better than a DtEach ( DtSecurityList or DtDiddleScopeList )
			{
			    bool could_be_security     = true;
				bool could_be_diddle_scope = true;
				DT_ARRAY* Array = (DT_ARRAY*) ObjectValue->Data.Pointer;
				CC_USING( std::find_if );
				find_if( Array->Element, Array->Element + Array->Size,
						 Convert_DtArray( could_be_security, could_be_diddle_scope ));
				if( could_be_diddle_scope ) target_type = DtDiddleScopeList;
				if( could_be_security     ) target_type = DtSecurityList;
			}

			Object = Object->FindCastTo( target_type, Db, SHARE );
			if( !Object )
				return NULL;
		}
	}


	// see if node exists (or find best match)
	SDB_NODE node_key;
	SDB_NODE** node_children = (SDB_NODE**) alloca( sizeof( SDB_NODE * ) * ( Argc + 3 ) );

	node_children[0] = Object;
	node_children[1] = ValueType;
	if( Argc )
		memcpy( node_children + 3, Argv, sizeof( SDB_NODE* ) * Argc );

    node_key.m_Argc = Argc;
    node_key.m_children = node_children;
	node_key.m_DbSet = 0;

	SDB_NODE
			*Best = NULL;

	SDB_DBSET_BEST
			*BestDbSet = NULL;

	SDB_NODE_SPLIT *Splits = NULL;		// list of nodes & db to split on

	bool have_physical_db = false;

	// iterate over matching nodes
	nodes_iterator first_node = nodes().lower_bound( &node_key );
	for( nodes_iterator it = first_node; it != nodes().end() && equal( &node_key, *it ); ++it )
	{
		SDB_DBSET_BEST *NodeDbSetBest = SDB_DBSET_BEST::Lookup( (*it)->DbSetGet(), Db );

		if( !NodeDbSetBest )			// some error happened in looking up the DbSetBest
			return NULL;

		// if Db is a member of Node->DbSetGet() then consider returning it
		if( NodeDbSetBest->m_Member &&
			( Share == SHARE
			  || ( NodeDbSetBest->m_Shareable && ( OkToShare == SHARE || (*it)->DbSetGet() == Db->DbSingleton ) ) ) )
			return *it;

		// if this DbSet is better than the other one's we found then consider it
		if( DbSetBestGreater( NodeDbSetBest, BestDbSet ))
		{
			Best = (*it);
			BestDbSet = NodeDbSetBest;
		}

		// add to list of splits if necessary
		if( NodeDbSetBest->m_NumSplits > 0 )
		{
			SDB_NODE_SPLIT *Split = (SDB_NODE_SPLIT *) alloca( sizeof( SDB_NODE_SPLIT ));
			Split->Node = *it;
			Split->DbSetBest = NodeDbSetBest;
			Split->Next = Splits;
			Splits = Split;
		}

		have_physical_db = have_physical_db || NodeDbSetBest->m_HavePhysicalDb;
	}

	// If we have a Best node and we can share it, split it's children and return it
	// Conditions for share are:
	// If Not Diddled then we can share only if Db is shareable by the BaseDb of the diddled node.
	// note that we cannot share Diddle Scope() nodes every. For them OkToShare will always be FALSE
	// also note that if we cannot build children then we can almost never share the node
	if( Best
		&& ( Share == SHARE || BestDbSet->m_Shareable )
		&& OkToShare == SHARE )
	{
		// If Children are valid, then attempt to share
		if( Best->ChildrenValidGet() )
		{
			SDB_NODE *Node = Best->SplitChildren( Db, CreateNode );
			if( Node == SDB_NODE_INVALID_PTR )
				return (SDB_NODE *) ErrMoreN( "Find(): Failed to share the node; SplitChildren failed." );
			else
				return Node;
		}
		// Children are invalid

		// If node is invalid, or node is diddled and the DbSet is not shareable by the Db then share
		// or node is cache set
		if( !Best->ValidGet() || Best->DiddleGet() || ( Best->CacheFlagsGet() & SDB_CACHE_SET ))
		{
			bool	CanShare;

			// If we the best node is diddled but none of the nodes considered was one in the PhysicalDb
			// then we most certainly have the wrong node, so create the one in the PhysicalDb and repeat
			// the Find
			// Note that this can never recurse infinitely because the next time we will have_physical_db
			if( Best->DiddleGet() && !have_physical_db )
			{
				if( !Create( DbToPhysicalDb( Db ), Object, ValueType, Argc, Argv, Best, first_node ) )
					return (SDB_NODE*) ErrMoreN( "Find(): Failed to create Node in base scope" );

				return Find( Object, ValueType, Argc, Argv, CreateNode, Db, Share );
			}

			// If Best has a VTI node, then we can only share the node if we find the same VTI node
			// in the new Db
			if( Best->VtiNodeGet() )
			{
				SDB_NODE
						*VtiNode;

				if( ObjectValue && ObjectValue->DataType == DtSecurity &&
					((SDB_OBJECT *) ObjectValue->Data.Pointer)->HaveObjectVti )
					VtiNode = FindDefaultVtiNode( Db, Object, ValueType, DO_NOT_CREATE );
				else
					VtiNode = Best->VtiNodeGet()->FindAgain( CREATE, Db, SHARE );

				if( VtiNode == SDB_NODE_INVALID_PTR )
					return NULL;
				CanShare = VtiNode == Best->VtiNodeGet();
			}
			else
				CanShare = true;

			if( CanShare )
			{
				Best->DbSetSet( SDB_DBSET_PLUS( Best->DbSetGet(), Db ));

				if( SecDbTraceFunc )
					(SecDbTraceFunc)( SDB_TRACE_NODE_SDB_DBSET_EXTEND, Best, Db );

				return Best;
			}
		}

		// Now Best is valid and we couldn't build the children so we must create a new node
	}

	// Create new node, because we completely failed to find an acceptable node
	if( CreateNode == DO_NOT_CREATE )
		return NULL;
	else if( !( Node = Create( Db, Object, ValueType, Argc, Argv, Best, first_node )))
	{
		ErrMore( "Find(): Failed to create node" );
		return NULL;
	}

	// Split nodes which have wrong DbSets
	// no need to free Splits 'cause they are allocated off the stack
	for( ; Splits; Splits = Splits->Next )
	{
		for( int i = 0; i < Splits->DbSetBest->m_NumSplits; ++i )
		{
			SDB_DB *DbToRemove = Splits->DbSetBest->SplitDb( i );

			// it is very important to set Node->DbSet to include DbToRemove before splitting
			// because if a child needs to be fixed up, we want it to find the node which we just
			// created instead of trying to create a new node & screwing us up

			Node->DbSetSet( SDB_DBSET_PLUS( Node->DbSetGet(), DbToRemove ));
			if( !Splits->Node->RemoveDbFromParents( DbToRemove ))
			{
				Destroy( Node, NULL );
				return NULL;
			}
		}
	}
	return Node->SplitChildren( Db, CREATE );
}



/****************************************************************
**	Routine: FindCastTo
**	Returns: Found or created node
**	Action : Created node is on
**					<CastDt>( "~Cast", CastFrom )
****************************************************************/

SDB_NODE *SDB_NODE::FindCastTo(
	DT_DATA_TYPE	CastToDt,
	SDB_DB			*Db,
	EnumShare		Share
)
{
	static SDB_OBJECT
			CastSec;

	SDB_NODE
			*CastValueType,
			*Argv[ 1 ],
			*CastNode;

	DT_VALUE
			TmpVal;

	SDB_NODE_CONTEXT
			Ctx;


	if( !CastObject )
	{
		memset( &CastSec, 0, sizeof( CastSec ));
		strcpy( CastSec.SecData.Name, "~Cast" );
		CastSec.ReferenceCount	= 1;
		CastSec.SdbClass 		=
		CastSec.Class 	 		= SecDbClassInfoFromType( SecDbClassTypeFromName( "SecDb Cast" ));
		CastSec.Db 		 		= SecDbRootDb;	// nasty hack

		DTM_INIT( &TmpVal );
		TmpVal.DataType		= DtSecurity;
		TmpVal.Data.Pointer	= &CastSec;
		CastObject = FindLiteral( &TmpVal, Db );
		if( !CastObject )
			return NULL;
	}

	DTM_INIT( &TmpVal );
	TmpVal.DataType		= DtValueType;
	if( !( TmpVal.Data.Pointer = SecDbValueTypeFromName( CastToDt->Name, CastToDt )))
		return NULL;
	CastValueType = FindLiteral( &TmpVal, Db );
	if( !CastValueType )
		return NULL;

	Argv[ 0 ] = this;
	CastNode = Find( CastObject, CastValueType, 1, Argv, CREATE, Db, Share );
	if( !CastNode )
		return( NULL );

	memset( &Ctx, 0, sizeof( Ctx ));
	CastNode->GetValue( &Ctx, Db );

	return( CastNode );
}



/****************************************************************
**	Routine: SDB_NODE::FindSubscriptOf
**	Returns: Found or created node
**	Action : Created node is on
**					<RetDt>( "~Subscript", Array, Index )
****************************************************************/

SDB_NODE *SDB_NODE::FindSubscriptOf(
	DT_DATA_TYPE	RetDt,
	DT_VALUE		*Index,
	SDB_DB			*Db,
	EnumShare		Share
)
{
	static SDB_OBJECT
			SubscriptSec;

	SDB_NODE
			*SubscriptValueType,
			*Argv[ 2 ],
			*SubscriptNode;

	DT_VALUE
			TmpVal;

	SDB_NODE_CONTEXT
			Ctx;


	if( !SubscriptObject )
	{
		memset( &SubscriptSec, 0, sizeof( SubscriptSec ));
		strcpy( SubscriptSec.SecData.Name, "~Subscript" );
		SubscriptSec.ReferenceCount = 1;
		SubscriptSec.SdbClass		=
		SubscriptSec.Class			= SecDbClassInfoFromType( SecDbClassTypeFromName( "SecDb Subscript" ));
		SubscriptSec.Db				= SecDbRootDb; // nasty hack

		DTM_INIT( &TmpVal );
		TmpVal.DataType		= DtSecurity;
		TmpVal.Data.Pointer	= &SubscriptSec;
		SubscriptObject = FindLiteral( &TmpVal, Db );
		if( !SubscriptObject )
			return( FALSE );
	}

	DTM_INIT( &TmpVal );
	TmpVal.DataType		= DtValueType;
	if( !( TmpVal.Data.Pointer = SecDbValueTypeFromName( RetDt->Name, RetDt )))
		return NULL;
	SubscriptValueType = FindLiteral( &TmpVal, Db );
	if( !SubscriptValueType )
		return( NULL );

	Argv[ 0 ] = this;
	Argv[ 1 ] = FindLiteral( Index, Db );
	if( !Argv[ 1 ] )
		return( NULL );

	SubscriptNode = Find( SubscriptObject, SubscriptValueType, 2, Argv, CREATE, Db, Share );
	if( !SubscriptNode )
		return( NULL );

	memset( &Ctx, 0, sizeof( Ctx ));
	SubscriptNode->GetValue( &Ctx, Db );

	return( SubscriptNode );
}
