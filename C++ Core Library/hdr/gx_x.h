/* $Header: /home/cvs/src/gx/src/gx_x.h,v 1.4 1998/10/09 22:14:19 procmon Exp $ */
/****************************************************************
**
**	GX_X.H		- X11 driver header
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_GX_X_H )
#define IN_GX_X_H


/*
**	X11 driver graphics handle 
*/

typedef struct XFontEntryStructure
{
	long	Size;

	GX_FONT_ENTRY
			*FontEntry;

	char
			XName[ 256 ];

	XFontStruct
			*XFont;

} X_FONT_ENTRY;


typedef struct XHandleStructure
{
	Display
			*XDisplay;

	GC
			XGraphicsContext;

	Drawable
			XWindow;

	int		X,
			Y,
			LineStyle;

	unsigned int
			LineWidth;

	long	CurrentColor,
			ColorInvert;

	unsigned long
			Pixel,
			InvertPixel;

	int		AreaCount,
			PointCount,
			AllocatedPoints;

	XPoint
			*PointBuffer;

	double	CharAngle;

	GX_HFONT
			CurrentFont;
	
	X_FONT_ENTRY
			*FontTable[ GX_FONT_MAX ];

} X_HANDLE;


typedef struct XSurfaceStructure
{
	Display
			*GxDisplay;

	Drawable
			GxWindow;

	Region
			GxRegion;

} X_SURFACE;


/*
**	Macros
*/

#define	XH		((X_HANDLE *) Gx->Handle)
#define XWIN	XH->XWindow
#define XDPY	XH->XDisplay
#define XGC		XH->XGraphicsContext
#define XROOT	RootWindow( XH->DisplayHandle, DefaultScreen( XH->DisplayHandle ))

#define FLIP( Y )		( Gx->ySize - Y )

/*
**	Prototype functions
*/

int		
		GxXCreate(			GX_HANDLE *Handle, void *Arg, long xSize, long ySize ),
		GxXMove(			GX_HANDLE *Handle, GX_POINT *Point ),
		GxXLine(			GX_HANDLE *Handle, GX_POINT *Point ),
		GxXRMove(	   		GX_HANDLE *Handle, GX_POINT *Point ),
		GxXRLine(	   		GX_HANDLE *Handle, GX_POINT *Point ),
		GxXPolyLine(		GX_HANDLE *Handle, long Count, GX_POINT *Points ),
		GxXSetColor(		GX_HANDLE *Handle, long Color ),
		GxXSetColorInvert(	GX_HANDLE *Handle, int Value ),
		GxXSetLineType(		GX_HANDLE *Handle, long LineType ),
		GxXBeginArea(		GX_HANDLE *Handle ),
		GxXEndArea(			GX_HANDLE *Handle ),
		GxXCreateFont(		GX_HANDLE *Handle, GX_HFONT HFont, GX_FONT_ENTRY *FontEntry, double Size, int SizeType ),
		GxXDestroyFont(		GX_HANDLE *Handle, GX_HFONT HFont ),
		GxXSetFont(			GX_HANDLE *Handle, GX_HFONT HFont ),
		GxXDrawText(		GX_HANDLE *Handle, char *Text, int Flags ),
		GxXErase(			GX_HANDLE *Handle ),
		GxXBitmapDraw(		GX_HANDLE *Handle, GX_BITMAP *Bitmap, long xSize, long ySize, long *ColorMap ),
		GxXImageLoad(		GX_HANDLE *Handle, char *FileName, GX_IMAGE_LINK *ImageLink ),
		GxXImageDraw(		GX_HANDLE *Handle, GX_IMAGE_LINK *ImageLink, long xSize, long ySize ),
		GxXSetMarker(		GX_HANDLE *Handle, long MarkerType ),
		GxXMarker(			GX_HANDLE *Handle, GX_POINT *Point ),
		GxXPolyMarker(		GX_HANDLE *Handle, long Count, GX_POINT *Points ),
		GxXPolySpline(		GX_HANDLE *Handle, long Count, GX_POINT *Points ),
		GxXSetLineWidth(	GX_HANDLE *Handle, long LineWidth ),
		GxXBeginPath(		GX_HANDLE *Handle ),
		GxXEndPath(			GX_HANDLE *Handle ),
		GxXStrokePath(		GX_HANDLE *Handle ),
		GxXBox(				GX_HANDLE *Handle, long Flags, GX_POINT *Corner ),
		GxXQueryTextBox(	GX_HANDLE *Handle, char *String, GX_POINT *TextBox ),
		GxXQueryFontMetrics(GX_HANDLE *Handle, GX_FONT_METRICS *FontMetrics ),
		GxXSetCharAngle(	GX_HANDLE *Handle, double vCharAngle ),
		GxXQueryCharAngle(	GX_HANDLE *Handle, double *CharAngle ),
		GxXGetColor(		GX_HANDLE *Handle, long Color );

void	
		GxXDestroy(			GX_HANDLE *Handle ),
		GxXImageUnload(		GX_IMAGE_LINK *GxImageLink );

XImage *
		GxXLoadBMP(			GX_HANDLE *Gx, char *FileName );

#endif
