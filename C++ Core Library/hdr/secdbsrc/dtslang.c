// -*-c++-*-
#define GSCVSID "$Header: /home/cvs/src/secdb/src/dtslang.c,v 1.57 2015/03/02 20:49:21 khodod Exp $"
/****************************************************************
**
**	DTSLANG.C	- Slang DataType
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: dtslang.c,v $
**	Revision 1.57  2015/03/02 20:49:21  khodod
**	Better error-checking to prevent crashing on the output of Function(";1").
**	AWR: #364207
**
**	Revision 1.56  2012/06/27 13:57:41  e19351
**	Include sstream directly instead of using empty wrapper.
**	AWR: #NA
**
**	Revision 1.55  2004/11/29 23:48:11  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
**	Revision 1.54  2001/11/27 23:23:34  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.51  2001/07/18 22:15:13  nauntm
**	bug fix to my last bug fix
**	
**	Revision 1.50  2001/07/16 19:03:03  nauntm
**	fixed positional arg bug
**	
**	Revision 1.49  2001/05/30 21:10:04  simpsk
**	HasReturn should be bool.
**	
**	Revision 1.48  2001/05/11 21:35:30  simpsk
**	changes to the way slang nodes are converted to strings.
**	
**	Revision 1.47  2001/05/07 20:30:55  simpsk
**	Make SlangFuncParemGet take a const DT_SLANG as it should not modify.
**	
**	Revision 1.46  2001/05/04 16:38:32  simpsk
**	added c++ comment for emacs.
**	
**	Revision 1.45  2001/04/11 22:25:03  simpsk
**	Add NO_DYNAMIC_PTRS switch.
**	
**	Revision 1.44  2001/02/12 16:36:12  vengrd
**	More comment editing.  No code changes.
**	
**	Revision 1.43  2001/02/10 17:09:02  vengrd
**	Typo in comment.
**	
**	Revision 1.42  2001/02/10 17:05:05  vengrd
**	Store away the name of the original datatype constructor used
**	for every slang function arg that is typed.  We need this info
**	to do typechecking on GsDt args later.
**	
**	Revision 1.41  2000/12/19 22:45:33  nauntm
**	fix param not found core-dump
**	
**	Revision 1.40  2000/11/28 23:22:59  singhki
**	Store Params by Index and by name to avoid strcmp in CallFunction
**	
**	Revision 1.39  2000/09/29 02:11:36  lundek
**	BugFix: null deref crash on certain Slang dt ops, e.g., ==
**	
**	Revision 1.38  2000/06/07 14:05:27  goodfj
**	Fix includes
**	
**	Revision 1.37  1999/10/05 22:35:48  singhki
**	Show slang functions with potential breakpoints with a bang
**	
**	Revision 1.36  1999/09/22 01:10:07  singhki
**	Allocate & copy ChildList in DT_SLANG because it's lifetime is different from VTI
**	
**	Revision 1.35  1999/09/01 15:29:26  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.34  1999/09/01 01:55:11  singhki
**	Get rid of SDB_CHILD_REFs, split intermediates and terminals into seperate
**	arrays in SDB_CHILD_LIST. Rationalize NoEval stuff, auto mark args/ints as
**	NoEval if they are never evaluated.
**	Color child links as purple so that we don't have to find the VTI in order
**	to correctly unlink them.
**	Uncolor arg/int which aren't purple so that they won't invalidate the tree.
**	
**	Revision 1.33  1999/08/23 15:36:04  singhki
**	no one seems to use SourceDatabaseId
**	
**	Revision 1.32  1999/07/25 21:19:51  singhki
**	cast to make OS/2 compiler happy
**	
**	Revision 1.31  1999/07/22 23:50:53  singhki
**	Use SlangParseTreeCopy to copy locals too
**	
**	Revision 1.30  1999/07/02 16:19:17  goodfj
**	Add SourceDatabaseId()
**	
**	Revision 1.29  1999/06/18 16:56:09  singhki
**	parameters will now be local variables if the func has been evaluated
**	
**	Revision 1.28  1999/06/18 10:06:18  goodfj
**	Slang supports more components
**	
**	Revision 1.27  1999/06/15 23:02:10  singhki
**	check for null in memsize
**	
**	Revision 1.26  1999/03/08 23:29:13  singhki
**	Free LocalNames
**	
**	Revision 1.25  1999/03/04 21:04:16  singhki
**	LeakFix: force parse tree to be freed
**	
**	Revision 1.24  1999/03/03 16:43:09  singhki
**	Slang Functions now use a local variable stack and their variables
**	are represented as an index into this stack.
**	Scoped variable nodes in the parse tree are replaced by references to
**	the variable itself.
**	Diddle Scopes inside a Slang Scope are now allocated on demand.
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include    <secdb/slang_util.h>
#include    <secexpr.h>
#include	<err.h>
#include	<dtformat.h>
#include	<hash.h>

#include    <algorithm>
#include    <sstream>

/*
**  Helper functors
*/

