#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_enum.c,v 1.97 2013/02/19 21:52:08 c06121 Exp $"
/****************************************************************
**
**	SDB_ENUM.C	- Security database child enumeration functions,
**				  and value type enumeration functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_enum.c,v $
**	Revision 1.97  2013/02/19 21:52:08  c06121
**
**	Check size of child array to prevent indexing off end of array.
**	AWR: #280245
**
**	Revision 1.96  2001/11/27 23:23:35  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.94  2001/09/27 20:55:20  singhki
**	a none filter to force graph to be built but not bother returning children
**	
**	Revision 1.93  2001/08/15 15:45:39  simpsk
**	remove deprecated #includes.
**	
**	Revision 1.92  2001/08/07 01:15:52  singhki
**	expand and print expression children properly
**	
**	Revision 1.91  2001/07/17 16:58:36  singhki
**	remove unused var
**	
**	Revision 1.90  2001/06/12 14:45:59  shahia
**	make ALT-F4 VT eval more friendly
**	
**	Revision 1.89  2001/05/23 21:03:52  simpsk
**	add printing of predicates to child list items.
**	
**	Revision 1.88  2001/05/08 01:08:50  simpsk
**	simplify predicate printing.
**	
**	Revision 1.87  2001/04/11 22:25:04  simpsk
**	Add NO_DYNAMIC_PTRS switch.
**	
**	Revision 1.86  2001/04/05 11:35:31  shahia
**	take out the SECDB_CHECK_NODE_CHILDREN_DISABLE flag
**	
**	Revision 1.85  2001/03/23 23:26:39  singhki
**	Make ValueTypeMap just an STL map.
**	
**	Revision 1.84  2001/03/13 23:48:30  simpsk
**	Fixed sign compare warning.
**	
**	Revision 1.83  2001/03/13 23:45:49  simpsk
**	Minor cleanups.  Scope variables.
**	
**	Revision 1.82  2001/03/01 17:34:23  goodfj
**	Use ConvertIntermediates arg to WriteChildComponent
**	
**	Revision 1.81  2001/03/01 14:45:32  goodfj
**	Predicates appear more tersely in child liist description
**	
**	Revision 1.80  2001/02/22 12:32:21  shahia
**	changed ValueTypeMap implementation to use STL map
**	
**	Revision 1.79  2001/02/06 16:37:34  singhki
**	Ask each node for its ExpectedChildDb during ForChildren because if we
**	need to build the graph we need to keep track of which Db to build it
**	in.
**	
**	This all really sucks we should write generic graph traveral code
**	which can then be used by getvalue, forchildren and whoever else needs
**	to traverse the graph. This approach just leads to duplicated code.
**	
**	Revision 1.78  2001/01/23 23:02:48  singhki
**	Added flag CycleIsNotFatal
**	
**	Revision 1.77  2001/01/11 22:21:47  singhki
**	pass SecDbRootDb as arg to NodeChildren to indicate Db we are
**	interested in.
**	Also reorganize retry look such that it first checks if the failure
**	was due to the DbSet being different.
**	
**	Revision 1.76  2000/12/19 17:20:43  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.75  2000/12/12 17:39:01  simpsk
**	Predicated children.
**	
**	GOB_ARGs have been mostly reworked.
**	See example in Dev DB "KEN: predchild".
**	
**	Revision 1.74  2000/11/20 12:22:05  shahia
**	use SecDbConfigurationGet instead of getenv
**	
**	Revision 1.73  2000/11/17 12:16:21  shahia
**	Make ForChildren bug fix, condtioned on env var
**	
**	Revision 1.72  2000/11/16 11:28:09  shahia
**	Limit the num retries to get the correct children, if splits occur
**	
**	Revision 1.71  2000/10/31 18:04:44  shahia
**	Bug Fix: ForChildern could return wrong set if an split occurs and Db is removed from its DB set
**	
**	Revision 1.70  2000/08/23 01:32:51  singhki
**	remove sanity check since it may hide real errors
**	
**	Revision 1.69  2000/08/16 14:44:56  shahia
**	Fix: to prevent crash during ForChildren
**	
**	Revision 1.68  2000/06/06 18:12:34  singhki
**	BELOW_SET applies to sets and diddles
**	
**	Revision 1.67  2000/05/16 10:18:02  goodfj
**	Check return values
**	
**	Revision 1.66  2000/04/28 14:18:22  vengrd
**	Move ExternalVtFromVt to .c in order to remove dep upon hash.h
**	Also make External + External_Vt the default behaviour for _External
**	controllable via SECDB_FOR_CHILDREN_USE_EXTERNAL_VT
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>

#include	<sdb_enum.h>
#include    <sdb_int.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secnode.h>
#include	<secnodei.h>

#include    <mempool.h>

#include	<secexpr.h>
#include	<secdb/slang_util.h>

#include <ios>
CC_USING( std::ios );

#include <ostream>
CC_USING( std::ostream );

#include <sstream>
CC_USING( std::ostringstream );

#include <string>
CC_USING( std::string );

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern CONFIGURATION 	*SecDbConfiguration;



/****************************************************************
**	Routine: ChildComponent
**	Returns: Nothing
**	Action :
****************************************************************/

