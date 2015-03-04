#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_mem.c,v 1.29 2001/07/17 17:49:43 singhki Exp $"
/****************************************************************
**
**	SDB_MEM.C
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_mem.c,v $
**	Revision 1.29  2001/07/17 17:49:43  singhki
**	split children and parents into separate arrays in SDB_NODE. allows us
**	to build cycles because we can add parents while a node is
**	locked. code is much cleaner too.
**
**	Revision 1.28  2001/06/29 15:22:15  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.27  2001/03/14 18:33:24  singhki
**	Rewrite nodes to use STL style multiset instead of skip list
**	
**	Revision 1.26  2000/04/24 11:04:23  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.25  2000/02/08 04:50:25  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.24  2000/02/01 22:54:17  singhki
**	Replace ValueTypeMap in SecPtr with the SDB_CLASS_INFO for the class
**	from which we should get VTIs. Class still points to the original
**	SDB_CLASS_INFO to be used for the ObjectFunc
**	
**	Revision 1.23  1999/12/30 13:28:03  singhki
**	Add UninitializedVarHook
**	
**	Revision 1.22  1999/11/17 23:42:06  davidal
**	Added two new attributes for VT as follows:
**	- SDB_RELEASE_VALUE;
**	- SDB_RELEASE_VALUE_WHEN_PARENTS_VALID.
**	Once set for the VT, the former will cause an immediate release of the node's value after it gets consumed
**	by a parent; the latter will do the same after *all* the parents are valid.
**	By default, both the attributes are set to FALSE; however, defining the env variable
**	SECDB_RELEASE_VALUE_WHEN_PARENTS_VALID as TRUE will set SDB_RELEASE_VALUE_WHEN_PARENTS_VALID for all
**	non-terminal nodes.
**	
**	Revision 1.21  1999/10/20 19:51:56  singhki
**	Use DtValueHeap for slang variable values instead of private SlangVariableValueHeap
**	
**	Revision 1.20  1999/10/20 17:05:33  singhki
**	fix up memory usage some more for SDB_DBs
**	
**	Revision 1.19  1999/09/29 22:48:16  singhki
**	Fix memusage for diddle scopes
**	
**	Revision 1.18  1999/09/01 15:29:31  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.17  1999/08/10 22:15:14  singhki
**	check for virtual DBs
**	
**	Revision 1.16  1999/07/15 16:01:02  singhki
**	remove nasty linked list of open dbs, use DatabaseIDHash and OpenCount instead.
**	
**	Revision 1.15  1999/06/21 03:05:38  gribbg
**	More const correctness
**	
**	Revision 1.14  1999/06/15 23:02:23  singhki
**	output node memory usage by object for terminals
**	
**	Revision 1.13  1999/03/03 16:43:11  singhki
**	Slang Functions now use a local variable stack and their variables
**	are represented as an index into this stack.
**	Scoped variable nodes in the parse tree are replaced by references to
**	the variable itself.
**	Diddle Scopes inside a Slang Scope are now allocated on demand.
**	
**	Revision 1.12  1998/11/16 22:59:58  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.11.4.1  1998/11/05 02:53:15  singhki
**	factored c++ stuff into internal header files
**	
**	Revision 1.11  1998/08/12 20:11:59  singhki
**	Get rid of SecDbValueFlagsHash, use SDB_CLASS_INFO->ValueTypeMap to get the same data
**	
**	Revision 1.10  1998/01/02 21:33:03  rubenb
**	added GSCVSID
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdlib.h>
#include	<heapinfo.h>
#include	<mempool.h>
#include	<date.h>
#include	<hash.h>
#include	<dynalink.h>
#include	<kool_ade.h>
#include	<datatype.h>
#include	<secdb.h>
#include	<secdrv.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<sdb_int.h>
#include	<secindex.h>
#include	<secexpr.h>
#include	<sectrace.h>
#include	<secobj.h>


#define	SET_NUM( Results_, Tag_, Size_ )	DtComponentSetNumber( Results_, Tag_, DtDouble, (double) (Size_) )
#define	SET_HASH( Hash_ )					{ Size = HashMemSize( Hash_ ); Total += Size; SET_NUM( Results, #Hash_, Size ); }
#define MEM_POOL_MEM_SIZE( MemPool_ )		( GSHeapMemSize( MemPool_ ) + ( (MemPool_)->MallocCount + (MemPool_)->CallocCount - (MemPool_)->FreeCount ) * sizeof( MEM_POOL_ELEM ) + (MemPool_)->Size )
#define	CLASS_MESSAGE( Class_, Msg_, Data_)	( SecDbTraceFunc ? SecDbMessageTrace( NULL, (Msg_), (Data_) ) : (*(Class_)->FuncPtr)( NULL, (Msg_), (Data_) ))


extern HASH
		*SlangRootHash,
		*SlangFunctionHash,
		*SlangConstantsHash,
		*SlangEnumsHash,
		*SlangNodeTypeHash,
		*SlangNodesByType,
		*SlangNodesByName,
		*SlangVarScopeHash;

extern HEAP
		*SlangVariableHeap;

extern CONFIGURATION
		*SecDbConfiguration;



/****************************************************************
**	Routine: ObjectUsage
**	Returns: Total bytes used by Objects
**	Action :
****************************************************************/

