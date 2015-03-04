#define GSCVSID "$Header: /home/cvs/src/secdb/src/dtds.c,v 1.47 2004/01/30 23:38:18 khodod Exp $"
/****************************************************************
**
**	dtds.c	- DiddleScope Datatype
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Log: dtds.c,v $
**	Revision 1.47  2004/01/30 23:38:18  khodod
**	Fixed the comparators for large pointer values.
**
**	Revision 1.46  2001/11/27 23:23:32  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.44  2001/09/10 22:21:07  singhki
**	better error handling for ds->array through SecDbDiddleScopeToArray
**	
**	Revision 1.43  2001/09/10 20:25:01  singhki
**	do not force phantom diddles to have values since the sideffect is all we need.
**	allows us to stream diddlescopes which are in an error state
**	
**	Revision 1.42  2001/08/15 15:45:38  simpsk
**	remove deprecated #includes.
**	
**	Revision 1.41  2001/06/29 15:22:12  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.40  2001/06/28 16:58:35  singhki
**	fixes for nested diddle scope cleanup
**	
**	Revision 1.39  2001/06/27 19:02:27  singhki
**	added SecDbCleanupDiddleScopes to clean up all unused diddle scopes
**	
**	Revision 1.38  2001/06/21 19:34:22  singhki
**	Completely get rid of remove diddle message. Rewrite side effect
**	diddle maps to actually work. Use it to remove all side effect
**	diddles. fix side effect/phantom interaction with DiddleStackMove.
**	
**	Also remove all diddles during destroy to prevent leaks.
**	
**	Revision 1.37  2001/05/31 01:00:01  singhki
**	remove extranous ;
**	
**	Revision 1.36  2001/05/30 21:07:43  simpsk
**	Let diddle scope lists be lists of diddle scope lists and/or diddle scopes.
**	
**	Revision 1.35  2001/05/25 22:49:19  simpsk
**	clean up DiddleScopeFree.
**	
**	Revision 1.34  2001/05/25 20:40:35  simpsk
**	small fixes/cleanups, remove arbitrary length restriction.
**	
**	Revision 1.33  2001/03/14 00:33:07  singhki
**	use AccessValue to get Node value in streaming diddle state as it will deal with pass through and vector context
**	
**	Revision 1.32  2001/03/06 21:12:04  singhki
**	add scope to deal with broken for scope lookup on NT
**	
**	Revision 1.31  2001/03/06 20:17:25  singhki
**	side effect diddles may be phantom in their own right
**	
**	Revision 1.30  2001/01/29 18:33:37  singhki
**	Do not cache diddle scopes if we cannot remove their diddles. Also
**	allow caching to be turned off via a flag.
**	
**	Revision 1.29  2001/01/26 01:16:49  singhki
**	Cache closed diddle scopes in order to reuse them later
**	
**	Revision 1.28  2001/01/19 19:15:53  shahia
**	Got rid of use of SDB_NODE_CONTEXTs Clear method, OK to PRODVER
**	
**	Revision 1.27  2001/01/10 01:26:59  simpsk
**	Minor updates for secdbcc.h
**	
**	Revision 1.26  2001/01/04 17:06:03  singhki
**	Added DiddleScopeTemporaryGet and extra member in SDB_DB to cache the temporary diddle scope for each Db
**	
**	Revision 1.25  2000/12/19 17:20:42  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.24  2000/11/15 17:52:07  goodfj
**	Prevent crash when asking for High/LowLimit()
**	
**	Revision 1.23  2000/08/29 14:13:03  goodfj
**	Replaced SDB_DB_SEARCH PATH with SDB_DB_UNION
**	
**	Revision 1.22  2000/04/26 20:00:21  vengrd
**	make TRUTH work on a DS
**	
**	Revision 1.21  2000/04/24 11:04:19  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.20  2000/04/12 19:32:37  singhki
**	Add SideEffectDiddleIterator to get side effect diddles of a phantom diddle
**	
**	Revision 1.19  2000/02/08 22:24:03  singhki
**	No more references to DbID for virtual databases. All virtual
**	databases have the same DbID. This removes the limit on the number of
**	virtual databases you can create in memory.
**	
**	Revision 1.18  2000/02/08 04:50:22  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.17  2000/01/20 20:18:33  singhki
**	Destroy ds literal node after removing diddles from the diddle scope
**	
**	Revision 1.16  2000/01/20 00:42:10  singhki
**	destroy diddle scope literal when freeing diddle scope.
**	
**	Revision 1.15  2000/01/14 20:33:37  singhki
**	set Handle to NULL when DiddleScope is freed
**	
**	Revision 1.14  1999/12/07 22:05:31  singhki
**	Add VTI to every node. Points to Class Value Func() by default.
**	New special class static VT, Class Value Func lives on Class object
**	in !Class Objects Db.
**	For the purpose of a DBSET_BEST computation, every Db behaves as if
**	it is of the for Db;Class Objects in order to find these Class VTIs
**	Use Class Value Func to calculate VFs for ~Cast and ~Subscript
**	
**	Revision 1.13  1999/10/19 00:29:31  singhki
**	use ! to prevent getbyname when nesting new diddle scope
**	
**	Revision 1.12  1999/09/01 15:29:25  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.11  1999/08/22 20:12:04  singhki
**	oops, pass SecDbRootDb not NULL
**	
**	Revision 1.10  1999/08/07 01:40:06  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.9  1999/07/16 21:22:12  singhki
**	NodeGetValue() now is passed the Db so that it can ensure that it builds
**	the childrne in the correct Db.
**	Fixed VTI interactions with diddle scopes, VTI's now not only cause splits
**	but also are set on extended nodes.
**	
**	Revision 1.8  1999/07/15 15:54:15  singhki
**	remove nasty linked list of open dbs, use DatabaseIDHash and OpenCount instead.
**	
**	Revision 1.7  1999/06/30 21:33:14  singhki
**	new child type for DiddleScopeUseSuffix(). Also some const fixes
**	
**	Revision 1.6  1999/06/16 16:20:51  singhki
**	Rationalize diddlescope from operator. Added DiddleScopeNestNew & DiddleScopeFromDb
**	
**	Revision 1.5  1999/04/20 14:29:56  singhki
**	Move the meat of ApplyDiddleState into DiddleScopeFromArray
**	
**	Revision 1.4  1999/04/20 02:13:58  singhki
**	Added unstream from array to diddle state.
**	
**	Revision 1.3  1999/04/14 16:48:30  singhki
**	Track sideffects of phantom diddles in order to be able to correctly stream out the diddle state
**	
**	Revision 1.2  1999/03/30 19:42:17  singhki
**	hash quick for diddle scope
**	
**	Revision 1.1  1998/12/23 18:09:13  singhki
**	moved DiddleScope datatypes into their own file. Support DiddleScope to Array in order to stream out diddle state
**	
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<secdbcc.h>
#include	<secdt.h>
#include	<secerror.h>
#include	<secexpr.h>
#include	<sdb_int.h>
#include	<dterr.h>
#include	<heapinfo.h>
#include	<secnode.h>
#include	<secnodei.h>

#include    <iomanip>
CC_USING( std::setw );

#include    <sstream>
CC_USING( std::ostringstream );

#include    <string>
CC_USING( std::string );

#include    <utility>
CC_USING( std::pair );

#include	<vector>
#include	<map>
#include	<set>
#include    <ccstl.h>

#include    <string.h> // for strdup
#include	<stdio.h>
#include	<stdlib.h>
#
/*
** local variables
*/