static void ChildComponent(
	DT_VALUE				*CompValue,
	SDB_CHILD_COMPONENT		*Comp
)
{
	const char
			*TypeName;


	switch( Comp->Type )
	{
		case SDB_CHILD_LIST_SELF_OBJ:
		case SDB_CHILD_LIST_SELF_VT:
		case SDB_CHILD_LIST_SELF_DB:			TypeName = "SELF";				break;
		case SDB_CHILD_LIST_LITERAL:			TypeName = "LITERAL";			break;
		case SDB_CHILD_LIST_ARGUMENT:			TypeName = "ARGUMENT";			break;
		case SDB_CHILD_LIST_INTERMEDIATE:		TypeName = "INTERMEDIATE";		break;
		case SDB_CHILD_LIST_TERMINAL:			TypeName = "TERMINAL";			break;
		case SDB_CHILD_LIST_CONSTANT:			TypeName = "CONSTANT";			break;
		case SDB_CHILD_LIST_ARG_OR_NULL:		TypeName = "ARG_OR_NULL";		break;
		case SDB_CHILD_LIST_CAST:				TypeName = "CAST";				break;
		case SDB_CHILD_LIST_ELLIPSIS:			TypeName = "...";				break;
		case SDB_CHILD_LIST_ELLIPSIS_SIZE:		TypeName = "Size( ... )";		break;
		case SDB_CHILD_LIST_DIDDLE_SCOPE:		TypeName = "Diddle Scope( Self )";break;
		default:								TypeName = "Unknown";			break;
	}
	DtComponentSetPointer( CompValue, "Type", DtString, TypeName );
	DtComponentSetNumber( CompValue, "Element",	DtDouble, Comp->Element );
	if( Comp->NoEval )
		DtComponentSetNumber( CompValue, "NoEval", DtDouble, 1 );
}


/****************************************************************
**	Routine: CompareChildListItems
**	Returns: 
**	Action : 
****************************************************************/

static bool CompareChildListItems(
	SDB_CHILD_LIST_ITEM	*a,
	SDB_CHILD_LIST_ITEM	*b
)
{
	if( a->Db != b->Db || a->Security != b->Security || a->ValueType != b->ValueType || a->Argc != b->Argc )
		return false;

	for( unsigned int i = 0; i < a->Argc; ++i )
		if( a->Argv[i] != b->Argv[i] )
			return false;

	return true;
}

static long& child_level(ios& stream)
{
    static int child_level_flag = ios::xalloc(); // we are guaranteed this to be 0 when we alloc it.
	return stream.iword( child_level_flag );
}

class UpChildLevel
{
public:
    explicit UpChildLevel( ios& stream, int flag = 1 ) :
	    m_stream( stream ),
		m_old_flag( child_level( stream ))
    {
	    child_level( m_stream ) = m_old_flag + flag;
	}

    ~UpChildLevel()
    {
	    child_level( m_stream ) = m_old_flag;
	}

private:
    ios& m_stream;
    long const      m_old_flag;
};

/****************************************************************
**	Routine: WriteChildComponent
**	Returns: BytesUsed
**	Action : Writes description of a child component
****************************************************************/

static void WriteChildComponent(
	ostream&            out,
	SDB_CHILD_COMPONENT	*Component,
	SDB_CHILD_LIST		*ChildList
)
{
    UpChildLevel u1( out );

	switch( Component->Type )
	{
		case SDB_CHILD_LIST_SELF_VT:
		case SDB_CHILD_LIST_SELF_OBJ:
		case SDB_CHILD_LIST_SELF_DB:
		    out << "Self";
			break;

		case SDB_CHILD_LIST_LITERAL:
		{
		    DT_VALUE
			        TempValue;

			TempValue.DataType = DtString;
			if( DTM_TO( &TempValue, &ChildList->Literals[ Component->Element ]->m_Value ))
			{
			    out << (char const*) TempValue.Data.Pointer;
				DTM_FREE( &TempValue );
			}
			break;
		}

		case SDB_CHILD_LIST_ARGUMENT:
		case SDB_CHILD_LIST_ARG_OR_NULL:
		{
			SDB_CHILD_LIST_ITEM
					*SubItem;

			if( Component->Element < ChildList->ArgChildCount && ( SubItem = &ChildList->Terminals[ Component->Element ])->Name )
			{
			    out << SubItem->Name;
			}
			else
			{
			    out << "Argv[" << Component->Element << "]";
			}

			if( Component->Type == SDB_CHILD_LIST_ARG_OR_NULL )
			    out << "/NULL";

			break;
		}

		case SDB_CHILD_LIST_CAST:
		    out << ChildList->Casts[ Component->Element ].DataType->Name << "( ";
			WriteChildComponent( out, &ChildList->Casts[ Component->Element ].Desc, ChildList );
			out << " )";
			break;

		case SDB_CHILD_LIST_INTERMEDIATE:
			if( Component->Element < ChildList->IntermediateCount )
			{
			    SecDbChildDescription( out, &ChildList->Intermediates[ Component->Element ], ChildList );
			}
			else
			{
			    out << "<notfound>";
			}
			break;

		default:
		    out << "<error>";
	}
}

