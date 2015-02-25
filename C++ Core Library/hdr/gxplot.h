/* $Header: /home/cvs/src/gxplot/src/gxplot.h,v 1.9 2000/05/17 10:36:56 goodfj Exp $ */
/****************************************************************
**
**	GXPLOT.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_GXPLOT_H )
#define IN_GXPLOT_H

#if !defined( IN_DATATYPE_H )
#	include	<datatype.h>
#endif

#if !defined( IN_TURBO_H )
#	include	<turbo.h>
#endif

#if !defined( IN_2DDATA_H )
#	include	<2ddata.h>
#endif

typedef struct expression
{
	char	*s,
			*label;

	long	Marker,
			Color,
			LineType,
			LineWidth,
			Style;

	int 	axis;

	unsigned
			enabled			: 1,
			visible			: 1,
			show_normal 	: 1,   	// For histograms
			show_mean 		: 1,
			show_std  		: 1,
			show_regline 	: 1;	// For scatter plots

	DT_VALUE
			Value;

	SYMBOL	out_sym;

	/* some statistical information computed gratis */
	TSDB_POINT	first,
			last,
			high,
			low;

	double	nobs,
			mean,
			std,
			sum_x,
			sum_x2,
			sum_xy;

	long	*histo;

	struct expression
			*next;

} EXPRESSION;

typedef struct
{
	int		code,			// code == 0 means a normal fixed date
			count;

} RELATIVE_DATE;


/*
**	Structure used to hold axis formatting flags
*/

typedef struct axis_format_struct
{
	unsigned
			Year			: 1,
			Month			: 1,
			Day				: 1,
			Dow				: 1,
			Hour 			: 1,
			Minute			: 1,
			Second			: 1,
			DelWeekend		: 1,

			Rolling			: 1,	// FIX - SHOULD BE REMOVED
			OpenClose 		: 1,
			Legend			: 1,

			Round 			: 1,
			Logscale		: 1,
			Fractions		: 1,
			ManualScale		: 1,
			Concise			: 1,
			Commas			: 1,
			Percent			: 1,
			AutoMagnitude	: 1;

	int		Decimals,
			Ticks;

	double	Divisor;

} AXIS_FORMAT;


typedef struct parms_struct
{
	GRAPH	*Graph;

	int		TurnedOff,
			Evaluated,
			RealTime,			// Real time graph
			Relative,			// Relative time axis
			Type;

	EXPRESSION
			*expr;

	int		changed;

	SYMBOL_TABLE
			out_table;

	int		xbuckets;			// for histograms only

	long	view_xlow;
	RELATIVE_DATE
			view_xlow_rdate;

	long	view_xhigh;
	RELATIVE_DATE
			view_xhigh_rdate;

	long	xopen,
			xclose;

	double	numeric_xhigh,		// for histograms and scatter plots only
			numeric_xlow;

	AXIS_FORMAT
			x_format,
			y_format,
			y1_format;

	double	yhigh,				// These values are used only when autoscale is off
			ylow,
			y1high,
			y1low;

	char	error_string[256];

} PLOT_PARMS;


/*
**	Prototype functions
*/

// In gxplot.c
DLLEXPORT GRAPH
		*GxPlotNew(		int RealTime, int Relative );

DLLEXPORT void
		GxPlotDestroy(	GRAPH *Graph ),
		GxPlotPaint(	GRAPH *Graph, PLOT_PARMS *parms );

#endif

