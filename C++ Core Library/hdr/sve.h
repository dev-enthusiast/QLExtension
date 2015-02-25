/* $Header: /home/cvs/src/sve/src/sve.h,v 1.39 2006/01/09 14:05:21 e45019 Exp $ */
/****************************************************************
**
**	SVE.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.39 $
**
****************************************************************/

#ifndef IN_SVE_H
#define IN_SVE_H

#if !defined( IN_SECDB_H )
#include	<secdb.h>
#endif
#if !defined( IN_WINDOW_H )
#include	<window.h>
#endif


/*
**	Define constants
*/

#define	SVE_EDIT_WIDTH				200
#define	SVE_EDIT_BUFFER_LENGTH		2000
#define	SVE_SCRIPT_BUFFER_SIZE		64000
#define	SVE_SEARCH_STRING_SIZE		60

#define	SVE_MAX_BUFFERS				256

#define	SVE_EDIT_BUFFER_TITLE_SIZE	80
#define	SVE_EDIT_BUFFER_NAME_SIZE 	50

#define	SVE_BUFFER_TYPE_OBJECT		0
#define	SVE_BUFFER_TYPE_FILE		1
#define	SVE_BUFFER_TYPE_SCRATCH		2
#define	SVE_BUFFER_TYPE_OUTPUT		3

#define	SVE_KEY_DEFAULT				-1


/*
**	Define undo codes
*/

#define	SVE_UNDO_POS				1
#define	SVE_UNDO_INSERT				2
#define	SVE_UNDO_INSERT_LINE		3
#define	SVE_UNDO_INSERT_CHUNK		4
#define	SVE_UNDO_INSERT_MOVE		5
#define	SVE_UNDO_DELETE				6
#define	SVE_UNDO_BACKSPACE			7
#define	SVE_UNDO_DROP_MARK			8
#define	SVE_UNDO_RAISE_MARK			9
#define	SVE_UNDO_CHANGE_MARK		10
#define	SVE_UNDO_PASTE				11
#define	SVE_UNDO_POP_PASTE			12


/*
**	Define mark types
*/

#define	SVE_MARK_NONE				0
#define	SVE_MARK_REGION				1
#define	SVE_MARK_COLUMN				2
#define	SVE_MARK_LINE				3


/*
**	Define types
*/

struct SLANG_SCOPE;						// forward declare

#ifdef __cplusplus
extern "C"
{
#endif
typedef	void	(*SVE_PF_KEY_ACTION)(	void *Arg );
typedef	void	(*SVE_PF_KEY_FREE)(		void *Arg );
#ifdef __cplusplus
};
#endif

typedef	long	SVE_BUFFER_ID;


/*
**	Structure used to contain key map entries
*/

typedef struct SveKeyMapEntryStructure
{
	long	KeyCode;		// Key to match

	SVE_PF_KEY_ACTION
			pfAction;		// Action to take when key is hit

	SVE_PF_KEY_FREE
			pfFree;			// Function to call when key is un-registered

	void	*Arg;			// Optional argument

} SVE_KEY_MAP_ENTRY;


/*
**	Structure used to contain key maps (these get stacked)
*/

typedef struct SveKeyMapStructure
{
	struct SveKeyMapStructure
			*Prev;				// Pointer to previous key map (stack)

	struct HashStructure
			*EntryHash;			// Hash of individual key map entries

	int		LookThrough;		// TRUE if previous key map should be checked
								// if key not found in this key map

	SVE_PF_KEY_ACTION
			pfDefaultAction;	// Default key action

	void	*Arg;				// Argument for default key action

} SVE_KEY_MAP;


/*
**	Structure used to hold mark information (anchors)
*/

typedef struct SveMarkAnchorStructure
{
	struct SveMarkAnchorStructure
			*Prev;

	int		Type,
			XStart,
			YStart,
			XEnd,
			YEnd;

} SVE_MARK_ANCHOR;


/*
**	Structure used to hold paste buffer information
*/

typedef struct SvePasteBufferStructure
{
	int		MarkType,
			Lines;
	
	char	**Data;		// Stored as a valid string table

} SVE_PASTE_BUFFER;


