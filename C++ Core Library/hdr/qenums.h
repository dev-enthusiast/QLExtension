/****************************************************************
**
**	qenums.h	- Enums from quant library for shared use
**
**		The enums in this header file are for common use
**		among all of the FICC quant libraries.  This includes
**		the old quant library, the new qxxxx libraries, and the
**		fixxxx & fqxxxx libraries.
**
**		Any enums placed here should be well defined, and have
**		a common prefix to reduce likelihood of confusion and
**		symbol clashes.
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/num/src/qenums.h,v 1.25 2001/11/27 23:02:39 procmon Exp $
**
****************************************************************/

#if !defined( IN_QENUMS_H )
#define IN_QENUMS_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif

/*
**	Define option types
*/

typedef enum
{
	Q_OPT_ERROR	  		= -1,
	Q_OPT_FWD	  		= 0,
	Q_OPT_CALL	  		= 1,
	Q_OPT_PUT	  		= 2,
	Q_OPT_DIGITAL_CALL	= 3,
	Q_OPT_DIGITAL_PUT	= 4,
	Q_OPT_FWD_BUY		= Q_OPT_FWD,
	Q_OPT_FWD_SELL		= 5,
	Q_OPT_CONST_PAYOUT	= 6,
	Q_OPT_STRADDLE		= 7
	
} Q_OPT_TYPE;


/*
**	Option styles
*/

typedef enum
{
	Q_OPT_EURO	  =	1,
	Q_OPT_AMER	  =	2,
	Q_OPT_WHALEY  =	3			// American option priced with Q_Whaley

} Q_OPT_STYLE;


/*
**	Option commodity type
*/

typedef enum
{
	Q_OPT_ON_SPOT	 =	1,
	Q_OPT_ON_FUTURE	 =	2,
	Q_OPT_ON_FORWARD =	3

} Q_OPT_ON;

/*
**	Margin convention
*/

typedef enum
{
	Q_MARK_REGULAR	 =	1,		// No margin
	Q_MARK_DAILY	 =	2		// Margin adjusted daily

} Q_MARGIN;

/*
**	Knockout option rebate types
*/

typedef enum
{
	Q_IMMEDIATE		 =	1,
	Q_DELAYED		 =	2
} Q_REBATE_TYPE;

/*
**	Knockout option types
*/

typedef enum
{
	Q_KNOCK_DOWN 	 =	1,
	Q_KNOCK_UP		 =	2
} Q_UP_DOWN;

/*
**	Knockout or Appearing Option
*/

typedef enum
{
	Q_KNOCK_IN  	 =	1,
	Q_KNOCK_OUT		 =	2
} Q_IN_OUT;

/*
**	Knockout alive or dead
*/

typedef enum
{
	Q_NOT_KNOCKED_OUT =	1,
	Q_KNOCKED_OUT     = 2
} Q_ALIVE_DEAD;

/*
**	Barrier types
*/

typedef enum
{
	Q_BARRIER_NONE			= 0,
	Q_BARRIER_SINGLE_UP		= 1,
	Q_BARRIER_SINGLE_DOWN	= 2,
	Q_BARRIER_DOUBLE		= 3
} Q_BARRIER_TYPE;

/*
**	Fadeout types
*/
typedef enum
{
	Q_FADEOUT_RESURRECTING			= 0,
	Q_FADEOUT_NON_RESURRECTING 		= 1,
	Q_FADEOUT_NON_RESURRECTING_ALT 	= 2,
	Q_FADEOUT_R_TO_VANILLA 			= 3,
	Q_FADEOUT_NON_R_TO_VANILLA 		= 4,
	Q_FADEOUT_NON_R_TO_VANILLA_ALT	= 5,
	Q_FADEOUT_NONE_DISCRETE_KO 		= 6
} Q_FADEOUT_TYPE;

/*
**	Lookback option strike types
*/

typedef enum
{
	Q_BEST_STRIKE	 =	1,
	Q_WORST_STRIKE	 =	2
} Q_BEST_WORST;


/*
**	Average option types
*/

typedef enum
{
	Q_AVG_ARITHM	= 1,
	Q_AVG_GEOM		= 2,
	Q_AVG_HARM		= 3
} Q_AVG_TYPE;

/*
**	Ending or full-term (continuous) knockout
*/