/****************************************************************
**	Routine: write_expression_child_description
**	Returns: true/false
**	Action : tries to write expr child description in a readable
**           form
****************************************************************/

static bool write_expression_child_description(
	ostream&                out,
	SDB_CHILD_LIST_ITEM		*item,		// Item to describe
	SDB_VALUE_TYPE			vt,			// vt we found
	SDB_CHILD_LIST			*child_list	// Child list item belongs to
)
{
	static SDB_CLASS_INFO* s_eval_info = SecDbClassInfoFromType( SecDbClassTypeFromName( "Slang Eval Class GOB" ) );

	SDB_VALUE_TYPE_INFO* vti;
	DT_SLANG *get_func = 0;

	SDB_M_DATA msg_data;
	DTM_INIT( &msg_data.ValueTypeData );

	if( s_eval_info
		&& ( vti = (*s_eval_info->ValueTypeMap)[ vt->ID ] )
		&& ( SDB_MESSAGE_VT_TRACE( 0, SDB_MSG_GET_VALUE_TYPE_DATA, &msg_data, vti ) )
	  )
		get_func = (DT_SLANG*) DtComponentGetPointer( &msg_data.ValueTypeData, "GET", DtSlang, 0 );

	DTM_FREE( &msg_data.ValueTypeData );
	// ok to free, get_func is still valid because it is a copy of a ref counted slang expr

	if( get_func )
	{
		SLANG_NODE* node = get_func->Expression->Argv[ get_func->Expression->Argc - 1 ];
		// skip Return for better printing;
		if( node->Type == SLANG_RESERVED_FUNC && stricmp( SLANG_NODE_STRING_VALUE( node ), "return" ) == 0 && node->Argc == 1 )
			node = node->Argv[ 0 ];

		ostringstream slang;
		SlangParseTreeToBriefString( node, 0, slang );
		string slang_str( slang.str() );

		unsigned argn;
		for( argn = 0; argn < item->Argc; ++argn )
		{
			ostringstream child_stream;
			child_stream << "ChildData[ " << argn << " ]";
			string child_str = child_stream.str();

			ostringstream tmp_child_stream;
			tmp_child_stream << "~Arg" << argn;

			size_t pos;
			while( ( pos = slang_str.find( child_str ) ) != string::npos )
				slang_str.replace( pos, child_str.size(), tmp_child_stream.str() );
		}

		for( argn = 0; argn < item->Argc; ++argn )
		{
			ostringstream tmp_child_stream;
			tmp_child_stream << "~Arg" << argn;
			string tmp_child_str = tmp_child_stream.str();

			ostringstream arg_str;
			WriteChildComponent( arg_str, &item->Argv[ argn ], child_list );

			size_t pos;
			while( ( pos = slang_str.find( tmp_child_str ) ) != string::npos )
			{
				slang_str.replace( pos, tmp_child_str.size(), arg_str.str() );
			}
		}
		out << slang_str;

		return true;
	}
	return false;
}



/****************************************************************
**	Routine: SecDbChildDescription
**	Returns: Bytes written to buffer (except null terminator)
**	Summary: String translation of child item
****************************************************************/