/*
**	Structure used to hold undo information
*/

typedef struct SveUndoStructure
{
	struct SveUndoStructure
			*Prev;

	int		Operation;

	long	KeyStrokeNumber;

	union
	{
		struct
		{
			int		x,
					y;

		} Pos;

		struct
		{
			char	*String;

		} Insert;

		struct
		{
			int		x,
					y,
					AlphaNum;
			
			char	*String;

		} InsertChunk;

		struct
		{
			int		Count;

		} Delete;

		struct
		{
			int		x,
					y;

		} Backspace;

		struct
		{
			SVE_MARK_ANCHOR
					MarkAnchor;

		} DropMark;

		struct
		{
			int		MarkType;

		} ChangeMark;

		struct
		{
			SVE_PASTE_BUFFER
					*PasteBuffer;

			int		XStart,
					YStart,
					XCursor,
					YCursor;

		} Paste;

		struct
		{
			SVE_PASTE_BUFFER
					*PasteBuffer;

		} PopPaste;			

	} Data;

} SVE_UNDO;


/*
**	Structure used for each edit buffer
*/

typedef struct SveEditBufferStructure
{
	struct SveEditBufferStructure
			*Prev,
			*Next;

	WINDOW	WndExpr;

	SVE_BUFFER_ID
			BufferID;

	char	Name[	SVE_EDIT_BUFFER_NAME_SIZE + 1 ],
			*Title,			// [ w_global.x_vres + 1 ]
			*Subtitle;		// [ w_global.x_vres + 1 ]

	int		Type,
			ReadOnly,
			Modified,
			UndoPop,
			MarkDirty;

	long	KeyStrokeNumber;

	SVE_UNDO
			*UndoStack;

	SVE_MARK_ANCHOR
			*MarkStack;

	void	*Arg;				// Extra all-purpose argument

	char	*BackupFileName,
			*VersionControlPath,
			*DatabaseName;

	int		SubtitleWidth;		// Allocated size of Subtitle buffer

	char	PrivateScope[	SVE_EDIT_BUFFER_NAME_SIZE + 1 ];

} SVE_EDIT_BUFFER;


/*
**	Global variable structure
*/

typedef struct SveGlobalStructure
{
	SVE_EDIT_BUFFER
			*CurrentBuffer;

	SVE_KEY_MAP
			*KeyMap;

	SVE_PASTE_BUFFER
		    *PasteBuffer;

	SLANG_SCOPE
			*Scope;

	struct HashStructure
			*ModuleHash;

	char	TmpDirectory[ 	FILENAME_MAX ],
			ProfilePrefix[	FILENAME_MAX ],
			SlangPrefix[	FILENAME_MAX ],
			*FileNamePrefix;

	int		ExitRequested,
			CurrentKeyCode,
			FreePasteBuffer;

} SVE_GLOBAL;


/*
**	Variables
*/

DLLEXPORT SVE_GLOBAL
		SveGlobal;

DLLEXPORT char
		*SveStrTabBufferType[],
		**SveStrTabBufferTypePtr;


/*
**	Macro used to make window reference easier
*/

#define	WND_EXPR	SveGlobal.CurrentBuffer->WndExpr


/*
**	Macro to assist drawing the cursor
*/

#define SVE_SHOW_CURSOR(eb,w)	\
		if( !w_isvis( (w), (w)->x_cur, (w)->y_cur, 1, 1 ))	\
			w_makevis( (w), (w)->x_cur, (w)->y_cur, 1, 1 );	\
		else												\
			w_adraw( (w), (w)->x_cur, (w)->y_cur, (w)->x_cur, (w)->y_cur );	\
		if( (eb)->MarkStack && (eb)->MarkStack->Type )								\
			SveFixMark( eb );


/*
**	Prototype functions
*/

DLLEXPORT void
		SveInitialize(				void ),
		SveEditor(					void ),
		SveRelease(					void );