struct SlangFuncParamCompare
{
    bool operator()( SLANG_FUNC_PARAM const* lhs, SLANG_FUNC_PARAM const* rhs ) const
    {
	    return ( stricmp( lhs->Name, rhs->Name ) < 0 );
	}
};

/****************************************************************
**	Routine: ExprToStructure
**	Returns: TRUE or FALSE
**	Action : Fill in given value with recursive structure mirroring
**			 the C parse tree structure
****************************************************************/

static int ExprToStructure(
	SLANG_NODE	*Root,
	DT_VALUE	*Value
)
{
	DT_VALUE
			Tmp,
			Arg;

	int		ArgNum;


	if( !DTM_ALLOC( Value, DtStructure ))
		return( FALSE );

	DtComponentSetPointer( Value, "Type", DtString, SlangNodeNameFromType( Root->Type ));

	if( Root->StringValue && Root->Type != SLANG_SCOPE_OPERATOR )
		DtComponentSetPointer( Value, "StringValue", DtString, Root->StringValue );
	if( Root->ValueDataType != NULL )
	{
		if( Root->Type != SLANG_DATA_TYPE_CREATOR )
		{
			SLANG_NODE_GET_VALUE( Root, &Tmp );
			DtComponentSet( Value, "Value", &Tmp );
		}
		else
			DtComponentSetPointer( Value, "DataType", DtString, Root->ValueDataType->Name );
	}

	if( Root->Argc )
	{
		DTM_ALLOC( &Tmp, DtArray );
		for( ArgNum = 0; ArgNum < Root->Argc; ArgNum++ )
			if( ExprToStructure( Root->Argv[ ArgNum ], &Arg ))
			{
				DtSubscriptSet( &Tmp, ArgNum, &Arg );
				DTM_FREE( &Arg );
			}
		DtComponentSet( Value, "Argv", &Tmp );
		DTM_FREE( &Tmp );
	}
	return( TRUE );
}



/****************************************************************
**	Routine: DtSlangParamsSet
**	Returns: void
**	Action : Walk through the function's arguments and extract
**			 type information and default value for each of
**			 them and store it in the Params array
**
**			 SlangXFunction has already made sure that there
**			 are no syntactic errors, so we don't need to
**			 check for them here
****************************************************************/

