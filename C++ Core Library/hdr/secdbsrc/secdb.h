/****************************************************************
**
**	secdb.h		- Security Master Definition
**
**	Copyright 1992-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb.h,v 1.279 2013/10/15 17:30:36 khodod Exp $
**
****************************************************************/

#ifndef IN_SECDB_H
#define IN_SECDB_H

#include <err.h>
#include <datatype.h>
#include <bstream.h>
#include <sdb_base.h>

#include <ccstl.h>
#include <map>

/*
**	Grammar for specifying full object name
**
**	bnf:
**		FullSecName		::= [ FullDbName ! ] SecName
**
**		FullDbName		::= [ FullDbName ; ] QualifiedDbName			(left-associative)
**						or  ( FullDbName )
**
**		QualifiedDbName	::= Qualifier { FullDbName } 
**
**		Qualifier		::= "ReadOnly"
**
**		SecName			::= Case-insensitive tring without ! or ;
**							with strlen between 1 and SDB_SEC_NAME_SIZE - 1
**							and not starting with NewSecurityPrefix.
**
**		DbName			::= Case-insensitive string without ! or ; or parentheses
**							with strlen between 0 and SDB_DB_NAME_SIZE - 1.
**
**	examples of FullSecNames for USD/DEM SecName:				(each item in parens below is a distinct database)
**		USD/DEM
**		NYC Production!USD/DEM									== (NYC Production)!USD/DEM
**		NYC Simulation;NYC Production!USD/DEM					== ((NYC Simulation);(NYC Production))!USD/DEM
**		User!Lundek;NYC Simulation;NYC Production!USD/DEM		== ((((User)!Lundek);(NYC Simulation));(NYC Production))!USD/DEM
**		User!Lundek!Test1;NYC Production!USD/DEM				== ((((User)!Lundek)!Test1);(NYC Production))!USD/DEM
**		Master!(Foo!Bar)!USD/DEM								== ((Master)!((Foo)!Bar))!USD/DEM
**		User!Lundek;ReadOnly{NYC Production}!UsD/DeM			== (((User)!Lundek);(ReadOnly{NYC Production}))!USD/DEM
**		ReadOnly{User!Lundek;NYC Production}!USD/DEM			== ReadOnly{(((User)!Lundek);(NYC Production)}!USD/DEM
*/

#define		SDB_SEARCH_DELIMITER_STRING			";"
#define		SDB_CLASSFILTER_PREFIX_STRING		":"
#define		SDB_CLASSFILTER_DELIMITER_STRING	","
#define		SDB_SECNAME_DELIMITER_STRING		"!"
#define		SDB_SUBDB_DELIMITER_STRING			"!"
#define		SDB_USERDB_PREFIX_STRING			"~"
#define		SDB_QUALIFIER_OPEN_STRING			"{"
#define		SDB_QUALIFIER_CLOSE_STRING			"}"
#define		SDB_ALIAS_ASSIGNMENT_STRING		   	"="

// Any valid delimiter
#define 	SDB_DELIMITER_STRING				SDB_SEARCH_DELIMITER_STRING SDB_SUBDB_DELIMITER_STRING SDB_CLASSFILTER_DELIMITER_STRING SDB_CLASSFILTER_PREFIX_STRING
#define 	SDB_DELIMITER_AND_QUALIFIER_STRING	SDB_DELIMITER_STRING SDB_QUALIFIER_OPEN_STRING SDB_QUALIFIER_CLOSE_STRING

char const  SDB_SEARCH_DELIMITER				= SDB_SEARCH_DELIMITER_STRING      [0];
char const  SDB_CLASSFILTER_PREFIX				= SDB_CLASSFILTER_PREFIX_STRING    [0];
char const  SDB_CLASSFILTER_DELIMITER			= SDB_CLASSFILTER_DELIMITER_STRING [0];
char const  SDB_SECNAME_DELIMITER				= SDB_SECNAME_DELIMITER_STRING     [0];
char const  SDB_SUBDB_DELIMITER					= SDB_SUBDB_DELIMITER_STRING       [0];
char const  SDB_USERDB_PREFIX					= SDB_USERDB_PREFIX_STRING          [0];
char const  SDB_QUALIFIER_OPEN					= SDB_QUALIFIER_OPEN_STRING        [0];
char const  SDB_QUALIFIER_CLOSE					= SDB_QUALIFIER_CLOSE_STRING       [0];


// Characters not allowed in security names
#define		SDB_WHITESPACE_FORBIDDEN_CHARS		"\t\n"
#define		SDB_OTHER_FORBIDDEN_CHARS			"^`\\|"
#define		SDB_DELIMITER_FORBIDDEN_CHARS		SDB_SEARCH_DELIMITER_STRING SDB_SUBDB_DELIMITER_STRING SDB_QUALIFIER_OPEN_STRING SDB_QUALIFIER_CLOSE_STRING
#define		SDB_PRINTABLE_FORBIDDEN_CHARS		SDB_DELIMITER_FORBIDDEN_CHARS SDB_OTHER_FORBIDDEN_CHARS
#define		SDB_FORBIDDEN_CHARS					SDB_WHITESPACE_FORBIDDEN_CHARS SDB_PRINTABLE_FORBIDDEN_CHARS


enum SDB_QUALIFIER {
	SDB_QUALIFIER_ERROR				= 0,
	SDB_QUALIFIER_READONLY 			= 1,
	SDB_QUALIFIER_CLASSFILTERUNION 	= 2,
	SDB_QUALIFIER_DUPLICATE			= 3
};

typedef		char SDB_FULL_SEC_NAME;			// SecName with path
typedef		char SDB_FULL_DB_NAME;			// Search list of qualified database names
typedef		char SDB_QUALIFIED_DB_NAME;		// Database name with possible database path
typedef		char SDB_SEC_NAME;				// SecName without database path
typedef		char SDB_DB_NAME;				// Database name without database path or search list


/*
**	Define constants
*/

unsigned const SDB_DB_NAME_SIZE			 = 32;			// Database name size
unsigned const SDB_SEC_NAME_SIZE		 = 32;			// Security name size (for allocating)
unsigned const SDB_SEC_NAME_LEN			 = SDB_SEC_NAME_SIZE - 1;	//Security name max usable length
unsigned const SDB_FULL_DB_NAME_SIZE	 = 128;			// Maximum FullDbName size
unsigned const SDB_FULL_SEC_NAME_SIZE	 = (SDB_FULL_DB_NAME_SIZE + SDB_SEC_NAME_SIZE);
unsigned const SDB_VALUE_NAME_SIZE		 = 48;			// Value type name size
unsigned const SDB_PATH_NAME_SIZE		 = 64;			// Path name size
unsigned const SDB_FUNC_NAME_SIZE		 = 32;			// Function name size
unsigned const SDB_DEFAULT_DEADPOOL_SIZE = 50;			// Default size of deadpool
unsigned const SDB_CLASS_NAME_SIZE  	 = 32;			// Max size of security class
unsigned const SDB_CLASS_ARG_SIZE		 = 128;			// Max size of class argument string
unsigned const SDB_USER_NAME_SIZE		 = 16;			// Max user name size
unsigned const SDB_APP_NAME_SIZE		 = 16;			// Max application name size


