#define GSCVSID "$Header: /home/cvs/src/secdb/src/slang_validate.cpp,v 1.14 2013/08/23 16:55:31 khodod Exp $"
/*************************************************************************
** 
**	Slang Code Safety API 
**
**	$Log: slang_validate.cpp,v $
**	Revision 1.14  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.13  2013/08/13 05:30:08  khodod
**	Correct export decorator to eliminate annoying linker warnings.
**	AWR: #310154
**
**	Revision 1.12  2011/05/17 01:32:14  khodod
**	Use gs_crypt.
**	AWR: #177463
**
**
**************************************************************************/

#define BUILDING_SECDB
#include <slang_validate.h>

#include <string>
CC_USING( std::string );

//#define _SLANG_VALIDATE_DEBUG
#ifdef _SLANG_VALIDATE_DEBUG
#	define DEBUG1( a1 ) fprintf( stderr, a1 )
#	define DEBUG2( a1, a2 ) fprintf( stderr, a1, a2 )
#	define DEBUG3( a1, a2, a3 ) fprintf( stderr, a1, a2, a3 )
#	define DEBUG4( a1, a2, a3, a4 ) fprintf( stderr, a1, a2, a3, a4 )
#else
#	define DEBUG1( a1 )
#	define DEBUG2( a1, a2 )
#	define DEBUG3( a1, a2, a3 )
#	define DEBUG4( a1, a2, a3, a4 )
#endif

struct Module_Info {
	Module_Info( const char *Name, bool Safe = false ) : 
		m_Name( Name ),
		m_Safe( Safe ), 
		m_Valid( true ),
		m_Children(),
		m_Parents()
		{}

	string m_Name;
	bool m_Safe;
	bool m_Valid;

	CC_STL_VECTOR( Module_Info *) m_Children;
	CC_STL_VECTOR( Module_Info *) m_Parents;
};



bool
	g_SecEditEditInProgress = false;

SLANG_NODE *
	g_CurrentSlangNode;

static HASH *
	ModuleDepTree;

static const char *
	AccessKey;


static bool 
	ComputeAndSetSafetyInfo( Module_Info *Node, int Depth = 0 );

static 
	void PrintModuleSafetyInfo( Module_Info *Node, int Depth = 0 );

static
	void PrintDepTree( void );

/****************************************************************
**	Routine: SlangInUserScript
**	Returns: TRUE if we are currently executing an application script
**	Summary:
*****************************************************************/

bool SlangInUserScript()
{
	return !g_SecEditEditInProgress && g_CurrentSlangNode;
}


/****************************************************************
**	Routine: BaseName
**	Returns: script name sans any db prefix
**	Summary: 
****************************************************************/

string BaseName( const char *ModuleName )
{
	string TempName = ModuleName;
	string::size_type pos;
    	
	if( ( pos = TempName.rfind( SDB_SUBDB_DELIMITER_STRING ) ) != string::npos )
		TempName = TempName.substr( pos + 1 );
    
	return( TempName );
}

/****************************************************************
**	Routine: HashModuleNameCmp
**	Returns: 
**	Summary: comparator for module names
****************************************************************/

int HashModuleNameCmp( const char *a, const char *b )
{
	return( stricmp( BaseName( a ).c_str(), BaseName( b ).c_str() ) );
}

/****************************************************************
**	Routine: HashModuleName
**	Returns: Hash of the ModuleName
**	Summary: 
****************************************************************/

int HashModuleName( const char *a )
{
	return HashStrHash( BaseName( a ).c_str() );
}