typedef enum
{
	Q_FULLTERM		 =	1,
	Q_ENDING		 =	2
} Q_KO_TYPE;

/*
**	Date count method types
*/

typedef enum
{
	Q_ACTUAL_365	 =	1,
	Q_ACTUAL_ACTUAL	 =	2,
	Q_ACTUAL_360	 =	3,
	Q_30_360		 =	4
} Q_DAY_COUNT;

/*
**	Offset types
*/

typedef enum
{
	Q_OFFSET_ADDITIVE		= 1,
	Q_OFFSET_MULTIPLICATIVE	= 2

} Q_OFFSET_TYPE;

/*
**	Compounding method types
*/

typedef enum
{
	Q_CONTINUOUS		= 1,
	Q_SIMPLE_INTEREST	= 2,
	Q_ANNUAL			= 3,
	Q_SEMI_ANNUAL		= 4,	// Bond equivalent
	Q_QUARTERLY			= 5,
	Q_MONTHLY			= 6,
	Q_DISCOUNT			= 7
}	Q_COMPOUNDING_TYPE;

/*
**	Return type for option models
*/

typedef enum
{
	Q_PREMIUM        =  1,
	Q_DELTA          =  2,
	Q_GAMMA          =  3,
	Q_KAPPA          =  4,
	Q_THETA          =  5,
	Q_ERROR          =  6,
	Q_RAW_PREMIUM    =  7,
	Q_RAW_ERROR      =  8
} Q_RETURN_TYPE;


/*
**	Function types for default prob as function of spot
*/

typedef enum
{
	Q_POINT_SLOPE	= 1,
	Q_STEP_FUNCTION	= 2,
	Q_STEP_TO_ONE 	= 3
}	Q_FN_TYPE;

/*
**	Function types for default prob as function of spot
*/

typedef enum
{
	Q_KNOCKOUT		= 1,
	Q_REFLECTING	= 2,
	Q_ABSORBING 	= 3
}	Q_DIFFUSION_BOUNDARY_TYPE;

/*
**	Matrix structure
*/

typedef struct q_matrix
{
	int		Rows,	// number of rows
			Cols;	// number of cols

	double	*M;		// the values

	int	Type;	// Matrix type fields

} Q_MATRIX;

#define Q_ELEM(Mat_,Row_,Col_) ( (Mat_)->M[ ((Mat_)->Cols) * (Row_) + (Col_) ] )

#define Q_MATRIX_ALLOC( Matrix_, Rows_, Cols_ )	\
	( (Matrix_)->Rows = (Rows_), (Matrix_)->Cols = (Cols_), ( (Matrix_)->M = (double *)malloc( sizeof( double ) * (Rows_) * (Cols_) ) ) )
	
#define Q_MATRIX_CALLOC(Matrix_, Rows_, Cols_ ) \
	( (Matrix_)->Rows = (Rows_), (Matrix_)->Cols = (Cols_), ( (Matrix_)->M = (double *)calloc( (Rows_) * (Cols_) , sizeof( double ) ) ) != NULL )

#define Q_MATRIX_MEMPOOL_ALLOC( MemPool_, Matrix_, Rows_, Cols_ )	\
	( (Matrix_)->Rows = (Rows_), (Matrix_)->Cols = (Cols_),( (Matrix_)->M = (double *)MemPoolMalloc( (MemPool_), sizeof( double ) * (Rows_) * (Cols_) ) ) )

#define Q_MATRIX_MEMPOOL_CALLOC( MemPool_, Matrix_, Rows_, Cols_ )	\
	( (Matrix_)->Rows = (Rows_), (Matrix_)->Cols = (Cols_), ( (Matrix_)->M = (double *)MemPoolCalloc( (MemPool_), (Rows_) * (Cols_), sizeof( double ) ) ) != NULL )

