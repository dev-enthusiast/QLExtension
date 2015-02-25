/* $Header: /home/cvs/src/dialog/src/glb.h,v 1.12 1998/10/09 22:01:23 procmon Exp $ */
/****************************************************************
**
**	GLB.H	- Generic List Box
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined( IN_GLB_H )
#define IN_GLB_H

#if !defined( IN_DATATYPE_H )
#	include <datatype.h>
#endif


/*
**	Define constants
*/

// Styles
#define	GLB_STYLE_TITLE				0x00000001L
#define	GLB_STYLE_VERTSCROLL		0x00000002L
#define	GLB_STYLE_VERTBUTTONS		0x00000004L
#define	GLB_STYLE_HORZSCROLL		0x00000008L
#define	GLB_STYLE_HORZBUTTONS		0x00000010L
#define	GLB_STYLE_COLUMNSCROLL		0x00000020L
#define	GLB_STYLE_COLUMNCURSOR		0x00000040L
#define	GLB_STYLE_COLUMNSEPARATORS	0x00000080L
#define	GLB_STYLE_LINECURSOR		0x00000100L
#define	GLB_STYLE_LINESEPARATORS	0x00000200L
#define	GLB_STYLE_INVERTCURSOR		0x00000400L
#define	GLB_STYLE_KEYSEARCH			0x00000800L
#define	GLB_STYLE_PULLDOWN			0x00001000L
#define	GLB_STYLE_BORDER			0x00002000L
#define	GLB_STYLE_NOBUTTON			0x00004000L

// Column Flags
#define	GLB_COLUMN_HIDDEN			0x00000001L
#define	GLB_COLUMN_RIGHT			0x00000002L
#define	GLB_COLUMN_CENTER			0x00000004L
#define	GLB_COLUMN_SELECTABLE		0x00000008L
#define	GLB_COLUMN_SELECTED			0x00000010L

// GlbProcess Messages				Num		Arguments
#define	GLB_CREATE					1		// GLB_ITEM *Item, void *DataDriverData
#define	GLB_DESTROY					2		// void
#define	GLB_RESIZE					3		// int Count, int NewWidth
#define	GLB_REDRAW					4		// void
#define	GLB_REFRESH_OFF				5		// void
#define	GLB_REFRESH_ON				6		// void
#define	GLB_CURSOR_HOME				7		// void
#define	GLB_CURSOR_END				8		// void
#define	GLB_CURSOR_DOWN				9		// void
#define	GLB_CURSOR_UP				10		// void
#define	GLB_CURSOR_PAGE_DOWN		11		// void
#define	GLB_CURSOR_PAGE_UP			12		// void
#define	GLB_CURSOR_LINE				13		// int Line
#define	GLB_CURSOR_LEFT				14		// void
#define	GLB_CURSOR_RIGHT			15		// void
#define	GLB_CURSOR_PAGE_LEFT		16		// void
#define	GLB_CURSOR_PAGE_RIGHT		17		// void
#define	GLB_CURSOR_COLUMN			18		// int ColNum
#define	GLB_CURSOR_ITEM_NUMBER		19		// long ItemNumber
#define	GLB_CURSOR_QUERY_COORDS		20		// GLB_CURSOR_COORDS *CursorCoords
#define	GLB_CURSOR_QUERY_LINE		21		// int *LineNumPtr
#define	GLB_COLUMN_LOCK_ORDER		22		// int Count, int *ColOrder
#define	GLB_COLUMN_SCROLL_ORDER		23		// int Count, int *ColOrder
#define	GLB_COLUMN_DEFINE			24		// GLB_COLUMN *ColPtr
#define	GLB_COLUMN_PICKED			25		// int ColNum
#define	GLB_ENTER					26		// void
#define	GLB_FOCUS					27		// int Focus
#define	GLB_DATA_INSERT				28		// GLB_ITEM *GlbItem
#define	GLB_DATA_DELETE				29		// GLB_ITEM *GlbItem
#define	GLB_DATA_UPDATE				30		// GLB_ITEM *GlbItem
#define	GLB_DATA_QUERY				31		// GLB_ITEM *GlbItem
#define	GLB_DATA_QUERY_BY_LINE		32		// int Line, GLB_ITEM *Item
#define	GLB_DATA_QUERY_TEXT_BY_LINE	33		// int Line, **Str
#define	GLB_DATA_QUERY_ITEM_NUMBER	34		// void
#define	GLB_DATA_DELETE_ALL			35		// void
#define	GLB_DATA_DRIVER_MESSAGE		36		// int MsgNum, void *DataDriverData
#define	GLB_KEY						37		// char Key
#define	GLB_SET_SEARCH_BUFFER		38		// char *SearchString
#define	GLB_RESET_SEARCH_BUFFER		39		// void

// DataDriver Get Messages
#define	GLB_GET_FIRST				1
#define	GLB_GET_LAST				2
#define	GLB_GET_LESS				3
#define	GLB_GET_GREATER				4
#define	GLB_GET_LE					5
#define	GLB_GET_GE					6

