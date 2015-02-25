/* $Header: /home/cvs/src/gxplot/src/gxgraph.h,v 1.12 1998/10/09 22:14:20 procmon Exp $ */
/****************************************************************
**
**	GXGRAPH.H	- 2 Dimensional Graph Library
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined( IN_GXGRAPH_H )
#define IN_GXGRAPH_H

#if !defined( IN_GX_H )
#	include <gx.h>
#endif


/*
**	Define constants
*/

#define	GRAPH_MAGNITUDE_SIZE		32

// Attributes
#define	GRAPH_ATTR_X_GRID			0x0001
#define	GRAPH_ATTR_Y_GRID			0x0002
#define	GRAPH_ATTR_Y1_GRID			0x0004
#define	GRAPH_ATTR_LOGO				0x0008
#define	GRAPH_ATTR_OUTLINE			0x0010
#define	GRAPH_ATTR_DELETE_WEEKENDS	0x0020
#define	GRAPH_ATTR_INVERT_COLORS	0x0040

// Region types
#define	GRAPH_REGION_UNDEFINED		0
#define	GRAPH_REGION_TITLE			1
#define	GRAPH_REGION_SUBTITLE		2
#define	GRAPH_REGION_XTITLE			3
#define	GRAPH_REGION_YTITLE			4
#define	GRAPH_REGION_Y1TITLE		5
#define	GRAPH_REGION_XLABEL			6
#define	GRAPH_REGION_YLABEL			7
#define	GRAPH_REGION_Y1LABEL		8
#define	GRAPH_REGION_LOGO			9
#define	GRAPH_REGION_DATA			10

// Axis tick types
#define	GRAPH_TF_TICK_FULL	  		0x0000
#define	GRAPH_TF_TICK_HALF	  		0x0001
#define	GRAPH_TF_TICK_QUARTER		0x0002
#define	GRAPH_TF_TICK_NONE			0x0004
#define	GRAPH_TF_GRID_FULL	  		0x0000
#define	GRAPH_TF_GRID_HALF	  		0x0010
#define	GRAPH_TF_GRID_QUARTER 		0x0020
#define	GRAPH_TF_GRID_NONE	  		0x0040
#define	GRAPH_TF_TEXT_CENTER		0x0000
#define	GRAPH_TF_TEXT_OFFSET		0x0100
#define	GRAPH_TF_CONDENSE_SKIP		0x0000
#define	GRAPH_TF_CONDENSE_FIRST		0x1000
#define	GRAPH_TF_TICK_ABOVE			0x2000


/*
**	Define types
*/

typedef struct GraphStructure			GRAPH;
typedef struct GraphRegionStructure		GRAPH_REGION;
typedef struct GraphFontStructure		GRAPH_FONT;
typedef struct GraphTickInfoStructure	GRAPH_TICK_INFO;

typedef	void	(*GRAPH_PF_LEGEND)(		GRAPH *p );
typedef int		(*GRAPH_PF_DATA)(		GRAPH *p );
typedef double	(*GRAPH_PF_XLATE)(	 	GRAPH *p, double x );
typedef	int		(*GRAPH_PF_ABORT)(		GRAPH *p );
typedef int		(*GRAPH_PF_LABEL)(		GRAPH *p, GRAPH_TICK_INFO **TickTable, char *Magnitude, double Low, double High, void *AxisInfo );


/*
**	Structure used to hold regions of the graph for collision detection
*/

struct GraphRegionStructure
{
	struct GraphRegionStructure
			*Next;

	int		Type;

	GX_RECTANGLE
			BoundingBox;

	long	Number;

};	// GRAPH_REGION


/*
**	Structure used to hold temporary tick information
*/

struct GraphTickInfoStructure
{
	double	Value;

	char	Line1[ 16 ],
			Line2[ 16 ];

	int		Flags;

};	// GRAPH_TICK_INFO


/*
**	Structure used to hold font information
*/

struct GraphFontStructure
{
	char	*Name;

	double	Size;

};	// GRAPH_FONT


/*
**	Structure used to hold a graph
*/

struct GraphStructure
{
	int		xsize,
			ysize;

	double	xlow,
			xhigh,
			ylow,
			yhigh,
			y1low,
			y1high;

	int		Attrs;

	long	BackgroundColor,
			GraphColor,
			TitleColor,
			LineColor,
			LogoColor,
			XGridColor,
			YGridColor,
			Y1GridColor;

	double	BorderPercent;

	long	LineType,
			Marker,
			LineWidth;

	char	*title,
			*subtitle,
			*xtitle,
			*ytitle,
			*y1title,
			*BackgroundImage;

	char	XMagnitude[ 	GRAPH_MAGNITUDE_SIZE ],
			YMagnitude[		GRAPH_MAGNITUDE_SIZE ],
			Y1Magnitude[	GRAPH_MAGNITUDE_SIZE ];

	int		FontSizeType;

	GRAPH_FONT
			AxisFont,
			SubtitleFont,
			TitleFont;

	void	*XAxisInfo,
			*YAxisInfo,
			*Y1AxisInfo;

	GRAPH_PF_DATA
			DataFunc;

	GRAPH_PF_LEGEND
			LegendFunc;

	GRAPH_PF_LABEL
			xlabel_func,
			ylabel_func,
			y1label_func;

	GRAPH_PF_XLATE
			x_xlate,
			x_rev_xlate;

	GRAPH_PF_ABORT
			AbortFunc;

	GRAPH_REGION
			*RegionList;

	GX_RECTANGLE
			ibox,
			DataBox;

	GX_HANDLE
			*Gx;

	long	count;

	double	x, y, z,
			high, low, open;

	void	*Extra;

};	// GRAPH



/*
**	Prototype functions
*/

/* in graph.c */

DLLEXPORT GRAPH	*
		graph_create( 			void );

DLLEXPORT void
		graph_destroy(			GRAPH *Graph ),
		graph_copy(		   		GRAPH *to, GRAPH *from ),
		graph_size(				GRAPH *Graph, int xsize, int ysize ),
		graph_axes(				GRAPH *Graph ),
		graph_titles( 			GRAPH *Graph ),
		graph_xy_to_winxy(     	GRAPH *Graph, double x, double y, GX_POINT *p ),
		graph_winxy_to_xy(     	GRAPH *Graph, GX_POINT *p, double *px, double *py );

DLLEXPORT int
		graph_line_plot(		GRAPH *Graph, int axis ),
		graph_step_plot(		GRAPH *Graph, int axis ),
		graph_spline_plot(		GRAPH *Graph, int axis ),
		graph_volume_plot(		GRAPH *Graph, int axis ),
		graph_hloc_plot(		GRAPH *Graph, int axis ),
		graph_hlocfill_plot(	GRAPH *Graph, int axis ),
		graph_candlestick_plot(	GRAPH *Graph, int axis ),
		graph_bar_plot(			GRAPH *Graph, int axis ),
		graph_area_plot(		GRAPH *Graph, int axis ),
		graph_shade_plot(		GRAPH *Graph );

DLLEXPORT int
		graph_region_find(		GRAPH *Graph, GX_POINT *p, long *Number );

DLLEXPORT void
		GxGraphDrawText(	  	 		GX_HANDLE *Gx, char *s, GX_RECTANGLE *r, long fore, long back, unsigned int flags );

#endif