int const MaxListSize = INT_MAX / sizeof( DT_DIDDLE_SCOPE * );


/*
**	Define constants
*/

unsigned const MaxCachedDiddleScopes = 100;


CC_STL_MAP_WITH_COMP( SDB_DB*, CC_STL_VECTOR( SDB_DB* ), SDB_DB::db_less )
	sdb_reusable_diddle_scopes;



/****************************************************************
**	Routine: DiddleScopeIncRef
**	Returns: void
**	Action : increment ref count of diddle scope
****************************************************************/

static DT_DIDDLE_SCOPE *DiddleScopeIncRef(
	DT_DIDDLE_SCOPE	*DiddleScope
)
{
	if( DiddleScope )
		DiddleScope->RefCount++;
	return DiddleScope;
}



/****************************************************************
**	Routine: DiddleScopeFree
**	Returns: TRUE if freed / FALSE if non zero refcount
**	Action : decrements ref count, frees if down to 0
****************************************************************/

static bool DiddleScopeFree(
	DT_DIDDLE_SCOPE	*DiddleScope
)
{
	static bool
			CacheDiddleScopes = stricmp( SecDbConfigurationGet( "SDB_CACHE_UNUSED_DIDDLE_SCOPES", NULL, NULL, "true" ), "true" ) == 0;

	if( !DiddleScope || --DiddleScope->RefCount != 0 ) // if we have no diddle scope, or if this diddle scope has more references, do nothing. (act casual)
	    return false;

	if( !DiddleScope->Db )
	{
	    SlangScopeDiddlesDelete( DiddleScope->Diddles );
	}
	else
	{
	    bool cache_ds = CacheDiddleScopes;

		SDB_DB* OldDb = SecDbSetRootDatabase( DiddleScope->Db );
		if( !SlangScopeDiddlesDelete( DiddleScope->Diddles ) )
		{
		    string error = ErrGetString();
			ErrMsg( ERR_LEVEL_ERROR, "Software Bug: Failed to delete diddles when freeing diddle scope %s\n%s",
					DiddleScope->Db->FullDbName.c_str(), error.c_str() );
			// this diddle scope is screwed, do not attempt to cache it but try to throw it away
			cache_ds = false;
		}
		SecDbSetRootDatabase( OldDb );
		DiddleScope->Db->Handle = 0;

		// squirell away diddle scope if we can

		if( cache_ds && DiddleScope->Db->Right )
			sdb_reusable_diddle_scopes[ DiddleScope->Db->Right ].push_back( DiddleScope->Db );
		else
		    SecDbDetach( DiddleScope->Db );
	}

	DT_VALUE TmpValue;
	TmpValue.DataType = DtDiddleScope;
	TmpValue.Data.Pointer = DiddleScope;

	SDB_NODE* Literal = SDB_NODE::FindLiteral( &TmpValue, SecDbRootDb );
	if( Literal )
	    SDB_NODE::Destroy( Literal, NULL );
	else
		free( DiddleScope );

	return true;
}



/****************************************************************
**	Routine: NodeToStructure
**	Returns: TRUE/FALSE
**	Action : Converts a node to a structure for the diddle list
****************************************************************/