static void DtSlangParamsSet(
	DT_SLANG	*SdtSlang   
)
{
	SLANG_NODE
			*Root = SdtSlang->Expression,
			*TmpNode,
			*ArgNode;

	SLANG_FUNC_PARAM
			*Params;

	int		ArgNum,
			NumArgs = SLANG_FUNC_ARITY( Root );


	Params = SdtSlang->Params =
			(SLANG_FUNC_PARAM *) ( NumArgs ? calloc( NumArgs, sizeof( SLANG_FUNC_PARAM )) : NULL );

	SdtSlang->ParamsByName = Params ? new SLANG_FUNC_PARAM *[ NumArgs ] : NULL;

	// Get pointers to parameter names and defaults from new copy of Root
	for( ArgNum = 0; ArgNum < NumArgs; ++ArgNum )
	{
		SdtSlang->ParamsByName[ ArgNum ] = Params + ArgNum;
		TmpNode = ArgNode =
				Root->Argv[ ArgNum ];

		// FIX - If the root of the arg tree is a datatype node, then
		// hack the original datatype name into the Params so
		// that we have something to look at to determine the original
		// name used for the type.  The place where we need this is
		// when we are trying to ascertain the GsDt type of a typed
		// arg after the type name has been whacked away by the arg
		// name.  We may actually be leaking a bit here if we don't
		// clean up DtConstructorName properly later.
				
		Params[ ArgNum ].DtConstructorName = NULL;

		while( TmpNode->Type != SLANG_TYPE_VARIABLE && TmpNode->Type != SLANG_TYPE_LOCAL_VARIABLE )
		{
			if( TmpNode->Type == SLANG_ADDRESS_OF )
				Params[ ArgNum ].PassByReference = 1;

			// FIX - See the FIX comment just above this loop. 
			if( SLANG_DATA_TYPE_CREATOR == TmpNode->Type )  
				Params[ ArgNum ].DtConstructorName = strdup( TmpNode->StringValue );

			TmpNode = TmpNode->Argv[ 0 ];
		}

		if( TmpNode != ArgNode )
		{
			SlangNodeStringReassign( ArgNode, strdup( TmpNode->StringValue ));
			SLANG_NODE_BUCKET_SET( ArgNode, SLANG_NODE_BUCKET( TmpNode ));
		}
		Params[ ArgNum ].Name = ArgNode->StringValue;
		Params[ ArgNum ].Index = ArgNum;

		if( ArgNode->Type == SLANG_COLON_ASSIGN || ArgNode->Type == SLANG_ASSIGN )
		{
			Params[ ArgNum ].DefaultExpression = ArgNode->Argv[ 1 ];
			Params[ ArgNum ].PassByName = ArgNode->Type == SLANG_COLON_ASSIGN;
			ArgNode = ArgNode->Argv[ 0 ];
		}
		if( ArgNode->Type == SLANG_DATA_TYPE_CREATOR )
			Params[ ArgNum ].DataType = ArgNode->ValueDataType;
	}

	if( Params )
	    CC_NS(std,sort)( SdtSlang->ParamsByName, SdtSlang->ParamsByName + NumArgs, SlangFuncParamCompare() );
}



/****************************************************************
**	Routine: ParamsToArray
**	Returns: DT_VALUE*
**	Action : Converts Params array into an Array
****************************************************************/