static int ObjectUsage(
	DT_VALUE	*Results
)
{
	SDB_DB	*pDb;

	HASH_ENTRY_PTR
			HashPtr,
			HashPtr1;

	SDB_OBJECT
			*SecPtr;

	long	Size,
			SecPtrSize,
			VTMapSize,
			ObjectSize,
			SubTotal,
			Total = 0;

	DT_VALUE
			SubResults;


	if( !DTM_ALLOC( Results, DtStructure ) || !DTM_ALLOC( &SubResults, DtStructure ))
	{
		DTM_INIT( Results );
		return( 0 );
	}

	HASH_FOR( HashPtr1, SecDbDatabaseIDHash )
	{
		pDb = (SDB_DB *) HashValue( HashPtr1 );
		if( !pDb->OpenCount || pDb->DbPhysical != pDb )
			continue;

		SubTotal = 0;

		Size		= pDb->MemUsed;
		SubTotal	+= Size;
		SET_NUM( &SubResults, "Driver", Size );

		Size		= HashMemSize( pDb->CacheHash );
		SubTotal	+= Size;
		SET_NUM( &SubResults, "CacheHash", Size );

		Size		= HashMemSize( pDb->PreCacheHash );
		SubTotal	+= Size;
		SET_NUM( &SubResults, "PreCacheHash", Size );

		Size		= HashMemSize( pDb->PageTableHash );
		SubTotal	+= Size;
		SET_NUM( &SubResults, "PageTableHash", Size );

		Size		= GSHeapMemSize( pDb->DeadPool );
		SubTotal	+= Size;
		SET_NUM( &SubResults, "DeadPool", Size );

		SecPtrSize 	=
		VTMapSize	=
		ObjectSize	= 0;
		HASH_FOR( HashPtr, pDb->CacheHash )
		{
			SecPtr = (SDB_OBJECT *) HashValue( HashPtr );
			SecPtrSize += GSHeapMemSize( SecPtr );
			ObjectSize += GSHeapMemSize( SecPtr->Object );
		}
		SubTotal += SecPtrSize;
		SET_NUM( &SubResults, "SecPtr", SecPtrSize );
		SubTotal += ObjectSize;
		SET_NUM( &SubResults, "Object", ObjectSize );

		Total += SubTotal;
		SET_NUM( &SubResults, "~Total", SubTotal );
		DtComponentSet( Results, pDb->DbName, &SubResults );
	}
	DTM_FREE( &SubResults );
	SET_NUM( Results, "~Total", Total );
	return( Total );
}



/****************************************************************
**	Routine: ClassUsage
**	Returns: Total bytes used by Class data structures
**	Action :
****************************************************************/