static bool NodeToStructure(
	SDB_NODE	*Node,
	DT_VALUE	*Value,
	SDB_DB		*Db
)
{
	DTM_ALLOC( Value, DtStructure );

	if( !Node->IsTerminal() )
		return Err( ERR_UNSUPPORTED_OPERATION, "Node isn't a terminal, cannot stream out diddle state" );

	SDB_NODE_CONTEXT
	        Ctx;

	for( int Arg1 = 0; Arg1 < Node->ArgcGet(); ++Arg1 )
		if( !Node->ArgGet( Arg1 )->ValidGet() && !Node->ArgGet( Arg1 )->GetValue( &Ctx, Db ))
			return false;

	if( !Node->ValidGet() )
		Node->GetValue( &Ctx, Db );

	DtComponentSetPointer( Value, "Security", DtString, ((SDB_OBJECT *) Node->ObjectGet()->AccessValue()->Data.Pointer )->SecData.Name );
	DtComponentSetPointer( Value, "ValueType", DtString, ((SDB_VALUE_TYPE) Node->ValueTypeGet()->AccessValue()->Data.Pointer )->Name );

	DT_VALUE
	        Args;

	DTM_ALLOC( &Args, DtArray );
	for( int Arg2 = 0; Arg2 < Node->ArgcGet(); ++Arg2 )
		DtAppend( &Args, Node->ArgGet( Arg2 )->AccessValue() );

	DtComponentSetPointer( Value, "Args", DtArray, Args.Data.Pointer );

	return true;
}



/****************************************************************
**	Routine: SecDbDiddleScopeToArray
**	Returns: DT_ARRAY * / Err
**	Action : Returns a list of the diddles in the diddle scope
****************************************************************/

DT_ARRAY *SecDbDiddleScopeToArray(
	DT_DIDDLE_SCOPE	*DiddleScope
)
{
	dt_value_var Array( DtArray );

	HASH_ENTRY_PTR
			HashPtr;

    SDB_NODE::NodeSet 
        visited_nodes;

	CC_STL_MAP( SDB_DIDDLE_ID, SDB_NODE* )
			phantom_diddles;


	if( DiddleScope && DiddleScope->Diddles && DiddleScope->Diddles->DiddleHash )
	{
		HASH_FOR( HashPtr, DiddleScope->Diddles->DiddleHash )
		{
			DT_VALUE
					Elem;

			SLANG_SCOPE_DIDDLE
					*Diddle = (SLANG_SCOPE_DIDDLE *) HashPtr->Key;

			SDB_NODE
					*Node = SDB_NODE::FindTerminalAndArgs( Diddle->SecPtr, Diddle->ValueType, Diddle->Argc, Diddle->Argv, SDB_NODE::DO_NOT_CHECK_SUPPORTED, DiddleScope->Db, SDB_NODE::SHARE );


			if( !Node || !NodeToStructure( Node, &Elem, DiddleScope->Db ))
				return 0;

			SDB_DIDDLE
					*TheDiddle = Node->DiddleFind( Diddle->DiddleID );

			bool is_phantom = false;
			if( TheDiddle && TheDiddle->Phantom )
			{
				DtComponentSetNumber( &Elem, "Phantom", DtDouble, TheDiddle->DiddleID );
				phantom_diddles[ TheDiddle->DiddleID ] = Node;
				is_phantom = true;
			}
			// FIX- Shouldn't really need to set the value for phantom diddles
			// but unfortunately we need the value's datatype to be right, so this is a temporary kludge
			// maybe we can just do a alloc of the correct datatype??
			DT_VALUE* Value = Node->ValidGet() ? Node->AccessValue() : 0;
			if( !Value )
			{
				SDB_VALUE_TYPE_INFO* vti;
				if( is_phantom && ( vti = Node->VtiGet() ) && vti->ValueType )
				{
					// if we cant get phantom diddle value then just use null value since it isnt all that necessary
					// as we will have the value of the sideeffect
					dt_value_var null_value( vti->ValueType->DataType );
					DtComponentSet( &Elem, "Value", null_value.get() );
				}
				else
					Err( ERR_NOT_CONSISTENT, "Cannot get value of diddled node %s in order to stream diddle state", Node->GetText( 0 ).c_str() );
				return 0;
			}
			else
				DtComponentSet( &Elem, "Value", Value );

			DtAppendNoCopy( Array.get(), &Elem );
		}
	}

	{
		for( CC_STL_MAP( SDB_DIDDLE_ID, SDB_NODE* )::iterator it = phantom_diddles.begin(); it != phantom_diddles.end(); ++it )
		{
			SDB_NODE::NodeSet const* side_effects = SDB_NODE::SideEffects( it->first );
			if( !side_effects )
				continue;

			for( SDB_NODE::NodeSet::const_iterator it1 = side_effects->begin(); it1 != side_effects->end(); ++it1 )
			{
				if( visited_nodes.find( *it1 ) == visited_nodes.end() )
				{
					SDB_NODE
							*Node = (*it1)->FindAgain( SDB_NODE::DO_NOT_CREATE, DiddleScope->Db, SDB_NODE::SHARE );

					SDB_DIDDLE_HEADER
							*DiddleHeader;

					DT_VALUE
							Elem,
							Diddles;

					if( !SDB_NODE::IsNode( Node ))
					{
						Err( ERR_NOT_CONSISTENT, "Cannot find Phantom diddle sideffect node in diddle scope" );
						return 0;
					}

					visited_nodes.insert( *it1 );
					if( !( DiddleHeader = Node->DiddleHeaderGet()))
					{
						Err( ERR_NOT_CONSISTENT, "@ Phantom diddle sideffect %s of node %s is not diddled",
							 Node->to_string().c_str(), it->second->to_string().c_str()
						   );
						return 0;
					}

					if( !NodeToStructure( Node, &Elem, DiddleScope->Db ))
						return 0;

					DtComponentSetNumber( &Elem, "SideEffect", DtDouble, 1 );
					DTM_ALLOC( &Diddles, DtArray );
					for( SDB_DIDDLE *Diddle = DiddleHeader->DiddleList; Diddle; Diddle = Diddle->Next )
						if( phantom_diddles.find( Diddle->DiddleID ) != phantom_diddles.end() )
						{
							DT_VALUE
									DiddleElem;

							DTM_ALLOC( &DiddleElem, DtStructure );
							DtComponentSetNumber( &DiddleElem, "DiddleID", DtDouble, Diddle->DiddleID );
							DtComponentSet( &DiddleElem, "Value", &Diddle->Value );
							if( Diddle->Phantom )
								DtComponentSetNumber( &DiddleElem, "Phantom", DtDouble, Diddle->DiddleID );
							DtAppendNoCopy( &Diddles, &DiddleElem );
						}
					DtComponentSetPointer( &Elem, "Diddles", DtArray, Diddles.Data.Pointer );
					DtAppendNoCopy( Array.get(), &Elem );
				}
			}
		}
	}
	return (DT_ARRAY *) Array.release().Data.Pointer;
}