/****************************************************************
**	Routine: SlangCheckWriteAccess
**	Returns: TRUE if Prod write access is enabled/FALSE o/w
**	Summary: 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME 
#define ARGCHECK_FUNCTION_NAME "SlangCheckWriteAccess"

int SlangCheckWriteAccess( SDB_DB *pDb )
{
	bool WritesAllowed = true;
	static int BypassCheck = -1;
	char *ComputedKey;
	if( BypassCheck == -1 ) 
	{
		BypassCheck = 0;
		if( ( AccessKey = SecDbConfigurationGet( "SECDB_BYPASS_WRITE_CHECK_KEY", NULL, NULL, 0 ) ) )
		{
			char *key = getenv( "SECDB_BYPASS_WRITE_CHECK" );
			if( key && !strcmp( ComputedKey = gs_crypt( key, ".." ), AccessKey ) )
				BypassCheck = 1;
		}
	}
	if( BypassCheck ) 
		return true;
	
	if( ( pDb->DbAttributes & SDB_DB_ATTRIB_PROD ) ) 
	{
		if( g_CurrentSlangNode ) // XXX ModuleType must not be STRING!
		{
			DEBUG2( ARGCHECK_FUNCTION_NAME ": %p\n", g_CurrentSlangNode );
			if( SlangModuleIsSafe( g_CurrentSlangNode->ErrorInfo.ModuleName ) ) 
			{
				WritesAllowed = true;
			} 
			else 
			{
				WritesAllowed = false;
				Err( ERR_UNKNOWN, "Cannot write to a Prod Db (%s) with unsafe code (%s)\n", pDb->FullDbName.c_str(), g_CurrentSlangNode->ErrorInfo.ModuleName );
			}
		} 
		else 
		{
			WritesAllowed = true;
			//Err( ERR_UNKNOWN, "Cannot write to a Prod Db (%s) interactively", pDb->FullDbName.c_str() );
		}
	}
	return WritesAllowed;
}

/****************************************************************
**	Routine: SlangModuleVerify
**	Returns: true if the code is safe/false o/w
**	Summary: 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME 
#define ARGCHECK_FUNCTION_NAME "SlangModuleVerify"

bool SlangModuleVerify( const char *ModuleName, const char *Code )
{
	SdbObjectPtr SecPtr;
    DEBUG2( ARGCHECK_FUNCTION_NAME ": %s\n", ModuleName );
	static SDB_DB *SourceDb = 0;

	SDB_VALUE_TYPE VT;

	VT = SecDbValueTypeFromName( "Version Control", DtStructure );
	if( !VT )
		return false;

	if( !SourceDb )
		SourceDb = SecDbAttach( "ProdSource", SDB_DEFAULT_DEADPOOL_SIZE, 0 );
	
	if( !SourceDb )
		return false;

	SecPtr = SecDbGetByName( ModuleName, SourceDb, SDB_ALWAYS_REFRESH );
	if( !SecPtr )
		return false;

	DT_VALUE *VCInfo = SecDbGetValue( SecPtr.val(), VT );
	if( !VCInfo )
		return false;

	unsigned BlessedCrc = DtComponentGetNumber( VCInfo, "Crc", DtDouble, -1 );
	if( BlessedCrc == (unsigned) -1 )
		return false;

	unsigned BlessedSize = DtComponentGetNumber( VCInfo, "Size", DtDouble, -1 );
	if( BlessedSize == (unsigned) -1 )
		return false;
	
	unsigned Size = strlen( Code );
	unsigned Crc = Crc16( Code, Size+1, 0 );

	DEBUG4( "ModuleName: %s Size: %d BlessedSize: %d", ModuleName, Size, BlessedSize );
	DEBUG3( "Crc: %d BlessedCrc: %d\n", Crc, BlessedCrc );

	if( BlessedSize != Size || BlessedCrc != Crc )
		return false;

	return true;
}

/****************************************************************
**	Routine: SlangModuleAttachChild
**	Returns: true for success; false o/w
**	Summary: Attach a new module as a child of a given module
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME 
#define ARGCHECK_FUNCTION_NAME "SlangModuleAttachChild"

bool SlangModuleAttachChild( SLANG_ERROR_INFO *ParentInfo, SLANG_ERROR_INFO *ChildInfo )
{
   	DEBUG3( ARGCHECK_FUNCTION_NAME ": %30.30s %30.30s\n", ParentInfo->ModuleName, ChildInfo->ModuleName );
	if( ParentInfo->ModuleType == SLANG_MODULE_STRING || ChildInfo->ModuleType == SLANG_MODULE_STRING )
		return false;

	Module_Info *Parent_Node = (Module_Info *) HashLookup( ModuleDepTree, (HASH_KEY) ParentInfo->ModuleName );
	if( !Parent_Node )
		return false;

	Module_Info *Child_Node = (Module_Info *) HashLookup( ModuleDepTree, (HASH_KEY) ChildInfo->ModuleName );
	if( !Child_Node )
		return false;
	
	if( Parent_Node == Child_Node )
		return false;
	
	DEBUG3( ARGCHECK_FUNCTION_NAME ": %p -> %p\n", Parent_Node, Child_Node );
	Parent_Node->m_Children.push_back( Child_Node );
	Child_Node->m_Parents.push_back( Parent_Node );

	return true;
}

/****************************************************************
**	Routine: SlangModuleAdd
**  Returns: true for success; false o/w
**	Summary: Add a new module to the dependency tree
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME 
#define ARGCHECK_FUNCTION_NAME "SlangModuleAdd"

bool SlangModuleAdd( SLANG_ERROR_INFO *ModuleErrorInfo, bool Safe )
{
	Module_Info *Node;
    DEBUG3( ARGCHECK_FUNCTION_NAME ": %s %d\n", ModuleErrorInfo->ModuleName, Safe );
	if( !ModuleDepTree ) 
	{
		ModuleDepTree = HashCreate( "ModuleDependencyTree", (HASH_FUNC) HashModuleName, (HASH_CMP) HashModuleNameCmp, 0, NULL );
	}

	if( ModuleErrorInfo->ModuleType == SLANG_MODULE_STRING )
		return false;

	if( ( Node = (Module_Info *) HashLookup( ModuleDepTree, (HASH_KEY) ModuleErrorInfo->ModuleName ) ) ) 
	{
		DEBUG2( ARGCHECK_FUNCTION_NAME ": modifying existing %s\n", ModuleErrorInfo->ModuleName );
		Node->m_Safe = Safe;
		if( !Safe ) 
		{
			CC_STL_VECTOR( Module_Info * )::iterator it = Node->m_Parents.begin();
			while( it != Node->m_Parents.end() ) 
			{
				(*it)->m_Valid = false;
				++it;
			}
		}
	} 
	else 
	{
		char *ModuleName = strdup( ModuleErrorInfo->ModuleName );
		Module_Info *Node = new Module_Info( ModuleName, Safe );
		DEBUG2( ARGCHECK_FUNCTION_NAME ": adding %s\n", ModuleErrorInfo->ModuleName );
		if( !HashInsert( ModuleDepTree, (HASH_KEY) ModuleName, (HASH_VALUE) Node ) ) 
		{
			delete Node;
			delete ModuleName;
			return false;
		}
	}
	return true;
}

/****************************************************************
**	Routine: SlangModuleIsSafe
**	Returns: The safety status of the Root node
**	Summary: Recursively update the safety status of a node 
**			 based on its children.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SlangModuleIsSafe"

bool SlangModuleIsSafe( const char *ModuleName )
{
	DEBUG2( ARGCHECK_FUNCTION_NAME  ": %s\n", ModuleName );

	Module_Info *Node = (Module_Info *) HashLookup( ModuleDepTree, (HASH_KEY) ModuleName );
	if( !Node )
		return false;

	if( !Node->m_Valid )
	{
		DEBUG3( ARGCHECK_FUNCTION_NAME ": %s %p\n", ModuleName, Node );
		ComputeAndSetSafetyInfo( Node );
	}
	DEBUG2( ARGCHECK_FUNCTION_NAME ": %d\n", Node->m_Safe );
	return Node->m_Safe;
}

/****************************************************************
**	Routine: SlangModuleRefreshSafetyInfo
**	Returns: The safety status of the Root node.
**	Summary: Recursively update the safety status of a node 
**			 based on its children.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME 
#define ARGCHECK_FUNCTION_NAME "SlangModuleRefreshSafetyInfo"

void SlangModuleRefreshSafetyInfo( const char *ModuleName )
{
    DEBUG2( ARGCHECK_FUNCTION_NAME ": %s\n", ModuleName );
	Module_Info *Root = (Module_Info *) HashLookup( ModuleDepTree, (HASH_KEY) ModuleName );
	if( !Root ) 
	{   
		DEBUG2( ARGCHECK_FUNCTION_NAME ": Failed to find %s\n", ModuleName );
		return;
	}
	ComputeAndSetSafetyInfo( Root );
}

/****************************************************************
**	Routine: SlangModuleMarkUnsafe
**	Returns: Nothing
**	Summary: Mark the current Slang Module as unsafe if 
**			 the security object came from an unsafe class.
**
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SlangModuleMarkUnsafe"

void SlangModuleMarkUnsafe( SDB_OBJECT *SecPtr )
{
	DEBUG2( ARGCHECK_FUNCTION_NAME ": %s\n", ( SecPtr ? SecPtr->SecData.Name : "<null>" ) );
	if( g_CurrentSlangNode && SecPtr && SecPtr->Class->Unsafe )
	{
		Module_Info *Node = (Module_Info *) HashLookup( ModuleDepTree, (HASH_KEY) g_CurrentSlangNode->ErrorInfo.ModuleName );
		if( Node && Node->m_Safe )
		{
			Node->m_Safe = false;

			CC_STL_VECTOR( Module_Info * )::iterator it = Node->m_Parents.begin();
			while( it != Node->m_Parents.end() ) 
			{
				(*it)->m_Valid = false;
				++it;
			}

			DEBUG4( ARGCHECK_FUNCTION_NAME ": %p %s %d\n", g_CurrentSlangNode, Node->m_Name.c_str(), Node->m_Safe ) ;
		}
	}
}

/****************************************************************
**	Routine: ComputeAndSetSafetyInfo
**	Returns: Recursively computed safety status of Node
**	Summary: Mark the current Slang Module as unsafe
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME 
#define ARGCHECK_FUNCTION_NAME "ComputeAndSetSafetyInfo"

bool ComputeAndSetSafetyInfo( Module_Info *Node, int Depth )
{
	bool childrenSafe = true;
	CC_STL_VECTOR( Module_Info *)::iterator it = Node->m_Children.begin();

	DEBUG4( "> " ARGCHECK_FUNCTION_NAME ": %s #children? %d scriptSafe? %d\n", Node->m_Name.c_str(), Node->m_Children.size(), Node->m_Safe );

	while( it != Node->m_Children.end() ) 
	{
		if( !ComputeAndSetSafetyInfo( *it, ++Depth ) )
			childrenSafe = false;
		++it;
	}
	if( !childrenSafe )
		Node->m_Safe = false;

	Node->m_Valid = true;

	DEBUG4( "< " ARGCHECK_FUNCTION_NAME ": %s childrenSafe? %d scriptSafe? %d\n", Node->m_Name.c_str(), childrenSafe, Node->m_Safe );

	return Node->m_Safe;
}


void PrintModuleSafetyInfo( Module_Info *Node, int Depth )
{
	DEBUG4( "> %s %d %d\n", Node->m_Name.c_str(), Node->m_Safe, Node->m_Children.size() );
	CC_STL_VECTOR( Module_Info *)::iterator it = Node->m_Children.begin();
	while( it != Node->m_Children.end() ) 
	{
		PrintModuleSafetyInfo( *it, ++Depth );
		++it;
	}
}

void PrintDepTree( )
{
    HASH_ENTRY_PTR ptr;
    Module_Info *Node;
    HASH_FOR( ptr, ModuleDepTree )
    {
        Node = (Module_Info *) ptr->Value;
        DEBUG4( "%x %s safe: %d", Node, Node->m_Name.c_str(), Node->m_Safe);
        DEBUG3( " #children: %d #parents: %d\n", Node->m_Children.size(), Node->m_Parents.size() );
    }
}
