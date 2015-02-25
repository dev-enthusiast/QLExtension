/* $Header: /home/cvs/src/gxplot/src/2ddata.h,v 1.10 1998/10/09 22:14:20 procmon Exp $ */
/****************************************************************
**
**	2DDATA.H	- 2 Dimensional graph data types
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_2DDATA_H )
#define IN_2DDATA_H

#if !defined( IN_DATATYPE_H )
#	include	<datatype.h>
#endif

/*
**	Define element styles
*/

#define CHART2D_STYLE_UNDEFINED	   		(-1)
#define CHART2D_STYLE_NORMAL			0
#define CHART2D_STYLE_SPLINE			1
#define CHART2D_STYLE_BAR		   		2
#define CHART2D_STYLE_CANDLE			3
#define CHART2D_STYLE_VOLUME			4
#define CHART2D_STYLE_STEP				5
#define CHART2D_STYLE_AREA				6
#define	CHART2D_STYLE_FILL				7
#define CHART2D_STYLE_SHADE				8


/*
**	Define element flags
*/

#define	CHART2D_FLAG_ENABLED			0x0001
#define	CHART2D_FLAG_VISIBLE			0x0002
#define	CHART2D_FLAG_SHOW_NORMAL		0x0004
#define	CHART2D_FLAG_SHOW_MEAN			0x0008
#define	CHART2D_FLAG_SHOW_STD			0x0010
#define	CHART2D_FLAG_SHOW_REGLINE		0x0020


/*
**	Define plot types
*/

#define CHART2D_TYPE_TIME_SERIES	 	0
#define CHART2D_TYPE_HISTOGRAM		 	1
#define CHART2D_TYPE_SCATTER_PLOT	 	2
#define CHART2D_TYPE_MATURITY		 	3


/*
**	Define defaults
*/

#define	CHART2D_ELEM_DEF_AXIS			0
#define	CHART2D_ELEM_DEF_FLAGS			(CHART2D_FLAG_ENABLED | CHART2D_FLAG_VISIBLE)
#define	CHART2D_ELEM_DEF_STYLE			CHART2D_STYLE_UNDEFINED
#define	CHART2D_ELEM_DEF_COLOR			(0x007f7f7fL)
#define	CHART2D_ELEM_DEF_LINETYPE		GX_LINETYPE_SOLID
#define	CHART2D_ELEM_DEF_LINEWIDTH		1
#define	CHART2D_ELEM_DEF_MARKERTYPE		GX_MARKTYPE_BLANK

#define	CHART2D_DEF_BACKGROUND_COLOR	(0x00000000L)
#define	CHART2D_DEF_GRAPH_COLOR			(0x00101010L)
#define	CHART2D_DEF_TITLE_COLOR			(0x00ffffffL)
#define	CHART2D_DEF_LOGO_COLOR			(0x009f9f9fL)
#define	CHART2D_DEF_XGRID_COLOR			(0x005f5f5fL)
#define	CHART2D_DEF_YGRID_COLOR			(0x005f5f5fL)
#define	CHART2D_DEF_Y1GRID_COLOR		(0x003f3f3fL)
#define	CHART2D_DEF_ATTRS				(GRAPH_ATTR_X_GRID | GRAPH_ATTR_Y_GRID)
#define	CHART2D_DEF_TYPE				CHART2D_TYPE_TIME_SERIES
#define	CHART2D_DEF_BORDER_PERCENT		(5.0)
#define	CHART2D_DEF_FONT_SIZE_TYPE		GX_ST_POINTS;
#define	CHART2D_DEF_FONT_NAME			"Helvetica"
#define	CHART2D_DEF_AXIS_FONT_SIZE		12.0
#define	CHART2D_DEF_TITLE_FONT_SIZE		20.0
#define	CHART2D_DEF_SUBTITLE_FONT_SIZE	12.0
#define	CHART2D_DEF_RELATIVE			FALSE


/*
**	Define types
*/

typedef struct Chart2dStructure			CHART2D;

typedef	void	(*CHART2D_PF_DRAW_IMAGE)(		CHART2D *Chart, struct GxHandleStructure *Gx, long ImageSizeX, long ImageSizeY );
typedef	void	(*CHART2D_PF_DEVICE_DESTROY)(	CHART2D *Chart );
typedef	void	(*CHART2D_PF_NEW_CHART)(		CHART2D *Chart );


/*
**	Structure used to contain chart elements
*/

typedef struct Chart2dElemStructure
{
	DT_VALUE
			Axis,
			Color,
			Flags,
			Label,
			LineType,
			LineWidth,
			MarkerType,
			Style,
			Value;

	DT_VALUE
			FirstValue,
			FirstDate,
			LastValue,
			LastDate,
			HighValue,
			HighDate,
			LowValue,
			LowDate,
			Points,
			Mean,
			StdDev;

} CHART2D_ELEM;


/*
**	Structure used to contain a chart
*/

struct Chart2dStructure
{
	int		ReferenceCount;

	CHART2D_PF_DRAW_IMAGE
			pfDrawImage;

	void	*DeviceHandle;

	CHART2D_PF_DEVICE_DESTROY
			pfDeviceDestroy;

	DT_VALUE
			AxisFontName,
			AxisFontSize,
			BackgroundColor,
			BackgroundImage,
			BorderPercent,
			Elements,
			End,
			Flags,
			FontSizeType,
			GraphColor,
			LogoColor,
			Relative,
			Start,
			Subtitle,
			SubtitleFontName,
			SubtitleFontSize,
			Title,
			TitleColor,
			TitleFontName,
			TitleFontSize,
			Type,
			XGridColor,
			XTitle,
			YGridColor,
			YTitle,
			Y1GridColor,
			Y1Title;

	void	*Extra;

};	// CHART2D


#endif
