/* $Header: /home/cvs/src/tap/src/tap.h,v 1.54 2011/11/19 17:12:07 e63100 Exp $ */
/****************************************************************
**
**	TAP.H
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.54 $
**
****************************************************************/

#ifndef IN_TAP_H
#define IN_TAP_H

#ifndef _IN_FORM_H
#include <form.h>
#endif
#ifndef _IN_TAPSKEY_H
#include <tapskey.h>
#endif

/*
** Forward declarations
*/

struct WINDOW;
struct EDIT_INFO;

/*
**	Define constants
*/

#define	TAP_STRING_AREA_SIZE       8192
#define SALES_ALLOCATION_SCREEN	   "Sales Breakdown"
#define SALES_GROUP				   "Metal Sales Group"

/*
**	Value type
*/

struct TAP_VALUE;
typedef TAP_VALUE *TAP_VALUE_TYPE;

/*
**	Define login structure
*/

#define	USER_PRIVILEGE_ORDINARY	   	0
#define	USER_PRIVILEGE_SUPER_USER	1

typedef int TapUserPrivilege;


struct TAP_USER
{
	char	*Trader,		// Trader doing trades during this session
			*Location,		// Location of trading during this session
			*DataLocation,	// Location to use for market data diddles
			*Company,		// Company of trader
			*Group,			// Group of trader
			*Preferences;	// Preferences portfolio

	TapUserPrivilege
			Privilege;		// What privilege level the user has.
};



/*
**	Define list box selection structure
*/

struct TAP_SELECTION
{
	struct MemPoolStructure
			*MemPool;

	char	*Trader,		// Select only trades by this trader
			*Location,		// Select only trades at this location
			*Class;			// Select only trades where the security traded
							//		is of this class

	time_t	Since,			// Select only trades with creation time on or
							//		after this time
			Before;			// Select only trades with creation time before
							//		this time


	TAP_VALUE_TYPE
			*SortList;		// Array of value types on which to sort --
							// 		sorts are ascending with comparison based on
							// 		the data type of each value type

	int		SortCount,		// Number of items in SortList
			SelectionCount;	// Number of value types needed for given selection
							// criteria -- filled in by TapValueMapsFixup

	struct HashStructure
			*ValueMapHash,	// Filled in by TapValueMapsFixup
			*SelSortHash;	// Filled in by TapValueMapsFixup

	TAP_VALUE
			**SelSortValues;// Filled in by TapValueMapsFixup

	unsigned
			PendingTrades:1;// Set if all trades are all inserted manually and
							// not selected from the database


};



typedef int TAP_MSG;

#define		TAP_MSG_DATA_TO_FIELD		1
#define		TAP_MSG_DATA_FROM_FIELD		2
#define		TAP_MSG_APPLY_DEFAULT		3
#define		TAP_MSG_FREE				4
#define		TAP_MSG_ERROR_SENTINEL		5



struct TAP_DATA;
typedef TAP_DATA _huge *TAP_DATA_PTR;
typedef int  (*TAP_FIELD_FUNC)	( FORM *, FIELD * );
typedef void (*TAP_VFIELD_FUNC)	( FORM *, FIELD * );
typedef int	 (*TAP_KEY_FUNC)	( FORM *, FIELD *, int );
typedef int	 (*TAP_DATA_FUNC)	( FIELD *, TAP_DATA_PTR, TAP_MSG );
typedef void (*TAP_MESSAGE_FUNC)( const char *Format, ... );


/*
**	Define data structure to hold each value type (and its associated string
**	table, if applicable).
*/

struct TAP_VALUE
{
	const char
			*Name,				// Name as displayed throughout tap
			*ValueName;			// Actual SecDb value type name -- NULL means same as Name

	int		EditFlags;

	int		Public;				// TRUE if selectable by user

	TAP_KEY_FUNC
			InputFunc;

	TAP_VFIELD_FUNC
			PosFunc;

	int		EditWidth,
			FormFieldType;

	TAP_DATA_FUNC
			DataFunc;

	const void
			*FieldData;			// e.g. string table or class name

	void	*Arg;

	SDB_VALUE_TYPE
			ValueType;

	DT_VALUE_RESULTS
			ErrorSentinel;