DLLEXPORT void
		SveKeyMapPush(				int LookThrough ),
		SveKeyMapPop(				void ),
		SveKeyRegister(				int KeyCode, SVE_PF_KEY_ACTION pfAction, SVE_PF_KEY_FREE pfFree, void *Arg ),
		SveKeyTableRegister(		SVE_KEY_MAP_ENTRY *KeyMapEntries ),
		SveKeyRegisterModule(		int KeyCode, char *ModuleName, char *FunctionName ),
		SveKeyProcess(				void );

DLLEXPORT int
		SveKeyRegisterExpression(	int KeyCode, char *Expression );

DLLEXPORT void
		SveScriptToWindow(			WINDOW *Wnd, const char *ScriptName );
DLLEXPORT int
		SveScriptUse(				const char *ScriptName, int Reload );

DLLEXPORT SVE_EDIT_BUFFER
		*SveBufferFromID(			SVE_BUFFER_ID BufferID ),
		*SveBufferCreate(			const char *Name, int Type, int xl, int yl, int xh, int yh ),
		*SveBufferSelect(			void ),
		*SveBufferFromName(			const char *Name );

DLLEXPORT void
		SveBufferSetName(			SVE_EDIT_BUFFER *EditBuffer, const char *Name, int Type ),
		SveBufferSetSubtitle(		SVE_EDIT_BUFFER *EditBuffer ),
		SveBufferSetVersionControl(	SVE_EDIT_BUFFER *EditBuffer, SDB_OBJECT *SecPtr ),
		SveBufferDestroy(			SVE_EDIT_BUFFER	*EditBuffer ),
		SveBufferUse(				SVE_EDIT_BUFFER	*EditBuffer ),
		SveBufferHide(				SVE_EDIT_BUFFER	*EditBuffer ),
		SveBufferShow(				SVE_EDIT_BUFFER	*EditBuffer ),
		SveBufferLink(				SVE_EDIT_BUFFER *EditBuffer ),
		SveSlangStatusPush(		    int Color, const char *Text ),
		SveSlangStatusPushNoSelect(	int	Color, const char *Text ),
		SveSlangStatusChange(	    int Color, const char *Text ),
		SveSlangStatusPop(		    void );

DLLEXPORT int
		SveBufferWrite(				SVE_EDIT_BUFFER	*EditBuffer ),
		SveBufferModifiedCount(		void ),
		SveBufferRename(			SVE_EDIT_BUFFER *EditBuffer ),
//		SveBufferSaveToFile(		SVE_EDIT_BUFFER *EditBuffer ),
		SveBufferUnlink(			SVE_EDIT_BUFFER	*EditBuffer );

DLLEXPORT void
		SveUndoPush(				SVE_EDIT_BUFFER *EditBuffer, int Operation, SVE_UNDO *Undo ),
		SveUndoDestroy(				SVE_EDIT_BUFFER *EditBuffer );

DLLEXPORT int
		SveUndoPop(					SVE_EDIT_BUFFER *EditBuffer, int Commit );


/*
**	Key handlers
*/

DLLEXPORT void
		KeyBackspace				( void *Arg ),
		KeyBufferEnd				( void *Arg ),
		KeyBufferHome 				( void *Arg ),
		KeyChangeBufferName			( void *Arg ),
		KeyCompleteFunctionName		( void *Arg ),
		KeyDeleteChar				( void *Arg ),
		KeyDeleteLine				( void *Arg ),
		KeyDeleteToEndOfLine		( void *Arg ),
		KeyDestroyBuffer  			( void *Arg ),
		KeyDitto					( void *Arg ),
		KeyDown						( void *Arg ),
		KeyEditFile					( void *Arg ),
		KeyEditObject				( void *Arg ),
		KeyEnd						( void *Arg ),
		KeyEnter					( void *Arg ),
		KeyExit						( void *Arg ),
		KeyExternalEditor			( void *Arg ),
		KeyGoToLine					( void *Arg ),
		KeyHome						( void *Arg ),
		KeyHyperlink				( void *Arg ),
		KeyInsertSelf				( void *Arg ),
		KeyLeft						( void *Arg ),
		KeyListAndPickBuffers		( void *Arg ),
		KeyNewBuffer				( void *Arg ),
		KeyNextBuffer				( void *Arg ),
		KeyPageDown					( void *Arg ),
		KeyPageUp					( void *Arg ),
		KeyRight					( void *Arg ),
		KeySearch					( void *Arg ),
		KeyISearch					( void *Arg ),
		KeySearchNext				( void *Arg ),
		KeySearchPrev				( void *Arg ),
		KeyShowHelp					( void *Arg ),
		KeyToggleInsertMode			( void *Arg ),
		KeyTranslate				( void *Arg ),
		KeyUndo						( void *Arg ),
		KeyUp						( void *Arg ),
		KeyWordLeft					( void *Arg ),
		KeyWordRight				( void *Arg ),
		KeyWriteBuffer				( void *Arg );
									

