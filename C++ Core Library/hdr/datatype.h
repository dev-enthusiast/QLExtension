/****************************************************************
**
**	DATATYPE.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/datatype/src/datatype.h,v 1.153 2010/11/22 22:02:05 E19385 Exp $
**
****************************************************************/

#ifndef IN_DATATYPE_H
#define IN_DATATYPE_H

// forward declare structures
struct HEAP;
struct HashStructure;
struct HASH_ENTRY;

#include <datatype/base.h>
#include <iosfwd>
#include <string.h>
#include <string>

/*
** Things that do not depend on DT_VALUE have been moved into the
** lower level file dtcore.h  so before making additions to datatype.h
** determine if they belong in dtcore.h instead.
*/

#ifndef IN_DTCORE_H
#include <dtcore.h>
#endif


/*
**	Define constants
*/

// Sizes
#define	DT_DATA_TYPE_NAME_SIZE 		32			// Data type name size
#define	DT_DATA_TYPE_PATH_SIZE		64			// Max size of DLL path
#define	DT_DATA_TYPE_FUNC_SIZE	 	32			// Max size of function name

// Flags
#define	DT_FLAG_NUMERIC					0x0001
#define	DT_FLAG_POINTER					0x0002
#define	DT_FLAG_SUBSCRIPTS				0x0004
#define	DT_FLAG_COMPONENTS				0x0008
#define	DT_FLAG_REFERENCE				0x0010
#define DT_FLAG_EVAL_ONCE_CONSTRUCTOR	0x0020		// Constructor is EvalOnce: FIX- Obsolete, remove
#define DT_FLAG_EVAL_ONCE_BIN_OPS		0x0040		// Binary operators are EvalOnce
#define DT_FLAG_FIRST_USER_FLAG			0x0100

// RegFlags
#define DT_REG_FLAG_TEMP				0x0001		// Temporary in-memory only datatype

// Request new DataType ID (turns on DT_REG_FLAG_TEMP)
#define DT_DATA_TYPE_ID_NEW				-2

// DtInitialize 'Flag'
#define DT_INITIALIZE_BUILTINS_ONLY		"<BuiltinsOnly>"


/*
**	Define types
*/

typedef int							DT_DATA_TYPE_ID;
typedef uint32_t					DT_HASH_CODE;
struct DT_VALUE;
typedef	int							(*DT_DATA_TYPE_FUNC)( int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b );
typedef int							(*DT_SORT_FUNC)( DT_VALUE *SortArg1, DT_VALUE *SortArg2 );
struct HashEntryStructure;

struct DT_COLLECTOR;
struct BStreamStructure;

/*
**	Structure for datatype help
*/

struct DT_HELP
{
	const char
		    *Type,		// Used for documenting FROM/TO messages
						// Use "To:Datatype" or "From:Datatype"
			*HelpText;	// Arbitrary text w/ newlines

};


/*
**	Define structure for a data type
*/

struct DT_DATA_TYPE_STRUCTURE
{
	char	Name[		DT_DATA_TYPE_NAME_SIZE ],
			DllPath[	DT_DATA_TYPE_PATH_SIZE ],
			FuncName[	DT_DATA_TYPE_FUNC_SIZE ];

	int		Size;

	DT_DATA_TYPE_FUNC
			Func;

	short	Flags,		// Flags set by DataType function
			RegFlags;	// Flags set by DtRegister function

	DT_DATA_TYPE_ID
			ID;

	DT_HELP	*Help;		// Terminated by HelpText == NULL

	char	*Arg;		// Argument string in datatype.dat

	void	*Extra;		// Private pointer used by datatype func
						// Used by TypeDefine to hold type info

};

typedef DT_DATA_TYPE_STRUCTURE* DT_DATA_TYPE;

/*
**	Data type information structure (used by DT_MSG_INFO message)
*/

struct DT_INFO
{
	int		Flags,
			DefaultWidth,
			DefaultHeight,
			ElementCount,
			ElementMaximum;

	DT_DATA_TYPE
			*ElementDataType;

	char	**ElementNames;

	long	MemUsed;			// Does not include mem for individual instances

};


/*
**	Union used to hold any kind of data (sort of...)
*/

union DT_VALUE_RESULTS
{
	double	Number;							// Used for all numeric values

	const void
			*Pointer;						// Used for all unions & structs

	char	*DbgStr;						// Used in debugger

	float	Single;							// Used for special case
};


/*
**	Define structure used to return evaluated data
*/

struct DT_VALUE
{
	DT_DATA_TYPE
			DataType;

	DT_VALUE_RESULTS
			Data;

};