static int ClassUsage(
	DT_VALUE	*Results
)
{
	SDB_ENUM_PTR
			EnumPtr;

	SDB_CLASS_INFO
			*ClassInfo;

	SDB_M_DATA
			MsgData;

	HASH_ENTRY_PTR
			HashPtr;

	long	Size,
			Total = 0;

	char	Tag[ SDB_CLASS_NAME_SIZE * 2 ];


	if( !DTM_ALLOC( Results, DtStructure ))
	{
		DTM_INIT( Results );
		return( 0 );
	}

	for( ClassInfo = SecDbClassEnumFirst( &EnumPtr ); ClassInfo; ClassInfo = SecDbClassEnumNext( EnumPtr ))
		if( ClassInfo->FuncPtr )
		{
			MsgData.MemUsed.Size = 0;
			MsgData.MemUsed.Class = ClassInfo;
			DTM_INIT( &MsgData.MemUsed.Detail );
			if( CLASS_MESSAGE( ClassInfo, SDB_MSG_MEM_USED, &MsgData ))
			{
				Total += MsgData.MemUsed.Size;
				if( MsgData.MemUsed.Detail.DataType != DtNull )
				{
					sprintf( Tag, "Class: %s", ClassInfo->Name );
					DtComponentSet( Results, Tag, &MsgData.MemUsed.Detail );
					DTM_FREE( &MsgData.MemUsed.Detail );
				}
			}
		}
	SecDbClassEnumClose( EnumPtr );

	SET_HASH( SecDbSecTypeHash )
	SET_HASH( SecDbSecDescHash )

	SET_HASH( SecDbValueTypeHash )

	Size = 0;
	HASH_FOR( HashPtr, SecDbValueTypeHash )
		Size += GSHeapMemSize( HashKey( HashPtr ));
	Total += Size;
	SET_NUM( Results, "ValueTypes", Size );

	SET_NUM( Results, "~Total", Total );
	return( Total );
}



/****************************************************************
**	Routine: DatabaseUsage
**	Returns: Total bytes used by Database data structures
**	Action :
****************************************************************/

static int DatabaseUsage(
	DT_VALUE	*Results
)
{
	SDB_INDEX
			*Index;

    SDB_DB  *SecDb;

	HASH_ENTRY_PTR
			HashPtr;

	long	Size,
			Total = 0;


	if( !DTM_ALLOC( Results, DtStructure ))
	{
		DTM_INIT( Results );
		return( 0 );
	}

	SET_HASH( SecDbDatabaseHash )
	SET_HASH( SecDbDatabaseIDHash )
	Size = 0;
	HASH_FOR( HashPtr, SecDbDatabaseHash )
	{
		SecDb = (SDB_DB *) HashValue( HashPtr );
		Size += GSHeapMemSize( SecDb ) + SecDb->FullDbName.size();
		switch( SecDb->DbOperator )
		{
			case SDB_DB_DIDDLE_SCOPE:
				break;

			case SDB_DB_SUBDB:
				Size += SecDb->Location.size();
				break;

			default:
				Size += SecDb->DllPath.size() + SecDb->FuncName.size() +
						SecDb->Argument.size() + SecDb->LockServer.size() +
						SecDb->MirrorGroup.size() + SecDb->Location.size() +
						SecDb->DatabasePath.size();
		}
	}

	Total += Size;
	SET_NUM( Results, "Dbs", Size );

	SET_HASH( SecDbIndexHash )
	Size = 0;
	HASH_FOR( HashPtr, SecDbIndexHash )
	{
		Index = (SDB_INDEX *) HashValue( HashPtr );
		Size += GSHeapMemSize( Index ) + GSHeapMemSize( Index->Classes );
	}
	Total += Size;
	SET_NUM( Results, "Indices", Size );

	Size = GSHeapMemSize( SecDbDatabaseBuffer );
	Total += Size;
	SET_NUM( Results, "SecDbDatabaseBuffer", Size );

	SET_NUM( Results, "~Total", Total );
	return( Total );
}



/****************************************************************
**	Routine: SlangUsage
**	Returns: Total bytes used by Slang data structures
**	Action :
****************************************************************/