/*
**	Junk
*/

DLLEXPORT int
		SveInsert(					SVE_EDIT_BUFFER *EditBuffer, const char *Text, int InsertMode ),
		SvePos(				 		SVE_EDIT_BUFFER *EditBuffer, int x, int y ),
		SveRPos(			 		SVE_EDIT_BUFFER *EditBuffer, int xOffset, int yOffset ),
		SvePrevChar(				SVE_EDIT_BUFFER *EditBuffer ),
		SvePrevWord(				SVE_EDIT_BUFFER *EditBuffer ),
		SveNextWord(				SVE_EDIT_BUFFER *EditBuffer ),
		SveBackspace(		 		SVE_EDIT_BUFFER *EditBuffer ),
		SveDeleteChar(		 		SVE_EDIT_BUFFER *EditBuffer ),
		SveDeleteLine(		 		SVE_EDIT_BUFFER *EditBuffer ),
		SveDeleteEOL(		 		SVE_EDIT_BUFFER *EditBuffer ),
		SveSearchForward(			SVE_EDIT_BUFFER *EditBuffer, char *SearchString ),
		SveSearchBackward(			SVE_EDIT_BUFFER *EditBuffer, char *SearchString ),
		SveTranslateForward(		SVE_EDIT_BUFFER *EditBuffer, char *SearchString, char *ReplaceString, int Global );

DLLEXPORT SVE_PASTE_BUFFER
		*SveBuildPasteBuffer( 		SVE_EDIT_BUFFER *EditBuffer );

DLLEXPORT void
		SveDropMark(				SVE_EDIT_BUFFER *EditBuffer, int MarkType, SVE_MARK_ANCHOR *Mark ),
		SveRaiseMark(				SVE_EDIT_BUFFER *EditBuffer ),
		SveChangeMark(				SVE_EDIT_BUFFER *EditBuffer, int MarkType ),
		SveFixMark(					SVE_EDIT_BUFFER *EditBuffer ),
		SvePaste(					SVE_EDIT_BUFFER *EditBuffer, SVE_PASTE_BUFFER *PasteBuffer ),
		SveRefOff(					SVE_EDIT_BUFFER *EditBuffer ),
		SveRefOn(					SVE_EDIT_BUFFER *EditBuffer ),
		SveClear(					SVE_EDIT_BUFFER *EditBuffer ),
		SveShiftLines(				SVE_EDIT_BUFFER *EditBuffer, int ShiftSize ),
		SveFreePasteBuffer(			SVE_PASTE_BUFFER *PasteBuffer ),
		SveSetCurrentPasteBuffer(	SVE_PASTE_BUFFER *PasteBuffer );

DLLEXPORT int
		SveCut(						SVE_EDIT_BUFFER *EditBuffer ),
		SveCopy(					SVE_EDIT_BUFFER *EditBuffer );

DLLEXPORT int
		IsSpaces(					char *s ),
		FileToWindow(				char *FileName, WINDOW *Wnd ),
		SveWindowToBuffer(			WINDOW *Wnd, char *BufPtr, unsigned int MaxSize ),
		SveWindowToFile(   			WINDOW *Wnd, char *FileName, char *Prefix, char *Postfix ),
		SveWindowReplaceText(		WINDOW *Wnd, char *Text ),
		EndOfLine(					WINDOW *Wnd, int Line );

DLLEXPORT void
		SveViewFile(  				char *FileName );

		
DLLEXPORT int
		DtFuncSve(					int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b );

#endif