/*
**	Individual datatypes' structures
*/

typedef struct DtGenCurveKnotStructure
{
	long	Date;					// Date or time

	DT_VALUE
			Value;					// Corresponding value

} DT_GENCURVE_KNOT, DT_GCURVE_KNOT;

typedef struct DtGenCurveStructure
{
	int		KnotCount,				// Number of knots
			KnotAlloc;				// Allocated size of curve

	DT_GCURVE_KNOT
			*Knots;					// Knot buffer

} DT_GENCURVE, DT_GCURVE, DT_GRCURVE;

struct DT_ARRAY
{
	int		Size;					// Number of elements

	DT_VALUE
			*Element;				// Array elements

};

struct DT_POINTER_ARRAY
{
	int		Size;					// Number of elements

	DT_VALUE
			**Element;				// Array elements

};

typedef struct DtMatrixStructure
{
	int		NumRows,
			NumCols,
			NeedFill:1;

	DT_ARRAY
			*Rows;

} DT_MATRIX, DT_CMATRIX;

struct DT_REFERENCE
{
	long	RefCount;

	DT_VALUE
			Value;

	int		Busy;		// Currently operating on this reference inside DtFuncReference

};

struct DT_RTIME		// mimics RTIME definition in date.h
{
	DT_RDATE
			RDate;

	int		Offset;

	char	*Location;

};

struct DT_FUNC_X;

typedef double (*DT_FUNC_X_CALLBACK)( DT_FUNC_X *Self, double x );

struct DT_FUNC_X
{
	DT_FUNC_X_CALLBACK
			*Func;

	DT_DATA_TYPE
			Domain;

	DT_VALUE
			*Parameters;

};
#define	DtFuncXEval( FuncX_, x_ )		((*(FuncX_)->Func)( (FuncX_), (x_) ))



/*
**	Define macros
*/

#define	DT_VALUE_TO_POINTER( av )	((av)->Data.Pointer)

#define	DT_VALUE_TO_NUMBER( av )	((av)->Data.Number)

#define	DT_POINTER_TO_VALUE( av, p, pt )	\
		{									\
			(av)->DataType 		= (pt);		\
			(av)->Data.Pointer	= (p);		\
		}

#define	DT_NUMBER_TO_VALUE( av, n, nt )	\
		{									\
			(av)->DataType 		= (nt);		\
			(av)->Data.Number	= (n);		\
		}

#define	DT_OP( op, rv, av, bv )		\
		((*(av)->DataType->Func)( (op), (rv), (av), (bv) ))

#ifdef CLEAN_DT
#	define DTM_INIT( av )	\
	( memset( (av), 0, sizeof( *(av) )), (av)->DataType = DtNull, TRUE )
#else
#	define DTM_INIT( av )	\
	( (av)->Data.Number = 0.0, (av)->DataType = DtNull, TRUE )
#endif

#define	DTM_ASSIGN( rv, av )	\
		(((av)->DataType->Flags & DT_FLAG_NUMERIC) ?	\
			((*(rv) = *(av)), TRUE) \
		:	\
			(DTM_INIT(rv), DT_OP( DT_MSG_ASSIGN, (rv), (av), NULL )))

#define DTM_REPLACE( rv, av, Success )						\
{																	\
		if( av == rv )												\
			Success = TRUE;											\
		else if( !( (av)->DataType->Flags & DT_FLAG_REFERENCE ))	\
		{															\
			DTM_FREE( rv );											\
			*(rv) = *(av);											\
			DTM_INIT( av );											\
			(av)->DataType = DtNull;								\
			Success = TRUE;											\
		}															\
		else														\
		{															\
			DT_VALUE 												\
					TmpValue;										\
																	\
			Success = DTM_ASSIGN( &TmpValue, av );					\
			if( Success )											\
			{														\
				DTM_FREE( rv );										\
				*(rv) = TmpValue;									\
			}														\
			DTM_FREE( av );											\
			DTM_INIT( av );											\
			(av)->DataType = DtNull;								\
		}															\
}

#define	DTM_FREE( av )	\
		(((av)->DataType->Flags & DT_FLAG_NUMERIC) ? TRUE : ((*(av)->DataType->Func)( DT_MSG_FREE, NULL, (av), NULL )))

#define	DTM_ALLOC( rv, dt )	\
		(DTM_INIT(rv), (rv)->DataType = (dt),	\
			(((dt)->Flags & DT_FLAG_NUMERIC) ? (((rv)->Data.Number = 0.0),TRUE)	\
		:	\
			(((dt)->Func)( DT_MSG_ALLOCATE, (rv), NULL, NULL ))))

