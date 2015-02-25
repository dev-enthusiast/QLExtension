/****************************************************************
**
**	DIALOG.H
**
**	Copyright 1994-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/dialog/src/dialog.h,v 1.37 2001/09/26 17:03:37 singhki Exp $
**
****************************************************************/

#if !defined( IN_DIALOG_H )
#define IN_DIALOG_H


/*
**	Driver types
*/

#define DLG_DRIVER_X				0
#define	DLG_DRIVER_PM				1
#define	DLG_DRIVER_WIN				2


/*
**	Miscellaneous constants
*/

#define	DLG_INVALID_HANDLE			0
#define	DLG_CLASS_NAME_SIZE			32
#define	DLG_DEFAULT_QUEUE_SIZE		128


/*
**	Error sources
*/

#define	DLG_ERR_SRC_DIALOG			0		// The DLG system itself
#define	DLG_ERR_SRC_DRIVER			1		// The window driver
#define	DLG_ERR_SRC_CALLBACK		2		// The app supplied callback


/*
**	Wait results
*/

#define	DLG_WAIT_NORMAL				0		// Timed out, etc.
#define	DLG_WAIT_QUIT				1		// Quit received
#define	DLG_WAIT_ERROR				2		// Error encountered
#define	DLG_WAIT_DISMISS			3		// Dismiss received


/*
**	Coordinate modes
*/

#define	DLG_COORD_MODE_AVG_CHAR		1
#define	DLG_COORD_MODE_AVG_PIXEL	2


/*
**	Define messages
*/

#define	DLG_MSG_BASE				4000

#define	DLG_MSG_UNDEFINED			 0
#define	DLG_MSG_GOT_FOCUS			 1
#define	DLG_MSG_LOST_FOCUS			 2
#define	DLG_MSG_CLICKED				 3
#define	DLG_MSG_DBL_CLICKED			 4
#define	DLG_MSG_CHANGE				 5
#define	DLG_MSG_ENTER				 6
#define	DLG_MSG_SCROLL				 7
#define	DLG_MSG_SELECT				 8
#define	DLG_MSG_ONE_UPLEFT	 		 9
#define	DLG_MSG_ONE_DOWNRIGHT		10
#define	DLG_MSG_MANY_UPLEFT	  		11
#define	DLG_MSG_MANY_DOWNRIGHT		12
#define	DLG_MSG_THUMB_TRACK			13
#define	DLG_MSG_THUMB_POS			14
#define	DLG_MSG_THUMB_DONE			15
#define	DLG_MSG_KEY					16
#define	DLG_MSG_CLOSE				17
#define	DLG_MSG_MENU				18
#define	DLG_MSG_DESTROY				19
#define	DLG_MSG_DRAW				20
#define	DLG_MSG_MEASURE				21
#define	DLG_MSG_SIZE				22
#define	DLG_MSG_CREATE				23
#define	DLG_MSG_CLASS_INIT			24
#define	DLG_MSG_TIMER				25
#define	DLG_MSG_SHOW_LIST			26

#define	DLG_MSG_SET_TEXT			100
#define DLG_MSG_SET_DATA			101
#define	DLG_MSG_QUERY_TEXT			102
#define	DLG_MSG_DISMISS				103
#define	DLG_MSG_QUERY_DATA			104
#define	DLG_MSG_QUIT				105
#define	DLG_MSG_SET_POS				106
#define	DLG_MSG_QUERY_POS			107
#define	DLG_MSG_ENABLE				108
#define	DLG_MSG_SHOW				109
#define	DLG_MSG_SET_FORECOLOR		110
#define	DLG_MSG_SET_BACKCOLOR		111
#define	DLG_MSG_QUERY_FORECOLOR		112
#define	DLG_MSG_QUERY_BACKCOLOR		113
#define	DLG_MSG_REFRESH				114
#define	DLG_MSG_SET_CHECK			115
#define	DLG_MSG_QUERY_CHECK			116
#define	DLG_MSG_DISPATCH_OFF		117
#define	DLG_MSG_DISPATCH_ON			118
#define	DLG_MSG_DISPATCH_RESET		119
#define	DLG_MSG_QUERY_FOCUS			120
#define	DLG_MSG_SET_FOCUS			121
#define	DLG_MSG_QUERY_SYS_VALUES	122
#define	DLG_MSG_SET_ZORDER			123
#define	DLG_MSG_QUERY_TEXT_SIZE		124
#define	DLG_MSG_QUERY_MESSAGES		125
#define	DLG_MSG_SET_FONT			126
#define	DLG_MSG_SET_VALUE			127
#define	DLG_MSG_QUERY_VALUE			128
#define	DLG_MSG_SET_FORMAT			129
#define DLG_MSG_SET_ACTIVE_WINDOW	130
#define DLG_MSG_SET_FOREGROUND_WINDOW	131
#define DLG_MSG_CAPTURE_TO_GIF		132
#define DLG_MSG_SET_MENU			133
#define DLG_MSG_UPDATE_MENU			134
#define DLG_MSG_DELETE_MENU_ITEM	135
#define DLG_MSG_INSERT_MENU_ITEM	136
#define	DLG_MSG_SET_ICON			137

