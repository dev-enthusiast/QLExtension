/****************************************************************
**
**	SDBCHILD.H	- SecDb Child Data structures
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdbchild.h,v 1.24 2001/08/22 20:22:39 singhki Exp $
**
****************************************************************/

#ifndef IN_SECDB_SDBCHILD_H
#define IN_SECDB_SDBCHILD_H

#include <string.h>
// for export macros
#include <sdb_base.h>
#if !defined( IN_SECDB_H )
#include <secdb.h>
#endif

/*
** SDB_CHILD_ITEM_TYPE
*/

typedef int SDB_CHILD_ITEM_TYPE;

// Child Component types for Db/Security/Vt and args

int const SDB_CHILD_LIST_SELF_DB        = 0x00;  // Current SecPtr or ValueType or Db
int const SDB_CHILD_LIST_LITERAL        = 0x01;  // Get from ChildList.Literals
int const SDB_CHILD_LIST_CONSTANT       = 0x02;  // Just use ChildList.Literals directly
int const SDB_CHILD_LIST_ARG_OR_NULL    = 0x03;  // Get from arg list, but if missing or DtNull, use DtNull
int const SDB_CHILD_LIST_CAST           = 0x04;  // Cast according to ChildList.Casts[ Element ]
int const SDB_CHILD_LIST_SELF_OBJ       = 0x05;  // Current SecPtr
int const SDB_CHILD_LIST_SELF_VT        = 0x06;  // Current ValueType
int const SDB_CHILD_LIST_DB_SUFFIX_SELF = 0x07;  // Suffix Db of node
int const SDB_CHILD_LIST_DB_SUFFIX_INT  = 0x08;  // Suffix Db of int
int const SDB_CHILD_LIST_GET_ITEM_VT    = 0x09;  // In order to get this child, just do a GetItem on Item->ValueType
int const SDB_CHILD_LIST_NO_PREDICATE   = 0x0a;  // No Predicate

// Child Component types for Results

int const SDB_CHILD_LIST_RESULT_TYPES	= 0x40;

int const SDB_CHILD_LIST_TERMINAL       = 0x41;  // Put to final childlist for value func
int const SDB_CHILD_LIST_ARGUMENT       = 0x42;  // Get from my argument list
int const SDB_CHILD_LIST_ELLIPSIS       = 0x43;  // Get all remaining arguments from my argument list
int const SDB_CHILD_LIST_ELLIPSIS_SIZE  = 0x44;  // Size( Ellipsis() )
int const SDB_CHILD_LIST_DIDDLE_SCOPE   = 0x45;  // DiddleScope( Self, SelfVt, Arg1, ... )
int const SDB_CHILD_LIST_INTERMEDIATE   = 0x46;  // Get from/put to some prior child

struct SDB_CHILD_COMPONENT
{
	SDB_CHILD_COMPONENT()
	{
		Reset();
	}

	void Reset()
	{
		memset( this, 0, sizeof( *this ));
	}

	bool equal( SDB_CHILD_COMPONENT const& Comp1 ) const
	{
		return Type == Comp1.Type && NoEval == Comp1.NoEval && Element == Comp1.Element;
	}

	unsigned char
			Type;						// Source type of child component
	bool    NoEval;						// Eval or not
	short	Element;					// Element number of source array
};

inline bool operator==( SDB_CHILD_COMPONENT const& lhs, SDB_CHILD_COMPONENT const& rhs )
{
	return lhs.equal( rhs );
}

inline bool operator!=( SDB_CHILD_COMPONENT const& lhs, SDB_CHILD_COMPONENT const& rhs )
{
	return !lhs.equal( rhs );
}


struct SDB_CHILD_LIST_ITEM
{
	SDB_CHILD_LIST_ITEM()
	{
		Reset();
	}

	void Reset()
	{
		memset( this, 0, sizeof( *this ));

		Db.Type = SDB_CHILD_LIST_SELF_DB;
		Pred.Type = SDB_CHILD_LIST_NO_PREDICATE;
		Security.Type = SDB_CHILD_LIST_SELF_OBJ;
		ValueType.Type = SDB_CHILD_LIST_SELF_VT;
	}

	SDB_CHILD_COMPONENT
			Db,					// Instructions for getting the Db
			Security,			// Instructions for getting security object
			ValueType,			// Instructions for getting value type
			Result,				// Where to put results
	        Pred;               // Reference to Predicate (may be NO_PREDICATE)

	unsigned int
			Argc        : 16,  		// Number of arguments
            ArrayReturn : 1,		// return dynamic pointers/numbers as arrays instead
			PassErrors  : 1,		// Pass errors arg to non terminal
	        PredFlags   : 1,        // True predicate
	        AsObject    : 1;        // If this child is being used as an object

	SDB_CHILD_COMPONENT
			*Argv;				// Instructions for getting arguments

	const char
			*Name;				// Name of child or arg

};

struct SDB_CHILD_PULL_VALUE
{
	SDB_CHILD_PULL_VALUE
			*Next;				// Next pull value item
			
	int		ChildNumber;		// Which child

	int		Offset;				// Byte offset into results
	
	DT_DATA_TYPE
			*DataType;

};

struct SDB_CHILD_CAST
{
	SDB_CHILD_COMPONENT
			Desc;