/****************************************************************
**	Routine: DiddleScopeFromArray
**	Returns: TRUE/Err
**	Action : Applies a diddle state
****************************************************************/

int DiddleScopeFromArray(
	DT_ARRAY	*DiddleState,			// Diddle State as returned by DiddleScopeToArray
	SLANG_SCOPE	*Scope					// Scope in which to apply diddle state
)
{
	typedef pair<SDB_DIDDLE_ID,SDB_NODE*> DiddleInfo;

	CC_STL_MAP( long, DiddleInfo ) phantom_diddles;

	int		RetCode = TRUE;

	for( int Num = 0; RetCode && Num < DiddleState->Size; ++Num )
	{
		DT_VALUE* Elem = &DiddleState->Element[ Num ];

		char const* SecName       = static_cast<char const*>( DtComponentGetPointer( Elem, "Security",  DtString, NULL ));
		char const* ValueTypeName = static_cast<char const*>( DtComponentGetPointer( Elem, "ValueType", DtString, NULL ));

		DT_ARRAY
				*ArgsArray = (DT_ARRAY *) DtComponentGetPointer( Elem, "Args", DtArray, NULL );

		// We have partially applied the diddle state, but I don't believe that it's worth the trouble to unwind it
		if( !SecName || !ValueTypeName || !ArgsArray )
		{
			RetCode = Err( ERR_INVALID_ARGUMENTS, "Malformed diddle state, Element %d", Num );
			break;
		}

		SDB_OBJECT* SecPtr = SecDbGetByName( SecName, SecDbRootDb, 0 );
		if( !SecPtr )
		{
			RetCode = ErrMore( "SecDbApplyDiddleState(): GetByName '%s' failed", SecName );
			break;
		}
		SDB_VALUE_TYPE ValueType = SecDbValueTypeFromName( ValueTypeName, NULL );

		// Fill in args
		DT_VALUE** Argv = (DT_VALUE **) malloc( ArgsArray->Size * sizeof( DT_VALUE * ));
		for( int ArgNum = 0; ArgNum < ArgsArray->Size; ++ArgNum )
			Argv[ ArgNum ] = ArgsArray->Element + ArgNum;

		DT_VALUE* Diddles;
		DT_VALUE* Value;
		if( DtComponentGet( Elem, "SideEffect", NULL ) && (Diddles = DtComponentGet( Elem, "Diddles", NULL )))	// SideEffect diddle
		{
			for( int DiddleNum = DtSize( Diddles ) - 1; DiddleNum >= 0; --DiddleNum )
			{
				DT_VALUE
						*Diddle = DtSubscriptGet( Diddles, DiddleNum, NULL );

				long ID = (long) DtComponentGetNumber( Diddle, "DiddleID", DtDouble, 0 );
				bool IsPhantom = (bool) DtComponentGetNumber( Diddle, "Phantom", DtDouble, 0 );

				SDB_SET_FLAGS Flags = SDB_SET_NO_MESSAGE;
				if( IsPhantom )
					Flags = Flags | SDB_DIDDLE_IS_PHANTOM;

				DiddleInfo& diddle_info = phantom_diddles[ ID ];

				// this sneekily inserts DiddleID in the PhantomDiddleHash in order to fool SetDiddle into thinking
				// that this is actually a sideffect of a phantom diddle. Should be a better way of doing this
				// Note that we must use SecDbSetDiddle here instead SlangScopeSetDiddle because the side-effect diddles
				// are not part of the slang scope.

				SDB_NODE::MarkPhantomDiddle( diddle_info.first, diddle_info.second );
				if( SecDbSetDiddleWithArgs( SecPtr, ValueType, ArgsArray->Size, Argv, DtComponentGet( Diddle, "Value", NULL ), Flags, diddle_info.first ) == SDB_DIDDLE_ID_ERROR )
					RetCode = ErrMore( "SetDiddle of a sideffect failed" );
				SDB_NODE::UnMarkPhantomDiddle( diddle_info.first );
			}
			SecDbFree( SecPtr );
		}
		else if( ( Value = DtComponentGet( Elem, "Value", NULL )) && DtComponentGetNumber( Elem, "Phantom", DtDouble, 0 ) == 0 ) // Plain old diddle
		{
			if( !SlangScopeSetDiddle( Scope, SecPtr, ValueType, ArgsArray->Size, Argv, Value ))
			{
				RetCode = ErrMore( "ScopeSetDiddle failed" );
				SecDbFree( SecPtr );
			}
		}
		else if( Value )				// Phantom diddle
		{
			SDB_DIDDLE_ID
					DiddleID = SlangScopeSetDiddleWithFlags( Scope, SecPtr, ValueType, ArgsArray->Size, Argv, Value, SDB_DIDDLE_ID_NEW_PHANTOM, SDB_SET_NO_MESSAGE );

			if( DiddleID != SDB_DIDDLE_ID_ERROR )
			{
				SDB_NODE* node = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, ArgsArray->Size, Argv, SDB_NODE::DO_NOT_CHECK_SUPPORTED, SecDbRootDb, SDB_NODE::SHARE );
				phantom_diddles[ (long) DtComponentGetNumber( Elem, "Phantom", DtDouble, 0 ) ] = DiddleInfo( DiddleID, node );
			}
			else
			{
				RetCode = ErrMore( "ScopeSetDiddle failed" );
				SecDbFree( SecPtr );
			}
		}
		else
		{
			RetCode = Err( ERR_INVALID_ARGUMENTS, "Malformed diddle state, Element %d", Num );
			SecDbFree( SecPtr );
		}
		free( Argv );
	}
	return RetCode;
}