// define datatype flag for special secdb datatypes

#define DT_FLAG_VALUE_TYPE			DT_FLAG_FIRST_USER_FLAG	 	// This datatype is supported as a value type by secdb
#define DT_FLAG_SDB_OBJECT			( DT_FLAG_VALUE_TYPE << 1 )	// This datatype is supported as an object by secdb


/*
**	Attach flags
*/

// Defaults
int const SDB_BACK		= 0x0000;		// Do not change SDB_ROOT_DATABASE unless there is none
int const SDB_READWRITE	= 0x0000;		// Allow write operations (if no permission, is demoted to READONLY automatically)
int const SDB_BLOCK		= 0x0000;		// Block when opening a connection to a busy server.

int const SDB_FRONT		= 0x0001;		// Set this Db as SDB_ROOT_DATABASE
		// Note SDB_READONLY returns an id that is different than if opened
		// READWRITE.  A database may be attached in both READ_ONLY and
		// READWRITE simultaneously but will appear to be different databases.
int const SDB_READONLY	= 0x0004;		// Disallow write operations, i.e., Add, Update, Delete, Rename, etc.
int const SDB_NEVER_WRITE = 0x0010;		// Demote all subsequent attachments to READONLY
int const SDB_TIMEOUT	= 0x0008;		// Opening a connection to a non-responsive server will time out instead of blocking.
int const SDB_SYNTHETIC	= 0x0100;		// Database is synthetic.  There is no server side at all.

int const SDB_NO_SECURITY_DATABASE = 0x0200;	// This database has no security database object.  This is true of synthetics.


/*
**	Database access codes used for get functions)
*/

int const SDB_ROOT_DATABASE		= 0x01000;		// Sentinel for default database as determined by SDB_FRONT or SecDbSetRootDatabase

int const SDB_REFRESH_CACHE		= 0x02000;		// Refresh sec from database even if in the cache
int const SDB_CACHE_ONLY		= 0x04000;		// Do not get from database even if not in the cache
int const SDB_IGNORE_PATH		= 0x08000;		// Get from specific database given by SDB_DB_ID without regard to search path
int const SDB_ALWAYS_REFRESH	= 0x10000;		// Always refresh security even if not changed
int const SDB_GET_FATAL_ERRORS_ONLY = 0x20000; 	// only call Err for fatal errors

/*
**	Database ids
*/

int const SDB_DB_ID_MAX			= 0x0ff0;		// Maximum SDB_DB_ID for a DbName, i.e. a database with no qualifiers and no searchpaths; all DbIDs in SDB_DISKs will be less than or equal to this
int const SDB_DB_VIRTUAL		= 0x1000;		// Bit which is set in all the arbitrary SDB_DB_IDs assigned to databases with qualifiers and/or searchpaths; or to distinguish READONLY from READWRITE
int const SDB_DB_ID_ERROR		= (-1);		// Error sentinel; returned from SecDbAttach, etc. on error
int const SDB_DB_ID_ALIAS		= (-2);		// Set in database.dat to indicate alias database


/*
**	Get operation types
**
**	FIX- this ordering sucks! They should be ordered so that a comparision
**	can determine whether a merge should do a max or a min
*/

int const SDB_GET_FIRST			= 1;
int const SDB_GET_LAST	  		= 2;
int const SDB_GET_EQUAL	  		= 3;
int const SDB_GET_LESS	  		= 4;
int const SDB_GET_LE		  	= 5;
int const SDB_GET_GREATER	  	= 6;
int const SDB_GET_GE		  	= 7;
int const SDB_GET_NEXT	  		= 8;
int const SDB_GET_PREV	  		= 9;


/*
**	Child enumeration request flags:
**	
**		FIRST_LEVEL and TREE:
**			FIRST_LEVEL is assumed unless TREE bit is set.
**			For FIRST_LEVEL include only direct children
**	
**		LEAVES_ONLY:
**			Only include values marked SDB_SET_RETAINED_FLAG
**			(unless BELOW_SET also set, then only include
**			values with no children).
**	
**		SELF_ONLY:
**			Stop recursing when a different SecPtr is encountered.
**	
**		BELOW_SET:
**			Do not stop recursing at values marked SDB_SET_RETAINED_FLAG.
**	
**		EXTERNAL:
**			Only include values marked SDB_EXTERNAL
**	
**		PRUNE:
**			Do not recurse below any matching nodes.
**			Meaningless without TREE bit.
**	
**		SORT:
**			Iterate in sort order
*/

int const SDB_ENUM_FIRST_LEVEL	= 0x0000;
int const SDB_ENUM_TREE			= 0x0001;
int const SDB_ENUM_LEAVES_ONLY	= 0x0002;
int const SDB_ENUM_SELF_ONLY	= 0x0004;
int const SDB_ENUM_EXTERNAL		= 0x0008;
int const SDB_ENUM_SORT			= 0x0010;
int const SDB_ENUM_PRUNE		= 0x0020;
int const SDB_ENUM_BELOW_SET	= 0x0100;
int const SDB_ENUM_GET_VALUE	= 0x0200;
int const SDB_ENUM_EXTERNAL_VT	= 0x0400;



/*
**	Child information flags
*/

int const SDB_CHILD_LEAF		= 0x0001;


/*
**	Define Value Type Flags
**
**	All value type flags should be defined here, even ones used internally
**	by Trade and Gob because this is the only way to make sure they are
**	assigned without overlaps.
*/

unsigned const SDB_CALCULATED                       = 0x00000001;       // Informational only - not used
unsigned const SDB_DEPRECATED                       = 0x00000002;       // Deprecated value type
unsigned const SDB_STATIC                           = 0x00000004;       // Informational only - not used
unsigned const SDB_EXTERNAL_FLAG                    = 0x00000008;       // 'External' market value
unsigned const SDB_IN_STREAM                        = 0x00000010;       // Value gets written to BSTREAM
unsigned const SDB_SET_RETAINED_FLAG                = 0x00000020;       // SetValue()s stick
unsigned const SDB_CLASS_STATIC                     = 0x00000040;       // VT is class static
unsigned const SDB_EXPORT_FLAG                      = 0x00000080;       // Available from other objects
unsigned const SDB_VALUE_FUNC_PER_INSTANCE          = 0x00000100;       // automatically instantiate VTI node
unsigned const SDB_ALIAS                            = 0x00000200;       // Identical to child