// Entry fields
#define	DLG_EFM_SET_WIDTH			201
#define	DLG_EFM_QUERY_SELECT		202
#define	DLG_EFM_SET_SELECT			203

// Frames
#define	DLG_FRM_SET_ICON			251
#define	DLG_FRM_QUERY_BORDERS		252
#define	DLG_FRM_SET_TITLE_COLOR		253

// Menus
#define	DLG_MNM_SET_TABLE			301
#define	DLG_MNM_SET_SUBTABLE		302
#define	DLG_MNM_QUERY_STATE			303
#define	DLG_MNM_SET_STATE			304
#define DLG_MNM_GET_ITEM_BY_TAG		305
#define DLG_MNM_SET_ITEM			306

// Scroll bars
#define	DLG_SBM_QUERY_RANGE			501
#define	DLG_SBM_SET_RANGE			502

// Gx widgets
#define	DLG_GXM_BUTTON_1_DOWN		601
#define	DLG_GXM_BUTTON_2_DOWN		602
#define	DLG_GXM_BUTTON_1_UP			603
#define	DLG_GXM_BUTTON_2_UP			604
#define	DLG_GXM_MOUSE_MOVED			605

// List boxes
#define DLG_LBM_QUERY_STRINGS		700
#define DLG_LBM_QUERY_NUM_STRINGS	701
#define DLG_LBM_QUERY_SEL			702
#define DLG_LBM_SET_SEL				703
#define DLG_LBM_QUERY_CUR_SEL		704
#define DLG_LBM_SET_CUR_SEL			705
#define DLG_LBM_GET_SEL_ITEMS		706
#define DLG_LBM_GET_SEL_COUNT		707
#define DLG_LBM_INSERT_STRING		708
#define DLG_LBM_DELETE_STRING		709

// Combo boxes
#define DLG_CBM_QUERY_STRINGS		800
#define DLG_CBM_QUERY_NUM_STRINGS	801
#define DLG_CBM_QUERY_CUR_SEL		804
#define DLG_CBM_SET_CUR_SEL			805
#define DLG_CBM_INSERT_STRING		808
#define DLG_CBM_DELETE_STRING		809
#define DLG_CBM_SHOW_DROP_DOWN		810

// Chart3d
#define	DLG_3DM_QUERY_ORIENTATION	900
#define	DLG_3DM_SET_ORIENTATION		901


/*
**	Widget styles
*/

// Common styles
#define	DLG_STY_HIDDEN				0x80000000L
#define	DLG_STY_NOTAB				0x40000000L
#define DLG_STY_ATTACH				0x20000000L

// Button widgets
#define	DLG_BUS_3STATE				0x00000001L
#define	DLG_BUS_AUTO3STATE			0x00000002L
#define	DLG_BUS_AUTOCHECKBOX		0x00000004L
#define	DLG_BUS_AUTORADIOBUTTON		0x00000008L
#define	DLG_BUS_CHECKBOX			0x00000010L
#define	DLG_BUS_DEFAULT				0x00000020L
#define	DLG_BUS_HELP				0x00000040L
#define	DLG_BUS_NOBORDER			0x00000080L
#define	DLG_BUS_NOCURSORSELECT		0x00000100L
#define	DLG_BUS_NOPOINTERFOCUS		0x00000200L
#define	DLG_BUS_PUSHBUTTON			0x00000400L
#define	DLG_BUS_RADIOBUTTON			0x00000800L
#define	DLG_BUS_SYSCOMMAND			0x00001000L
#define	DLG_BUS_GX					0x00002000L
#define DLG_BUS_CANCEL				0x00004000L

