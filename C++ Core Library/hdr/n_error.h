/* $Header: /home/cvs/src/num/src/n_error.h,v 1.32 2000/12/07 01:13:44 rozena Exp $ */
/****************************************************************
**
**	n_error.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.32 $
**
****************************************************************/

#if !defined( IN_N_ERROR_H )
#define IN_N_ERROR_H

#ifndef IN_QTYPES_H
#include	<qtypes.h>
#endif
#ifndef IN_ERR_H
#include	<err.h>
#endif

EXPORT_C_NUM int 
		N_ArgCheck_Q_VECTOR( const Q_VECTOR *V, int MinSize, int Flags, const char *FnName, const char *VectorName ),
		N_ArgCheck_Q_MATRIX( const Q_MATRIX *M, int MinSizeRows, int MinSizeCols, int Flags, const char *FnName, const char *MatrixName ),
		N_ArgCheck_DT_CURVE( const DT_CURVE *Crv, int MinSize, int Flags, const char *FnName, const char *CurveName ),
		Q_CheckBarriers(
			double			Spot,					// the spot
			Q_VECTOR		*LowerBarrierLevels,	// Lower Knockouts (a level <=0 or HUGE means it doesn't exist)
			Q_VECTOR		*UpperBarrierLevels,	// Upper Knockouts (a level <=0 or HUGE means it doesn't exist)
			Q_VECTOR		*LowerRebates,			// the lower rebates
			Q_VECTOR		*UpperRebates,			// the upper rebates
			Q_VECTOR		*EndSegments,			// end times of the segments
			int				*nSegments,				// the number of segments util expirey, or until we are definite to knock out
			double			*Term,					// time to maturity (input + output) set to HUGE if we have already knocked out.
			Q_IN_OUT		InOrOut,				// Fade In Or Out
			double			*LogLowerBarriers,		// logarithms of the lower barriers (HUGE means no barrier) (output only)
			double			*LogUpperBarriers,		// logarithms of the upper barriers (HUGE means no barrier) (output only)
			double			MinGrid,				// the value of the minimum grid point
			double			MaxGrid,				// the value of the maximum grid point
			double			*Value					// value of the option when it knocks out (set to HUGE if it is not definite to knock out)
			),
		Q_CheckBarriersVec(
			double			minSpot,				// the lower spot
			double			maxSpot,				// the upper spot
			Q_VECTOR		*LowerBarrierLevels,	// Lower Knockouts (a level <=0 or HUGE means it doesn't exist)
			Q_VECTOR		*UpperBarrierLevels,	// Upper Knockouts (a level <=0 or HUGE means it doesn't exist)
			Q_VECTOR		*LowerRebates,			// the lower rebates
			Q_VECTOR		*UpperRebates,			// the upper rebates
			Q_VECTOR		*EndSegments,			// end times of the segments
			int				*nSegments,				// the number of segments util expirey, or until we are definite to knock out
			double			*Term,					// time to maturity (input + output) set to HUGE if we have already knocked out.
			Q_IN_OUT		InOrOut,				// Fade In Or Out
			double			*LogLowerBarriers,		// logarithms of the lower barriers (HUGE means no barrier) (output only)
			double			*LogUpperBarriers,		// logarithms of the upper barriers (HUGE means no barrier) (output only)
			double			MinGrid,				// the value of the minimum grid point
			double			MaxGrid,				// the value of the maximum grid point
			double			*Value					// value of the option when it knocks out (set to HUGE if it is not definite to knock out)
			);


#define ARGCHECK_MONOTONIC						0x0001
#define ARGCHECK_STRICT_ORDER_FLAG				0x0002
#define ARGCHECK_MONOTONIC_UP					0x0004
#define ARGCHECK_MONOTONIC_DOWN					0x0008
#define ARGCHECK_MONOTONIC_STRICT				( ARGCHECK_MONOTONIC		| ARGCHECK_STRICT_ORDER_FLAG )
#define ARGCHECK_MONOTONIC_UP_STRICT			( ARGCHECK_MONOTONIC_UP		| ARGCHECK_STRICT_ORDER_FLAG )
#define ARGCHECK_MONOTONIC_DOWN_STRICT			( ARGCHECK_MONOTONIC_DOWN	| ARGCHECK_STRICT_ORDER_FLAG )
#define ARGCHECK_DONT_CHECK_VALUES				0x0010
#define ARGCHECK_MUST_BE_POSITIVE				0x0020
#define ARGCHECK_MUST_BE_NEGATIVE				0x0040
#define ARGCHECK_MUST_BE_NONPOSITIVE			0x0080
#define ARGCHECK_MUST_BE_NONNEGATIVE			0x0100