#define	DTM_HIGH_LIMIT( rv, dt )	\
		(DTM_INIT(rv), (rv)->DataType = (dt),			\
		(((dt)->Func)( DT_MSG_HIGH_LIMIT, (rv), NULL, NULL )))

#define	DTM_LOW_LIMIT( rv, dt )		\
		(DTM_INIT(rv), (rv)->DataType = (dt),			\
		(((dt)->Func)( DT_MSG_LOW_LIMIT, (rv), NULL, NULL )))

#define	DTM_TO( v_to, v_from )	\
		(memset( &(v_to)->Data, 0, sizeof( (v_to)->Data )),	\
		(*(v_from)->DataType->Func)( DT_MSG_TO, (v_to), (v_from), NULL )	\
				|| (*(v_to)->DataType->Func)( DT_MSG_FROM, NULL, (v_to), (v_from))	\
				|| ErrMore( "Can't convert %s to a %s", (v_from)->DataType->Name, (v_to)->DataType->Name ) \
				|| !DTM_INIT(v_to))

#define	DTM_FROM( v_to, v_from )	\
		(memset( &(v_to)->Data, 0, sizeof( (v_to)->Data )),	\
		(*(v_to)->DataType->Func)( DT_MSG_FROM, NULL, (v_to), (v_from))	\
				|| (*(v_from)->DataType->Func)( DT_MSG_TO, (v_to), (v_from), NULL )	\
				|| ErrMore( "Can't convert %s to a %s", (v_from)->DataType->Name, (v_to)->DataType->Name ) \
				|| !DTM_INIT(v_to))

/*
**	Iteration macro for iterating through multi-element datatypes:
**
**	Example:
**
**	int PrintElements( DT_VALUE *Value )
**	{
**		DT_VALUE	Enum,
**					ElemValue,
**					StrValue;
**
**		DTM_FOR( &Enum, Value )
**		{
**			StrValue.DataType = DtString;
**			DTM_TO( &StrValue, &Enum )
**			printf( "%s:\n", (char *) DT_VALUE_TO_POINTER( &StrValue ));
**			DTM_FREE( &StrValue );
**
**			DtSubscriptGetCopy( Value, &Enum, &ElemValue );
**			StrValue.DataType = DtString;
**			DTM_TO( &StrValue, &Enum );
**			printf( "%s\n\n", (char *) DT_VALUE_TO_POINTER( &StrValue ));
**			DTM_FREE( &StrValue );
**			DTM_FREE( &ElemValue );
**
**			if( UserAbort())
**			{
**				DTM_FOR_BREAK( &Enum, Value );
**				break;
**			}
**		}
**	}
*/

#define DTM_FOR( EnumPtr, ValuePtr )	\
		for( DTM_INIT(EnumPtr), (EnumPtr)->DataType = DtNull; DT_OP( DT_MSG_SUBSCRIPT_NEXT, (EnumPtr), (ValuePtr), NULL ); )

#define DTM_FOR_BREAK( EnumPtr, ValuePtr )	\
		{																	\
			DT_OP( DT_MSG_SUBSCRIPT_LAST, (EnumPtr), (ValuePtr), NULL );	\
			DT_OP( DT_MSG_SUBSCRIPT_NEXT, (EnumPtr), (ValuePtr), NULL );	\
		}

// StructPtr doesn't have to point to a structure, could be an array too, but I couldn't think of a better name
#define DTM_FOR_VALUE( SubscriptPtr, ValuePtr, StructPtr ) \
		for( DTM_INIT( SubscriptPtr ), DTM_INIT( ValuePtr ); DT_OP( DT_MSG_SUBSCRIPT_VALUE_NEXT, (SubscriptPtr), (StructPtr), (ValuePtr) ); DTM_FREE( ValuePtr ))

#define DTM_FOR_VALUE_BREAK( SubscriptPtr, ValuePtr, StructPtr ) \
		{  														 \
			DTM_FREE( ValuePtr );								 \
			DT_OP( DT_MSG_SUBSCRIPT_VALUE_LAST, (SubscriptPtr), (StructPtr), (ValuePtr) );	\
			DTM_FREE( ValuePtr );															\
			DT_OP( DT_MSG_SUBSCRIPT_VALUE_NEXT, (SubscriptPtr), (StructPtr), (ValuePtr) );	\
		}


/*
**	Macros for referencing matrix entries
*/

