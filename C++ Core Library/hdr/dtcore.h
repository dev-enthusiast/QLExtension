/* $Header: /home/cvs/src/datatype/src/dtcore.h,v 1.31 2011/10/14 21:15:22 khodod Exp $ */
/****************************************************************
**
**	DTCORE.H - Lower level stuff split off from datatype.h
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/datatype/src/dtcore.h,v 1.31 2011/10/14 21:15:22 khodod Exp $
**
****************************************************************/

#ifndef IN_DTCORE_H
#define IN_DTCORE_H

// For KA_CURVE_KNOT. Note that everyone should include kool_ade.h anyway.
#if !defined( IN_KOOL_ADE_H )
#include <kool_ade.h>
#endif

/*
**	Define constants
*/

// Interpolation methods
#define	DT_EXTRAPOLATE				0x0000
#define	DT_INTERPOLATE_ONLY	   		0x0001
#define	DT_INTERPOLATE_FLAT	   		0x0002
#define	DT_KNOTS_ONLY		   		0x0004
#define	DT_INTERPOLATE_STEP	   		0x0008
#define	DT_INTERPOLATE_PRESTEP 		0x0010
#define	DT_INTERPOLATE_LOG	 		0x0020
#define	DT_INTERPOLATE_CUBIC	 	0x0040
#define	DT_INTERPOLATE_LOG_SMOOTH	0x0080
#define DT_KNOTS_ONLY_ZAP		    (0x0100 | DT_KNOTS_ONLY)

// Average methods
#define DT_ARITHMETIC_MEAN			0x0001
#define DT_GEOMETRIC_MEAN			0x0002
#define DT_HARMONIC_MEAN			0x0004
#define DT_QUADRATIC_MEAN			0x0008
#define DT_WEIGHTED_MEAN			0x0010
#define DT_MEDIAN					0x0020	
#define DT_MIDRANGE					0x0040
#define DT_SUM						0x0080

/*
**	Define messages sent to data types
*/

#define	DT_MSG_START				0x0001		// a->DataType->Size = #of bytes
#define	DT_MSG_END					0x0002
#define	DT_MSG_ALLOCATE				0x0003		// r = new
#define	DT_MSG_FREE					0x0004		// free( a )
#define	DT_MSG_TO					0x0005		// r = to( a )
#define	DT_MSG_SIZE					0x0006		// r->Number = (double) size;
#define	DT_MSG_INFO					0x0007		// r->Pointer = info( a )
#define	DT_MSG_MEMSIZE				0x0008		// r->Number = (double) memsize;
#define	DT_MSG_FROM					0x0009		// a = from( b )
#define	DT_MSG_HIGH_LIMIT			0x000A		// r = new with high limit
#define	DT_MSG_LOW_LIMIT			0x000B		// r = new with low limit
#define	DT_MSG_TO_STREAM			0x000C		// r->Pointer contains BSTREAM
#define	DT_MSG_FROM_STREAM			0x000D		// r->Pointer contains BSTREAM
#define	DT_MSG_HASH_PORTABLE		0x000E		// r->Pointer is ulong*, use DtHashPortable to call
#define DT_MSG_HASH_QUICK			0x000F		// r->Pointer is ulong*, quick and dirty hash (machine dependant, do *not* use for streaming)

#define	DT_MSG_ADD					0x0010		// r = a + b
#define	DT_MSG_SUBTRACT				0x0011		// r = a - b
#define	DT_MSG_MULTIPLY				0x0012		// r = a * b
#define	DT_MSG_DIVIDE				0x0013		// r = a / b
#define	DT_MSG_AND					0x0014		// r = a & b
#define	DT_MSG_SHIFT				0x0015		// r = a shifted by b
#define	DT_MSG_UNION				0x0016		// r = a merged with b
#define	DT_MSG_AND_NC				0x0017		// r = a & b (w/no copy--meaningless)