void SecDbChildDescription(
	ostream&                out,
	SDB_CHILD_LIST_ITEM		*Item,		// Item to describe
	SDB_CHILD_LIST			*ChildList	// Child list item belongs to
)
{
	DT_VALUE
			TempValue;

	if( Item->Name )
	{
	    int MaxNameLen = 0;

	    // FIX - admittedly silly to look through the whole childlist everytime to get the maxnamelength
	    for( int ItemNum = 0; ItemNum < ChildList->TerminalCount; ++ItemNum )
		{
		    if( ChildList->Terminals[ ItemNum ].Name )
			{
			    int const Len = strlen( ChildList->Terminals[ ItemNum ].Name );
				MaxNameLen = MAX( MaxNameLen, Len );
			}
		}

	    out << Item->Name << ": " << string( MaxNameLen - strlen( Item->Name ), ' ' );
	}
	
	
	if( Item->Result.Type == SDB_CHILD_LIST_ELLIPSIS )
	{
	    out << "Argv[" << Item->Result.Element << "]";
		return;
	}

	if( Item->Result.Type == SDB_CHILD_LIST_DIDDLE_SCOPE )
	{
	    out << "DiddleScope( Self, SelfVt, ... )";
		return;
	}

	if( Item->Result.Type == SDB_CHILD_LIST_ARGUMENT )
	    out << "Argv[" << Item->Result.Element << "], default: ";


	if( Item->Security.Type == SDB_CHILD_LIST_CONSTANT )
	{
	    out << "(" << ChildList->Literals[ Item->Security.Element ]->m_Value.DataType->Name << ")";
		TempValue.DataType = DtString;
		if( DTM_TO( &TempValue, &ChildList->Literals[ Item->Security.Element ]->m_Value ))
		{
		    out << (const char*) TempValue.Data.Pointer;
			DTM_FREE( &TempValue );
		}
		return;
	}

	// check if VT is ~Eval, if so then print string representation of expr child
	if( Item->ValueType.Type == SDB_CHILD_LIST_LITERAL )
	{
		SDB_NODE* literal_vt = ChildList->Literals[ Item->ValueType.Element ];
		SDB_VALUE_TYPE vt;
		if( literal_vt->m_Value.DataType == DtValueType
			&& ( vt = (SDB_VALUE_TYPE) literal_vt->m_Value.Data.Pointer )
			&& strnicmp( vt->Name, "~eval", 5 ) == 0
			&& write_expression_child_description( out, Item, vt, ChildList )
		  )
			return;
	}

	WriteChildComponent( out, &Item->ValueType, ChildList );

	out << "( ";

	// Write Db if present
	if( Item->Db.Type != SDB_CHILD_LIST_SELF_DB )
	{
		WriteChildComponent( out, &Item->Db, ChildList );
		out << "!";
	}

	WriteChildComponent( out, &Item->Security, ChildList );

	for( unsigned ArgNum = 0; ArgNum < Item->Argc; ++ArgNum )
	{
	    out << ", ";
		WriteChildComponent( out, &Item->Argv[ ArgNum ], ChildList );
	}

	out << " )";

	// Write Predicate if present
	if( Item->Pred.Type != SDB_CHILD_LIST_NO_PREDICATE )
	{
	    if( !child_level( out )) // only print predicate stuff for top-level children
		{
		    out << " if( ";
			if( !Item->PredFlags ) out << "!";
			WriteChildComponent( out, &Item->Pred, ChildList );
			out << " )";
		}
	}
}



/****************************************************************
**	Routine: ChildListItemsToArray
**	Returns: void
**	Action : converts SDB_CHILD_LIST_ITEMs into a DtArray
****************************************************************/

static void ChildListItemsToArray(
	DT_VALUE			*TempValue,
	DT_VALUE			*Translation,
	SDB_CHILD_LIST_ITEM	*Item,
	int					ItemCount,
	SDB_CHILD_LIST		*ChildList
)
{
    DTM_ALLOC( TempValue, DtArray );

	DT_VALUE
			SubValue,
	        CompValue;

	DTM_ALLOC( &CompValue, DtStructure );
	DTM_ALLOC( &SubValue, DtStructure );

	for( ; ItemCount > 0; ++Item, --ItemCount )
	{
		ChildComponent( &CompValue, &Item->Db );
		DtComponentSet( &SubValue, "Db", &CompValue );

		ChildComponent( &CompValue, &Item->Pred );
		DtComponentSet( &SubValue, "Predicate", &CompValue );

		DtComponentSetNumber( &SubValue, "Pred true", DtDouble, Item->PredFlags );

		ChildComponent( &CompValue, &Item->Security );
		DtComponentSet( &SubValue, "Security", &CompValue );

		ChildComponent( &CompValue, &Item->ValueType );
		DtComponentSet( &SubValue, "ValueType", &CompValue );
		
		ChildComponent( &CompValue, &Item->Result );
		DtComponentSet( &SubValue, "Result", &CompValue );
		
		DtComponentSetNumber( &SubValue, "Argc", DtDouble, Item->Argc );

		{
		    DT_VALUE
			        ArgValue;

			DTM_ALLOC( &ArgValue, DtArray );
			for( unsigned Num = 0; Num < Item->Argc; ++Num )
			{
			    ChildComponent( &CompValue, &Item->Argv[ Num ] );
				DtSubscriptSet( &ArgValue, Num, &CompValue );
			}
			DtComponentSet( &SubValue, "Argv", &ArgValue );
			DTM_FREE( &ArgValue );
		}
			
		DtComponentSetNumber( &SubValue, "AsObject", 	   DtDouble, Item->AsObject );
		
		DtComponentSetPointer( &SubValue, "Name", DtString, Item->Name ? Item->Name : "" );

		DtComponentSetNumber( &SubValue, "ArrayReturn", DtDouble, Item->ArrayReturn );
		DtComponentSetNumber( &SubValue, "PassErrors",  DtDouble, Item->PassErrors );

		DtAppend( TempValue, &SubValue );
		
		if(		Item->Result.Type == SDB_CHILD_LIST_TERMINAL
			||	Item->Result.Type == SDB_CHILD_LIST_ARGUMENT 
			||	Item->Result.Type == SDB_CHILD_LIST_ELLIPSIS
			||	Item->Result.Type == SDB_CHILD_LIST_DIDDLE_SCOPE )
		{
		    ostringstream out;
			SecDbChildDescription( out, Item, ChildList );
			DtAppendPointer( Translation, DtString, out.str().c_str() );
		}
	}
	DTM_FREE( &SubValue );
	DTM_FREE( &CompValue );
}