unsigned const SDB_FORCE_INHERIT_NON_INSTREAM       = 0x00000400;       // Force this value type to be explicitly inherited in derived class.
                                                                // This is useful when you declare 'pure virtual' vts in a base class.

unsigned const GOB_REPLACE_VT                       = 0x00000800;       // Overload inherited value type
unsigned const GOB_DISINHERIT_VT                    = 0x00001000;       // Delete this inherited value type
unsigned const GOB_INHERIT_VT                       = 0x00002000;       // Place this inherited value type at this location in value table
unsigned const GOB_INHERIT_CLASS_FLAG               = 0x00004000;       // Inherit all value types from class given by Vti->Name
unsigned const GOB_MUSH_IGNORE_OLD                  = 0x00008000;       // Exclude value type from hash code (old)
unsigned const GOB_MUSH_IGNORE_NEW                  = 0x00010000;       // Exclude value type from hash code (new)

unsigned const SDB_RELEASE_DESCENDENT_VALUES        = 0x00020000;       // Release descendent node tagged with SDB_RELEASE_BY_ANCESTOR flag


// The following unused bits are available for redefinition
unsigned const SDB_ALWAYS_RECOMPUTE                 = 0x00040000;       // Treat VT as a method; always call its value function, even if valid
unsigned const SDB_UNUSED_BIT_1                     = 0x00080000;       // unused


unsigned const SDB_SELF_DESC_INDEXED                = 0x00100000;       // Tag the VT as one that should go into a self-describing stream for indexing


// These are mutually exclusive flags
unsigned const SDB_RELEASE_VALUE                    = 0x00200000;       // release the value after it gets consumed by a parent
unsigned const SDB_RELEASE_VALUE_WHEN_PARENTS_VALID = 0x00400000;       // release the value after *all* the parents are valid 
unsigned const SDB_RELEASE_VALUE_BY_ANCESTOR        = 0x00800000;      // value is removed after ancestor with SDB_RELEASE_DESCENDENT_VALUES is computed
unsigned const SDB_RELEASE_VALUE_MASK               = 0x00E00000;      // 

//  Trade Private Value Type Flags
unsigned const TRADE_ALLOW_OVERRIDE_VT              = 0x01000000;       // Allow Security Traded vt to override default behavior
unsigned const TRADE_SEC_TRADED_IN_STREAM           = 0x02000000;       // Value is an in-stream value of Security Traded

unsigned const GOB_IMPLEMENT_INTERFACE_FLAG         = 0x04000000;       // Implement all value types specified in the class given by Vti->Name
unsigned const GOB_DISINHERIT_INTERFACE_FLAG        = 0x08000000;       // Disinherit interface inherited from a parent

unsigned const SDB_AUTO_CHILD_LIST                  = 0x10000000;      // Allow child discovery during gets
unsigned const TRADE_IGNORE_OVERRIDE_VT             = 0x20000000;      // Don't allow Security Traded vt to override default behaviour

unsigned const SDB_TRACK_NODES_WITH_ARGS            = 0x40000000;      // Track nodes with args for this VT
unsigned const SDB_EXPENSIVE                        = 0x80000000;      // Node is expensive

// For backwards compatibility, GOB_MUSH_IGNORE is set to ignore only for old mush.
int const GOB_MUSH_IGNORE						= GOB_MUSH_IGNORE_OLD;

// Newly added value types should be ignored in both the old and new
// mush using the following:
int const GOB_MUSH_IGNORE_COMPLETELY            = ( GOB_MUSH_IGNORE_OLD | GOB_MUSH_IGNORE_NEW );

// Moribund flags
int const SDB_HIDDEN			= 0x0000;		// Moribund - no semantics (was 0x0002)
int const SDB_CACHE				= 0x0000;		// Moribund - no semantics (was 0x0100)

// Combination flags
int const SDB_SET_RETAINED	    = ( SDB_CACHE | SDB_SET_RETAINED_FLAG );
int const SDB_EXPORT			= ( SDB_CACHE | SDB_EXPORT_FLAG );
int const SDB_EXTERNAL			= ( SDB_CACHE | SDB_EXTERNAL_FLAG | SDB_EXPORT_FLAG );
int const SDB_STORED			= ( SDB_CACHE | SDB_IN_STREAM | SDB_SET_RETAINED_FLAG );


/*
**	Define validate flags
*/

int const SDB_VALIDATE_OK	  	= 0x0000;
int const SDB_VALIDATE_ERROR	= 0x0001;
int const SDB_VALIDATE_WARNING	= 0x0002;
int const SDB_VALIDATE_MISSING	= 0x0004;
int const SDB_VALIDATE_INVALID	= 0x0008;
int const SDB_VALIDATE_RANGE   	= 0x0010;


/*
**	SDB_SET_FLAGS (NOTE: must fit in unsigned 12 bit value)
*/

int const SDB_CACHE_SET			    = 0x001;
int const SDB_CACHE_NO_FREE		    = 0x002;
int const SDB_CACHE_DIDDLE			= 0x004;
int const SDB_NODE_CACHE_FLAGS_MASK	= 0x007;

int const SDB_SET_INTERACTIVE		= 0x008;
int const SDB_CACHE_TOUCH			= 0x010;
int const SDB_CACHE_NO_COPY			= 0x020;
int const SDB_SET_NO_MESSAGE		= 0x040;
int const SDB_SET_UNDER_DIDDLE		= 0x080;
int const SDB_SET_COERCE_TYPES		= 0x100;
int const SDB_SET_DONT_LOOKUP_VTI	= 0x200;	/* this is an internal flag, you better not use it unless you know what you are doing */
int const SDB_DIDDLE_IS_PHANTOM     = 0x400;    /* this is an internal flag, you better not use it unless you know what you are doing */
int const SDB_CACHE_PASS_THROUGH	= ( SDB_CACHE_NO_COPY | SDB_CACHE_NO_FREE );


/*
**	Define diddle ID values
*/

long const SDB_DIDDLE_ID_NEW         = (-1L);
long const SDB_DIDDLE_ID_ERROR       = (-2L);
long const SDB_DIDDLE_ID_SET_VALUE   = (-3L);
long const SDB_DIDDLE_ID_NEW_PHANTOM = (-4L);
long const SDB_DIDDLE_ID_WHITEOUT    = (-5L);

