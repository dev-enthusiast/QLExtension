#define GSCVSID "$Header: /home/cvs/src/secdb/src/sq_arg.c,v 1.1 2000/09/01 10:03:58 goodfj Exp $"
/****************************************************************
**
**	SQ_ARG.C	- Auto-slang argument support
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sq_arg.c,v $
**	Revision 1.1  2000/09/01 10:03:58  goodfj
**	sq_arg moved in from num
**
**	Revision 1.3  2000/03/06 09:56:47  graysa
**	Change to SqArg_VECTOR to handle UseDates correctly
**	
**	Revision 1.2  1999/12/23 15:59:46  goodfj
**	Include sq_crvfn.h
**	
**	Revision 1.1  1999/08/20 14:39:50  goodfj
**	Moved in from squant
**	
**	Revision 1.18  1999/05/24 23:44:09  gribbg
**	Const correctness
**	
****************************************************************/

#define BUILDING_SECDB

#include <portable.h>
#include <sq_arg.h>
#include <sq_crvfn.h>
#include <malloc.h>



/****************************************************************
**	Routine: SqArg_String
**	Returns: SLANG_OK or SlangError
**
**	Action : Convert a DT_VALUE to a String
****************************************************************/

int SqArg_String(
	DT_VALUE	*Value,
	String		*String,
	SLANG_NODE	*Root,
	SLANG_SCOPE	*Scope,
	const char	*ArgName,
	int			ArgNum
)
{
	*String = (char *) Value->Data.Pointer;

	return SLANG_OK;
}



/****************************************************************
**	Routine: Q_VectorAlloc
**	Returns: TRUE if memory could be allocated
**	Action : Allocate memory, set vector size
****************************************************************/

int Q_VectorAlloc(
	Q_VECTOR	*Vector,
	unsigned	Size
)
{
	Vector->Elem = Size;
	Vector->V = (double *) calloc( Size, sizeof( *Vector->V ) );
	if( Vector->V || Size == 0 )
		return TRUE;
	return FALSE;
}


/****************************************************************
**	Routine: SqArg_DT_CURVE
**	Returns: Success
**	Action : Converts a DT_VALUE to a DT_CURVE
****************************************************************/

int	SqArg_DT_CURVE (
	DT_VALUE		*Value,
	DT_CURVE   		*Curve,
	SLANG_NODE		*Root,
	SLANG_SCOPE		*Scope,
	const char		*ArgName,
	int				ArgNum
)
{
	if( Value->DataType == DtCurve )
	{
		*Curve = *(DT_CURVE *) Value->Data.Pointer;

		return SLANG_OK;
	}

	return SlangEvalError( Root, SLANG_EVAL_RVALUE, NULL, Scope,
						   "%s: Arg %d (%s) expected Curve, got %s",
                           Root->StringValue, ArgNum, ArgName,
                           Value->DataType->Name );
}



/****************************************************************
**	Routine: SqArg_VECTOR
**	Returns: SLANG_OK or SlangError
**
**	Action : Convert a DT_VALUE to a Q_VECTOR or Q_DATE_VECTOR
**			 Chooses Dates or Values from curves based on
**			 UseDates flag
****************************************************************/

static int SqArg_VECTOR(
	DT_VALUE	*Value,
	Q_VECTOR	*Vector,
	SLANG_NODE	*Root,
	SLANG_SCOPE	*Scope,
	const char	*ArgName,
	int			ArgNum,
	int			UseDates
)
{
	int		Elem;


	if( Value->DataType == DtArray )
	{
		DT_ARRAY	*Array;


		Array = (DT_ARRAY *) Value->Data.Pointer;
		for( Elem = 0; Elem < Array->Size; Elem++ )
			if( UseDates )
			{
				if( Array->Element[ Elem ].DataType != DtDate )
					return SlangEvalError( Root, SLANG_EVAL_RVALUE, NULL, Scope,
										   "%s: Arg %d (%s) expected array of dates\n"
										   "   element %d is a %s",
										   Root->StringValue, ArgNum, ArgName, Elem,
										   Array->Element[ Elem ].DataType->Name );
			}
			else 
				if( Array->Element[ Elem ].DataType != DtDouble )
					return SlangEvalError( Root, SLANG_EVAL_RVALUE, NULL, Scope,
										   "%s: Arg %d (%s) expected array of doubles\n"
										   "   element %d is a %s",
										   Root->StringValue, ArgNum, ArgName, Elem,
										   Array->Element[ Elem ].DataType->Name );

		if( !Q_VectorAlloc( Vector, Array->Size ) )
			return SlangEvalError( Root, SLANG_EVAL_RVALUE, NULL, Scope,
								   "%s: Arg %d (%s) out of memory",
                                   Root->StringValue, ArgNum, ArgName );
		for( Elem = 0; Elem < Array->Size; Elem++ )
			Vector->V[ Elem ] = Array->Element[ Elem ].Data.Number;
		return SLANG_OK;
	}
	else if( Value->DataType == DtCurve )
	{
		DT_CURVE	*Curve;


		Curve = (DT_CURVE *) Value->Data.Pointer;
		if( !Q_VectorAlloc( Vector, Curve->KnotCount ) )
			return SlangEvalError( Root, SLANG_EVAL_RVALUE, NULL, Scope,
								   "%s: Arg %d (%s) out of memory",
                                   Root->StringValue, ArgNum, ArgName );
		if( UseDates )
			for( Elem = 0; Elem < Curve->KnotCount; Elem++ )
				Vector->V[ Elem ] = Curve->Knots[ Elem ].Date;
		else
			for( Elem = 0; Elem < Curve->KnotCount; Elem++ )
				Vector->V[ Elem ] = Curve->Knots[ Elem ].Value;

		return SLANG_OK;
	}

	return SlangEvalError( Root, SLANG_EVAL_RVALUE, NULL, Scope,
						   "%s: Arg %d (%s) expected Curve or Array, got %s",
                           Root->StringValue, ArgNum, ArgName,
                           Value->DataType->Name );
}



