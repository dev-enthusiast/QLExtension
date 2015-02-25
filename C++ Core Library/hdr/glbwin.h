/* $Header: /home/cvs/src/dialog/src/glbwin.h,v 1.2 1998/10/09 22:01:24 procmon Exp $ */
/****************************************************************
**
**	glbwin.h
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_GLBWIN_H )
#define IN_GLBWIN_H

#include	<nt_event.h>

/*
**	Window messages
*/

#define	WM_GLB_INITIALIZE			(WM_FIRST_GLB_MESSAGE + 1)
#define	WM_GLB_DRAW					(WM_FIRST_GLB_MESSAGE + 2)
#define	WM_GLB_SHIFT				(WM_FIRST_GLB_MESSAGE + 3)
#define	WM_GLB_ENTER				(WM_FIRST_GLB_MESSAGE + 4)
#define	WM_GLB_CURSOR_MOVED			(WM_FIRST_GLB_MESSAGE + 5)
#define	WM_GLB_SIZES_CHANGED		(WM_FIRST_GLB_MESSAGE + 6)
#define	WM_GLB_MESSAGE				(WM_FIRST_GLB_MESSAGE + 7)
#define	WM_GLB_ITEM_COUNT_CHANGED	(WM_FIRST_GLB_MESSAGE + 8)
#define	WM_GLB_CURSOR_ITEM_CHANGED	(WM_FIRST_GLB_MESSAGE + 9)
#define	WM_GLB_QUERY_GLB			(WM_FIRST_GLB_MESSAGE + 10)


/*
**	Structure used to initialize a GLB window
*/

typedef struct InitDataStructure
{
	GLB_APP_DRIVER
			*AppDriver;

	GLB_DATA_DRIVER
			*DataDriver;

	GLB_ITEM
			*InitialItem;

	int		Style;

	void	*DataDriverData;

} INIT_DATA;

struct DlgWidgetStructure;
struct DlgWinCreateDataStructure;

/*
**	Variables
*/

extern char
		*ClassGLB,
		*ClassTitle,
		*ClassPullDown;


/*
**	Prototype functions
*/

LRESULT CALLBACK WndProcGlb(
	HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
);

LRESULT CALLBACK WndProcTitle(
	HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
);

LRESULT CALLBACK WndProcPullDown(
	HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
);

DLLEXPORT char
	*WinGlbClass(			int GlbStyle );

DLLEXPORT int
	WinGlbCreate(			DlgWidgetStructure *Widget, GLB_APP_DRIVER *AppDriver, GLB_DATA_DRIVER *DataDriver, GLB_ITEM *InitialItem, int GlbStyle, void *DataDriverData );

DLLEXPORT int
	WinGlbMessage(			HWND hwnd, int Msg, void *Arg );

DLLEXPORT GLB
	*WinGlbFromWindow(		HWND hwnd );

#endif