/****************************************************************
**	Routine: DiddleScopeTemporaryGet
**	Returns: TRUE/Err
**	Action : constructs/returns a new nested temporary diddle scope
**			 returns NULL for error
**			 Note that this makes a reference to the Db using
**			 DbAttach, you must do a SecDbDetach when you are done
****************************************************************/

SDB_DB *DiddleScopeTemporaryGet(
)
{
	// FIX this is another artifact due to Dbs never really being freed
	if( !SecDbRootDb->TemporaryDS || SecDbRootDb->TemporaryDS->OpenCount == 0 )
	{
		DT_VALUE
				DS;
		if( !DiddleScopeNestNew( SecDbRootDb, &DS ) )
			return NULL;

		SecDbRootDb->TemporaryDS = ((DT_DIDDLE_SCOPE *) DS.Data.Pointer)->Db;
		// Initial creation makes an extra reference to the TemporaryDS by the Db
	}
	DbAttach( SecDbRootDb->TemporaryDS, 0, 0 );
	return SecDbRootDb->TemporaryDS;
}



/****************************************************************
**	Routine: DiddleScopeNestNew
**	Returns: TRUE/ERR
**	Action : constructs a new nested diddle scope
****************************************************************/

int DiddleScopeNestNew(
	SDB_DB		*BaseDb,
	DT_VALUE	*DiddleScopeValue
)
{
	static int
			NextDiddleScopeId = 1;

	SDB_DB* Db;

	CC_STL_VECTOR( SDB_DB * )& cached = sdb_reusable_diddle_scopes[ BaseDb ];

	if( !cached.empty() )
	{
		Db = cached.back();
		cached.pop_back();
	}
	else
	{
	    ostringstream ostr;
		ostr << "!" << NextDiddleScopeId++;
		SecDbDiddleScopeNew( ostr.str().c_str());

		ostr << ";" << BaseDb->FullDbName;
		Db = SecDbAttach( ostr.str().c_str(), SDB_DEFAULT_DEADPOOL_SIZE, 0 );
	}

	if( !Db )
		return ErrMore( "DiddleScope(): Can't create another diddle scope" );

	DTM_ALLOC( DiddleScopeValue, DtDiddleScope );

	DT_DIDDLE_SCOPE* DiddleScope	= (DT_DIDDLE_SCOPE*) DiddleScopeValue->Data.Pointer;
	Db->Handle 						= DiddleScope;
	DiddleScope->Db					= Db;

	return TRUE;
}



/****************************************************************
**	Routine: DiddleScopeFromDb
**	Returns: TRUE/ERR
**	Action : returns the DiddleScope associated with a Db
**			 you *MUST* DTM_FREE the diddle scope when you are
**			 done, else it will leak
****************************************************************/

int DiddleScopeFromDb(
	DT_VALUE	*DiddleScopeValue,
	SDB_DB		*Db
)
{
	DT_DIDDLE_SCOPE* DiddleScope = Db->DiddleScope();
	if( !DiddleScope )
		return Err( ERR_INVALID_ARGUMENTS, "Database isn't a diddle scope" );

	DiddleScopeIncRef( DiddleScope );
	DiddleScopeValue->DataType = DtDiddleScope;
	DiddleScopeValue->Data.Pointer = DiddleScope;
	return TRUE;
}



/****************************************************************
**	Routine: DiddleScopeSuffix
**	Returns: TRUE/Err
**	Action : Constructs the suffix of the Db
****************************************************************/

int DiddleScopeSuffix(
	SDB_DB		*Db,
	DT_VALUE	*Value
)
{
	if( Db->DbOperator != SDB_DB_UNION )
		return Err( ERR_INVALID_ARGUMENTS, "%s is not a union, cannot construct suffix", Db->FullDbName.c_str() );

	return DiddleScopeFromDb( Value, Db->Right );
}



/****************************************************************
**	Routine: DiddleScopeSuffix
**	Returns: TRUE/ERR
**	Action : Constructs the suffix of the DiddleScope
**			 you must free the Suffix when done
****************************************************************/

int DiddleScopeSuffix(
	DT_VALUE	*Src,
	DT_VALUE	*Suffix
)
{
	if( Src->DataType == DtDiddleScope )
		return DiddleScopeSuffix( ( (DT_DIDDLE_SCOPE *) Src->Data.Pointer )->Db, Suffix );

	if( Src->DataType == DtDiddleScopeList )
	{
		DT_VALUE
				Subscript;

		dt_value_var ret( DtDiddleScopeList );

		DT_DIDDLE_SCOPE_LIST
				*DSList = (DT_DIDDLE_SCOPE_LIST *) Src->Data.Pointer;

		Subscript.DataType = DtDouble;
		for( int i = DSList->Size; --i >= 0; )
		{
			DT_VALUE
					ElemSuffix;

			if( !DiddleScopeSuffix( DSList->DiddleScopeLists + i, &ElemSuffix ))
			    return ErrMore( "DiddleScopeSuffix failed at element %d", i );

			Subscript.Data.Number = i;
			if( !DtSubscriptReplace( ret.get(), &Subscript, &ElemSuffix ))
			{
				ErrMore( "DiddleScopeSuffix, cannot replace subscript %d", i );
				return FALSE;
			}
		}
		*Suffix = ret.release();
		return TRUE;
	}
	return Err( ERR_INVALID_ARGUMENTS, "DiddleScopeSuffix(): must be a diddle scope or a diddle scope list" );
}



