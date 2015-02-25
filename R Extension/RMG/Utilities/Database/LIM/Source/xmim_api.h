/* ******************************************************* 
 * xmim_api.h -- 
 * 
 * Copyright (c) 1991-2004 by LIM International
 * All Rights Reserved.
 *
 * History:
 * ESP3251: TLS XmimEnableCancelling, XmimForceDisconnect
 * Move C++ ostream friends into xmim_api_ostream.h
 * *******************************************************/

#ifndef _XMIM_API_H_
#define _XMIM_API_H_

#if defined(__STDC__) || defined(__cplusplus) || defined(WIN32)
#define CONST const
#define PROTO(x) x
#else
#define CONST    
#define PROTO(x) ()
#endif

#ifdef WIN32
//typedef unsigned int size_t;
//typedef int ssize_t;  // this line causes error on "win_build" vmware machine

#ifdef BUILD_LIB    
#define DllDecl __declspec( dllexport )
#elif defined(NON_XMIM_LIB)
#define DllDecl
#else
#define DllDecl __declspec( dllimport )
#endif /* BUILD_LIB */

#else
#define DllDecl
    
#endif /* WIN32 */

#ifdef __cplusplus

extern "C" {
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef False
#define False 0
#endif
#ifndef false
#define false 0
#endif    
#ifndef TRUE
#define TRUE 1
#endif
#ifndef True
#define True 1
#endif
#ifndef true
#define true 1
#endif
    
/********************************/
/*				*/    
/* 	Global Data Types 	*/		
/*				*/    
/********************************/
       
    typedef char		XmimBoolean;
    typedef char*		XmimString;
    typedef int			XmimClientHandle;
    typedef int			XmimTableHandle;
    typedef int 		XmimCursorHandle;
    typedef unsigned long 	XmimQueryResultHandle;
    typedef unsigned long 	XmimQueryReportHandle;
    typedef unsigned long 	XmimQueryReportBlockHandle;
    typedef unsigned long	XmimQueryPLReportHandle;

    typedef enum {
	XMIM_END_ARGS = 0,

	XMIM_CLIENT_HANDLE,

	XMIM_SERVER_NAME,
	XMIM_SERVER_NUMBER,

	XMIM_DATABASE,
	XMIM_DB_LOCKED,
	
	XMIM_ORIG_RELATION,
	XMIM_RELATION,
	XMIM_RELATION_LIST,
	XMIM_RELATION_ARRAY,
	XMIM_RELATION_FILE,

	XMIM_PARENT,
	XMIM_TYPE,
	XMIM_DESCRIPTION,
	XMIM_DESCRIPTION_ARRAY,
	XMIM_DESCRIPTION_LIST,
	XMIM_DESCRIPTION_FILE,
	XMIM_EXCHANGE,
	XMIM_EXCHANGE_ARRAY,
	XMIM_TIME_ZONE,
	XMIM_TIME_ZONE_ARRAY,
	XMIM_START_TRADE,
	XMIM_END_TRADE,
	XMIM_CONTRACT_UNITS,
	XMIM_EXPIRATION_DATE,
	XMIM_FIRST_NOTICE_DATE,
	XMIM_ROLLOVER_DAY,
	XMIM_ROLLOVER_DAY_STRUCT,
	XMIM_ROLLOVER_POLICY,
	XMIM_ROLLOVER_POLICY_STRUCT,
	XMIM_ROLLOVER_DATA_TYPE,
	XMIM_STUDY_RULE,
	XMIM_NUM_CONTRACTS,
	XMIM_NUM_PERIODS,
	XMIM_CONTRACTS,
	XMIM_FROM_UNITS,
	XMIM_TO_UNITS,
	XMIM_ROLL_DATES,

	XMIM_PATH,
	
	XMIM_ORIG_COLUMN,
	XMIM_COLUMN,
	XMIM_COLUMN_LIST,
	XMIM_COLUMN_ARRAY,
	XMIM_COLUMN_FILE,

	XMIM_NUM_FIELDS,
	
	XMIM_OBJ_TYPE,
	XMIM_OBJ_TYPE_ARRAY,
	XMIM_OBJ_TYPE_LIST,
	XMIM_OBJ_TYPE_FILE,
	
	XMIM_AGGR_RULE,
	XMIM_AGGR_RULE_LIST,
	XMIM_AGGR_RULE_ARRAY,
	XMIM_AGGR_RULE_FILE,
	
	XMIM_DELTA,
	XMIM_DELTA_LIST,
	XMIM_DELTA_ARRAY,
	XMIM_DELTA_FILE,
	
	XMIM_CONSTANT,
	XMIM_CONSTANT_ARRAY,
	XMIM_CONSTANT_FILE,

	XMIM_QUERY,
	XMIM_QUERY_HAS_CLIENT_MACROS,
	XMIM_QUERY_FILE_NAME,
	XMIM_QUERY_OUT_FILE_NAME,
	XMIM_QUERY_SUBSTITUTION,
	XMIM_QUERY_SUBSTITUTION_ARRAY,
	XMIM_QUERY_SUBSTITUTION_LIST,
	XMIM_QUERY_SUBSTITUTION_FILE,
	XMIM_QUERY_VERBOSE_MODE,
	XMIM_APPEND_MODE,
	XMIM_QUERY_APPEND_MODE,		/* for backward compatibility*/
	XMIM_PRINT,
	
	XMIM_FILENAME,

	XMIM_OUTPUT_FILENAME,
	
	XMIM_FORMAT,
	XMIM_FORMAT_LIST,
	XMIM_FORMAT_ARRAY,
	XMIM_FORMAT_FILE,
	XMIM_RECORD_FORMAT,
	XMIM_MERGE_MODE,
	XMIM_TICK_MODE,

	XMIM_UNITS,
	XMIM_FROM_DATE,
	XMIM_TO_DATE,
	XMIM_FROM_TIME,
	XMIM_TO_TIME,
	XMIM_LIMIT,

	XMIM_HAS_CHILDREN,
	
	XMIM_ROOT,

	XMIM_NUM_RECORDS,
	XMIM_VALUES,
	XMIM_DATE_TIMES,
	XMIM_DATE_TIME_OBJ,
	XMIM_TIMES,
	XMIM_DATE,
	XMIM_DATE_TIME,
	XMIM_FILL_OPTION,

	XMIM_CONDITION,
	XMIM_CONDITION_ARRAY,
	XMIM_CONDITION_LIST,
	XMIM_CONDITION_FILE,
	XMIM_SHOW_ATTR,
	XMIM_SHOW_ATTR_LIST,
	XMIM_SHOW_ATTR_ARRAY,
	XMIM_SHOW_ATTR_FILE,
	XMIM_SHOW_BEFORE_REPEAT,
	XMIM_SHOW_AFTER_REPEAT,
	XMIM_CONDITION_SUCCESS,

	XMIM_NAN_VALUE,
	XMIM_HOLIDAY_NAN_VALUE,
	XMIM_PRINT_SECONDS,
	XMIM_DATE_FORMAT,
	XMIM_EXP_DATE_FORMAT,
	XMIM_TIME_FORMAT,

	XMIM_MAX_DEPTH,
	XMIM_ALL_CONTRACTS,
	XMIM_VERBOSE,

	XMIM_PATTERN,
	XMIM_SEARCH_BY_NAME,
	XMIM_SEARCH_FILTER,
	XMIM_CASE_SENSITIVE,
	
	XMIM_REAL_TICK,
	XMIM_INTRADAY,
	XMIM_DAILY,
	XMIM_FIELD,
	XMIM_FIELD_ARRAY,
	XMIM_FIELD_LIST,
	XMIM_FIELD_FILE,

	XMIM_FROM_EXP_DATE,
	XMIM_TO_EXP_DATE,
	XMIM_EXPIRATION_DATES,
	XMIM_OPTION_TYPE,
	XMIM_OPTION_TYPES,
	XMIM_FROM_STRIKE,
	XMIM_TO_STRIKE,
	XMIM_STRIKE_PRICE,
	XMIM_STRIKE_PRICES,
	XMIM_STRIKE_WEIGHT,
	XMIM_SELECT_ONCE,
	XMIM_UNITS_TO_EXPIR,

	XMIM_SET_COMPRESSION,
	XMIM_SIZE,

	XMIM_LOG_LEVEL,
	XMIM_LOG_MESSAGE,
	XMIM_CURRENT_TICK_USAGE,
	XMIM_MEMORY_MAP,
	XMIM_MEMORY_MAP_ARRAY,
	XMIM_PROCESS_ID,
	XMIM_USER_NAME,
	XMIM_NUM_DATES,
	XMIM_DATES,
	XMIM_LAUNCH_SERVER,	/* backward compatibility */
	XMIM_KILL_SERVER,	/* backward compatibility */

	XMIM_TABLE_HANDLE,
	XMIM_CURSOR_HANDLE,
	XMIM_CURSOR_EXHAUSTED,
	XMIM_KEY_FIELD_INDEX,
	XMIM_INHERIT_FIELD_INDEX,
	XMIM_SINGLE_VALUE,
	XMIM_READ_ONLY,
	XMIM_NEW_NAME,
	XMIM_TABLE_NAME,
	XMIM_TABLE_ARRAY,
	XMIM_TUPLE,
	XMIM_TUPLE_INDEX,

	XMIM_TIME,
	XMIM_VALUE,

	XMIM_TRADING_PATTERN,
	XMIM_SEGMENTS,

	XMIM_LET,
	XMIM_LET_LIST,
	XMIM_LET_ARRAY,
	XMIM_LET_FILE,

	XMIM_INDEX,
	XMIM_QUERY_RESULT_HANDLE,
	XMIM_QUERY_NUM_REPORTS,
	XMIM_QUERY_REPORT_HANDLE,
	XMIM_QUERY_FINAL_REPORT_HANDLE,
	XMIM_QUERY_REPORT_TITLE,
	XMIM_QUERY_NUM_REPORT_BLOCKS,
	XMIM_QUERY_REPORT_BLOCK_HANDLE,
	XMIM_QUERY_REPORT_BLOCK_NUM_ATTRS,
	XMIM_QUERY_REPORT_BLOCK_ATTR_HEADING,
	XMIM_QUERY_REPORT_BLOCK_NUM_COLS,
	XMIM_QUERY_REPORT_BLOCK_COL_HEADING,
	XMIM_QUERY_REPORT_BLOCK_NUM_ROWS,
	XMIM_QUERY_REPORT_BLOCK_HAS_TIMES,
	XMIM_QUERY_REPORT_BLOCK_HAS_REPPEAT,
	XMIM_QUERY_REPORT_BLOCK_NUM_SUMMARY_RECORDS,
	XMIM_QUERY_SUMMARY_RECORD,
	XMIM_QUERY_PL_REPORT_HANDLE,
	XMIM_QUERY_PL_NUM_TRADES,
	XMIM_QUERY_PL_TRADE_RECORD,
	XMIM_QUERY_PL_STAT_RECORD,
	XMIM_QUERY_PL_STAT_ITEM,
	XMIM_QUERY_PL_NUM_EQUITY_RECORDS,
	XMIM_QUERY_PL_EQUITY_RECORD,

	XMIM_CORRECTIONS_DATE,

	XMIM_FUTURES_ARRAY,
	
	XMIM_DATABASE_LIST,
	XMIM_DATABASE_ARRAY,
	XMIM_DATABASE_FILE,
	XMIM_FUNCTION,
	XMIM_FUNCTION_ARRAY,
	XMIM_ARG_ARRAY,
	XMIM_ARG_LIST,
	XMIM_ARG_FILE,
	XMIM_ARG,
	XMIM_RESULT_ARRAY,
	XMIM_DESCRIPTION_ARRAYS,

	XMIM_DAILY_MULTIPLICITY,
	XMIM_INTRADAY_MULTIPLICITY,
	XMIM_TICK_MULTIPLICITY,

	XMIM_CLIENT_TYPE,

	XMIM_GROUP,
	XMIM_GROUP_ARRAY,
	XMIM_PERMISSION_ARRAY,
	XMIM_PERMISSION,
	XMIM_PASSWORD,
	XMIM_NEW_PASSWORD,
	XMIM_USER,
	XMIM_USER_ARRAY,
	XMIM_ROOT_P,
	XMIM_REC_SIZE,
	XMIM_UNION_DATE,
	XMIM_PASS_FAIL,
	XMIM_RESULT,
	XMIM_FORMULA,  /*ESP9039*/
	XMIM_FORMULA_INDEX,
	XMIM_INCLUDE_HIDDEN, /* ESP20339*/
	XMIM_TIME_CHANGE,
	XMIM_DIRNAME
    } XmimVaArgs;

#define XMIM_HISTORIC_ONLY  XMIM_CURRENT_TICK_USAGE
    
    typedef struct {
	unsigned	year   	    : 12;
	unsigned	month  	    :  4;
	unsigned	day    	    :  5;
	unsigned	hour   	    :  5;
	unsigned	minute 	    :  6;
	unsigned	second 	    :  6;
	unsigned	millisecond : 10;
    } XmimDateTime; 

    typedef struct {
	unsigned	year   : 12;
	unsigned	month  :  4;
	unsigned	day    :  5;
    } XmimDate;

    typedef struct {
	unsigned	hour   :  5;
	unsigned	minute :  6;
	unsigned	second :  6;
    } XmimTime;

    typedef enum {
	XMIM_REL_INVALID,
	XMIM_REL_CATEGORY,
	XMIM_REL_NORMAL,
	XMIM_REL_FUTURES,
	XMIM_REL_FUTURES_CONTRACT,
	XMIM_REL_FUTURES_CONTINUOUS,
	XMIM_REL_OPTIONS
    } XmimRelType;

    typedef enum {
	XMIM_ROLLOVER_DATA_INVALID = -1,
	XMIM_ROLLOVER_NONE,
	XMIM_ROLLOVER_DAILY,
	XMIM_ROLLOVER_TICK,
	XMIM_ROLLOVER_BOTH
    } XmimRolloverDataType;
    
    typedef enum {
	XMIM_COL_INVALID,
	XMIM_COL_CATEGORY,
	XMIM_COL_NORMAL,
	XMIM_COL_COMPOSITE
    } XmimColType;

    typedef enum {
	XMIM_RELCOL_INVALID = 1,
	XMIM_RELCOL_BASE,
	XMIM_RELCOL_CONSTANT,
	XMIM_RELCOL_SPARSE,
	XMIM_RELCOL_INHERIT
    } XmimRelColType;

    typedef enum {
	XMIM_RELCOL_OBJ_INVALID = 1,
	XMIM_RELCOL_OBJ_FLOAT,
	XMIM_RELCOL_OBJ_DOUBLE,
	XMIM_RELCOL_OBJ_INT,
	XMIM_RELCOL_OBJ_DATE,
	XMIM_RELCOL_OBJ_TIME,
	XMIM_RELCOL_OBJ_BOOLEAN
    } XmimRelColObjType;

#define XMIM_RELCOL_OBJ_BLOCK(i)	-i

    typedef enum {
	XMIM_RELCOL_AGGR_INVALID = 1,
	XMIM_RELCOL_AGGR_OPEN,
	XMIM_RELCOL_AGGR_HIGH,
	XMIM_RELCOL_AGGR_LOW,
	XMIM_RELCOL_AGGR_CLOSE,
	XMIM_RELCOL_AGGR_SUM,
	XMIM_RELCOL_AGGR_AVERAGE,
	XMIM_RELCOL_AGGR_KEY,
	XMIM_RELCOL_AGGR_INHERIT
    } XmimRelColAggr;

    typedef enum {
	XMIM_RELCOL_DELTA_INVALID = 2,
	XMIM_RELCOL_DELTA_NONE  =  1,
	XMIM_RELCOL_DELTA_8THS  = -1,
	XMIM_RELCOL_DELTA_16THS = -2,
	XMIM_RELCOL_DELTA_32NDS = -3,
	XMIM_RELCOL_DELTA_64THS = -4
    } XmimRelColDelta;

    typedef enum {
	XMIM_RELCOL_MULTIPLICITY_INVALID = -1,
	XMIM_RELCOL_MULTIPLICITY_SINGLE = 0,
	XMIM_RELCOL_MULTIPLICITY_MULTIPLE = 1
    } XmimRelColMultiplicity;

    typedef enum {
	XMIM_RECFMT_FREE,
	XMIM_RECFMT_FIXED
    } XmimRecordFormat;

    typedef enum {
	XMIM_REPLACE = 0,
	XMIM_QUERY_REPLACE = 0,		/*for backward compatibility*/
	XMIM_MERGE = 1,
	XMIM_APPEND = 1,
	XMIM_QUERY_APPEND = 1		/*for backward compatibility*/
    } XmimMergeMode;

    typedef XmimMergeMode  XmimAppendMode;
	
    typedef enum {
	XMIM_TICK_NONE,
	XMIM_TICK_PRE_SAMPLED,
	XMIM_TICK_BY_TICK,
	XMIM_TICK_CONVERT,
	XMIM_TICK_MILLISECOND
    } XmimTickMode;

    typedef enum {
	XMIM_FIELD_DATE = 1,
	XMIM_FIELD_TIME,
	XMIM_FIELD_RELATION,
	XMIM_FIELD_COLUMN,
	XMIM_FIELD_EXP_DATE,
	XMIM_FIELD_OPT_TYPE,
	XMIM_FIELD_STRIKE,
	XMIM_FIELD_FILLER
    } XmimFieldType;
    
    typedef struct {
	XmimFieldType	field;
	short		width;
	short		decimal;
    } XmimFieldFormat;

    typedef enum {
	XMIM_UNITS_INVALID = 0,
	XMIM_MILLISECONDS  = -3,
	XMIM_SECONDS  = 1,
	XMIM_MINUTES  = 2,
	XMIM_HOURS    = 3,
	XMIM_DAYS     = 4,
	XMIM_WEEKS    = 5,
	XMIM_MONTHS   = 6,
	XMIM_QUARTERS = 7,
	XMIM_YEARS    = 8
    } XmimUnits;

    typedef enum {
	XMIM_LIMIT_INVALID = -1,
	XMIM_LIMIT_BY_RECORDS,
	XMIM_LIMIT_BY_MEMORY
    } XmimLimitMode;

    typedef enum {
	XMIM_FILL_INVALID = -1,
	XMIM_FILL_NAN = 0,
	XMIM_FILL_FORWARD,
	XMIM_FILL_BACKWARD,
	XMIM_FILL_INTERP_LIN,
	XMIM_FILL_INTERP_GEO,
	XMIM_FILL_INTERP_LOG,
	XMIM_FILL_NEAREST
    } XmimFillOption;

    typedef enum {
	XMIM_SKIP_INVALID = -1,
	XMIM_SKIP_NONE,
	XMIM_SKIP_ALL_NAN
    } XmimSkipAllNaN;
    
    typedef enum {
	XMIM_NAN_TYPE_HOLIDAY,
	XMIM_NAN_TYPE_MISS_DATA,
	XMIM_NAN_TYPE_ALL
    } XmimNaNType;
    
    typedef enum {
	XMIM_DATE_mmsddsyy = 1,		/*  03/10/84 */
	XMIM_DATE_mmsddsyyyy,		/*  03/10/1984 */
	XMIM_DATE_ddsmmsyy,		/*  10/03/84 */
	XMIM_DATE_ddsmmsyyyy,		/*  10/03/1984 */
  
	XMIM_DATE_mm_dd_yy,		/*  03-10-84 */
	XMIM_DATE_mm_dd_yyyy,		/*  03-10-1984 */
	XMIM_DATE_dd_mm_yy,		/*  10-03-84 */
	XMIM_DATE_dd_mm_yyyy,		/*  10-03-1984 */
  
	XMIM_DATE_yymmdd,		/* 840310 */
	XMIM_DATE_yyyymmdd,		/* 19840310 */
  
	XMIM_DATE_mon_dd_yy,		/* Mar 10, 84 */
	XMIM_DATE_mon_dd_yyyy,		/* Mar 10, 1984 */
	
	XMIM_DATE_monp_dd_yy,		/* Mar. 10, 84 */
	XMIM_DATE_monp_dd_yyyy,		/* Mar. 10, 1984 */
	
	XMIM_DATE_month_dd_yy,		/* March 10, 84 */
	XMIM_DATE_month_dd_yyyy,	/* March 10, 1984 */
  
	XMIM_DATE_dd_mon_yy,		/* 10-Mar-84 */
	XMIM_DATE_dd_mon_yyyy,		/* 10-Mar-1984 */
	XMIM_DATE_dd_month_yy,		/* 10-March-84 */
	XMIM_DATE_dd_month_yyyy		/* 10-March-1984 */

    } XmimDateFormat;

    typedef enum {
	XMIM_TIME_hh_mm_am = 1,		 /*  9:05am */
	XMIM_TIME_hh_mm_AM,		 /* 11:15AM */
	XMIM_TIME_hhmm_am,		 /*  905am */
	XMIM_TIME_hhmm_AM,		 /* 1115AM */
	XMIM_TIME_hh_mm,		 /* 09:05 */
	XMIM_TIME_hhmm,			 /* 2315 */
	XMIM_TIME_hh_mm_ss_am,		 /*  9:05:01am */
	XMIM_TIME_hh_mm_ss_AM,		 /* 11:15:01AM */
	XMIM_TIME_hhmmss_am,		 /*  90501am */
	XMIM_TIME_hhmmss_AM,		 /* 111501AM */
	XMIM_TIME_hh_mm_ss,		 /* 09:05:01 */
	XMIM_TIME_hhmmss		 /* 231501 */
    } XmimTimeFormat;

    typedef enum {
	XMIM_QUERY_SILENT,
	XMIM_QUERY_VERBOSE,
	XMIM_QUERY_VERY_VERBOSE
    } XmimVerboseMode;
    
    typedef struct {
	char	*leftHandSide;
	int	numRightHandSides;
	char	**rightHandSides;
    } XmimSubstitution;

    typedef enum {
	XMIM_SWITCH_INVALID = 0,
	XMIM_SWITCH_LAST_TRADING_DAY = 1,
	XMIM_SWITCH_LAST_DATA_DAY = 1,
	XMIM_SWITCH_USER_DEFINED,
	XMIM_SWITCH_FIRST_NOTICE_DAY,
	XMIM_SWITCH_EXPIRATION_DAY,
	XMIM_SWITCH_DATE_EXPRESSION,
	XMIM_SWITCH_VOLUME_CROSSOVER,
	XMIM_SWITCH_OPEN_INTEREST_CROSSOVER
    } XmimSwitchingMethod;

    typedef enum {
	XMIM_DAY_INVALID = 0,
	XMIM_DAY_MON,
	XMIM_DAY_TUE,
	XMIM_DAY_WED,
	XMIM_DAY_THU,
	XMIM_DAY_FRI,
	XMIM_DAY_SAT,
	XMIM_DAY_SUN,
	XMIM_DAY_OF_MONTH
    } XmimDay;

    typedef enum {
	XMIM_MONTH_INVALID = 0,
	XMIM_MONTH_JAN,
	XMIM_MONTH_FEB,
	XMIM_MONTH_MAR,
	XMIM_MONTH_APR,
	XMIM_MONTH_MAY,
	XMIM_MONTH_JUN,
	XMIM_MONTH_JUL,
	XMIM_MONTH_AUG,
	XMIM_MONTH_SEP,
	XMIM_MONTH_OCT,
	XMIM_MONTH_NOV,
	XMIM_MONTH_DEC
    } XmimMonth;
    
    typedef struct {
	int			nTh;
	    /* the member variable nTh is the compilation of 		*/
	    /* the `(nTH | [nTH TO] LAST)' clause; its inerpretation is:*/
	    /* 		o 1 is first, 2 is second, etc.			*/
	    /*		o 0 is invalid					*/
	    /*		o -1 is last, -2 is "2nd to last", etc.		*/
	XmimDay			theDay;
	    /* theDay encodes the compilation of the 			*/
	    /* `(DAY | day_of_week)' clause				*/
	int			numMonths;
	    /* numMonths encodes the compilation of the 		*/
	    /*`[n MONTHS BEFORE]' clause				*/
    } XmimDateExpression;
    
    typedef struct {
	int			numUnits;
	    /* 'numUnits < 0' means 'before' 				*/
	    /* 'numUnits > 0' means 'after' 				*/
	    /* 'numUnits == 0' means no units specification (default) 	*/
	XmimUnits		units;
	XmimSwitchingMethod	method;
	XmimDateExpression	dateExpression;
	XmimMonth		contractMonth;
    } XmimRolloverDay;

    typedef enum {
	XMIM_ROLLOVER_INVALID,
	XMIM_ROLLOVER_ACTUAL_PRICES,
	XMIM_ROLLOVER_ADJUSTED_PRICES,
	XMIM_ROLLOVER_PERPETUAL,
	XMIM_ROLLOVER_SMOOTHED
    } XmimRolloverMethod;
        
    typedef enum {
	XMIM_ADJUST_INVALID,
	XMIM_ADJUST_FORWARD,
	XMIM_ADJUST_BACKWARD,
	XMIM_ADJUST_CASH
    } XmimAdjustMethod;

    typedef enum {
	XMIM_INTERPOLATE_INVALID,
	XMIM_INTERPOLATE_LINEAR,
	XMIM_INTERPOLATE_LOG,
	XMIM_INTERPOLATE_GEOMETRIC
    } XmimInterpAlgorithm;

    typedef struct {
	int			numContracts;
	XmimInterpAlgorithm	algorithm;
    } XmimSmoothMethod;
    
    typedef struct {
	int			numUnits;
	XmimUnits		units;
	XmimInterpAlgorithm	algorithm;
    } XmimInterpMethod;

    typedef struct {
	XmimRolloverMethod 	rolloverMethod;
	union {
	    XmimAdjustMethod	adjustMethod;
	    XmimSmoothMethod	smoothMethod;
	    XmimInterpMethod	perpetMethod;
	} theMethod;
    } XmimRolloverDetail;

    typedef enum {
	XMIM_YEAR_TO_YEAR_NO  = 0,
	XMIM_YEAR_TO_YEAR_JAN = 0x0001,
	XMIM_YEAR_TO_YEAR_FEB = 0x0002,
	XMIM_YEAR_TO_YEAR_MAR = 0x0004,
	XMIM_YEAR_TO_YEAR_APR = 0x0008,
	XMIM_YEAR_TO_YEAR_MAY = 0x0010,
	XMIM_YEAR_TO_YEAR_JUN = 0x0020,
	XMIM_YEAR_TO_YEAR_JUL = 0x0040,
	XMIM_YEAR_TO_YEAR_AUG = 0x0080,
	XMIM_YEAR_TO_YEAR_SEP = 0x0100,
	XMIM_YEAR_TO_YEAR_OCT = 0x0200,
	XMIM_YEAR_TO_YEAR_NOV = 0x0400,
	XMIM_YEAR_TO_YEAR_DEC = 0x0800,
	XMIM_YEAR_TO_YEAR_YES = 13
    } XmimYearToYear;
	
    typedef struct {
	int			nearby;
	XmimInterpMethod	interpolate;
	XmimYearToYear		yearToYear;
	XmimRolloverDetail	rolloverDetail;
    } XmimRolloverPolicy;

    typedef enum {
	XMIM_ADJ_STUDY_NONE,
	XMIM_ADJ_STUDY_MOVE,
	XMIM_ADJ_STUDY_PCT_MOVE,
	XMIM_ADJ_STUDY_AVG,
	XMIM_ADJ_STUDY_SUM,
	XMIM_ADJ_STUDY_COUNT,
	XMIM_ADJ_STUDY_HIGH,
	XMIM_ADJ_STUDY_LOW,
	XMIM_ADJ_STUDY_STDDEV,
	XMIM_ADJ_STUDY_REGRESSION,
	XMIM_ADJ_STUDY_VARIANCE
    }  XmimAdjustedStudy;
	
    typedef enum {
	XMIM_OPTION_CALL,
	XMIM_OPTION_PUT,
	XMIM_OPTION_ALL
    } XmimOptionType;
    
    typedef struct {
	unsigned	julNum;
	XmimDate	fromDate;
	XmimDate	toDate;
	int		numUnits;
	XmimUnits	units;
    } XmimDateTimeObj;

    typedef enum {
	XMIM_FATAL_ERROR = -1,
	XMIM_ERROR = 0,
	XMIM_SUCCESS,
	XMIM_WARNING
    } XmimReturnCode;
	
    typedef enum {
	XMIM_SEARCH_LEAVES,
	XMIM_SEARCH_NON_LEAVES,
	XMIM_SEARCH_ALL
    } XmimSearchFilter;

    typedef enum {
	XMIM_DATA_TICK,
	XMIM_DATA_INTRADAY,
	XMIM_DATA_DAILY,
	XMIM_DATA_ALL,
	XMIM_DATA_MILLISECOND
    } XmimDataType;

    typedef enum {
	XMIM_ERR_DUP_ADD,
	XMIM_ERR_DUP_DEL,
	XMIM_ERR_LOSE_WEEKEND,
	XMIM_ERR_RES_WORDS,
	XMIM_ERR_ALL,
	XMIM_ERR_RELATION_NOT_FOUND
    } XmimErrorType;

    typedef enum {
	XMIM_APPEND_TO_ALL = 0,      	/* to both daily and intraday */
	XMIM_APPEND_TO_NONE = 1,	/* do not use current tick */
	XMIM_APPEND_TO_DAILY = 2,    	/* current tick to daily only */
	XMIM_APPEND_TO_TICK = 3 	/* current tick to subday data only */
    } XmimCurrentTickUsage;
    
	typedef enum {
		XMIM_CORRECTIONS_DAILY=0,
		XMIM_CORRECTIONS_INTRADAY=1
	} XmimCorrectionType;

    typedef enum {
	XMIM_TAB_FIELD_INT,
	XMIM_TAB_FIELD_FLOAT,
	XMIM_TAB_FIELD_DOUBLE,
	XMIM_TAB_FIELD_DATE,
	XMIM_TAB_FIELD_TIME,
	XMIM_TAB_FIELD_STRING,
	XMIM_TAB_FIELD_ATOM,
	XMIM_TAB_FIELD_BLOCK,
	XMIM_TAB_FIELD_REL,
	XMIM_TAB_FIELD_COL
    } XmimTableFieldType;
    
    typedef struct {
	XmimString		name;
	XmimTableFieldType	type;
	int			length;
    } XmimTableFieldDesc;

    typedef struct {
	XmimBoolean		wildcardP;
	union {
	    int			intValue;
	    float		floatValue;
	    double		doubleValue;
	    XmimString		stringValue;
	    XmimDate		dateValue;
	    XmimTime		timeValue;
	} value;
    } XmimTableFieldVal;

    typedef struct {
	XmimTime		fromTime;
	XmimTime		toTime;
    } XmimTimePeriod;

    typedef char		XmimTradingPattern;

#define XMIM_MONDAY	(XmimTradingPattern)(1)
#define XMIM_TUESDAY	(XmimTradingPattern)(1<<1)
#define XMIM_WEDNESDAY	(XmimTradingPattern)(1<<2)
#define XMIM_THURSDAY	(XmimTradingPattern)(1<<3)
#define XMIM_FRIDAY	(XmimTradingPattern)(1<<4)
#define XMIM_SATURDAY	(XmimTradingPattern)(1<<5)
#define XMIM_SUNDAY	(XmimTradingPattern)(1<<6)
#define XMIM_MTWTF	(XmimTradingPattern)(XMIM_MONDAY|XMIM_TUESDAY|XMIM_WEDNESDAY|XMIM_THURSDAY|XMIM_FRIDAY)
#define XMIM_MTWTFSD	(XmimTradingPattern)(XMIM_MONDAY|XMIM_TUESDAY|XMIM_WEDNESDAY|XMIM_THURSDAY|XMIM_FRIDAY|XMIM_SATURDAY|XMIM_SUNDAY)
#define XMIM_MTWTFD	(XmimTradingPattern)(XMIM_MONDAY|XMIM_TUESDAY|XMIM_WEDNESDAY|XMIM_THURSDAY|XMIM_FRIDAY|XMIM_SUNDAY)        

    typedef struct {
	XmimDate		segStartDate;
	XmimTradingPattern	tradingPattern;
	int			numPeriods;
	XmimTimePeriod		*timePeriods;
    } XmimRelSegment;

    typedef struct {
	XmimString		var;
	XmimString		val;
    } XmimPair;

    typedef struct {
	double			sum;
	double			average;
	double			averagePositive;
	double			averageNegative;
	double			percentPositive;
	double			percentNegative;
	double			highest;
	double			lowest;
	double			standardDeviation;
	double			zstat;
	double			variance;
    } XmimQuerySummaryRecord;
    
    typedef enum {
	XMIM_PL_NUM_PERIODS,
	XMIM_PL_NUM_WIN_PERIODS,
	XMIM_PL_PCT_WIN_PERIODS,
	XMIM_PL_MAX_WIN_PERIODS,
	XMIM_PL_AVG_WIN_PERIODS,
	XMIM_PL_NUM_LOSE_PERIODS,
	XMIM_PL_PCT_LOSE_PERIODS,
	XMIM_PL_MAX_LOSE_PERIODS,
	XMIM_PL_AVG_LOSE_PERIODS,
    
	XMIM_PL_NUM_TRADES,
	XMIM_PL_NUM_WIN_TRADES,
	XMIM_PL_PCT_WIN_TRADES,
	XMIM_PL_MAX_WIN_TRADES,
	XMIM_PL_AVG_WIN_TRADES,
	XMIM_PL_PCT_NEW_HIGH_TRADES,
	XMIM_PL_NUM_LOSE_TRADES,
	XMIM_PL_PCT_LOSE_TRADES,
	XMIM_PL_MAX_LOSE_TRADES,
	XMIM_PL_AVG_LOSE_TRADES,
	XMIM_PL_PCT_NEW_LOW_TRADES,
	 
	XMIM_PL_AVG_LENGTH,
	XMIM_PL_AVG_WIN_LENGTH,
	XMIM_PL_AVG_LOSE_LENGTH,
	  
	XMIM_PL_NUM_CONTRACTS,
	XMIM_PL_MAX_CONTRACTS,

	XMIM_PL_AVG_TRADE,
	XMIM_PL_AVG_WIN_TRADE,
	XMIM_PL_AVG_LOSE_TRADE,
	XMIM_PL_RATIO_AVG,
	XMIM_PL_PROFIT_FACTOR,
	XMIM_PL_PESSIMISTIC_RETURN,
	XMIM_PL_PERFORMANCE_RATIO,

	XMIM_PL_AVG_PERIOD,
	XMIM_PL_CMP_AVG_PERIOD,
	XMIM_PL_PCT_HIGH_PERIOD,
	XMIM_PL_STD_DEV_PERIOD,
	XMIM_PL_SHARPE_RATIO,
	XMIM_PL_ZSTAT,
	XMIM_PL_STANDARD_ERROR,
	XMIM_PL_STD_ERROR_RATIO,
	XMIM_PL_GEOM_ERROR_RATIO,

	XMIM_PL_BIGGEST_WIN,
	XMIM_PL_BIGGEST_LOSS,
	XMIM_PL_MAX_OPEN_LOSS,
	XMIM_PL_MAX_OPEN_EQUITY,
	XMIM_PL_MIN_OPEN_EQUITY,
	XMIM_PL_MAX_CLOSED_EQUITY,
	XMIM_PL_MIN_CLOSED_EQUITY,
	XMIM_PL_MAX_DRAWDOWN,
    
	XMIM_PL_EQUITY,
	XMIM_PL_OPEN_EQUITY,
	XMIM_PL_CLOSED_EQUITY,
	XMIM_PL_COMMISSION,
	XMIM_PL_SLIPPAGE
    } XmimQueryPLStatItem;

    typedef enum {
	XMIM_PL_TRADE_BUY,
	XMIM_PL_TRADE_SELL,
	XMIM_PL_TRADE_LONG,
	XMIM_PL_TRADE_SHORT
    } XmimQueryPLTradeType;

    typedef enum {
	XMIM_TRADE_NO_REASON,
	XMIM_TRADE_SUBTRADE,
	XMIM_TRADE_DO_WHEN,
	XMIM_TRADE_DO_EXIT,
	XMIM_TRADE_ROLLOVER,
	XMIM_TRADE_PYRAMID,
	XMIM_TRADE_OVERNIGHT,
	XMIM_TRADE_STOP_LOSS,
	XMIM_TRADE_STOP_LOSS_TRAIL,
	XMIM_TRADE_PROFIT,
	XMIM_TRADE_PROFIT_TRAIL,
	XMIM_TRADE_REVERSE,
	XMIM_TRADE_EXPIRED
    } XmimQueryPLTradeReason;
    
    typedef struct {
	XmimQueryPLTradeType		type;
	XmimDate			entryDate;
	XmimTime			entryTime;
	char				entryReasonLabel[64];
	XmimQueryPLTradeReason		entryReason;
	XmimDate			exitDate;
	XmimTime			exitTime;
	char				exitReasonLabel[64];
	XmimQueryPLTradeReason		exitReason;
	int				numContracts;
	double				entryPrice;
	double				exitPrice;
	double				move;
	double				percentMove;
	double				profit;
	double				cumProfit;
    } XmimQueryPLTradeRecord;
    
    typedef struct {
	char				label[64];
	double				combinedValue;
	XmimDate			combinedDate;
	XmimTime			combinedTime;
	double				longValue;
	XmimDate			longDate;
	XmimTime			longTime;
	double				shortValue;
	XmimDate			shortDate;
	XmimTime			shortTime;
    } XmimQueryPLStatRecord;

    typedef struct {
	XmimDate			date;
	XmimTime			time;
	double				combined;
	double				open;
	double				close;
    } XmimQueryPLEquityRecord;

    typedef struct {
	XmimString			futuresRelName;
	int				numContracts;
	XmimString			*contractNames;
	XmimDate			*expirDates;
    } XmimContractsExpirDates;
    
    typedef struct
    {
	XmimString	relation;
	XmimString	column;
	int		readFlag;
	int		writeFlag;
    } XmimPermissionRecord; 

    extern DllDecl CONST
    XmimDate XMIM_INVALID_DATE;

    extern DllDecl CONST
    XmimTime XMIM_INVALID_TIME;

    extern DllDecl CONST
    XmimDateTime XMIM_INVALID_DATE_TIME;

    extern DllDecl CONST
    XmimDateTimeObj XMIM_INVALID_DATE_TIME_OBJ;

    extern DllDecl CONST
    XmimClientHandle XMIM_INVALID_HANDLE;

    extern DllDecl CONST
    XmimTableHandle XMIM_INVALID_TABLE_HANDLE;

    extern DllDecl CONST
    XmimCursorHandle XMIM_INVALID_TABLE_CURSOR;

    extern DllDecl CONST
    XmimQueryResultHandle XMIM_INVALID_QUERY_RESULT_HANDLE;

    extern DllDecl CONST
    XmimQueryReportHandle XMIM_INVALID_QUERY_REPORT_HANDLE;

    extern DllDecl CONST
    XmimQueryReportBlockHandle XMIM_INVALID_QUERY_REPORT_BLOCK_HANDLE;

    extern DllDecl CONST
    XmimQueryPLReportHandle XMIM_INVALID_PL_REPORT_HANDLE;
    
    extern DllDecl CONST
    XmimFieldFormat XMIM_FORMAT_DATE_FIELD;

    extern DllDecl CONST
    XmimFieldFormat  XMIM_FORMAT_EXP_DATE_FIELD;

    extern DllDecl CONST
    XmimFieldFormat  XMIM_FORMAT_OPT_TYPE_FIELD;

    extern DllDecl CONST
    XmimFieldFormat  XMIM_FORMAT_STRIKE_PRICE_FIELD;

    extern DllDecl CONST
    XmimFieldFormat  XMIM_FORMAT_TIME_FIELD;

    extern DllDecl CONST
    XmimFieldFormat  XMIM_FORMAT_RELATION_FIELD;

    extern DllDecl CONST
    XmimFieldFormat  XMIM_FORMAT_COLUMN_FIELD;

    extern DllDecl CONST
    XmimFieldFormat  XMIM_FORMAT_FILLER_FIELD;

    extern DllDecl float XMIM_MISS_DATA_NAN;
    extern DllDecl float XMIM_HOLIDAY_NAN;

    
/****************************************************************/
/*								*/
/* 	Opening and Closing Connection to Xmim Server 		*/
/*								*/
/****************************************************************/

    extern DllDecl XmimReturnCode XmimSetClientType
        PROTO((const char *type));
    
    extern DllDecl XmimReturnCode XmimConnect
	PROTO((char			*serverHostName,
	       int			serverNumber,
	       XmimClientHandle		*handle));

    extern DllDecl XmimReturnCode XmimVaConnect
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimDisconnect
	PROTO((XmimClientHandle		handle, ...));
   
    extern DllDecl XmimReturnCode XmimVaDisconnect
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimCheckServer
	PROTO((char			*serverHostName,
	       int			serverNumber,
	       XmimClientHandle		*handle));

    extern DllDecl XmimReturnCode XmimEnableCancelling
	PROTO((void));
        
    extern DllDecl XmimReturnCode XmimForceDisconnect
	PROTO((XmimClientHandle		handle, ...));

    extern DllDecl XmimReturnCode XmimBeginSessionForUser
	PROTO((XmimClientHandle		handle,
	       char*			user_name,
	       char*			host_name,
	       char*		        clnt_type));

    extern DllDecl XmimReturnCode XmimEndSessionForUser
	PROTO((XmimClientHandle		handle));
    
    extern DllDecl XmimReturnCode XmimGetServerInfo
	PROTO((XmimClientHandle		handle,
	       int			*serverNumber,
	       XmimString		*serverHost,
	       XmimString		*database,
	       XmimString		*mmapFile,
	       XmimString		*ownerName,
	       int			*pid));

    extern DllDecl XmimReturnCode XmimVaGetServerInfo
	PROTO((XmimVaArgs, ...));
    
/****************************************/
/*					*/
/*	Bmim Equivalent Commands 	*/
/*					*/
/****************************************/

    extern DllDecl XmimReturnCode XmimNewDatabase
	PROTO((XmimClientHandle		handle,
	       char			*database));
    
    extern DllDecl XmimReturnCode XmimVaNewDatabase
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimOpenDatabase
	PROTO((XmimClientHandle		handle,
	       char			*database));
    
    extern DllDecl XmimReturnCode XmimVaOpenDatabase
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimCloseDatabase
	PROTO((XmimClientHandle		handle,
	       char			*database));
    
    extern DllDecl XmimReturnCode XmimVaCloseDatabase
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimNarrowDatabases
	PROTO((XmimClientHandle		handle,
	       int			numDatabases,
	       char			**databases));
    
    extern DllDecl XmimReturnCode XmimVaNarrowDatabases
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimWidenDatabases
	PROTO((XmimClientHandle		handle));
    
    extern DllDecl XmimReturnCode XmimVaWidenDatabases
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimFocusDatabase
	PROTO((XmimClientHandle		handle,
	       char			*database));
    
    extern DllDecl XmimReturnCode XmimVaFocusDatabase
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetDatabases
	PROTO((XmimClientHandle		handle,
	       int			*numDatabases,
	       char			***databases));
    
    extern DllDecl XmimReturnCode XmimVaGetDatabases
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimLockDatabase
	PROTO((XmimClientHandle		handle));
    
    extern DllDecl XmimReturnCode XmimVaLockDatabase
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimIsDatabaseLocked
	PROTO((XmimClientHandle		handle,
	       XmimBoolean		*locked));
    
    extern DllDecl XmimReturnCode XmimVaIsDatabaseLocked
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimUnlockDatabase
	PROTO((XmimClientHandle		handle));

    extern DllDecl XmimReturnCode XmimVaUnlockDatabase
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDatabaseCommit
	PROTO((XmimClientHandle		handle));

    extern DllDecl XmimReturnCode XmimVaDatabaseCommit
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDeleteAllFiles
	PROTO((XmimClientHandle		handle));

    extern DllDecl XmimReturnCode XmimVaDeleteAllFiles
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimLoadBmimScript
	PROTO((XmimClientHandle		handle,
	       char			*scriptFileName,
	       char			*outputFileName));
	       
    extern DllDecl XmimReturnCode XmimVaLoadBmimScript
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimPrintSchema
	PROTO((XmimClientHandle		handle,
	       char			*relationName,
	       char			*fileName,
	       int			maxDepth,
	       XmimBoolean		allContracts,
	       XmimAppendMode		appendMode,
	       XmimBoolean		print,
	       XmimBoolean		verbose));
    
    extern DllDecl XmimReturnCode XmimVaPrintSchema
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimPrintColumns
	PROTO((XmimClientHandle		handle,
	       char			*columnName,
	       char			*fileName));
    
    extern DllDecl XmimReturnCode XmimVaPrintColumns
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimDeleteFacts
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*colName));

    extern DllDecl XmimReturnCode XmimVaDeleteFacts
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDeleteFactsSelective
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			numOfCols,
	       char			**colNames,
	       XmimDataType		type,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime));

    extern DllDecl XmimReturnCode XmimVaDeleteFactsSelective
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimSetHolidaySchedule
	PROTO((XmimClientHandle		handle,
	       char			*relOrExchangeName,
	       char			*fileName));

    extern DllDecl XmimReturnCode XmimVaSetHolidaySchedule
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimAddFormula
	PROTO((XmimClientHandle		handle,
	       char			*relationName,
	       char			*formulaFileName,
	       int			numOfColumns,
	       char			*columns[],
	       char			*post_process));

    extern DllDecl XmimReturnCode XmimModFormula
	PROTO((XmimClientHandle		handle,
	       char			*relationName,
	       char			*formulaFileName,
	       int			numOfColumns,
	       char			*columns[],
	       char			*post_process));

    extern DllDecl XmimReturnCode XmimAddRelation
	PROTO((XmimClientHandle		handle,
	       char			*relationName,
	       char			*description,
	       char			*parentName,
	       XmimRelType		type,
	       char			*exchangeName,
	       char			*timeZone,
	       XmimTime			startTrade,
	       XmimTime			endTrade,
	       float			contractUnits,
	       XmimDate			expirationDate,
	       XmimDate			firstNoticeDay,
	       char			*rolloverDay,
	       char			*rolloverPolicy,
	       XmimRolloverDataType	rolloverData));
    
    extern DllDecl XmimReturnCode XmimVaAddRelation
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimAddRelationAlias
        PROTO((XmimClientHandle 	handle,
	       char			*cat,
	       char			*origRel,
	       char			*rel));
    
    extern DllDecl XmimReturnCode XmimVaAddRelationAlias
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimModRelation
	PROTO((XmimClientHandle		handle,
	       char			*origRelationName,
	       char			*relationName,
	       char			*description,
	       char			*parentName,
	       XmimRelType		type,
	       char			*exchangeName,
	       char			*timeZone,
	       XmimTime			startTrade,
	       XmimTime			endTrade,
	       float			contractUnits,
	       XmimDate			expirationDate,
	       XmimDate			firstNoticeDay,
	       char			*rolloverDay,
	       char			*rolloverPolicy,
	       XmimRolloverDataType	rolloverData));

    extern DllDecl XmimReturnCode XmimVaModRelation
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDelRelation
	PROTO((XmimClientHandle		handle,
	       char			*relationName));
    
    extern DllDecl XmimReturnCode XmimVaDelRelation
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimAddColumn
	PROTO((XmimClientHandle		handle,
	       char			*columnName,
	       char			*description,
	       char			*parentName,
	       XmimColType		type));
    
    extern DllDecl XmimReturnCode XmimVaAddColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimModColumn
	PROTO((XmimClientHandle		handle,
	       char			*origColumnName,
	       char			*columnName,
	       char			*description,
	       char			*parentName,
	       XmimColType		type));
    
    extern DllDecl XmimReturnCode XmimVaModColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDelColumn
	PROTO((XmimClientHandle		handle,
	       char			*columnName));
    
    extern DllDecl XmimReturnCode XmimVaDelColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimAddMFColumn
	PROTO((XmimClientHandle		handle,
	       char			*name,
	       char			*description,
	       char			*parent,
	       int			numFields,
	       char			**fieldNames,
	       char			**fieldDescriptions));

    extern DllDecl XmimReturnCode XmimVaAddMFColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimModMFColumn
	PROTO((XmimClientHandle		handle,
	       char			*origName,
	       char			*name,
	       char			*description,
	       char			*parent));

    extern DllDecl XmimReturnCode XmimVaModMFColumn
	PROTO((XmimVaArgs, ...));
	       
    extern DllDecl XmimReturnCode XmimDelMFColumn
	PROTO((XmimClientHandle		handle,
	       char			*colName));

    extern DllDecl XmimReturnCode XmimVaDelMFColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimAddRelColumn
	PROTO((XmimClientHandle		handle,
	       char			*relationName,
	       char			*columnName,
	       XmimRelColType		type,
	       XmimRelColObjType	objType,
	       XmimRelColAggr		aggr,
	       XmimRelColDelta		delta,
	       float			constant));
    
    extern DllDecl XmimReturnCode XmimVaAddRelColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimModRelColumn
	PROTO((XmimClientHandle		handle,
	       char			*relationName,
	       char			*columnName,
	       XmimRelColType		type,
	       XmimRelColObjType	objType,
	       XmimRelColAggr		aggr,
	       XmimRelColDelta		delta,
	       float			constant));
    
    extern DllDecl XmimReturnCode XmimVaModRelColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDelRelColumn
	PROTO((XmimClientHandle		handle,
	       char			*relationName,
	       char			*columnName));
    
    extern DllDecl XmimReturnCode XmimVaDelRelColumn
	PROTO((XmimVaArgs, ...));
						     
    extern DllDecl XmimReturnCode XmimAddMFRelColumn
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*colName,
	       XmimRelColType		type,
	       int			numFields,
	       int			*objTypeList,
	       XmimRelColAggr		*aggrList,
	       XmimRelColDelta		*deltaList,
	       float			*constantList));

    extern DllDecl XmimReturnCode XmimVaAddMFRelColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimModMFRelColumn
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*colName,
	       XmimRelColType		type,
	       int			numFields,
	       int			*objTypeList,
	       XmimRelColAggr		*aggrList,
	       XmimRelColDelta		*deltaList,
	       float			*constantList));

    extern DllDecl XmimReturnCode XmimVaModMFRelColumn
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimDelMFRelColumn
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*colName));
    
    extern DllDecl XmimReturnCode XmimVaDelMFRelColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimClearEntitlements
        PROTO((XmimClientHandle		handle));

    extern DllDecl XmimReturnCode XmimVaClearEntitlements
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimClearEntitlementsUsers
        PROTO((XmimClientHandle		handle));
    
    extern DllDecl XmimReturnCode XmimReadEntitlements
	PROTO((XmimClientHandle		handle,
	       char			*fileName));

    extern DllDecl XmimReturnCode XmimVaReadEntitlements
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimWriteEntitlements
	PROTO((XmimClientHandle		handle,
	       char			*fileName));
    
    extern DllDecl XmimReturnCode XmimVaWriteEntitlements
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimWriteAllEntitlements
	PROTO((XmimClientHandle		handle,
	       char			*fileName));
    
    extern DllDecl XmimReturnCode XmimVaWriteAllEntitlements
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimReadFacts
	PROTO((XmimClientHandle		handle,
	       char			*fileName,
	       char			*relationName,
	       int			numOfColumns,
	       char			**columns,
	       int			numOfFields,
	       XmimFieldFormat		*fieldFormats,
	       XmimRecordFormat		recordFormat,
	       XmimMergeMode		mergeMode,
	       XmimTickMode		tickMode));

    extern DllDecl XmimReturnCode XmimVaReadFacts
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimWriteFacts
	PROTO((XmimClientHandle		handle,
	       char			*fileName,
	       int			numOfRelations,
	       char			**relations,
	       int			numOfColumns,
	       char			**columns,
	       int			numOfFields,
	       XmimFieldFormat		*fieldFormats,
	       int			num_units,
	       XmimUnits		units,
	       XmimDateTime		unitsFrom,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime));
    
    extern DllDecl XmimReturnCode XmimVaWriteFacts
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimExecQuery
	PROTO((XmimClientHandle		handle,
	       char			*query,
	       char			*outputFileName,
	       int			numOfSubstitutions,
	       XmimSubstitution		*substitutions,
	       XmimAppendMode		appendMode,
	       XmimBoolean		print,
	       XmimVerboseMode		verboseMode));

    extern DllDecl XmimReturnCode XmimExecQueryXML
	PROTO((XmimClientHandle		handle,
	       char			*query,
	       XmimAppendMode		appendMode,
	       XmimBoolean		print,
	       XmimVerboseMode		verboseMode,
	       int*			len,
	       char**			val));
    
    extern DllDecl XmimReturnCode XmimVaExecQuery
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGraphQuery
	PROTO((XmimClientHandle		handle,
	       char			*query,
	       char			*outputFileName,
	       int			numOfSubstitutions,
	       XmimSubstitution		*substitutions,
	       XmimAppendMode		appendMode,
	       XmimBoolean		print,
	       XmimVerboseMode		verboseMode));
    
    extern DllDecl XmimReturnCode XmimVaGraphQuery
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimSetCompression
	PROTO((XmimClientHandle		handle,
	       XmimBoolean		onOrOff,	/* True for on */
	       XmimDataType		type,
	       int			size));

    extern DllDecl XmimReturnCode XmimVaSetCompression
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimSetDefaultFactsFile
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*file,
	       XmimDataType		type));

    extern DllDecl XmimReturnCode XmimVaSetDefaultFactsFile
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimRegenRollover
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       XmimDataType		type));

    extern DllDecl XmimReturnCode XmimVaRegenRollover
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimSuppressErrorMessage
	PROTO((XmimClientHandle		handle,
	       XmimErrorType		err));

    extern DllDecl XmimReturnCode XmimVaSuppressErrorMessage
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimEnableErrorMessage
	PROTO((XmimClientHandle		handle,
	       XmimErrorType		err));

    extern DllDecl XmimReturnCode XmimVaEnableErrorMessage
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimAddCorrections
	PROTO((XmimClientHandle		handle,
	       XmimString		file));

    extern DllDecl XmimReturnCode XmimVaAddCorrections
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimGetExpirDates
	PROTO((XmimClientHandle		handle,
	       XmimString		root,
	       int			*numFuturesRels,
	       XmimContractsExpirDates	**expirDatesPerFutures));

    extern DllDecl XmimReturnCode XmimVaGetExpirDates
	PROTO((XmimVaArgs, ...));
    

