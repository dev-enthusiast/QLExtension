/****************************************************************
**
**	editinfo.h - EDIT_INFO DataType
**
**	$Header: /home/cvs/src/secedit/src/editinfo.h,v 1.58 2001/03/29 16:31:07 simpsk Exp $
**
****************************************************************/

#if !defined( IN_EDITINFO_H )
#define IN_EDITINFO_H

#include <secdb.h>
#include <secexpr.h>
#include <wfkeys.h>


/*
**	Constants
*/

int const EDIT_INFO_FORMAT_MAX = 256;		// For EditInfoFormatValue


/*
**	Types of EDIT_INFO fields
**		also add entries to editinfo.c/EditInfoTypeInfo[]
**	NOTE: These fields must remain in order because they get stored in database
*/

enum EDIT_INFO_TYPE
{
	EI_NULL	= 0,	// End of EDIT_INFO array
	EI_BLOCK_OBJ,	// .Extra is name of Edit Table object
	EI_TITLE,		// Display Prompt only
	EI_STRING,
	EI_STRTAB,		// .Extra points to string table handle (char***)
	EI_INT,
	EI_DOUBLE,
	EI_FLOAT = EI_DOUBLE,
	EI_DATE,
	EI_YESNO,
	EI_CURVE,
	EI_PARAGRAPH,
	EI_ARRAY,		// .Extra points to another EDIT_INFO
	EI_TIME,
	EI_STRUCT,		// .Extra points to array of EDIT_INFO
	EI_SECPICK,		// .Extra points to security class name
	EI_BLOCK,		// .Extra points to a movable block
	EI_BLOCKBEGIN,
	EI_BLOCKEND,
	EI_STRTABNUM,	// .Extra points to string table handle
	EI_CONFIG,		// .Extra points to EDIT_INFO_CONFIG
	EI_BLOCK_VT,	// .Extra points to name of valuetype for block edit info
	EI_RDATE,
	EI_RCURVE,
	EI_BUTTON,
	EI_POSITION,
	EI_STRTAB_VT,	// .Extra points to name of valuetype for string table
	EI_STRTAB_OBJ,	// .Extra points to name of a String Table object
	EI_RTIME,

	EI_LAST			// Must always be the last value
};


// EDIT_INFO/Flags
#define EIF_NORMAL				0x0000
#define EIF_READ_ONLY			0x0001
#define EIF_STATIC_FLAG			0x0002
#define EIF_STATIC				( EIF_STATIC_FLAG | EIF_READ_ONLY | EIF_PRIVATE )
#define EIF_STATIC_DIDDLE		( EIF_STATIC_FLAG | EIF_DIDDLE )
#define EIF_CALCULATED_FLAG		0x0004
#define EIF_CALCULATED			( EIF_CALCULATED_FLAG | EIF_READ_ONLY )
#define EIF_CALC_DIDDLE			( EIF_CALCULATED_FLAG | EIF_DIDDLE )
#define EIF_HIGHLIGHT			0x0008
#define EIF_DIDDLE				0x0010
#define EIF_DELAY_CALC			0x0020
#define	EIF_PRIVATE				0x0040
#define	EIF_NO_ENTER			0x0080
#define	EIF_NO_CALLBACK_ENTER	0x0100
#define	EIF_NO_CALLBACK_EXIT 	0x0200


// Also update table in editinfo.c
enum EDIT_INFO_EVENT
{
	EIE_FIELD_ENTER,	// Arg: CallBackInfo	-just entered
	EIE_FIELD_EXIT,		// Arg: CallBackInfo	-before exiting (no change)
	EIE_FIELD_CHANGE,	// Arg: CallBackInfo	-before exiting (w/ change)
	EIE_FIELD_KEY,		// Arg: CallBackInfo	-for each key
	EIE_FIELD_PRESSED,	// Arg: CallBackInfo	-EI_BUTTON pressed

	EIE_LOCATE,			// Arg: LocationInfo	-position field
	EIE_COPY,			// Arg: NULL			-copy CallBackArg
	EIE_DELETE,			// Arg: NULL			-delete CallBackArg

	EIE_DIALOG_ENTER,	// Arg: CallBackInfo	-before display
	EIE_DIALOG_EXIT,	// Arg: CallBackInfo	-before leaving
	EIE_DIALOG_KEY,		// Arg: CallBackInfo	-Undef key at top level

