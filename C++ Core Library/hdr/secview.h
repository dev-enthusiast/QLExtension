/****************************************************************
**
**	SECVIEW.H	- Header information for security viewer
**
**	$Header: /home/cvs/src/secview/src/secview.h,v 1.61 2001/11/27 23:26:38 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECVIEW_H )
#define IN_SECVIEW_H

#ifndef _IN_WINDOW_H
#include <window.h>
#endif
#ifndef _IN_SECDB_H
#include <secdb.h>
#endif

#if 0
typedef struct SecViewDialogStructure
{
	DT_VALUE
			EditArray,
			Values;

	char	*Title,					// Just set these values
			*SubTitle;

} SECVIEW_DIALOG;
#endif


/*
**	Prototype functions
*/

DLLEXPORT SDB_DB
		*SecViewStartup(				const char *DatabaseName, SDB_MSG_FUNC MsgFunc = 0, SDB_MSG_FUNC ErrMsgFunc = 0 );

DLLEXPORT void
		SecViewShutdown(				void ),
		SecViewInitWindows(				void ),
		SecViewWindowTitle(             const char *Title ),
		SecViewKillWindows(				int EndType ),
		SecViewMain(					void ),
		SecViewEditExpressions(			const char *SecName ),
		SecViewMessage(					const char *Msg ),
		SecViewErrorMessage(			const char *Msg ),
		SecViewErrorLogEnable(			int State ),
		SecViewErrorLogBrowse(			void ),
		SecViewShowObjects(				void ),
		SecViewShowSecurity(			SDB_OBJECT *SecPtr ),
		SecViewShowChildren(			SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType ),
		SecViewShowValueTypes(			SDB_OBJECT *SecPtr ),
		SecViewShowObjectData(			SDB_OBJECT *SecPtr ),
		SecViewDumpObjectData(			SDB_OBJECT *SecPtr ),
		SecViewShowCachedValues(		SDB_OBJECT *SecPtr ),
		SecViewShowCachedValuesForVT(	SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType ),
		SecViewShowStatistics(			void ),
		SecViewShowGlobalValueTypes(	void ),
		SecViewShowValue(				DT_VALUE *Value ),
		SecViewShowMethods(				DT_VALUE *Value ),
		SecViewShowNamedValue(			const char *Name, DT_VALUE *Value, int ShowStringOnly ),
		SecViewSetMode(					void ),
		SecViewCatchFPErrors(			void ),
		SecViewScratchPad(				void ),
		SecViewTraceStart(				int Flags ),
		SecViewTraceEnd(				void ),
		SecViewTraceMsgStart(			int Verbose, int NumLevels ),
		SecViewTraceMsgEnd(				void );

DLLEXPORT DT_DATA_TYPE
		SecViewShowGlobalDataTypes(		void );

DLLEXPORT int
		SecViewAvailable(				void ),
		SecGuiAvailable(				void ),
		SecJavaAvailable(               void ),
		SecViewCreateObject(			SDB_SEC_TYPE SecType, char *SecName ),
		SecViewPrintFunc(				void *Handle, const char *Format, ... ) ARGS_LIKE_PRINTF(2),
		SecViewShowCacheNode(			SDB_NODE *Node ),
		SecViewShowDatabases(			char *DatabaseName ),
		SecViewShowRefs(                SDB_DB	*Db, char *SecName ),
		SecViewSlangRegisterHotKey(		int Key, const char *ScriptName, int CacheFlag ),
        SecViewTrapAbort(               int State ),
		SecViewValidate(				SDB_OBJECT *SecPtr, int *ReturnedFlags, SDB_VALUE_TYPE *ReturnedValueType ),
        SecViewWindowSave(		        WINDOW *Wnd ),
        SecViewTraceBreak(				SDB_OBJECT *SecPtr, DT_ARRAY *VtApply, int Message, int OnOrOff, bool exclude = false );

DLLEXPORT char
		*SecViewSlangShowFunctions(		const char *StartingFunction ),
		*SecViewSlangShowConstants(		void );


#if defined( IN_SECEXPR_H )
DLLEXPORT void
		SecViewSlangShowVariables(		SLANG_SCOPE_FUNCTION *ScopeFunc ),
		SecViewSlangShowVariableScopes( void ),
		SecViewSlangBrowseOutput(		void ),
		SecViewSlangBrowseOutputs(		void ),
//		SecViewSlangScriptToWindow(		WINDOW *Wnd, const char *ScriptName ),
		SecViewSlangShowElapsedTime(	void ),
		SecViewSlangSetContext(			void ),
		SecViewSlangOutputPush(			const char *Title ),
		SecViewSlangOutputPop(			void ),
		SetSecGuiAvailable(				int ),
		SetSecJavaAvailable(            int State );

DLLEXPORT int
		SecViewSlangEvaluate(			SLANG_NODE *Expression, SLANG_SCOPE *Scope, const char *ProfileFileName ),
		SecViewSlangAbortCheck(	   		void *Handle, SLANG_NODE *Root, SLANG_SCOPE *Scope ),
		SecViewSlangDebug(				void *Handle, SLANG_NODE *Root, SLANG_SCOPE *Scope );

DLLEXPORT SLANG_RET_CODE
		SecViewSlangError(				void *Handle, SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, int ErrCode );

DLLEXPORT SLANG_SCOPE_FUNCTION
		*SecViewSlangShowCallStack(		SLANG_SCOPE *Scope, int *Depth );
#endif


#if defined( IN_SECINDEX_H )
DLLEXPORT SDB_INDEX
		*SecViewShowIndices( 			void );

DLLEXPORT void
		SecViewShowIndexDetail(			SDB_INDEX *Index );
#endif


#if defined( IN_SECTRANS_H )
DLLEXPORT void
		SecViewShowTransaction(		SDB_DB *Db, SDB_TRANS_ID TransID ),
		SecViewShowTransPart(		SDB_TRANS_HEADER *Header, SDB_TRANS_PART *Trans, int PartNumber );

DLLEXPORT SDB_TRANS_ID
		SecViewShowTransactions(	SDB_DB *Db );
#endif


DLLEXPORT void
		SecViewShowDescription(		SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, SDB_VALUE_TYPE_INFO *Vti );

DLLEXPORT int
		w_generic_pick(				WINDOW *w, int MinLine, int MaxLine, int SearchMode );

DLLEXPORT void
		WindowHexDump(				WINDOW *w, void *MemToDump, int Size );

DLLEXPORT int
		SecViewRunning(				void );

#endif