/****************************************/
/*					*/
/*	Hierarchy Browsing Commands	*/
/*					*/
/****************************************/

    extern DllDecl XmimReturnCode XmimGetAllPathsForRelation
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			*num_rels,
	       char			***dbs,
	       char			***paths));

    extern DllDecl XmimReturnCode XmimGetAllPathsForColumn
	PROTO((XmimClientHandle		handle,
	       char			*colName,
	       int			*num_cols,
	       char			***dbs,
	       char			***paths));

    extern DllDecl XmimReturnCode XmimGetAllRelations
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       XmimSearchFilter		filter,
	       int			*numRels,
	       char			***relations));

    extern DllDecl XmimReturnCode XmimVaGetAllRelations
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetAllColumns
	PROTO((XmimClientHandle		handle,
	       char			*colName,
	       XmimSearchFilter		filter,
	       int			*numCols,
	       char			***columns));

    extern DllDecl XmimReturnCode XmimVaGetAllColumns
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetAllExchanges
	PROTO((XmimClientHandle		handle,
	       int			*numExchanges,
	       char			***exchanges));

    extern DllDecl XmimReturnCode XmimGetCustomSearches
	PROTO((XmimClientHandle	handle,
	       int		*num,
	       XmimString	** funcs));

    extern DllDecl XmimReturnCode XmimVaGetCustomSearches
	PROTO((XmimVaArgs, ...));
	       
    extern DllDecl XmimReturnCode XmimGetCustomSearchArgs
	PROTO((XmimClientHandle handle,
	       XmimString 	func, 
	       int		*num,
	       XmimString	**args,
	       int		**types));

    extern DllDecl XmimReturnCode XmimVaGetCustomSearchArgs
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimProcessCustomSearch
	PROTO((XmimClientHandle handle,
	       XmimString	func,
	       XmimString	root,
	       int		numArgs,
	       XmimString	*arguments,
	       int		*num,
	       XmimString	**result));

    extern DllDecl XmimReturnCode XmimVaProcessCustomSearch
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimVaGetAllExchanges
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetAllTimeZones
	PROTO((XmimClientHandle		handle,
	       int			*numTimeZones,
	       char			***timeZones));

    extern DllDecl XmimReturnCode XmimVaGetAllTimeZones
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimFindRelations
	PROTO((XmimClientHandle		handle,
	       char			*pattern,
	       XmimBoolean		byName,
	       XmimSearchFilter		filter,
	       XmimBoolean		caseSensitive,
	       int			*numRels,
	       char			***relations));
    extern DllDecl XmimReturnCode XmimFindRelationsXML
	PROTO((XmimClientHandle	handle,
	       char		*pattern,
	       XmimBoolean	byName,
	       XmimSearchFilter	filter,
	       XmimBoolean	caseSensitive,
	       char		**xmlString));

    extern DllDecl XmimReturnCode XmimVaFindRelations
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimFindColumns
	PROTO((XmimClientHandle		handle,
	       char			*pattern,
	       XmimBoolean		byName,
	       XmimSearchFilter		filter,
	       XmimBoolean		caseSensitive,
	       int			*numCols,
	       char			***columns));

    extern DllDecl XmimReturnCode XmimVaFindColumns
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRelation
	PROTO((XmimClientHandle		handle,
	       char			*relationName,
	       char			**description,
	       char			**parentName,
	       XmimRelType		*type,
	       char			**exchange,
	       char			**time_zone,
	       XmimTime			*startTrade,
	       XmimTime			*endTrade,
	       float			*contractUnits,
	       XmimDate			*expirationDate,
	       XmimDate			*firstNoticeDate,
	       char			**rolloverDay,
	       char			**rolloverPolicy,
	       XmimRolloverDataType	*rolloverData));

    extern DllDecl XmimReturnCode XmimVaGetRelation
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetColumn
	PROTO((XmimClientHandle		handle,
	       char			*colName,
	       char			**description,
	       char			**parentName,
	       XmimColType		*type));

    extern DllDecl XmimReturnCode XmimVaGetColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetMFColumn
	PROTO((XmimClientHandle		handle,
	       char			*colName,
	       char			**description,
	       char			**parent,
	       int			*numFields,
	       char			***fieldNames,
	       char			***fieldDescriptions));

    extern DllDecl XmimReturnCode XmimVaGetMFColumn
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimGetRelColumn
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*colName,
	       XmimRelColType		*type,
	       XmimRelColObjType	*objType,
	       XmimRelColAggr		*aggr,
	       XmimRelColDelta		*delta,
	       float			*constant));

    extern DllDecl XmimReturnCode XmimVaGetRelColumn
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetMFRelColumn
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*colName,
	       XmimRelColType		*type,
	       int			*numFields,
	       int			**objTypeList,
	       XmimRelColAggr		**aggrList,
	       XmimRelColDelta		**deltaList,
	       float			**constantList));

    extern DllDecl XmimReturnCode XmimVaGetMFRelColumn
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimGetRelChildren
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			*numChildren,
	       char			***children));

    extern DllDecl XmimReturnCode XmimVaGetRelChildren
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimRelHasChildren
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       XmimBoolean		*hasChildren));

    extern DllDecl XmimReturnCode XmimVaRelHasChildren
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRelParent
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			**parent));
    
    extern DllDecl XmimReturnCode XmimVaGetRelParent
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRelType
	PROTO((XmimClientHandle		handle,
	       char 			*relName,
	       XmimRelType		*type));
    
    extern DllDecl XmimReturnCode XmimVaGetRelType
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRelRoot
	PROTO((XmimClientHandle		handle,
	       char 			*relName,
	       char			**root));
    
    extern DllDecl XmimReturnCode XmimVaGetRelRoot
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimGetRelColumns
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			*numCols,
	       char			***columns));
    
    extern DllDecl XmimReturnCode XmimVaGetRelColumns
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimGetRelColumnsWithData
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			*numColumns,
	       char			***columns));
    
    extern DllDecl XmimReturnCode XmimVaGetRelColumnsWithData
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimGetColChildren
	PROTO((XmimClientHandle		handle,
	       char			*colName,
	       int			*numCols,
	       char			***children));
    
    extern DllDecl XmimReturnCode XmimVaGetColChildren
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimColHasChildren
	PROTO((XmimClientHandle		handle,
	       char			*colName,
	       XmimBoolean		*hasChildren));

    extern DllDecl XmimReturnCode XmimVaColHasChildren
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetColParent
	PROTO((XmimClientHandle		handle,
	       char			*colName,
	       char			**parent));
    
    extern DllDecl XmimReturnCode XmimVaGetColParent
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetColType
	PROTO((XmimClientHandle		handle,
	       char 			*colName,
	       XmimColType 		*type));
    
    extern DllDecl XmimReturnCode XmimVaGetColType
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetColRoot
	PROTO((XmimClientHandle		handle,
	       char 			*columnName,
	       char			**root));
    
    extern DllDecl XmimReturnCode XmimVaGetColRoot
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetTargetForRelationAlias
        PROTO((XmimClientHandle handle,
             char            *rel,
             char            **target));

    extern DllDecl XmimReturnCode XmimGetRelPath
	PROTO((XmimClientHandle		handle,
	       char			*rel,
	       char			**path));

    extern DllDecl XmimReturnCode XmimVaGetRelPath
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetColPath
	PROTO((XmimClientHandle		handle,
	       char			*col,
	       char			**path));

    extern DllDecl XmimReturnCode XmimVaGetColPath
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetXMLRelationInfo
	PROTO((XmimClientHandle		handle,
	       char			*rel,
	       char			**xml_string));
    
    extern DllDecl XmimReturnCode XmimGetXMLRelationInfoNoChildren
	PROTO((XmimClientHandle		handle,
	       char			*rel,
	       char			**xml_string));

    extern DllDecl XmimReturnCode XmimHolidayScheduleP
	PROTO((XmimClientHandle 	handle,
	       int*		holiday_p));


