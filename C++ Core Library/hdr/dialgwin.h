/****************************************************************
**
**	dialgwin.h
**
**	Copyright 1994-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/dialog/src/dialgwin.h,v 1.15 2012/03/16 18:20:22 e19351 Exp $
**
****************************************************************/

#if !defined( IN_DIALOGWIN_H )
#define IN_DIALOGWIN_H


/*
**	Define constants
*/

// Style add-ons for static control (static plus)
#define	SSS_INDENT					0x00010000L
#define	SSS_EMBOSS					0x00020000L
#define	SSS_SHADOW					0x00040000L

// Styles for separator class
#define	SEP_STYLE_VERTICAL			0x00000001L
#define	SEP_STYLE_HORIZONTAL		0x00000002L

// Notifications from separators
#define	SEP_NOTIFY_MOVE_START		1
#define	SEP_NOTIFY_MOVING			2
#define	SEP_NOTIFY_MOVE_END			3

#define	DLG_WIN_STARTING_ID			100


// FIX - THIS PROBABLY DOESN'T BELONG HERE
#define	WM_USER_SET_DATA			(WM_USER + 1001)
#define	WM_USER_SET_MODE			(WM_USER + 1002)

#define	WM_USER_DLG_INITIALIZE 		(WM_USER + 1010)
#define	WM_USER_DLG_QUIT			(WM_USER + 1011)
#define	WM_USER_DLG_DISMISS			(WM_USER + 1012)
#define	WM_USER_DLG_MESSAGE			(WM_USER + 1013)
#define	WM_USER_MENU_SET_TABLE		(WM_USER + 1014)
#define	WM_USER_MENU_SET_SUBTABLE	(WM_USER + 1015)
#define	WM_USER_MENU_QUERY_STATE	(WM_USER + 1016)
#define	WM_USER_MENU_SET_STATE		(WM_USER + 1017)
#define	WM_USER_SET_ICON			(WM_USER + 1018)


typedef struct DlgWinWidgetData
{
	HFONT	Font;						// font, if we own it

	HFONT	CurrFont;					// used to store font because Windows doesn't seem to keep it

} DLG_WIN_WIDGET_DATA;

/*
**	Structure used to initialize the WIN driver
*/

typedef struct DlgWinDriverDataStructure
{
	HINSTANCE hInstance;

	HWND	hwndDesktop,
			hwndHourglass;

	char	*FontNameSize;

	int		HourglassState;

} DLG_WIN_DRIVER_DATA;


/*
**	Structure used to hold data for a frame or dialog window
*/

typedef struct DlgWinDataStructure
{
	short	MenuIDCounter;

	struct MemPoolStructure
			*MemPool;

	struct HashStructure
			*MenuHash;

	DT_VALUE
			*ReturnedValue;

	DLG_HANDLE
			DlgHandle;		// normally the window ID but not for frames

	DLG_HANDLE
			DefPushButtonHandle,
			CancelButtonHandle,
			MenuHandle;

	int		DefPushButtonID;

} DLG_WIN_DATA;


/*
**	Structure used to hold menu information
*/

typedef struct DlgWinMenuDataStructure
{
	int		ID;

	DLG_HANDLE
			MenuHandle;

	DT_VALUE
			Tag;

	HMENU	hMenu;

} DLG_WIN_MENU_DATA;


/*
**	Structure used to store menu widget info
*/

struct DLG_WIN_MENU_WIDGET
{
	DLG_HANDLE
			FrameHandle;

	HMENU	hMenu;
};


/*
**	Structure used to pass menu item create information to the window proc
*/

typedef struct DlgWinMenuMsgDataStructure
{
	HWND	hwndMenu;

	DLG_WIDGET
			*MenuWidget;

	DT_VALUE
			*Arguments;

} DLG_WIN_MENU_MSG_DATA;


/*
**	Structure used to hold widget creation data
*/

typedef struct DlgWinCreateDataStructure
{
	DLG_WIDGET
			*Parent;

	unsigned long
			Style;

	short	Xlow,
			Ylow,
			Xsize,
			Ysize;

} DLG_WIN_CREATE_DATA;


/*
**	Structure used to match Dlg flags to WIN flags
*/

typedef struct DlgWinStyleMapStructure
{
	unsigned long
			DlgStyle,
			WinStyle;

} DLG_WIN_STYLE_MAP;


/*
**	Structure used to pass dialog handle and value through a LPARAM
*/

typedef struct DlgUserMsgInfoStructure
{
	DLG_HANDLE	hDlg;
	DT_VALUE	Value;

} DLG_USER_MSG_INFO;


/*
**	function to dispatch window messages
*/

typedef void (*DlgWindowMessageDispatchFunc)();


/*
**	Variables
*/