#define	DT_MATRIX_ROW(Matrix,Row)			((Matrix)->Rows->Element[(Row)].Data.Pointer)
#define	DT_MATRIX_ELEM(Matrix,Row,Column)	((DT_VECTOR *)DT_MATRIX_ROW(Matrix,Row))->Elem[(Column)]
#define	DT_MATRIX_FILL(Matrix)				( ((DT_MATRIX *)(Matrix))->NeedFill ? DtMatrixFill( (DT_MATRIX *)(Matrix) ) : TRUE )

#define	DT_CMATRIX_ROW(Matrix,Row)			((Matrix)->Rows->Element[(Row)].Data.Pointer)
#define	DT_CMATRIX_ELEM(Matrix,Row,Column)	((DT_CVECTOR *)DT_CMATRIX_ROW(Matrix,Row))->Elem[(Column)]
#define	DT_CMATRIX_FILL(Matrix)				( ((DT_CMATRIX *)(Matrix))->NeedFill ? DtCMatrixFill( (DT_CMATRIX *)(Matrix) ) : TRUE )


/*
** Macros for setting a structure component, etc.
** Datum must be allocated by callers, and becomes owned by the structure.
*/

#define DT_COMPONENT_GIVE( Struct, Component, Datum ) \
{                                                     \
DT_VALUE IndexValue ;                                 \
DTM_INIT( &IndexValue );							  \
IndexValue.DataType = DtString;                       \
IndexValue.Data.Pointer = (void*) Component;          \
DtComponentReplace( Struct, &IndexValue, Datum);      \
}

#define DT_SUBSCRIPT_GIVE( Struct, Subscript, Datum ) \
{                                                     \
DT_VALUE IndexValue ;                                 \
DTM_INIT( &IndexValue );							  \
IndexValue.DataType = DtDouble;                       \
IndexValue.Data.Number = Subscript;                   \
DtSubscriptReplace( Struct, &IndexValue, Datum);      \
}

#define DT_HASH_MIX( a, b, c) \
{ \
  a=a-b;  a=a-c;  a=a^(c>>13); \
  b=b-c;  b=b-a;  b=b^(a<<8);  \
  c=c-a;  c=c-b;  c=c^(b>>13); \
  a=a-b;  a=a-c;  a=a^(c>>12); \
  b=b-c;  b=b-a;  b=b^(a<<16); \
  c=c-a;  c=c-b;  c=c^(b>>5);  \
  a=a-b;  a=a-c;  a=a^(c>>3);  \
  b=b-c;  b=b-a;  b=b^(a<<10); \
  c=c-a;  c=c-b;  c=c^(b>>15); \
}


/*
**	Variables
*/

DLLEXPORT HashStructure
		*DtNamesHash,
		*DtReferencesHash;

DLLEXPORT int
		DtReferencesNum,		// Number of Reference Objects
		DtReferencesCount;		// Number of References to Reference Objects

DLLEXPORT DT_DATA_TYPE			// Pre-defined data types:
		DtAny,					//   Flag to allow any datatype (similar to Null)
		DtArray,				//   Arrays of anything
		DtBinary,				//   Binary data
		DtCMatrix,				//   Matrix of complex
		DtCVector,				//   Vector of complex
		DtComplex,				//   Complex number
		DtCurve,	  			//   Curves (int rates, fwd pnts...)
		DtDate,	  				//   Dates
		DtDouble,  				//   Doubles (recast longs, etc...)
		DtError,				//   Just an error (similar to DtNull)
  	    DtFloat,                //   Alternate double representation
		DtGCurve,	  			//   Curves (int rates, fwd pnts...)
		DtGRCurve,	  			//   Curves (int rates, fwd pnts...)
		DtGStructure,	  		//   Structure ( keys are arbitrary DtValues instead of just strings )
		DtMathOperation,		//   Math operation [[not a real datatype]]
		DtMatrix,				//   Matrix of reals
		DtNull,					//   NULL (undefined value)
		DtPointerArray,			//	 Array of pointers to DT_VALUEs
		DtRCurve,	  			//   Relative Curves (int rates, fwd pnts...)
		DtRDate,				//	 Relative Dates
		DtReference,			//   Reference to another DT_VALUE
		DtRTime,				//	 Relative Times
		DtString,  				//   Character strings
		DtStructure,			//   Structures of anything
		DtStructureCase,		//   Case sensitive structures of anything
		DtSymbolCase,			//   Case sensitive symbols
		DtTCurve,				//	 Time Curves
		DtTime,	  				//   Times
		DtVector;				//   Vector of reals

DLLEXPORT HEAP
		*DtValueHeap;			// Heap to allocate DT_VALUE's from


/*
**
*/

DLLEXPORT DT_COMPLEX
		CxZero,
		CxOne;