/********************************/
/*				*/    
/*	Dynamic Schema		*/
/*				*/    
/********************************/

    extern DllDecl XmimReturnCode XmimCreateTable
	PROTO((XmimClientHandle		handle,
	       XmimString		tableName,
	       int			numFields,
	       XmimTableFieldDesc	*fields,
	       int			keyFieldIndex,
	       XmimBoolean		singleValuedP,
	       XmimTableHandle		*tableHandle));

    extern DllDecl XmimReturnCode XmimVaCreateTable
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimOpenTable
	PROTO((XmimClientHandle		handle,
	       XmimString		tableName,
	       XmimBoolean		readOnlyP,
	       XmimTableHandle		*tableHandle));
	
    extern DllDecl XmimReturnCode XmimVaOpenTable
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimCloseTable
	PROTO((XmimClientHandle		handle,
	       XmimTableHandle		tableHandle));
	
    extern DllDecl XmimReturnCode XmimVaCloseTable
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimRenameTable
	PROTO((XmimClientHandle		handle,
	       XmimString		origName,
	       XmimString		newName));
	
    extern DllDecl XmimReturnCode XmimVaRenameTable
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDeleteTable
	PROTO((XmimClientHandle		handle,
	       XmimString		tableName));
	
    extern DllDecl XmimReturnCode XmimVaDeleteTable
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetTable
	PROTO((XmimClientHandle		handle,
	       XmimTableHandle		tableHandle,
	       int			*numFields,
	       XmimTableFieldDesc	**fields,
	       int			*keyFieldIndex,
	       XmimBoolean		*singleValuedP));
	
    extern DllDecl XmimReturnCode XmimVaGetTable
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetTableNames
	PROTO((XmimClientHandle		handle,
	       int			*numTables,
	       XmimString		**tables));
	
    extern DllDecl XmimReturnCode XmimVaGetTableNames
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimAddTuple
	PROTO((XmimClientHandle		handle,
	       XmimTableHandle		tableHandle,
	       XmimTableFieldVal	*val));

    extern DllDecl XmimReturnCode XmimVaAddTuple
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimCreateTableCursor
	PROTO((XmimClientHandle		handle,
	       XmimTableHandle		tableHandle,
	       XmimTableFieldVal	*val,
	       int			inheritFieldIndex,
	       unsigned int		cacheSize,	/* num tuples cached */
	       XmimCursorHandle		*cursor));
	
    extern DllDecl XmimReturnCode XmimVaCreateTableCursor
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetTuple
	PROTO((XmimClientHandle		handle,
	       XmimCursorHandle		cursor,
	       XmimTableFieldVal	*val));
	
    extern DllDecl XmimReturnCode XmimVaGetTuple
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimIsCursorExhausted
	PROTO((XmimClientHandle		handle,
	       XmimCursorHandle		cursor,
	       XmimBoolean		*done));
	
    extern DllDecl XmimReturnCode XmimVaIsCursorExhausted
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDeleteCursor
	PROTO((XmimClientHandle		handle,
	       XmimCursorHandle		cursor));
	
    extern DllDecl XmimReturnCode XmimVaDeleteCursor
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimDeleteTuple
	PROTO((XmimClientHandle		handle,
	       XmimTableHandle		tableHandle,
	       XmimTableFieldVal	*val));
	
    extern DllDecl XmimReturnCode XmimVaDeleteTuple
	PROTO((XmimVaArgs, ...));

    