#define	DT_MSG_ASSIGN				0x0020		// r = a
#define	DT_MSG_ASSIGN_ADD			(DT_MSG_ASSIGN | DT_MSG_ADD)		// a += b
#define	DT_MSG_ASSIGN_SUBTRACT		(DT_MSG_ASSIGN | DT_MSG_SUBTRACT)	// a -= b
#define	DT_MSG_ASSIGN_MULTIPLY		(DT_MSG_ASSIGN | DT_MSG_MULTIPLY)	// a *= b
#define	DT_MSG_ASSIGN_DIVIDE		(DT_MSG_ASSIGN | DT_MSG_DIVIDE)		// a /= b
#define	DT_MSG_ASSIGN_AND			(DT_MSG_ASSIGN | DT_MSG_AND)		// a &= b
#define	DT_MSG_ASSIGN_SHIFT			(DT_MSG_ASSIGN | DT_MSG_SHIFT)		// a shifted by b
#define	DT_MSG_ASSIGN_AND_NC		(DT_MSG_ASSIGN | DT_MSG_AND_NC)		// a &= b (b is not copied)

#define	DT_MSG_EQUAL				0x0040		// r = (double) (a == b)
#define	DT_MSG_NOT_EQUAL			0x0041		// r = (double) (a != b)
#define	DT_MSG_GREATER_THAN			0x0042		// r = (double) (a > b)
#define	DT_MSG_LESS_THAN			0x0043		// r = (double) (a < b)
#define	DT_MSG_GREATER_OR_EQUAL		0x0044		// r = (double) (a >= b)
#define	DT_MSG_LESS_OR_EQUAL		0x0045		// r = (double) (a <= b)
#define	DT_MSG_COMPARE				0x0046		// r = (double) (a <=> b) (like strcmp)

#define	DT_MSG_SET					0x0050		// Every element of a set to b
#define	DT_MSG_SETV					0x0051		// Every valid element of a set to b (replaces a*0+b)  -ted22Jun05

#define	DT_MSG_UNARY_MINUS			0x0081		// r = -a
#define	DT_MSG_INCREMENT			0x0082		// ++a
#define	DT_MSG_DECREMENT			0x0083		// --a
#define	DT_MSG_INVERT				0x0084		// a = 1/a
#define	DT_MSG_DEREFERENCE			0x0085		// r->Pointer = &(*a)
#define	DT_MSG_DEREFERENCE_VALUE	0x0086		// r = *a
#define	DT_MSG_MATH_OPERATE_ON_ALL	0x0087	 	// r = (*b->Pointer)( a[0..n] )

#define	DT_MSG_SUBSCRIPT			0x0100		// r->Pointer = &a[ b ]
#define	DT_MSG_SUBSCRIPT_REPLACE	0x0101		// a[ b ] = r and free old a[ b ]
#define	DT_MSG_SUBSCRIPT_VALUE		0x0102		// r = a[ b ]
#define	DT_MSG_SUBSCRIPT_DESTROY	0x0103		// Destroy( a[ b ] )
#define	DT_MSG_SUBSCRIPT_NEXT		0x0104		// Replace r with next subscript of a
#define	DT_MSG_SUBSCRIPT_LAST		0x0105		// Replace r with last subscript of a
#define DT_MSG_SUBSCRIPT_VALUE_NEXT	0x0106		// r = next subscript of a, b = a[ r ]
#define DT_MSG_SUBSCRIPT_VALUE_LAST 0x0107		// r = last subscript of a, b = a[ r ]
#define	DT_MSG_COMPONENT			0x0200		// r->Pointer = &a.b
#define	DT_MSG_COMPONENT_REPLACE	0x0201		// a.b = r and free old a.b
#define	DT_MSG_COMPONENT_VALUE		0x0202		// r = a.b
#define	DT_MSG_COMPONENT_DESTROY	0x0203		// Destroy( a.b )

#define	DT_MSG_TRUTH				0x0300		// returned value TRUE or FALSE (on a)
#define	DT_MSG_QUERY				0x0320		// r = Query( a ), where Query is in (b)

#define DT_MSG_FORMAT				0x0400		// r->Pointer contains DT_MSG_FORMAT_INFO
												// include <dtformat.h> to use this

#define	DT_MSG_PRINT				0x0500		// print contents of b 'into' a
#define	DT_MSG_PRINT_START			0x0501		// start of block printing
#define	DT_MSG_PRINT_END			0x0502		// end of block printing