// Entry field widgets
#define	DLG_EFS_AUTOTAB				0x00000002L
#define	DLG_EFS_CENTER				0x00000004L
#define	DLG_EFS_LEFT				0x00000008L
#define	DLG_EFS_BORDER				0x00000010L
#define	DLG_EFS_READONLY			0x00000020L
#define	DLG_EFS_RIGHT				0x00000040L
#define	DLG_EFS_UNREADABLE			0x00000080L

// Frame widgets
#define	DLG_FRS_BORDER				0x00000001L
#define	DLG_FRS_DLGBORDER			0x00000002L
#define	DLG_FRS_SIZEBORDER			0x00000004L
#define	DLG_FRS_TITLEBAR			0x00000008L
#define	DLG_FRS_SYSMENU				0x00000010L
#define	DLG_FRS_MINBUTTON			0x00000020L
#define	DLG_FRS_MAXBUTTON			0x00000040L
#define	DLG_FRS_MAIN				0x00000080L
#define	DLG_FRS_AUTOPOS				0x00000100L
#define	DLG_FRS_AUTOSIZE			0x00000200L
#define	DLG_FRS_AUTOCENTER			0x00000400L
#define DLG_FRS_RESIZABLE			0x00001000L
#define	DLG_FRS_OVERLAPPED			0x00002000L
#define DLG_FRS_OVERLAPPEDWINDOW	0x00004000L
#define DLG_FRS_MODALFRAME			0x00010000L

// Scroll bar widgets
#define	DLG_SBS_VERTICAL			0x00000001L
#define	DLG_SBS_HORIZONTAL			0x00000002L

// Slider widgets
#define	DLG_SLS_VERTICAL			0x00000001L
#define	DLG_SLS_HORIZONTAL			0x00000002L

// Constant widgets
#define	DLG_COS_BKGNDFRAME			0x00000001L
#define	DLG_COS_BKGNDRECT			0x00000002L
#define	DLG_COS_FGNDFRAME			0x00000004L
#define	DLG_COS_FGNDRECT			0x00000008L
#define	DLG_COS_GROUPBOX			0x00000010L
#define	DLG_COS_HALFTONEFRAME		0x00000020L
#define	DLG_COS_HALFTONERECT		0x00000040L
#define	DLG_COS_UNDERSCORE			0x00000080L
#define	DLG_COS_LEFT				0x00000000L
#define	DLG_COS_RIGHT				0x00000100L
#define	DLG_COS_HCENTER				0x00000200L
#define	DLG_COS_TOP					0x00000000L
#define	DLG_COS_BOTTOM				0x00000400L
#define	DLG_COS_VCENTER				0x00000800L
#define	DLG_COS_INDENT				0x00001000L
#define	DLG_COS_EMBOSS				0x00002000L
#define	DLG_COS_SHADOW				0x00004000L

// Text boxes
#define	DLG_TBS_BORDER				0x00000001L
#define	DLG_TBS_HSCROLL				0x00000002L
#define	DLG_TBS_READONLY	   		0x00000004L
#define	DLG_TBS_VSCROLL		   		0x00000008L
#define	DLG_TBS_WORDWRAP	   		0x00000010L

// Gx widgets
#define	DLG_GXS_PUSHBUTTON			0x00000001L
#define	DLG_GXS_CHECKBOX			0x00000002L
#define	DLG_GXS_AUTOCHECKBOX		0x00000004L
#define	DLG_GXS_MOUSEABLE			0x00000008L
#define	DLG_GXS_INVERTSELECT		0x00000010L
#define	DLG_GXS_NOPOINTERFOCUS		0x00000020L
#define	DLG_GXS_NOERASE				0x00000040L
#define	DLG_GXS_NOBORDER			0x00000080L
#define	DLG_GXS_IMAGE				0x00000100L