/********************************/
/*				*/    
/*	Get and Put Data	*/
/*				*/    
/********************************/

    extern DllDecl XmimReturnCode XmimGetDataRange
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			numOfCols,
	       char			**colNames,
	       XmimUnits		units,
	       XmimDate			*fromDate,
	       XmimDate			*toDate ));

    extern DllDecl XmimReturnCode XmimVaGetDataRange
	PROTO((XmimVaArgs, ...));

#define  XmimGetTickRange(h, r, n, c, f, t) \
    XmimGetDataRange(h, r, n, c, XMIM_MINUTES, f, t)

#define XmimGetDailyRange(h, r, n, c, f, t) \
    XmimGetDataRange(h, r, n, c, XMIM_DAYS, f, t)	

    extern DllDecl XmimReturnCode XmimGetDataRangeOption
	PROTO((XmimClientHandle		handle,
	       char			*optionName,
	       int			numColumns,
	       char			**columns,
	       XmimUnits		units,
	       XmimDate			expDate,
	       XmimOptionType		optType,
	       float			strike,
	       XmimDate			*fromDate,
	       XmimDate			*toDate));

    extern DllDecl XmimReturnCode XmimVaGetDataRangeOption
	PROTO((XmimVaArgs, ...));

#define XmimGetDailyRangeOption(h, o, n, c, e, t, s, f, to) \
    XmimGetDataRangeOption(h, o, n, c, XMIM_DAYS, e, t, s, f, to)