#define DT_MSG_CALL_MEMBER			0x0600		// a.b(...)
#define DT_MSG_CALL_MEMBER_BLOCK	0x0601		// a.b(...) { ... }
												// b->Pointer is DT_FUNC_ARGS
												// r is not used

#define DT_MSG_TO_XML_STRING		0x0602		/* r->Data.Pointer contains ostream* */

#define	DT_MSG_USER					0x1000


#define DT_EPSILON                  1e-6

/*
**	Individual datatypes' structures
*/

typedef long DT_TIME;
typedef long DT_DATE;
typedef long DT_RDATE;
typedef double DT_DOUBLE;

typedef char *DT_STRING;
typedef struct HashStructure DT_STRUCTURE;


/*
**  DT_CURVE_KNOT is based on KA_CURVE_KNOT. TSDB_POINT is asserted to be
**  the same. Note that TSDB_POINT accesses the members as x and y while 
**  DT_CURVE_KNOT calls them Date and Value. Keeping DT_CURVE_KNOT and
**  TSDB_POINT identical in their memory structure allows us to cast from
**  one to the other.
*/

typedef KA_CURVE_KNOT DT_CURVE_KNOT;

struct DT_CURVE
{
	int		KnotCount,				// Number of knots
			KnotAlloc;				// Allocated size of curve

	DT_CURVE_KNOT
			*Knots;					// Knot buffer
};

typedef DT_CURVE DT_RCURVE, DT_TCURVE;

struct DT_BINARY
{
	unsigned int
			Size;					// Size of memory block

	void	*Memory;				// Memory block

};

struct DT_COMPLEX
{
	double	Real,
			Imag;

};

struct DT_VECTOR
{
	int		Size,
			Alloc;

	double	*Elem;

};

struct DT_CVECTOR
{
	int		Size,
			Alloc;

	DT_COMPLEX
			*Elem;

};

struct DT_MATH_OPERATION
{
    char        *Name;

    double      (*DblFunc)( double Arg );

    DT_COMPLEX  *(*CxFunc)( DT_COMPLEX *Arg, DT_COMPLEX *Result );

    int         RangeCheck;			// True if High/Low should be checked

    int         LowLimitCondition;	// 0 for less than, 1 for less than or equal

    double      LowLimit;

    int         HighLimitCondition;	// 0 for greater than, 1 for greater than or equal

    double      HighLimit;

};


/*
**	Prototype functions
*/

DLLEXPORT double
		DtDoubleFunc( DT_MATH_OPERATION *FuncInfo, double Number );

DLLEXPORT DT_CURVE
		*DtCurveAdd(				DT_CURVE *Curve1, DT_CURVE	*Curve2	),
		*DtCurveAlign(				DT_CURVE *CurveA, DT_CURVE *CurveB, int Method ),
		*DtCurveAlloc(				int KnotCount ),
        *DtCurveAppendCurve(		DT_CURVE *Curve1,      DT_CURVE *Curve2 ),
		*DtCurveAssign(				DT_CURVE *TargetCurve, DT_CURVE *SourceCurve ),
		*DtCurveBucketize(			DT_CURVE *SourceCurve, long StartDate, long EndDate ),
		*DtCurveCopy(				DT_CURVE *Curve ),
		*DtCurveDeleteKnots(		DT_CURVE *Curve1, DT_CURVE 	*Curve2 ),
		*DtCurveIntersection(		DT_CURVE *Curve1, DT_CURVE 	*Curve2 ),
		*DtCurveMerge(				DT_CURVE *CurveA, DT_CURVE *CurveB, long StartDate, long EndDate ),
		*DtCurveMult(				DT_CURVE *Curve1, DT_CURVE	*Curve2	),
		*DtCurveZap(				DT_CURVE *Curve, double Value, double Epsilon ),
		*DtCurveOperate(			DT_CURVE *Curve, int Operation, double Value ),
		*DtCurveOperateAsCurve(		DT_CURVE *Curve1, DT_CURVE	*Curve2, int Operation, int InterpMethod ),
		*DtCurveOperateAsStruct(	DT_CURVE *Curve1, DT_CURVE	*Curve2, int Operation ),
		*DtCurveSort(				DT_CURVE *Curve ),
		*DtCurveSortByValue(		DT_CURVE *Curve ),
		*DtCurveUnion(				DT_CURVE *Curve1, DT_CURVE *Curve2 ),
		*DtMakeDailyAllDaysCurve(	long StartDate, long LastDate );
		