	TAP_SORT_KEY
			*SortKey;

};



/*
**	Define data structure to explain where to put a value in an edit box.
*/

struct TAP_EDIT_VALUE
{
	char	*Name;

	int		EditPromptX,
			EditPromptY,
			EditDataX,
			EditDataY,
			EditWidth,
			EditHeight;

	TAP_VALUE
			*Value;

};



/*
**	Define data structure to hold value mappings for each SecDb class.
*/

struct TAP_VALUE_MAP
{
	char	*ClassName;

	int		ValuePtrCount;		// Number of pointers in ValuePtrs

	TAP_VALUE
			**SelectValues,		// Array of pointers into a Value Table for
								// each of the selection criteria value types

			**SortValues,		// Array of pointers into a Value Table for
								// each of the sort criteria value types

			**OtherValues,		// Array of pointers into a Value Table for
								// all the other values in ValueTable not
								// represented by SelectValues or SortValues

			*ValuePtrs[ 1 ];	// The SelectValues, SortValues, and
								// OtherValues arrays
};



/*
**	Define data structure to hold the actual data value instances for a
**	given trade.
*/

struct TAP_DATA
{
	DT_VALUE_RESULTS
			Data;				// The data for this trade/value type
								// returned by SecDbGetValue

	TAP_VALUE
			**ValuePtr;			// Pointer to the corresponding element in
								// the ValuePtrs array.

	struct TapTradeStructure _huge
			*Trade;				// The corresponding trade.

};



/*
**	Define data structure for storing the list of selected trades
**	retrieved for SecDb.
*/

typedef struct TapTradeStructure
{
	SDB_OBJECT
			*SecPtr;			// SecPtr for the Trade object

	TAP_VALUE_MAP
			*ValueMap;			// ValueMap for the SecurityTraded's SecDb Class

	TAP_DATA_PTR
			DataTable;			// Points to table of data values for each
								// of the types in ValueMap->SortValues
								// followed by the types in ValueMap->Other

	int		DataCount;			// Number of data values filled in DataTable.

	int     ExternalTradeIdCount;  // Number of trades with this External Trade Id

	unsigned
			Modified:1,		   	// Trade has been modified with set values.
			New:1,				// Trade is new
			Padding:14;			// Padding so sizeof( TAP_TRADE ) is a power of two

} TAP_TRADE, _huge *TAP_TRADE_PTR, _huge * _huge *TAP_TRADE_PTR_PTR;


/*
**	Define data structure to keep track of all selected trades and their
**	associated values.
*/

struct TAP_TRADE_LIST
{
	struct MemPoolStructure
			*MemPool;

	long	TradeCount,			// Number of trades in TradeTable and TradePtrTable
			DataCount,			// Number of values in DataTable
			TradeAllocCount,	// Number of trades and tradeptrs allocated
			DataAllocCount;		// Number of items allocated in datatable

	TAP_TRADE_PTR
			TradeTable;			// Array of trades

	TAP_TRADE_PTR_PTR
			TradePtrTable;		// Sorted array of pointers to trades

	TAP_DATA _huge
			*DataTable;

	int		IncompleteFlag:1,	// Set if aborted during selection
			UnsortedFlag:1;		// Trades were inserted without sorting

	char	RefreshTimeString[ 26 ];		// Time last refreshed

};



/*
**	Define a display field.
*/

typedef struct field_struct TAP_FIELD;



/*
**	Define a structure to keep track of all display fields.
*/

struct TAP_FIELD_LIST
{
	struct MemPoolStructure
			*MemPool;

	int		FieldCount;

	TAP_FIELD
			*FieldTable;

	char	*StringArea;

};



/*
**	Structure used for viewing trade list
*/

struct TAP_VIEW_CONTEXT
{
	TAP_SELECTION
			*TapSelection;

	TAP_TRADE_LIST
			*TradeList;

	TAP_FIELD_LIST
			*FieldList;

	WINDOW
			*Wnd,
			*UpdateBar;

	long	CurTrade,
			TopTrade,
			ScanningTradeNum;

	int		Column;

};



/*
**	Define structure for a table of class information
*/

struct TAP_CLASS_MAP
{
	char	*ClassName;