// Generic list boxes
#define	DLG_GLS_HORZSCROLL			0x00000001L
#define	DLG_GLS_VERTSCROLL			0x00000002L
#define	DLG_GLS_TITLE				0x00000004L
#define	DLG_GLS_COLUMNSCROLL		0x00000008L
#define	DLG_GLS_COLUMNCURSOR		0x00000010L
#define	DLG_GLS_COLUMNSEPARATORS	0x00000020L
#define	DLG_GLS_LINECURSOR			0x00000040L
#define	DLG_GLS_LINESEPARATORS		0x00000080L
#define	DLG_GLS_INVERTCURSOR		0x00000100L
#define	DLG_GLS_PULLDOWN			0x00000200L
#define	DLG_GLS_BORDER				0x00000400L
#define	DLG_GLS_NOBUTTON			0x00000800L

// Separators
#define	DLG_SES_HORIZONTAL			0x00000001L
#define	DLG_SES_VERTICAL			0x00000002L

// List Boxes
#define DLG_LBS_NOTIFY				0x00000001L
#define DLG_LBS_SORT				0x00000002L
#define DLG_LBS_NOREDRAW			0x00000004L
#define DLG_LBS_MULTIPLESEL			0x00000008L
#define DLG_LBS_OWNERDRAWFIXED		0x00000010L
#define DLG_LBS_OWNERDRAWVARIABLE	0x00000020L
#define DLG_LBS_HASSTRINGS			0x00000040L
#define DLG_LBS_USETABSTOPS			0x00000080L
#define DLG_LBS_NOINTEGRALHEIGHT	0x00000100L
#define DLG_LBS_MULTICOLUMN			0x00000200L
#define DLG_LBS_WANTKEYBOARDINPUT	0x00000400L
#define DLG_LBS_EXTENDEDSEL			0x00000800L
#define DLG_LBS_DISABLENOSCROLL		0x00001000L
#define DLG_LBS_NODATA				0x00002000L
#define DLG_LBS_NOSEL			    0x00004000L
#define DLG_LBS_STANDARD			0x00008000L

// Combo Boxes
#define DLG_CBS_SIMPLE				0x00000001L
#define DLG_CBS_DROPDOWN			0x00000002L
#define DLG_CBS_DROPDOWNLIST		0x00000004L
#define DLG_CBS_OWNERDRAWFIXED		0x00000010L
#define DLG_CBS_OWNERDRAWVARIABLE	0x00000020L
#define DLG_CBS_AUTOHSCROLL			0x00000040L
#define DLG_CBS_OEMCONVERT			0x00000080L
#define DLG_CBS_SORT				0x00000100L
#define DLG_CBS_HASSTRINGS			0x00000200L
#define DLG_CBS_NOINTEGRALHEIGHT	0x00000400L
#define DLG_CBS_DISABLENOSCROLL		0x00000800L
#define DLG_CBS_UPPERCASE			0x00002000L
#define DLG_CBS_LOWERCASE			0x00004000L


/*
**	Define types
*/

typedef	long	DLG_HANDLE;
typedef	int		DLG_CLASS_NUMBER;
typedef int		DLG_MSG;

typedef	struct	DlgWidgetStructure			DLG_WIDGET;
typedef	struct	DlgDriverStructure			DLG_DRIVER;
typedef	struct	DlgClassStructure			DLG_CLASS;
typedef struct	DlgMsgQElementStructure		DLG_MSG_Q_ELEMENT;
typedef struct	DlgMsgQStructure			DLG_MSG_Q;