/****************************************************************
**	Routine: DtFuncDiddleScope
**	Returns: TRUE	- Operation worked
**			 FALSE	- Operation failed
**	Action : Handle operations for Database data type
****************************************************************/

int DtFuncDiddleScope(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
    switch( Msg ) // each case should return
	{
		case DT_MSG_START:
			a->DataType->Size	= sizeof( DT_DIDDLE_SCOPE );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_REFERENCE | DT_FLAG_SDB_OBJECT;
			return TRUE;

		case DT_MSG_ALLOCATE:
		{
			DT_DIDDLE_SCOPE* DiddleScope = (DT_DIDDLE_SCOPE *) malloc( sizeof( *DiddleScope ));
			DiddleScope->RefCount	= 1;
			DiddleScope->Db			= NULL;
			DiddleScope->Diddles	= SlangScopeDiddlesNew();

			r->Data.Pointer			= DiddleScope;
			return TRUE;
		}
		case DT_MSG_FREE:
		{
			DT_DIDDLE_SCOPE* DiddleScope = (DT_DIDDLE_SCOPE *) a->Data.Pointer;
			if( DiddleScopeFree( DiddleScope ))
				a->Data.Pointer = NULL;
			return TRUE;
		}
		case DT_MSG_ASSIGN:
		{
			DT_DIDDLE_SCOPE* DiddleScope = (DT_DIDDLE_SCOPE *) a->Data.Pointer;
			DiddleScopeIncRef( DiddleScope );
			*r = *a;
			return TRUE;
		}
		case DT_MSG_TO:
		{
			DT_DIDDLE_SCOPE* DiddleScope = (DT_DIDDLE_SCOPE *) a->Data.Pointer;
			if( r->DataType == DtString )
			{
				r->Data.Pointer = strdup( DiddleScope && DiddleScope->Db ? DiddleScope->Db->FullDbName.c_str() : "NULL" );
				return TRUE;
			}
			else if( r->DataType == DtArray )
			{
				if( !( r->Data.Pointer = SecDbDiddleScopeToArray( DiddleScope )))
					return ErrMore( "DiddleScope to Array failed" );
				return TRUE;
			}
			else if( r->DataType == a->DataType )
			    return DTM_ASSIGN( r, a );
			else if( r->DataType == DtStructure )
			{
				dt_value_var res( DtStructure );

				DT_VALUE db_val;
				db_val.DataType = DtDatabase;
				if( !DTM_TO( &db_val, a ) )
					return FALSE;

				DtComponentReplacePointer( res.get(), "Database", db_val.DataType, db_val.Data.Pointer );

				DT_VALUE diddles_val;
				diddles_val.DataType = DtArray;
				if( !DTM_TO( &diddles_val, a ) )
					return FALSE;

				DtComponentReplacePointer( res.get(), "Diddles", diddles_val.DataType, diddles_val.Data.Pointer );

				DtComponentSetNumber( res.get(), "References",           DtDouble, DiddleScope->RefCount );
				*r = res.release();
				return TRUE;
			}

			return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", a->DataType->Name, r->DataType->Name );
		}
		case DT_MSG_FROM:
			if( b->DataType == DtDatabase )
				return DiddleScopeFromDb( a, ((DT_DATABASE *) b->Data.Pointer)->Db );

			return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", b->DataType->Name, a->DataType->Name );

		case DT_MSG_EQUAL:
			if( a->DataType != b->DataType )
				r->Data.Number = FALSE;
			else
				r->Data.Number = (DT_DIDDLE_SCOPE *) a->Data.Pointer == (DT_DIDDLE_SCOPE *) b->Data.Pointer;
			r->DataType = DtDouble;
			return TRUE;

		case DT_MSG_NOT_EQUAL:
			if( a->DataType != b->DataType )
				r->Data.Number = TRUE;
			else
				r->Data.Number = (DT_DIDDLE_SCOPE *) a->Data.Pointer != (DT_DIDDLE_SCOPE *) b->Data.Pointer;
			r->DataType = DtDouble;
			return TRUE;

		case DT_MSG_TRUTH:
			return( a->Data.Pointer && ((DT_DIDDLE_SCOPE *) a->Data.Pointer )->Db ? TRUE : FALSE );

		case DT_MSG_SIZE:
			r->DataType 	= DtDouble;
			r->Data.Number	= 1.0;
			return TRUE;

		case DT_MSG_MEMSIZE:
			r->DataType 	= DtDouble;
			r->Data.Number	= 0.0;
			return TRUE;

		case DT_MSG_INFO:
		{
		    DT_INFO* DataTypeInfo = (DT_INFO *) calloc( 1, sizeof( DT_INFO ));
			DataTypeInfo->Flags = DT_FLAG_POINTER;
			r->Data.Pointer = DataTypeInfo;
			return TRUE;
		}
		case DT_MSG_HASH_QUICK:
		{
			unsigned long
					Hash1 = (unsigned long) a->Data.Pointer,
					Hash2 = (unsigned long) DtDiddleScope,
					Hash3 = *(DT_HASH_CODE *) r->Data.Pointer;

			DT_HASH_MIX( Hash1, Hash2, Hash3 );
			*(DT_HASH_CODE *) r->Data.Pointer = Hash3;
			return TRUE;
		}
	}


/*
**	Unsupported operation
*/

	if( b ) return Err( ERR_UNSUPPORTED_OPERATION, "@ - %s between %s and %s", DtMsgToString( Msg ), a->DataType->Name, b->DataType->Name );
	if( a ) return Err( ERR_UNSUPPORTED_OPERATION, "@ - %s on %s",             DtMsgToString( Msg ), a->DataType->Name );

	return         Err( ERR_UNSUPPORTED_OPERATION, "@ - %s",                   DtMsgToString( Msg ) );
}