#define Q_MATRIX_MEMPOOL_INIT( MemPool_, Matrix_, Rows_, Cols_ )	\
	if( ! ( ((Matrix_)->Rows = (Rows_)) && ((Matrix_)->Cols = (Cols_)) && 				\
			((Matrix_)->M = (double *)MemPoolCalloc( (MemPool_), (Rows_) * (Cols_), sizeof( double ) ) ) ) ) \
			{ MemPoolDestroy( (MemPool_) ); return Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" ); }

#define Q_MATRIX_INIT( Matrix_ )	\
	( (Matrix_)->M = NULL )
#define Q_MATRIX_FREE( Matrix_ )	\
	( free( (Matrix_)->M ) )

/*
**	Vector structure
*/

typedef struct q_vector
{
	int	Elem;	// number of rows

	double		*V;		// the values

} Q_VECTOR;

/*
**	Tensor structure
*/

typedef struct q_tensor
{
	int		dim;	// the dimension
	int		*size;	// vector of sizes in each dimension
	int		*step;	// vector of steps to move in each dimension

	double 	*V;		// the values

} Q_TENSOR;

/*
**	general d-dimensional diffusion
*/

typedef struct q_diffusion
{
	double	 *MinRange;		// min of range in each dimension
	double	 *MaxRange;		// max of range in each dimension
	Q_DIFFUSION_BOUNDARY_TYPE* LowerBoundaryType;	// lower boundary types
	Q_DIFFUSION_BOUNDARY_TYPE* UpperBoundaryType;	// upper boundary types

	Q_TENSOR Discount;		// discount factors
	Q_TENSOR *Drift;		// vector of d drift coefficients
	Q_TENSOR *Variance;		// variance & covariance terms ( d(d+1)/2 of them )
							// Cov[i,j] is ( i(i+1)/2 + j )'th entry, where i >= j
} Q_DIFFUSION;

/*
**	Linear operator, returns TRUE/FALSE
*/

typedef int (*Q_OPERATOR)(const Q_VECTOR *Input, void *args, Q_VECTOR *Output);

typedef struct q_op
{
	Q_OPERATOR Operator;
	void *Args;
} Q_OP;

#define Q_OP_APPLY( Operator_, Input_, Output_ ) ( (*(Operator_)->Operator)( (Input_), (Operator_)->Args, (Output_)))

#ifdef Q_VECTOR_DEBUG
	#define Q_VELEM( Vector_, index_ )	( (Vector_)->V[( index_ < 0 ) || ( index_ >= (Vector_)->Elem ) ?										\
						( ( fprintf(stderr,"Index Error in Q_VELEM Macro...\n"), N_AccessViolate(), 0)) :					\
						( index_ )])
#else               
	#define Q_VELEM( Vector_, index_ )	( (Vector_)->V[ index_ ] )
#endif //Q_VECTOR_DEBUG


#define Q_VECTOR_ALLOC( Vector_, NumElem_ )		\
	( (Vector_)->Elem = (NumElem_), ((Vector_)->V = (double *)malloc( sizeof( double ) * (NumElem_) )) != NULL )
	
#define Q_VECTOR_MEMPOOL_ALLOC( MemPool_, Vector_, NumElem_ )		\
	( (Vector_)->Elem = (NumElem_), ((Vector_)->V = (double *)MemPoolMalloc( (MemPool_), sizeof( double ) * (NumElem_) )) != NULL )
	
#define Q_VECTOR_INIT( Vector_ )	\
	( (Vector_)->V = NULL, (Vector_)->Elem = 0 )
	
#define Q_VECTOR_FREE( Vector_ )	\
	( free( (Vector_)->V ))

#define Q_VECTOR_CALLOC( Vector_, NumElem_ )		\
	( (Vector_)->Elem = (NumElem_), ((Vector_)->V = (double *) calloc ( NumElem_ , sizeof( double ) ) ) != NULL )
	
#define Q_VECTOR_MEMPOOL_CALLOC( MemPool_, Vector_, NumElem_ )		\
	( (Vector_)->Elem = (NumElem_), ((Vector_)->V = (double *)MemPoolCalloc( (MemPool_), NumElem_ , sizeof( double ) ) ) != NULL )

#define Q_VECTOR_MEMPOOL_INIT( MemPool_, Vector_, NumElem_ )	\
	if( ! ( ((Vector_)->Elem = (NumElem_)) && 					\
			((Vector_)->V = (double *)MemPoolCalloc( (MemPool_), (Vector_)->Elem, sizeof( double ) )) ) ) \
			{ MemPoolDestroy( (MemPool_) ); return Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" ); }

EXPORT_C_NUM Q_DAY_COUNT SQ_StrToBasis( char *String );

#endif

