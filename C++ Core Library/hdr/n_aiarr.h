/* $Header: /home/cvs/src/num/src/n_aiarr.h,v 1.45 2000/12/05 16:24:44 lowthg Exp $ */
/****************************************************************
**
**
**	n_aiarr.h	- Implement various array structure needed for the FIP model.
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.45 $
**
****************************************************************/

#if !defined( IN_N_AIARR_H )
#define IN_N_AIARR_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif

EXPORT_C_NUM void N_AccessViolate( void );

#undef DEBUG

#include	<assert.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<n_nrarr.h>

#ifdef DEBUG
	#define At( T, I )	( (T).data[( I < (T).lowBound ) || ( I > (T).highBound ) ?										\
						( ( fprintf(stderr,"Index Error in At Macro...\n"), N_AccessViolate(), 0)) :					\
						( I - (T).lowBound )])
#else               
	#define At( T, I )	( (T).data[ I - (T).lowBound ] )
#endif //DEBUG

#define AIA_HOW_MANY( T )	( (T).highBound - (T).lowBound + 1 )
#define AIA_FIRST( T )		At((T), (T).lowBound )
#define AIA_SECOND( T )		At((T), (T).lowBound+1 )
#define AIA_THIRD( T )		At((T), (T).lowBound+2 )
#define AIA_BEFORE_BEFORE_LAST( T ) At((T), (T).highBound-2 )
#define AIA_BEFORE_LAST( T )At((T), (T).highBound-1 )
#define AIA_LAST( T )		At((T), (T).highBound )

#define FROM_Q_VECTOR_TO_AIA( _qv, _aia )	( (*_aia).lowBound = 0, (*_aia).highBound = (*_qv).Elem-1, (*_aia).data = (*_qv).V )

#ifdef DEBUG
	#define Att( T, I, J )	( (T).data[( I < (T).lowBoundLine ) || ( I > (T).highBoundLine ) ?							\
							( ( fprintf(stderr,"Index Error in Att Macro First Index...\n"), N_AccessViolate(), 0)) :	\
							( I - (T).lowBoundLine )]																	\
									  [( J < (T).lowBoundColumn ) || ( J > (T).highBoundColumn ) ?						\
							( ( fprintf(stderr,"Index Error in Att Macro Second Index...\n"), N_AccessViolate(), 0)) :	\
							( J - (T).lowBoundColumn )])
#else               
	#define Att( T, I, J )	( (T).data[ I - (T).lowBoundLine ][ J - (T).lowBoundColumn ] )
#endif //DEBUG

#define LAST( T )			AIA_LAST( T )

#define INITIALIZE( pS ) memset( pS, 0, sizeof( *pS ))


//----------------------------------------------------------------------
//              Arbitrary Indexed Array of Double
//----------------------------------------------------------------------

typedef struct {
	double	*data;
	int   	lowBound;
	int   	highBound;
} AIA;

int AIA_is_strictly_increasing( AIA *a );
int AIA_is_positive( AIA *a );
int AIA_is_strictly_positive( AIA *a ); 

int AIA_Throw_Negative_Time( 
	AIA	*T, 
	AIA	*fT 
); 

EXPORT_C_NUM void N_AIA_Initialize(
	AIA		*a
);

EXPORT_C_NUM void N_AIA_Alloc(
	AIA		*a, 
	int		lo,
	int		hi
);

EXPORT_C_NUM void N_AIA_Free(
	AIA		*a 
);

void AIA_Multiply(
	AIA		target,
	double	k,
	AIA		source 
);

void AIA_Multiply_AIA(
	AIA 	target, 
	AIA		k,
	AIA 	source 
);

void AIA_Copy(
	AIA		target, 
	AIA		source 
);

void AIA_AllocCopy(
	AIA *target, 
	AIA *source 
);

void AIA_Switch(
	AIA *a1, 
	AIA *a2 
);

void AIA_Locate(
	AIA		*xx, 
	int		*j,
	double 	x 
);

void AIA_SortNoDuplicate(
	AIA		*a, 
	double 	eps 
);

void AIA_Insert(
	AIA 	*a, 
	int 	index,
	double	x
	
);

void AIA_Remove(
	AIA 	*a, 
	int 	index
	
);

//----------------------------------------------------------------------
//              Arbitrary Indexed Array of Int
//----------------------------------------------------------------------


typedef struct {
	int		*data;
	int   	lowBound;
	int   	highBound;
} AIA_INT;

void AIA_INT_Initialize(
	AIA_INT	*a
);

void AIA_INT_Alloc(
	AIA_INT	*a, 
	int		lo,
	int		hi
);

void AIA_INT_Free(
	AIA_INT	*a 
);

void AIA_INT_Copy(
	AIA_INT target, 
	AIA_INT source 
);

