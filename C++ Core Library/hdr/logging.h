/* $Header: /home/cvs/src/kool_ade/src/logging.h,v 1.8 2001/11/27 22:49:06 procmon Exp $ */
/****************************************************************
**
**	LOGGING.H	- support routines for logging errors
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_LOGGING_H )
#define IN_LOGGING_H


/*
**	Types of log messages
*/

#define LOG_ERROR				0x0001	// Error messages
#define LOG_WARN  				0x0002	// Warnings
#define LOG_INFO				0x0004	// Informative messages (not too many...)
#define LOG_VERB  				0x0008	// Verbose things
#define	LOG_ERRLOG				0x0010	// Shove in Error Log (even though not error)
#define LOG_MEM   				0x0020	// Memory stuff
#define LOG_USER_BASE			0x0100	// Top 8 bits for application


/*
**	Abort condtions mask bits for LogConditionRegister and LogConditionCheck
*/

#define	LOG_COND_TIMESTAMP		0x0001	// Timestamp in errlog
#define	LOG_COND_ESC_KEY		0x0002	// Check for <Esc> key
#define	LOG_COND_MAX_MEM		0x0004	// Check for excessive heap usage
#define	LOG_COND_FP_EXCEPTION	0x0010	// Check for prior floating point exception (LogMathErrors must have been used)
#define	LOG_COND_CTRL_C			0x0020	// Check for prior Ctrl-C (LogCtrlC must have been used)
#define	LOG_COND_FLAG_FILE		0x0040	// Check for flag file, <BaseFileName> + ".FLG"
#define	LOG_COND_SEMAPHORE		0x0080	// Check for cleared semaphore, \sem\<BaseFileNameRoot>
#define	LOG_COND_RENAME_LOG		0x0100	// Rename log
#define LOG_COND_USER_BASE		0x1000	// First bit available for application defined conditions



/*
**	Constants used by LogRegisterAll
*/

#define LOG_PERIOD_TIMESTAMP	( 60 * 10 )			// Every 10 minutes
#define	LOG_PERIOD_MAX_MEM		( 60 * 5 )			// Every 5 minutes
#define LOG_PERIOD_FLAG_FILE	( 60 * 1 )			// Every minute
#define LOG_PERIOD_RENAME_LOG	( 60 * 60 * 24 )	// Every day
// All others conditions periods are zero, i.e. every time

#define	LOG_MAX_MEM				( 512 * 1024L )		// 500K max heap
#define LOG_RENAME_TIMEOFDAY	( 60 * 60 * 23 )	// 11PM



/*
**	Condition function callback for LogConditionCheck.
**	
**		Returns: FALSE		- LogConditionCheck continues looking for other
**						  	  conditions
**				 non-zero	- LogConditionCheck immediately returns TRUE
*/

			
typedef int (*LOG_CONDITION_FUNC)( void * );



/*
**	Logging routines
*/

							
DLLEXPORT void
		 LogPrintf( 				int Mask, const char *Format, ... ) ARGS_LIKE_PRINTF(2);

DLLEXPORT int
		LogPrintfMask(				int NewMask ),
		LogOpenFiles(				char *BaseName ),
		
		LogConditionCheck(			int ConditionMask ),
		LogConditionRegister(		int ConditionMask, long PeriodSeconds, long LastChecked, LOG_CONDITION_FUNC ConditionFunc, void *ConditionFuncArg ),
		LogConditionRegisterAll(	void );
		
		
// LOG_CONDITION_FUNCs

DLLEXPORT int
		LogConditionFuncAbort(		void * ),		// Always returns TRUE
		LogConditionFuncContinue(	void * ),		// Always returns FALSE
		LogConditionFuncRenameLog(	void * ),		// Renames files and returns TRUE
		LogConditionFuncMaxMem(		void * );		// Returns TRUE if heap usage above LOG_MAX_MEM
		

#endif