static DT_VALUE *ParamsToArray(
	DT_SLANG	*SdtSlang,
	DT_VALUE	*Result
)
{
	SLANG_NODE
			*Root = SdtSlang->Expression;

	int		Arg,
			NumArgs = SLANG_FUNC_ARITY( Root );

	SLANG_FUNC_PARAM
			*Param;

	DT_VALUE
			TmpValue,
			TmpValue1,
			Component;


	DTM_ALLOC( Result, DtArray );
	if( NumArgs == 0 )
		return Result;

	DtSubscriptSetNumber( Result, NumArgs - 1, DtDouble, 0 );
	for( Arg = 0; Arg < NumArgs; ++Arg )
	{
		Param = SlangFuncParamGet( SLANG_NODE_STRING_VALUE( Root->Argv[ Arg ]), SdtSlang );
	    if( !Param )
            continue;

		DTM_ALLOC( &TmpValue, DtStructure );
		DtComponentSetPointer( &TmpValue, "Name", 			DtString, Param->Name 			 );
		DtComponentSetNumber(  &TmpValue, "PassByName", 	DtDouble, Param->PassByName 	 );
		DtComponentSetNumber(  &TmpValue, "PassByReference",DtDouble, Param->PassByReference );
		if( Param->DataType )
			DtComponentSetPointer( &TmpValue, "DataType", DtString, Param->DataType->Name );
		if( Param->DefaultExpression && ExprToStructure( Param->DefaultExpression, &TmpValue1 ))
		{
			DT_POINTER_TO_VALUE( &Component, "Default", DtString );
			DtComponentReplace( &TmpValue, &Component, &TmpValue1 );
		}

		// FIX - These next two lines are related to the hack in
		// DtSlangParamsSet() which copies the original datatype name
		// when grunging around in function arg lists.  It lets us see
		// it when we convert to a structure, which makes debugging
		// easier.

		if( Param->DtConstructorName )
			DtComponentSetPointer( &TmpValue, "DataTypeConstructor", DtString, Param->DtConstructorName );

		DT_NUMBER_TO_VALUE( &Component, Arg, DtDouble );
		DtSubscriptReplace( Result, &Component, &TmpValue );
	}
	return Result;
}



/****************************************************************
**	Routine: DtSlangFree
**	Returns: void
**	Action : Free's parse tree and parameter info
****************************************************************/

static void DtSlangFreeData(
	DT_SLANG	*SdtSlang
)
{
	free( SdtSlang->Params );
	delete [] SdtSlang->ParamsByName;

	// We really want to free the slang parse tree this time
	SdtSlang->Expression->Flags &= ~SLANG_NODE_DONT_FREE;
	SlangFree( SdtSlang->Expression );

	if( SdtSlang->Locals )
		HashDestroy( SdtSlang->Locals );

	free( SdtSlang->LocalNames );

	if( SdtSlang->ChildList )
	{
		SdtSlang->ChildList->Free();
		free( SdtSlang->ChildList );
	}

	SdtSlang->Params = NULL;
	SdtSlang->Expression = NULL;
	SdtSlang->Locals = NULL;
	SdtSlang->LocalNames = NULL;
	SdtSlang->ChildList = NULL;
	SdtSlang->ParamsByName = NULL;
}



/****************************************************************
**	Routine: DtSlangExpressionReplace
**	Returns: TRUE/Err()
**	Action : Replaces the expression of a DT_SLANG
**			 If you pass in Expression instead of Source, then
**			 you must Bind the local variables yourself
****************************************************************/

int DtSlangExpressionReplace(
	DT_VALUE	*Value,
	SLANG_NODE	*Expression,
	short		RequiredPositionalArgs,
	bool		HasReturn,
	DT_SLANG	*Source
)
{
	DT_SLANG
			*Slang = (DT_SLANG *) Value->Data.Pointer;


	if( Value->DataType != DtSlang )
		return Err( ERR_INVALID_ARGUMENTS, "DtSlangExpressionReplace: Value must be a DtSlang" );

	if( !Source && !Expression )
		return Err( ERR_INVALID_ARGUMENTS, "DtSlangExpressionReplace: Expression must not be NULL" );

	if( Slang )
		DtSlangFreeData( Slang );
	else
	{
		Slang = (DT_SLANG *) (Value->Data.Pointer = calloc( 1, sizeof( SLANG_DATA_TYPE )));
		Slang->ReferenceCount = 1;
	}
	if( Source )
	{
		Slang->LocalNames			= (char const**) calloc( Source->Locals->KeyCount, sizeof( char const* ));
		// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
		Slang->Locals				= HashCreate( "Locals", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, Source->Locals->Size, NULL );

		// Silly hack to force SlangParseTreeCopyInt() to bind the local variables
		Source->Expression->Flags	&= ~SLANG_NODE_PUSH_SCOPE;
		Slang->Expression			= SlangParseTreeCopyInt( Source->Expression, NULL, Slang );
		Source->Expression->Flags	|= SLANG_NODE_PUSH_SCOPE;
		Slang->Expression->Flags	|= SLANG_NODE_PUSH_SCOPE;
	}
	else
		Slang->Expression 			= Expression;
	Slang->RequiredPositionalArgs	= RequiredPositionalArgs;
	Slang->HasReturn				= HasReturn;
	DtSlangParamsSet( Slang );

	return TRUE;
}