/*
**	SDB_GETBYINFERENCE_FLAGS flags
*/

int const SDB_GET_CREATE							= 0x0001;
int const SDB_GET_CACHE_ONLY						= 0x0002;
int const SDB_GET_CACHE_ONLY_INFERRED_NAME   		= 0x0006;	/* Do not consult disk when computing implied name (rename) */

/*
**	SDB_ADD flags
*/

int const SDB_ADD_PRESERVE_DISK_INFO                = 0x0001;   /* Preserve the disk info record when copying. */
int const SDB_ADD_PRESERVE_SRC_DB_ID                = 0x0002;   /* Duplicate source id; used to mimic transactions. */

/*
**	Pass through SecDbError API to Err API
*/


#define		SecDbErrorMessage(Msg)		( ERR_STR_DONT_CHANGE == Msg ?                       \
                                            ErrMsg( ERR_LEVEL_ERROR, ERR_STR_DONT_CHANGE ) : \
                                            ErrMsg( ERR_LEVEL_ERROR, "%s", Msg ) )

#define		SecDbMessage(Msg)			( ERR_STR_DONT_CHANGE == Msg ?                       \
                                            ErrMsg( ERR_LEVEL_INFO,  ERR_STR_DONT_CHANGE ) : \
                                            ErrMsg( ERR_LEVEL_INFO,  "%s", Msg ) )

/*
** Forward declare some types we use
*/

// from kool_ade/hash.h
struct HashStructure;
struct HASHPAIR;
struct HASH_ENTRY;

/*
**  Forward declare types we define here
*/

struct SDB_OBJECT;
struct SDB_VALUE_TYPE_STRUCTURE;
struct SDB_VALIDATE;
struct SDB_CLASS_INFO;
struct SDB_ENUM_STRUCT;

/*
**  Forward declare types we define elsewhere
*/

union SDB_M_DATA;             // in secobj.h
class SDB_NODE;               // in secnodei.h
struct SDB_CHILD_ENUM_FILTER; // in sdb_enum.h

// from sdbchild.h
struct SDB_VALUE_TYPE_INFO;
struct SDB_CHILD_COMPONENT;
struct SDB_CHILD_CAST;
struct SDB_CHILD_LIST;
struct SDB_CHILD_LIST_ITEM;
struct SDB_CHILD_INFO;
struct SDB_CHILD_PULL_VALUE;
struct SDB_M_GET_CHILD_LIST;
struct DT_VALUE_TYPE_INFO;

// from sectrans.h
struct SDB_TRANS_HEADER;
struct SDB_TRANS_MAP;
struct SDB_TRANS_PART;

// from secindex.h
struct SDB_INDEX;
struct SDB_INDEX_POINTER;
struct SDB_INDEX_POS;
struct SDB_OBJECT_CHECK_SUM;

// from secdrv.h
struct SDB_DB;
struct SDB_DBSET;



/*
**	Define types
*/

typedef int32_t 							SDB_TRANS_ID;	// Sequential transaction number
typedef SDB_VALUE_TYPE_STRUCTURE*           SDB_VALUE_TYPE;
typedef SDB_ENUM_STRUCT*                    SDB_ENUM_PTR;

typedef	int32_t				SDB_UNIQUE_ID;
typedef long				SDB_DIDDLE_ID;
typedef int16_t				SDB_DB_ID;
typedef long				SDB_DB_ID_WITH_FLAGS;
typedef	uint16_t			SDB_SEC_TYPE;
typedef unsigned int		SDB_VALUE_FLAGS;
typedef unsigned int		SDB_SET_FLAGS;
typedef int                 SDB_GETBYINFERENCE_FLAGS;
typedef int 				SDB_VALUE_TYPE_ID;
typedef	char				SDB_SEC_NAME_ENTRY[ SDB_SEC_NAME_SIZE ];
typedef	int					(*SDB_OBJ_FUNC)( SDB_OBJECT* SecPtr, int MsgType, SDB_M_DATA* MsgData );
typedef	int					(*SDB_VALUE_FUNC)( SDB_OBJECT* SecPtr, int MsgType, SDB_M_DATA* MsgData, SDB_VALUE_TYPE_INFO* ValueTypeInfo );
typedef void				(*SDB_MSG_FUNC)( char const* Msg );
typedef	int					(*SDB_INIT_FUNC)( void );
typedef int					SDB_ATTACH_FLAGS;

typedef CC_STL_MAP( SDB_CLASS_INFO *, const char * ) SecDbInterfacesMap;
typedef CC_STL_MAP( SDB_VALUE_TYPE_ID, SDB_VALUE_TYPE_INFO* ) SecDbValueTypeMap;

typedef SDB_DIDDLE_ID       (*SDB_DIDDLE_FUNC)( SDB_OBJECT*, SDB_VALUE_TYPE, int, DT_VALUE**, DT_VALUE*, SDB_SET_FLAGS, SDB_DIDDLE_ID );

/*
**	Define structure used to store security class
**	information in the hash table
*/

struct EXPORT_CLASS_SECDB SDB_CLASS_INFO
{
	struct class_info_less {
		bool operator() ( SDB_CLASS_INFO const * const p, SDB_CLASS_INFO const * const q ) const
		{
			return p->Type < q->Type;
		}
	};

	SDB_SEC_TYPE
			Type;							// Security class type

	char	DllPath[	SDB_PATH_NAME_SIZE ],
			FuncName[	SDB_FUNC_NAME_SIZE ],
			Name[	 	SDB_CLASS_NAME_SIZE ],
			Argument[	SDB_CLASS_ARG_SIZE ];

	int		ObjectVersion;					// Version number for entire class

	SDB_OBJ_FUNC
			FuncPtr;						// Pointer to function in dll
			
	SecDbValueTypeMap*
			ValueTypeMap;					// Default value type map

	unsigned long
			MaxStreamSize;					// Maximum stream size in db for objects of this type

	void	*ClassData;						// Class specific data

	SDB_INDEX_POINTER
			*IndexList;						// Pointer to related indices

	SDB_VALUE_TYPE_INFO
			*ValueTable;					// Table of values & functions

	BSTREAM_COMPRESS_INIT
			*FixedInit;						// Pre-defined strings for streaming

	BSTREAM_COMPRESS
			*FixedData;						// Constructed from FixedInit
			
	int		ObjectCount;					// Number of instances in all caches
	
	SDB_OBJECT								// Head and tail of a list of all objects of this class.
			*FirstSec,
			*LastSec,
			*ClassSec;						// Security which holds class static VTs