/*
**	Datatype class functions (only DtNull is needed outside here)
*/

DLLEXPORT int
		DtFuncNull(					int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b );

struct DtLessDouble
{
    bool operator()( DT_VALUE const& lhs, DT_VALUE const& rhs ) const
    {
        return lhs.Data.Number < rhs.Data.Number;
    }
};

/*
**	Prototype functions
*/

DLLEXPORT void
		DtFreeInfo(					DT_INFO *DtInfo );

DLLEXPORT int
		DtInitialize(				const char *Path, const char *TableName ),
		DtLoadTable(				const char *Path, const char *TableName );

DLLEXPORT DT_DATA_TYPE
		DtRegister(					const char *Name,
									const char *DllPath, const char *FuncName, DT_DATA_TYPE_FUNC Func,
									DT_DATA_TYPE_ID DtID, const char *Arg, void *Extra ),
		DtRegisterExternal(			const char *Name, const char *DllPath, const char *FuncName, DT_DATA_TYPE_ID DtID, const char *Arg ),
		DtRegisterExtra(			const char *Name, DT_DATA_TYPE_FUNC Func, void *Extra ),
		DtRegisterInternal(			const char *Name, DT_DATA_TYPE_FUNC Func, DT_DATA_TYPE_ID DtID );

DLLEXPORT DT_DATA_TYPE
		DtFromName(					const char *DataName ),
		DtFromID(					DT_DATA_TYPE_ID DtID ),
		DtLookupOnly(				const char *DataName );

DLLEXPORT DT_DATA_TYPE_ID nextAvailableDtID(void);

DLLEXPORT const char
		*DtNameFromType(			DT_DATA_TYPE DataType );

DLLEXPORT int
		DtToStructure(				DT_DATA_TYPE DataType, DT_VALUE *Result );

DLLEXPORT int
		DtArrayDelete(				DT_ARRAY *Array, int Element, int Count ),
		DtArrayInsert(				DT_ARRAY *Array, int Element, int Count ),
		DtArraySortTable(			DT_ARRAY *Array, const char * const * KeyStrTab ),
		DtArrayValidate(			DT_ARRAY *Array, DT_DATA_TYPE DataType );

DLLEXPORT DT_ARRAY
		*DtArrayAlloc(				int Size );


DLLEXPORT DT_VECTOR
		*DtVectorSubVector(			DT_VECTOR *Vector,  int Start, int Size );

DLLEXPORT DT_CURVE
		*DtCurveDeleteKnotByDate(	DT_CURVE *Curve,  DT_VALUE *ArgPointDate ),
		*DtCurveDeleteKnotByIndex(	DT_CURVE *Curve,  DT_VALUE *PointIndex ),
		*DtCurveExcludeByRank(		DT_CURVE *Curve,  DT_VALUE *Limit ),
		*DtCurveSubCurve(			DT_CURVE *Curve,  int Index, int Size );


DLLEXPORT DT_GCURVE
		*DtGCurveAlloc(				int KnotCount ),
		*DtGCurveCopy(				DT_GCURVE *Curve ),
		*DtGCurveSort(				DT_GCURVE *Curve ),
		*DtGCurveOperate(			int Msg, DT_GCURVE *ACurve, DT_GCURVE *BCurve ),
		*DtGCurveDeleteKnotByDate(	DT_GCURVE *Curve,  DT_VALUE *ArgPointDate ),
		*DtGCurveDeleteKnotByIndex(	DT_GCURVE *Curve,  DT_VALUE *PointIndex ),
		*DtGCurveDeleteKnots(		DT_GCURVE *Curve1, DT_GCURVE *Curve2 ),
		*DtGCurveSubCurve(			DT_GCURVE *Curve,  int Index, int Size );

DLLEXPORT int
		DtGCurveAppend(				DT_GCURVE *Curve, DT_GCURVE_KNOT *Point ),
		DtGCurveInsertKnot(			DT_GCURVE *Curve, DT_GCURVE_KNOT *Point ),
		DtGCurveInsertKnotByDate(	DT_GCURVE *Curve, DT_VALUE *PointDate, DT_VALUE *PointValue ),
		DtGCurveSearch(				DT_GCURVE *Curve, long KnotDate, int *KnotNum );

DLLEXPORT void
		DtGCurveFree(				DT_GCURVE *Curve );