/****************************************************************
**	Routine: SecDbChildListInfo
**	Returns: TRUE or FALSE
**	Summary: Fills in DtStructure with child list data structures
****************************************************************/

int SecDbChildListInfo(
	SDB_OBJECT			*SecPtr,		// Pointer to security
	SDB_VALUE_TYPE		ValueType,		// Value type to get info
	SDB_VALUE_TYPE_INFO	*Vti,			// Vti to use, maybe NULL in which case we look it up
	DT_VALUE			*RetStruct		// Structure to store result in
)
{
	DT_VALUE
			TempValue,
			SubValue,
			Translation;

	if( !SecPtr || !ValueType )
		return Err( ERR_INVALID_ARGUMENTS, "SecDbChildListInfo: @" );

	if( !Vti )
	{
		SDB_NODE* const Node = SDB_NODE::FindTerminalInt( SecPtr, ValueType, 0, NULL, SDB_NODE::DO_NOT_CREATE, SecDbRootDb, SDB_NODE::SHARE );

		if( Node )
			Vti = Node->VtiGet();
		else
			Vti = SecPtr->SdbClass->LookupVti( ValueType->ID );

		if( !Vti )
			return Err( ERR_UNSUPPORTED_OPERATION, "Unsupported value type %s( %s )", ValueType->Name, SecPtr->SecData.Name );
	}
	else if( !Vti->ChildList.ChildListValid )
		return Err( ERR_UNSUPPORTED_OPERATION, "Child list is not valid %s( %s )", ValueType->Name, SecPtr->SecData.Name );

	SDB_CHILD_LIST* const ChildList = &Vti->ChildList;
	
	DTM_ALLOC( RetStruct, DtStructure );
	DTM_ALLOC( &Translation, DtArray );

	DtComponentSetPointer( RetStruct, "ValueType",			DtString, Vti->Name );
	DtComponentSetPointer( RetStruct, "SecurityType",		DtString, SecPtr->Class->Name );
	DtComponentSetPointer( RetStruct, "DataType",			DtString, Vti->ValueType->DataType->Name );

	DtComponentSetNumber( RetStruct, "TerminalCount",		DtDouble, ChildList->TerminalCount );
	DtComponentSetNumber( RetStruct, "IntermediateCount",	DtDouble, ChildList->IntermediateCount );
	DtComponentSetNumber( RetStruct, "LiteralCount",		DtDouble, ChildList->LiteralCount );
	DtComponentSetNumber( RetStruct, "CastCount",			DtDouble, ChildList->CastCount );
	DtComponentSetNumber( RetStruct, "ChildDataSize",		DtDouble, ChildList->ChildDataSize );
	DtComponentSetNumber( RetStruct, "PassErrors",			DtDouble, ChildList->PassErrors );
	DtComponentSetNumber( RetStruct, "ForceExport",			DtDouble, ChildList->ForceExport );
	DtComponentSetNumber( RetStruct, "ArgEllipsis",			DtDouble, ChildList->ArgEllipsis );
	DtComponentSetNumber( RetStruct, "CycleIsNotFatal",		DtDouble, ChildList->CycleIsNotFatal );

	ChildListItemsToArray( &TempValue, &Translation, ChildList->Terminals, ChildList->TerminalCount, ChildList );
	DtComponentSetPointer( RetStruct, "Terminals", DtArray, TempValue.Data.Pointer );

	ChildListItemsToArray( &TempValue, &Translation, ChildList->Intermediates, ChildList->IntermediateCount, ChildList );
	DtComponentSetPointer( RetStruct, "Intermediates", DtArray, TempValue.Data.Pointer );
 
	{
	    DTM_ALLOC( &TempValue, DtArray );
		{
		    for( int Num = 0; Num < ChildList->LiteralCount; ++Num )
			    DtSubscriptSet( &TempValue, Num, &ChildList->Literals[ Num ]->m_Value );
		}
		DtComponentSet( RetStruct, "Literals", &TempValue );
		DTM_FREE( &TempValue );
	}

	DT_VALUE
	        CompValue;

	DTM_ALLOC( &CompValue, DtStructure );
	{
	    DTM_ALLOC( &TempValue, DtArray );
		DTM_ALLOC( &SubValue, DtStructure );
		{
		    for( int Num = 0; Num < ChildList->CastCount; ++Num )
			{
			    ChildComponent( &CompValue, &(ChildList->Casts[Num].Desc) );
				DtComponentSet( &SubValue, "Desc", &CompValue );

				DtComponentSetPointer( &SubValue, "DataType",	DtString, ChildList->Casts[Num].DataType->Name );
				DtAppend( &TempValue, &SubValue );
			}
		}
		DtComponentSet( RetStruct, "Casts", &TempValue );
		DTM_FREE( &SubValue );
		DTM_FREE( &TempValue );
	}

	{
	    DTM_ALLOC( &TempValue, DtArray );
		DTM_ALLOC( &SubValue, DtStructure );
		{
		    for( SDB_CHILD_PULL_VALUE* PullVal = ChildList->PullValues; PullVal; PullVal = PullVal->Next )
			{
			    DtComponentSetNumber( &SubValue, "ChildNumber",	DtDouble, PullVal->ChildNumber );
				DtComponentSetNumber( &SubValue, "Offset",		DtDouble, PullVal->Offset );
				DtAppend( &TempValue, &SubValue );
			}
		}
		DtComponentSet( RetStruct, "PullValues", &TempValue );
		DTM_FREE( &TempValue );
		DTM_FREE( &SubValue );
	}

	if( SDB_NODE::ArgumentUsage( SecPtr, ValueType, Vti, &TempValue ))
	{
		DtComponentSet( RetStruct, "Arguments", &TempValue );
		DTM_FREE( &TempValue );
	}
	
	DtComponentSet( RetStruct, "Translation", &Translation );
	DTM_FREE( &Translation );
	
	return TRUE;
}