/****************************************************************
**	Routine: DtFuncDiddleScopeList
**	Returns: TRUE	- Operation worked
**			 FALSE	- Operation failed
**	Action : Handle operations for diddle scope list data type
****************************************************************/

int DtFuncDiddleScopeList(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
    switch( Msg ) // each case should return
	{
		case DT_MSG_START:
			a->DataType->Size	= sizeof( a->Data.Pointer );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_SUBSCRIPTS | DT_FLAG_SDB_OBJECT;
			return TRUE;

		case DT_MSG_ALLOCATE:
			return ERR_CALLOC( r->Data.Pointer, DT_DIDDLE_SCOPE_LIST, 1, sizeof( DT_DIDDLE_SCOPE_LIST ));

		case DT_MSG_FREE:
		{
			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;
			for( int ii = 0; ii < Array->Size; ++ii )
			    DTM_FREE( Array->DiddleScopeLists + ii );
			free( Array->DiddleScopeLists );
			free( Array );
			return TRUE;
		}
		case DT_MSG_ASSIGN:
		{
		    DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) malloc( sizeof( DT_DIDDLE_SCOPE_LIST ));
			r->Data.Pointer = Array;
			Array->Size = ((DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer)->Size;
			if( Array->Size )
			{
				Array->DiddleScopeLists = (DT_VALUE *) malloc( Array->Size * sizeof( DT_VALUE ));
				DT_VALUE* rhs = ((DT_DIDDLE_SCOPE_LIST*) a->Data.Pointer)->DiddleScopeLists;
				for( int ii = 0; ii < Array->Size; ++ii )
					DTM_ASSIGN( Array->DiddleScopeLists + ii, rhs + ii );
			}
			else
				Array->DiddleScopeLists = NULL;

			r->DataType = a->DataType;
			return TRUE;
		}
		case DT_MSG_ASSIGN_AND:
		{
			if( b->DataType != DtDiddleScope && b->DataType != DtDiddleScopeList )
				return Err( ERR_INVALID_ARGUMENTS, "@ - an element in a %s must be a %s, not a %s", a->DataType->Name, DtDiddleScopeList->Name, b->DataType->Name );
			
			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;

			if( Array->Size + 1 < MaxListSize )
			{
				Array->Size++;
				Array->DiddleScopeLists = (DT_VALUE*) realloc( Array->DiddleScopeLists, Array->Size * sizeof( DT_VALUE ));
				DTM_ASSIGN( Array->DiddleScopeLists + Array->Size - 1, b );
				return TRUE;
			}

			return Err( ERR_OVERFLOW, "Array - @ maximum size allowed is %d", MaxListSize );
		}
		case DT_MSG_SUBSCRIPT_REPLACE:	// a[ b ] = r and free old a[ b ]
		{
			if( r->DataType != DtDiddleScope && r->DataType != DtDiddleScopeList )
				return Err( ERR_INVALID_ARGUMENTS, "@ - an element in a %s must be a %s, not a %s", a->DataType->Name, DtDiddleScopeList->Name, r->DataType->Name );

			// Get element number from b
			if( b->DataType != DtDouble )
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript must be a Double, not a %s", b->DataType->Name );

			int ii = (int) b->Data.Number;
			if( ii < 0 || ii >= MaxListSize )
				return Err( DT_ERR_SUBSCRIPT_OUT_OF_RANGE, "@" );

			// Resize array if necessary
			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;

			if( ii >= Array->Size )
			{
				int jj = Array->Size;
				Array->Size = ii + 1;
				Array->DiddleScopeLists = (DT_VALUE*) realloc( Array->DiddleScopeLists, Array->Size * sizeof( DT_VALUE ));

				for( ; jj < Array->Size; ++jj )
					DTM_INIT( Array->DiddleScopeLists + jj );
			}

			DTM_FREE( Array->DiddleScopeLists + ii );
			DTM_ASSIGN( Array->DiddleScopeLists + ii, r );
			return TRUE;
		}
		case DT_MSG_SUBSCRIPT_VALUE:	// r = a[ b ]
		{
			if( b->DataType != DtDouble )
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript must be a Double" );

			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;
			int ii = (int) b->Data.Number;
			if( ii < 0 || ii >= Array->Size )
				return Err( DT_ERR_SUBSCRIPT_OUT_OF_RANGE, "@" );

			DTM_ASSIGN( r, Array->DiddleScopeLists + ii );
			return TRUE;
		}
		case DT_MSG_SUBSCRIPT_DESTROY:
		{
			if( b->DataType != DtDouble )
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript must be a Double" );

			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;
			int ii = (int) b->Data.Number;
			if( ii < 0 || ii >= Array->Size )
				return Err( DT_ERR_SUBSCRIPT_OUT_OF_RANGE, "@" );

			DTM_FREE( Array->DiddleScopeLists + ii );
			DTM_INIT( Array->DiddleScopeLists + ii );
			return TRUE;
		}
		case DT_MSG_SUBSCRIPT_LAST:
		{
			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;
			if( Array->Size > 0 )
			{
				r->DataType = DtDouble;
				r->Data.Number = Array->Size - 1;
			}
			else
			{
				DTM_INIT( r );
				return FALSE;
			}
			return TRUE;
		}
		case DT_MSG_SUBSCRIPT_NEXT:
		{
			if( r->DataType == DtDouble )
				r->Data.Number += 1;
			else
			{
				r->DataType = DtDouble;
				r->Data.Number = 0;
			}
			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;
			if( (int) r->Data.Number >= Array->Size )
			{
				DTM_INIT( r );
				return FALSE;
			}
			return TRUE;
		}
		case DT_MSG_EQUAL:
		case DT_MSG_NOT_EQUAL:
			r->DataType = DtDouble;
			if( a->DataType != b->DataType )
				r->Data.Number = 0.0;
			else
			{
			    DT_DIDDLE_SCOPE_LIST* Array  = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;
				DT_DIDDLE_SCOPE_LIST* ArrayB = (DT_DIDDLE_SCOPE_LIST *) b->Data.Pointer;

				if( Array == ArrayB )
					r->Data.Number = 1.0;
				else if( !Array || !ArrayB || Array->Size != ArrayB->Size )
					r->Data.Number = 0.0;
				else
				{
					r->Data.Number = 1.0;
					for( int ii = 0; ii < Array->Size; ++ii )
					{
					    DT_OP( DT_MSG_EQUAL, r, Array->DiddleScopeLists + ii, ArrayB->DiddleScopeLists + ii );
						if( !r->Data.Number )
							break;
					}
				}
			}
			if( DT_MSG_NOT_EQUAL == Msg )
				r->Data.Number = (r->Data.Number ? 0.0 : 1.0);
			return TRUE;

		case DT_MSG_TO:
		{
			if( r->DataType == a->DataType )
				return DTM_ASSIGN( r, a );

			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;

			if( r->DataType == DtString )
			{
			    ostringstream ostr;
				for( int ii = 0; ii < Array->Size; ++ii )
				{
				    ostr << "[" << setw(4) << ii << "] = ";
					if( DTM_TO( r, Array->DiddleScopeLists + ii ) )
					{
					    ostr << (char const*) r->Data.Pointer << "\n";
						free( (void*) r->Data.Pointer );
					}
					else
					{
					    return FALSE;
					}
				}
				r->Data.Pointer = strdup( ostr.str().c_str() );
				return TRUE;
			}

			if( r->DataType == DtArray || r->DataType == DtEach )
			{
				DT_DATA_TYPE
						DtTmp = r->DataType;

				DTM_ALLOC( r, DtTmp );
				for( int ii = 0; ii < Array->Size; ++ii )
				    DtSubscriptSet( r, ii, Array->DiddleScopeLists + ii );

				return TRUE;
			}

			return Err( ERR_UNSUPPORTED_OPERATION, "Diddle Scope List, @" );
		}
		case DT_MSG_FROM:
		{
			if(( b->DataType->Flags & ( DT_FLAG_SUBSCRIPTS | DT_FLAG_COMPONENTS )) == 0 )
			    return Err( ERR_UNSUPPORTED_OPERATION, "Diddle Scope List, @" );

			DT_VALUE
			        Subscript,
			        Value;

			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) (a->Data.Pointer = calloc( 1, sizeof( DT_DIDDLE_SCOPE_LIST )));
			Array->Size = DtSize( b );
			Array->DiddleScopeLists = (DT_VALUE*) calloc( Array->Size, sizeof( DT_VALUE ));
			int ii = 0;
			DTM_FOR_VALUE( &Subscript, &Value, b )
			{
				DTM_ASSIGN( Array->DiddleScopeLists + ii, &Value );
				++ii;
			}
			return TRUE;
		}
		case DT_MSG_TRUTH:
			return TRUE;

		case DT_MSG_SIZE:
		{
			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;
			r->DataType 	= DtDouble;
			r->Data.Number	= Array->Size;
			return TRUE;
		}
		case DT_MSG_MEMSIZE:
		{
			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST *) a->Data.Pointer;
			r->DataType 	= DtDouble;
			r->Data.Number	= GSHeapMemSize( Array ) + GSHeapMemSize( Array->DiddleScopeLists );
			return TRUE;
		}
		case DT_MSG_INFO:
		{
		    DT_INFO* DataTypeInfo = (DT_INFO *) calloc( 1, sizeof( DT_INFO ));
			DataTypeInfo->Flags = DT_FLAG_POINTER | DT_FLAG_SUBSCRIPTS;
			r->Data.Pointer = DataTypeInfo;
			return TRUE;
		}
		case DT_MSG_HASH_QUICK:
		{
		    // code stolen from datatype/dtarray.c
			DT_HASH_CODE
					CurHash = *(DT_HASH_CODE *) r->Data.Pointer;

			// Always hash in datatype so that a single element array does not hash
			// to the same place as the value of the single element.
			{
				unsigned long
						Hash1 = (unsigned long) a->DataType,
						Hash2 = 0;

				DT_HASH_MIX( Hash1, Hash2, CurHash );
			}

			DT_DIDDLE_SCOPE_LIST* Array = (DT_DIDDLE_SCOPE_LIST*) a->Data.Pointer;

			if( Array )
			{
				DT_VALUE
						CurHashVal;

				CurHashVal.Data.Pointer = &CurHash;

				size_t ElemIncr = ( Array->Size + 7 ) >> 3;

				for( int ElementNumber = 0; ElementNumber < Array->Size; ElementNumber += ElemIncr )
				{
					if( !DT_OP( DT_MSG_HASH_QUICK, &CurHashVal, Array->DiddleScopeLists + ElementNumber, NULL )) // these are either DiddleScopes or DiddleScopeLists
						return ErrMore( "Could not hash diddle scope list element #%d", ElementNumber );
				}
			}
			*(DT_HASH_CODE *) r->Data.Pointer = CurHash;

			return TRUE;
		}
	} // switch

	return Err( ERR_UNSUPPORTED_OPERATION, "Diddle Scope List, @" );
}
