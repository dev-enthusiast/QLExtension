#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_eval.c,v 1.191 2013/08/14 19:45:50 khodod Exp $"
/****************************************************************
**
**	SDB_EVAL.C	- expression evaluator
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_eval.c,v $
**	Revision 1.191  2013/08/14 19:45:50  khodod
**	Do not crash if we exceed the Slang stack frame limit.
**	AWR: #310820
**
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<mempool.h>
#include	<hash.h>
#include	<heap.h>
#include	<outform.h>
#include	<secdb.h>
#include	<secexpr.h>
#include	<sdb_x.h>
#include	<dtptr.h>
#include	<secnode.h>
#include 	<gsdt/GsDtToDtValue.h>
#include 	<gsdt/x_gsdt.h>

CC_USING_NAMESPACE( Gs );

/*
**	Variables
*/

static long
		SlangContextUpdateCount = 0;

SLANG_CONTEXT
		*SlangContext			= NULL;

SLANG_SCOPE_DIDDLES
		*SlangStickyDiddles		= NULL;

unsigned SlangFunctionDepth		= 0;

HASH	*SlangVariableHash		= NULL,		// Hash table for global variables
		*SlangRootHash			= NULL,		// Roots and their memory
		*SlangFunctionHash		= NULL,		// Functions
		*SlangConstantsHash		= NULL,		// Constants
		*SlangEnumsHash			= NULL,		// Enums
		*SlangVarScopeHash		= NULL,		// Variable scopes
		*SlangNodeTypeHash		= NULL,		// Node types
		*SlangNodesByName		= NULL,		// NodeTypes by constant name
		*SlangNodesByType		= NULL;		// NodeTypes by constant value

HEAP	*SlangVariableHeap		= NULL;		// Heap for variable memory

HEAP	*SlangScopeFunctionHeap	= NULL;		// Heap for scopes

static SDB_DB
		*SourceDb = 0;

#include    <list>
CC_USING( std::list );
typedef CC_STL_LIST( SLANG_SCOPE_DIDDLE * ) slangScopeDiddleList;

static HASH    *g_ValidDiddleScopes = NULL;    // HASH of slangScopeDiddleList's.
static int      g_DiddleScopesFixEnabled = 1;  // Enable the fix by default.     

// secdb slang scopes
CC_STL_STACK( SLANG_SCOPE* ) SecDb_Slang_Scope::m_scopes;



/****************************************************************
**	Routine: SlangScopeFunctionAlloc
**	Returns: Newly allocated scope, NULL for error
**	Action : Allocates scope on scope heap
****************************************************************/

static inline SLANG_SCOPE_FUNCTION *SlangScopeFunctionAlloc(
)
{
	return (SLANG_SCOPE_FUNCTION *) GSHeapMalloc( SlangScopeFunctionHeap );
}


/****************************************************************
**	Routine: SlangScopeFunctionFree
**	Returns: void
**	Action : Frees scope from scope heap
****************************************************************/

static inline void SlangScopeFunctionFree(
	SLANG_SCOPE_FUNCTION *Scope
)
{
	GSHeapFree( SlangScopeFunctionHeap, Scope );
}


/****************************************************************
**	Routine: SlangVariableAlloc
**	Returns: Newly allocated variable, NULL for error
**	Action : Allocaates variable and optionally the associated value
****************************************************************/

SLANG_VARIABLE *SlangVariableAlloc(
	int		AllocValue			// If FALSE, do not allocate value
)
{
	SLANG_VARIABLE
			*Variable;


	if( !( Variable = (SLANG_VARIABLE *) GSHeapCalloc( SlangVariableHeap )))
		return( NULL );
	if( AllocValue )
	{
		if( !( Variable->Value = (DT_VALUE *) GSHeapMalloc( DtValueHeap )))
		{
			GSHeapFree( SlangVariableHeap, Variable );
			return( NULL );
		}
		DTM_INIT( Variable->Value );
	}
	return( Variable );
}



/****************************************************************
**	Routine: SlangVariableAllocLocal
**	Returns: SLANG_VARIABLE *
**	Action : Allocates a local variable
****************************************************************/

SLANG_VARIABLE *SlangVariableAllocLocal(
	const char	*Name,
	int			Flags
)
{
	SLANG_VARIABLE
			*Var = SlangVariableAlloc( !( Flags & SLANG_VARIABLE_DESTROYED ));

	if( Flags & SLANG_VARIABLE_DESTROYED )
		Flags |= SLANG_VARIABLE_RETAIN;

	Var->Flags |= Flags | SLANG_VARIABLE_LOCAL;
	Var->Name = Name;
	return Var;
}



/****************************************************************
**	Routine: SlangScopeLocalsToHash
**	Returns: void
**	Action : Converts Locals to Hash
****************************************************************/