DLLEXPORT DT_TCURVE
		*CurveToTCurve( DT_CURVE *Curve, const char *TimeZone, int Hour, int Minute, int Second );

DLLEXPORT int
		DtCurveAddKnot(				DT_CURVE *Curve, DT_CURVE_KNOT 	*Point ),
		DtCurveAppend(				DT_CURVE *Curve, DT_CURVE_KNOT 	*Point ),
		DtCurveInsertKnot(			DT_CURVE *Curve, DT_CURVE_KNOT 	*Point ),
		DtCurveSearch(				DT_CURVE *Curve, long KnotDate, int *KnotNum );

DLLEXPORT void
		DtCurveFree(				DT_CURVE *Curve );

DLLEXPORT double
		DtCurveInterpolate(			DT_CURVE *Curve, long Date, int Method ),
		DtCurveInterpolateByIndex(		DT_CURVE *Curve, long Date, int Method, int KnotNum );

DLLEXPORT DT_COMPLEX
		*DtComplexNew( 				double x, double y ),
		*DtComplexCopy(				DT_COMPLEX *z ),

		*CxAbs(						DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxConj(					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxCos( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxCosh( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxCot( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxCoth( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxCsc( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxCsch( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxExp( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxLog( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxSec( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxSech( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxSin( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxSinh( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxSqrt(					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxTan( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxTanh( 					DT_COMPLEX *z, DT_COMPLEX* Answer ),
		*CxPow(						DT_COMPLEX *v, DT_COMPLEX *w );

DLLEXPORT double
		CxArg( 						DT_COMPLEX *z ),
		CxMod( 						DT_COMPLEX *z );


#define	CxAdd( v, w, Answer ) 		DtComplexAdd( 		(Answer) ? (Answer) : DtComplexNew( 0, 0 ), (v), (w) )
#define	CxSub( v, w, Answer ) 		DtComplexSubtract(	(Answer) ? (Answer) : DtComplexNew( 0, 0 ), (v), (w) )
#define	CxMult( v, w, Answer ) 		DtComplexMultiply(	(Answer) ? (Answer) : DtComplexNew( 0, 0 ), (v), (w) )
#define	CxMultConj( v, w, Answer ) 	DtComplexMultiplyConj(	(Answer) ? (Answer) : DtComplexNew( 0, 0 ), (v), (w) )
#define	CxDiv( v, w, Answer ) 		DtComplexDivide(	(Answer) ? (Answer) : DtComplexNew( 0, 0 ), (v), (w) )
#define CxInit( x, y, Answer )		( (Answer) ? ( (Answer)->Real = (x), (Answer)->Imag = (y), (Answer) ) : DtComplexNew( (x), (y) ))
#define	CxEqual( v, w )				( (v)->Real == (w)->Real && (v)->Imag == (w)->Imag )

DLLEXPORT DT_COMPLEX
		*DtComplexAdd(				DT_COMPLEX *z, DT_COMPLEX *v, DT_COMPLEX *w ),
		*DtComplexSubtract(			DT_COMPLEX *z, DT_COMPLEX *v, DT_COMPLEX *w ),
		*DtComplexMultiply(			DT_COMPLEX *z, DT_COMPLEX *v, DT_COMPLEX *w ),
		*DtComplexMultiplyConj(		DT_COMPLEX *z, DT_COMPLEX *v, DT_COMPLEX *w ),
		*DtComplexDivide(			DT_COMPLEX *z, DT_COMPLEX *v, DT_COMPLEX *w );

DLLEXPORT void
		DtComplexFree( 				DT_COMPLEX *z );


DLLEXPORT DT_VECTOR
		*DtVectorAlloc(				int	Size ),
		*DtVectorAssign(			DT_VECTOR *Target, DT_VECTOR *Source ),
		*DtVectorCopy(				DT_VECTOR *Vector ),
		*DtVectorReverse(			DT_VECTOR *Vector ),
		*DtVectorSort(				DT_VECTOR *Vector ),
		*DtVectorOperate(			int Message, DT_VECTOR *VectorA, DT_VECTOR *VectorB ),
		*DtVectorOperateScalar(		int Message, DT_VECTOR *VectorA, double Value ),
		*DtVectorScan( 				int Message, DT_VECTOR *Vector ),
		*DtVectorFromDoubles(		double *Values, int	Length, DT_VECTOR *Vector ),
		*DtVectorDifference(		DT_VECTOR *Vector, DT_VECTOR *Answer );


DLLEXPORT double
		DtVectorNorm(				DT_VECTOR *Vector ),
		DtCVectorNorm(				DT_CVECTOR *CVector ),
		DtVectorInnerProduct(		DT_VECTOR *VectorA, DT_VECTOR *VectorB ),
		DtVectorReduce(				int	Message, DT_VECTOR *Vector );


DLLEXPORT int
		DtVectorAppend(				DT_VECTOR *Vector, double Value ),
		DtVectorInsert(				DT_VECTOR *Vector, int index, double Value ),
		DtVectorSearch(				DT_VECTOR *Vector, double Value, int *index ),
		DtVectorInsertElements(		DT_VECTOR *Vector, int Element, int Count ),
		DtVectorDeleteElements(		DT_VECTOR *Vector, int Element, int Count );

DLLEXPORT void
	 	DtVectorResize(				DT_VECTOR *Vector, int Size ),
		DtVectorFree(				DT_VECTOR *Vector );


DLLEXPORT DT_CVECTOR
		*DtCVectorAlloc(			int	Size ),
		*DtCVectorAssign(			DT_CVECTOR *Target, DT_CVECTOR *Source ),
		*DtCVectorCopy(				DT_CVECTOR *CVector ),
		*DtCVectorReverse(			DT_CVECTOR *CVector ),
		*DtCVectorOperate(			int Message, DT_CVECTOR *CVectorA, DT_CVECTOR *CVectorB ),
		*DtCVectorOperateScalar(	int Message, DT_CVECTOR *CVectorA, DT_COMPLEX *Value ),
		*DtCVectorScan( 			int Message, DT_CVECTOR *CVector ),
		*DtCVectorFromComplex(		DT_COMPLEX *Values, int	length, DT_CVECTOR *CVector ),
		*DtCVectorDifference(		DT_CVECTOR *CVector, DT_CVECTOR *Answer );

DLLEXPORT DT_COMPLEX
		*DtCVectorInnerProduct(		DT_CVECTOR *CVectorA, DT_CVECTOR *CVectorB, DT_COMPLEX *Answer ),
		*DtCVectorReduce(			int	Message, DT_CVECTOR *CVector, DT_COMPLEX *Answer );


DLLEXPORT int
		DtCVectorAppend(			DT_CVECTOR *CVector, DT_COMPLEX *Value ),
		DtCVectorInsert(			DT_CVECTOR *CVector, int index, DT_COMPLEX *Value ),
		DtCVectorSearch(			DT_CVECTOR *CVector, DT_COMPLEX *Value, int *index );

DLLEXPORT void
	 	DtCVectorResize(			DT_CVECTOR *CVector, int Size ),
		DtCVectorFree(				DT_CVECTOR *CVector );

DLLEXPORT double
		DtInterpolateLogSmooth(		DT_CURVE *Curve, long Date ),
		DtInterpolateQuadraticMatrixSetup( double *A, double *B, DT_VECTOR *xVector, DT_VECTOR *rfwd, DT_VECTOR *boundary, int k ),
		DtInterpolatePiecewiseQuadratic( DT_VECTOR *xVector, DT_VECTOR *yVector, double x );

// get/set the max size for datatypes to string conversions
DLLEXPORT int
		DtToStringMaxSizeGet(),
		DtToStringMaxSizeSet( int max ); // returns old max size

#endif /* IN_DTCORE_H */