/****************************************************************
**	Routine: SecDbValueTypeUsageInfo
**	Returns: TRUE or FALSE
**	Summary: Fills in DtStructure with child list data structures
****************************************************************/

int SecDbValueTypeUsageInfo(
	SDB_OBJECT			*SecPtr,		// Pointer to security
	SDB_VALUE_TYPE		ValueType,		// Value type to get info
	DT_VALUE*           pDTResult      // Resulting array of args
)
{
	SDB_VALUE_TYPE_INFO	*Vti = 0;

	if( !SecPtr || !ValueType )
		return Err( ERR_INVALID_ARGUMENTS, "SecDbValueTypeUsageInfo: @" );

	// if Node for sec is there get the Vti from it.
	SDB_NODE* const Node = SDB_NODE::FindTerminalInt( SecPtr, ValueType, 0, NULL, SDB_NODE::DO_NOT_CREATE, SecDbRootDb, SDB_NODE::SHARE );

	if( Node )
		Vti = Node->VtiGet();
	else
		Vti = SecPtr->SdbClass->LookupVti( ValueType->ID );

	if( !Vti )
		return Err( ERR_UNSUPPORTED_OPERATION, "Unsupported value type %s( %s )", ValueType->Name, SecPtr->SecData.Name );
	else if( !Vti->ChildList.ChildListValid )
		return Err( ERR_UNSUPPORTED_OPERATION, "Child list is not valid %s( %s )", ValueType->Name, SecPtr->SecData.Name );

	if( !SDB_NODE::ArgumentUsage( SecPtr, ValueType, Vti, pDTResult ))
    {
		return Err( ERR_UNSUPPORTED_OPERATION, "Failed to call SDB_NODE:: ArgumentUsage %s( %s )", ValueType->Name, SecPtr->SecData.Name);
	}
	
	return TRUE;
}



/****************************************************************
**	Routine: ChildInfoCmp
**	Returns: <, >, == 0 like strcmp
**	Action : Sorts first by SecPtr, then by ValueType, then args
****************************************************************/

static int ChildInfoCmp(
	const HASHPAIR_PTR KeyA,
	const HASHPAIR_PTR KeyB
)
{
	const SDB_CHILD_INFO
			*ChildA	= (SDB_CHILD_INFO *) KeyA->Key,
			*ChildB = (SDB_CHILD_INFO *) KeyB->Key;
			
	int		r;
	
	
	r = stricmp( ChildA->ValueType->Name, ChildB->ValueType->Name );
	if( r )
		return( r );

	r = stricmp( ChildA->SecPtr->SecData.Name, ChildB->SecPtr->SecData.Name );
	if( r )
		return( r );

	// FIX - could compare args
	
	return( 0 );
}



/****************************************************************
**	Routine: SecDbChildEnumFirst
**	Returns: Pointer to first child's information
**			 NULL if no children
**	Summary: Start the enumeration of children for a valuetype
****************************************************************/