void SlangScopeLocalsToHash(
	SLANG_SCOPE_FUNCTION	*ScopeFunc
)
{
	if( !ScopeFunc->Locals )
		return;
        
	if( !ScopeFunc || ScopeFunc->VariableHash )
		return;

	ScopeFunc->VariableHash = HashCreate( (char *) ScopeFunc->FunctionName, (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );
	for( int Num = ScopeFunc->SlangFunc->Locals->KeyCount - 1; Num >= 0; --Num )
		if( ScopeFunc->Locals[ Num ])
			HashInsert( ScopeFunc->VariableHash, ScopeFunc->Locals[ Num ]->Name, ScopeFunc->Locals[ Num ]);
	free( ScopeFunc->Locals );
	ScopeFunc->Locals = NULL;
}



/****************************************************************
**	Routine: SlangVariableGetScopedByBucket
**	Returns: SLANG_VARIABLE * / NULL
**	Action : looks up a slang variable in the given scope
****************************************************************/

SLANG_VARIABLE *SlangVariableGetScopedByBucket(
	HASH			*VarScope,
	const char		*Name,
	unsigned long	Bucket
)
{
	SLANG_VARIABLE
			*Variable = (SLANG_VARIABLE *) HashLookupByBucket( VarScope ? VarScope : SlangVariableHash, Name, Bucket );

	return Variable && !( Variable->Flags & SLANG_VARIABLE_DESTROYED ) ? Variable : NULL;
}



/****************************************************************
**	Routine: SlangVariableGetByBucket
**	Returns: SLANG_VARIABLE* /NULL
**	Action : Looks up a slang variable from the current scope 
****************************************************************/

SLANG_VARIABLE *SlangVariableGetByBucket(
	const char				*Name,
	SLANG_SCOPE_FUNCTION	*Scope,
	unsigned long			Bucket
)
{
	if( Scope )
	{
		if( Scope->Locals )
		{
			long		Local = (long) HashLookupByBucket( Scope->SlangFunc->Locals, Name, Bucket );

			SLANG_VARIABLE
					*Variable = Local ? Scope->Locals[ Local - 1 ] : NULL;

			return Variable && !( Variable->Flags & SLANG_VARIABLE_DESTROYED ) ? Variable : NULL;
		}
		return SlangVariableGetScopedByBucket( Scope->VariableHash, Name, Bucket );
	}
	return SlangVariableGetScopedByBucket( SlangVariableHash, Name, Bucket );
}



/****************************************************************
**	Routine: SlangVariableSetByBucket
**	Returns: Pointer to variable
**			 NULL - variable not found
**	Action : Set a variable's value (create the variable if needed)
****************************************************************/

SLANG_VARIABLE *SlangVariableSetByBucket(
	const char				*Name,		// Name of variable
	DT_VALUE				*Value,		// Value of variable
	SLANG_SCOPE_FUNCTION	*Scope,		// Slang Scope in which to set variable - NULL means global scope
	int						Flags,		// Variable flags
	unsigned long			Bucket		// Hash bucket
)
{
	if( !Value )
		Flags &= ~SLANG_VARIABLE_RETAIN;

	if( Scope && Scope->Locals )
	{
		long Local = (long) HashLookupByBucket( Scope->SlangFunc->Locals, Name, Bucket );
		if( !Local )
		{
			SlangScopeLocalsToHash( Scope );
			return SlangVariableSetScopedByBucket( Scope->VariableHash, Name, Value, Flags, Bucket );
		}

		SLANG_VARIABLE *Variable;
		if( !( Variable = Scope->Locals[ --Local ]))
		{
			Variable = Scope->Locals[ Local ] = SlangVariableAllocLocal( Name, Flags );
			SlangVariableIncrRef( Variable );
		}

		return SlangVariableSetValue( Variable, Value, Flags ) ? Variable : NULL;
	}
	return SlangVariableSetScopedByBucket( Scope ? Scope->VariableHash : SlangVariableHash, Name, Value, Flags, Bucket );
}



/****************************************************************
**	Routine: SlangVariableSetScopedByBucket
**	Returns: SLANG_VARIABLE * / NULL
**	Action : Sets variables value
****************************************************************/

SLANG_VARIABLE * SlangVariableSetScopedByBucket(
	HASH			*VarScope,
	const char		*Name,
	DT_VALUE		*Value,
	int				Flags,
	unsigned long	Bucket
)
{
	// If the variable doesn't exist, create a new variable
	if( strlen( Name ) >= SLANG_VARIABLE_NAME_SIZE )
	{
		Err( ERR_OVERFLOW, "Variable name '%s' is too long", Name );
		return( NULL );
	}

	if( ! VarScope )
		VarScope = SlangVariableHash;

	SLANG_VARIABLE *Variable = (SLANG_VARIABLE *) HashLookupByBucket( VarScope, Name, Bucket );
	if( !Variable )
	{
		Variable = SlangVariableAlloc( !( Flags & SLANG_VARIABLE_RETAIN ));
		Variable->Name = strdup( Name );
		Variable->Flags |= SLANG_VARIABLE_FREE_NAME | ( Flags & SLANG_VARIABLE_RETAIN );
		Variable->HashTable			= VarScope;
		Variable->ReferenceCount	= 1;
		HashInsertByBucket( VarScope, Variable->Name, Bucket, Variable );
	}

	return SlangVariableSetValue( Variable, Value, Flags ) ? Variable : NULL;
}



/****************************************************************
**	Routine: SlangVariableSetLocal
**	Returns: SLANG_VARIABLE * / NULL in case of nasty error
**	Action : sets a local variable
****************************************************************/

SLANG_VARIABLE *SlangVariableSetLocal(
	SLANG_SCOPE_FUNCTION	*ScopeFunc,
	int						Num,
	DT_VALUE				*Value,
	int						Flags
)
{
	if( !ScopeFunc->Locals )
		return SlangVariableSet( ScopeFunc->SlangFunc->LocalNames[ Num ], Value, ScopeFunc, Flags );

	SLANG_VARIABLE
			**Variable = ScopeFunc->Locals + Num;

	if( !*Variable )
		SlangVariableIncrRef( *Variable = SlangVariableAllocLocal( ScopeFunc->SlangFunc->LocalNames[ Num ], SLANG_VARIABLE_DESTROYED ));
	SlangVariableSetValue( *Variable, Value, Flags );
	return *Variable;
}



/****************************************************************
**	Routine: SlangVariableSetValue
**	Returns: TRUE/Err
**	Action : Set an existing variable's value
****************************************************************/

int SlangVariableSetValue(
	SLANG_VARIABLE	*Variable,			// Variable to set
	DT_VALUE		*Value,				// Value of variable
	int				Flags				// Variable flags
)
{
	if( !Value )
		Flags &= ~SLANG_VARIABLE_RETAIN;

	// Check protection bit
	if( Variable->Flags & SLANG_VARIABLE_PROTECT )
		return Err( SDB_ERR_OBJECT_IN_USE, "'%s' variable is in use", Variable->Name );

	// If variable does exist, free value unless Var->Flags & SLANG_VARIABLE_RETAIN
	if( Variable->Flags & SLANG_VARIABLE_RETAIN )
	{
		if( !( Flags & SLANG_VARIABLE_RETAIN ))
		{
			if( !( Variable->Value = (DT_VALUE *) GSHeapMalloc( DtValueHeap )))
				return Err( ERR_MEMORY, "'%s' can't allocate DT_VALUE for Variable", Variable->Name );
		}
	}
	else
	{
		DTM_FREE( Variable->Value );
		if( Flags & SLANG_VARIABLE_RETAIN )
			GSHeapFree( DtValueHeap, Variable->Value );
	}

	// Now we have the variable, so assign the value
	if( Flags & SLANG_VARIABLE_RETAIN )
		Variable->Value = Value;
	else if( Value )
	{
		if( Flags & SLANG_VARIABLE_NO_COPY )
			*Variable->Value = *Value;
		else
			DTM_ASSIGN( Variable->Value, Value );
	}
	else
		DTM_INIT( Variable->Value );

	Variable->Flags = ( Variable->Flags & ~(SLANG_VARIABLE_RETAIN | SLANG_VARIABLE_NO_COPY | SLANG_VARIABLE_DESTROYED) ) | Flags;
	return TRUE;
}



/****************************************************************
**	Routine: SlangVariableScopeGet
**	Returns: Pointer to variable scope
**			 NULL - variable scope not found
**	Action : Get a variable scope by name
****************************************************************/

SLANG_VARIABLE_SCOPE *SlangVariableScopeGet(
	const char	*Name,		// Name of scope
	int			AutoCreate	// Create it if necessary
)
{
	SLANG_VARIABLE_SCOPE
			*VarScope;


	VarScope = (SLANG_VARIABLE_SCOPE *) HashLookup( SlangVarScopeHash, Name );
	if( !VarScope && AutoCreate )
	{
		char *Title = strdup( Name ? Name : "Unnamed" );
		VarScope = new SLANG_VARIABLE_SCOPE;
		// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
		VarScope->VariableHash = HashCreate( Title, (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 100, NULL );
		VarScope->UninitializedVarHook = NULL;
		HashInsert( SlangVarScopeHash, Title, VarScope );
	}
	return VarScope;
}



/****************************************************************
**	Routine: SlangVariableScopeDestroy
**	Returns: TRUE
**	Action : Destroys a variable scope
****************************************************************/

int SlangVariableScopeDestroy(
	SLANG_VARIABLE_SCOPE	*VarScope
)
{
	SlangVariableHashDestroy( VarScope->VariableHash );
	free( VarScope );
	return TRUE;
}



/****************************************************************
**	Routine: SlangVariableHashDestroy
**	Returns: TRUE
**	Action : Destroys all the variables in a hash
****************************************************************/

int SlangVariableHashDestroy(
	HASH	*VarHash
)
{
	HASH_ENTRY_PTR
			Ptr;

	SLANG_VARIABLE
			*Variable;

	HASH_FOR( Ptr, VarHash )
	{
		Variable = (SLANG_VARIABLE *) HashValue( Ptr );
		SlangVariableFree( Variable );
	}
	HashDestroy( VarHash );
	return TRUE;
}



/****************************************************************
**	Routine: SlangVariableScopes
**	Returns: Pointer to allocated array of variable scope ptrs
**			 NULL - out of memory
**	Action : Get all scopes
****************************************************************/

SLANG_VARIABLE_SCOPE **SlangVariableScopes(
	void
)
{
	SLANG_VARIABLE_SCOPE
			**Scopes;

	HASH_ENTRY
			*Entry;

	int		Count = 0;


	HASH_FOR( Entry, SlangVarScopeHash )
		Count++;

	Scopes = (SLANG_VARIABLE_SCOPE **) calloc( Count + 1, sizeof( *Scopes ) ) ;
	if( Scopes )
	{
		Count = 0;
		HASH_FOR( Entry, SlangVarScopeHash )
			Scopes[ Count++ ] = (SLANG_VARIABLE_SCOPE *) HashValue( Entry );
	}
	else
		Err( ERR_MEMORY, "SlangVariableScopes, @" );

	return Scopes;
}



/****************************************************************
**	Routine: SlangVariableFree
**	Returns: Nothing
**	Action : Decrements reference count and frees variable if zero
**			 (assumes that symbol table is already taken care of).
****************************************************************/

void SlangVariableFree(
	SLANG_VARIABLE	*Variable
)
{
	if( !Variable )
		return;


/*
**	If variable still has references, keep it around.  Except
**	if variable is no longer in any scope, make sure at least some of
**	the references are external with DtPointerGarbageCollect.
*/

	if( --Variable->ReferenceCount > 0 )
	{
		if( !Variable->HashTable && !( Variable->Flags & ( SLANG_VARIABLE_TEMPORARY | SLANG_VARIABLE_LOCAL )))
			DtPointerCollectGarbage( Variable );

		return;
	}

	SlangVariableDestroy( Variable );

	if( !( Variable->Flags & SLANG_VARIABLE_RETAIN ))
	{
		DTM_FREE( Variable->Value );
		GSHeapFree( DtValueHeap, Variable->Value );
	}

	if( Variable->Flags & SLANG_VARIABLE_FREE_NAME )
		free( (void *) Variable->Name ); // FIX- shouldn't cast away const
	GSHeapFree( SlangVariableHeap, Variable );
}



/****************************************************************
**	Routine: SlangVariableIncrRef
**	Returns: Nothing
**	Action : Increments reference count of variable
****************************************************************/

SLANG_VARIABLE *SlangVariableIncrRef(
	SLANG_VARIABLE	*Variable
)
{
	Variable->ReferenceCount++;
	return( Variable );
}



/****************************************************************
**	Routine: SlangVariableDestroyAll
**	Returns: nothing
**	Action : Destroy all variables in a Variable Scope
****************************************************************/

static void SlangVariableDestroyAll(
	HASH	*HashTable
)
{
	HASH_ENTRY_PTR
			Ptr;

	SLANG_VARIABLE
			*Variable;


	HASH_FOR( Ptr, HashTable )
	{
		Variable = (SLANG_VARIABLE *) HashValue( Ptr );
		SlangVariableDestroy( Variable );
	}
}



/****************************************************************
**	Routine: SlangVariableDestroy
**	Returns: TRUE/FALSE
**	Action : Destroys a Variable from the HashTable which it
**			 belongs to
****************************************************************/

int SlangVariableDestroy(
	SLANG_VARIABLE	*Variable
)
{
	if( !Variable )
		return TRUE;

	Variable->Flags &= ~SLANG_VARIABLE_PROTECT;

	if( Variable->Flags & SLANG_VARIABLE_DESTROYED )
		return TRUE;

	SlangVariableSetValue( Variable, NULL, SLANG_VARIABLE_DESTROYED );
	return TRUE;
}



/****************************************************************
**	Routine: SlangVariableDestroyByName
**	Returns: TRUE	- Variable destroyed without error
**			 FALSE	- Variable not found
**	Action : Destroy a variable or all of the variables
****************************************************************/

int SlangVariableDestroyByName(
	const char				*Name,		// Name of variable, NULL for all variables
	SLANG_SCOPE_FUNCTION	*Scope		// Scope from which to destroy it, NULL means global scope
)
{
	SLANG_VARIABLE
			*Variable;

	HASH	*HashTable = Scope ? Scope->VariableHash : SlangVariableHash;


	if( HashTable )
	{
		// Single variable
		if( Name )
		{
			if( !(Variable = (SLANG_VARIABLE *) HashLookup( HashTable, Name )))
				return FALSE;

			if( ( Variable->Flags & SLANG_VARIABLE_PROTECT ))
				return Err( SDB_ERR_OBJECT_IN_USE, "'%s' variable is in use", Name );

			return SlangVariableDestroy( Variable );
		}

		// All variables in this scope
		SlangVariableDestroyAll( HashTable );
	}
	else if( Scope->SlangFunc->Locals )
	{
		// Single variable
		if( Name )
		{
			long Local = (long) HashLookup( Scope->SlangFunc->Locals, Name );
			if( !Local || !( Variable = Scope->Locals[ Local - 1 ]) || (Variable->Flags & SLANG_VARIABLE_DESTROYED ))
				return Err( ERR_NOT_FOUND, "Variable '%s' does not exist. Cannot destroy", Name );
			if( Variable->Flags & SLANG_VARIABLE_PROTECT )
				return Err( SDB_ERR_OBJECT_IN_USE, "'%s' variable is in use", Name );
			return SlangVariableDestroy( Variable );
		}
		else
			for( int Num = Scope->SlangFunc->Locals->KeyCount - 1; Num >= 0; --Num )
				SlangVariableDestroy( Scope->Locals[ Num ]);
	}
	else
		return Err( ERR_NOT_CONSISTENT, "VariableDestroy(): No Hash table and no Locals" );
			
	return TRUE;
}



/****************************************************************
**	Routine: SlangEnumFirstVar
**	Returns: Pointer to first variable
**			 NULL if no variables
**	Summary: Start enumeration of variable names
****************************************************************/

SLANG_VARIABLE *SlangEnumFirstVar(
	SDB_ENUM_PTR	*EnumPtr,	// Enumeration pointer
	HASH			*HashTable	// Hash table to use - NULL means global
)
{
	if( !HashTable )
		HashTable = SlangVariableHash;

	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));

	(*EnumPtr)->Hash	= HashTable;
	(*EnumPtr)->Count	= HashTable->KeyCount;
	(*EnumPtr)->Ptr		= HashFirst( HashTable );
	if( HashEnd( (*EnumPtr)->Ptr ))
		return 0;
	return( (SLANG_VARIABLE *) HashValue( (*EnumPtr)->Ptr ));
}



/****************************************************************
**	Routine: SlangEnumNextVar
**	Returns: Pointer to next variable
**			 NULL if no more variables
**	Summary: Get the next variable
****************************************************************/