typedef int		(*DLG_PF_DRIVERINIT)(		DLG_DRIVER *Driver, void *InitData );
typedef int		(*DLG_PF_MSG)(				DLG_WIDGET *DlgWidget, DLG_MSG Msg, DT_VALUE *Arguments, DT_VALUE *Result );
typedef long	(*DLG_PF_COLOR_DIALOG)(		const char *Title, long StartingColor, int SolidColorOnly );
typedef char	*(*DLG_PF_FILE_DIALOG)(		DLG_WIDGET *Widget, const char *Title, const char *FileSpec, int SaveMode );
typedef	int		(*DLG_PF_ABORT)(			void *Args );
typedef int		(*DLG_PF_WAIT_FOR_QUIT)(	int Polling, DLG_PF_ABORT pfAbort, void *Args );
typedef int		(*DLG_PF_WAIT_FOR_DISMISS)(	DLG_WIDGET *ParentWidget, DLG_WIDGET *ChildWidget, int Destroy, DLG_PF_ABORT pfAbort, void *Args, DT_VALUE *Results );
typedef int		(*DLG_PF_MAP_POINT)(		DLG_WIDGET *WidgetFrom, DLG_WIDGET *WidgetTo, long *x, long *y );
typedef void	(*DLG_PF_HOURGLASS)(		DLG_WIDGET *Widget, int State );
typedef int		(*DLG_PF_WINDOW_SNAP)(		const char *Gif );


/*
**	Structure used to hold widget class data
*/

struct DlgClassStructure
{
	DLG_CLASS_NUMBER
			Number;

	char	Name[ DLG_CLASS_NAME_SIZE ];

	DLG_PF_MSG
			pfMsg;

	unsigned long
			Style;

};	// DLG_CLASS


/*
**	Structure used to hold a window
*/

struct DlgWidgetStructure
{
	DLG_WIDGET
			*TabPrev,
			*TabNext,
			*FirstTabChild;

	DLG_HANDLE
			Handle,
			ParentHandle;

	DLG_CLASS
			*Class;

	unsigned long
			Style;

	int		ClassNumber,
			Painting : 1,
			Creating : 1;

	long	AvgCharX,
			AvgCharY;

	struct
	{
		long	Number;

		void	*Pointer;

	} DriverData;

	DLG_PF_MSG
			pfCallBack;

	DT_VALUE
			Data;

	void	*Extra;

};	// DLG_WIDGET


/*
**	Structure used to hold message queue elements
*/

struct DlgMsgQElementStructure
{
	DLG_HANDLE
			Handle;

	DLG_MSG	Msg;

	DT_VALUE
			Args;

};	// DLG_MSG_Q_ELEMENT


/*
**	Structure used to hold the message queue
*/

struct DlgMsgQStructure
{
	int		Size,
			In,
			Out;

	DLG_MSG_Q_ELEMENT
			*Elements;

};	// DLG_MSG_Q


/*
**	Structure used to hold driver information
*/

struct DlgDriverStructure
{
	int		Initialized,
			Type;

	DLG_HANDLE
			HandleDesktop;

	DLG_PF_WAIT_FOR_QUIT
			pfWaitForQuit;

	DLG_PF_WAIT_FOR_DISMISS
			pfWaitForDismiss;

	DLG_PF_COLOR_DIALOG
			pfColorDialog;

	DLG_PF_FILE_DIALOG
			pfFileDialog;

	DLG_PF_MAP_POINT
			pfMapPoint;

	DLG_PF_HOURGLASS
			pfHourglass;

	DLG_PF_WINDOW_SNAP
			pfWindowSnap;

	struct
	{
		long	Number;

		void	*Pointer;

	} DriverData;

};	// DLG_DRIVER


/*
**	Variables
*/

DLLEXPORT DLG_DRIVER
		*DlgDriver;

extern DLG_MSG_Q
		*DlgMsgQ;

DLLEXPORT DLG_PF_MSG
		DlgSpyFunction;

DLLEXPORT int
		DlgCoordinateMode,
		DlgErrSource;

DLLEXPORT struct HashStructure
		*DlgHandleHash;


/*
**	Prototype functions
*/

DLLEXPORT int
		DlgInitialize(			int DriverType, void *InitData ),
		DlgWaitForQuit(			int Polling, DLG_PF_ABORT pfAbort, void *AbortArgs ),
		DlgWaitForDismiss(		DLG_HANDLE ParentHandle, DLG_HANDLE DialogHandle, int Destroy, DLG_PF_ABORT pfAbort, void *AbortArgs, DT_VALUE *Result ),
		DlgMapPoint(			DLG_HANDLE HandleFrom, DLG_HANDLE HandleTo, long *x, long *y );