SDB_CHILD_INFO *SecDbChildEnumFirst(
	SDB_ENUM_PTR			*EnumPtr,		// Pointer to pointer to return
	SDB_OBJECT				*SecPtr,		// Pointer to security to enumerate
	SDB_VALUE_TYPE 			ValueType,		// Value type to enumerate for
	int						Argc,			// Count of Argv
	DT_VALUE				**Argv,			// Args of value type being enumerated
	SDB_CHILD_ENUM_FILTER	*Filter,		// child enum filter
	bool					Sorted			// Sort children?
)
{
	if( !NodeLockAll( "SecDbChildEnum" ))
		return NULL;

	if( !ERR_CALLOC( *EnumPtr, SDB_ENUM_STRUCT, 1, sizeof( **EnumPtr )))
		return NULL;
		
	SDB_NODE* pNode = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, Argc, Argv, SDB_NODE::CHECK_SUPPORTED, SecDbRootDb, SDB_NODE::SHARE );
	if( !pNode )
	{
		(*EnumPtr)->Count = -1;
		return NULL;
	}

	SDB_DB* ExpectedDb = pNode->ExpectedChildDb( SecDbRootDb );

    // FIX: Here we could have a node which is shared by another DB, DS (e.g [1;dev, dev]). assuming that
    // we are looking in dev, as a result of building children we might split to 1;dev (as a result of
    // childern spliting and removing the DB from node's DB set). We need to check for the condition
    // and try again if it occurs
    // Max nRetry is set to 1 more the num of Dbs in Dbset (just in case)
    bool bRetry = true;
    int nRetry = 0;
    for( nRetry = pNode->DbSetGet()->Size()+1; ( bRetry && nRetry ); --nRetry )
    {
        bool const Status = NodeChildren( pNode, &(*EnumPtr)->Hash, Filter, ExpectedDb );
    
        // oops the Db we are looking for is removed from the DB set
        if( !pNode->DbSetGet()->Member( ExpectedDb ) )
        {
             NodeChildrenHashDestroy((*EnumPtr)->Hash);
            (*EnumPtr)->Hash = 0;
            (*EnumPtr)->Count = -1;
            pNode = pNode->FindAgain( SDB_NODE::CREATE, SecDbRootDb, SDB_NODE::SHARE );
            if( !pNode )
                return NULL;

            ExpectedDb = pNode->ExpectedChildDb( SecDbRootDb );
            bRetry = true;
        }
        else if( !Status )
        {
            (*EnumPtr)->Count = -1;
            return NULL;
        }
        else
            bRetry = false;
    }

    // Just in case
    if( !pNode->DbSetGet()->Member( ExpectedDb ) )
        return NULL;
		
/*
**	If the application requires a sorted list...
*/

	(*EnumPtr)->Count = (*EnumPtr)->Hash->KeyCount;
	if( Sorted )
	{
		(*EnumPtr)->HashPairPos = (*EnumPtr)->HashPair = HashSortedArray( (*EnumPtr)->Hash, ChildInfoCmp );
		SDB_CHILD_INFO* const ChildInfo = (SDB_CHILD_INFO *) (*EnumPtr)->HashPairPos->Key;
		++(*EnumPtr)->HashPairPos;
		return ChildInfo;
	}

/*
**	Unsorted
*/
	(*EnumPtr)->HashPairPos = (*EnumPtr)->HashPair = NULL;
	(*EnumPtr)->Ptr         = HashFirst( (*EnumPtr)->Hash );

	if( HashEnd( (*EnumPtr)->Ptr ))
		return NULL;

	return (SDB_CHILD_INFO *) HashKey( (*EnumPtr)->Ptr );
}	



/****************************************************************
**	Routine: SecDbChildEnumNext
**	Returns: Pointer to next child's information
**			 NULL if no more children
**	Summary: Get the next child in the enumeration of child values
****************************************************************/