	int		Tradeable;

	char	**ValueTypesStrTab;

	TAP_EDIT_VALUE
			*EditValues;

	int		ValueCount;

};


/*
**	Define structure containing information from each "Tradable Class"
**	object.
*/

struct TAP_TRADABLE_ELEMENT
{
	char	*DisplayedName,
			*TypeOfSecurityTraded,
			*EditTableName;

	DT_VALUE
			EditArray;

};


/*
**	Access global tap user data
*/

DLLEXPORT TAP_USER
		TapUserGlobal;

extern DT_VALUE
		*TapReportProfiles;	// DtArray of Profiles from Preferences


/*
**	Declare function prototypes
*/


// TAPSTART.C

DLLEXPORT int		TapStartup				( char *DbName, char *SourceDbName );
DLLEXPORT void	TapShutdown				( void );
DLLEXPORT int	TapSlang				( void );


// TAPUSER.C

DLLEXPORT int		TapUserEdit				( TAP_USER *TapUser );

DLLEXPORT int 
		TapUserGetAuthorizedUserName( 	char **UserName ),
		TapUserGetUserName(				char **UserName );

DLLEXPORT  
		int TapUserGetPrivilege( const char *UserName, TapUserPrivilege *Privilege );


// TAPCRIT.C

DLLEXPORT int		TapSelectionEdit		( TAP_SELECTION *TapSelection );

char 	**TapCreateClassesStringTable
								( struct MemPoolStructure *MemPool );

// TAPABORT.C

DLLEXPORT int		TapAbortCheck			( void );
DLLEXPORT void	TapAbortPush			( void ),
		TapAbortPop				( void );

// TAPVIEW.C

DLLEXPORT int		TapView					( TAP_VIEW_CONTEXT *Ctx, long NewLineNumber, int ProcessKeys );

// TAPVIEWT.C

DLLEXPORT int		TapViewTrades			( TAP_SELECTION *TapSelection );

void	TapViewTradesRefresh	( int ReloadDb );
int		TapRunAllocationScreen 	( TAP_TRADE_PTR	Trade );

// TAPBOX.C

DLLEXPORT TAP_FIELD_LIST
		*TapFieldsCreate		( int FieldCount );

DLLEXPORT void	TapFieldsDestroy		( TAP_FIELD_LIST *FieldList ),
	 	TapFieldsEditRelease	( TAP_FIELD *StartField, TAP_EDIT_VALUE *EditValues, TAP_DATA_PTR DataTable );

//int	TapViewEditBox			( TAP_VIEW_CONTEXT *Ctx, TAP_EDIT_VALUE *EditValues, TAP_TRADE_PTR Trade, long CurTrade, char *ClassName );

DLLEXPORT int		TapViewEditTrade		( TAP_VIEW_CONTEXT *Ctx, TAP_TRADE_PTR Trade, long CurTrade, char *ClassName );
int		TapDeveloper			( char *FunctionName ),
		TapTradeReload			( TAP_TRADE_PTR Trade ),
		SecListReload			( DT_VALUE *SecList, SDB_DB *Db, unsigned Flags, char *SecNameToExclude ); 

void	TapDeveloperClear		( void );


// TAPTRADE.C

DLLEXPORT TAP_TRADE_LIST
		*TapTradesSelect		( TAP_SELECTION *TapSelection, TAP_MESSAGE_FUNC Message, TAP_MESSAGE_FUNC WMessage );

DLLEXPORT void	TapTradesReselect		( TAP_TRADE_LIST *TradeList, TAP_SELECTION *TapSelection ),
		TapTradesFree			( TAP_TRADE_LIST *TradeList, TAP_SELECTION *TapSelection ),
		TapTradeInsert			( TAP_TRADE_LIST *TradeList, TAP_TRADE_PTR Trade, long TradeNumber );

DLLEXPORT TAP_TRADE_PTR
		TapTradeNew				( TAP_TRADE_LIST *TradeList, char *ClassName, TAP_SELECTION *TapSelection, SDB_OBJECT *OptionalSecPtr );