#define XmimGetTickRangeOption(h, o, n, c, e, t, s, f, to) \
    XmimGetDataRangeOption(h, o, n, c, XMIM_MINUTES, e, t, s, f, to)

    extern DllDecl XmimReturnCode XmimGetTradingTime
	PROTO((XmimClientHandle		handle,
	       XmimString		relName,
	       int			numColumns,
	       XmimString		*colNames,
	       XmimUnits		units,
	       XmimDate			tradingDate,
	       XmimTime			*startTime,
	       XmimTime			*endTime ));

    extern DllDecl XmimReturnCode XmimVaGetTradingTime
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimGetRecords
	PROTO((XmimClientHandle		handle,
	       int			numOfRels,
	       char			**relNames,
	       int			numOfCols,
	       char			**colNames,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       int			numUnits,
	       XmimUnits		units,
	       XmimFillOption		holidayFillOption,
	       XmimFillOption		missDataFillOption,
	       XmimSkipAllNaN		skipAllNaNRecord,
	       int			limit,
	       XmimLimitMode		limitMode,
	       int			*numRecords,
	       XmimDateTime		**dateTimes,
	       float			**values));
    
    extern DllDecl XmimReturnCode XmimVaGetRecords
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRecordsAllColumns
	PROTO((XmimClientHandle		handle,
	       int			numOfRels,
	       char			**relNames,
	       XmimDate			fromDate,
	       XmimDate 		toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       int			numUnits,
	       XmimUnits 		units,
	       XmimFillOption		holidayFillOption,
	       XmimFillOption		missDataFillOption,
	       XmimSkipAllNaN		skipAllNaNRecord,
	       int			limit,
	       XmimLimitMode 		limitMode,
	       int			*numOfCols,
	       char			***colNames,
	       int			*numRecords,
	       XmimDateTime 		**dateTimes,
	       float			**values));
    
    extern DllDecl XmimReturnCode XmimVaGetRecordsAllColumns
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRecordsRollover
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			numOfCols,
	       char			**colNames,
	       XmimDate			fromDate,
	       XmimDate 		toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       int			numUnits,
	       XmimUnits		units,
	       XmimFillOption		holidayFillOption,
	       XmimFillOption		missDataFillOption,	       
	       XmimSkipAllNaN		skipAllNaNRecord,
	       int			limit,
	       XmimLimitMode 		limitMode,
	       char			*rolloverDay,
	       char			*rolloverPolicy,
	       XmimAdjustedStudy	study,
	       int			numFromUnits,
	       XmimUnits		fromUnits,
	       int			numToUnits,
	       XmimUnits		toUnits,
	       int			*numRecords,
	       XmimDateTime		**dateTimes,
	       float			**values));

    extern DllDecl XmimReturnCode XmimVaGetRecordsRollover
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRecordsRolloverAllColumns
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       XmimDate			fromDate,
	       XmimDate 		toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       int			numUnits,
	       XmimUnits		units,
	       XmimFillOption		holidayFillOption,
	       XmimFillOption		missDataFillOption,
	       XmimSkipAllNaN		skipAllNaNRecord,
	       int			limit,
	       XmimLimitMode 		limitMode,
	       char			*rolloverDay,
	       char			*rolloverPolicy,
	       XmimAdjustedStudy	study,
	       int			numFromUnits,
	       XmimUnits		fromUnits,
	       int			numToUnits,
	       XmimUnits		toUnits,
	       int			*numOfCols,
	       char			***colNames,
	       int			*numRecords,
	       XmimDateTime		**dateTimes,
	       float			**values));

    extern DllDecl XmimReturnCode XmimGetCorrections
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*colName,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       XmimCorrectionType	units,
	       int			limit,
	       int			*numRecords,
	       XmimDateTime		**dateTimes1,
	       XmimDateTime		**dateTimes2,
	       double			**values));


    extern DllDecl XmimReturnCode XmimDeleteCorrections
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       char			*colName,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       XmimCorrectionType	units,
	       int			*numRecords));

    extern DllDecl XmimReturnCode XmimVaGetRecordsRolloverAllColumns
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRolloverDates
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			numOfCols,
	       char			**colNames,
	       int			numUnits,
	       XmimUnits		units,
	       char			*rolloverDay,
	       char			*rolloverPolicy,
	       int			*numContracts,
	       int			*numPeriods,
	       XmimDate			**expirationDates,
	       XmimDate			**contracts));

    extern DllDecl XmimReturnCode XmimVaGetRolloverDates
	PROTO((XmimVaArgs, ...));
	       	       
    extern DllDecl XmimReturnCode XmimGetRolloverDatesAllColumns
	PROTO((XmimClientHandle		handle,
	       char			*relName,
	       int			numUnits,
	       XmimUnits		units,
	       char			*rolloverDay,
	       char			*rolloverPolicy,
	       int			*numOfCols,
	       char			***colNames,
	       int			*numContracts,
	       int			*numPeriods,
	       XmimDate			**expirationDates,
	       XmimDate			**contracts));

    extern DllDecl XmimReturnCode  XmimVaGetRolloverDatesAllColumns
	PROTO((XmimVaArgs, ...));
	       	       
    extern DllDecl XmimReturnCode  XmimGetRecordsOption
	PROTO((XmimClientHandle		handle,
	       int			numOptions,
	       char			**optionNames,
	       int			numColumns,
	       char			**columns,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       XmimDate			fromExpDate,
	       XmimDate			toExpDate,
	       float			fromStrike,
	       float			toStrike,
	       XmimOptionType		optionType,
	       int			numUnits,
	       XmimUnits		units,
	       XmimFillOption		holidayFillOption,
	       XmimFillOption		missDataFillOption,
	       XmimSkipAllNaN		skipAllNaNRecord,
	       int			limit,
	       XmimLimitMode		limitMode,
	       int			*numRecords,
	       XmimDateTime		**dateTimes,
	       XmimDate			**expDates,
	       XmimOptionType		**optTypes,
	       float			**strikes,
	       float			**values));

    extern DllDecl XmimReturnCode XmimVaGetRecordsOption
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRecordsOptionAllColumns
	PROTO((XmimClientHandle		handle,
	       int			numOptions,
	       char			**optionNames,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       XmimDate			fromExpDate,
	       XmimDate			toExpDate,
	       float			fromStrike,
	       float			toStrike,
	       XmimOptionType		optionType,
	       int			numUnits,
	       XmimUnits		units,
	       XmimFillOption		holidayFillOption,
	       XmimFillOption		missDataFillOption,
	       XmimSkipAllNaN		skipAllNaNRecord,
	       int			limit,
	       XmimLimitMode		limitMode,
	       int			*numColumns,
	       char			***columns,
	       int			*numRecords,
	       XmimDateTime		**dateTimes,
	       XmimDate			**expDates,
	       XmimOptionType		**optTypes,
	       float			**strikes,
	       float			**values));

    extern DllDecl XmimReturnCode XmimVaGetRecordsOptionAllColumns
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRecordsOptionRelative
	PROTO((XmimClientHandle		handle,
	       int			numOptions,
	       char			**optionNames,
	       int			numColumns,
	       char			**columns,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       int			numUnitsToExpir,
	       XmimUnits		unitsToExpir,
	       float			strikeWeight,
	       XmimOptionType		optionType,
	       XmimBoolean		selectOnce,
	       int			numUnits,
	       XmimUnits		units,
	       XmimFillOption		holidayFillOption,
	       XmimFillOption		missDataFillOption,
	       XmimSkipAllNaN		skipAllNaNRecord,
	       int			limit,
	       XmimLimitMode		limitMode,
	       int			*numRecords,
	       XmimDateTime		**dateTimes,
	       XmimDate			**expDates,
	       XmimOptionType		**optTypes,
	       float			**strikes,
	       float			**values));

    extern DllDecl XmimReturnCode XmimVaGetRecordsOptionRelative
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetRecordsOptionRelativeAllColumns
	PROTO((XmimClientHandle		handle,
	       int			numOptions,
	       char			**optionNames,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,
	       XmimTime			toTime,
	       int			numUnitsToExpir,
	       XmimUnits		unitsToExpir,
	       float			strikeWeight,
	       XmimOptionType		optionType,
	       XmimBoolean		selectOnce,
	       int			numUnits,
	       XmimUnits		units,
	       XmimFillOption		holidayFillOption,
	       XmimFillOption		missDataFillOption,
	       XmimSkipAllNaN		skipAllNaNRecord,
	       int			limit,
	       XmimLimitMode		limitMode,
	       int			*numColumns,
	       char			***columns,
	       int			*numRecords,
	       XmimDateTime		**dateTimes,
	       XmimDate			**expDates,
	       XmimOptionType		**optTypes,
	       float			**strikes,
	       float			**values));

    extern DllDecl XmimReturnCode XmimVaGetRecordsOptionRelativeAllColumns
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimSelectRecords
	PROTO((XmimClientHandle		handle,
	       int			numConds,
	       char			**whenCond,
	       int			numOfShow,
	       char			**showAttrs,
	       int			beforeRepeat,
	       int			afterRepeat,
	       int			numExecUnits,
	       XmimUnits		execUnits,
	       int			*numRecords,
	       XmimBoolean		**whenSuccess,
	       XmimDateTime		**dateTimes,
	       float			**values));
    
    extern DllDecl XmimReturnCode XmimVaSelectRecords
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimPutRecords
	PROTO((XmimClientHandle		handle,
	       int			numOfRels,
	       char			**relNames,
	       int			numOfCols,
	       char			**colNames,
	       XmimUnits		units,
	       int			numOfRec,
	       XmimDateTime		*dateTimes,
	       float			*values));

    extern DllDecl XmimReturnCode XmimVaPutRecords
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimPutRecordsOption
	PROTO((XmimClientHandle		handle,
	       int			numOptions,
	       char			**optionNames,
	       int			numColumns,
	       char			**columns,
	       XmimUnits		units,
	       int			numRecords,
	       XmimDateTime		*dateTimes,
	       XmimDate			*expDates,
	       XmimOptionType		*optTypes,
	       float			*strikes,
	       float			*values));

    extern DllDecl XmimReturnCode XmimVaPutRecordsOption
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimReplaceRecords
	PROTO((XmimClientHandle		handle,
	       int			numRels,
	       char		       	**relNames,
	       int			numOfCols,
	       char			**colNames,
	       XmimUnits		units,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,         
	       XmimTime			toTime,           
	       int			numRec,
	       XmimDateTime		*dateTimes,
	       float			*values));

    extern DllDecl XmimReturnCode XmimVaReplaceRecords
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimReplaceRecordsOption
	PROTO((XmimClientHandle		handle,
	       int			numRels,
	       char		       	**relNames,
	       int			numOfCols,
	       char			**colNames,
	       XmimUnits		units,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimTime			fromTime,         
	       XmimTime			toTime,           
	       int			numRec,
	       XmimDateTime		*dateTimes,
	       XmimDate			*expDates,
	       XmimOptionType		*optTypes,
	       float			*strikes,
	       float			*values));

    extern DllDecl XmimReturnCode XmimVaReplaceRecordsOption
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimLockCurrentTick
	PROTO((XmimClientHandle		handle));
    
    extern DllDecl XmimReturnCode XmimVaLockCurrentTick
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimUnlockCurrentTick
	PROTO((XmimClientHandle		handle));
    
    extern DllDecl XmimReturnCode XmimVaUnlockCurrentTick
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimUpdateCurrentTick
	PROTO((XmimClientHandle		handle,
	       XmimDate			date,
	       int			numOfRels,
	       char			**relNames,
	       int			numOfCols,
	       char			**colNames,
	       XmimBoolean		realTick,
	       int			numOfRec,
	       XmimTime			*times,
	       float			*values));

    extern DllDecl XmimReturnCode XmimUpdateCurrentTickFast
	PROTO((XmimClientHandle		handle,
	       XmimDate			date,
	       int			numOfRels,
	       char			**relNames,
	       int			numOfCols,
	       char			**colNames,
	       XmimBoolean		realTick,
	       int			recLen,
	       int			numOfRec,
	       XmimTime			*times,
	       float			*values));
    
    extern DllDecl XmimReturnCode XmimVaUpdateCurrentTick
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimUpdateCurrentTickBar
	PROTO((XmimClientHandle 	handle,
	       char			*rel,
	       XmimDate			date,
	       XmimTime			time,
	       float			val));
    
    extern DllDecl XmimReturnCode XmimVaUpdateCurrentTickBar
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimFoldCurrentTick
	PROTO((XmimClientHandle		handle,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimBoolean		realTick));
    
    extern DllDecl XmimReturnCode XmimVaFoldCurrentTick
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimFoldCurrentTickSelective
	PROTO((XmimClientHandle		handle,
	       XmimString		relation,
	       int			numOfCols,
	       char			**colNames,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimBoolean		realTick,
	       XmimBoolean		intraday,
	       XmimBoolean		daily));
    
    extern DllDecl XmimReturnCode XmimVaFoldCurrentTickSelective
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimClearCurrentTick
	PROTO((XmimClientHandle		handle,
	       XmimDate			fromDate,
	       XmimDate			toDate,
	       XmimBoolean		realTick));
    
    extern DllDecl XmimReturnCode XmimVaClearCurrentTick
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimGetCurrentTickDataDates
	PROTO((XmimClientHandle		handle,
	       int			*num_dates,
	       XmimDate			**dates));

    extern DllDecl XmimReturnCode XmimVaGetCurrentTickDataDates
	PROTO((XmimVaArgs, ...));