void AIA_INT_AllocCopy(
	AIA_INT *target, 
	AIA_INT *source 
);

//----------------------------------------------------------------------
//              Arbitrary Indexed 2D-Array of Double
//----------------------------------------------------------------------

typedef struct {
	double	**data;
	int	  	lowBoundLine;
	int		highBoundLine;
	int     lowBoundColumn;
	int		highBoundColumn;
} AIA_DOUBLE_2D;

EXPORT_C_NUM void N_AIA_DOUBLE_2D_Initialize(
	AIA_DOUBLE_2D   *a
);

EXPORT_C_NUM void N_AIA_DOUBLE_2D_Alloc(
	AIA_DOUBLE_2D   *a,
	int				lowBoundLine,
	int				highBoundLine,
	int				lowBoundColumn,
	int				highBoundColumn
);

EXPORT_C_NUM void N_AIA_DOUBLE_2D_Free(
	AIA_DOUBLE_2D	
			*a
);

//----------------------------------------------------------------------
//              Arbitrary Indexed Dynamic Array of Double
//----------------------------------------------------------------------

typedef struct {
	double	*data;
	int		howMany;
	int		maxNumber;
	int 	lowBound;
	int		highBound;
	int		sizeIncrement;

} DYNAMIC_ARRAY;

void DYNAMIC_ARRAY_Initialize(
	DYNAMIC_ARRAY 	*a
);

void DYNAMIC_ARRAY_Alloc(
	DYNAMIC_ARRAY 	*a,
	int 			N,
	int				_sizeIncrement
);

void DYNAMIC_ARRAY_Free(
	DYNAMIC_ARRAY	*a
);

void DYNAMIC_ARRAY_Add(
	DYNAMIC_ARRAY	*a,
	double			x
);

void DYNAMIC_ARRAY_Reset(
	DYNAMIC_ARRAY 	*a
);

void DYNAMIC_ARRAY_Sort(
	DYNAMIC_ARRAY 	*a
);
//----------------------------------------------------------------------
//              Array of CURVE_POINT
//----------------------------------------------------------------------

typedef enum {
	CONSTANT_LEFT_CONTINUOUS,
	CONSTANT_RIGHT_CONTINUOUS,
	LINEAR,
	LOG_LINEAR,
	MIDDLE,
	VOL_INTERPOLATE
} INTERPOLATION_TYPE;

typedef struct {
	double 		t;	 
	double 		ft;
} CURVE_POINT;

typedef struct {
	char 		info[256];
	CURVE_POINT *data;			// The data[].t should be sorted.
	int 		lowBound;
	int 		highBound;
} AIA_CURVE;

EXPORT_C_NUM void N_AIA_CURVE_Initialize(
	AIA_CURVE *a
);

EXPORT_C_NUM void N_AIA_CURVE_Alloc(
	AIA_CURVE *a, 
	int		lo,
	int		hi
);

EXPORT_C_NUM void N_AIA_CURVE_Alloc_From_AIA(
	AIA_CURVE	*a, 
	AIA		*cT,
	AIA		*c
);

EXPORT_C_NUM int N_AIA_CURVE_Alloc_From_AIA_StartAt_T(
	  AIA_CURVE		*a, 
	  AIA			*cT,
	  AIA			*c,
	  double		T,
	  INTERPOLATION_TYPE
			  		it
);

EXPORT_C_NUM void N_AIA_CURVE_Free(
	AIA_CURVE 	*a 
);

EXPORT_C_NUM double N_AIA_CURVE_At(
	AIA_CURVE	c,					
	double		x,
    INTERPOLATION_TYPE 
				it
);

int AIA_CURVE_LineUp(
	double 		tStart,
	double	   	tEnd,
	int			howMany,
	AIA_CURVE	**CurveArray,
	int			*interpolation
);

/****************************************************************
**	Routine: AIA_ChangeTime
**	Returns: void
**	Action : Change Time scaling of a curve
****************************************************************/

void	N_AIA_CURVE_ChangeTime( 
	AIA_CURVE	*c,
	AIA			oldTime, 
	AIA			newTime,
    INTERPOLATION_TYPE 
				it
);

double	toNewTime( 
	double		time,
	AIA			oldTime, 
	AIA			newTime
);

//----------------------------------------------------------------------
//              Arbitrary Indexed Array of void pointers
//----------------------------------------------------------------------

typedef struct {
	void		**data;
	int 		lowBound;
	int 		highBound;
} AIA_VOID;

void AIA_VOID_Initialize(
	  AIA_VOID	*a
);

void AIA_VOID_Alloc(
	  AIA_VOID	*a, 
	  int	  	lo,
	  int	  	hi
);

void AIA_VOID_Free(
	 AIA_VOID 	*a 
);

#endif