SLANG_VARIABLE *SlangEnumNextVar(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashNext( EnumPtr->Hash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return 0;
	return( (SLANG_VARIABLE *) HashValue( EnumPtr->Ptr ));
}



/****************************************************************
**	Routine: SlangEnumVarClose
**	Returns: Nothing
**	Summary: End enumeration of variables
**	Action : End enumeration of variables
****************************************************************/

void SlangEnumVarClose(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
		free( EnumPtr );
}



/****************************************************************
**	Routine: SlangContextSet
**	Returns: None
**	Summary: Set a value in the context
**	Action : Set an element within the slang context.  If Value
**			 is NULL, the old element is removed and no new
**			 element is inserted.
**	SeeAlso: SlangContextGet, SlangContextPush, SlangContextPop
****************************************************************/

void SlangContextSet(
	const char	*Name,		// Name of element
	void		*Value		// Value of element
)
{
	SLANG_CONTEXT_ELEMENT
			*Element;

	HASH	*Hash = SlangContext->Hash;


	Element = (SLANG_CONTEXT_ELEMENT *) HashLookup( Hash, Name );
	if( Element )
	{
		HashDelete( Hash, Name );
		MemPoolFree( Hash->MemPool, Element->Name );
		MemPoolFree( Hash->MemPool, Element );
	}

	if( Value )
	{
		Element			= (SLANG_CONTEXT_ELEMENT *) MemPoolMalloc( Hash->MemPool, sizeof( SLANG_CONTEXT_ELEMENT ));
		Element->Name	= MemPoolStrDup( Hash->MemPool, Name );
		Element->Value	= Value;
		HashInsert( Hash, Element->Name, Element );
	}

	// Special case the abort stuff for speed purposes
	if( !strnicmp( "Abort ", Name, 6 ))
	{
		if( !stricmp( "Function", Name + 6 ))
			SlangContext->AbortFunc = (SLANG_ABORT_FUNC) Value;
		else if( !stricmp( "Handle", Name + 6 ))
			SlangContext->AbortHandle = Value;
		else if( !stricmp( "Check Granularity", Name + 6 ))
			SlangContext->AbortCheckGranularity = (int) (long) Value;
	}
	if( !stricmp( "Debug Context", Name ))
		SlangContext->DebugContext = (SlangDebugContextStructure *) Value;

	SlangContext->UpdateCount = SlangContextUpdateCount++;
}



/****************************************************************
**	Routine: SlangContextGet
**	Returns: Value associated with the name or NULL
**	Action : Get information from the slang context
**	Summary: Retrieve a value from the context
**	SeeAlso: SlangContextPush, SlangContextPop, SlangContextSet
****************************************************************/

void *SlangContextGet(
	const char	*Name
)
{
	SLANG_CONTEXT_ELEMENT
			*Element;


	Element = (SLANG_CONTEXT_ELEMENT *) HashLookup( SlangContext->Hash, Name );

	return Element? Element->Value : NULL;
}



/****************************************************************
**	Routine: SlangContextPush
**	Returns: None
**	Action : Push a new context onto the stack
**	Summary: Push a new context onto the stack
**	SeeAlso: SlangContextGet, SlangContextSet, SlangContextPop
****************************************************************/

void SlangContextPush(
	int		DupeFlag	// TRUE to copy previous context
)
{
	SLANG_CONTEXT
			*NewContext;


	NewContext = (SLANG_CONTEXT *) calloc( 1, sizeof( *NewContext ));
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	NewContext->Hash = HashCreate( "Slang Context", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, MemPoolCreate());
	NewContext->Next = SlangContext;

	if( DupeFlag )
	{
		SLANG_CONTEXT_ELEMENT
				*OldElement,
				*NewElement;

		HASH_ENTRY_PTR
				Ptr;


		HASH_FOR( Ptr, SlangContext->Hash )
		{
			OldElement = (SLANG_CONTEXT_ELEMENT *) HashValue( Ptr );
			NewElement = (SLANG_CONTEXT_ELEMENT *) MemPoolMalloc( NewContext->Hash->MemPool, sizeof( SLANG_CONTEXT_ELEMENT ));
			NewElement->Name	= MemPoolStrDup( NewContext->Hash->MemPool, OldElement->Name );
			NewElement->Value	= OldElement->Value;
			HashInsert( NewContext->Hash, NewElement->Name, NewElement );
		}
		NewContext->AbortFunc				= SlangContext->AbortFunc;
		NewContext->AbortHandle				= SlangContext->AbortHandle;
		NewContext->AbortCheckGranularity	= SlangContext->AbortCheckGranularity;
	}
	else
		NewContext->AbortCheckGranularity	= 1;

	SlangContext = NewContext;
	SlangContext->UpdateCount = SlangContextUpdateCount++;
}



/****************************************************************
**	Routine: SlangContextPop
**	Returns: None
**	Action : Pop a context from the stack
**	Summary: Pop a context from the stack
**	SeeAlso: SlangContextGet, SlangContextSet, SlangContextPush
****************************************************************/

void SlangContextPop( void )
{
	SLANG_CONTEXT
			*Context = SlangContext->Next;


	MemPoolDestroy( SlangContext->Hash->MemPool );
	free( SlangContext );
	SlangContext = Context;
}



/****************************************************************
**	Routine: SlangRetFree
**	Returns: nothing
**	Action : Free memory allocated to a SLANG_RET structure
****************************************************************/

void SlangRetFree(
	SLANG_RET	*Ret
)
{
	int		Arg;


	if( Ret->Type == SLANG_TYPE_VALUE )
		DTM_FREE( &Ret->Data.Value );
	else if( (Ret)->Type == SLANG_VALUE_TYPE && Ret->Data.Eval.Argv )
	{
		for( Arg = 0; Arg < Ret->Data.Eval.Argc; Arg++ )
			if( Ret->Data.Eval.Argv[ Arg ] )
			{
				DTM_FREE( Ret->Data.Eval.Argv[ Arg ] );
				free( Ret->Data.Eval.Argv[ Arg ] );
			}
		free( Ret->Data.Eval.Argv );
		Ret->Data.Eval.Argv = NULL;
	}
}



/****************************************************************
**	Routine: SlangEvalWarnV
**	Returns: SLANG_ERROR
**	Summary: Slang error/warning handler
****************************************************************/

SLANG_RET_CODE SlangEvalWarnV(
	SLANG_NODE		*Root,		// Node where error ocurred
	SLANG_EVAL_FLAG	EvalFlag,	// EvalFlag for function where error occured
	SLANG_RET		*Ret,		// Ret for function where error occured
	SLANG_SCOPE		*Scope,		// Scope when error ocurred
	int				ErrCode,
	const char 		*Format,	// printf compatible format string
	va_list			Marker		// printf in a va_list
)
{
	static SLANG_ERROR_FUNCTION
			ErrorFunction;

	static void
			*ErrorHandle;

	static long
			UpdateCount;

    SLANG_RET_CODE
			RetVal;


/*
**	Set the error information
*/

	Scope->FunctionStack->ErrorInfo = &Root->ErrorInfo;
	SLANG_SET_ERROR( Root );

	if( Format && Format != ERR_STR_DONT_CHANGE )
	{
		ErrV( Format, Marker );
		ErrNum = SDB_ERR_SLANG_EVAL;
	}


/*
**	If there is an error handling function in the context, call it
*/

	if( UpdateCount != SlangContext->UpdateCount )
	{
		UpdateCount		= SlangContext->UpdateCount;
		ErrorFunction	= (SLANG_ERROR_FUNCTION) SlangContextGet( "Error Function" );
		ErrorHandle		= SlangContextGet( "Error Handle" );
	}

	if( ErrorFunction )
	{
		RetVal = (*ErrorFunction)( ErrorHandle, Root, EvalFlag, Ret, Scope, ErrCode );
		while( Ret && RetVal == SLANG_RETRY )
			RetVal = SlangEvaluate( Root, EvalFlag, Ret, Scope );
	}
	else
		RetVal = SLANG_ERROR;

	return RetVal;
}



/****************************************************************
**	Routine: SlangEvalError
**	Returns: SLANG_ERROR
**	Summary: Slang error handler
****************************************************************/

SLANG_RET_CODE SlangEvalError(
	SLANG_NODE		*Root,		// Node where error ocurred
	SLANG_EVAL_FLAG	EvalFlag,	// EvalFlag for function where error occured
	SLANG_RET		*Ret,		// Ret for function where error occured
	SLANG_SCOPE		*Scope,		// Scope when error ocurred
	const char 		*Format,	// printf compatible format string
	...							// Optional printf arguments
)
{
    va_list	Marker;

	SLANG_RET_CODE
			RetCode;


	va_start( Marker, Format );
	RetCode = SlangEvalWarnV( Root, EvalFlag, Ret, Scope,
                              SLANG_ERROR_UNKNOWN, Format, Marker );
	va_end( Marker );

	if( Ret && RetCode == SLANG_ERROR )
		Ret->Type = SLANG_TYPE_NO_VALUE;

	return RetCode;
}



/****************************************************************
**	Routine: SlangEvalWarn
**	Returns: SLANG_ERROR
**	Summary: Slang warning handler
****************************************************************/

SLANG_RET_CODE SlangEvalWarn(
	SLANG_NODE		*Root,		// Node where error ocurred
	SLANG_EVAL_FLAG	EvalFlag,	// EvalFlag for function where error occured
	SLANG_RET		*Ret,		// Ret for function where error occured
	SLANG_SCOPE		*Scope,		// Scope when error ocurred
	int				WarnCode,	// Also could be an error code
	const char 		*Format,	// printf compatible format string
	...							// Optional printf arguments
)
{
    va_list	Marker;

	SLANG_RET_CODE
			RetCode;


	va_start( Marker, Format );
	RetCode = SlangEvalWarnV( Root, EvalFlag, Ret, Scope,
                              WarnCode, Format, Marker );
	va_end( Marker );

	return RetCode;
}



/****************************************************************
**	Routine: SlangArgInfo
**	Returns: Structured usage info (SLANG_ARG_INFO)
**	Action : Format argument list into returned structure
**	Summary: Used by SlangFunctionInfo
****************************************************************/

int SlangArgInfo(
	SLANG_NODE				*Root,		// Root node passed to SlangX function
	SLANG_EVAL_FLAG			EvalFlag,	// Eval flag passed to SlangX function
	SLANG_RET				*Ret,		// Ret structure passed to SlangX function
	SLANG_SCOPE				*Scope,		// Scope passed to SlangX function
	const SLANG_ARG_LIST	*ArgList,	// SLANG_ARG_LIST for function
	int						ArgCount	// SLANG_ARG_COUNT macro
)
{
	const SLANG_ARG_LIST
			*Arg;

	DT_VALUE
			ArgArray,
			SubStruct;

	int		ArgNum,
			Status = TRUE;


	if( !DTM_ALLOC( &Ret->Data.Value, DtStructure )
			|| !DTM_ALLOC( &ArgArray, DtArray )
			|| !DTM_ALLOC( &SubStruct, DtStructure ))
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );

	for( ArgNum = 0; ArgNum < ArgCount; ArgNum++ )
	{
		Arg = &ArgList[ ArgNum ];
		Status = Status && DtComponentSetPointer( &SubStruct, "Name",			DtString, NULLSTR( Arg->Name ));
		Status = Status && DtComponentSetPointer( &SubStruct, "DataType",		DtString, ( Arg->DataType && *Arg->DataType ) ? (*Arg->DataType)->Name : "*" );
		Status = Status && DtComponentSetPointer( &SubStruct, "Description",	DtString, NULLSTR( Arg->Description ));
		Status = Status && DtComponentSetNumber(  &SubStruct, "Flags",			DtDouble, (double) Arg->Flags );
		Status = Status && DtAppend( &ArgArray, &SubStruct );
	}
	Status = Status && DtComponentSet( &Ret->Data.Value, "Arguments", &ArgArray );
	DTM_FREE( &ArgArray );
	DTM_FREE( &SubStruct );
	if( !Status )
	{
		DTM_FREE( &Ret->Data.Value );
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	}
	Ret->Type = SLANG_TYPE_VALUE;
	return( SLANG_ARG_INFO );
}



/****************************************************************
**	Routine: SlangArgParse
**	Returns: SLANG_OK or SLANG_ERROR
**	Action : Parse SlangX arguments and display error if necessary
****************************************************************/