static int SlangUsage(
	DT_VALUE	*Results
)
{
	DT_VALUE
			SubResults;

	HASH_ENTRY_PTR
			HashPtr,
			VarPtr;

	SLANG_VARIABLE_SCOPE
			*VarScope;

	SLANG_VARIABLE
			*Variable;

	SLANG_CONTEXT
			*Context;

	HEAP_STATS
			HeapStats;

	SLANG_FUNCTION_INFO
			*FunctionInfo;

	SLANG_ENUM
			*Enum;

	SLANG_CONSTANT
			*Constant;

	long	Size,
			SubTotal,
			Total = 0;


	if( !DTM_ALLOC( Results, DtStructure ) || !DTM_ALLOC( &SubResults, DtStructure ))
	{
		DTM_INIT( Results );
		return( 0 );
	}

	SET_HASH( SlangNodesByName )
	SET_HASH( SlangNodesByType )
	SET_HASH( SlangRootHash )

	Size = HashMemSize( SlangConstantsHash );
	HASH_FOR( HashPtr, SlangConstantsHash )
	{
		Constant = (SLANG_CONSTANT *) HashValue( HashPtr );
		Size += GSHeapMemSize( Constant );
		Size += GSHeapMemSize( Constant->Name );
		Size += GSHeapMemSize( Constant->EnumName );
	}
	Total += Size;
	SET_NUM( Results, "SlangConstantsHash", Size );

	Size = HashMemSize( SlangEnumsHash );
	HASH_FOR( HashPtr, SlangEnumsHash )
	{
		Enum = (SLANG_ENUM *) HashValue( HashPtr );
		Size += GSHeapMemSize( Enum );
		Size += GSHeapMemSize( Enum->EnumName );
		Size += HashMemSize( Enum->Members );
	}
	Total += Size;
	SET_NUM( Results, "SlangEnumsHash", Size );

	Size = HashMemSize( SlangNodeTypeHash );
	HASH_FOR( HashPtr, SlangNodeTypeHash )
	{
		FunctionInfo = (SLANG_FUNCTION_INFO *) HashValue( HashPtr );
		Size += GSHeapMemSize( FunctionInfo );
		Size += GSHeapMemSize( FunctionInfo->DllPath );
		Size += GSHeapMemSize( FunctionInfo->FuncName );
	}
	Total += Size;
	SET_NUM( Results, "SlangNodeTypeHash", Size );

	Size = HashMemSize( SlangFunctionHash );
	HASH_FOR( HashPtr, SlangFunctionHash )
	{
		FunctionInfo = (SLANG_FUNCTION_INFO *) HashValue( HashPtr );
		Size += GSHeapMemSize( FunctionInfo );
		Size += GSHeapMemSize( FunctionInfo->Name );
		Size += GSHeapMemSize( FunctionInfo->DllPath );
		Size += GSHeapMemSize( FunctionInfo->FuncName );
	}
	Total += Size;
	SET_NUM( Results, "SlangFunctionHash", Size );

	SET_HASH( SlangVarScopeHash )
	SubTotal = 0;
	HASH_FOR( HashPtr, SlangVarScopeHash )
	{
		Size		= 0;
		VarScope	= (SLANG_VARIABLE_SCOPE *) HashValue( HashPtr );

		Size += GSHeapMemSize( VarScope );
		Size += HashMemSize( VarScope->VariableHash );
		HASH_FOR( VarPtr, VarScope->VariableHash )
		{
			Variable = (SLANG_VARIABLE *) HashValue( VarPtr );
			if( !( Variable->Flags & SLANG_VARIABLE_RETAIN ))
				Size += DtMemSize( Variable->Value );
		}
		SubTotal += Size;
		SET_NUM( &SubResults, VarScope->VariableHash->Title, Size );
	}
	Total += SubTotal;
	SET_NUM( &SubResults, "~Total", SubTotal );
	DtComponentSet( Results, "VariableScopes", &SubResults );
	DTM_FREE( &SubResults );

	Size = 0;
	for( Context = SlangContext; Context; Context = Context->Next )
		Size += GSHeapMemSize( Context ) + MEM_POOL_MEM_SIZE( Context->Hash->MemPool );
	Total += Size;
	SET_NUM( Results, "Contexts", Size );

	GSHeapStatistics( SlangVariableHeap, &HeapStats );
	Size = HeapStats.UsedMemory;
	Total += Size;
	SET_NUM( Results, "VariableHeapUsed", Size );
	Size = HeapStats.FreeMemory;
	Total += Size;
	SET_NUM( Results, "VariableHeapFree", Size );

	SET_NUM( Results, "~Total", Total );
	return( Total );
}



/****************************************************************
**	Routine: NodeMemoryUsage
**	Returns: Total bytes used by Nodes
**	Action : Creates a DtStructure with memory usage stats
****************************************************************/