/*
**	For backwards compatibility...
*/
#define ARGCHECK_Q_VECTOR_MONOTONIC				ARGCHECK_MONOTONIC            
#define ARGCHECK_Q_VECTOR_STRICT_ORDER_FLAG		ARGCHECK_STRICT_ORDER_FLAG    
#define ARGCHECK_Q_VECTOR_MONOTONIC_UP			ARGCHECK_MONOTONIC_UP         
#define ARGCHECK_Q_VECTOR_MONOTONIC_DOWN		ARGCHECK_MONOTONIC_DOWN       
#define ARGCHECK_Q_VECTOR_MONOTONIC_STRICT		ARGCHECK_MONOTONIC_STRICT     
#define ARGCHECK_Q_VECTOR_MONOTONIC_UP_STRICT	ARGCHECK_MONOTONIC_UP_STRICT  
#define ARGCHECK_Q_VECTOR_MONOTONIC_DOWN_STRICT	ARGCHECK_MONOTONIC_DOWN_STRICT
#define ARGCHECK_Q_VECTOR_MUST_BE_POSITIVE		ARGCHECK_MUST_BE_POSITIVE     
#define ARGCHECK_Q_VECTOR_MUST_BE_NEGATIVE		ARGCHECK_MUST_BE_NEGATIVE     
#define ARGCHECK_Q_VECTOR_MUST_BE_NONPOSITIVE	ARGCHECK_MUST_BE_NONPOSITIVE  
#define ARGCHECK_Q_VECTOR_MUST_BE_NONNEGATIVE	ARGCHECK_MUST_BE_NONNEGATIVE  

/*
**	These macros are private. Use the ones further down
*/

#define _ARGCHECK_NULL_MACRO( _ptr, ErrFunc, _Cast )					\
	if( NULL == (_ptr) )												\
        return (_Cast) ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Missing " #_ptr );
		