DLLEXPORT char
		*DlgWindowSnapToGif();

DLLEXPORT DLG_WIDGET
		*DlgWidgetAlloc(		void ),
		*DlgWidgetFromHandle(	DLG_HANDLE Handle );

DLLEXPORT void
		DlgWidgetFree(			DLG_HANDLE Handle ),
		DlgWidgetDestroyAll(	void );

DLLEXPORT DLG_HANDLE
		DlgQueryDesktop( 		void ),
		DlgQueryParent(			DLG_HANDLE ChildHandle ),
		DlgWidgetCreate(		DLG_HANDLE ParentHandle, const char *ClassName, unsigned long Style, DT_VALUE *Data, short Xlow, short Ylow, short Xsize, short Ysize, DLG_PF_MSG pfCallBack, void *Extra ),
		DlgAttachMenu(			DLG_HANDLE ParentHandle, DT_VALUE *MenuTagArray, DT_VALUE *Data, DLG_PF_MSG pfCallBack, void *Extra ),
		DlgTabNext(				DLG_HANDLE TabFrom ),
		DlgTabPrev(				DLG_HANDLE TabFrom );

DLLEXPORT int
		DlgAttachSubMenu(		DLG_HANDLE MenuHandle, DT_VALUE *MenuTags, DT_VALUE *AttachTag );

DLLEXPORT DLG_CLASS_NUMBER
		DlgClassRegister(		const char *ClassName, DLG_PF_MSG pfMsg, unsigned long Style );

DLLEXPORT DLG_CLASS
		*DlgClassFromName(		const char *ClassName );

DLLEXPORT int
		DlgClassList(			DT_VALUE *Results ),
		DlgMsgList(				DT_VALUE *Results, DLG_CLASS *Class );

DLLEXPORT DLG_MSG
		DlgMsgRegister(			const char *MsgName ),
		DlgMsgNumFromName(		const char *MsgName );

DLLEXPORT const char
		*DlgMsgNameFromNum(		DLG_MSG MsgNum );

DLLEXPORT long
		DlgColorDialog(			const char *Title, long StartingColor, int SolidColorOnly );

DLLEXPORT char
		*DlgFileDialog(			DLG_HANDLE Handle, const char *Title, const char *FileSpec, int SaveMode );

DLLEXPORT void
		DlgHourglass(			DLG_HANDLE Handle, int State );


DLLEXPORT DLG_MSG_Q
		*DlgMsgQCreate(			int Size );

DLLEXPORT void
		DlgMsgQDestroy(			DLG_MSG_Q *MsgQ ),
		DlgMsgQClear(			DLG_MSG_Q *MsgQ );

DLLEXPORT int
		DlgMsg(					DLG_HANDLE Handle, DLG_MSG Msg, DT_VALUE *Arguments, DT_VALUE *Result ),
		DlgMsgPost(				DLG_HANDLE Handle, DLG_MSG Msg, DT_VALUE *Arguments ),
		DlgMsgGet(				DLG_MSG_Q_ELEMENT *QElem );

/*
**	Macros
*/

inline short DLG_X_PIXEL_FROM_MODE( DLG_WIDGET* widget, int c )
{
	if( DlgCoordinateMode != DLG_COORD_MODE_AVG_CHAR )
		return (short) c;
	return ((short)c * widget->AvgCharX ) / 10;
}

inline short DLG_Y_PIXEL_FROM_MODE( DLG_WIDGET* widget, int c )
{
	if( DlgCoordinateMode != DLG_COORD_MODE_AVG_CHAR )
		return (short) c;
	return ((short)c * widget->AvgCharY ) / 10;
}

inline short DLG_X_PIXEL_TO_MODE( DLG_WIDGET* widget, int p )
{
	if( DlgCoordinateMode != DLG_COORD_MODE_AVG_CHAR )
		return (short) p;
	return ((short)p * 10 ) / widget->AvgCharX;
}

inline short DLG_Y_PIXEL_TO_MODE( DLG_WIDGET* widget, int p )
{
	if( DlgCoordinateMode != DLG_COORD_MODE_AVG_CHAR )
		return (short) p;
	return ((short)p * 10 ) / widget->AvgCharY;
}


#endif