	DT_DATA_TYPE
			DataType;

	SDB_CHILD_CAST()
	{
		memset( this, 0, sizeof( *this ));
	}

	bool equal( SDB_CHILD_CAST const& Cast1 ) const
	{
		return Desc == Cast1.Desc && DataType == Cast1.DataType;
	}
};

inline bool operator==( SDB_CHILD_CAST const& lhs, SDB_CHILD_CAST const& rhs )
{
	return lhs.equal( rhs );
}

inline bool operator!=( SDB_CHILD_CAST const& lhs, SDB_CHILD_CAST const& rhs )
{
	return !lhs.equal( rhs );
}


struct EXPORT_CLASS_SECDB SDB_CHILD_LIST
{
	SDB_CHILD_LIST_ITEM
			*Terminals,					// first n are default values for arguments
			*Intermediates;				// intermediate children

	SDB_NODE
			**Literals;			// Literal values used by child items

	SDB_CHILD_CAST
			*Casts;				// Cast

	SDB_CHILD_PULL_VALUE
			*PullValues;		// List of details of how to pull values into ChildData

	int		ChildDataSize;		// Total number of bytes in pull values result

	short
			ArgChildCount,		// Number of children which are default values for args
			TerminalCount,		// Number of children eventually used by value function
			IntermediateCount,	// Number of children used to build other children
			LiteralCount,		// Number of nodes in Literals
			CastCount;			// Number of casts in Casts

	bool	PassErrors,			// Do not fail if fail to get children
			ForceExport,		// Allow non-exported children
			ArgEllipsis,		// Have variable number of args
			ChildListValid,
 	        CycleIsNotFatal,    // make cycle error non-fatal so it can be PassError_ed
			DynamicChildList;	// child list is generated by value function

	void Free();

	int Copy( SDB_CHILD_LIST& Source );
	int Copy( SDB_M_GET_CHILD_LIST* ChildList );

	SDB_CHILD_LIST_ITEM* CopyItems( SDB_CHILD_LIST_ITEM* Items, int Count );

	void LiteralLink( SDB_NODE* Literal );
	void LiteralUnlink( SDB_NODE* Literal );

};


/*
**	Intermediate Child list structure use while sending the message to the VF
*/

struct EXPORT_CLASS_SECDB SDB_M_GET_CHILD_LIST
{
	SDB_VALUE_TYPE_INFO
			*Vti;

	short	CastAdd( SDB_CHILD_CAST *Cast );

	short	LiteralAdd( DT_VALUE *LiteralValue, DT_DATA_TYPE ExpectedType );

	short	ChildAdd( SDB_CHILD_LIST_ITEM *Child, int TerminalOrIntermediate );

	bool	CheckDesc( SDB_CHILD_COMPONENT *Desc, bool NoEval );

	bool	handle_string_objects( SDB_CHILD_LIST_ITEM* child );

    SDB_CHILD_LIST_ITEM*
	        FindChild( SDB_CHILD_COMPONENT const& );

	static int
			Initialize();

	static short
			MaxTerminals;

	static short
			MaxIntermediates;

	static short
			MaxCasts;

	static short
			MaxLiterals;

	static SDB_CHILD_LIST_ITEM
			*Terminals;

	static SDB_CHILD_LIST_ITEM
			*Intermediates;

	static SDB_NODE
			**Literals;

	static SDB_CHILD_CAST
			*Casts;

	enum StateType {
		Uninitialized,
		Initialized,
		GettingChildList
	};

    static StateType
	        State;
};


struct SDB_CHILD_INFO
{
	SDB_OBJECT
			*SecPtr;	// Pointer to child security
			
	SDB_VALUE_TYPE
			ValueType;	// Valuetype on child security
			
	int		Argc;		// Arg count (0 if no args)
	
	DT_VALUE
			*Argv;		// Arguments (NULL if no args)

	SDB_NODE
			*Node;		// Associated node

    SDB_DBSET
		    *DbSet;     // Records db set this node lives in

};


/*
**	Structure used to describe value types
*/

struct SDB_VALUE_TYPE_INFO
{
	const char
			*Name;			// Name of value type

	DT_DATA_TYPE
			*DataType;		// Pointer to data type of value

	SDB_VALUE_FUNC
			Func;			// Message function for given value type

	const void	*ValueData;		// Value Type specific data

	SDB_VALUE_FLAGS
			ValueFlags;

	SDB_CHILD_LIST
			ChildList;		// Child list for this value type

	void	*Data;			// Object-supplied data

	SDB_VALUE_TYPE
			ValueType;

	SDB_VALUE_TYPE_INFO
			*Parent;		// Vti from which inherited

	SDB_CLASS_INFO
			*Class;			// Class which we belong to (NULL if this is an instance VTI)

};


/*
**	Datatype wrapper around SDB_VALUE_TYPE_INFO
*/
struct DT_VALUE_TYPE_INFO
{
	SDB_VALUE_TYPE_INFO
			*Vti;		// The Vti

	int		Refcount,	// Number of references to this struct
			Free;		// Free Vti when Refcount drops to 0

};

// lame Sun compiler can't deal with constant structures
#define SDB_CHILD_LIST_NULL { NULL, NULL, NULL, NULL, NULL, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0 }

#endif 