SDB_CHILD_INFO *SecDbChildEnumNext(
	SDB_ENUM_PTR	EnumPtr		// Child enumeration pointer
)
{
	if( EnumPtr->HashPairPos )
	{
	    SDB_CHILD_INFO* const ChildInfo = (SDB_CHILD_INFO *) EnumPtr->HashPairPos->Key;
		++EnumPtr->HashPairPos;
		return ChildInfo;
	}
	EnumPtr->Ptr = HashNext( EnumPtr->Hash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;

	return( (SDB_CHILD_INFO *) HashKey( EnumPtr->Ptr ));
}



/****************************************************************
**	Routine: SecDbChildEnumClose
**	Returns: Nothing
**	Summary: End the enumeration of child values
****************************************************************/

void SecDbChildEnumClose(
	SDB_ENUM_PTR	EnumPtr		// Child enumeration pointer
)
{
	if( EnumPtr )
	{
		NodeUnlockAll( "SecDbChildEnum" );
		if( EnumPtr->HashPair )
			MemPoolFree( EnumPtr->Hash->MemPool, EnumPtr->HashPair );
		if( EnumPtr->Count >= 0 )
			NodeChildrenHashDestroy( EnumPtr->Hash );
		free( EnumPtr );
		EnumPtr = NULL;
	}
}



/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER::~SDB_CHILD_ENUM_FILTER
**	Returns: 
**	Action : 
****************************************************************/

SDB_CHILD_ENUM_FILTER::~SDB_CHILD_ENUM_FILTER(
)
{
	delete m_NextFilter;
}



/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER::Recurse
**	Returns: TRUE/FALSE
**	Action : Returns true if we should recurse on this node
****************************************************************/

bool SDB_CHILD_ENUM_FILTER::Recurse(
	SDB_NODE	*Node,
	bool		Matched
)
{
	return !m_NextFilter || m_NextFilter->Recurse( Node, Matched );
}

/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_FLAGS::ExternalVtFromVt
**	Returns: SDB_VALUE_TYPE
**	Action : Returns the value type for Vt Is External
****************************************************************/

SDB_VALUE_TYPE SDB_CHILD_ENUM_FILTER_FLAGS::ExternalVtFromVt(
	SDB_VALUE_TYPE	Vt
)
{
	static HASH
			*ExternalVtMap = HashCreate( "ExternalVTMap", NULL, NULL, 0, NULL );

	SDB_VALUE_TYPE
			ExternalVt = (SDB_VALUE_TYPE) HashLookup( ExternalVtMap, Vt );

	if( !ExternalVt )
	{
		if( NULL == Vt )
			return (SDB_VALUE_TYPE) ErrN( ERR_INVALID_ARGUMENTS, "ExternalVtFromVt: NULL Vt" );


		char	*Name = StrPaste( Vt->Name, " Is External", NULL );

		if( !(ExternalVt = SecDbValueTypeFromName( Name, DtDouble )))
			return NULL;

		HashInsert( ExternalVtMap, Vt, ExternalVt );
	}
	return ExternalVt;
}



/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_FLAGS::Recurse
**	Returns: TRUE/FALSE
**	Action : Returns true if we should recurse on this node
****************************************************************/

bool SDB_CHILD_ENUM_FILTER_FLAGS::Recurse(
	SDB_NODE	*Node,
	bool		Matched
)
{
	return  ( m_Flags & SDB_ENUM_TREE ) &&
			!(( m_Flags & SDB_ENUM_PRUNE ) && Matched ) &&
			!( !( m_Flags & SDB_ENUM_BELOW_SET ) && Node->ValidGet() && ( Node->CacheFlagsGet() & ( SDB_CACHE_SET | SDB_CACHE_DIDDLE ))) &&
			( !m_NextFilter || m_NextFilter->Recurse( Node, Matched ));
}



/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_FLAGS::Match
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_CHILD_ENUM_FILTER_FLAGS::Match(
	SDB_NODE	*Node
)
{
	SDB_VALUE_TYPE_INFO
			*NodeVti;

	return !(
		( ( m_Flags & SDB_ENUM_LEAVES_ONLY )
		  && !( m_Flags & SDB_ENUM_BELOW_SET )
		  && !( Node->ValidGet() && ( Node->CacheFlagsGet() & SDB_CACHE_SET ) )
		)
		|| ( ( m_Flags & ( SDB_ENUM_EXTERNAL | SDB_ENUM_EXTERNAL_VT ))
			 && !( ( ( NodeVti = Node->VtiGet())
					 && ( NodeVti->ValueFlags & SDB_EXTERNAL_FLAG )
				   )
				   || ( m_MatchExternalAndExternalVt && MatchExternalVt( Node ) )
				 )
		   )
		|| ( m_NextFilter && !m_NextFilter->Match( Node ) )
	);
}

/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_SEC_TYPE::Match
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_CHILD_ENUM_FILTER_SEC_TYPE::Match(
	SDB_NODE	*Node
)
{
	SDB_OBJECT* const SecPtr = (SDB_OBJECT *) Node->ObjectGet()->AccessValue()->Data.Pointer;

	return SecPtr->SecData.Type == m_SecType && ( !m_NextFilter || m_NextFilter->Match( Node ));
}

/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_VALUE_TYPE::Match
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_CHILD_ENUM_FILTER_VALUE_TYPE::Match(
	SDB_NODE	*Node
)
{
	SDB_VALUE_TYPE const ValueType = (SDB_VALUE_TYPE) Node->ValueTypeGet()->AccessValue()->Data.Pointer;

	return ValueType == m_ValueType && ( !m_NextFilter || m_NextFilter->Match( Node ));
}

/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_SELF::Match
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_CHILD_ENUM_FILTER_SELF::Match(
	SDB_NODE	*Node
)
{
	return !m_NextFilter || m_NextFilter->Match( Node );
}



/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_SELF::Recurse
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_CHILD_ENUM_FILTER_SELF::Recurse(
	SDB_NODE	*Node,
	bool		Matched
)
{
	return m_SecPtr == (SDB_OBJECT *) Node->ObjectGet()->AccessValue()->Data.Pointer && ( !m_NextFilter || m_NextFilter->Recurse( Node, Matched ));
}



/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_NONE::Match
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_CHILD_ENUM_FILTER_NONE::Match(
	SDB_NODE	*
)
{
	return false;
}



/****************************************************************
**	Routine: SDB_CHILD_ENUM_FILTER_NONE::Recurse
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_CHILD_ENUM_FILTER_NONE::Recurse(
	SDB_NODE	*Node,
	bool		Matched
)
{
	return !m_NextFilter || m_NextFilter->Recurse( Node, Matched );
}