int SlangArgParse(
	SLANG_NODE				*Root,		// Root node passed to SlangX function
	SLANG_EVAL_FLAG			EvalFlag,	// Eval flag passed to SlangX function
	SLANG_RET				*Ret,		// Ret structure passed to SlangX function
	SLANG_SCOPE				*Scope,		// Scope passed to SlangX function
	const SLANG_ARG_LIST	*ArgList,	// SLANG_ARG_LIST for function
	int						ArgCount,	// SLANG_ARG_COUNT macro
	SLANG_RET				*Rets		// SLANG_ARG_VARS variable
)
{
	int		ArgNum,
			RetCode,
			Flags,
			HasBlock,
			NeedVariable,
			CheckDataType,
			Eachiness = -1;

	SLANG_EVAL_FLAG
			ArgEvalFlag;

	SLANG_NODE
			*ArgNode;

	DT_VALUE
			ValuePtr,
			*PtrValue,
			**EachArray = NULL;

	DT_POINTER
			*Ptr;


/*
**	Check if last argument is a block, then just do not parse it.
*/

	HasBlock = ArgCount > 0 && ( ArgList[ ArgCount - 1 ].Flags & SLANG_ARG_BLOCK );


/*
**	Check number of arguments
*/

	if( Root->Argc != ArgCount )
	{
		// Negative count means spill usage
		if( Root->Argc < 0 )
		{
			if( Root->Argc == SLANG_ARGC_INFO )
				return SlangArgInfo( SLANG_ERROR_PARMS, ArgList, ArgCount );
			return SlangArgError( SLANG_ERROR_PARMS, ArgList, ArgCount );
		}

		// Too many
		if( Root->Argc > ArgCount )
		{
			RetCode = SlangArgError( SLANG_ERROR_PARMS, ArgList, ArgCount );
			ErrMore( "Too many arguments (%d)\n", Root->Argc );
			return( RetCode );
		}

		// Zero always too few if expecting an attached block
		if( Root->Argc == 0 && HasBlock )
		{
			RetCode = SlangArgError( SLANG_ERROR_PARMS, ArgList, ArgCount );
			ErrMore( "Attached block required\n" );
			return( RetCode );
		}

		// Too few
		if( !( ArgList[ Root->Argc - HasBlock ].Flags & SLANG_ARG_OPTIONAL ))
		{
			RetCode = SlangArgError( SLANG_ERROR_PARMS, ArgList, ArgCount );
			ErrMore( "%s - argument required\n", ArgList[ Root->Argc - HasBlock ].Name );
			return( RetCode );
		}
	}


/*
**	Evaluate arguments
*/

	for( ArgNum = 0; ArgNum < Root->Argc - HasBlock; ArgNum++ )
	{
		Flags   = ArgList[ ArgNum ].Flags;
		ArgNode = Root->Argv[ ArgNum ];

		if(( Flags & SLANG_ARG_LVALUE ))
		{
			RetCode 		= SLANG_OK;
			NeedVariable 	= Flags & SLANG_ARG_VAR_FLAG;
			CheckDataType	= ArgList[ ArgNum ].DataType != NULL;
			if( NeedVariable && !CheckDataType )
				ArgEvalFlag = SLANG_EVAL_LVALUE_CREATE;	// Could be a new variable
			else
				ArgEvalFlag	= SLANG_EVAL_LVALUE;		// Ow must be preexisting

			// Evaluate as LValue
			if( RetCode == SLANG_OK )
				RetCode = SlangEvaluate( ArgNode, ArgEvalFlag, &Rets[ ArgNum ], Scope );
			if( RetCode == SLANG_OK && Rets[ ArgNum ].Type != SLANG_TYPE_VALUE && Rets[ ArgNum ].Type != SLANG_TYPE_STATIC_VALUE )
			{
				SLANG_RET_FREE( &Rets[ ArgNum ] );
				RetCode = SlangEvalError( SLANG_ERROR_PARMS, "%s: Arg[%d] '%s' - must be an lvalue", Root->StringValue, ArgNum, ArgList[ ArgNum ].Name );
			}

			// VAR_FLAG gets DtPointer, otherwise dereference DtPointer
			// But even if VAR_FLAG, need to dereference it if the datatype needs to be checked
			if( RetCode == SLANG_OK && ( !NeedVariable || CheckDataType ))
			{
				// Dereference the DtPointer (FIX - this will fail for non-native pointers, e.g. Curve[i])
				if( !DT_OP( DT_MSG_DEREFERENCE, &ValuePtr, &Rets[ ArgNum ].Data.Value, NULL ))
				{
					RetCode = SlangEvalError( SLANG_ERROR_PARMS, "%s: Arg[%d] '%s' - must be a dereferencable lvalue, e.g., a variable", Root->StringValue, ArgNum, ArgList[ ArgNum ].Name );
					goto FreeArgsAndExit;
				}

				// Check the datatype
				if( RetCode == SLANG_OK && CheckDataType && ((DT_VALUE *) ValuePtr.Data.Pointer)->DataType != *ArgList[ ArgNum ].DataType )
				{
					RetCode = SlangEvalError( SLANG_ERROR_PARMS, "%s: Arg[%d] '%s' - must be of type %s", Root->StringValue, ArgNum, ArgList[ ArgNum ].Name, (*ArgList[ ArgNum ].DataType)->Name );
					goto FreeArgsAndExit;
				}

				// Replace the DtPointer in Ret with the pointed to value (see SLANG_LVALUE)
				if( RetCode == SLANG_OK && !NeedVariable )
				{
					// FIX - we know that DT_MSG_DEREFERNCE worked, so freeing DtPointer is harmless
					SLANG_RET_FREE( &Rets[ ArgNum ] );
					Rets[ ArgNum ].Type = SLANG_TYPE_STATIC_VALUE;
					Rets[ ArgNum ].Data.Value = ValuePtr;
				}
			}

			// Check that it is in fact a variable
			if( NeedVariable )
			{
				PtrValue = &Rets[ ArgNum ].Data.Value;
				Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );
				if( PtrValue->DataType != DtPointer || PtrVariable != Ptr->Type )
				{
					RetCode = SlangEvalError( SLANG_ERROR_PARMS, "%s: Arg[%d] '%s' - must be a variable", Root->StringValue, ArgNum, ArgList[ ArgNum ].Name );
					goto FreeArgsAndExit;
				}
			}
		}
		else
			RetCode = SlangEvaluate( ArgNode, SLANG_EVAL_RVALUE, &Rets[ ArgNum ], Scope );

		if( SLANG_OK != RetCode && SLANG_EXCEPTION != RetCode )
		{
			if( SLANG_RETURN != RetCode )
				Rets[ ArgNum ].Type = SLANG_TYPE_NO_VALUE; // don't free current arg
			if( SLANG_ABORT != RetCode )
				RetCode = SLANG_ERROR;
		}
		else if (SLANG_EXCEPTION == RetCode )
		{
			*Ret = Rets [ ArgNum ];
			DTM_INIT(&Rets[ArgNum].Data.Value); // so that we won't free the exceptionsdb_eval.c
		}
		else if( !( Flags & SLANG_ARG_LVALUE ))
		{
			if( ArgList[ ArgNum ].DataType )
			{
				// Got the required specific datatype for this arg
				if( *ArgList[ ArgNum ].DataType == Rets[ ArgNum ].Data.Value.DataType )
						continue;

				// Got a Null(), but func says that is ok
				if(( ArgList[ ArgNum ].Flags & SLANG_ARG_NULLS ) && DtNull == Rets[ ArgNum ].Data.Value.DataType )
						continue;

				// Got a GsDt, which is not what was requested.  Try converting it.

				if( ( SLANG_TYPE_STATIC_VALUE == Rets[ ArgNum ].Type ) && ( DtGsDt == Rets[ ArgNum ].Data.Value.DataType ) )
				{
					DT_VALUE
							ConvertedGsDt;

					DTM_INIT( &ConvertedGsDt );

					
					if( GsDtToDtValue( *( ( const GsDt * ) DT_VALUE_TO_POINTER( &( Rets[ ArgNum ].Data.Value ) ) ),
									   &ConvertedGsDt, GSDT_TO_DTV_FAIL_WITH_DTGSDT ) )
					{
						if( *( ArgList[ ArgNum ].DataType ) == ConvertedGsDt.DataType )
						{
							// Conversion got us what we want.  Make it the arg and go on.
							// No need to DTM_ASSIGN, just bit copy since we no longer
							// care about the temporary ConvertedGsDt and will free it when
							// we free Rets[ ArgNum ] later.
							SLANG_RET_FREE( &Rets[ ArgNum ] );
							Rets[ ArgNum ].Data.Value = ConvertedGsDt;							
							continue;
						}
						else 
							DTM_FREE( &ConvertedGsDt );
					}									   
				}
			}

			// Not the specifically requested datatype, how about an Each()
			if( Rets[ ArgNum ].Data.Value.DataType == DtEach && !( ArgList[ ArgNum ].Flags & SLANG_ARG_EACH ))
			{
				int NewSize = DtSize( &Rets[ ArgNum ].Data.Value );

				// First Each() arg seen
				if( Eachiness == -1 )
				{
					Eachiness = NewSize;
					ERR_CALLOC( EachArray, DT_VALUE*, Root->Argc, sizeof( DT_VALUE * ));
				}

				// Matches size of other Each() args seen so far
				if( Eachiness == NewSize )
				{
					// Keep track of Each() args for later call to SlangApplyEach
					EachArray[ ArgNum ] = &Rets[ ArgNum ].Data.Value;
					continue;
				}

				// Does not match size of other Each() args
				RetCode = SlangEvalError( SLANG_ERROR_PARMS, "%s: Arg %d (%s), an Each() of size %d does not the match the size (%d) of other Each() arguments",
						Root->StringValue,
						ArgNum,
						ArgList[ ArgNum ].Name,
						NewSize,
						Eachiness );
			}

			// But the func may not care what type it is
			else if( !ArgList[ ArgNum ].DataType )
				continue;

			// Invalid datatype
			else
				RetCode = SlangEvalError( SLANG_ERROR_PARMS, "%s: Arg %d (%s) expected %s found %s",
						Root->StringValue,
						ArgNum,
						ArgList[ ArgNum ].Name,
						(*ArgList[ ArgNum ].DataType)->Name,
						Rets[ ArgNum ].Data.Value.DataType->Name );
		}
		else
			continue;

FreeArgsAndExit:
		for( ; ArgNum >= 0; ArgNum-- )
			SLANG_RET_FREE( &Rets[ ArgNum ] );
		return RetCode;
	}


/*
**	Do Each() expansion
*/

	if( Eachiness != -1 )
	{
		RetCode = SlangApplyEach( Root, EvalFlag, Ret, Scope, EachArray );
		free( EachArray );
		for( ArgNum = 0; ArgNum < Root->Argc - HasBlock; ArgNum++ )
			SLANG_RET_FREE( &Rets[ ArgNum ] );
		return RetCode | SLANG_RC_ARG_PARSE_RET;
	}


/*
**	Fill in omitted args with nulls
*/

	for( ; ArgNum < ArgCount; ArgNum++ )
	{
		Rets[ ArgNum ].Type = SLANG_TYPE_STATIC_VALUE;
		DTM_INIT( &Rets[ ArgNum ].Data.Value );
	}
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangArgError
**	Returns: result of SlangError (SLANG_ERROR)
**	Action : Format argument list into an error message
**	Summary: Error function used by SLANG_ARG_PARSE
****************************************************************/

int SlangArgError(
	SLANG_NODE				*Root,		// Root node passed to SlangX function
	SLANG_EVAL_FLAG			EvalFlag,	// Eval flag passed to SlangX function
	SLANG_RET				*Ret,		// Ret structure passed to SlangX function
	SLANG_SCOPE				*Scope,		// Scope passed to SlangX function
	const SLANG_ARG_LIST	*ArgList,	// SLANG_ARG_LIST for function
	int						ArgCount	// SLANG_ARG_COUNT macro
)
{
	int		ArgNum,
			Flags,
			Len,
			MaxLenArg		= 0,
			MaxLenDataType	= 0,
			HasBlock;

	SLANG_RET_CODE
			RetCode;


	HasBlock = ArgCount > 0 && ArgList[ ArgCount - 1 ].Flags == SLANG_ARG_BLOCK;
	if( HasBlock )
		ArgCount--;

	Err( SDB_ERR_SLANG_EVAL, "%s( ", Root->StringValue );
	for( ArgNum = 0; ArgNum < ArgCount; ArgNum++ )
	{
		ErrAppend( "%s", ArgList[ ArgNum ].Name );
		if( ArgNum != ArgCount - 1 )
			ErrAppend( ", " );
		if( (Len = strlen( ArgList[ ArgNum ].Name )) > MaxLenArg )
			MaxLenArg = Len;
		if( !ArgList[ ArgNum ].DataType )
			Len = 1;
		else
			Len = strlen( (*ArgList[ ArgNum ].DataType)->Name );
		if( Len > MaxLenDataType )
			MaxLenDataType = Len;
	}
	ErrAppend( " )" );

	for( ArgNum = 0; ArgNum < ArgCount; ArgNum++ )
	{
		Flags = ArgList[ ArgNum ].Flags;
		ErrAppend( "\n// (%-*.*s) %-*.*s",
				 MaxLenDataType,
				 MaxLenDataType,
				 ArgList[ ArgNum ].DataType ? (*ArgList[ ArgNum ].DataType)->Name : "*",
				 MaxLenArg,
				 MaxLenArg,
				 ArgList[ ArgNum ].Name );

		if( ArgList[ ArgNum ].Description )
			ErrAppend( " - %s", ArgList[ ArgNum ].Description );
		if( Flags & ( SLANG_ARG_NULLS | SLANG_ARG_OPTIONAL ))
		{
			if( !( Flags & SLANG_ARG_OPTIONAL ))
				ErrAppend( " - (Null)" );
			else if( !( Flags & SLANG_ARG_NULLS ))
				ErrAppend( " - (Optional)" );
			else
				ErrAppend( " - (Optional/Null)" );
		}
		if( Flags & SLANG_ARG_VAR_FLAG )
			ErrAppend( " - (Variable)" );
		else if( Flags & SLANG_ARG_LVALUE )
			ErrAppend( " - (LValue)" );
	}

	if( HasBlock )
		ErrAppend( "\n{\n\t/* %s */\n\t%s;\n};", ArgList[ ArgCount ].Description, ArgList[ ArgCount ].Name );

	// Err string is already setup correctly, do not change it now
	RetCode = SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );

	// Sleazily whack ErrNum since SlangEvalError() has no API for it
	ErrNum = SDB_ERR_SLANG_ARG;

	return RetCode;
}



/****************************************************************
**	Routine: SlangScopeCreate
**	Returns: Allocated SLANG_SCOPE
**	Summary: Create a new scope
**	Action : Create a new SLANG_SCOPE.
**	SeeAlso: SLANG_SCOPE, SlangScopeDestroy
****************************************************************/

SLANG_SCOPE *SlangScopeCreate( void )
{
	SLANG_SCOPE
			*Scope;


	Scope = (SLANG_SCOPE *) calloc( 1, sizeof( SLANG_SCOPE ));

	if( !SlangScopePushVariableHash( Scope, "(Main)", NULL, NULL, SlangVariableHash ) )
	{
		free( Scope );
		return NULL;
	}

	Scope->Diddles	= (SLANG_SCOPE_DIDDLES *) calloc( 1, sizeof( SLANG_SCOPE_DIDDLES ));
	Scope->Diddles->DiddleDepth = Scope->DiddleDepth = 0;

	return( Scope );
}



/****************************************************************
**	Routine: SlangScopeDestroy
**	Returns: None
**	Summary: Release SLANG_SCOPE memory
**	Action : Cleanup the scope contents: Diddles, Variables,
**			 and Functions
****************************************************************/

void SlangScopeDestroy(
	SLANG_SCOPE	*Scope
)
{
	if( !Scope )
		return;

	while( Scope->Diddles )
		SlangScopePopDiddles( Scope, TRUE );

	while( Scope->FunctionStack )
		SlangScopePopFunction( Scope );

	free( Scope );
}



/****************************************************************
**	Routine: DiddleHashFunc
**	Returns: hash value
**	Action : hashes a slang diddle
****************************************************************/

static HASH_BUCKET DiddleHashFunc(
	HASH_KEY	Key
)
{
	SLANG_SCOPE_DIDDLE
			*Diddle = (SLANG_SCOPE_DIDDLE *) Key;

	DT_HASH_CODE
			Hash1 = (DT_HASH_CODE) (intptr_t) Diddle->SecPtr,
			Hash2 = (DT_HASH_CODE) (intptr_t) Diddle->ValueType,
			Hash3 = (DT_HASH_CODE) Diddle->Argc;

	DT_VALUE
			Result;

	DT_HASH_MIX( Hash1, Hash2, Hash3 );
	Result.Data.Pointer = &Hash3;
	for( int i = 0; i < Diddle->Argc; ++i )
	{
		if( !DT_OP( DT_MSG_HASH_QUICK, &Result, Diddle->Argv[ i ], NULL ))
			DtHashPortable( Diddle->Argv[ i ], &Hash3 );
	}
	return Hash3;
}



/****************************************************************
**	Routine: DiddleCmpFunc
**	Returns: 0, !0
**	Action : compares two diddles
****************************************************************/