// DataDriver flags
#define	GLB_OPERATE_EXTERNAL		0x0001
#define	GLB_ITEM_COUNT_KNOWN		0x0002

// Miscellaneous Constants
#define	GLB_COL_GROWTH_AMOUNT		10
#define	GLB_SEARCH_BUFFER_SIZE		64


/*
**	Define types
*/

typedef struct GlbStructure								GLB;
typedef struct GlbDataDriverStructure					GLB_DATA_DRIVER;
typedef struct GlbScreenDriverStructure					GLB_SCREEN_DRIVER;
typedef	struct GlbAppDriverStructure					GLB_APP_DRIVER;
typedef struct GlbItemStructure							GLB_ITEM;
typedef struct GlbColumnStructure						GLB_COLUMN;
typedef struct GlbColumnOrderStructure					GLB_COLUMN_ORDER;
typedef struct GlbScreenMetricsStructure				GLB_SCREEN_METRICS;
typedef struct GlbCursorCoordsStructure					GLB_CURSOR_COORDS;

typedef	void	(*GLB_PF_DATA_CREATE)(					GLB *Glb, void *DataDriverData );
typedef	void	(*GLB_PF_DATA_DESTROY)(					GLB *Glb );
typedef	int		(*GLB_PF_DATA_GET)(						GLB *Glb, int GetType, GLB_ITEM *Base, int Count, GLB_ITEM *ItemBuffer );
typedef	void	(*GLB_PF_DATA_FREE)(					GLB *Glb, int Count, GLB_ITEM *ItemBuffer );
typedef int		(*GLB_PF_DATA_SET_STRINGS)(				GLB *Glb, GLB_ITEM *Item, char **StringTable );
typedef void	(*GLB_PF_DATA_FREE_STRINGS)(			GLB *Glb, int Count, char **StringTable );
typedef	void	(*GLB_PF_DATA_ENTER)(					GLB *Glb, GLB_ITEM *Item, DT_VALUE *Value );
typedef	int		(*GLB_PF_DATA_OPERATE)(					GLB *Glb, int Msg, GLB_ITEM *GlbItem );
typedef	int		(*GLB_PF_DATA_COMPARE)(					GLB *Glb, GLB_ITEM *ItemA, GLB_ITEM *ItemB );
typedef	int		(*GLB_PF_DATA_ITEM_FROM_SEARCH_STRING)(	GLB *Glb, GLB_ITEM *Item, char *SearchString );
typedef int		(*GLB_PF_DATA_MESSAGE)(					GLB *Glb, int MsgNum, void *Arg );

typedef	void	(*GLB_PF_SCREEN_CREATE)(				GLB *Glb );
typedef	void	(*GLB_PF_SCREEN_DESTROY)(				GLB *Glb );
typedef	void	(*GLB_PF_SCREEN_CURSOR_MOVED)(			GLB *Glb, int NewCursor );
typedef	void	(*GLB_PF_SCREEN_DRAW)(					GLB *Glb, int From, int Size );
typedef	void	(*GLB_PF_SCREEN_SHIFT)(					GLB *Glb, int From, int To, int Size );
typedef	void	(*GLB_PF_SCREEN_SIZES_CHANGED)(			GLB *Glb );
typedef	void	(*GLB_PF_SCREEN_ITEM_COUNT_CHANGED)(	GLB *Glb, long ItemCount );
typedef void	(*GLB_PF_SCREEN_CURSOR_ITEM_CHANGED)(	GLB *Glb, long CursorItem );
typedef int		(*GLB_PF_SCREEN_CURSOR_COORDS)(			GLB *Glb, GLB_CURSOR_COORDS *Coords );

typedef	void	(*GLB_PF_APP_ENTER)(					GLB *Glb, DT_VALUE *Value );
typedef	void	(*GLB_PF_APP_FOCUS)(					GLB *Glb, int Focus );
typedef	void	(*GLB_PF_APP_COLUMN_PICKED)(			GLB *Glb, int ColNum );
typedef void	(*GLB_PF_APP_SEARCH_CHANGED)(			GLB *Glb, char *SearchString );
typedef void	(*GLB_PF_APP_CURSOR_CHANGED)(			GLB *Glb );


/*
**	Structure used to return cursor coordinates
*/

struct GlbCursorCoordsStructure
{
	long	xLow,
			yLow,
			xHigh,
			yHigh;

};	// GLB_CURSOR_COORDS


/*
**	Structure used to hold handle/data info
*/

typedef struct GlbPartStructure
{
	void	*Pointer;

	double	Number;

} GLB_PART;


/*
**	Structure used to contain list items
*/

struct GlbItemStructure
{
	GLB_PART
			Handle,
			Data;

};	// GLB_ITEM


/*
**	Structure used to contain column information
*/

struct GlbColumnStructure
{
	char	*Name,
			*Title;

	unsigned long
			Flags;