long NodeMemoryUsage(
	DT_VALUE	*Results
)
{
	struct
	{
		int		Flag;

		const char
				*Label;

	} EnumFlags[] = {
		{ SDB_NODE_ENUM_TERMINALS,		"Terminals"		},
		{ SDB_NODE_ENUM_NON_TERMINALS,	"NonTerminals"	},
		{ SDB_NODE_ENUM_LITERALS,		"Literals"		},
	};

	HEAP_STATS
			HeapStats;

	HASH_ENTRY_PTR
			HashPtr;

	SDB_ENUM_PTR
			EnumPtr;

	SDB_NODE
			*Node,
			*NodeObject;

	DT_VALUE
			SubResults,
			ObjResults;

	SDB_DIDDLE_HEADER
			*DiddleHeader;

	SDB_DIDDLE
			*Diddle;

	int		Num;

	long	Size,
			NodevSize,
			ValuesSize,
			NodeCount,
			SubTotal,
			HeapTotal	= 0,
			Total		= 0;

	long	ObjectNodevSize = 0,
			ObjectNodeCount = 0,
			ObjectValuesSize = 0;


	if( !DTM_ALLOC( Results, DtStructure ) || !DTM_ALLOC( &SubResults, DtStructure ) ||
		!DTM_ALLOC( &ObjResults, DtStructure ))
	{
		DTM_INIT( Results );
		return( 0 );
	}

	Size = HashMemSize( SDB_NODE::LiteralHash() );
	Total += Size;
	DtComponentSetNumber( Results, "LiteralHash", DtDouble, (double) Size );

	//Size = SkipLikoStats( SDB_NODE::NodeSkipLiko() );
	//Total += Size;
	//DtComponentSetNumber( Results, "NodeSkipList", DtDouble, (double) Size );


	for( Num = sizeof( EnumFlags ) / sizeof( EnumFlags[ 0 ] ) - 1; Num >= 0; Num-- )
	{
		SubTotal	=
		NodevSize 	=
		ValuesSize	=
		NodeCount	= 0;

		NodeObject	= NULL;
		for( Node = SecDbNodeEnumFirst( NULL, EnumFlags[ Num ].Flag, &EnumPtr ); Node; )
		{
			NodeCount++;
			if( !Node->RefCountedParents() )
			{
				NodevSize += GSHeapMemSize( Node->m_children );
				NodevSize += GSHeapMemSize( Node->m_parents );
			}
			if( Node->ValidGet() && !( Node->CacheFlagsGet() & SDB_CACHE_NO_FREE ))
				ValuesSize += DtMemSize( &Node->m_Value );

			SDB_NODE *NextNode = SecDbNodeEnumNext( EnumPtr );
			if( Node->IsTerminalQuick() )
			{
				if( NodeObject != Node->ObjectGet() || NextNode == NULL )
				{
					if( NodeObject )
					{
						DtComponentSetNumber( &ObjResults, "Pointers",	DtDouble, (double) ObjectNodevSize );
						DtComponentSetNumber( &ObjResults, "Values",	DtDouble, (double) ObjectValuesSize );
						DtComponentSetNumber( &ObjResults, "Nodes",		DtDouble, (double) ( ObjectNodeCount * sizeof( SDB_NODE )) );
						DtComponentDestroy(   &ObjResults, "~Total" );

						DtComponentSet( &SubResults, ((SDB_OBJECT *) NodeObject->m_Value.Data.Pointer)->SecData.Name, &ObjResults );
					}
					NodeObject 			= Node->ObjectGet();
					ObjectNodevSize 	= 0;
					ObjectNodeCount 	= 0;
					ObjectValuesSize 	= 0;
				}
				++ObjectNodeCount;
				if( !Node->RefCountedParents() )
				{
					ObjectNodevSize += GSHeapMemSize( Node->m_children );
					ObjectNodevSize += GSHeapMemSize( Node->m_parents );
				}
				if( Node->ValidGet() && !( Node->CacheFlagsGet() & SDB_CACHE_NO_FREE ))
					ObjectValuesSize += DtMemSize( &Node->m_Value );
			}
			Node = NextNode;
		}
		SecDbNodeEnumClose( EnumPtr );

		SubTotal += NodevSize;
		DtComponentSetNumber( &ObjResults, "Pointers", DtDouble, (double) NodevSize );

		SubTotal += ValuesSize;
		DtComponentSetNumber( &ObjResults, "Values", DtDouble, (double) ValuesSize );

		Size = NodeCount * sizeof( SDB_NODE );
		SubTotal += Size;
		HeapTotal += Size;
		DtComponentSetNumber( &ObjResults, "Nodes", DtDouble, (double) Size );

		Total += SubTotal;
		DtComponentSetNumber( &ObjResults, "~Total", DtDouble, SubTotal );
		DtComponentSet( &SubResults, "~Total", &ObjResults );

		DtComponentSet( Results, EnumFlags[ Num ].Label, &SubResults );
	}
	DTM_FREE( &SubResults );
	DTM_FREE( &ObjResults );

	GSHeapStatistics( SDB_NODE::NodeHeap(), &HeapStats );

	Size = HeapStats.UsedMemory - HeapTotal;
	Total += Size;
	DtComponentSetNumber( Results, "NodeHeapOverhead", DtDouble, (double) Size );

	Size = HeapStats.FreeMemory;
	Total += Size;
	DtComponentSetNumber( Results, "NodeHeapFree", DtDouble, (double) Size );

	if( SDB_NODE::DiddleHash() )
	{
		Size = HashMemSize( SDB_NODE::DiddleHash() );
		Total += Size;
		DtComponentSetNumber( Results, "NodeDiddleHash", DtDouble, (double) Size );

		Size = 0;
		HASH_FOR( HashPtr, SDB_NODE::DiddleHash() )
		{
			DiddleHeader = (SDB_DIDDLE_HEADER *) HashValue( HashPtr );
			Size += GSHeapMemSize( DiddleHeader );
			for( Diddle = DiddleHeader->DiddleList; Diddle; Diddle = Diddle->Next )
				Size += GSHeapMemSize( Diddle );
		}
		Total += Size;
		DtComponentSetNumber( Results, "Diddles", DtDouble, (double) Size );
	}

	if( SDB_NODE::InvParentsHash() )
	{
		Size = HashMemSize( SDB_NODE::InvParentsHash() );
		Total += Size;
		DtComponentSetNumber( Results, SDB_NODE::InvParentsHashName(), DtDouble, (double) Size );
	}

	DtComponentSetNumber( Results, "~Total", DtDouble, Total );

	return( Total );
}



