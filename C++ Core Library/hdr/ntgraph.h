/* $Header: /home/cvs/src/ntgraph/src/ntgraph.h,v 1.15 2001/12/03 11:13:30 penala Exp $ */
/****************************************************************
**
**	PMGRAPH.H	- PM Graph Library
**
**	$Revision: 1.15 $
**
****************************************************************/

//#define pmgraph_winproc PMGRAPH_WINPROC

#define WINDOW_ARGS HWND hwnd, UINT msg, WPARAM mp1, LPARAM mp2

typedef struct pmplot_struct {

	HINSTANCE hab;
	HDC hdc;
	PAINTSTRUCT PaintStruct;
	HWND w;
	HWND coord_w;

	int xsize;
	int ysize;
	RECT ibox;
	int MarginLeft;
	int MarginRight;
	int MarginTop;
	int MarginBottom;

	double xlow, xhigh;
	double ylow, yhigh;
	double y1low, y1high;

	double x, y, z, high, low, open;
	COLORREF line_color;
	long line_type;
	long marker;
	long marker_width;		// in 100ths of an inch
	long line_width;

	/* Data function should put points in x and y, and should return 0 until done */
	int (*data_func)(struct pmplot_struct *p);

	long count;

	COLORREF background_color;
	COLORREF InnerColor1;
	COLORREF InnerColor2;
	int InnerMix;
#define ntgraph_MIX_NONE		0
#define ntgraph_MIX_VERT		1
#define ntgraph_MIX_VERT2		2
#define ntgraph_MIX_HORZ		3
#define ntgraph_MIX_HORZ2		4
#define ntgraph_MIX_LDIAG		5
#define ntgraph_MIX_LDIAG2		6
#define ntgraph_MIX_RDIAG		7
#define ntgraph_MIX_RDIAG2		8
#define ntgraph_MIX_CENTER		9

	char *title;
	COLORREF title_color;
	LOGFONT title_font;

	char *subtitle;
	LOGFONT subtitle_font;

	LOGFONT axis_font;
	char *xtitle;
	char *ytitle;
	char *y1title;
	int gslogo;

	COLORREF grid_color;	// formerly was title_color
	void (*legend_func)(struct pmplot_struct *p);
	char *(*xlabel_func)(struct pmplot_struct *p);
	int xgrid;
	char *(*ylabel_func)(struct pmplot_struct *p);
	int ygrid;
	char *(*y1label_func)(struct pmplot_struct *p);

	char *marker_string;

	double (pascal *x_xlate)(struct pmplot_struct *p, double x);
	double (pascal *x_rev_xlate)(struct pmplot_struct *p, double x);

// These are save for when we hide frame controls
	HWND w_title_bar;
	HWND w_sys_menu;
	HWND w_min_max;
	HWND w_menu;
	POINT border_size;
	int frame_hidden;
	ULONG frame_flags;
	USHORT resource;

	void *extra;

} PMPLOT;

/* These are the same numeric values as PM's MARKSYMs */
#define pmplot_MARKER_DEFAULT                 0L
#define pmplot_MARKER_CROSS                   1L
#define pmplot_MARKER_PLUS                    2L
#define pmplot_MARKER_DIAMOND                 3L
#define pmplot_MARKER_SQUARE                  4L
#define pmplot_MARKER_SIXPOINTSTAR            5L
#define pmplot_MARKER_EIGHTPOINTSTAR          6L
#define pmplot_MARKER_SOLIDDIAMOND            7L
#define pmplot_MARKER_SOLIDSQUARE             8L
#define pmplot_MARKER_DOT                     9L
#define pmplot_MARKER_SMALLCIRCLE            10L
#define pmplot_MARKER_UPARROW                11L
#define pmplot_MARKER_DOWNARROW              12L
#define pmplot_MARKER_BLANK                  64L

/* forward declarations */

/* in ntgraph.c */
DLLEXPORT HWND pmgraph_open(HINSTANCE hab, HWND parent, DWORD style, USHORT resource);
//DLLEXPORT LRESULT CALLBACK pmgraph_winproc(WINDOW_ARGS);
DLLEXPORT PMPLOT *pmgraph_init(HWND w);
DLLEXPORT int pmgraph_size(HWND w, PMPLOT *pmplot, int xsize, int ysize);
DLLEXPORT void pmgraph_coord_w(PMPLOT *pmplot);
DLLEXPORT int pmgraph_axes(PMPLOT *pmplot);
DLLEXPORT void draw_text(HDC hps, int len, char *s, RECT *r, LONG fore, LONG back, unsigned int flags);
DLLEXPORT int pmgraph_line_plot(PMPLOT *pmplot, int axis);
DLLEXPORT int pmgraph_spline_plot(PMPLOT *pmplot, int axis);
DLLEXPORT int pmgraph_volume_plot(PMPLOT *pmplot, int axis);
DLLEXPORT int pmgraph_hloc_plot(PMPLOT *pmplot, int axis);
DLLEXPORT int pmgraph_candlestick_plot(PMPLOT *pmplot, int axis);
DLLEXPORT int pmgraph_bar_plot(PMPLOT *pmplot, int axis);
DLLEXPORT int pmgraph_fill_plot(PMPLOT *pmplot, int axis);
DLLEXPORT HWND pmgraph_zoom(HWND w, PMPLOT *pmplot, int newzoom);
DLLEXPORT void pmgraph_copy(PMPLOT *to, PMPLOT *from);
DLLEXPORT void pmgraph_xy_to_winxy(PMPLOT *pmplot, double x, double y, POINT *p);
DLLEXPORT void pmgraph_winxy_to_xy(PMPLOT *pmplot, POINT *p, double *px, double *py);
DLLEXPORT void pmgraph_destroy(PMPLOT *pmplot);
DLLEXPORT void pmgraph_polymarker(HDC hdc, long npoints, POINT *point_array, long marker, long marker_width, COLORREF marker_color);
DLLEXPORT void pmgraph_polystring(HDC hdc, long npoints, POINT *point_array, char *MarkerString, COLORREF MarkerColor);

/* in scale.c */
DLLEXPORT void pmgraph_scale(double Start, double End, double Lines, double *NewStart, double *NewEnd, double *NewStep);