/****************************************/
/*					*/
/*	Query Execution			*/
/*					*/
/****************************************/

    extern DllDecl XmimReturnCode XmimQueryExecute
        PROTO((XmimClientHandle			handle,
	       XmimString			query,
	       XmimBoolean			hasClntMacroP,
	       int				multiple,
	       XmimUnits			units,
	       XmimDateTime			endingDateTime,
	       XmimFillOption			holidayFill,
	       XmimFillOption			missDataFill,
	       XmimCurrentTickUsage		ctdUsage,
	       XmimQueryResultHandle		*rsltSetHandle));

    extern DllDecl XmimReturnCode XmimVaQueryExecute
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryPrintResult
        PROTO((XmimClientHandle			handle,
	       XmimQueryResultHandle		rsltHandle,
	       XmimString			output));

    extern DllDecl XmimReturnCode XmimVaQueryPrintResult
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryDeleteResult
        PROTO((XmimClientHandle			handle,
	       XmimQueryResultHandle		rsltHandle));
    
    extern DllDecl XmimReturnCode XmimVaQueryDeleteResult
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetNumReports
        PROTO((XmimClientHandle			handle,
	       XmimQueryResultHandle		rsltHandle,
	       int				*numReports));

    extern DllDecl XmimReturnCode XmimVaQueryGetNumReports
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetReport
        PROTO((XmimClientHandle			handle,
	       XmimQueryResultHandle		rsltHandle,
	       int				i,
	       XmimQueryReportHandle 		*reportHandle));

    extern DllDecl XmimReturnCode XmimVaQueryGetReport
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetReportTitle
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportHandle		reportHandle,
	       XmimString			*title));

    extern DllDecl XmimReturnCode XmimVaQueryGetReportTitle
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetFinalReport
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportHandle		reportHandle,
	       XmimQueryReportHandle		*finalReportHandle));

    extern DllDecl XmimReturnCode XmimVaQueryGetFinalReport
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetNumReportBlocks
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportHandle		reportHandle,
	       int				*numReportBlocks));

    extern DllDecl XmimReturnCode XmimVaQueryGetNumReportBlocks
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetReportBlock
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportHandle		reportHandle,
	       int				i,
	       XmimQueryReportBlockHandle 	*reportBlockHandle));
    
    extern DllDecl XmimReturnCode XmimVaQueryGetReportBlock
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetNumAttributes
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				*numAttrs));

    extern DllDecl XmimReturnCode XmimVaQueryGetNumAttributes
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetAttributeHeading
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				i,
	       XmimString			*attrHeading));

    extern DllDecl XmimReturnCode XmimVaQueryGetAttributeHeading
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetNumColumns
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				*numCols));

    extern DllDecl XmimReturnCode XmimVaQueryGetNumColumns
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetNumColumnsForAttr
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				attrIndex,
	       int				*numCols));

    extern DllDecl XmimReturnCode XmimVaQueryGetNumColumnsForAttr
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetColumnHeading
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				i,
	       XmimString			*columnHeading));
    
    extern DllDecl XmimReturnCode XmimVaQueryGetColumnHeading
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetNumRows
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				*numRows));

    extern DllDecl XmimReturnCode XmimVaQueryGetNumRows
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetRowDate
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				rowIndex,
	       XmimDate				*date));
    
    extern DllDecl XmimReturnCode XmimVaQueryGetRowDate
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryHasTimes
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       XmimBoolean			*hasTimes));

    extern DllDecl XmimReturnCode XmimVaQueryHasTimes
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetRowTime
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				rowIndex,
	       XmimTime				*time));
    
    extern DllDecl XmimReturnCode XmimVaQueryGetRowTime
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetValue
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				rowIndex,
	       int				colIndex,
	       double				*value));

    extern DllDecl XmimReturnCode XmimVaQueryGetValue
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryHasRepeat
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       XmimBoolean			*hasRepeat));

    extern DllDecl XmimReturnCode XmimVaQueryHasRepeat
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetRepeatDate
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				rowIndex,
	       int				colIndex,
	       XmimDate				*date));
    
    extern DllDecl XmimReturnCode XmimVaQueryGetRepeatDate
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetRepeatTime
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int				rowIndex,
	       int				colIndex,
	       XmimTime				*time));
    
    extern DllDecl XmimReturnCode XmimVaQueryGetRepeatTime
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetNumSummaryRecords
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int 				*numSummaryRecords));

    extern DllDecl XmimReturnCode XmimVaQueryGetNumSummaryRecords
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetSummaryRecord
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportBlockHandle 	reportBlockHandle,
	       int 				summaryRecordIndex,
	       XmimQuerySummaryRecord		*summaryRecord));
    
    extern DllDecl XmimReturnCode XmimVaQueryGetSummaryRecord
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetPLReport
        PROTO((XmimClientHandle			handle,
	       XmimQueryReportHandle		reportHandle,
	       XmimQueryPLReportHandle		*plReportHandle));

    extern DllDecl XmimReturnCode XmimVaQueryGetPLReport
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetPLNumTradeRecords
        PROTO((XmimClientHandle			handle,
	       XmimQueryPLReportHandle		plReportHandle,
	       int				*numOfTradeRecords));

    extern DllDecl XmimReturnCode XmimQueryGetPLNumTrades
        PROTO((XmimClientHandle			handle,
	       XmimQueryPLReportHandle		plReportHandle,
	       int				*numOfTrades));

    extern DllDecl XmimReturnCode XmimVaQueryGetPLNumTrades
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetPLTradeRecord
        PROTO((XmimClientHandle			handle,
	       XmimQueryPLReportHandle		reportHandle,
	       int				tradeIndex,
	       XmimQueryPLTradeRecord		*tradeRecord));

    extern DllDecl XmimReturnCode XmimVaQueryGetPLTradeRecord
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetPLStatRecord
        PROTO((XmimClientHandle			handle,
	       XmimQueryPLReportHandle		plReportHandle,
	       XmimQueryPLStatItem		item,
	       XmimQueryPLStatRecord		*statRecord));

    extern DllDecl XmimReturnCode XmimVaQueryGetPLStatRecord
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetPLNumEquityRecords
        PROTO((XmimClientHandle			handle,
	       XmimQueryPLReportHandle		plReportHandle,
	       int				*numOfEquityRecords));

    extern DllDecl XmimReturnCode XmimVaQueryGetPLNumEquityRecords
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimQueryGetPLEquityRecord
        PROTO((XmimClientHandle			handle,
	       XmimQueryPLReportHandle		plReportHandle,
	       int				equityRecordIndex,
	       XmimQueryPLEquityRecord   	*equityRecord));

    extern DllDecl XmimReturnCode XmimVaQueryGetPLEquityRecord
	PROTO((XmimVaArgs, ...));
    
	