DLLEXPORT int
		DtCurveInsertKnotByDate(	DT_CURVE *Curve, DT_VALUE *PointDate, DT_VALUE *PointValue ),
		DtCurveToDates(				DT_CURVE *Curve, DT_VALUE *Result, DT_DATA_TYPE Type ),
		DtCurveToVector(			DT_CURVE *Curve, DT_VALUE *Result ),
		DtCurveToValues(			DT_CURVE *Curve, DT_VALUE *Result );

DLLEXPORT int
		DtCurveFromDates(			DT_VALUE *DateArray, double Value, DT_VALUE *CurveResult );

DLLEXPORT int
		DtGCurveOperateScalar(		int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b );

DLLEXPORT DT_RCURVE
		*DtRCurveSort(				DT_RCURVE *Curve, long BaseDate, const char *Calendar1, const char *Calendar2 );

DLLEXPORT DT_GRCURVE
		*DtGRCurveSort(				DT_GRCURVE *Curve, long BaseDate, const char *Calendar1, const char *Calendar2 );

DLLEXPORT int
		DtSize(						DT_VALUE const *Value ),
		DtMemSize(					DT_VALUE const *Value ),
		DtHashAsBinary(				DT_VALUE *Value, DT_HASH_CODE *CurHash ),
		DtHashMem(					void const *Mem, unsigned int Len, DT_HASH_CODE *CurHash ),
		DtHashPortable(				DT_VALUE *Value, DT_HASH_CODE *CurHash );

DLLEXPORT unsigned long
		DtHashQuick(				unsigned char *Key, unsigned long Len, unsigned long PrevHash ),
		DtHashiQuick(				unsigned char *Key, unsigned long Len, unsigned long PrevHash );

DLLEXPORT int
		DtAppend(					DT_VALUE *ArrayValue, DT_VALUE *ElemValue ),
		DtAppendNoCopy(				DT_VALUE *ArrayValue, DT_VALUE *ElemValue ),
		DtAppendNumber(				DT_VALUE *ArrayValue, DT_DATA_TYPE Type, double Number ),
		DtAppendPointer(			DT_VALUE *ArrayValue, DT_DATA_TYPE Type, const void *Pointer );

DLLEXPORT int
		DtComponentSet( 			DT_VALUE *Struct, const char *Component, DT_VALUE *Value ),
		DtComponentSetPointer(		DT_VALUE *Struct, const char *Component, DT_DATA_TYPE Type, const void *Pointer ),
		DtComponentSetNumber(		DT_VALUE *Struct, const char *Component, DT_DATA_TYPE Type, double Number ),
		DtComponentDestroy(			DT_VALUE *Struct, const char *Component ),
		DtComponentReplace(			DT_VALUE *Value, DT_VALUE *Subscript, DT_VALUE *NewElement ),
		DtComponentReplacePointer(  DT_VALUE *Struct, const char *Component, DT_DATA_TYPE Type, const void *Pointer );

DLLEXPORT DT_ARRAY
		*DtArrayFromDoubles(		double *Values, int Size );

DLLEXPORT DT_VALUE
		*DtComponentGet(			DT_VALUE *Struct, const char *Component, DT_VALUE *Default ),
		DtStringReverse( 			DT_VALUE *String ),
		DtArrayAllocAndInit(		int Size, DT_VALUE *InitValue ),
		DtArrayReverse(				DT_VALUE *Array ),
		DtArrayExtract(				DT_ARRAY *Array1, int Element, int Countb ),
		DtArrayConcatenate(			DT_ARRAY *Arg1, DT_ARRAY *Array2, int Element, int Countb );

DLLEXPORT int
		DtCvt2Doubles(				DT_VALUE *Value, int KeepFlag, int DoAllNested ),
		DtArray2Doubles(			DT_ARRAY *Array, int KeepFlag, int DoAllNested ),
		DtStructure2Doubles(		DT_STRUCTURE *Struct, int KeepFlag ),
		DtArrayConcatenateInPlace(	DT_ARRAY *Arg1, DT_ARRAY *Array2, int Element, int Countb ),
		DtArraySum(					DT_VALUE *RetVal, DT_ARRAY *Array );

DLLEXPORT const void
		*DtComponentGetPointer(		DT_VALUE *Struct, const char *Component, DT_DATA_TYPE Type, const void *Default );

DLLEXPORT double
		DtComponentGetNumber(		DT_VALUE *Struct, const char *Component, DT_DATA_TYPE Type, double Default );