/****************************************************************
**	Routine: SecDbMemoryUsage
**	Returns: Total bytes used by SecDb
**	Action : Creates and fills in a DtStructure with memory usage
**			 stats.
****************************************************************/

int SecDbMemoryUsage(
	DT_VALUE	*Results		// Returned DtStructure
)
{
	DT_VALUE
			SubUsage;

	HASH_ENTRY_PTR
			HashPtr;

	DLL_VERSION_INFO
			*Info;

	long	Size,
			Total = 0;


	if( !DTM_ALLOC( Results, DtStructure ))
		return( FALSE );

	Total += NodeMemoryUsage( &SubUsage );
	DtComponentSet( Results, "Cache", &SubUsage );
	DTM_FREE( &SubUsage );

	Total += ObjectUsage( &SubUsage );
	DtComponentSet( Results, "Object", &SubUsage );
	DTM_FREE( &SubUsage );

	Total += ClassUsage( &SubUsage );
	DtComponentSet( Results, "Class", &SubUsage );
	DTM_FREE( &SubUsage );

	Total += DatabaseUsage( &SubUsage );
	DtComponentSet( Results, "Database", &SubUsage );
	DTM_FREE( &SubUsage );

	Total += SlangUsage( &SubUsage );
	DtComponentSet( Results, "Slang", &SubUsage );
	DTM_FREE( &SubUsage );

	Total += DtMemoryUsage( &SubUsage );
	DtComponentSet( Results, "DataType", &SubUsage );
	DTM_FREE( &SubUsage );

	Size = DateHolidaysMemSize();
	Total += Size;
	SET_NUM( Results, "DateHolidays", Size );

	Size = HashMemSize( DynaLinkVersionHash );
	HASH_FOR( HashPtr, DynaLinkVersionHash )
	{
		Info = (DLL_VERSION_INFO *) HashValue( HashPtr );
		Size += GSHeapMemSize( Info );
		Size += GSHeapMemSize( Info->DllName );
		Size += GSHeapMemSize( Info->Allowed ); 	// FIX - whole list?
	}
	Total += Size;
	SET_NUM( Results, "DynaLink", Size );

	Size = MEM_POOL_MEM_SIZE( SecDbConfiguration->MemPool );
	Total += Size;
	SET_NUM( Results, "SecDbConfiguration", Size );

	SET_NUM( Results, "~Total", Total );

	return( Total );
}
