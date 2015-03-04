#define GSCVSID "$Header: /home/cvs/src/secdb/src/x_slgsdt.c,v 1.9 2001/11/27 23:23:49 procmon Exp $"
/****************************************************************
**
**	x_slgsdt.c	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: x_slgsdt.c,v $
**	Revision 1.9  2001/11/27 23:23:49  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.7  2001/05/11 13:39:51  vengrd
**	Change err msg slightly.
**	
**	Revision 1.6  2001/05/09 21:56:59  vengrd
**	Better error message on common misuse of binary @ operator.
**	
**	Revision 1.5  2001/05/01 13:09:05  johnsoc
**	Added DataTypeOf
**	
**	Revision 1.4  2001/03/23 20:13:58  singhki
**	Use ERR_STR_DONT_CHANGE to avoid printf warning
**	
**	Revision 1.3  2001/02/05 22:19:53  simpsk
**	Fix compile warnings.
**	
**	Revision 1.2  2001/02/01 21:36:15  shahia
**	clean up header dependancy in x_slgsdt.c
**	
**	Revision 1.1  2001/02/01 20:14:10  shahia
**	move SlangXGsDt funcs from gsdt to secdb to get rid of circular depend
**	
**
****************************************************************/

#define BUILDING_SECDB

#include    <portable.h>
#include	<gsdt/GsDtDate.h>
#include	<gsdt/GsDtGeneric.h>
#include	<gsdt/GsDtRDate.h>
#include	<gscore/GsRDateRDate.h>
#include	<gscore/GsRDateRDatePeriod.h>
#include	<secexpr.h>
#include	<gsdt/GsDtToDtValue.h>
#include	<gsdt/x_gsdt.h>
#include    <sdb_x.h>



#define GSDT_CAST(P) (static_cast<GsDt *>( const_cast<void*>( P )))
#define ITER_CAST(P) (static_cast<GsDt::Iterator *>( const_cast<void *>( P )))

CC_USING_NAMESPACE(Gs);

/****************************************************************
**	Routine: SlangXGsDt
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Construct a GsDt datatype
****************************************************************/

SLANG_RET_CODE SlangXGsDt( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Value",	NULL,	"Double/String/Array/Vector...", SLANG_ARG_OPTIONAL	}
			};

	SLANG_ARG_VARS;

	GsDt	*GsVal = NULL;

	SLANG_RET_CODE
			RetCode = SLANG_OK;


	SLANG_ARG_PARSE();

	GsVal = GsDtFromDtValue( &SLANG_VAL(0) );
	if( GsVal )
	{
		Ret->Type = SLANG_TYPE_VALUE;
		Ret->Data.Value.DataType	= DtGsDt;
		Ret->Data.Value.Data.Pointer= GsVal;
	}
	else
	{
		GsErrMore( "GsDt( %s ), conversion failed", SLANG_VAL(0).DataType->Name );
		RetCode = SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	}
	SLANG_ARG_FREE();

	return RetCode;
}



/****************************************************************
**	Routine: SlangXGsDtToDtValue
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Convert a GsDt datatype to Dt datatype
****************************************************************/

SLANG_RET_CODE SlangXGsDtToDtValue( SLANG_ARGS )
{

	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "GsDtValue",			&DtGsDt,	"Value to convert"									},
				{ "Conversion Flag",	&DtDouble,	"When to give up on conversion", SLANG_ARG_OPTIONAL }
			};

	SLANG_ARG_VARS;

	SLANG_RET_CODE
			RetCode = SLANG_OK;

	GSDT_TO_DTVALUE_FLAG
			ConversionFlag = GSDT_TO_DTV_FAIL_WITH_EXCEPTION;

	SLANG_ARG_PARSE();

	const GsDt	*DtArg = (GsDt *) SLANG_PTR(0);

	if( !SLANG_ARG_OMITTED( 1 ) )
	{
		double	DoubleFlag = SLANG_NUM( 1 );
 
		if( DoubleFlag == static_cast<double>( GSDT_TO_DTV_FAIL_WITH_DTGSDT ) )
			ConversionFlag = GSDT_TO_DTV_FAIL_WITH_DTGSDT;
		else if( DoubleFlag == static_cast<double>( GSDT_TO_DTV_FAIL_WITH_EXCEPTION ) ) 
			ConversionFlag = GSDT_TO_DTV_FAIL_WITH_EXCEPTION;
		else
		{
			SLANG_ARG_FREE();

			DTM_INIT( &Ret->Data.Value );
			Ret->Type = SLANG_TYPE_VALUE;

			Err( ERR_INVALID_ARGUMENTS, "Unrecognized Conversion Flag %d.", int(DoubleFlag) );

			return SLANG_OK;
		}
	}

	if( !GsDtToDtValue( *DtArg, &Ret->Data.Value, ConversionFlag ))
		RetCode = SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	else
		Ret->Type = SLANG_TYPE_VALUE;

	SLANG_ARG_FREE();
	return RetCode;
}



/****************************************************************
**	Routine: SlangXGsDtCast
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Convert a GsDt datatype to another GsDt datatype
****************************************************************/