	EIE_LAST			// Placeholder, must be last enum
};


// Actions to be performed upon return from callback
// Also update table in editinfo.c
enum EDIT_INFO_ACTION
{
	EIA_DEFAULT,			// Perform default action (event not yet handled)
	EIA_NO_ACTION,			// Do not perform default (event already handled)
	EIA_DIALOG_EXIT,		// Exit successful	[F9]
	EIA_DIALOG_ABORT,		// Exit abort		[ESC]
	EIA_DIALOG_ABORT_ERROR,	// Exit abort due to software error
	EIA_ERROR_NOTIFY,		// Inform user of user error (beep or set field to red)
	EIA_ERROR_SILENT,		// Something did not work, but already handled
	EIA_DIALOG_CHANGED,		// Dialog data changed, restart editor
	EIA_DISABLE_EVENT,		// Disable this event, never call again

	EIA_LAST				// Placeholder, must be last enum
};


// Also update table in editinfo.c
enum EDIT_INFO_COLOR
{
	EIC_DEFAULT = 0,
	EIC_BLACK,
	EIC_BLUE,
	EIC_GREEN,
	EIC_CYAN,
	EIC_RED,
	EIC_MAGENTA,
	EIC_BROWN,
	EIC_WHITE,
	EIC_DARK_GREY,
	EIC_BRIGHT_BLUE,
	EIC_BRIGHT_GREEN,
	EIC_BRIGHT_CYAN,
	EIC_BRIGHT_RED,
	EIC_BRIGHT_MAGENTA,
	EIC_YELLOW,
	EIC_BRIGHT_WHITE,

	EIC_LAST				// Placeholder, must be last enum
};


// EDIT_INFO_CONFIG/Flags
#define EICF_RIGHT_JUST		0x0001
#define EICF_NO_DEFAULT		0x0002
#define EICF_NO_RETVAL		0x0004


/*
**	Callback function
*/

struct EDIT_INFO_CALL_BACK;

typedef EDIT_INFO_ACTION
		(EDIT_INFO_CALL_BACK_FUNC)(
				EDIT_INFO_CALL_BACK	*CallBack,
				EDIT_INFO_EVENT		Event,
				void				*EventArg );


/*
**	Structures
*/

// from kool_ade/outform.h
struct OUTPUT_FORMAT;

// Structure contents defined in GSD.H
struct GSD_DRIVER;
struct GSD_DIALOG;
struct GSD_FIELD;
struct GSD_FIELD_ARRAY;


struct EDIT_INFO_CALL_BACK
{
	EDIT_INFO_CALL_BACK_FUNC
			*Func;

	void	*Arg;

};


struct EDIT_INFO
{
	const char
			*ValueTypeName,	// Can be NULL if Type == EI_TITLE
			*Prompt;		// If NULL, use ValueTypeName

	EDIT_INFO_TYPE
			Type;

	short	Flags;

	short	PromptX,		// Absolute, (-1,-1) -> automatically computed
			PromptY,
			DataX,			// Relative, ( 0, 0) -> automatically computed
			DataY,			//    [ relative to PromptX, PromptY ]
			DataWidth,		// 0 -> Use DataType default value
			DataHeight;		// 0 -> Use DataType default value

	const void
			*Extra;			// Extra information (string table, ...)

	EDIT_INFO_CALL_BACK
			CallBack;

	short	UniqueId;		// Used by EditTable functions

};


#define EI_EXTRA_PAD	,{NULL,NULL},0

#define EIP_DEFAULT		-1
#define EIP_CENTER		-2
#define EIP_LEFT		-3
#define EIP_RIGHT		-4
#define EIP_TOP			-5
#define EIP_BOTTOM		-6
#define EIP_FIT			-7
#define EIP_MAX			-8

struct EDIT_INFO_POSITION
{
	int		Flags;			// EIPF_xxx flags

	int		XPos,			// Use EIP_xxx for special values
			YPos,			// or >= 0 for absolute values
			XSize,
			YSize;

};


struct EDIT_INFO_CONFIG
{
	int		Flags;	// EICF_xxx flags