DLLEXPORT int
		DtSubscriptSet( 			DT_VALUE *ArrayValue, int Element, DT_VALUE *Value ),
		DtSubscriptSetPointer(		DT_VALUE *ArrayValue, int Element, DT_DATA_TYPE Type, const void *Pointer ),
		DtSubscriptSetNumber(		DT_VALUE *ArrayValue, int Element, DT_DATA_TYPE Type, double Number ),
		DtSubscriptDestroy(			DT_VALUE *ArrayValue, int Element ),
		DtSubscriptReplace(			DT_VALUE *Value, DT_VALUE *Subscript, DT_VALUE *NewElement ),
		DtSubscriptReplacePointer(  DT_VALUE *Struct,     int Element, DT_DATA_TYPE Type, const void *Pointer );


DLLEXPORT DT_VALUE
		*DtSubscriptGet(			DT_VALUE *ArrayValue, int Element, DT_VALUE *Default ),
		*DtSubscriptGetComponent(	DT_VALUE *TableValue, int Element, const char *Component, DT_VALUE *Default );

DLLEXPORT const void
		*DtSubscriptGetPointer(		DT_VALUE *ArrayValue, int Element, DT_DATA_TYPE Type, const void *Default );

DLLEXPORT double
		DtSubscriptGetNumber(		DT_VALUE *ArrayValue, int Element, DT_DATA_TYPE Type, double Default );

DLLEXPORT int
		DtSubscriptGetCopy(			DT_VALUE *Value, DT_VALUE *Subscript, DT_VALUE *ReturnedElement );

DLLEXPORT int
		DtStructureUnion(			DT_VALUE *ValueA, DT_VALUE *ValueB );

DLLEXPORT DT_TCURVE 
		*TCurveFromDtStructure(		DT_VALUE *UserSpec );

DLLEXPORT const char
		*DtMsgToString(				int Msg );

DLLEXPORT int
		DtSort(						DT_VALUE *ArrayValue, DT_SORT_FUNC SortFunc ),
		DtSearchTable(				DT_VALUE *Table, DT_VALUE *SearchVal, const char *Component, int *Index );

DLLEXPORT int
		DtMatrixEqualityMsg( 		int	Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b );

DLLEXPORT double
		DtMatrixInnerProduct(		DT_MATRIX *MatrixA, DT_MATRIX *MatrixB ),
		DtMatrixPlusReduce(			DT_MATRIX *MatrixA );

DLLEXPORT DT_MATRIX
		*DtMatrixAlloc(				int NumRows, int NumCols ),
		*DtMatrixCopy(				const DT_MATRIX *Source ),
		*DtMatrixOperateScalar(		int Message, DT_MATRIX *Matrix, double Scalar ),
		*DtMatrixMultiply(			const DT_MATRIX *MatrixA, const DT_MATRIX *MatrixB ),
		*DtMatrixTranspose(			const DT_MATRIX *Matrix ),
		*DtMatrixIdentity(			int NumRows, int NumCols ),
		*DtMatrixInvert(			const DT_MATRIX *Matrix ),
		*DtMatrixSolve(				const DT_MATRIX *LinearSystem, const DT_MATRIX *Solutions ),
		*DtMatrixComponentMultiply(	const DT_MATRIX *MatrixA, const DT_MATRIX *MatrixB ),
		*DtMatrixLUBacksubstitute(	const DT_MATRIX *LUDecomp, const int *SwapIndex, const DT_MATRIX *Solutions );

DLLEXPORT int
		*DtMatrixLUDecompose(		DT_MATRIX *Matrix ),
		DtMatrixEqual(				const DT_MATRIX *MatrixA, const DT_MATRIX *MatrixB ),
		DtMatrixFill(				const DT_MATRIX *MatrixA );

DLLEXPORT void
		DtMatrixFree(				DT_MATRIX *Matrix );

DLLEXPORT DT_CMATRIX
		*DtCMatrixAlloc(			int NumRows, int NumCols ),
		*DtCMatrixCopy(				const DT_CMATRIX *Source ),
		*DtCMatrixOperateScalar(	int Message, DT_CMATRIX *CMatrix,DT_COMPLEX *Scalar ),
		*DtCMatrixMultiply(			const DT_CMATRIX *MatrixA, const DT_CMATRIX *MatrixB ),
		*DtCMatrixTranspose(		const DT_CMATRIX *Matrix ),
		*DtCMatrixIdentity(			int NumRows, int NumCols ),
		*DtCMatrixInvert(			const DT_CMATRIX *Matrix ),
		*DtCMatrixSolve(			const DT_CMATRIX *LinearSystem, const DT_CMATRIX *Solutions ),
		*DtCMatrixLUBacksubstitute(	const DT_CMATRIX *LUDecomp, const int *SwapIndex, const DT_CMATRIX *Solutions );