SLANG_RET_CODE SlangXGsDtCast( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "NewType",	&DtString,	"GsDt type name"	},
				{ "GsDtValue",	&DtGsDt,	"Value to convert"	}
			};

	SLANG_ARG_VARS;

	SLANG_RET_CODE
			RetCode = SLANG_OK;


	SLANG_ARG_PARSE();

	const char
			*Type = SLANG_STR(0);

	GsDt	*DtArg = (GsDt *) SLANG_PTR(1),
			*GsVal = NULL;

	const GsType
			*Dtt = GsType::Lookup( Type );

	if( !Dtt )
		RetCode = SlangEvalError( SLANG_ERROR_PARMS, "GsDtCast( %s ), Unknown type", Type );
	else
	{
		GsVal = Dtt->ConvertFrom( *DtArg );
		if( !GsVal )
		{
			GsErrMore( "GsDtCast( %s, %s ), cast failed", Type, DtArg->typeName() );
			RetCode = SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
		}
		else
		{
			Ret->Type = SLANG_TYPE_VALUE;
			Ret->Data.Value.DataType	= DtGsDt;
			Ret->Data.Value.Data.Pointer= GsVal;
		}
	}

	SLANG_ARG_FREE();
	return RetCode;
}



/****************************************************************
**	Routine: SlangXGsDtTypeOf
**	Returns: Evaluation retufrn code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for GsDtType
****************************************************************/

SLANG_RET_CODE SlangXGsDtTypeOf( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "GsDtValue",	&DtGsDt,	"Value to get type of"	}
			};

	SLANG_ARG_VARS;


	SLANG_ARG_PARSE();

	GsDt	*Dt = GSDT_CAST( SLANG_PTR(0) );

	Ret->Type = SLANG_TYPE_VALUE;
	Ret->Data.Value.DataType	= DtString;
	Ret->Data.Value.Data.Pointer= strdup( Dt->typeName() );

	SLANG_ARG_FREE();

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXGsDtTypes
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Array of all GsDtTypes
****************************************************************/

SLANG_RET_CODE SlangXGsDtTypes( SLANG_ARGS )
{
	SLANG_ARG_NONE();

	Ret->Type = SLANG_TYPE_VALUE;
	Ret->Data.Value.DataType	= DtGsDt;
	Ret->Data.Value.Data.Pointer= GsType::getTypes();

	return SLANG_OK;
}

/****************************************************************
**	Routine: SlangXBinaryAtOp
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Array of all GsDtTypes
****************************************************************/

/*
** FIX - At some point we may want to convert this function to use a DT_MSG
**       but at the moment the only lhs is GsRDate
*/

SLANG_RET_CODE SlangXBinaryAtOp( SLANG_ARGS )
{
	SLANG_RET_CC
		Temp1,
		Temp2;

	SLANG_RET_CODE
		RetCode = SLANG_OK;

	SLANG_EVAL_ARG(0, SLANG_EVAL_RVALUE, &Temp1);
	SLANG_EVAL_ARG(1, SLANG_EVAL_RVALUE, &Temp2);

	const GsRDate::GsDtType* 
		DtRDate = NULL;
	const GsDateGen::GsDtType* 
		DtDateGen = NULL;

	if( Temp1.Data.Value.DataType == DtGsDt )
		DtRDate = GsRDate::GsDtType::Cast( static_cast< const GsDt* >( Temp1.Data.Value.Data.Pointer ) );
		
	if( Temp2.Data.Value.DataType == DtGsDt )
		DtDateGen = GsDateGen::GsDtType::Cast( static_cast< const GsDt* >( Temp2.Data.Value.Data.Pointer ) );

	if( !DtRDate )
	{
		if( DtSlang == Temp2.Data.Value.DataType )
			RetCode = SlangEvalError( SLANG_ERROR_PARMS, "@@ operator syntax error.  Likely missing ; (or , if in DefineClass/TableInit)\n  before using @@ to call function." );
		else if( !DtDateGen )
			RetCode = SlangEvalError( SLANG_ERROR_PARMS, "@@ operator syntax error.  I'm confused.  This looks like a binary use of @, but the types used are innapropriate." );
		else
			RetCode = SlangEvalError( SLANG_ERROR_PARMS, "@@ LHS not GsRDate" );
	}
	else if( !DtDateGen )
		RetCode = SlangEvalError( SLANG_ERROR_PARMS, "@@ Left hand side is a GsRDate, but RHS is not a GsDateGen" );
	else
	{
		GsRDate RDate( DtRDate->data() );
		GsRDateDefaultDateGen* DG = new GsRDateDefaultDateGen( DtDateGen->data() );
		RDate.setDefaultDateGen( DG );
		DG->unRef();

		Ret->Type = SLANG_TYPE_VALUE;
		Ret->Data.Value.DataType = DtGsDt;
		Ret->Data.Value.Data.Pointer = new GsRDate::GsDtType( RDate );
	}

	return 	RetCode;
}

/****************************************************************
**	Routine: SlangXDataTypeOf
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Return the type of a data type in string form
****************************************************************/

SLANG_RET_CODE SlangXDataTypeOf( SLANG_ARGS )
{
	DT_VALUE
			TmpValue;

	char *DtType = NULL;

	if( Root->Argc != 1 )
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "DataTypeOf: ( Expression )" );

	SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, Ret );

	DtType = (char *) DtNameFromType( Ret->Data.Value.DataType );
	
	TmpValue.DataType		= DtString;

	if( 0 == strcmp( DtType, "GsDt" ) )
	{
		const GsDt *Dt = static_cast< const GsDt * >( Ret->Data.Value.Data.Pointer );
		TmpValue.Data.Pointer = strdup( Dt->typeName() );
	}
	else
		TmpValue.Data.Pointer	= strdup( DtType );

	SLANG_RET_FREE( Ret );

	Ret->Type = SLANG_TYPE_VALUE;
	Ret->Data.Value = TmpValue;

	return SLANG_OK;
}