	SDB_NODE
			*ClassSecNode;
		
	unsigned
			Temporary		:1,				// Class is under development (no db writes allowed)
			Abstract		:1,				// Class is abstract, no instantiation allowed
			Interface		:1,				// Interface is not a class -- cannot have implementation
			SelfDescStream	:1,				// This class participates in self-describing streaming
			CompressedStream:1,				// Stream is compressed
			Unsafe			:1;				// Class is implemented with trusted code (used for UFOs only)

	int		MaxUnparentedNodes,				// Maximum number of unparented nodes an object is allowed
			GCBlockSize;					// number of nodes to GC when limit is reached

	SecDbInterfacesMap	
			*ImplementedInterfaces;			// Map of all interfaces implemented by the class

	bool	GenerateClassSec( char* Prefix );
	void	MarkAsNotStarted();				// Registration failed, so mark it as not started

	SDB_VALUE_TYPE_INFO *LookupVti( SDB_VALUE_TYPE_ID Id ) const
	{
		SecDbValueTypeMap::const_iterator it = ValueTypeMap->find( Id );
		return it != ValueTypeMap->end() ? it->second : NULL;
	}

};


/*
**	Define structure for a value type
*/

struct SDB_VALUE_TYPE_STRUCTURE
{
	char	Name[ SDB_VALUE_NAME_SIZE ];

	SDB_VALUE_TYPE_ID
			ID;							// ValueType ID

	char	NoEval,						// Don't evaluate arguments to this value type
			IsSpecialVt,
			ClassStaticVt;				// Is class static VT

	DT_DATA_TYPE
			DataType;

	SDB_NODE
			*SpecialVt;					// Literal node representing special Vt

};


/*
**	Define disk based object information
*/

struct SDB_DISK
{
	char	Name[ SDB_SEC_NAME_SIZE ];	// SecName: Security unique name

	SDB_SEC_TYPE
			Type;						// Type of security (object)

	SDB_TRANS_ID
			LastTransaction;			// Transaction id of creation or last modification
			
	int32_t	TimeUpdated,				// Time security was last modified
			UpdateCount;				// Number of times security was modified

	unsigned short
			DateCreated;				// Local date of creator

	SDB_DB_ID
			DbIDUpdated;				// ID Number of database where modified

	short	VersionInfo;				// See SDB_DISK_VERSION

};


/*
**	Structure used to decompose SDB_DISK->VersionInfo which contains
**	ObjVersion & StreamVersion.  Use the SecDbDiskInfoGetVersion and
**	SecDbDiskInfoSetVersion functions.
*/

struct SDB_DISK_VERSION
{
	int		Stream,				// [0..1]
			Object;				// [0..15]

};


/*
**	Structure used to hold database stored object information
*/

struct SDB_STORED_OBJECT
{
	SDB_DISK
			SdbDisk;

	unsigned long
			MemSize;

	void*   Mem;

};


/*
**	Define object structure
*/

struct SDB_OBJECT
{
	SDB_CLASS_INFO
			*Class;							// Pointer to class information (this is the class as it is intended to be streamed
											// hence it will be sent all object messages)

	SDB_DB	*Db;							// Db which it belongs to

	int		ReferenceCount,					// Number of pending references
			DeadPoolID;						// ID when in the deadpool

	void	*Object;						// Pointer to object specific data

	SDB_CLASS_INFO
			*SdbClass;						// This is the class information which SecDb get value type information from
											// This distinction is made for the purposes of Trade which is streamed as Trade
											// but is essentially a different class based based on it's Security Traded
	
	SDB_DISK
			SecData;						// Instance data

	SDB_DB	*SourceDb;						// Source Db (for search path db)

	SDB_OBJECT							   
			*ClassNext,						// Next and previous objects of this class.
			*ClassPrev;

	SDB_NODE
			*NodeCache;						// Cache of nodes with no parents

	int		NumCachedNodes;					// Number of cached nodes with no parents

	unsigned char
			Modified,						// If the instreams have been modified in memory
			HaveObjectVti;					// If any node has Object Vti

	void	Free();

};

/*
**  Function for testing if a security has ever been written to its database
*/

inline bool SecDbIsNew( SDB_OBJECT const* ptr ) throw() { return (ptr->SecData.DateCreated == 0);}

/*
**	Define structures used to enumerate things
*/

struct SDB_ENUM_STRUCT
{
	long	Count;							// Number of children enumerated

	HashStructure
			*Hash;							// Hash table of children

	HASHPAIR
			*HashPair,						// Used when sorting children
			*HashPairPos;					// Current position

   	SDB_VALIDATE
   			*Validate;						// For validate info

	HASH_ENTRY
			*Ptr;							// Used when iterating thru hash

	void	*Extra;							// Extra pointer

};

#include <sdbchild.h>

/*
**	Structure used to return object validation information
*/

struct SDB_VALIDATE
{
	SDB_VALUE_TYPE
			ValueType;

	int		Flags;

	char	*Text;

	SDB_VALIDATE
			*Next;

};


/*
**	Structure used to hold database information (for enumeration)
*/

struct SDB_DB_INFO
{
	SDB_FULL_DB_NAME const*
			DbName;				    // Full name of database (including search paths and qualifiers)

	char const* Location,			// Physical location of the database
			*Region,				// Global region (NYC/LDN/TKO/SGP/HKG)
			*MirrorGroup,			// Mirror group of database
			*DllPath,				// Dll path
			*FuncName,				// Main database function
			*DatabasePath,			// Path for database
			*Argument,				// Optional argument
			*LockServer,			// Server where locks are kept
			*Qualifier;				// Qualifier, e.g. "ReadOnly"

	SDB_DB_ID
			DbID;					// Unique ID number of database

	SDB_DB	*Db;					// Pointer to SDB_DB structure for DB

	int		OpenCount,			 	// Greater than 0 if database is open
			DbAttributes;

};


/*
**	Structure used to return statistics
*/

struct SDB_STATISTICS
{
	long	ValueTypeUsage,
			SecTypeUsage,
			DataTypeUsage;

	long	DeadPoolSize,
			DeadPoolUsage,
			ObjectCacheUsage,
			ObjectRefCount,
			DiddleCount,
			ObjectMemory,
			ValueCacheCount,
			ValueCacheMemory;
	
};


/*
**	Global variables
*/


EXPORT_C_SECDB int
		SecDbInitialized;					// TRUE if SecDb initialized

EXPORT_C_SECDB SDB_DB
		*SecDbRootDb;						// Current Root database


EXPORT_C_SECDB char
		SecDbApplicationName[],				// Current application name
		SecDbUserName[],					// Current user name
		SecDbPath[],						// Security database path
		SecDbTypesPath[];					// Security database types path