/****************************************************************
**	Routine: SqArg_Q_VECTOR
**	Returns: SLANG_OK or SlangError
**
**	Action : Convert a DT_VALUE to a Q_VECTOR
****************************************************************/

int SqArg_Q_VECTOR(
	DT_VALUE	*Value,
	Q_VECTOR	*QVector,
	SLANG_NODE	*Root,
	SLANG_SCOPE	*Scope,
	const char	*ArgName,
	int			ArgNum
)
{
	DT_VALUE
			VecVal;

	DT_VECTOR
			*Vector;

	//First attempt to convert to a DT_VECTOR, but don't allow DtDouble
	VecVal.DataType = DtVector;
	if( Value->DataType != DtDouble && DTM_TO( &VecVal, Value ))
	{
		Vector = (DT_VECTOR *) VecVal.Data.Pointer;

		QVector->Elem = Vector->Size;
		QVector->V	  = Vector->Elem;

		free( Vector );
		return SLANG_OK;
	}

	// Default
	return SqArg_VECTOR( Value, QVector, Root, Scope, ArgName, ArgNum, FALSE );
}



/****************************************************************
**	Routine: SqArg_Q_DATE_VECTOR
**	Returns: SLANG_OK or SlangError
**
**	Action : Convert a DT_VALUE to a Q_DATE_VECTOR
****************************************************************/

int	SqArg_Q_DATE_VECTOR(
	DT_VALUE 		*Value,
	Q_DATE_VECTOR 	*Vector,
	SLANG_NODE 		*Root,
	SLANG_SCOPE 	*Scope,
	const char		*ArgName,
	int 			ArgNum
)
{
	return SqArg_VECTOR( Value, Vector, Root, Scope, ArgName, ArgNum, TRUE );
}



/****************************************************************
**	Routine: SqArg_Q_MATRIX
**	Returns: SLANG_OK or SlangError
**
**	Action : Convert a DT_VALUE to a Q_MATRIX
****************************************************************/

int SqArg_Q_MATRIX(
	DT_VALUE	*Value,
	Q_MATRIX	*Matrix,
	SLANG_NODE	*Root,
	SLANG_SCOPE	*Scope,
	const char	*ArgName,
	int			ArgNum
)
{
	if( SQ_ValueToMatrix( Value, Matrix ) )
		return SLANG_OK;

	return SlangEvalError( Root, SLANG_EVAL_RVALUE, NULL, Scope,
						   "%s: Arg %d (%s) expected 2-D array of doubles, got a %s",
                           Root->StringValue, ArgNum, ArgName,
                           Value->DataType->Name );
}

/****************************************************************
**	Routine: SqArg_Q_CURVE_MATRIX
**	Returns: SLANG_OK or SlangError
**
**	Action : Convert a DT_VALUE to a Q_MATRIX
****************************************************************/

int SqArg_Q_CURVE_MATRIX(
	DT_VALUE		*Value,
	Q_CURVE_MATRIX	*Matrix,
	SLANG_NODE		*Root,
	SLANG_SCOPE		*Scope,
	const char		*ArgName,
	int				ArgNum
)
{
	if( SQ_ValueToCurveMatrix( Value, Matrix ) )
		return SLANG_OK;

	return SlangEvalError( Root, SLANG_EVAL_RVALUE, NULL, Scope,
						   "%s: Arg %d (%s) expected 2-D array of doubles, got a %s",
                           Root->StringValue, ArgNum, ArgName,
                           Value->DataType->Name );
}