DLLEXPORT int		TapTradeRelease			( TAP_TRADE_LIST *TradeList, TAP_TRADE_PTR Trade, TAP_SELECTION *TapSelection ),
		TapTradeDelete			( TAP_TRADE_LIST *TradeList, long TradeNumber, int DeleteFromSecDb ),
		TapTradeCmp				( void _huge *TradeXPtrPtr, void _huge *TradeYPtrPtr );


// TAPVTAB.C

DLLEXPORT char	**TapEditValuesStrTab	( void );

int 	TapClassMapInit			( TAP_CLASS_MAP *TapClass );


TAP_CLASS_MAP
		*TapClassMapFromName	( char *ClassName );


// TAPPREF.C

DLLEXPORT void	TapSetUser				( TAP_USER *User );

DLLEXPORT const char
		**TapValueTypesStrTab	( void );

TAP_TRADABLE_ELEMENT
		*TapTradableFromDisplayedName	( char *Name ),
		*TapTradableFromClassName		( char *Name );

DT_VALUE
		*TapPrefEditArray		( char *ClassName );


// TAPVMAP.C

DLLEXPORT int		TapValueMapsStartup		( void );

DLLEXPORT void	TapValueMapsShutdown 	( void ),
	 	TapValueMapCreateStrings( void );

DLLEXPORT TAP_VALUE_TYPE
		TapValueTypeFromName	( const char *Name );

DLLEXPORT const char
		*TapValueNameFromType	( TAP_VALUE_TYPE Type );

DLLEXPORT int		TapValueMapsFixup		( TAP_SELECTION *TapSelection );
void	TapValueMapsCleanup		( TAP_SELECTION *TapSelection );

DLLEXPORT TAP_VALUE_MAP
		*TapValueMapFind		( char *ClassName, TAP_SELECTION *TapSelection );

DLLEXPORT TAP_EDIT_VALUE
		*TapValueMapEditValues	( char *ClassName );

DLLEXPORT int		TapValueMapValuePtrs	( struct MemPoolStructure *MemPool, TAP_EDIT_VALUE *EditValues, TAP_VALUE ***ValuePtrs );

void	TapValueMapEditInfoDump	( TAP_EDIT_VALUE *Value, char *Buffer );

TAP_VALUE_TYPE
		TapValueAdd				( TAP_VALUE *Value ),
		TapValueAddByEditInfo	( EDIT_INFO *EditInfo );


// TAPDATA.C

DLLEXPORT int		TapDataFill				( TAP_TRADE_PTR Trade, TAP_DATA_PTR DataTable, TAP_VALUE **ValuePtrs, int ValueCount ),
		TapDataSet				( TAP_TRADE_PTR Trade, TAP_DATA_PTR DataTable, TAP_VALUE **ValuePtrs, int ValueCount, int OverrideFlag ),
		TapDataApplyDefaults	( TAP_TRADE_PTR Trade, TAP_DATA_PTR DataTable, TAP_VALUE **ValuePtrs, int ValueCount ),
		TapDataCopyValues		( TAP_DATA_PTR Target, TAP_DATA_PTR Source, TAP_VALUE **ValuePtrs, int ValueCount ),
		TapDataFreeValues		( TAP_DATA_PTR DataTable, TAP_VALUE **ValuePtrs, int ValueCount ),
		TapDataReload			( TAP_TRADE_PTR Trade );


// TAPEXER.C

DLLEXPORT int		TapExercise				( void );
DLLEXPORT int		TapExerciseWithParam	( DATE NextTransDate, char *PortSelection, char *Location, DT_ARRAY *Tradables, int TapExerciseBatchFlag, int Stop_Show_Results );

// TAPREP.C

int		TapPrintReports			( TAP_VIEW_CONTEXT *Ctx, TAP_TRADE_PTR Trade ),
		TapScriptEvaluate		( char *ScriptName ),
		TapPrintReportProfile	( TAP_VIEW_CONTEXT *Ctx, TAP_TRADE_PTR Trade, SDB_OBJECT *SecPtrProfile );

void	TapViewOldReports		( void ),
		TapViewProcmonReports	( void );

#if defined( IN_SECEXPR_H )
void	TapRepHotKeyEvaluator	( SLANG_NODE *Root, char *ScriptName );
#endif

#endif