extern DLG_CLASS_NUMBER
		ClassNumButton,
		ClassNumChart2d,
		ClassNumChart3d,
		ClassNumConstant,
		ClassNumDate,
		ClassNumEntryField,
		ClassNumFileView,
		ClassNumFloat,
		ClassNumFrame,
		ClassNumGlb,
		ClassNumGx,
		ClassNumInteger,
		ClassNumMenu,
		ClassNumScrollBar,
		ClassNumSecPick,
		ClassNumSeparator,
		ClassNumSlider,
		ClassNumStringTable,
		ClassNumTextBox,
		ClassNumTimer;

DLLEXPORT DT_DATA_TYPE
		DtWidget;


/*
**	Helper macros
*/

#define	WinYToGxY( WndData, Y )		( (WndData)->ClientRect.bottom - 1 - (Y) )
#define	GxYToWinY( WndData, Y )		WinYToGxY( (WndData), (Y) )

// since we are using MSVC it's ok to use inline functions

inline DLG_HANDLE DLG_HANDLE_FROM_HWND( HWND hWnd )
{
	DLG_WIN_DATA
			*DlgWinData;

	if( DlgWinData = (DLG_WIN_DATA*) GetWindowLong( hWnd, GWL_USERDATA ))
		return DlgWinData->DlgHandle;
	else
		return (DLG_HANDLE) GetWindowLong( hWnd, GWL_ID );
}

/*
**	Prototype functions
*/

LRESULT CALLBACK
		WndProcDlgHandler(		HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

BOOL CALLBACK
		DlgProcDlgHandler(		HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT CALLBACK
		WndProcSeparator(		HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT CALLBACK
		WndProcStaticPlus(		HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT CALLBACK
		WndProcStringTable(		HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

void	DlgWinWidgetDestroy(	DLG_WIDGET *Widget ),
		DlgWinHourglass(		DLG_WIDGET *Widget, int State );

DLLEXPORT void
		DlgWinAppendMessages(	DT_VALUE *Result, DLG_MSG *MsgListPtr );

DLLEXPORT unsigned long
		DlgWinGetStyle(			unsigned long Style, DLG_WIN_STYLE_MAP *StyleMap );

long	DlgWinColorDialog(		const char *Title, long StartingColor, int SolidColorOnly );

char	*DlgWinFileDialog(		DLG_WIDGET *Widget, const char *Title, const char *FileSpec, int SaveMode );

DLLEXPORT int
		DlgWinGetCreateData(	DT_VALUE *Args, DLG_WIN_CREATE_DATA *CreateData );

int		DlgWinWidgetCreate(		DLG_WIDGET *Widget, DLG_WIN_CREATE_DATA *CreateData, PSZ WinClassName, unsigned long Style, DT_VALUE *Results ),
		DlgWinWaitForQuit(	  	int Polling, DLG_PF_ABORT pfAbort, void *AbortArgs ),
		DlgWinWaitForDismiss(	DLG_WIDGET *ParentWidget, DLG_WIDGET *DialogWidget, int Destroy, DLG_PF_ABORT pfAbort, void *AbortArgs, DT_VALUE *Results ),
		DlgWinMapPoint(			DLG_WIDGET *WidgetFrom, DLG_WIDGET *WidgetTo, long *x, long *y ),
		DlgWinKeyLookup(		int WinKey, int KeyFlags );

HBITMAP	DlgCopyScreenToBitmap(	LPRECT );

int		DlgBitmapToGif(			HBITMAP, const char * );

int		DlgWinWindowSnapToGif(	const char *Gif );

DLLEXPORT int
		DlgWinDefaultHandler(	DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results );

DLLEXPORT void
		DlgWinWidgetInit(		HWND Handle );

int		DlgWinButton(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinChart2d(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinChart3d(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinConstant(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinDate(				DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinEntryField(		DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinFileView(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinFloat(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinFrame(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinGlb(				DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinGx(				DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinInteger(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinMenu(				DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinScrollBar(		DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinSecPick(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinSeparator(		DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinSlider(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinStringTable(		DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinTextBox(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinTimer(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinListBox(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results ),
		DlgWinComboBox(			DLG_WIDGET *Widget, int Command, DT_VALUE *Args, DT_VALUE *Results );
		
DLLEXPORT struct HashStructure
		*TopLevelWindowsHash;

DLLEXPORT DlgWindowMessageDispatchFunc
		DlgWindowMessageDispatcher;

DLLEXPORT DLG_WIN_MENU_DATA
		*DlgMenuFindFromTag( struct HashStructure *MenuHash, DT_VALUE *Tag ),
		*DlgMenuItemInsert( DLG_WIDGET *Widget, DLG_WIN_DATA *DlgWinData, DT_VALUE *Tag, int CommandID, HMENU hMenu ),
		*DlgMenuFindFromID( struct HashStructure *MenuHash, int ID );

DLLEXPORT int
		DlgMenuItemDelete( DLG_WIN_DATA *DlgWinData, DT_VALUE *Tag );

DLLEXPORT char
		*DlgMenuTagFromText( const char *Text );

#endif