	int		Width,
			TitleLines;

};	// GLB_COLUMN


/*
**	Structure used to keep track of column order and screen location
*/

struct GlbColumnOrderStructure
{
	int		ColNum,			// Column number
			XLeft,			// Leftmost reference
			XStart,			// Start of displayed data
			XEnd,			// End of displayed data
			XSeparator;		// Start of separator line (-1 means no line)

};	// GLB_COLUMN_ORDER


/*
**	Structure used to contain screen metrics
*/

struct GlbScreenMetricsStructure
{
	int		ItemHeight,
			XAvgChar,
			XIndentWidth,
			XSeparatorWidth;

};	// GLB_SCREEN_METRICS


/*
**	Data driver
*/

struct GlbDataDriverStructure
{
	GLB_PF_DATA_CREATE
			pfCreate;

	GLB_PF_DATA_DESTROY
			pfDestroy;

	GLB_PF_DATA_GET
			pfGetItems;

	GLB_PF_DATA_FREE
			pfFreeItems;

	GLB_PF_DATA_SET_STRINGS
			pfSetStrings;

	GLB_PF_DATA_FREE_STRINGS
			pfFreeStrings;

	GLB_PF_DATA_ENTER
			pfEnter;

	GLB_PF_DATA_OPERATE
			pfOperate;

	GLB_PF_DATA_COMPARE
			pfCompare;

	GLB_PF_DATA_ITEM_FROM_SEARCH_STRING
			pfItemFromSearchString;

	GLB_PF_DATA_MESSAGE
			pfMessage;

	GLB_PART
			Data;

	int		Flags,
			ColCount;

	long	ItemCount;

};	// GLB_DATA_DRIVER


/*
**	Screen driver
*/

struct GlbScreenDriverStructure
{
	GLB_PF_SCREEN_CREATE
			pfCreate;

	GLB_PF_SCREEN_DESTROY
			pfDestroy;

	GLB_PF_SCREEN_CURSOR_MOVED
			pfCursorMoved;

	GLB_PF_SCREEN_DRAW
			pfDraw;

	GLB_PF_SCREEN_SHIFT
			pfShift;
			
	GLB_PF_SCREEN_SIZES_CHANGED
			pfSizesChanged;

	GLB_PF_SCREEN_ITEM_COUNT_CHANGED
			pfItemCountChanged;

	GLB_PF_SCREEN_CURSOR_ITEM_CHANGED
			pfCursorItemChanged;

	GLB_PF_SCREEN_CURSOR_COORDS
			pfCursorCoords;

	GLB_PART
			Data;

	int		ItemHeight,
			AvgCharX;

};	// GLB_SCREEN_DRIVER;


/*
**	Application driver
*/

struct GlbAppDriverStructure
{
	GLB_PF_APP_ENTER
			pfEnter;

	GLB_PF_APP_FOCUS
			pfFocus;

	GLB_PF_APP_COLUMN_PICKED
			pfColumnPicked;

	GLB_PF_APP_SEARCH_CHANGED
			pfSearchChanged;

	GLB_PF_APP_CURSOR_CHANGED
			pfCursorChanged;

	GLB_PART
			Data;

};	// GLB_APP_DRIVER;


/*
**	Structure used to hold generic list box
*/

struct GlbStructure
{
	unsigned long
			Style;

	int		CursorLine,
			ScreenSize,
			ScreenWidth,
			LastScreenLine,
			LineCount,
			TitleLines,
			RefreshCount;

	int		ColCursor,
			ColLeft,
			ColLeftMax,
			ColCount,
			ColAllocCount,
			xScrollSize,
			xScrollPos,
			xLockSize;

	int		LockCount,
			ScrollCount;

	int		OrderAutomatically;

	char	**StringTable,
			*SearchBuffer,
			*WorkingBuffer;

	GLB_ITEM
			*Items,
			*NewItems;

	GLB_COLUMN
			*Columns;

	GLB_COLUMN_ORDER
			*Locked,
			*Scrolled;

	GLB_SCREEN_METRICS
			ScreenMetrics;

	GLB_DATA_DRIVER
			DataDriver;
			
	GLB_SCREEN_DRIVER
			ScreenDriver;
			
	GLB_APP_DRIVER
			AppDriver;

};	// GLB


/*
**	Prototype functions
*/

DLLEXPORT GLB
		*GlbCreate(			GLB_APP_DRIVER *AppDriver, GLB_DATA_DRIVER *DataDriver, GLB_SCREEN_DRIVER *ScreenDriver, GLB_ITEM *InitialItem, unsigned long Style, void *DataDriverData );

DLLEXPORT void
		GlbDestroy(			GLB *Glb ),
		GlbColumnsToCoords(	GLB *Glb, int ColStart, int ColEnd, int *xLeft, int *xRight );

DLLEXPORT int
		GlbProcess(			GLB *Glb, int Msg, ... ),
		GlbColumnFromCoord(	GLB *Glb, int xCoord );

#endif

