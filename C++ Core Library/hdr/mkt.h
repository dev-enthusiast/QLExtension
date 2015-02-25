/* $Header: /home/cvs/src/mkt/src/mkt.h,v 1.28 2001/06/28 16:05:59 goodfj Exp $ */
/****************************************************************
**
**	MKT.H	
**
**	$Revision: 1.28 $
**
****************************************************************/

#if !defined( IN_MKT_H )
#define IN_MKT_H

#if !defined( IN_DATE_H )
#	include		<date.h>
#endif

#if !defined( IN_GOB_H )
#	include		<gob.h>
#endif


/*
**	Diddle assistance
*/

typedef DT_CURVE *(DIDDLE_CALLBACK)( DT_CURVE *Diddle, DATE PricingDate );

DLLEXPORT SDB_DIDDLE_ID
		MktChangeDiddle( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, DT_VALUE *Results, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleID );

typedef SDB_DIDDLE_ID DIDDLE_FUNC( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, DT_VALUE *Results, SDB_SET_FLAGS Flags, SDB_DIDDLE_ID DiddleID );

#define	GET_DIDDLE_FUNC()	DiddleFunc = ( SDB_MSG_DIDDLE_SET == Msg ) ? SecDbSetDiddle : MktChangeDiddle


DLLEXPORT int
		MktValueFuncCross(			GOB_VALUE_FUNC_ARGS ),
		MktValueFuncPrefix(			GOB_VALUE_FUNC_ARGS ),
		MktValueFuncVolPrefix(		GOB_VALUE_FUNC_ARGS );

#endif