EXPORT_C_SECDB char const*
		SecDbUniqueIDFileName;				// Filename for unique identifiers


extern long
		SecDbCurrentDate;					// Current date at startup time

EXPORT_C_SECDB long
		SecDbGetValueDepth;					// Current SecDbGetValueWithArgs depth


EXPORT_C_SECDB SDB_VALUE_TYPE					// Pre-defined value types:
		SecDbValueClassValueFunc,			//	 Class Value Func
		SecDbValueCurrentDate,				//   Current date
		SecDbValueDiddleScope,				//	 Diddle Scope
		SecDbValueObjectValueFunc,			//	 Object Value Func
		SecDbValuePricingDate,				//   Pricing date
		SecDbValueSecurityData,				//	 Binary data for security
		SecDbValueSecurityName,				//   Name of the security
		SecDbValueSecurityType,				//   Type of the security
		SecDbValueTimeModified,				//   Time security was modified
		SecDbValueUpdateCount,				//	 Update count of object
		SecDbValueVersion,					//   Version number of object
		SecDbValueValueFunc,				//   Value Func of Node
		SecDbValueValueTypeArgs;			//   Value Type Args of Node


EXPORT_C_SECDB DT_DATA_TYPE			 			// Pre-defined data types:
		DtSecurity,							//   Security
		DtSecurityList,						//	 Security List
		DtValueType,						//	 Value Type
		DtSlang,							//   Slang tree
		DtPointer,							//   Pointer
		DtDatabase,							//   Database handle
		DtDiddleScope,						//	 Diddle Scope handle
		DtDiddleScopeList,					//	 Diddle Scope List
		DtValueArray,						//	 Value Array (for Vector Diddles)
		DtEach,								//	 Each (for iterative GetValue arguments)
		DtValueTypeInfo,					//   Value Type Info
		DtEllipsis,							//	 Ellipsis (for variable arguments)
		DtGsNodeValues,                     //	 Access to 'this' node values
		DtVtApply,							//	 VtApply (for applying vt with args to an object)
		DtPassThrough,						//	 For PassThru nodes
		DtSecDbNode;						// 	 SecDb Nodes


/*
**	Prototype functions
*/

EXPORT_C_SECDB SDB_DB
		*SecDbStartup(				const SDB_FULL_DB_NAME *DbName, int DeadPoolSize, SDB_MSG_FUNC MsgFunc, SDB_MSG_FUNC ErrMsgFunc, const char *ApplicationName, const char *UserName ),
		*SecDbAttach(				const SDB_FULL_DB_NAME *DbName, int DeadPoolSize, SDB_ATTACH_FLAGS Flags ),
		*SecDbDiddleScopeNew(		const char *DiddleScopeName ),
		*SecDbDbUpdateGet(			SDB_DB	*pDb, SDB_SEC_TYPE ClassId ),
		*SecDbDefaultDbUpdateGet(	SDB_DB *pDb ),
		*SecDbSourceDbGet(),
		*SecDbSourceDbSet(			const char *SourceDbName ),
		*SecDbLookupDb(				SDB_DB_ID DbID );

EXPORT_C_SECDB SDB_DB
		*SecDbGetRootDatabase();

EXPORT_C_SECDB SDB_DB
		*SecDbSetRootDatabase(		SDB_DB *Db );

EXPORT_C_SECDB int
		SecDbInitialize(			SDB_MSG_FUNC MsgFunc, SDB_MSG_FUNC ErrMsgFunc, const char *ApplicationName, const char *UserName ),
		SecDbDetach(				SDB_DB *Db );

EXPORT_C_SECDB void
		SecDbNoMessage(				const char *Msg );

EXPORT_C_SECDB int
		SecDbSetTimeout(			SDB_DB *Db, int Timeout );

EXPORT_C_SECDB int
		SecDbSetRetryCommits(		SDB_DB *Db, int AllowRetries );

EXPORT_C_SECDB void
		SecDbShutdown(				int FastAndLoose );

EXPORT_C_SECDB int
		SecDbDbValidate(			SDB_DB *Db ),
		SecDbDbInfoFromName(		const SDB_FULL_DB_NAME *DbName, SDB_DB_INFO *DbInfo ),
 		SecDbDbInfoFromID(			SDB_DB_ID DbID, SDB_DB_INFO *DbInfo ),
		SecDbDbInfoFromDb(			SDB_DB *Db, SDB_DB_INFO *DbInfo ),
		SecDbDbReload(				SDB_DB *Db, unsigned Flags );