DLLEXPORT int
		*DtCMatrixLUDecompose(		DT_CMATRIX *Matrix ),
		DtCMatrixEqual(				const DT_CMATRIX *MatrixA, const DT_CMATRIX *MatrixB ),
		DtCMatrixFill(				const DT_CMATRIX *MatrixA );

DLLEXPORT void
		DtCMatrixFree(				DT_CMATRIX *Matrix );

DLLEXPORT int
		DtFuncXEvalVector(			DT_FUNC_X *FuncX, DT_VECTOR *Vector );

DLLEXPORT int
		DtHashKeys(					DT_VALUE *Value );

DLLEXPORT long
		DtMemoryUsage(				DT_VALUE *ResultsStructure );


DLLEXPORT int
		DtMathOperateOnAll(			DT_VALUE *Result, DT_VALUE *Value, DT_MATH_OPERATION *FuncInfo );


/*
**	BStream functions (only available if bstream.h included)
*/

DLLEXPORT int
		DtValueToStream(			struct BStreamStructure *BStream, DT_DATA_TYPE DataType, DT_VALUE const *Value ),
		DtValueFromStream(			struct BStreamStructure *BStream, DT_DATA_TYPE DataType, DT_VALUE *Value );

DLLEXPORT bool
		DtValueToXmlString(			CC_NS(std,ostream)& os, DT_VALUE* value );

DLLEXPORT DT_COLLECTOR
		*DtCollectorNew(			void );

DLLEXPORT void
		DtCollectorDestroy(			DT_COLLECTOR	*Collector );

DLLEXPORT int
		DtCollectorCollect(			DT_COLLECTOR	*Collector, DT_DATA_TYPE DataType ),
		DtCollectorToStream(		DT_COLLECTOR	*Collector, struct BStreamStructure *BStream ),
		DtCollectorFromStream(		DT_COLLECTOR	*Collector, struct BStreamStructure *BStream );

DLLEXPORT DT_DATA_TYPE
		DtCollectorType(			DT_COLLECTOR	*Collector, int Id );

DLLEXPORT HASH_ENTRY
		*DtHashStatsToStruct(		HashStructure *Hash, DT_VALUE *Value );
		
// A DT_VALUE wrapper with almost auto_ptr semantics

struct dt_value_var;

// internal class - don't use directly
struct dt_value_var_ref
{
    dt_value_var_ref(DT_VALUE const& val) : m_val(val) {}
    DT_VALUE m_val;
};

struct dt_value_var
{
	// this constructor takes ownership of the dt_value you give it
    explicit dt_value_var( DT_VALUE* pval = 0 )                          { if( pval ) m_val = *pval; else DTM_INIT( &m_val ); }
    explicit dt_value_var( DT_DATA_TYPE dt    )                          { DTM_ALLOC( &m_val, dt ); }
             dt_value_var( dt_value_var& src  ) : m_val( src.release() ) {}
            ~dt_value_var(                    )                          { DTM_FREE( &m_val ); }

	dt_value_var& operator=(dt_value_var& a) { if( &a != this ) { DTM_FREE( &m_val ); m_val = a.release(); } return *this; }

    DT_VALUE const* get() const { return &m_val; }
    DT_VALUE*       get()       { return &m_val; }

    DT_VALUE const* operator->() const { return get(); }
    DT_VALUE*       operator->()       { return get(); }
    DT_VALUE const& operator*()  const { return m_val; }
    DT_VALUE&       operator*()        { return m_val; }

	DT_VALUE release() { DT_VALUE tmp = m_val; DTM_INIT( &m_val ); return tmp; }

	void reset( DT_VALUE* pval = 0 ) { DTM_FREE( &m_val ); if( pval ) m_val = *pval; else DTM_INIT( &m_val ); }
	void reset( DT_DATA_TYPE dt    ) { DTM_FREE( &m_val ); DTM_ALLOC( &m_val, dt ); }

    // special conversion with auxiliary type to enable copies and assignments
    dt_value_var(dt_value_var_ref const& rhs) : m_val(rhs.m_val) {}
    operator dt_value_var_ref() { return dt_value_var_ref(release());}

    dt_value_var& operator=( dt_value_var_ref const& rhs ) { DTM_FREE( &m_val ); m_val = rhs.m_val; return *this; }

private:
	DT_VALUE m_val;
};

EXPORT_CPP_DATATYPE CC_NS(std,string) DtToString( DT_VALUE* val );

// hack to allow us to specialize addin args which need a DT_VALUE
class dt_value_addin_arg
{
public:
	dt_value_addin_arg( DT_VALUE* val ) : m_value( val ) {}

	DT_VALUE* m_value;
};

#endif