/****************************************************************
**	Routine: DtFuncSlang
**	Returns: TRUE	- Operation worked
**			 FALSE	- Operation failed
**	Action : Handle operations for string data type
****************************************************************/

int DtFuncSlang(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	DT_INFO
			*DataTypeInfo;

	DT_SLANG
			*DataTypeSlang;


	if( b && ( b->DataType != a->DataType )
		&& ( Msg != DT_MSG_COMPONENT_VALUE )
		&& ( Msg != DT_MSG_COMPONENT_REPLACE )
		&& ( Msg != DT_MSG_SUBSCRIPT_VALUE )
		&& ( Msg != DT_MSG_SUBSCRIPT_REPLACE ))
		return Err( ERR_UNSUPPORTED_OPERATION, "@ - Between %s and %s", a->DataType->Name, b->DataType->Name );

	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size	= sizeof( a->Data.Pointer );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_REFERENCE;
			return TRUE;

		case DT_MSG_ALLOCATE:
			r->Data.Pointer = NULL;
			return TRUE;

		case DT_MSG_FREE:
			DataTypeSlang = (DT_SLANG *) a->Data.Pointer;
			if( DataTypeSlang && --(DataTypeSlang->ReferenceCount) == 0 )
			{
				DtSlangFreeData( DataTypeSlang );
				free( DataTypeSlang );
				a->Data.Pointer = NULL;
			}
			return TRUE;

		case DT_MSG_ASSIGN:
			DataTypeSlang = (DT_SLANG *) (r->Data.Pointer = a->Data.Pointer);
			if( DataTypeSlang )
				++(DataTypeSlang->ReferenceCount);
			break;

		case DT_MSG_TO:
			if( r->DataType == a->DataType )
			{
				DataTypeSlang = (DT_SLANG *) (r->Data.Pointer = a->Data.Pointer);
				if( DataTypeSlang )
					++(DataTypeSlang->ReferenceCount);
			}
			else if( r->DataType == DtString )
			{
			    DataTypeSlang = (DT_SLANG *) a->Data.Pointer;
				if( !DataTypeSlang )
				{
					r->Data.Pointer = strdup( "" );
				}
				else
				{
				    CC_NS(std,ostringstream) stream;
					SlangParseTreeToString( DataTypeSlang->Expression, 0, stream );
					r->Data.Pointer = strdup( stream.str().c_str());
				}
			}
			else if( r->DataType == DtStructure )
			{
				SLANG_DATA_TYPE
						*SlangA = (DT_SLANG *) a->Data.Pointer;

				DT_VALUE
						Tmp;

				if( !DTM_ALLOC( r, DtStructure ))
					return( FALSE );
				if( SlangA )
				{
					DtComponentSetNumber( r, "ReferenceCount", 	DtDouble, SlangA->ReferenceCount );
					DtComponentSetNumber( r, "HasReturn",  		DtDouble, SlangA->HasReturn );
					DtComponentSetNumber( r, "ChildList", 		DtDouble, (double) (long) SlangA->ChildList );
					DtComponentSetNumber( r, "RequiredPositionalArgs", DtDouble, SlangA->RequiredPositionalArgs );
					DtComponentSet( 	  r, "Params",			ParamsToArray( SlangA, &Tmp ) );
					DTM_FREE( &Tmp );
					if( ExprToStructure( SlangA->Expression, &Tmp ))
					{
						DtComponentSet( r, "Expression", &Tmp );
						DTM_FREE( &Tmp );
					}
				}
				return( TRUE );
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", a->DataType->Name, r->DataType->Name );
			return TRUE;

		case DT_MSG_COMPONENT_REPLACE:	// a.b = r
		case DT_MSG_SUBSCRIPT_REPLACE:
		{
			if( b->DataType != DtString && stricmp( (char *) b->Data.Pointer, "_RefValue" ))
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - only _RefValue component allowed" );
			else if( r->DataType != DtSlang )
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - _RefValue must be a DtSlang" );

			DataTypeSlang = (DT_SLANG *) r->Data.Pointer;
			if( !DataTypeSlang || !DataTypeSlang->Expression )
				return Err( ERR_UNKNOWN, "@ - NULL Expression in DtSlang" );

			return DtSlangExpressionReplace( a, NULL, DataTypeSlang->RequiredPositionalArgs, DataTypeSlang->HasReturn, DataTypeSlang );
		}

		case DT_MSG_COMPONENT_VALUE:	// r = a.b  Only supported for some components. See below
		case DT_MSG_SUBSCRIPT_VALUE:	// r = a[ b ]
			if( b->DataType != DtString )
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript/Component must be a String, not a %s", b->DataType->Name );

			if( !stricmp( (char *) DT_VALUE_TO_POINTER( b ), "Params" ))
			{
				ParamsToArray( ( DT_SLANG *) DT_VALUE_TO_POINTER( a ), r );
				return TRUE;
			}

			DataTypeSlang = (DT_SLANG *) DT_VALUE_TO_POINTER( a );

			if( !stricmp( (char *) DT_VALUE_TO_POINTER( b ), "ReferenceCount" ))
				DT_VALUE_TO_NUMBER( r ) = DataTypeSlang ? DataTypeSlang->ReferenceCount : 0;

			else if( !stricmp( (char *) DT_VALUE_TO_POINTER( b ), "Expression" ))
				DT_VALUE_TO_NUMBER( r ) = DataTypeSlang ? (double)((long)DataTypeSlang->Expression) : 0;

			else if( !stricmp( (char *) DT_VALUE_TO_POINTER( b ), "HasReturn" ))
				DT_VALUE_TO_NUMBER( r ) = DataTypeSlang ? DataTypeSlang->HasReturn : 0;

			else if( !stricmp( (char *) DT_VALUE_TO_POINTER( b ), "ChildList" ))
				DT_VALUE_TO_NUMBER( r ) = DataTypeSlang ? (double) (long)DataTypeSlang->ChildList : 0;

			else if( !stricmp( (char *) DT_VALUE_TO_POINTER( b ), "RequiredPositionalArgs" ))
				DT_VALUE_TO_NUMBER( r ) = DataTypeSlang ? DataTypeSlang->RequiredPositionalArgs : 0;

			else if( DataTypeSlang && !stricmp( (char *) DT_VALUE_TO_POINTER( b ), "_RefValue" ))
			{
				DTM_ALLOC( r, DtSlang );
				return DT_OP( DT_MSG_COMPONENT_REPLACE, a, r, b );
			}
			else
				return Err( ERR_INVALID_ARGUMENTS, "@ - only ReferenceCount, Expression, Params, HasReturn, ChildList, RequiredPositionalArgs and _RefValue components allowed" );

			r->DataType = DtDouble;

			return TRUE;

		case DT_MSG_EQUAL:
		case DT_MSG_NOT_EQUAL:
		{
			SLANG_DATA_TYPE
					*SlangA = (DT_SLANG *) a->Data.Pointer,
					*SlangB = (DT_SLANG *) b->Data.Pointer;

			int		Equal;

			if( b->DataType != a->DataType )
				Equal = FALSE;
			else
				Equal = SlangA == SlangB || ( SlangA && SlangB && SlangParseTreeEqual( SlangA->Expression, SlangB->Expression ));

			if( DT_MSG_EQUAL == Msg )
				r->Data.Number = Equal;
			else
				r->Data.Number = !Equal;
			r->DataType = DtDouble;
			return TRUE;
		}

		case DT_MSG_TRUTH:
			return( a->Data.Pointer ? TRUE : FALSE );

		case DT_MSG_SIZE:
			r->DataType 	= DtDouble;
			r->Data.Number	= 1.0;
			return TRUE;

		case DT_MSG_MEMSIZE:
			r->DataType 	= DtDouble;
			if(( DataTypeSlang = (DT_SLANG *) a->Data.Pointer ))
			{
				r->Data.Number = SlangParseTreeMemSize( DataTypeSlang->Expression );

				// FIX: need to add sizes of default values for params
				if( DataTypeSlang->Params )
					r->Data.Number +=
							( sizeof( SLANG_FUNC_PARAM ) + sizeof( unsigned int ) ) *
							SLANG_FUNC_ARITY( DataTypeSlang->Expression );
			}
			else
				r->Data.Number = 0;

			return TRUE;

		case DT_MSG_INFO:
			DataTypeInfo = (DT_INFO *) calloc( 1, sizeof( DT_INFO ));
			DataTypeInfo->Flags = DT_FLAG_POINTER;
			r->Data.Pointer = DataTypeInfo;
			return TRUE;

		case DT_MSG_FORMAT:
		{
			DT_MSG_FORMAT_INFO
					*FormatInfo = (DT_MSG_FORMAT_INFO *)r->Data.Pointer;

			int		Width;


			if( !( DataTypeSlang = (DT_SLANG *) a->Data.Pointer ))
				return Err( ERR_UNSUPPORTED_OPERATION, "@: Null parse tree" );

			switch( FormatInfo->Code )
			{
				default:
					return Err( ERR_UNSUPPORTED_OPERATION, "Slang, format code: '%c'", FormatInfo->Code );

				case 'v':
					if( !DTM_FORMAT_ALLOC( FormatInfo, 60 ))
						return FALSE;
					sprintf( FormatInfo->Buffer, "%.32s:%d-%d%s",
							 DataTypeSlang->Expression->ErrorInfo.ModuleName,
							 DataTypeSlang->Expression->ErrorInfo.BeginningLine,
							 DataTypeSlang->Expression->ErrorInfo.EndingLine,
							 DataTypeSlang->Expression->Argc && DataTypeSlang->Expression->Argv[ DataTypeSlang->Expression->Argc - 1 ]->Extra
							 ? "!" : "" );
					if(( Width = FormatInfo->OutForm.Width ) > 0 )
						FormatInfo->Buffer[ Width ] = '\0';
					break;
			}
			return TRUE;
		}

		default:
			return Err( ERR_UNSUPPORTED_OPERATION, "Slang, @" );
	}

	r->DataType = a->DataType;
	return TRUE;
}

/****************************************************************
**	Routine: SlangFuncParamGet
**	Returns: SLANG_FUNC_PARAM*
**	Action : Returns structure for a slang function
**			 parameters type/default value info
****************************************************************/

SLANG_FUNC_PARAM *SlangFuncParamGet(
	char const	   *ParamName,
	DT_SLANG const *SdtSlang
)
{
	SLANG_FUNC_PARAM
	        TmpParam;

    if( !ParamName )
        return( NULL );

	TmpParam.Name = const_cast<char*>( ParamName ); // kludge, but I really do want ParamName to be a const char*

	SLANG_FUNC_PARAM** const TheEnd = SdtSlang->ParamsByName + SLANG_FUNC_ARITY( SdtSlang->Expression );
	SLANG_FUNC_PARAM** const result = CC_NS(std,lower_bound)( SdtSlang->ParamsByName, TheEnd, &TmpParam, SlangFuncParamCompare() );
	if ( result == TheEnd ) 
		return NULL;
	if ( stricmp( (*result)->Name, ParamName ))
		return NULL;
	return *result;
}