#define _ARGCHECK_INVALID_CURVE( _crv, _MinSize, _Flags, _RetVal, _Cast )		\
	if( !N_ArgCheck_DT_CURVE( (_crv), (_MinSize), (_Flags), ARGCHECK_FUNCTION_NAME, #_crv ) )	\
		return (_Cast) (_RetVal);
		
#define _ARGCHECK_INVALID_Q_VECTOR( _v, _MinSize, _Flags, _RetVal, _Cast )		\
	if( !N_ArgCheck_Q_VECTOR( (_v), (_MinSize), (_Flags), ARGCHECK_FUNCTION_NAME, #_v ) )	\
		return (_Cast) (_RetVal);
#define _ARGCHECK_Q_VECTORS_SAME_SIZE_MACRO( _v, _w, _RetVal, _Cast )	\
	if( !N_ArgCheck_Q_VECTOR( (_v), 0, ARGCHECK_DONT_CHECK_VALUES, ARGCHECK_FUNCTION_NAME, #_v ) )	\
		return (_Cast) (_RetVal);												\
	else if( !N_ArgCheck_Q_VECTOR( (_w), 0, ARGCHECK_DONT_CHECK_VALUES, ARGCHECK_FUNCTION_NAME, #_w ) )	\
		return (_RetVal);												\
	else if( (_v)->Elem != (_w)->Elem )									\
	{																	\
        Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #_v " and " #_w " must be same size (%d != %d)", (_v)->Elem, (_w)->Elem );	\
		return (_RetVal);												\
	}
	
#define _ARGCHECK_INVALID_Q_MATRIX( _m, _MinSizeRows, _MinSizeCols, _Flags, _RetVal, _Cast )		\
	if( !N_ArgCheck_Q_MATRIX( (_m), (_MinSizeRows), (_MinSizeCols), (_Flags), ARGCHECK_FUNCTION_NAME, #_m ) )	\
		return (_Cast) (_RetVal);

#define _ARGCHECK_Q_MATRICES_SAME_SIZE_MACRO( _m, _n, _RetVal, _Cast )	\
	if( !N_ArgCheck_Q_MATRIX( (_m), 0, 0, ARGCHECK_DONT_CHECK_VALUES, ARGCHECK_FUNCTION_NAME, #_m ) )	\
		return (_Cast) (_RetVal);												\
    else if( !N_ArgCheck_Q_MATRIX( (_n), 0, 0, ARGCHECK_DONT_CHECK_VALUES, ARGCHECK_FUNCTION_NAME, #_m ) )	\
	    return (_Cast) (_RetVal);										\
	else if( (_m)->Rows != (_n)->Rows  )								\
	{																	\
        Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #_m " and " #_n " must have the same number of rows (%d != %d)", (_m)->Rows, (_n)->Rows );	\
		return (_RetVal);												\
	}																	\
	else if( (_m)->Cols != (_n)->Cols  )						   		\
	{																	\
        Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #_m " and " #_n " must have the same number of columns (%d != %d)", (_m)->Cols, (_n)->Cols );	\
		return (_RetVal);												\
	}
	
#define _ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, _operator, ErrFunc, _Cast )	\
	if( !( (_D1) _operator (_D2) ) )									\
	{																	\
		char	DateBuf1[ DATE_STRING_SIZE ],							\
				DateBuf2[ DATE_STRING_SIZE ];							\
																		\
																		\
		return (_Cast) ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #_D1 " (%s) must be " #_operator " " #_D2 " (%s)",	\
				DateToString( DateBuf1, (_D1) ), DateToString( DateBuf2, (_D2) ) );	\
	}

#define _ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, _operator, ErrFunc, _Cast )	\
	if( !( (_T1) _operator (_T2) ) )									\
	{																	\
		char	TimeBuf1[ TIME_STRING_SIZE ],							\
				TimeBuf2[ TIME_STRING_SIZE ];							\
																		\
																		\
		return (_Cast) ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #_T1 " (%s) must be " #_operator " " #_T2 " (%s)",	\
				TimeToString( TimeBuf1, (_T1), NULL ), TimeToString( TimeBuf2, (_T2), NULL ) );	\
	}
	
#define _ARGCHECK_CONDITION_MACRO( _cond, ErrFunc, _Cast )				\
	if( (_cond) )														\
        return (_Cast) ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #_cond );
#define _ARGCHECK_LIMIT_MACRO( _x, _limit, _operator, _format, ErrFunc, _Cast )	\
	if( !( (_x) _operator (_limit) ) )									\
        return (_Cast) ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #_x " (" #_format ") must be " #_operator " " #_limit " (" #_format ")", (_x), (_limit) );
#define _ARGCHECK_INVALID_DATE_MACRO( _x, ErrFunc, _Cast )				\
    if( !VALID_DATE( (_x) ) )											\
        return (_Cast) ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - error " #_x );
#define _ARGCHECK_INVALID_TIME_MACRO( _x, ErrFunc )						\
    if( !VALID_TIME( (_x) ) )											\
        return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - error " #_x );
#define _ARGCHECK_INVALID_RDATE_MACRO( _x, _ErrFunc, _Cast )						\
    if( !RDateIsValid( (_x) ) )											\
        return (_Cast) _ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - error " #_x );
#define _ARGCHECK_INVALID_DOUBLE_MACRO( _x, ErrFunc, _Cast )			\
    if( !VALID_DOUBLE( (_x) ) )											\
        return (_Cast) ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - error " #_x );
#define _ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, _operator, _qualifier, ErrFunc, _Cast )	\
	_ARGCHECK_INVALID_DOUBLE_MACRO( (_x), ErrFunc, _Cast ) 	\
    else if( (_x) _operator 0.0 )											\
        return (_Cast) ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #_x " (%g) must be " #_qualifier, (_x) );
#define _ARGCHECK_INVALID_Q_OPT_TYPE_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_OPT_FWD:								\
		case Q_OPT_CALL:							\
		case Q_OPT_PUT:								\
		case Q_OPT_DIGITAL_CALL:					\
		case Q_OPT_DIGITAL_PUT:						\
		case Q_OPT_FWD_SELL:						\
		case Q_OPT_CONST_PAYOUT:					\
		case Q_OPT_STRADDLE:						\
		   	break;									\
		case Q_OPT_ERROR:							\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_OPT_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_RETURN_TYPE_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_PREMIUM:    							\
		case Q_DELTA:      							\
		case Q_GAMMA:      			   			 	\
		case Q_KAPPA:      							\
		case Q_THETA:      							\
		case Q_ERROR:      			  			  	\
		case Q_RAW_PREMIUM:							\
		case Q_RAW_ERROR:  							\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_RETURN_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_IN_OUT_MACRO( _x, ErrFunc )		\
	switch( (_x) )									\
	{												\
		case Q_KNOCK_IN:   							\
		case Q_KNOCK_OUT:  							\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_IN_OUT " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_REBATE_TYPE_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_IMMEDIATE:  							\
		case Q_DELAYED:  							\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_REBATE_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_OPT_STYLE_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_OPT_EURO:  							\
			 Q_OPT_AMER:							\
			 Q_OPT_WHALEY:							\
			break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_OPT_STYLE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_OPT_ON_MACRO( _x, ErrFunc )		\
	switch( (_x) )									\
	{												\
		case Q_OPT_ON_SPOT:  						\
		case Q_OPT_ON_FUTURE: 						\
		case Q_OPT_ON_FORWARD:						\
		   	break;		 	   						\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_OPT_ON " #_x " (%d)", (_x) );	\
	}

#define _ARGCHECK_INVALID_Q_MARGIN_MACRO( _x, ErrFunc )		\
	switch( (_x) )									\
	{												\
		case Q_MARK_REGULAR:						\
		case Q_MARK_DAILY:  						\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_MARGIN " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_UP_DOWN_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_KNOCK_DOWN:  						\
		case Q_KNOCK_UP: 	 						\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_UP_DOWN " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_ALIVE_DEAD_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_NOT_KNOCKED_OUT:						\
		case Q_KNOCKED_OUT:  						\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_ALIVE_DEAD " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_BARRIER_TYPE_MACRO( _x, ErrFunc )\
	switch( (_x) )									\
	{												\
		case Q_BARRIER_NONE:  						\
		case Q_BARRIER_SINGLE_UP:  					\
		case Q_BARRIER_SINGLE_DOWN:					\
		case Q_BARRIER_DOUBLE: 						\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_BARRIER_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_FADEOUT_TYPE_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_FADEOUT_RESURRECTING:				\
		case Q_FADEOUT_NON_RESURRECTING:			\
		case Q_FADEOUT_NON_RESURRECTING_ALT:		\
		case Q_FADEOUT_R_TO_VANILLA:				\
		case Q_FADEOUT_NON_R_TO_VANILLA:			\
		case Q_FADEOUT_NON_R_TO_VANILLA_ALT:		\
		case Q_FADEOUT_NONE_DISCRETE_KO:  			\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_FADEOUT_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_BEST_WORST_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_BEST_STRIKE:  						\
		case Q_WORST_STRIKE:  						\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_BEST_WORST " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_AVG_TYPE_MACRO( _x, ErrFunc )		\
	switch( (_x) )									\
	{												\
		case Q_AVG_ARITHM:  	   					\
		case Q_AVG_GEOM:    	   					\
		case Q_AVG_HARM:    	   					\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_AVG_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_KO_TYPE_MACRO( _x, ErrFunc )		\
	switch( (_x) )									\
	{												\
		case Q_FULLTERM:  							\
		case Q_ENDING:		  						\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_KO_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_DAY_COUNT_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_ACTUAL_365   :  						\
		case Q_ACTUAL_ACTUAL:  						\
		case Q_ACTUAL_360   :  						\
		case Q_30_360       :  						\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_DAY_COUNT " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_OFFSET_TYPE_MACRO( _x, ErrFunc )	\
	switch( (_x) )									\
	{												\
		case Q_OFFSET_ADDITIVE:  					\
		case Q_OFFSET_MULTIPLICATIVE:  				\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_OFFSET_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_COMPOUNDING_TYPE_MACRO( _x, ErrFunc )\
	switch( (_x) )									\
	{												\
		case Q_CONTINUOUS:  						\
		case Q_SIMPLE_INTEREST:  					\
		case Q_ANNUAL:  							\
		case Q_SEMI_ANNUAL:  						\
		case Q_QUARTERLY:  							\
		case Q_MONTHLY:  							\
		case Q_DISCOUNT:  							\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_COMPOUNDING_TYPE " #_x " (%d)", (_x) );	\
	}
#define _ARGCHECK_INVALID_Q_FN_TYPE_MACRO( _x, ErrFunc )		\
	switch( (_x) )									\
	{												\
		case Q_POINT_SLOPE:  						\
		case Q_STEP_FUNCTION:  						\
		case Q_STEP_TO_ONE:  						\
		   	break;									\
		default:									\
			return ErrFunc( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Invalid Q_FN_TYPE " #_x " (%d)", (_x) );	\
	}

/*
**	These macros are public
*/


/*
**	Check arbitrary pointers
*/
#define ARGCHECK_ERR_ON_NULL( _ptr )			   			_ARGCHECK_NULL_MACRO( _ptr, Err, int )
#define ARGCHECK_ERRD_ON_NULL( _ptr )			   			_ARGCHECK_NULL_MACRO( _ptr, ErrD, double )
#define ARGCHECK_ERRN_ON_NULL( _ptr, _Cast )	   			_ARGCHECK_NULL_MACRO( _ptr, ErrN, _Cast )

/*
**	Check Q_VECTORS
*/
#define ARGCHECK_ERR_ON_INVALID_Q_VECTOR( _v, _MinSize, _Flags )			_ARGCHECK_INVALID_Q_VECTOR( _v, _MinSize, _Flags, FALSE, int )
#define ARGCHECK_ERRD_ON_INVALID_Q_VECTOR( _v, _MinSize, _Flags )			_ARGCHECK_INVALID_Q_VECTOR( _v, _MinSize, _Flags, HUGE_VAL, double )
#define ARGCHECK_ERRN_ON_INVALID_Q_VECTOR( _v, _MinSize, _Flags, _Cast )	_ARGCHECK_INVALID_Q_VECTOR( _v, _MinSize, _Flags, NULL, _Cast )
#define ARGCHECK_ERR_ON_Q_VECTOR_MISMATCH( _v, _w )							_ARGCHECK_Q_VECTORS_SAME_SIZE_MACRO( _v, _w, FALSE, int )
#define ARGCHECK_ERRD_ON_Q_VECTOR_MISMATCH( _v, _w )						_ARGCHECK_Q_VECTORS_SAME_SIZE_MACRO( _v, _w, HUGE_VAL, double )
#define ARGCHECK_ERRN_ON_Q_VECTOR_MISMATCH( _v, _w, _Cast )					_ARGCHECK_Q_VECTORS_SAME_SIZE_MACRO( _v, _w, NULL, _Cast )

/*
**	Check Q_MATRICES
*/
#define ARGCHECK_ERR_ON_INVALID_Q_MATRIX( _m, _MinSizeRows, _MinSizeCols, _Flags )			_ARGCHECK_INVALID_Q_MATRIX( _m, _MinSizeRows, _MinSizeCols, _Flags, FALSE, int )
#define ARGCHECK_ERRD_ON_INVALID_Q_MATRIX( _m, _MinSizeRows, _MinSizeCols, _Flags )			_ARGCHECK_INVALID_Q_MATRIX( _m, _MinSizeRows, _MinSizeCols, _Flags, HUGE_VAL, double )
#define ARGCHECK_ERRN_ON_INVALID_Q_MATRIX( _m, _MinSizeRows, _MinSizeCols, _Flags, _Cast )	_ARGCHECK_INVALID_Q_MATRIX( _m, _MinSizeRows, _MinSizeCols, _Flags, NULL, _Cast )
#define ARGCHECK_ERR_ON_Q_MATRICES_MISMATCH( _m, _n )							_ARGCHECK_Q_MATRICES_SAME_SIZE_MACRO( _m, _n, FALSE, int )
#define ARGCHECK_ERRD_ON_Q_MATRICES_MISMATCH( _m, _n )							_ARGCHECK_Q_MATRICES_SAME_SIZE_MACRO( _m, _n, HUGE_VAL, double )
#define ARGCHECK_ERRN_ON_Q_MATRICES_MISMATCH( _m, _n, _Cast )					_ARGCHECK_Q_MATRICES_SAME_SIZE_MACRO( _m, _n, NULL, _Cast )

/*
**	Check DT_CURVEs
*/
#define ARGCHECK_ERR_ON_INVALID_CURVE( _Crv, _MinSize, _Flags )			_ARGCHECK_INVALID_CURVE( _Crv, _MinSize, _Flags, FALSE, int )
#define ARGCHECK_ERRD_ON_INVALID_CURVE( _Crv, _MinSize, _Flags )		_ARGCHECK_INVALID_CURVE( _Crv, _MinSize, _Flags, HUGE_VAL, double )
#define ARGCHECK_ERRN_ON_INVALID_CURVE( _Crv, _MinSize, _Flags, _Cast )	_ARGCHECK_INVALID_CURVE( _Crv, _MinSize, _Flags, NULL, _Cast )

/*
**	Check ints vs a limit
*/
#define ARGCHECK_ERR_ON_EQ_INT( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit, !=, %d, Err,  int )
#define ARGCHECK_ERRD_ON_EQ_INT( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit, !=, %d, ErrD, double )
#define ARGCHECK_ERRN_ON_EQ_INT( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit, !=, %d, ErrN, _Cast )
#define ARGCHECK_ERR_ON_NE_INT( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit, ==, %d, Err,  int )
#define ARGCHECK_ERRD_ON_NE_INT( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit, ==, %d, ErrD, double )
#define ARGCHECK_ERRN_ON_NE_INT( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit, ==, %d, ErrN, _Cast )
#define ARGCHECK_ERR_ON_GT_INT( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit, <=, %d, Err,  int )
#define ARGCHECK_ERRD_ON_GT_INT( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit, <=, %d, ErrD, double )
#define ARGCHECK_ERRN_ON_GT_INT( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit, <=, %d, ErrN, _Cast )
#define ARGCHECK_ERR_ON_GE_INT( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit,  <, %d, Err,  int )
#define ARGCHECK_ERRD_ON_GE_INT( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit,  <, %d, ErrD, double )
#define ARGCHECK_ERRN_ON_GE_INT( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit,  <, %d, ErrN, _Cast )
#define ARGCHECK_ERR_ON_LT_INT( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit, >=, %d, Err,  int )
#define ARGCHECK_ERRD_ON_LT_INT( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit, >=, %d, ErrD, double )
#define ARGCHECK_ERRN_ON_LT_INT( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit, >=, %d, ErrN, _Cast )
#define ARGCHECK_ERR_ON_LE_INT( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit,  >, %d, Err,  int )
#define ARGCHECK_ERRD_ON_LE_INT( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit,  >, %d, ErrD, double )
#define ARGCHECK_ERRN_ON_LE_INT( _x, _limit, _Cast )		_ARGCHECK_LIMIT_MACRO( _x, _limit,  >, %d, ErrN, _Cast )

/*
**	Check doubles
*/
#define ARGCHECK_ERR_ON_INVALID_DBL( _x )					_ARGCHECK_INVALID_DOUBLE_MACRO( _x, Err,  int )
#define ARGCHECK_ERRD_ON_INVALID_DBL( _x )					_ARGCHECK_INVALID_DOUBLE_MACRO( _x, ErrD, double )
#define ARGCHECK_ERRN_ON_INVALID_DBL( _x, _Cast )			_ARGCHECK_INVALID_DOUBLE_MACRO( _x, ErrN, _Cast )
#define ARGCHECK_ERR_ON_INVALID_NEGATIVE_DBL( _x )			_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, <, "non-negative", Err,  int )
#define ARGCHECK_ERRD_ON_INVALID_NEGATIVE_DBL( _x )			_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, <, "non-negative", ErrD, double )
#define ARGCHECK_ERRN_ON_INVALID_NEGATIVE_DBL( _x, _Cast )	_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, <, "non-negative", ErrN, _Cast )
#define ARGCHECK_ERR_ON_INVALID_POSITIVE_DBL( _x )			_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, >, "non-positive", Err,  int )
#define ARGCHECK_ERRD_ON_INVALID_POSITIVE_DBL( _x )			_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, >, "non-positive", ErrD, double )
#define ARGCHECK_ERRN_ON_INVALID_POSITIVE_DBL( _x, _Cast )	_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, >, "non-positive", ErrN, _Cast )
#define ARGCHECK_ERR_ON_INVALID_NONNEGATIVE_DBL( _x )		_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, >=, "negative", Err,  int )
#define ARGCHECK_ERRD_ON_INVALID_NONNEGATIVE_DBL( _x )		_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, >=, "negative", ErrD, double )
#define ARGCHECK_ERRN_ON_INVALID_NONNEGATIVE_DBL(_x,_Cast)	_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, >=, "negative", ErrN, _Cast )
#define ARGCHECK_ERR_ON_INVALID_NONPOSITIVE_DBL( _x )		_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, <=, "positive", Err,  int )
#define ARGCHECK_ERRD_ON_INVALID_NONPOSITIVE_DBL( _x )		_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, <=, "positive", ErrD, double )
#define ARGCHECK_ERRN_ON_INVALID_NONPOSITIVE_DBL(_x,_Cast)	_ARGCHECK_INVALID_SIGN_DOUBLE_MACRO( _x, <=, "positive", ErrN, _Cast )
#define ARGCHECK_ERR_ON_EQ_DBL( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit, !=, %g, Err,  int )
#define ARGCHECK_ERRD_ON_EQ_DBL( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit, !=, %g, ErrD, double )
#define ARGCHECK_ERRN_ON_EQ_DBL( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit, !=, %g, ErrN, _Cast )
#define ARGCHECK_ERR_ON_NE_DBL( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit, ==, %g, Err,  int )
#define ARGCHECK_ERRD_ON_NE_DBL( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit, ==, %g, ErrD, double )
#define ARGCHECK_ERRN_ON_NE_DBL( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit, ==, %g, ErrN, _Cast  )
#define ARGCHECK_ERR_ON_GT_DBL( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit, <=, %g, Err,  int )
#define ARGCHECK_ERRD_ON_GT_DBL( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit, <=, %g, ErrD, double )
#define ARGCHECK_ERRN_ON_GT_DBL( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit, <=, %g, ErrN, _Cast  )
#define ARGCHECK_ERR_ON_GE_DBL( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit,  <, %g, Err,  int )
#define ARGCHECK_ERRD_ON_GE_DBL( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit,  <, %g, ErrD, double )
#define ARGCHECK_ERRN_ON_GE_DBL( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit,  <, %g, ErrN, _Cast  )
#define ARGCHECK_ERR_ON_LT_DBL( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit, >=, %g, Err,  int )
#define ARGCHECK_ERRD_ON_LT_DBL( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit, >=, %g, ErrD, double )
#define ARGCHECK_ERRN_ON_LT_DBL( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit, >=, %g, ErrN, _Cast  )
#define ARGCHECK_ERR_ON_LE_DBL( _x, _limit )	   			_ARGCHECK_LIMIT_MACRO( _x, _limit,  >, %g, Err,  int )
#define ARGCHECK_ERRD_ON_LE_DBL( _x, _limit )   			_ARGCHECK_LIMIT_MACRO( _x, _limit,  >, %g, ErrD, double )
#define ARGCHECK_ERRN_ON_LE_DBL( _x, _limit, _Cast )   		_ARGCHECK_LIMIT_MACRO( _x, _limit,  >, %g, ErrN, _Cast  )

/*
**	Check DATEs
*/
#define ARGCHECK_ERR_ON_INVALID_DATE( _x )					_ARGCHECK_INVALID_DATE_MACRO( _x, Err,  int )
#define ARGCHECK_ERRD_ON_INVALID_DATE( _x )					_ARGCHECK_INVALID_DATE_MACRO( _x, ErrD, double )
#define ARGCHECK_ERRN_ON_INVALID_DATE( _x, _Cast )			_ARGCHECK_INVALID_DATE_MACRO( _x, ErrN, _Cast )
#define ARGCHECK_ERR_ON_EQ_DATES( _D1, _D2 )		   	   	_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, !=, Err,  int )
#define ARGCHECK_ERRD_ON_EQ_DATES( _D1, _D2 )		   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, !=, ErrD, double )
#define ARGCHECK_ERRN_ON_EQ_DATES( _D1, _D2, _Cast )   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, !=, ErrN, _Cast )
#define ARGCHECK_ERR_ON_NE_DATES( _D1, _D2 )		   	   	_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, ==, Err,  int )
#define ARGCHECK_ERRD_ON_NE_DATES( _D1, _D2 )		   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, ==, ErrD, double )
#define ARGCHECK_ERRN_ON_NE_DATES( _D1, _D2, _Cast )   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, ==, ErrN, _Cast )
#define ARGCHECK_ERR_ON_GT_DATES( _D1, _D2 )		   	   	_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, <=, Err,  int )
#define ARGCHECK_ERRD_ON_GT_DATES( _D1, _D2 )		   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, <=, ErrD, double )
#define ARGCHECK_ERRN_ON_GT_DATES( _D1, _D2, _Cast )   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, <=, ErrN, _Cast )
#define ARGCHECK_ERR_ON_GE_DATES( _D1, _D2 )		   	   	_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2,  <, Err,  int )
#define ARGCHECK_ERRD_ON_GE_DATES( _D1, _D2 )		   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2,  <, ErrD, double )
#define ARGCHECK_ERRN_ON_GE_DATES( _D1, _D2, _Cast )   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2,  <, ErrN, _Cast )
#define ARGCHECK_ERR_ON_LT_DATES( _D1, _D2 )		   	   	_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, >=, Err,  int )
#define ARGCHECK_ERRD_ON_LT_DATES( _D1, _D2 )		   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, >=, ErrD, double )
#define ARGCHECK_ERRN_ON_LT_DATES( _D1, _D2, _Cast )   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2, >=, ErrN, _Cast )
#define ARGCHECK_ERR_ON_LE_DATES( _D1, _D2 )		   	   	_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2,  >, Err,  int )
#define ARGCHECK_ERRD_ON_LE_DATES( _D1, _D2 )		   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2,  >, ErrD, double )
#define ARGCHECK_ERRN_ON_LE_DATES( _D1, _D2, _Cast )   		_ARGCHECK_COMPARE_DATE_MACRO( _D1, _D2,  >, ErrN, _Cast )

/*
**	Check TIMEs
*/
#define ARGCHECK_ERR_ON_INVALID_TIME( _x )					_ARGCHECK_INVALID_TIME_MACRO( _x, Err,  int )
#define ARGCHECK_ERRD_ON_INVALID_TIME( _x )					_ARGCHECK_INVALID_TIME_MACRO( _x, ErrD, double )
#define ARGCHECK_ERRN_ON_INVALID_TIME( _x, _Cast )			_ARGCHECK_INVALID_TIME_MACRO( _x, Errn, _Cast  )
#define ARGCHECK_ERR_ON_EQ_TIMES( _T1, _T2 )		   	   	_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, !=, Err,  int )
#define ARGCHECK_ERRD_ON_EQ_TIMES( _T1, _T2 )		   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, !=, ErrD, double )
#define ARGCHECK_ERRN_ON_EQ_TIMES( _T1, _T2, _Cast )   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, !=, ErrN, _Cast )
#define ARGCHECK_ERR_ON_NE_TIMES( _T1, _T2 )		   	   	_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, ==, Err,  int )
#define ARGCHECK_ERRD_ON_NE_TIMES( _T1, _T2 )		   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, ==, ErrD, double )
#define ARGCHECK_ERRN_ON_NE_TIMES( _T1, _T2, _Cast )   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, ==, ErrN, _Cast )
#define ARGCHECK_ERR_ON_GT_TIMES( _T1, _T2 )		   	   	_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, <=, Err,  int )
#define ARGCHECK_ERRD_ON_GT_TIMES( _T1, _T2 )		   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, <=, ErrD, double )
#define ARGCHECK_ERRN_ON_GT_TIMES( _T1, _T2, _Cast )   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, <=, ErrN, _Cast )
#define ARGCHECK_ERR_ON_GE_TIMES( _T1, _T2 )		   	   	_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2,  <, Err,  int )
#define ARGCHECK_ERRD_ON_GE_TIMES( _T1, _T2 )		   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2,  <, ErrD , double)
#define ARGCHECK_ERRN_ON_GE_TIMES( _T1, _T2, _Cast )   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2,  <, ErrN , _Cast )
#define ARGCHECK_ERR_ON_LT_TIMES( _T1, _T2 )		   	   	_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, >=, Err,  int )
#define ARGCHECK_ERRD_ON_LT_TIMES( _T1, _T2 )		   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, >=, ErrD, double )
#define ARGCHECK_ERRN_ON_LT_TIMES( _T1, _T2, _Cast )   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2, >=, ErrN, _Cast )
#define ARGCHECK_ERR_ON_LE_TIMES( _T1, _T2 )		   	   	_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2,  >, Err,  int )
#define ARGCHECK_ERRD_ON_LE_TIMES( _T1, _T2 )		   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2,  >, ErrD, double )
#define ARGCHECK_ERRN_ON_LE_TIMES( _T1, _T2, _Cast )   		_ARGCHECK_COMPARE_TIME_MACRO( _T1, _T2,  >, ErrN, _Cast )

/*
**	Check RDATEs
*/
#define ARGCHECK_ERR_ON_INVALID_RDATE( _x )					_ARGCHECK_INVALID_RDATE_MACRO( _x, Err, int )
#define ARGCHECK_ERRD_ON_INVALID_RDATE( _x )				_ARGCHECK_INVALID_RDATE_MACRO( _x, ErrD, double )
#define ARGCHECK_ERRN_ON_INVALID_RDATE( _x, _Cast )			_ARGCHECK_INVALID_RDATE_MACRO( _x, ErrN, _Cast )

/*
**	Check arbitrary boolean conditions
*/
#define ARGCHECK_ERR_ON_CONDITION( _cond )					_ARGCHECK_CONDITION_MACRO( _cond, Err, int )
#define ARGCHECK_ERRD_ON_CONDITION( _cond )					_ARGCHECK_CONDITION_MACRO( _cond, ErrD, double )
#define ARGCHECK_ERRN_ON_CONDITION( _cond, _Cast )			_ARGCHECK_CONDITION_MACRO( _cond, ErrN, _Cast )

/*
**	Check enumerated types
*/
#define ARGCHECK_ERR_ON_INVALID_Q_OPT_TYPE( _x )			_ARGCHECK_INVALID_Q_OPT_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_OPT_TYPE( _x )			_ARGCHECK_INVALID_Q_OPT_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_RETURN_TYPE( _x )	   		_ARGCHECK_INVALID_Q_RETURN_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_RETURN_TYPE( _x )		_ARGCHECK_INVALID_Q_RETURN_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_IN_OUT( _x )				_ARGCHECK_INVALID_Q_IN_OUT_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_IN_OUT( _x )				_ARGCHECK_INVALID_Q_IN_OUT_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_REBATE_TYPE( _x )	   		_ARGCHECK_INVALID_Q_REBATE_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_REBATE_TYPE( _x )	   	_ARGCHECK_INVALID_Q_REBATE_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_OPT_STYLE( _x )	   		_ARGCHECK_INVALID_Q_OPT_STYLE _MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_OPT_STYLE( _x )	   		_ARGCHECK_INVALID_Q_OPT_STYLE _MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_OPT_ON( _x )	   			_ARGCHECK_INVALID_Q_OPT_ON_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_OPT_ON( _x )	   			_ARGCHECK_INVALID_Q_OPT_ON_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_MARGIN( _x )	   			_ARGCHECK_INVALID_Q_MARGIN_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_MARGIN( _x )	   			_ARGCHECK_INVALID_Q_MARGIN_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_UP_DOWN( _x )	   			_ARGCHECK_INVALID_Q_UP_DOWN_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_UP_DOWN( _x )	   		_ARGCHECK_INVALID_Q_UP_DOWN_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_ALIVE_DEAD( _x )	   		_ARGCHECK_INVALID_Q_ALIVE_DEAD_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_ALIVE_DEAD( _x )	   		_ARGCHECK_INVALID_Q_ALIVE_DEAD_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_BARRIER_TYPE( _x )	   	_ARGCHECK_INVALID_Q_BARRIER_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_BARRIER_TYPE( _x )		_ARGCHECK_INVALID_Q_BARRIER_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_FADEOUT_TYPE( _x )	   	_ARGCHECK_INVALID_Q_FADEOUT_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_FADEOUT_TYPE( _x )		_ARGCHECK_INVALID_Q_FADEOUT_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_BEST_WORST( _x )	   		_ARGCHECK_INVALID_Q_BEST_WORST_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_BEST_WORST( _x )	   		_ARGCHECK_INVALID_Q_BEST_WORST_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_AVG_TYPE( _x )	   		_ARGCHECK_INVALID_Q_AVG_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_AVG_TYPE( _x )	   		_ARGCHECK_INVALID_Q_AVG_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_KO_TYPE( _x )	   			_ARGCHECK_INVALID_Q_KO_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_KO_TYPE( _x )	   		_ARGCHECK_INVALID_Q_KO_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_DAY_COUNT( _x )	   		_ARGCHECK_INVALID_Q_DAY_COUNT _MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_DAY_COUNT( _x )	   		_ARGCHECK_INVALID_Q_DAY_COUNT _MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_OFFSET_TYPE ( _x )	   	_ARGCHECK_INVALID_Q_OFFSET_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_OFFSET_TYPE ( _x )		_ARGCHECK_INVALID_Q_OFFSET_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_COMPOUNDING_TYPE( _x )	_ARGCHECK_INVALID_Q_COMPOUNDING_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_COMPOUNDING_TYPE( _x )	_ARGCHECK_INVALID_Q_COMPOUNDING_TYPE_MACRO( _x, ErrD )
#define ARGCHECK_ERR_ON_INVALID_Q_FN_TYPE( _x )	   			_ARGCHECK_INVALID_Q_FN_TYPE_MACRO( _x, Err )
#define ARGCHECK_ERRD_ON_INVALID_Q_FN_TYPE( _x )	   		_ARGCHECK_INVALID_Q_FN_TYPE_MACRO( _x, ErrD )

#endif