	EDIT_INFO_COLOR
			WindowFg,
			WindowBg,
			WindowBorderFg,
			WindowBorderBg,
			WindowTitleFg,
			WindowTitleBg,
			WindowSubtitleFg,
			WindowSubtitleBg,
			TitleFg,
			TitleBg,
			PromptFg,
			PromptBg,
			FieldFg,
			FieldBg,
			CleanFg,
			CleanBg,
			EditedFg,
			EditedBg,
			ErrorFg,
			ErrorBg,
			UpdatedFg,
			UpdatedBg,
			DiddleCleanFg,
			DiddleCleanBg,
			DiddleEditedFg,
			DiddleEditedBg,
			DiddleErrorFg,
			DiddleErrorBg,
			DiddleUpdatedFg,
			DiddleUpdatedBg,
			ReadOnlyFg,
			ReadOnlyBg;

	FKEYS	*DialogFKeys,	// Displayed for entire dialog
			*FieldFKeys;	// Displayed only for fields created after

	EDIT_INFO_CALL_BACK
			DialogCallBack;	// CallBack for dialog events

};


struct EDIT_TITLE_FORMAT
{
	int		Flags;		// EITF_xxx flags

	char	TitleFg,	// Overrides anything set in EI_CONFIG
			TitleBg;

	char	FillChar,	// Fill character for EITF_CENTER
			GapCenter,	// Number chars gap around title
			GapEdge;	// Number chars gap at edge of dialog

};


struct EDIT_INFO_LOCATION
{
	int		MaxWidth,		// Maximum width of a prompt (including space)
			AltMaxWidth,
			LineNumber,		// Next line number
			MaxX,			// For determining the max size of dialog
			MaxY,
			OriginX,	  	// Added to X and Y positions
			OriginY;

	EDIT_INFO_CONFIG
			*Config;

};


struct EDIT_INFO_INDEX
{
	const char
			*Component;		// ValueType or Component name

	int		Element;		// Array element (if Component==NULL)
							// has value -1 otherwise, -2 for last element
};


struct EDIT_INFO_CALL_BACK_INFO
{
	SDB_OBJECT
			*SecPtr;		// Security being editted

	DT_VALUE
			*InitialValue,	// Initial structure value for Dialog()
			*ReturnValue;	// Current/return value for Dialog()

	int		Key;			// For EIA_FKEY / EIA_FIELD_KEY

	GSD_DIALOG
			*GsdDialog;

	GSD_FIELD
			*GsdField;

	EDIT_INFO
			*EditInfo;		// Same as GsdField->EditInfo

	char	*FieldValue;	// For EIA_FIELD_CHANGE

	void	*UserData;		// From SECEDIT_DIALOG/_DEFAULTS -> UserData

	EDIT_INFO_INDEX			// IndexArray[IndexSize].Component==NULL
			*IndexArray;	// IndexArray[IndexSize].Elem==-2

};


/*
**	Accessor macros for EDIT_INFO/.Extra
*/

#define EIM_BLOCK_NAME(E)		((const char *)(E)->Extra)
#define EIM_BLOCK_NAME_MOD(E)	((char *)(E)->Extra)
#define EIM_BLOCK_NAME_SET(E,N)	((E)->Extra=N)
#define EIM_BLOCK_NAME_FREE(E)	(free( EIM_BLOCK_NAME_MOD((E)) ), EIM_BLOCK_NAME_SET( (E),NULL ))

#define EIM_STRTAB_NAME(E)			EIM_BLOCK_NAME(E)
#define EIM_STRTAB_NAME_SET(E,N)	EIM_BLOCK_NAME_SET(E)
#define EIM_STRTAB_NAME_FREE(E)		EIM_BLOCK_NAME_FREE(E)

#define EIM_CONFIG(E)			((EDIT_INFO_CONFIG*)(E)->Extra)
#define EIM_CONFIG_SET(E,C)		((E)->Extra = (C))

#define EIM_POSITION(E)			((EDIT_INFO_POSITION*)(E)->Extra)
#define EIM_POSITION_SET(E,P)	((E)->Extra = (P))

#define EIM_EDITINFO(E) 		((EDIT_INFO*)(E)->Extra)
#define EIM_EDITINFO_SET(E,S)	((E)->Extra = (S))

#define EIM_FORMAT(E) 			((OUTPUT_FORMAT*)(E)->Extra)
#define EIM_FORMAT_SET(E,F)		((E)->Extra = (F))