EXPORT_C_SECDB SDB_DB_INFO
		*SecDbDbInfoEnumFirst(		SDB_ENUM_PTR *EnumPtr ),
		*SecDbDbInfoEnumNext(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB void
		SecDbDbInfoEnumClose(		SDB_ENUM_PTR EnumPtr ),
		SecDbObjectEnumClose(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB SDB_OBJECT
		*SecDbGetFromSyncPoint(		SDB_FULL_SEC_NAME const* SecName, SDB_DB* Db, int SyncPointOffset ),
		*SecDbIncrementReference(	SDB_OBJECT* SecPtr ),
		*SecDbObjectEnumFirst(		SDB_DB* pDb, SDB_ENUM_PTR* EnumPtr ),
		*SecDbObjectEnumNext(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB SDB_OBJECT
		*SecDbNew(					SDB_FULL_SEC_NAME const* SecName, SDB_SEC_TYPE Type, SDB_DB* Db ),
		*SecDbNewByClass(			SDB_FULL_SEC_NAME const* SecName, SDB_CLASS_INFO* Class, SDB_DB* Db ),
		*SecDbGetByNameAndClass(	SDB_FULL_SEC_NAME const* SecName, SDB_SEC_TYPE Type, SDB_DB* Db, unsigned Flags ),
		*SecDbGetByNameAndClassName(SDB_FULL_SEC_NAME const* SecName, char const* ClassName, SDB_DB* Db, unsigned Flags ),
		*SecDbGetByName(			SDB_FULL_SEC_NAME const* SecName, SDB_DB* Db, unsigned Flags, SDB_DB* pDbPhysical = 0 );

EXPORT_C_SECDB int
		SecDbNewLoad(				SDB_OBJECT* SecPtr, SDB_STORED_OBJECT* StoredObj ),
		SecDbAdd(					SDB_OBJECT* SecPtr ),
		SecDbAddWithFlags(		   	SDB_OBJECT* SecPtr, int NoSideEffects, int AddFlags ),
		SecDbSelfDescAdd(			SDB_OBJECT* SecPtr ),
		SecDbUpdate(				SDB_OBJECT* SecPtr ),
		SecDbUpdateIncremental(		SDB_OBJECT* SecPtr, unsigned int MemSize, void* Mem ),
		SecDbReload(				SDB_OBJECT* SecPtr ),
		SecDbRename(				SDB_OBJECT* SecPtr, SDB_SEC_NAME const* NewName ),
		SecDbRenameWithFlags(		SDB_OBJECT* SecPtr, SDB_SEC_NAME const* NewName, int CacheOnlyInferredName ),
		SecDbInferredName(			SDB_OBJECT* SecPtr, SDB_SEC_NAME* SecName ),
		SecDbInferredNameWithFlags(	SDB_OBJECT* SecPtr, SDB_SEC_NAME* SecName, int CacheOnlyInferredName ),
		SecDbGetByInference(		SDB_OBJECT**SecPtr, int CreateFlag ),
		SecDbGetMany(				SDB_DB* Db, unsigned Flags, DT_VALUE* SecNameArray, DT_VALUE* RetSecList, int PassErrors ),
		SecDbGetManyByName(			SDB_DB* Db, unsigned Flags, SDB_SEC_NAME const** SecNames, int Count, SDB_OBJECT** SecPtrs ),
		SecDbCopyByName(			SDB_FULL_SEC_NAME const* SecName, SDB_DB* SrcDb, SDB_DB* DstDb, SDB_FULL_SEC_NAME const* NewName, int AddFlags ),
		SecDbDeleteByName(			SDB_FULL_SEC_NAME const* SecName, SDB_DB* Db, int NoLoadFlag ),
		SecDbNameLookup(			SDB_SEC_NAME* SecName, SDB_SEC_TYPE SecType, int GetType, SDB_DB* Db ),
		SecDbNameLookupMany(		SDB_SEC_NAME const* SecName, SDB_SEC_NAME_ENTRY* SecNames, int TableSize, SDB_SEC_TYPE SecType, int GetType, SDB_DB* Db ),
		SecDbNameLookupManySecTypes(SDB_SEC_NAME const* SecName, SDB_SEC_NAME_ENTRY* SecNames, int TableSize, SDB_SEC_TYPE* SecTypes, int NumSecTypes, int GetType, SDB_DB* Db ),
		SecDbNameLookupSecTypes(	SDB_SEC_NAME* SecName, SDB_SEC_TYPE* SecTypes, int NumSecTypes, int GetType, SDB_DB* Db ),
		SecDbNameUsed(				SDB_SEC_NAME const* SecName, SDB_DB* Db ),
		SecDbRemoveFromDeadPool(	SDB_DB* Db, unsigned Flags, SDB_FULL_SEC_NAME const* SecName ),
		SecDbGetDiskInfo(			SDB_DISK* DiskInfo, SDB_FULL_SEC_NAME const* SecName, SDB_DB* Db, unsigned Flags ),
		SecDbDiskInfoGetVersion(	SDB_DISK const* DiskInfo, SDB_DISK_VERSION* Version ),
		SecDbDiskInfoSetVersion(	SDB_DISK *DiskInfo, SDB_DISK_VERSION const* Version ),
		SecDbDiskInfoToStructure(	SDB_DISK const* SdbDisk, DT_VALUE* Struct ),
		SecDbDbInfoToStructure(		SDB_DB_INFO const* DbInfo, DT_VALUE* Struct ),
		SecDbClassInfoToStructure(	SDB_CLASS_INFO const* ClassInfo, DT_VALUE* Struct );

EXPORT_C_SECDB void
		SecDbFree(					SDB_OBJECT* SecPtr );

EXPORT_C_SECDB int
		SecDbGetDiskRecord(			SDB_STORED_OBJECT* DiskObj, SDB_FULL_SEC_NAME const* SecName, SDB_DB* Db );

EXPORT_C_SECDB void
		SecDbFreeDiskRecord(		SDB_STORED_OBJECT* DiskObj );

EXPORT_C_SECDB DT_VALUE		
		*SecDbGetValue(				SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType ),
		*SecDbGetValueWithArgs(		SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE** Argv, DT_DATA_TYPE DataType );

EXPORT_CPP_SECDB DT_VALUE
		*SecDbSubscribeValue(		SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc = 0, DT_VALUE** Argv = 0, DT_DATA_TYPE DataType = 0, bool DoNotSubscribe = false );

EXPORT_C_SECDB int
		SecDbSetValue(				SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, DT_VALUE* NewValue, SDB_SET_FLAGS Flags ),
		SecDbSetValueWithArgs(		SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE** Argv, DT_VALUE* NewValue, SDB_SET_FLAGS Flags ),
		SecDbInvalidate(			SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE** Argv );

EXPORT_C_SECDB SDB_SET_FLAGS
		SecDbGetFlags(				SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE** Argv );

EXPORT_C_SECDB SDB_DIDDLE_ID
		SecDbSetDiddle(				SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, DT_VALUE* NewValue, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleID ),
		SecDbSetDiddleWithArgs(		SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE** Argv, DT_VALUE* NewValue, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleID );

EXPORT_C_SECDB SDB_DIDDLE_ID
		SecDbChangeDiddle(			SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, DT_VALUE* NewValue, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleID ),
		SecDbChangeDiddleWithArgs(	SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE** Argv, DT_VALUE* NewValue, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleID );

EXPORT_C_SECDB bool
		SecDbRemoveDiddle(			SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, SDB_DIDDLE_ID DiddleID ),
		SecDbRemoveDiddleWithArgs(	SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE** Argv, SDB_DIDDLE_ID DiddleID );
		
EXPORT_C_SECDB int
		SecDbIsEqual(				SDB_OBJECT* SecPtr1, SDB_OBJECT* SecPtr2, SDB_VALUE_FLAGS ValueFlags ),
		SecDbDuplicate(				SDB_OBJECT* TgtSecPtr, SDB_OBJECT* SrcSecPtr, SDB_VALUE_FLAGS ValueFlags, SDB_SET_FLAGS SetFlags, int SkipSecurities = FALSE, int IgnoreSavedSecurities = FALSE ),
		SecDbDuplicateCache(        SDB_OBJECT *TgtSecPtr, SDB_OBJECT *SrcSecPtr, SDB_SET_FLAGS CacheFlags, SDB_SET_FLAGS SetFlags, int SkipSecurities = FALSE, int IgnoreSavedSecurities = FALSE );



EXPORT_C_SECDB SDB_UNIQUE_ID
		SecDbUniqueID(				SDB_DB* Db );

EXPORT_C_SECDB SDB_CHILD_INFO
		*SecDbChildEnumFirst(		SDB_ENUM_PTR* EnumPtr, SDB_OBJECT* SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE** Argv, SDB_CHILD_ENUM_FILTER* Filter, bool Sorted ),
		*SecDbChildEnumNext(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB void
		SecDbChildEnumClose(		SDB_ENUM_PTR EnumPtr ),
		SecDbValueTypeEnumClose(	SDB_ENUM_PTR EnumPtr ),
		SecDbDataTypeEnumClose(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB SDB_VALUE_TYPE_INFO
		*SecDbValueTypeEnumFirst( 	SDB_OBJECT* SecPtr, SDB_ENUM_PTR* EnumPtr ),
		*SecDbValueTypeEnumReset(	SDB_ENUM_PTR EnumPtr ),
		*SecDbValueTypeEnumNext(	SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB SDB_VALUE_TYPE
		SecDbValueTypeFromName(		char const* ValueName, DT_DATA_TYPE DataType );

EXPORT_C_SECDB SDB_VALUE_FLAGS
		SecDbValueFlags(			SDB_SEC_TYPE SecType, SDB_VALUE_TYPE ValueType );

EXPORT_C_SECDB SDB_SEC_TYPE
		SecDbClassTypeFromName(		char const* ClassName );

EXPORT_C_SECDB DT_DATA_TYPE
		SecDbDataTypeEnumFirst(		SDB_ENUM_PTR* EnumPtr ),
		SecDbDataTypeEnumNext(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB const char
		*SecDbValueNameFromType(	SDB_VALUE_TYPE ValueType ),
		*SecDbClassNameFromType(	SDB_SEC_TYPE Type );

EXPORT_C_SECDB size_t
		SecDbValueTypeCount();	// RAA Added 14-9-00

EXPORT_C_SECDB size_t
		SecDbValueTypeCount();	// RAA Added 14-9-00

EXPORT_C_SECDB int
		SecDbClassTypesFromNameList( char const* NameList, char const Delimitter, SDB_SEC_TYPE* SecTypes, int* NumSecTypes );

EXPORT_C_SECDB int
		SecDbStatistics(			SDB_STATISTICS* Stats, SDB_DB* Db ),
		SecDbMemoryUsage(			DT_VALUE* ResultsStructure );

EXPORT_C_SECDB int
		SecDbLoadObjTable(			char const* Path, char const* TableName ),
		SecDbLoadDatabaseTable(		char const* Path, char const* TableName );

EXPORT_C_SECDB SDB_CLASS_INFO
		*SecDbClassInfoFromType( 	SDB_SEC_TYPE Type ),
		*SecDbClassInfoLookup(		SDB_SEC_TYPE Type ),
		*SecDbClassEnumFirst(		SDB_ENUM_PTR* EnumPtr ),
		*SecDbClassEnumNext(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB void
		SecDbClassEnumClose(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB SDB_VALIDATE
		*SecDbValidateEnumFirst(	SDB_OBJECT* SecPtr, SDB_ENUM_PTR* EnumPtr ),
		*SecDbValidateEnumNext(		SDB_ENUM_PTR EnumPtr ),
		*SecDbValidateEnumRewind(	SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB void
		SecDbValidateEnumClose(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB SDB_OBJECT_CHECK_SUM
		*SecDbObjectCheckSum(		SDB_FULL_SEC_NAME const* SecName, SDB_DB* Db );

EXPORT_C_SECDB void
		SecDbObjectCheckSumFree(	SDB_OBJECT_CHECK_SUM* CheckSum );
		
EXPORT_C_SECDB int
		SecDbSearchPathDelete(		SDB_DB* ParentDb, SDB_DB* ChildDb );
		
EXPORT_C_SECDB SDB_DB
		*SecDbSearchPathAppend(		SDB_DB* ParentDb, SDB_QUALIFIED_DB_NAME const* ChildName );

EXPORT_C_SECDB SDB_DB
		**SecDbSearchPathSet(		SDB_DB* ParentDb, SDB_QUALIFIED_DB_NAME const** ChildNames ),
		**SecDbSearchPathGet(		SDB_DB* ParentDb );

EXPORT_C_SECDB char const*
		SecDbConfigurationGet(		char const* ConfigVariable, char const* ConfigFile, char const* ConfigPath, char const* Default );


EXPORT_C_SECDB int 
		SecDbGlobalSymLinkCreate( char const* LinkName, char const* TargetName );

EXPORT_C_SECDB char const*
		SecDbGlobalSymLinkResolve(	char const* LinkName );

EXPORT_C_SECDB int
		DbObjFuncSecDbBase(			SDB_OBJECT* Secptr, int Msg, SDB_M_DATA* MsgData ),
		DbObjFuncSecDbBaseInterface(SDB_OBJECT* Secptr, int Msg, SDB_M_DATA* MsgData );

EXPORT_C_SECDB int
		SecDbReclassify(   	        SDB_OBJECT* SecPtr,	SDB_SEC_TYPE Type );

EXPORT_C_SECDB int
		SecDbInGetValue();


EXPORT_C_SECDB int 
	SecDbIndexGetNoLoadFlag();

EXPORT_C_SECDB void 
	SecDbIndexSetNoLoadFlag( int iFlag );


EXPORT_C_SECDB int
		SecDbGetSecurityNamesInRange( char const* SecDbClassName, char const* Start, char const* End, DT_VALUE* ResultArray, int* NumFound, SDB_DB* Db );		

EXPORT_C_SECDB DT_VALUE* 
		SecDbGetLoadHistory();

// FIX- temporary API for OS/2
EXPORT_C_SECDB void
		SecDbClassMarkAsNotStarted( SDB_CLASS_INFO* Class );

// Hash the instreams of a sec.
EXPORT_C_SECDB int SecDbHashInStreams( SDB_OBJECT *SecPtr, DT_HASH_CODE	*Hash );

// convert a structure to a stored object
EXPORT_C_SECDB bool SecDbStoredObjectFromStructure( SDB_STORED_OBJECT*, DT_VALUE* );

// Replicate FOREACH_CLASS_VTI macros to make transition easy, new code should *not* use these
// just treat the map as standard STL map

#define FOREACH_CLASS_VTI_BEGIN( _map, _pvti )													\
{																								\
	for( SecDbValueTypeMap::const_iterator it_ = (_map)->begin(); it_ != (_map)->end(); ++it_ )	\
	{																							\
		(_pvti) = const_cast<SDB_VALUE_TYPE_INFO*>( it_->second );

#define FOREACH_CLASS_VTI_END()					\
    }											\
}

#endif