static int DiddleCmpFunc(
	HASH_KEY	Key1,
	HASH_KEY	Key2
)
{
	SLANG_SCOPE_DIDDLE
			*Diddle1 = (SLANG_SCOPE_DIDDLE *) Key1,
			*Diddle2 = (SLANG_SCOPE_DIDDLE *) Key2;

	DT_VALUE
			Equal;

	if( Diddle1->SecPtr != Diddle2->SecPtr || Diddle1->ValueType != Diddle2->ValueType || Diddle1->Argc != Diddle2->Argc )
		return 1;

	for( int i = 0; i < Diddle1->Argc; ++i )
		if( !DT_OP( DT_MSG_EQUAL, &Equal, Diddle1->Argv[ i ], Diddle2->Argv[ i ]) || !Equal.Data.Number )
			return 1;
	return 0;
}



/****************************************************************
**	Routine: SlangScopeDiddlesNew
**	Returns: SLANG_SCOPE_DIDDLES *
**	Action : allocates new slang diddle scope
****************************************************************/

SLANG_SCOPE_DIDDLES *SlangScopeDiddlesNew(
    const char* sHashName // optional, defualt don't create hash
)
{
	SLANG_SCOPE_DIDDLES
			*ScopeDiddles = (SLANG_SCOPE_DIDDLES *) malloc( sizeof( SLANG_SCOPE_DIDDLES ));

	ScopeDiddles->Next = NULL;
    if ( sHashName )
    {
        ScopeDiddles->DiddleHash = HashCreate( sHashName, DiddleHashFunc, DiddleCmpFunc, 0, NULL );

    }
    else
    {
	    ScopeDiddles->DiddleHash = NULL;
    }
	ScopeDiddles->DiddleDepth = 0;
	return ScopeDiddles;
}



/****************************************************************
**	Routine: SlangScopeDiddlesDelete
**	Returns: true/Err
**	Action : Deletes a Slang Diddle Scope
****************************************************************/

bool SlangScopeDiddlesDelete(
	SLANG_SCOPE_DIDDLES	*ScopeDiddles
)
{
	bool Ret = true;

	if( ScopeDiddles->DiddleHash )
	{
		Ret = (bool) SlangScopeFreeDiddles( ScopeDiddles, NULL, NULL, 0, NULL );
		HashDestroy( ScopeDiddles->DiddleHash );
	}
	free( ScopeDiddles );
	return Ret;
}



/****************************************************************
**	Routine: SlangScopePushDiddles
**	Returns: None
**	Action : Push a new layer of scope diddles
**			 FIX - Nested sticky diddles don't work (and should
**			 corrupt things)
****************************************************************/

void SlangScopePushDiddles(
	SLANG_SCOPE			*Scope,
	SLANG_SCOPE_DIDDLES	*ScopeDiddles
)
{
	if( ScopeDiddles )
	{
		ScopeDiddles->Next = Scope->Diddles;
		ScopeDiddles->DiddleDepth = ++Scope->DiddleDepth;
		Scope->Diddles = ScopeDiddles;
	}
	else
		++Scope->DiddleDepth;
}



/****************************************************************
**	Routine: SlangScopePopDiddles
**	Returns: TRUE	- Something was freed
**			 FALSE	- Nothing was freed
**	Action : Pop and free the scope diddles
****************************************************************/

void SlangScopePopDiddles(
	SLANG_SCOPE	*Scope,
	int			FreeDiddles
)
{
	SLANG_SCOPE_DIDDLES
			*ScopeDiddles = Scope->Diddles;

	if( !ScopeDiddles )
		return;

	if( Scope->Diddles->DiddleDepth == Scope->DiddleDepth )
	{
		ERR_OFF();
		Scope->DiddleDepth = ScopeDiddles->DiddleDepth - 1;
		Scope->Diddles = ScopeDiddles->Next;
		if( FreeDiddles )
			SlangScopeDiddlesDelete( ScopeDiddles );
		else
			ScopeDiddles->Next = NULL;
		ERR_ON();
	}
	else
		--Scope->DiddleDepth;
}



/****************************************************************
**	Routine: SlangScopeFreeDiddles
**	Returns: TRUE	- Something was freed
**			 FALSE	- Nothing was freed
**	Action : Free the scope diddles
****************************************************************/

int SlangScopeFreeDiddles(
	SLANG_SCOPE_DIDDLES	*ScopeDiddles,	// Pointer to scope diddles
	SDB_OBJECT			*SecPtr,		// NULL to free all diddles
	SDB_VALUE_TYPE		ValueType,		// NULL to free all diddles
	int					Argc,
	DT_VALUE			**Argv			// further identify it.
)
{
	int		RetVal = TRUE,
			i;

	DT_VALUE
			Equal;

	HASH_ENTRY_PTR
			HashPtr;

	int		Token = 0;


	if( !ScopeDiddles->DiddleHash )
		return FALSE;					// no diddles in this scope

/*
**	Find diddles (defined by SecPtr x ValueType x Argc x Argv )
**	A null value for SecPtr, ValueType, or Argc is a wildcard
*/

	SDB_NODE::Destroy( NULL, &Token );
	HASH_FOR( HashPtr, ScopeDiddles->DiddleHash )
	{
		SLANG_SCOPE_DIDDLE
				*ScopeDiddle = (SLANG_SCOPE_DIDDLE *) HashPtr->Key;

		if(( SecPtr && ( ScopeDiddle->SecPtr != SecPtr ))
				|| ( ValueType && ( ScopeDiddle->ValueType != ValueType ))
				|| ( Argc && ( ScopeDiddle->Argc != Argc )))
			continue;					// no match

		for( i = 0; i < Argc; i++ )
			if( !DT_OP( DT_MSG_EQUAL, &Equal, Argv[ i ], ScopeDiddle->Argv[ i ])  || !Equal.Data.Number )
				break;					// no match
		if( i < Argc )
			continue;					// no match

		/*
		** we found a match. Delete this from the hash
		*/

		HashDeleteEntry( ScopeDiddles->DiddleHash, HashPtr );

		if( !g_DiddleScopesFixEnabled || ScopeDiddle->SecPtr ) 
		{
			RetVal = SecDbRemoveDiddleWithArgs(	ScopeDiddle->SecPtr, ScopeDiddle->ValueType,
								ScopeDiddle->Argc, ScopeDiddle->Argv,
								ScopeDiddle->DiddleID ) && RetVal;

			if( g_DiddleScopesFixEnabled ) 
			{
				slangScopeDiddleList *ds = (slangScopeDiddleList *) HashLookup( g_ValidDiddleScopes, (HASH_KEY) ScopeDiddle->SecPtr );
				if( ds ) 
				{
					ds->remove( ScopeDiddle );
					if( ds->size() == 0 )
					{
						HashDelete( g_ValidDiddleScopes, (HASH_KEY) ScopeDiddle->SecPtr );
						delete ds;
					}
				} 
			}
		}
		for( i = 0; i < ScopeDiddle->Argc; ++i )
		{
			DTM_FREE( ScopeDiddle->Argv[ i ]);
			free( ScopeDiddle->Argv[ i ]);
		}
		free( ScopeDiddle->Argv );
		free( ScopeDiddle );
	}
	SDB_NODE::Destroy( NULL, &Token );

	return( RetVal );
}



/****************************************************************
**	Routine: SlangScopeSetDiddleWithFlags
**	Returns: DiddleID/DIDDLE_ID_ERROR
**			 If not error takes SecPtr from caller
**			 else does not take SecPtr
**	Action : Set/change diddle and keep track of it in given scope
****************************************************************/

SDB_DIDDLE_ID SlangScopeSetDiddleWithFlags(
	SLANG_SCOPE			*Scope,			// Pointer to scope to assign
	SDB_OBJECT			*SecPtr,
	SDB_VALUE_TYPE		ValueType,
	int					Argc,
	DT_VALUE			**Argv,			// note: we don't own this
	DT_VALUE			*Value, 		// nor this
	SDB_DIDDLE_ID		DiddleID,		// DiddleID
	int					Flags			// SetDiddle flags
)
{

	// make sure we are diddling in the right scope
	if( Scope->Diddles->DiddleDepth != Scope->DiddleDepth )
	{
		SLANG_SCOPE_DIDDLES
				*Diddles = SlangScopeDiddlesNew();
		Diddles->DiddleDepth = Scope->DiddleDepth;
		Diddles->Next = Scope->Diddles;
		Scope->Diddles = Diddles;
	}

	// make sure the diddle hash exists
	if( !Scope->Diddles->DiddleHash )
		Scope->Diddles->DiddleHash = HashCreate( "Slang Scope Diddles", DiddleHashFunc, DiddleCmpFunc, 0, NULL );

	return ( SlangDiddlesSetDiddleWithFlags( Scope->Diddles, SecPtr, ValueType, Argc, Argv, Value, DiddleID, Flags, FALSE ) );
}

/****************************************************************
**	Routine: SlangUpdateScopeDiddles
**	Returns: 
**			 1, if successfully updated all diddle scopes referencing the
**           SecPtr.
**			 0, otherwise
**  Action:  
****************************************************************/

int SlangUpdateScopeDiddles( 
    SDB_OBJECT *SecPtr 
) 
{
	if( !g_DiddleScopesFixEnabled )
		return 0;
    
	slangScopeDiddleList *list = (slangScopeDiddleList *) HashDelete( g_ValidDiddleScopes, (HASH_KEY) SecPtr );
	if (list)
	{
		for( slangScopeDiddleList::iterator it = list->begin(); it != list->end(); it++) {
			(*it)->SecPtr = 0;
		}
		list->clear();
		delete list;
		return 1;
	} 
	return 0;
}

/****************************************************************
**	Routine: SlangDiddlesSetDiddleWithFlags
**	Returns: DiddleID/DIDDLE_ID_ERROR
**			 If not error takes SecPtr from caller
**			 else does not take SecPtr
**	Action : Set/change diddle and keep track of it in given SLANG_SCOPE_DIDDLES
****************************************************************/

SDB_DIDDLE_ID SlangDiddlesSetDiddleWithFlags(
	SLANG_SCOPE_DIDDLES	*Diddles,			// contains diddles so far
	SDB_OBJECT			*SecPtr,
	SDB_VALUE_TYPE		ValueType,
	int					Argc,
	DT_VALUE			**Argv,			// note: we don't own this
	DT_VALUE			*Value, 		// nor this
	SDB_DIDDLE_ID		DiddleID,		// DiddleID
	int					Flags,			// SetDiddle flags
	int					bChangeOnly		// Change only if exists
)
{
	SLANG_SCOPE_DIDDLE
			Diddle,
			*ScopeDiddle;

	int		i,
			Status;


	// Lookup diddle: ValueType( SecPtr, Argv )

	Diddle.SecPtr = SecPtr;
	Diddle.ValueType = ValueType;
	Diddle.Argc = Argc;
	Diddle.Argv = Argv;

	// if found, then change it

	if( ( ScopeDiddle = (SLANG_SCOPE_DIDDLE *) HashLookup( Diddles->DiddleHash, (HASH_KEY) &Diddle )) )
		return SecDbChangeDiddleWithArgs( SecPtr, ValueType, Argc, Argv, Value, Flags, ScopeDiddle->DiddleID )
				? ScopeDiddle->DiddleID : SDB_DIDDLE_ID_ERROR;
	else if ( bChangeOnly ) // if chanage only if exits is requested, then don't try to create diddle
		return SDB_DIDDLE_ID_ERROR;
	// else allocate a new one

	DiddleID = SecDbSetDiddleWithArgs( SecPtr, ValueType, Argc, Argv, Value, Flags, DiddleID );
	Status = DiddleID != SDB_DIDDLE_ID_ERROR;
	Status = Status && ERR_CALLOC( ScopeDiddle, SLANG_SCOPE_DIDDLE, 1, sizeof( SLANG_SCOPE_DIDDLE ));
	Status = Status && ERR_CALLOC( ScopeDiddle->Argv, DT_VALUE*, Argc, sizeof( DT_VALUE* ));
	for( i = 0; Status && i < Argc; i++ )
	{
		Status = ERR_MALLOC( ScopeDiddle->Argv[ i ], DT_VALUE, sizeof( DT_VALUE ));
		Status = Status && DTM_ASSIGN( ScopeDiddle->Argv[ i ], Argv[ i ] );
	}
	if( !Status )
	{
		if( ScopeDiddle )
		{
			for( i = 0; i < Argc; i++ )
				free( ScopeDiddle->Argv[ i ] );
			free( ScopeDiddle->Argv );
			free( ScopeDiddle );
		}
		return SDB_DIDDLE_ID_ERROR;
	}

	ScopeDiddle->Argc		= Argc;
	ScopeDiddle->SecPtr		= SecPtr;
	ScopeDiddle->ValueType	= ValueType;
	ScopeDiddle->DiddleID	= DiddleID;

	HashInsert( Diddles->DiddleHash, (HASH_KEY) ScopeDiddle, (HASH_VALUE) ScopeDiddle );

	if( g_DiddleScopesFixEnabled )
	{
		slangScopeDiddleList *list = (slangScopeDiddleList *) HashLookup( g_ValidDiddleScopes, (HASH_KEY) SecPtr );
		if( !list ) 
		{
			list = new slangScopeDiddleList;
			HashInsert( g_ValidDiddleScopes, (HASH_KEY) SecPtr, (HASH_VALUE) list );
		}
		list->push_back( ScopeDiddle );
	}
	return DiddleID;
}