/****************************************/
/*					*/
/*	Entitlements API		*/
/*					*/
/****************************************/
    
    extern DllDecl XmimReturnCode XmimEntitlementCreateGroup
    PROTO((XmimClientHandle		handle,
	   XmimString			group_name));
    
    extern DllDecl XmimReturnCode XmimVaEntitlementCreateGroup
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementGetPermissionsForGroup
    PROTO((XmimClientHandle		handle,
	   XmimString			group_name,
	   int				*no_of_recs,
	   XmimPermissionRecord		**perm_records));
    
    extern DllDecl XmimReturnCode XmimVaEntitlementGetPermissionsForGroup
    PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimEntitlementGetGroupForUser
    PROTO((XmimClientHandle		handle,
	   XmimString			user_name,
	   XmimString			*group_name));

    extern DllDecl XmimReturnCode XmimVaEntitlementGetGroupForUser
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementGetGroupsForUser
    PROTO((XmimClientHandle		handle,
	   XmimString			user_name,
	   int				*no_of_groups,
	   XmimString			**group_names));

    extern DllDecl XmimReturnCode XmimEntitlementGetUsersForGroup
    PROTO((XmimClientHandle		handle,
	   XmimString			group_name,
	   int				*no_of_users,
	   XmimString			**user_names));

    extern DllDecl XmimReturnCode XmimVaEntitlementGetUsersForGroup
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementGetAllGroups
    PROTO((XmimClientHandle		handle,
	   int				*no_of_groups,
	   XmimString			**group_names));

    extern DllDecl XmimReturnCode XmimVaEntitlementGetAllGroups
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementGetAllUsers
    PROTO((XmimClientHandle		handle,
	   int				*no_of_users,
	   XmimString			**user_names));

    extern DllDecl XmimReturnCode XmimVaEntitlementGetAllUsers
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementAddPermissionForGroup
    PROTO((XmimClientHandle		handle,
	   XmimString			group_name,
	   XmimPermissionRecord		perm_record));

    extern DllDecl XmimReturnCode XmimVaEntitlementAddPermissionForGroup
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementDeletePermissionForGroup
    PROTO((XmimClientHandle		handle,
	   XmimString			group_name,
	   XmimPermissionRecord		perm_record));

    extern DllDecl XmimReturnCode XmimVaEntitlementDeletePermissionForGroup
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementAddUserForGroup
    PROTO((XmimClientHandle		handle,
	   XmimString			user_name,
	   XmimString			group_name));

    extern DllDecl XmimReturnCode XmimVaEntitlementAddUserForGroup
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementDeleteUserForGroup
    PROTO((XmimClientHandle		handle,
	   XmimString			user_name,
	   XmimString			group_name));

    extern DllDecl XmimReturnCode XmimVaEntitlementDeleteUserForGroup
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementDeleteGroup
    PROTO((XmimClientHandle		handle,
	   XmimString			group_name));

    extern DllDecl XmimReturnCode XmimVaEntitlementDeleteGroup
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementIsRootPassword
    PROTO((XmimClientHandle		handle,
	   XmimString			password,
	   XmimBoolean			*root_p));

    extern DllDecl XmimReturnCode XmimVaEntitlementIsRootPassword
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementSetRootPassword
    PROTO((XmimClientHandle		handle,
	   XmimString			old_passwd,
	   XmimString			new_passwd));

    extern DllDecl XmimReturnCode XmimVaEntitlementSetRootPassword
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementLock
    PROTO((XmimClientHandle		handle,
	   XmimString			password));

    extern DllDecl XmimReturnCode XmimVaEntitlementLock
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementUnlock
    PROTO((XmimClientHandle		handle));

    extern DllDecl XmimReturnCode XmimVaEntitlementUnlock
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementLogin
    PROTO((XmimClientHandle		handle,
	   XmimString			password));

    extern DllDecl XmimReturnCode XmimVaEntitlementLogin
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimEntitlementLogout
    PROTO((XmimClientHandle		handle));

    extern DllDecl XmimReturnCode XmimVaEntitlementLogout
    PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimUserEntitledP
    PROTO((XmimClientHandle handle,    
	   XmimString user_name,
	   XmimString relation_name,
	   XmimString column_name,
	   XmimBoolean pass_fail,
	   int *match_p));


/****************************************/
/*					*/
/*	Set and Get Global Options	*/
/*					*/
/****************************************/

    extern DllDecl XmimBoolean XmimValidClientHandle
	PROTO((XmimClientHandle		handle));
    
    extern DllDecl XmimReturnCode XmimSetDefaultHandle
	PROTO((XmimClientHandle		handle));
    
    extern DllDecl XmimReturnCode XmimSetNaN
	PROTO((XmimClientHandle		handle,
	       float 			NaN,
	       XmimNaNType		type));

    extern DllDecl XmimReturnCode XmimGetNaN
	PROTO((XmimClientHandle		handle,
	       XmimNaNType		type,
	       float			*NaN));

extern DllDecl float	DEFAULT_NAN_VEC[];
extern DllDecl float	DEFAULT_HOLIDAY_NAN_VEC[];

#ifdef WIN32
#define XmimIsDefaultNaN(x) _isnan((x))

    extern DllDecl XmimBoolean XmimTestNaNType
	PROTO((XmimClientHandle		handle,
	       float			x,
	       XmimNaNType		type));

    extern DllDecl XmimBoolean XmimTestNaN
	PROTO((XmimClientHandle		handle,
	       float			x));
#else
#define	XmimIsDefaultNaN(x) (((*(int*)(&(x)) & 0x7f000000) == 0x7f000000) && \
			     (*(int*)(&(x)) == 0xffffffff || \
			      *(int*)(&(x)) == 0xfff00000 || \
			      *(int*)(&(x)) == 0xffff0000 || \
			      *(int*)(&(x)) == 0xffffe000 || \
			      *(int*)(&(x)) == 0xfff80000 || \
			      *(int*)(&(x)) == 0xff800000 || \
			      *(int*)(&(x)) == 0xffc00000 || \
			      *(int*)(&(x)) == 0x7fc00000 || \
			      *(int*)(&(x)) == 0x7f800000 || \
			      *(int*)(&(x)) == 0x7ff80000 || \
			      *(int*)(&(x)) == 0x7ff00000 || \
			      *(int*)(&(x)) == 0x7fff0000 || \
			      *(int*)(&(x)) == 0x7fffe000 || \
			      *(int*)(&(x)) == 0x7fffffff))

#define XmimTestNaNType(handle,value,type)				\
    ( ( (type) == XMIM_NAN_TYPE_MISS_DATA ) ?				\
      ( XmimIsDefaultNaN (DEFAULT_NAN_VEC[handle]) ?			\
	XmimIsDefaultNaN (value) :					\
	( (value) == DEFAULT_NAN_VEC[handle] ) ) :			\
      ( ( (type) == XMIM_NAN_TYPE_HOLIDAY ) ?				\
	( XmimIsDefaultNaN (DEFAULT_HOLIDAY_NAN_VEC[handle]) ?		\
	  XmimIsDefaultNaN (value) :					\
	  ( (value) == DEFAULT_HOLIDAY_NAN_VEC[handle] ) ) :		\
	( (value) == DEFAULT_NAN_VEC[handle] || 			\
	  (value) == DEFAULT_HOLIDAY_NAN_VEC[handle] )			\
	)								\
      )
	
#define XmimTestNaN(handle,value)					\
    ( ( ( XmimIsDefaultNaN (DEFAULT_NAN_VEC[handle]) ||			\
	  XmimIsDefaultNaN (DEFAULT_HOLIDAY_NAN_VEC[handle]) ) ?	\
	XmimIsDefaultNaN (value) :					\
	0 ) ||								\
      ( (value) == DEFAULT_NAN_VEC[handle] || 				\
	(value) == DEFAULT_HOLIDAY_NAN_VEC[handle] ) )