#define EIM_SECCLASS(E)			((const char *)(E)->Extra)
#define EIM_SECCLASS_MOD(E)		((char *)(E)->Extra)
#define EIM_SECCLASS_SET(E,S)	((E)->Extra = (S))

#define EIM_STRTAB(E)			(*(STRING_TABLE **)(E)->Extra)
#define EIM_STRTAB_MOD(E)		(*(STRING_TABLE_MOD **)(E)->Extra)
#define EIM_STRTAB_SET(E,S)		(*(STRING_TABLE **)(E)->Extra = (S))
#define EIM_STRTABPTR_FREE(E)	(free((void*)(E)->Extra),(E)->Extra=NULL)
#define EIM_STRTABPTR_ALLOC(E)	((E)->Extra = calloc( 1, sizeof(STRING_TABLE **) ))

#define EIM_CALL_BACK(CB,V,A)	((CB).Func ? (*(CB).Func)(&(CB),(V),(A)) : EIA_DEFAULT )

/*
**	EDIT_INFO_INIT structure
*/

struct EDIT_INFO_INIT;

typedef void (*EDIT_INFO_INIT_FUNC)( EDIT_INFO_INIT *Self, int ForceReinit);

struct EDIT_INFO_INIT
{
	EDIT_INFO					*Array;

	EDIT_INFO_INIT_FUNC			InitFunc;	// Call before using Array

	int							InitVar;	// To be used by InitFunc

	DT_ARRAY                    *SdbArray;	// Used by EditInfoValueHelper

};

// EDITINFO.C

DLLEXPORT int
		DtFuncEditInfo( int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b );

DLLEXPORT void
	 	EditInfoStart( void );

DLLEXPORT EDIT_INFO
		*EditInfoNew( 			void ),
		*EditInfoCopy( 			const EDIT_INFO *EditInfo, EDIT_INFO *Copy ),
		*EditInfoCopyArray(		const EDIT_INFO *EditInfoArray ),
		*EditInfoConvertArray(	DT_VALUE *ArrayValue, SLANG_SCOPE *Scope, short *UniqueCounter );

DLLEXPORT void
		EditInfoDelete(			EDIT_INFO *EditInfo, int FreeSelf ),
		EditInfoDeleteArray(	EDIT_INFO *EditInfoArray ),
		EditInfoFillArray(		DT_VALUE *ArrayValue,  EDIT_INFO *EditInfo ),
		EditInfoValueHelper(	DT_VALUE *ReturnValue, EDIT_INFO_INIT *EditInfoInit ),
		EditInfoFillIn(			EDIT_INFO *FinalInfo, const EDIT_INFO *BaseInfo, const EDIT_INFO_LOCATION *LocationInfo );

DLLEXPORT int
		EditInfoConvertElement(	DT_VALUE *Value, EDIT_INFO *EditInfo, SLANG_SCOPE *Scope, short *UniqueCounter );

DLLEXPORT int
		EditInfoPosFromStruct(	DT_VALUE *Value, EDIT_INFO_POSITION *Pos );

DLLEXPORT const char
		*EditInfoPrompt( 		const EDIT_INFO *EditInfo );

DLLEXPORT const char
		*EditInfoActionName(	EDIT_INFO_ACTION	Action	),
		*EditInfoColorName(		EDIT_INFO_COLOR		Color	),
		*EditInfoEventName(		EDIT_INFO_EVENT		Event	),
		*EditInfoTypeName(		EDIT_INFO_TYPE		Type	);

DLLEXPORT int
		EditInfoType(			const EDIT_INFO *EditInfo ),
		EditInfoFlags(			const EDIT_INFO *EditInfo );

DLLEXPORT void
		EditInfoInitInit(		EDIT_INFO_INIT *EII, int Force ),
		EditInfoInitDefault(	EDIT_INFO_INIT *EII, int Force );

DLLEXPORT EDIT_INFO_ACTION
		EditInfoSlangCallBack(	EDIT_INFO_CALL_BACK *CallBack, EDIT_INFO_EVENT Event, void *EventArg );

DLLEXPORT int
		EditInfoUpdateStrTab(	EDIT_INFO_CALL_BACK_INFO *CallBackInfo, STRING_TABLE_MOD *StrTab );

DLLEXPORT int
		EditInfoFormatValue(	EDIT_INFO *EditInfo, DT_VALUE *Value, char *Buffer );

DLLEXPORT int
		EditInfoContainsDialogCallBack(	EDIT_INFO *EditInfo );