/****************************************************************
**	Routine: SlangScopeSetDiddle
**	Returns: TRUE	- Diddle was set ( takes SecPtr from caller )
**			 FALSE	- Diddle was not set ( SecPtr not taken )
**	Action : Set/change diddle and keep track of it in given scope
****************************************************************/

int SlangScopeSetDiddle(
	SLANG_SCOPE			*Scope,			// Pointer to scope to assign
	SDB_OBJECT			*SecPtr,
	SDB_VALUE_TYPE		ValueType,
	int					Argc,
	DT_VALUE			**Argv,			// note: we don't own this
	DT_VALUE			*Value			// nor this
)
{
	return SlangScopeSetDiddleWithFlags( Scope, SecPtr, ValueType, Argc, Argv, Value, SDB_DIDDLE_ID_NEW, 0 ) != SDB_DIDDLE_ID_ERROR;
}



/****************************************************************
**	Routine: SlangScopePushLocals
**	Returns: TRUE/FALSE
**	Action : Pushes a function with scoped local variables
****************************************************************/

int SlangScopePushLocals(
	SLANG_SCOPE			*Scope,
	const char			*FunctionName,
	DT_SLANG			*Function,
	SLANG_ERROR_INFO	*CallerErrorInfo,
	SLANG_VARIABLE		**Locals
)
{
	if( SlangFunctionDepth >= SLANG_MAX_NESTING_DEPTH )
	{
		if( !Scope->FunctionStack )
			return Err( ERR_OVERFLOW, "Slang function stack @ <unknown_caller>" );

		SLANG_ERROR_INFO *ErrorInfo = Scope->FunctionStack->ErrorInfo;

		return Err( ERR_OVERFLOW, "Slang function stack @ in %s/%s line %d", 
					ErrorInfo->ModuleName, 
					Scope->FunctionStack->FunctionName,
					ErrorInfo->BeginningLine );
	}

	SLANG_SCOPE_FUNCTION
			*Func = SlangScopeFunctionAlloc();

	if( !Func )
		return FALSE;

	SlangFunctionDepth++;

	if( Scope->FunctionStack && CallerErrorInfo )
		Scope->FunctionStack->ErrorInfo = CallerErrorInfo;

	Func->Next			= Scope->FunctionStack;
	Func->FunctionName	= FunctionName;
	Func->SlangFunc		= Function;

	if( Locals )
		Func->Locals	= Locals;
	else if( Function->Locals->KeyCount )
		Func->Locals	= (SLANG_VARIABLE **) calloc( sizeof( SLANG_VARIABLE * ), Function->Locals->KeyCount );
	else
		Func->Locals	= NULL;

	Func->VariableHash	= NULL;
	Func->ErrorInfo		= &Function->Expression->ErrorInfo;
	Func->UninitializedVarHook = NULL;

	Scope->FunctionStack= Func;

	return TRUE;
}



/****************************************************************
**	Routine: SlangScopePushVariableHash
**	Returns: TRUE/FALSE
**	Action : Push a new function along with a hash table for
**			 variables.
****************************************************************/

int SlangScopePushVariableHash(
	SLANG_SCOPE			*Scope,
	const char			*FunctionName,
	SLANG_NODE			*Root,
	SLANG_ERROR_INFO	*CallerErrorInfo,
	HASH				*VariableHash
)
{
	if( SlangFunctionDepth >= SLANG_MAX_NESTING_DEPTH )
	{
		if( !Scope->FunctionStack )
			return Err( ERR_OVERFLOW, "Slang function stack @ <unknown_caller>" );

		SLANG_ERROR_INFO *ErrorInfo = Scope->FunctionStack->ErrorInfo;

		return Err( ERR_OVERFLOW, "Slang function stack @ in %s/%s line %d", 
					ErrorInfo->ModuleName, 
					Scope->FunctionStack->FunctionName,
					ErrorInfo->BeginningLine );
	}

	SLANG_SCOPE_FUNCTION
			*Func = SlangScopeFunctionAlloc();

	if( !Func )
		return FALSE;

	SlangFunctionDepth++;

//	// Convert current scope to hash
//	SlangScopeLocalsToHash( Scope->FunctionStack );

	if( !VariableHash )
		VariableHash = HashCreate( FunctionName, (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );

	if( Scope->FunctionStack && CallerErrorInfo )
		Scope->FunctionStack->ErrorInfo = CallerErrorInfo;

	Func->Next			= Scope->FunctionStack;
	Func->FunctionName	= FunctionName;
	Func->SlangFunc		= (DT_SLANG *) malloc( sizeof( *Func->SlangFunc ));
	Func->Locals		= NULL;
	Func->VariableHash	= VariableHash;
	Func->ErrorInfo		= Root ? &Root->ErrorInfo : NULL;
	Func->UninitializedVarHook = NULL;

	Scope->FunctionStack= Func;

	memset( Func->SlangFunc, 0, sizeof( *Func->SlangFunc ));
	Func->SlangFunc->Expression = Root;

	return TRUE;
}



/****************************************************************
**	Routine: SlangScopePopFunction
**	Returns: TRUE	- Scope popped
**			 FALSE	- No scope to pop
**	Action : Pop a function scope
****************************************************************/

int SlangScopePopFunction(
	SLANG_SCOPE	*Scope,
	bool		FreeLocals
)
{
	SLANG_SCOPE_FUNCTION
			*Func = Scope->FunctionStack;

	if( Func )
	{
		Scope->FunctionStack = Func->Next;
		--SlangFunctionDepth;

		if( Func->VariableHash && Func->Next && Func->Next->VariableHash != Func->VariableHash && Func->VariableHash != SlangVariableHash )
			SlangVariableHashDestroy( Func->VariableHash );
		else if( Func->Locals )
		{
			for( int Num = Func->SlangFunc->Locals->KeyCount - 1; Num >= 0; --Num )
				SlangVariableFree( Func->Locals[ Num ]);
			if( FreeLocals )
				free( Func->Locals );
		}
		// If Func has no locals, then we alloc'd it, so free it
		if( !Func->SlangFunc->Locals )
			free( Func->SlangFunc );

		if( Func->UninitializedVarHook )
		{
			DT_VALUE
					TmpValue;

			TmpValue.DataType = DtSlang;
			TmpValue.Data.Pointer = Func->UninitializedVarHook;
			DTM_FREE( &TmpValue );
		}
		SlangScopeFunctionFree( Func );
	}
	return TRUE;
}



/****************************************************************
**	Routine: SlangModuleNameFromType
**	Returns: Module type in string form
**	Summary: Convert module type to string
**	Action : Convert a module type into a module name string
****************************************************************/

const char *SlangModuleNameFromType(
	int		ModuleType
)
{
	static const char
			*ModuleName[] =
			{
				"Unknown",
				"File",
				"Object",
				"String",
				"Keyboard"
			};


	if( ModuleType >= SLANG_MODULE_USER )
		return "Application Defined";

	return( ModuleName[ ModuleType ] );
}



/****************************************************************
**	Routine: SlangInitializeNodes
**	Returns: None
**	Action : Build two hash tables:
**				The first is NodeName by Type
**				The second is NodeType by Name
****************************************************************/

static void SlangInitializeNodes( void )
{
#define	ADD_NODES_TO_HASH_TABLES(m_Name,PubName)	\
		HashInsert( SlangNodesByName, (HASH_KEY) #m_Name, (HASH_VALUE) (m_Name));	\
		HashInsert( SlangNodesByType, (HASH_KEY) (m_Name), (HASH_VALUE) PubName );


	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	SlangNodesByName	= HashCreate( "Slang Nodes By Name", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 100, NULL );
	SlangNodesByType	= HashCreate( "Slang Nodes By Type", NULL, NULL, 100, NULL );

	ADD_NODES_TO_HASH_TABLES( SLANG_VALUE_TYPE			, "VALUE_TYPE" )
	ADD_NODES_TO_HASH_TABLES( SLANG_VALUE_TYPE_FUNC		, "VALUE_TYPE_FUNC" )

	ADD_NODES_TO_HASH_TABLES( SLANG_ASSIGN				, "=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_ADD_ASSIGN			, "+=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_SUBTRACT_ASSIGN		, "-=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_MULTIPLY_ASSIGN		, "*=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_DIVIDE_ASSIGN		, "/=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_COLON_ASSIGN		, ":=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_ADD					, "+" )
	ADD_NODES_TO_HASH_TABLES( SLANG_SUBTRACT			, "-" )
	ADD_NODES_TO_HASH_TABLES( SLANG_MULTIPLY			, "*" )
	ADD_NODES_TO_HASH_TABLES( SLANG_DIVIDE				, "/" )
	ADD_NODES_TO_HASH_TABLES( SLANG_UNARY_MINUS			, "UNARY_MINUS" )
	ADD_NODES_TO_HASH_TABLES( SLANG_EQUAL				, "==" )
	ADD_NODES_TO_HASH_TABLES( SLANG_NOT_EQUAL			, "!=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_GREATER_THAN		, ">" )
	ADD_NODES_TO_HASH_TABLES( SLANG_LESS_THAN			, "<" )
	ADD_NODES_TO_HASH_TABLES( SLANG_GREATER_OR_EQUAL	, ">=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_LESS_OR_EQUAL		, "<=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_LOGICAL_AND			, "&&" )
	ADD_NODES_TO_HASH_TABLES( SLANG_LOGICAL_OR			, "||" )
	ADD_NODES_TO_HASH_TABLES( SLANG_UNARY_NOT			, "!" )
	ADD_NODES_TO_HASH_TABLES( SLANG_ADDRESS_OF			, "&" )
	ADD_NODES_TO_HASH_TABLES( SLANG_AND_ASSIGN			, "&=" )
	ADD_NODES_TO_HASH_TABLES( SLANG_POST_INCREMENT		, "()++" )
	ADD_NODES_TO_HASH_TABLES( SLANG_POST_DECREMENT		, "()--" )
	ADD_NODES_TO_HASH_TABLES( SLANG_PRE_INCREMENT		, "++()" )
	ADD_NODES_TO_HASH_TABLES( SLANG_PRE_DECREMENT		, "--()" )
	ADD_NODES_TO_HASH_TABLES( SLANG_COMPARE				, "<=>" )
	ADD_NODES_TO_HASH_TABLES( SLANG_SCOPE_OPERATOR		, "::" )
	ADD_NODES_TO_HASH_TABLES( SLANG_POUND				, "#" )

	ADD_NODES_TO_HASH_TABLES( SLANG_COMPONENT			, "." )
	ADD_NODES_TO_HASH_TABLES( SLANG_BLOCK				, "{}" )
	ADD_NODES_TO_HASH_TABLES( SLANG_SUBSCRIPT			, "[]" )
	ADD_NODES_TO_HASH_TABLES( SLANG_WHILE				, "WHILE" )
	ADD_NODES_TO_HASH_TABLES( SLANG_ARRAY_INIT			, "ARRAY_INIT" )

	ADD_NODES_TO_HASH_TABLES( SLANG_RESERVED_FUNC		, "RESERVED_FUNC" )
	ADD_NODES_TO_HASH_TABLES( SLANG_BINARY_AT_OP		, ".@." )
	ADD_NODES_TO_HASH_TABLES( SLANG_AT_FUNC				, "@" )
	ADD_NODES_TO_HASH_TABLES( SLANG_DOUBLE_AT_FUNC 		, "@@" )
	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_DOLLAR         , "$" )
	ADD_NODES_TO_HASH_TABLES( SLANG_MEMBER_FUNC 		, "MEMBER_FUNC" )

	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_VALUE			, "VALUE" )
	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_CONSTANT		, "CONSTANT" )
	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_VARIABLE		, "VARIABLE" )
	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_LOCAL_VARIABLE	, "LOCAL_VARIABLE" )
	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_NULL			, "NULL" )
	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_STATIC_VALUE	, "STATIC_VALUE" )
	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_VARIABLE_INDEX	, "VARIABLE_INDEX" )

	ADD_NODES_TO_HASH_TABLES( SLANG_VALUE_POINTER		, "VALUE_POINTER" )

	ADD_NODES_TO_HASH_TABLES( SLANG_DATA_TYPE_CREATOR	, "DATA_TYPE_CREATOR" )

	ADD_NODES_TO_HASH_TABLES( SLANG_TYPE_EVAL_ONCE		, "EVAL_ONCE"	)

#undef ADD_NODES_TO_HASH_TABLES
}

