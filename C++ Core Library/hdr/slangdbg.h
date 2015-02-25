/* $Header: /home/cvs/src/slang/src/slangdbg.h,v 1.17 2001/11/27 23:27:15 procmon Exp $ */
/****************************************************************
**
**	SLANGDBG.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.17 $
**
****************************************************************/

#if !defined( IN_SLANGDBG_H )
#define IN_SLANGDBG_H

#ifndef IN_SECEXPR_H
#include <secexpr.h>
#endif

/*
**	Define debugging types
*/

typedef enum
{
	SlangDebugStepTypeNone = 0,
	SlangDebugStepTypeDown,
	SlangDebugStepTypeNext,
	SlangDebugStepTypeUp,
	SlangDebugStepTypeReturn,
	SlangDebugStepTypeRetry,
	SlangDebugStepTypeBackup,
	SlangDebugStepTypeLineInto,
	SlangDebugStepTypeLineOver,
	SlangDebugStepTypeStop

} SLANG_DEBUG_STEP_TYPE;


/*
**	Define types (for backward compatibility)
*/

typedef	SLANG_DEBUG_FUNC PF_SLANG_DEBUG;


/*
**	Structure used to hold debugging context
*/

typedef struct SlangDebugContextStructure
{
	SLANG_DEBUG_STEP_TYPE
			StepType;

	unsigned
			LastCallDepth,
			LastNodeDepth;
	
	int		 LastLineNumber;
			
	SLANG_NODE
			*LastFunc,
			*LastSet;
			
} SLANG_DEBUG_CONTEXT;


/*
**	Define function prototypes
*/

DLLEXPORT void
		SlangDebugAttach( 			SLANG_NODE *Node ),
		SlangDebugDetach(			SLANG_NODE *Node ),
		SlangDebugNoteReturn(		SLANG_DEBUG_CONTEXT *DebugContext );

DLLEXPORT void SlangDebugEnsureContext(
      SLANG_NODE      *Node,
      SLANG_DEBUG_CONTEXT *pDebugContext
);
		
DLLEXPORT int
		SlangDebugStepTypeSet(		SLANG_DEBUG_STEP_TYPE StepType, SLANG_NODE *CurNode ),
		SlangDebugNodeStats(		SLANG_NODE *Node, int *CallDepth, int *NodeDepth );

DLLEXPORT int
		SlangDebugIntercept(		SLANG_ARGS );

DLLEXPORT int
        SlangDebugBreakpointToggle     ( DT_SLANG   *Slang );

DLLEXPORT bool
        SlangDebugBreakpointToggleNode ( SLANG_NODE *Node  );

#endif