#endif      

    extern DllDecl XmimReturnCode XmimGetUpdateInfo
        PROTO((XmimClientHandle handle,
	       int*		num_recs,
	       char***		records));
    
    extern DllDecl XmimReturnCode XmimGetUpdateInfoBuffered
        PROTO((XmimClientHandle handle,
	       int*		num_recs,
	       char***		records,
	       int*		more));
    
    extern DllDecl XmimReturnCode XmimSetUnits
	PROTO((XmimClientHandle		handle,
	       int			numUnits,
	       XmimUnits 		units));
    
    extern DllDecl XmimReturnCode XmimGetUnits
	PROTO((XmimClientHandle		handle,
	       int			*numUnits,
	       XmimUnits 		*units));

    extern DllDecl XmimReturnCode XmimSetUnitsFrom
	PROTO((XmimClientHandle		handle,
	       XmimDateTime		unitsFrom));
    
    extern DllDecl XmimReturnCode XmimGetUnitsFrom
	PROTO((XmimClientHandle		handle,
	       XmimDateTime		*unitsFrom));

    extern DllDecl XmimReturnCode XmimSetLimit
	PROTO((XmimClientHandle		handle,
	       int			limit,
	       XmimLimitMode		mode));

    extern DllDecl XmimReturnCode XmimGetLimit
	PROTO((XmimClientHandle		handle,
	       int			*limit,
	       XmimLimitMode		*mode));

    extern DllDecl XmimReturnCode XmimSetFillOption
	PROTO((XmimClientHandle		handle,
	       XmimFillOption		missDataFill,
	       XmimFillOption		holidayFill,
	       XmimSkipAllNaN		skipAllNaNRecords));
    
    extern DllDecl XmimReturnCode XmimGetFillOption
	PROTO((XmimClientHandle		handle,
	       XmimFillOption		*missDataFill,
	       XmimFillOption		*holidayFill,
	       XmimSkipAllNaN		*skipAllNaNRecords));
    
    extern DllDecl XmimReturnCode XmimSetPrintSeconds
	PROTO((XmimClientHandle		handle,
	       XmimBoolean		printSeconds));
    
    extern DllDecl XmimReturnCode XmimGetPrintSeconds
	PROTO((XmimClientHandle		handle,
	       XmimBoolean		*printSeconds));

    extern DllDecl XmimReturnCode XmimSetDateFormat
	PROTO((XmimClientHandle		handle,
	       XmimDateFormat 		format));
    
    extern DllDecl XmimReturnCode XmimGetDateFormat
	PROTO((XmimClientHandle		handle,
	       XmimDateFormat 		*format));

    extern DllDecl XmimReturnCode XmimSetExpDateFormat
	PROTO((XmimClientHandle		handle,
	       XmimDateFormat 	        format));
    
    extern DllDecl XmimReturnCode XmimGetExpDateFormat
	PROTO((XmimClientHandle		handle,
	       XmimDateFormat 	        *format));

    extern DllDecl XmimReturnCode XmimSetTimeFormat
	PROTO((XmimClientHandle		handle,
	       XmimTimeFormat 		format));
    
    extern DllDecl XmimReturnCode XmimGetTimeFormat
	PROTO((XmimClientHandle		handle,
	       XmimTimeFormat 		*format));

    extern DllDecl XmimReturnCode XmimVaSetOption
	PROTO((XmimVaArgs, ...));
    
    extern DllDecl XmimReturnCode XmimVaGetOption
	PROTO((XmimVaArgs, ...));

    extern DllDecl XmimReturnCode XmimLogMessage
	PROTO((XmimClientHandle		handle,
	       int			logLevel,
	       char			*msg));

    extern DllDecl XmimReturnCode XmimVaLogMessage
	PROTO((XmimVaArgs, ...));
    

/***************************************************************/

/* Every function below this line is a utility function. They are
   provided only for the purpose of convenience to the XMIM api
   programmers. These functions don't take client handle as
   argument and don't necessarily follow the rule of returning an
   XmimReturnCode value. */
    
/***************************************************************/

/****************************************/
/*					*/
/*	Memory Management Utilities	*/
/*					*/
/****************************************/

    extern DllDecl int  XmimFreeArray
	PROTO((void 			*array));

    extern DllDecl void *XmimMallocArray
	PROTO((int 			numOfElements,
	       int 			sizeOfElement));

    extern DllDecl void *XmimCopyArray
	PROTO((void 			*src,
	       int 			numOfElements,
	       int 			sizeOfElement));

    extern DllDecl int  XmimFreeStrings
	PROTO((char 			**stringArray,
	       int 			numOfStrings));

    extern DllDecl char** XmimMallocStrings
	PROTO((int 			n));
	      
    extern DllDecl char** XmimCopyStrings
	PROTO((char 			**src,
	       int 			numOfStrings));
    
#define XmimCopyValues(src, n) \
    (float*)XmimCopyArray((void*)(src), n, sizeof(float))

#define XmimMallocValues(n) \
    (float*)XmimMallocArray(n, sizeof(float))

#define XmimFreeValues(array) \
    XmimFreeArray((void*)(array))
	
#define XmimCopyDateTimes(src, n) \
    (XmimDateTime*)XmimCopyArray((void*)(src), n, sizeof(XmimDateTime))

#define XmimMallocDateTimes(n) \
    (XmimDateTime*)XmimMallocArray(n, sizeof(XmimDateTime))

#define XmimFreeDateTimes(array) \
    XmimFreeArray((void*)(array))
	
#define XmimCopyBooleans(src, n) \
    (XmimBoolean*)XmimCopyArray((void*)(src), n, sizeof(XmimBoolean))

#define XmimMallocBooleans(n) \
    (XmimBoolean*)XmimMallocArray(n, sizeof(XmimBoolean))	

#define XmimFreeBooleans(array) \
    XmimFreeArray((void*)(array))
	

/********************************/
/*				*/
/*	Date/Time Utilities	*/
/*				*/
/********************************/

    extern DllDecl int		XmimDateCmp
	PROTO((XmimDate 		date1,
	       XmimDate			date2));

    extern DllDecl int		XmimTimeCmp
	PROTO((XmimTime			time1,
	       XmimTime			time2));

    extern DllDecl int		XmimDateTimeCmp
	PROTO((XmimDateTime		dt1,
	       XmimDateTime		dt2));

    extern DllDecl XmimBoolean	XmimValidDate
	PROTO((XmimDate			date));

    extern DllDecl XmimBoolean	XmimValidTime
	PROTO((XmimTime			time));

    extern DllDecl XmimBoolean 	XmimValidDateTime
	PROTO((XmimDateTime		dt));

    extern DllDecl XmimDate	XmimSetDate
	PROTO((unsigned 		y,
	       unsigned 		m,
	       unsigned 		d));

    extern DllDecl XmimTime	XmimSetTime
	PROTO((unsigned 		h,
	       unsigned 		m,
	       unsigned			s));

    extern DllDecl XmimDateTime	XmimSetDateTime
	PROTO((unsigned 		y,
	       unsigned 		m,
	       unsigned 		d,
	       unsigned 		h,
	       unsigned 		mi,
	       unsigned			s));

    extern DllDecl XmimDateTime	XmimSetDateTimeMillis
	PROTO((unsigned 		y,
	       unsigned 		m,
	       unsigned 		d,
	       unsigned 		h,
	       unsigned 		mi,
	       unsigned			s,
	       unsigned                 millis));

    extern DllDecl char* XmimStringExpDate
	PROTO((XmimDate			d));

#define XmimPrintExpDate(d) \
    printf(XmimStringExpDate(d))

    extern DllDecl char* XmimStringExpDateWithFormat
	PROTO((XmimDate			d,
	       XmimDateFormat	fmt));
    
#define XmimPrintExpDateWithFormat(d, f) \
    printf(XmimStringExpDateWithFormat(d, f))
    
    extern DllDecl char* XmimStringDate
	PROTO((XmimDate			d));

#define XmimPrintDate(d) \
    printf(XmimStringDate(d))

    extern DllDecl char* XmimStringDateWithFormat
	PROTO((XmimDate			d,
	       XmimDateFormat		fmt));

#define XmimPrintDateWithFormat(d, f) \
    printf(XmimStringDateWithFormat(d, f))

    extern DllDecl char* XmimStringTime
	PROTO((XmimTime			t,
	       XmimBoolean		printSecond));

#define XmimPrintTime(d, s) \
    printf(XmimStringTime(d, s))

    extern DllDecl char* XmimStringTimeWithFormat
	PROTO((XmimTime			t,
	       XmimBoolean		printSecond,
	       XmimTimeFormat		fmt));

#define XmimPrintTimeWithFormat(d, s, f) \
    printf(XmimStringTimeWithFormat(d, s, f))

    extern DllDecl char* XmimStringDateTime
	PROTO((XmimDateTime		dt,
	       XmimBoolean		printTime,
	       XmimBoolean		printSecond));

#define XmimPrintDateTime(d, t, s) \
    printf(XmimStringDateTime(d, t, s))

    extern DllDecl char* XmimStringDateTimeWithFormat
	PROTO((XmimDateTime		dt,
	       XmimBoolean		printTime,
	       XmimBoolean		printSecond,
	       XmimDateFormat	        dateFmt,
	       XmimTimeFormat		timeFmt));

#define XmimPrintDateTimeWithFormat(d, t, s, f1, f2) \
    printf(XmimStringDateTimeWithFormat(d, t, s, f1, f2))
	
    extern DllDecl void XmimPrintError
	PROTO((char			*prefixMsg));

    extern DllDecl char* XmimStringError
	PROTO((void));
    
    extern DllDecl int XmimGetRecordIndex
	PROTO((XmimDateTime		dateTime,
	       int			numOfDateTimes,
	       XmimDateTime		*dateTimes,
	       XmimFillOption 		fillOption));
    
    extern DllDecl int XmimVaGetRecordIndex
	PROTO((XmimVaArgs, ...));

    extern DllDecl int XmimSetRolloverDay
	PROTO((char			*rolloverDayStr,
	       XmimRolloverDay 		*rolloverDayStruct));

    extern DllDecl char* XmimRolloverDayAsString
	PROTO((XmimRolloverDay		rolloverDayStruct));

    extern DllDecl int XmimSetRolloverPolicy
	PROTO((char			*rolloverPolicyStr,
	       XmimRolloverPolicy 	*rolloverPolicyStruct));

    extern DllDecl char* XmimRolloverPolicyAsString
	PROTO((XmimRolloverPolicy	rolloverPolicyStruct));

    extern DllDecl XmimDateTime XmimGetDateTime
	PROTO((XmimDateTimeObj		*dateTimeObj,
	       int			i));

    extern DllDecl unsigned XmimDateToJulianDate
	PROTO((XmimDate			*date));
    
    extern DllDecl XmimDate XmimDateFromJulianDate
	PROTO((unsigned 		jul_date));
    
    extern DllDecl unsigned XmimTimeToSecondsOfDay
	PROTO((XmimTime			*time));
    
    extern DllDecl XmimTime XmimTimeFromSecondsOfDay
	PROTO((unsigned 		seconds_of_day));
    

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
inline XmimBoolean	operator==(XmimDateTime 	dt1,
				   XmimDateTime		dt2)
{
    return XmimDateTimeCmp(dt1, dt2) == 0;
}
    
inline XmimBoolean	operator!=(XmimDateTime 	dt1,
				   XmimDateTime		dt2)
{
    return XmimDateTimeCmp(dt1, dt2) != 0;
}

inline XmimBoolean	operator<(XmimDateTime		dt1,
				  XmimDateTime		dt2)
{
    return XmimDateTimeCmp(dt1, dt2) < 0;
}

inline XmimBoolean	operator<=(XmimDateTime		dt1,
				   XmimDateTime		dt2)
{
    return XmimDateTimeCmp(dt1, dt2) <= 0;
}

inline XmimBoolean	operator>(XmimDateTime		dt1,
				  XmimDateTime		dt2)
{
    return XmimDateTimeCmp(dt1, dt2) > 0;
}

inline XmimBoolean	operator>=(XmimDateTime		dt1,
				   XmimDateTime		dt2)
{
    return XmimDateTimeCmp(dt1, dt2) >= 0;
}

inline XmimBoolean	operator==(XmimDate 		d1,
				   XmimDate		d2)
{
    return XmimDateCmp(d1, d2) == 0;
}
    
inline XmimBoolean	operator!=(XmimDate 		d1,
				   XmimDate		d2)
{
    return XmimDateCmp(d1, d2) != 0;
}

inline XmimBoolean	operator<(XmimDate		d1,
				  XmimDate		d2)
{
    return XmimDateCmp(d1, d2) < 0;
}

inline XmimBoolean	operator<=(XmimDate		d1,
				   XmimDate		d2)
{
    return XmimDateCmp(d1, d2) <= 0;
}

inline XmimBoolean	operator>(XmimDate		d1,
				  XmimDate		d2)
{
    return XmimDateCmp(d1, d2) > 0;
}

inline XmimBoolean	operator>=(XmimDate		d1,
				   XmimDate		d2)
{
    return XmimDateCmp(d1, d2) >= 0;
}

inline XmimBoolean	operator==(XmimTime 		t1,
				   XmimTime		t2)
{
    return XmimTimeCmp(t1, t2) == 0;
}
    
inline XmimBoolean	operator!=(XmimTime	 	t1,
				   XmimTime		t2)
{
    return XmimTimeCmp(t1, t2) != 0;
}

inline XmimBoolean	operator<(XmimTime		t1,
				  XmimTime		t2)
{
    return XmimTimeCmp(t1, t2) < 0;
}

inline XmimBoolean	operator<=(XmimTime		t1,
				   XmimTime		t2)
{
    return XmimTimeCmp(t1, t2) <= 0;
}

inline XmimBoolean	operator>(XmimTime		t1,
				  XmimTime		t2)
{
    return XmimTimeCmp(t1, t2) > 0;
}

inline XmimBoolean	operator>=(XmimTime		t1,
				   XmimTime		t2)
{
    return XmimTimeCmp(t1, t2) >= 0;
}

#endif /* __cplusplus */

#undef PROTO
#undef CONST

#endif /* !defined(_XMIM_API_H) */