#ifdef __unix
#   include <unistd.h>
#endif

void 
PrintMemoryInfo(
	void
)
{
#ifdef __unix
	char buf[128];
	fprintf( stderr, "VM Size in KB:\n" );	
	sprintf( buf, "/usr/bin/ps -o pid,vsz | grep %d | awk '{print $2}'", getpid() ); 
	system( buf );
#else
    fprintf( stderr, "Not implemented on this platform\n" );
#endif
}

/****************************************************************
**	Routine: ValidDiddleScopesStats
**	Returns: None
**	Action : Print out information about the ValidDiddleScopes Hash.
****************************************************************/

static void ValidDiddleScopesStats(
    void
)
{
	HASH_USABILITY 
		Usability;
    
	HASH_ENTRY_PTR ptr;
	HASH *t = g_ValidDiddleScopes;
	if (!t) 
		return;
    
	HashStat( t, &Usability, (void *) stderr, (HASH_PRINT_FUNC) fprintf );
	HASH_FOR( ptr, t )
	{
		SDB_OBJECT *SecPtr = (SDB_OBJECT *) ptr->Key;
		slangScopeDiddleList *list = (slangScopeDiddleList *) ptr->Value;
		fprintf( stderr, "SecPtr %p (%s) :  %d diddle scope(s)\n", SecPtr, SecPtr->SecData.Name, (int) list->size() );
	}
}


/****************************************************************
**	Routine: SlangInitialize
**	Returns: None
**	Action : Initialize the expression system
****************************************************************/

void SlangInitialize( void )
{
	// Initialize the context
	SlangContext = (SLANG_CONTEXT *) calloc( 1, sizeof( *SlangContext ));
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	SlangContext->Hash = HashCreate( "Slang Context", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, MemPoolCreate());

	// Initialize the sticky diddles
	SlangStickyDiddles = (SLANG_SCOPE_DIDDLES *) calloc( 1, sizeof( SLANG_SCOPE_DIDDLES ));

	// Create hash tables
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	SlangVarScopeHash		= HashCreate( "Slang Variable Scopes",	(HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 100, NULL );
	SlangFunctionHash		= HashCreate( "Slang Functions", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 1500, NULL );
	SlangConstantsHash		= HashCreate( "Slang Constants", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 1500, NULL );
	SlangEnumsHash			= HashCreate( "Slang Enums", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 20, NULL );
	SlangVariableHash		= SlangVariableScopeGet( "Global", TRUE )->VariableHash;
	SlangVariableHeap		= GSHeapCreate( "Slang Variable Heap", sizeof( SLANG_VARIABLE ), 200 );
	SlangScopeFunctionHeap	= GSHeapCreate( "Slang Scope Function Heap", sizeof( SLANG_SCOPE_FUNCTION ), 200 );
	SlangNodeTypeHash		= HashCreate( "Slang Node Types",		NULL,			NULL,			150,	NULL	);
	SlangRootHash	 		= HashCreate( "Slang MemPool Map",		NULL,			NULL,			0,		NULL	);

	// Hack to handle the Dangling SecPtrs in Diddle Scopes problem.
	
	const char *fix;
	if( ( fix = SecDbConfigurationGet( "SECDB_DIDDLE_SCOPES_FIX", NULL, NULL, NULL ) ) )
		g_DiddleScopesFixEnabled = atoi( fix );
    
	if( g_DiddleScopesFixEnabled )
	{
		g_ValidDiddleScopes = HashCreate( "Valid Diddle Scopes",    NULL,           NULL,           0,      NULL    );
		if( g_DiddleScopesFixEnabled == 2 )
			(void) atexit( ValidDiddleScopesStats );
	}
    
	// Build hash tables of node types
	SlangInitializeNodes( );

	// Register slang node types
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	SlangNodeTypeRegister( SLANG_ASSIGN				, NULL, NULL, (SLANG_FUNC) SlangXAssign		  	, SLANG_PARSE_TIME	);
	SlangNodeTypeRegister( SLANG_ADD_ASSIGN			, NULL, NULL, (SLANG_FUNC) SlangXAssign		   	, SLANG_PARSE_TIME	);
	SlangNodeTypeRegister( SLANG_SUBTRACT_ASSIGN	, NULL, NULL, (SLANG_FUNC) SlangXAssign		   	, SLANG_PARSE_TIME	);
	SlangNodeTypeRegister( SLANG_MULTIPLY_ASSIGN	, NULL, NULL, (SLANG_FUNC) SlangXAssign		   	, SLANG_PARSE_TIME	);
	SlangNodeTypeRegister( SLANG_DIVIDE_ASSIGN		, NULL, NULL, (SLANG_FUNC) SlangXAssign		   	, SLANG_PARSE_TIME	);
	SlangNodeTypeRegister( SLANG_AND_ASSIGN			, NULL, NULL, (SLANG_FUNC) SlangXAssign		   	, SLANG_PARSE_TIME	);
	SlangNodeTypeRegister( SLANG_ADD				, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE	);
	SlangNodeTypeRegister( SLANG_SUBTRACT			, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_MULTIPLY			, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_DIVIDE				, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_EQUAL				, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_NOT_EQUAL			, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_GREATER_THAN		, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_LESS_THAN			, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_GREATER_OR_EQUAL	, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_LESS_OR_EQUAL		, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_COMPARE			, NULL, NULL, (SLANG_FUNC) SlangXOperator  	   	, SLANG_EVAL_ONCE	);
	SlangNodeTypeRegister( SLANG_LOGICAL_AND		, NULL, NULL, (SLANG_FUNC) SlangXLogicalAnd	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_LOGICAL_OR			, NULL, NULL, (SLANG_FUNC) SlangXLogicalOr 	   	, SLANG_EVAL_ONCE 	);
	SlangNodeTypeRegister( SLANG_UNARY_NOT			, NULL, NULL, (SLANG_FUNC) SlangXUnaryNot  	   	, SLANG_EVAL_ONCE   );
	SlangNodeTypeRegister( SLANG_UNARY_MINUS		, NULL, NULL, (SLANG_FUNC) SlangXUnaryMinus	   	, SLANG_EVAL_ONCE   );
	SlangNodeTypeRegister( SLANG_PRE_INCREMENT		, NULL, NULL, (SLANG_FUNC) SlangXPreIncrement  	, 0					);
	SlangNodeTypeRegister( SLANG_PRE_DECREMENT		, NULL, NULL, (SLANG_FUNC) SlangXPreDecrement  	, 0					);
	SlangNodeTypeRegister( SLANG_POST_INCREMENT		, NULL, NULL, (SLANG_FUNC) SlangXPostIncrement 	, 0 				);
	SlangNodeTypeRegister( SLANG_POST_DECREMENT		, NULL, NULL, (SLANG_FUNC) SlangXPostDecrement 	, 0 				);
	SlangNodeTypeRegister( SLANG_SCOPE_OPERATOR		, NULL, NULL, (SLANG_FUNC) SlangXScopeOperator 	, 0 				);
	SlangNodeTypeRegister( SLANG_COLON_ASSIGN		, NULL, NULL, (SLANG_FUNC) SlangXColonAssign	, 0 				);

	SlangNodeTypeRegister( SLANG_BLOCK				, NULL, "{}", (SLANG_FUNC) SlangXBlock		 	, SLANG_CHECK_RETURN);
	SlangNodeTypeRegister( SLANG_WHILE				, NULL, NULL, (SLANG_FUNC) SlangXWhile		 	, 0   				);

	// ARRAY_INIT is Eval_once too, but since the parser handles that specially, it is of no purpose to set it here
	SlangNodeTypeRegister( SLANG_ARRAY_INIT			, NULL, NULL, (SLANG_FUNC) SlangXArrayInit	 	, 0   				);

	SlangNodeTypeRegister( SLANG_VALUE_TYPE			, NULL, NULL, (SLANG_FUNC) SlangXValueMethod 	, 0			   		);
	SlangNodeTypeRegister( SLANG_VALUE_TYPE_FUNC	, NULL, NULL, (SLANG_FUNC) SlangXValueMethodFunc, 0			   		);
	SlangNodeTypeRegister( SLANG_DATA_TYPE_CREATOR	, NULL, NULL, (SLANG_FUNC) SlangXDataTypeCreator, 0			   		);
	SlangNodeTypeRegister( SLANG_SUBSCRIPT			, NULL, NULL, (SLANG_FUNC) SlangXSubscript		, 0			   		);
	SlangNodeTypeRegister( SLANG_STAR				, NULL, NULL, (SLANG_FUNC) SlangXStar			, 0			   		);
	SlangNodeTypeRegister( SLANG_COMPONENT			, NULL, NULL, (SLANG_FUNC) SlangXComponent		, 0			   		);
	SlangNodeTypeRegister( SLANG_TYPE_VARIABLE		, NULL, NULL, (SLANG_FUNC) SlangXVariable		, 0			   		);

	SlangNodeTypeRegister( SLANG_TYPE_DOLLAR		, NULL, NULL, (SLANG_FUNC) SlangXDollar			, 0					);

	// Null should really be a constant, but unfortunately constants can only be doubles
	// so we have to make it an eval_once function which converts itself into a constant
	SlangNodeTypeRegister( SLANG_TYPE_NULL			, NULL, NULL, (SLANG_FUNC) SlangXNull			, SLANG_EVAL_ONCE	);

	// Constants are Eval_once by default
	SlangNodeTypeRegister( SLANG_TYPE_CONSTANT		, NULL, NULL, (SLANG_FUNC) SlangXConstantNode	, 0			   		);
	SlangNodeTypeRegister( SLANG_TYPE_VARIABLE_INDEX, NULL, NULL, (SLANG_FUNC) SlangXVariableIndex	, 0			   		);

	SlangNodeTypeRegister( SLANG_TYPE_EVAL_ONCE		, NULL, NULL, (SLANG_FUNC) SlangXEvalOnce		, 0					);

	SlangFunctionRegister( "Global"					, NULL, NULL, (SLANG_FUNC) SlangXGlobal			, 0	);
	SlangFunctionRegister( "DoGlobal"				, NULL, NULL, (SLANG_FUNC) SlangXDoGlobal		, SLANG_BIND_BLOCK | SLANG_NO_PARENS );
	SlangFunctionRegister( "Sticky"					, NULL, NULL, (SLANG_FUNC) SlangXSticky			, 0	);
	SlangFunctionRegister( "StickyDiddle"			, NULL, NULL, (SLANG_FUNC) SlangXStickyDiddle	, SLANG_BIND_BLOCK | SLANG_NO_PARENS );
	SlangFunctionRegister( "Null"					, NULL, NULL, (SLANG_FUNC) SlangXNull			, SLANG_NO_PARENS | SLANG_PARSE_TIME );
	SlangFunctionRegister( "While"					, NULL, NULL, (SLANG_FUNC) SlangXWhile			, SLANG_BIND_BLOCK );
	SlangFunctionRegister( "If"						, NULL, NULL, (SLANG_FUNC) SlangXIf				, SLANG_BIND_BLOCK | SLANG_BIND_COLON | SLANG_EVAL_ONCE | SLANG_CHECK_RETURN );

	SlangFunctionRegister( "Try"					, NULL, NULL, (SLANG_FUNC) SlangXTry			, SLANG_BIND_BLOCK | SLANG_BIND_COLON | SLANG_EVAL_ONCE | SLANG_CHECK_RETURN );
	SlangFunctionRegister( "Throw"					, NULL, NULL, (SLANG_FUNC) SlangXThrow			, SLANG_EVAL_ONCE );

	SlangFunctionRegister( "Link"					, NULL, NULL, (SLANG_FUNC) SlangXLink			, SLANG_PARSE_TIME	);
	SlangFunctionRegister( "SmartLinkEnable"		, NULL, NULL, (SLANG_FUNC) SlangXSmartLinkEnable, SLANG_PARSE_TIME	);
	SlangFunctionRegister( "LinkFile"				, NULL, NULL, (SLANG_FUNC) SlangXLinkFile		, SLANG_PARSE_TIME | SLANG_DEPRECATED	);
	SlangFunctionRegister( "LinkLog"				, NULL, NULL, (SLANG_FUNC) SlangXLinkLog		, 0	);
	SlangFunctionRegister( "Constant"				, NULL, NULL, (SLANG_FUNC) SlangXConstant		, SLANG_PARSE_TIME	);
	SlangFunctionRegister( "Enum"					, NULL, NULL, (SLANG_FUNC) SlangXEnum			, SLANG_PARSE_TIME | SLANG_NO_PARENS | SLANG_BIND_BLOCK );

	SlangFunctionRegister( "EvalOnce"  				, NULL, NULL, (SLANG_FUNC) SlangXEvalOnce		, SLANG_EVAL_ONCE | SLANG_PARSE_TIME );
	SlangFunctionRegister( "GsDt"					, NULL, NULL, (SLANG_FUNC) SlangXGsDt			, SLANG_DATA_TYPE_CONSTRUCTOR );
	SlangFunctionRegister( "GsDtCast"				, NULL, NULL, (SLANG_FUNC) SlangXGsDtCast	       	, 0 );
	SlangFunctionRegister( "GsDtToDtValue"				, NULL, NULL, (SLANG_FUNC) SlangXGsDtToDtValue	        , 0 );
	SlangFunctionRegister( "GsDtTypeOf"				, NULL, NULL, (SLANG_FUNC) SlangXGsDtTypeOf    	        , 0 );
	SlangFunctionRegister( "GsDtTypes"				, NULL, NULL, (SLANG_FUNC) SlangXGsDtTypes	        , 0 );
	SlangFunctionRegister( "DataTypeOf"				, NULL, NULL, (SLANG_FUNC) SlangXDataTypeOf	        , 0 );

/*
**	Register constants
*/

	SlangRegisterConstant( "False",			(double) FALSE					);
	SlangRegisterConstant( "True",			(double) TRUE					);

	SlangRegisterConstant( "Pi",			(double) 3.1415926535897932385 	);
	SlangRegisterConstant( "Error Value",	(double) HUGE		  			);

	SlangRegisterConstant( "Cipher",		(double) OUTFLAG_CIPHER			);
	SlangRegisterConstant( "Commas",		(double) OUTFLAG_COMMAS			);
	SlangRegisterConstant( "Concise",		(double) OUTFLAG_CONCISE		);
	SlangRegisterConstant( "Blank Zero",	(double) OUTFLAG_BLANK_ZERO		);
	SlangRegisterConstant( "Parens",		(double) OUTFLAG_PARENS			);
	SlangRegisterConstant( "Percent",		(double) OUTFLAG_PERCENT		);
	SlangRegisterConstant( "Scale",			(double) OUTFLAG_SCALE_ALL		);

	SlangRegisterConstant( "External",		(double) SDB_ENUM_EXTERNAL		);
	SlangRegisterConstant( "Leaves Only",	(double) SDB_ENUM_LEAVES_ONLY	);
	SlangRegisterConstant( "Prune",			(double) SDB_ENUM_PRUNE			);
	SlangRegisterConstant( "Sorted",		(double) SDB_ENUM_SORT			);
	SlangRegisterConstant( "Tree",			(double) SDB_ENUM_TREE			);

	SlangRegisterConstant( "Get Equal",  	(double) SDB_GET_EQUAL			);
	SlangRegisterConstant( "Get First", 	(double) SDB_GET_FIRST			);
	SlangRegisterConstant( "Get Ge",	 	(double) SDB_GET_GE				);
	SlangRegisterConstant( "Get Greater",	(double) SDB_GET_GREATER		);
	SlangRegisterConstant( "Get Last",		(double) SDB_GET_LAST			);
	SlangRegisterConstant( "Get Le",	 	(double) SDB_GET_LE				);
	SlangRegisterConstant( "Get Less",		(double) SDB_GET_LESS			);
	SlangRegisterConstant( "Get Next",		(double) SDB_GET_NEXT			);
	SlangRegisterConstant( "Get Prev",		(double) SDB_GET_PREV			);
	SlangRegisterConstant( "SDB_SEC_NAME_LEN",	SDB_SEC_NAME_SIZE - 1		);		//object name max length
	SlangRegisterConstant( "SDB_PATH_NAME_LEN",	SDB_PATH_NAME_SIZE - 1		);		//path name max length


/*
**	New form of constants
*/

	SlangRegisterConstant( "_Pi",			(double) 3.1415926535897932385	);
	SlangRegisterConstant( "_Error Value",	(double) HUGE		  			);

	SlangRegisterEnum( "FORMAT_FLAG", "_Cipher",		(double) OUTFLAG_CIPHER			);
	SlangRegisterEnum( "FORMAT_FLAG", "_Commas",		(double) OUTFLAG_COMMAS			);
	SlangRegisterEnum( "FORMAT_FLAG", "_Concise",		(double) OUTFLAG_CONCISE		);
	SlangRegisterEnum( "FORMAT_FLAG", "_Blank Zero",	(double) OUTFLAG_BLANK_ZERO		);
	SlangRegisterEnum( "FORMAT_FLAG", "_Parens",		(double) OUTFLAG_PARENS			);
	SlangRegisterEnum( "FORMAT_FLAG", "_Percent",		(double) OUTFLAG_PERCENT		);
	SlangRegisterEnum( "FORMAT_FLAG", "_Scale",			(double) OUTFLAG_SCALE_ALL		);
	SlangRegisterEnum( "FORMAT_FLAG", "_Pad Zeros",		(double) OUTFLAG_PAD_ZEROS		);
	SlangRegisterEnum( "FORMAT_FLAG", "_Plus Sign",		(double) OUTFLAG_PLUS			);
	SlangRegisterEnum( "FORMAT_FLAG", "_Trim Leading",	(double) OUTFLAG_TRIM_LEADING	);
	SlangRegisterEnum( "FORMAT_FLAG", "_Mush Compatible",	(double) OUTFLAG_MUSH_COMPATIBLE	);
	SlangRegisterEnum( "FORMAT_FLAG", "_Full Precision",	(double) OUTFLAG_FULL_PRECISION		);

	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "_External",	(double) SDB_ENUM_EXTERNAL		);
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "_First Level",	(double) SDB_ENUM_FIRST_LEVEL	);
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "_Leaves Only",	(double) SDB_ENUM_LEAVES_ONLY	);
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "_Prune",	   	(double) SDB_ENUM_PRUNE			);
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "_Sorted",		(double) SDB_ENUM_SORT			);
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "_Tree",		(double) SDB_ENUM_TREE			);
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "_Below Set",	(double) SDB_ENUM_BELOW_SET		);
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "SDB_ENUM_GET_VALUE", (double) SDB_ENUM_GET_VALUE );
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "SDB_ENUM_EXTERNAL_VT", (double) SDB_ENUM_EXTERNAL_VT );
	SlangRegisterEnum( "FOR_CHILDREN_FLAG", "_External Vt",	(double) SDB_ENUM_EXTERNAL_VT	);

	SlangRegisterEnum( "INDEX_GET_TYPE",	"_Equal",  		(double) SDB_GET_EQUAL			);
	SlangRegisterEnum( "INDEX_GET_TYPE",	"_First", 		(double) SDB_GET_FIRST			);
	SlangRegisterEnum( "INDEX_GET_TYPE",	"_Ge",	 		(double) SDB_GET_GE				);
	SlangRegisterEnum( "INDEX_GET_TYPE",	"_Greater",		(double) SDB_GET_GREATER		);
	SlangRegisterEnum( "INDEX_GET_TYPE",	"_Last",		(double) SDB_GET_LAST			);
	SlangRegisterEnum( "INDEX_GET_TYPE",	"_Le",	 		(double) SDB_GET_LE				);
	SlangRegisterEnum( "INDEX_GET_TYPE",	"_Less",		(double) SDB_GET_LESS			);
	SlangRegisterEnum( "INDEX_GET_TYPE",	"_Next",		(double) SDB_GET_NEXT			);
	SlangRegisterEnum( "INDEX_GET_TYPE",	"_Prev",		(double) SDB_GET_PREV			);

	SlangRegisterEnum( "TRIM_FLAG",	"_Leading",		(double) SLANG_TRIM_LEADING		);
	SlangRegisterEnum( "TRIM_FLAG",	"_Trailing",	(double) SLANG_TRIM_TRAILING	);

	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Extrapolate",				DT_EXTRAPOLATE			);
	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Interpolate Only",		DT_INTERPOLATE_ONLY		);
	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Interpolate Flat",		DT_INTERPOLATE_FLAT		);
	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Knots Only",				DT_KNOTS_ONLY			);
	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Interpolate Step",		DT_INTERPOLATE_STEP		);
	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Interpolate Prestep",		DT_INTERPOLATE_PRESTEP	);
	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Interpolate Log",			DT_INTERPOLATE_LOG		);
	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Interpolate Cubic",		DT_INTERPOLATE_CUBIC	);
	SlangRegisterEnum( "INTERPOLATE_METHOD",	"_Interpolate LogSmooth",	DT_INTERPOLATE_LOG_SMOOTH	);
	SlangRegisterEnum( "INTERPOLATE_METHOD", 	"_Knots Only Zap",			DT_KNOTS_ONLY_ZAP		);

	SlangRegisterEnum( "AVERAGE_METHOD",		"_Sum",						DT_SUM					);
	SlangRegisterEnum( "AVERAGE_METHOD",		"_Arithmetic Mean",			DT_ARITHMETIC_MEAN		);
	SlangRegisterEnum( "AVERAGE_METHOD",		"_Geometric Mean",			DT_GEOMETRIC_MEAN		);
	SlangRegisterEnum( "AVERAGE_METHOD",		"_Harmonic Mean",			DT_HARMONIC_MEAN		);
	SlangRegisterEnum( "AVERAGE_METHOD",		"_Quadratic Mean",			DT_QUADRATIC_MEAN		);
	SlangRegisterEnum( "AVERAGE_METHOD",		"_Weighted Mean",			DT_WEIGHTED_MEAN		);
	SlangRegisterEnum( "AVERAGE_METHOD",		"_Median",					DT_MEDIAN				);
	SlangRegisterEnum( "AVERAGE_METHOD",		"_Midrange",				DT_MIDRANGE				);
	
	SlangRegisterEnum( "SDB_DB_OPERATOR",		"_SDB_DB_LEAF",				SDB_DB_LEAF				);
	SlangRegisterEnum( "SDB_DB_OPERATOR",		"_SDB_DB_QUALIFIER",   		SDB_DB_QUALIFIER		);
	SlangRegisterEnum( "SDB_DB_OPERATOR",		"_SDB_DB_UNION",  			SDB_DB_UNION			);
	SlangRegisterEnum( "SDB_DB_OPERATOR",		"_SDB_DB_READONLY",			SDB_DB_READONLY			);
	SlangRegisterEnum( "SDB_DB_OPERATOR",		"_SDB_DB_ALIAS",			SDB_DB_ALIAS			);
	SlangRegisterEnum( "SDB_DB_OPERATOR",		"_SDB_DB_SUBDB",			SDB_DB_SUBDB			);       
	SlangRegisterEnum( "SDB_DB_OPERATOR",		"_SDB_DB_DIDDLE_SCOPE",		SDB_DB_DIDDLE_SCOPE		);

	// Set a startup time for specific session identification
	DT_VALUE StartupTime;
	DTM_ALLOC( &StartupTime, DtTime );
	StartupTime.Data.Number = time( NULL );
	SlangVariableSet( "Startup Time", &StartupTime, NULL, FALSE );
	DTM_FREE( &StartupTime );

	//atexit(PrintMemoryInfo);
}