// EIFKEYS.C

DLLEXPORT FKEYS
		*FKeysConvert(			DT_ARRAY *Array ),
		*FKeysCopy(				FKEYS *FKeys );

DLLEXPORT void
		FKeysDelete(			FKEYS *Fkeys );

DLLEXPORT int
		FKeysToArray(			FKEYS *FKeys, DT_VALUE *Value );


// EISTRTAB.C

DLLEXPORT STRING_TABLE_MOD
		*StringTableClasses,			// All SecDb object classes
		*StringTableCompany,			// Aron, Aron-TOK, Aron-LDN
		*StringTableCurrency,			// USD, DEM, ... (Metals too)
		*StringTableLocation;			// NYC, TKO, ...

DLLEXPORT STRING_TABLE
		*StringTableBestWorst,			// Best, Worst
		*StringTableBondQuote,			// "Pct of Par/Clean", etc.
		*StringTableBookAliasPrefix,	// BA, BAO
		*StringTableCategory,			// GAM, EXC, EI, CUS, FWD, ADJ, (Trades)
		*StringTableCompoundFreq,		// Continuous, Simple, ...
		*StringTableContinent, 			// Europe, North America, ...
		*StringTableCurveType,			// "Forward, European style", ...
		*StringTableDayCount,			// "Actual/365", "Actual/Actual", ...
		*StringTableDelayImm,			// "", "Delayed", "Immediate"
		*StringTableDownUp,				// Down, Up
		*StringTableFrequency,			// Daily, Weekly, ... (see spotavg.c)
		*StringTableGsDateDayCount,		// adc, anldc, isda...
		*StringTableGsDateDaysPerTerm,	// 360dpt, 365dpt, 365_25dpt, ...
		*StringTableGsDateInvalidIncr,	// ldom, fdonm, ndonm
		*StringTableGsDateMonthIncr,	// pdom, pdomaeom, pwom
		*StringTableGsDateRollRef,		// noroll, roll
		*StringTableGsDateRoundingConv,	// nbd, nbdsm, nr
		*StringTableGsDateTermFormula,	// fptf,vptf,nptf,siatf
		*StringTableInOut,				// In, Out
		*StringTableOptStyle,			// European, American
		*StringTableOptType,			// Call, Put
		*StringTableOptTypeAndFwd,		// Call, Put, Fwd
		*StringTablePayment,			// Physical, Cash
		*StringTablePricingModel,		// Quick and Dirty, Standard, Robust
		*StringTableTradeState,			// New, ... N/A
		*StringTableTradeType,			// Buy, Sell, ...
		*StringTableTraderType;			// Trader, SalesPerson, Developer ...

DLLEXPORT void
		StringTableCreateDefault(	int Recreate ),
		StringTableDelete(			STRING_TABLE_MOD *StringTable ),
		StringTableSort(			STRING_TABLE_MOD *StringTable );

DLLEXPORT STRING_TABLE_MOD
		*StringTableCopy(			STRING_TABLE *StringTable ),
		*StringTableCreate(			SDB_DB *Db, const char *SecDbClass,  int InitialEmpty, int Preload ),
		*StringTableCreateMult(		SDB_DB *Db, STRING_TABLE *SecDbClasses, int InitialEmpty, int Preload ),
		*StringTableFromArray(		DT_VALUE *ArrayValue );

DLLEXPORT int
		StringTableLength(			STRING_TABLE *Table ),
		StringTableToArray(			STRING_TABLE *Table, DT_VALUE *ArrayValue );


// EIFORMAT.C

DLLEXPORT OUTPUT_FORMAT
		EIFormatStandard,			// .6 Concise + Commas + Right + Blank Zero
		EIFormatM,					// .3 Standard + Scale
		EIFormat0,					// .0 Standard - Concise
		EIFormat0L,					// .0 Left
		EIFormat0R,					// .0 Right + Blank Zero
		EIFormat0LC,				// .0 Left  + Commas
		EIFormat0RC,				// .0 Right + Commas + Blank Zero
		EIFormat1,					// .1 Standard - Concise
		EIFormat2,					// .2 Standard
		EIFormat4,					// .4 Standard - Concise
		EIFormat6,					// .6 Standard - Concise
		EIFormatPercent;			// .3 Percent + Concise + Right + Blank Zero

#endif