/****************************************************************
**	Routine: SecDbSourceDbGet
**	Returns: SDB_DB *, NULL on error
**	Action : Return the db where Source objects (e.g. Slang
**			 expressions) live. Look for it on SECDB_SOURCE_DATABASE
**			 configuration variable
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbSourceDbGet"

SDB_DB *SecDbSourceDbGet()
{
	const char
			*SourceDbName;

	if( SourceDb )
		return SourceDb;

	ErrPush();
	SourceDbName = SecDbConfigurationGet( "SECDB_SOURCE_DATABASE", NULL, NULL, NULL );
	ErrPop();

	if( !SourceDbName )
		return SecDbRootDb;

	if( !( SourceDb = SecDbSourceDbSet( SourceDbName )))
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to set source db to '%s'", SourceDbName );

	return SourceDb;
}


/****************************************************************
**	Routine: SecDbSourceDbSet
**	Returns: SDB_DB *, NULL on error
**	Action : Set the db where Source objects (e.g. Slang
**			 expressions) live.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbSourceDbSet"

SDB_DB *SecDbSourceDbSet(
	const char *SourceDbName
)
{
	SDB_DB	*NewDb = NULL, // pacify compiler
			*UpdateDb;


	if( !( NewDb = SecDbAttach( SourceDbName, SDB_DEFAULT_DEADPOOL_SIZE, SDB_BACK )))
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach to source db '%s'", SourceDbName );

	UpdateDb = SecDbDbUpdateGet( NewDb, SecDbClassTypeFromName( "Slang Expression" ) );

/*
**	Don't want to allow people to writing to Source RW via SourceDatabase(). We will control access to Source RW.
*/
	if( UpdateDb && !stricmp( UpdateDb->MirrorGroup.c_str(), "Source RW" ) )
	{
		Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": '%s' is not a valid source database (contains Source RW)", NewDb->FullDbName.c_str() );
		ERR_OFF();
		SecDbDetach( NewDb );
		ERR_ON();
		return NULL;
	}

	if( SourceDb )
		SecDbDetach( SourceDb );

	return SourceDb = NewDb;
}
