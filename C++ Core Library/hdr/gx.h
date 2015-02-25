/* $Header: /home/cvs/src/gx/src/gx.h,v 1.23 2001/11/27 22:48:15 procmon Exp $ */
/****************************************************************
**
**	GX.H		- Graphics API
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.23 $
**
****************************************************************/

#if !defined( IN_GX_H )
#define IN_GX_H


/*
**	Define types
*/

typedef struct GxHandleStructure		GX_HANDLE;
typedef struct GxPointStructure			GX_POINT;
typedef struct GxRectangleStructure		GX_RECTANGLE;
typedef struct GxBitmapStructure		GX_BITMAP;
typedef struct GxImageLinkStructure		GX_IMAGE_LINK;
typedef struct GxImageStructure			GX_IMAGE;
typedef struct GxImageInfoStructure		GX_IMAGE_INFO;
typedef struct GxFontEntryStructure		GX_FONT_ENTRY;
typedef struct GxFontMetricsStructure	GX_FONT_METRICS;
typedef struct GxMagnitudeStructure		GX_MAGNITUDE;

typedef	int								GX_HFONT;


/*
**	Define driver types (GxCreate)
*/

#define	GX_DRIVER_COUNT				5

#define	GX_DRIVER_PS				0
#define	GX_DRIVER_PM				1
#define	GX_DRIVER_X					2
#define	GX_DRIVER_GIF				3
#define	GX_DRIVER_WIN				4


/*
**	Define line types (GxSetLineType)
*/

#define	GX_LINETYPE_INVISIBLE		0
#define	GX_LINETYPE_SOLID			1
#define	GX_LINETYPE_DOT				2
#define	GX_LINETYPE_SHORTDASH		3
#define	GX_LINETYPE_DASHDOT			4
#define	GX_LINETYPE_DOUBLEDOT		5
#define	GX_LINETYPE_LONGDASH		6
#define	GX_LINETYPE_DASHDOUBLEDOT	7
#define	GX_LINETYPE_ALTERNATE		8


/*
**	Define mix modes (GxSetMixMode)
*/

#define	GX_MIXMODE_AND              0
#define	GX_MIXMODE_INVERT           1
#define	GX_MIXMODE_LEAVEALONE       2
#define	GX_MIXMODE_MASKSRCNOT       3
#define	GX_MIXMODE_MERGENOTSRC      4
#define	GX_MIXMODE_MERGESRCNOT      5
#define	GX_MIXMODE_NOTCOPYSRC       6
#define	GX_MIXMODE_NOTMASKSRC       7
#define	GX_MIXMODE_NOTMERGESRC      8
#define	GX_MIXMODE_NOTXORSRC        9
#define	GX_MIXMODE_ONE              10
#define	GX_MIXMODE_OR               11
#define	GX_MIXMODE_OVERPAINT        12
#define	GX_MIXMODE_SUBTRACT         13
#define	GX_MIXMODE_XOR              14
#define	GX_MIXMODE_ZERO             15


/*
**	Define size types (GxCreateFont)
*/

#define	GX_ST_HEIGHT				0
#define	GX_ST_POINTS				1
#define	GX_ST_TWIPS					2
#define	GX_ST_XPERCENT				3
#define	GX_ST_YPERCENT				4


/*
**	Define draw text types (GxDrawText)
*/

#define	GX_DT_LEFT					0x0001
#define	GX_DT_RIGHT					0x0002
#define	GX_DT_TOP					0x0004
#define	GX_DT_BOTTOM				0x0008
#define	GX_DT_QUERYEXTENT			0x0010

#define	GX_DT_HCENTER				(GX_DT_LEFT | GX_DT_RIGHT)
#define	GX_DT_VCENTER				(GX_DT_TOP | GX_DT_BOTTOM)
#define	GX_DT_CENTER				(GX_DT_HCENTER | GX_DT_VCENTER)
#define	GX_DT_NORMAL				(GX_DT_LEFT | GX_DT_BOTTOM)


/*
**	Define text box things (GxQueryTextBox)
*/

#define	GX_TXTBOX_COUNT				5

#define	GX_TXTBOX_TOPLEFT			0
#define	GX_TXTBOX_BOTTOMLEFT		1
#define	GX_TXTBOX_TOPRIGHT			2
#define	GX_TXTBOX_BOTTOMRIGHT		3
#define	GX_TXTBOX_CONCAT			4


/*
**	Define marker types (GxSetMarker)
*/

#define GX_MARKTYPE_BLANK			0L
#define GX_MARKTYPE_CROSS           1L
#define GX_MARKTYPE_PLUS            2L
#define GX_MARKTYPE_DIAMOND         3L
#define GX_MARKTYPE_SQUARE          4L
#define GX_MARKTYPE_SIXPOINTSTAR    5L
#define GX_MARKTYPE_EIGHTPOINTSTAR  6L
#define GX_MARKTYPE_SOLIDDIAMOND    7L
#define GX_MARKTYPE_SOLIDSQUARE     8L
#define GX_MARKTYPE_DOT             9L
#define GX_MARKTYPE_SMALLCIRCLE     10L
#define GX_MARKTYPE_UPARROW         11L
#define GX_MARKTYPE_DOWNARROW       12L


/*
**	Define box flags (GxBox)
*/

#define	GX_DRO_FILL					1L
#define	GX_DRO_OUTLINE				2L
#define	GX_DRO_OUTLINEFILL			3L


/*
**	Define bitmap types (GxBitmapCreate)
*/

#define	GX_BMT_1BIT					1
#define	GX_BMT_8BIT					8
#define	GX_BMT_24BIT				24


/*
**	Define font information
*/

#define	GX_FONT_MAX					256

#define	GX_FONT_BOLD				0x0001
#define	GX_FONT_ITALIC				0x0002

#define	GX_FONT_BY_NAME				0x0001
#define	GX_FONT_BY_FAMILY			0x0002


/*
**	Point structure
*/

struct GxPointStructure
{
	long	x,
			y;

};	// GX_POINT


/*
**	Rectangle structure
*/

struct GxRectangleStructure
{
	long	xLeft,
			yBottom,
			xRight,
			yTop;

};	// GX_RECTANGLE


/*
**	Bitmap structure
*/

struct GxBitmapStructure
{
	unsigned char
			*Buffer;

	long	xSize,
			ySize;

	int		Type;

	unsigned int
			ByteCount;

};	// GX_BITMAP


/*
**	Image structure
*/

struct GxImageLinkStructure
{
	GX_IMAGE_LINK
			*Next;

	int		DriverType;

	struct
	{
		long	Number;

		void	*Pointer;

	} DriverData;

};	// GX_IMAGE_LINK

struct GxImageStructure
{
	GX_HANDLE
			*GxHandle;

	char	*FileName;

	GX_IMAGE_LINK
			*Links;

};	// GX_IMAGE

struct GxImageInfoStructure
{
	long	xSize,
			ySize,
			Bits;

};	// GX_IMAGE_INFO


/*
**	Font entry structure
*/

struct GxFontEntryStructure
{
	char	*Family,
			*FontName,
			*FileName,
			*PmName;

	int		Flags;

	void	*DriverData[ GX_DRIVER_COUNT ];

};	// GX_FONT_ENTRY


/*
**	Font metrics structure
*/

struct GxFontMetricsStructure
{
	GX_FONT_ENTRY
			*FontEntry;

	long	MaxAscender,		// Space above the baseline
			MaxDescender,		// Space below the baseline
			MaxHeight,			// Ascender - Descender
			UpperHeight,		// Height of upper case characters
			LowerHeight;		// Height of lower case characters

	double	ItalicAngle;		// Slant in degrees

	GX_RECTANGLE
			FontBBox;

};	// GX_FONT_METRICS


/*
**	Structure used to return magnitude information
*/

struct GxMagnitudeStructure
{
	double	Divisor;

	char	Description[ 16 ];

};	// GX_MAGNITUDE


/*
**	Structure used to hold GxHandle data for a datatype
*/

typedef struct DtGxStructure
{
	int		ReferenceCount;

	GX_HANDLE
			*GxHandle;

} DT_GX;


/*
**	Structure used to hold GxImage data for a datatype
*/

typedef struct DtGxImageStructure
{
	int		ReferenceCount;

	GX_IMAGE
			*GxImage;

} DT_GX_IMAGE;


/*
**	Graphic drawing space handle
*/

typedef	int		(*GX_PF_DRIVERINIT)			(	GX_HANDLE *Handle );
typedef	int		(*GX_PF_CREATE)				(	GX_HANDLE *Handle, void *Arg, long xSize, long ySize );
typedef	void 	(*GX_PF_DESTROY)			(	GX_HANDLE *Handle );
typedef int		(*GX_PF_IMAGELOAD)			(	GX_HANDLE *Handle, char *FileName, GX_IMAGE_LINK *GxImageLink );
typedef void	(*GX_PF_IMAGEUNLOAD)		(	GX_IMAGE_LINK *GxImageLink );
typedef int		(*GX_PF_IMAGEDRAW)			(	GX_HANDLE *Handle, GX_IMAGE_LINK *GxImageLink, long xSize, long ySize );
typedef int		(*GX_PF_IMAGEINFO)			(	GX_HANDLE *Handle, GX_IMAGE_LINK *GxImageLink, GX_IMAGE_INFO *GxImageInfo );

typedef	int		(*GX_PF_MOVE)				(	GX_HANDLE *Handle, GX_POINT *Point );
typedef	int 	(*GX_PF_LINE)				(	GX_HANDLE *Handle, GX_POINT *Point );
typedef	int		(*GX_PF_RMOVE)				(	GX_HANDLE *Handle, GX_POINT *Point );
typedef	int 	(*GX_PF_RLINE)				(	GX_HANDLE *Handle, GX_POINT *Point );
typedef	int 	(*GX_PF_POLYLINE)			(	GX_HANDLE *Handle, long Count, GX_POINT *Points );
typedef int 	(*GX_PF_SETCOLOR)			(	GX_HANDLE *Handle, long Color );
typedef int 	(*GX_PF_SETCOLORINVERT)		(	GX_HANDLE *Handle, int value );
typedef int 	(*GX_PF_SETLINETYPE)		(	GX_HANDLE *Handle, long LineType );
typedef int 	(*GX_PF_BEGINAREA)			(	GX_HANDLE *Handle );
typedef int 	(*GX_PF_ENDAREA)			(	GX_HANDLE *Handle );
typedef int		(*GX_PF_CREATEFONT)			(	GX_HANDLE *Handle, GX_HFONT HFont, GX_FONT_ENTRY *FontEntry, double Size, int SizeType );
typedef int		(*GX_PF_DESTROYFONT)		(	GX_HANDLE *Handle, GX_HFONT HFont );
typedef int 	(*GX_PF_SETFONT)			(	GX_HANDLE *Handle, GX_HFONT HFont );
typedef int 	(*GX_PF_DRAWTEXT)			(	GX_HANDLE *Handle, char *Text, int Flags );
typedef	int		(*GX_PF_ERASE)				(	GX_HANDLE *Handle );
typedef	int		(*GX_PF_BITMAPDRAW)			(	GX_HANDLE *Handle, GX_BITMAP *Bitmap, long xSize, long ySize, long *ColorTable );
typedef int		(*GX_PF_SETMARKER)			(	GX_HANDLE *Handle, long MarkerType );
typedef int		(*GX_PF_MARKER)				(	GX_HANDLE *Handle, GX_POINT *Point );
typedef int		(*GX_PF_POLYMARKER)			(	GX_HANDLE *Handle, long Count, GX_POINT *Points );
typedef int		(*GX_PF_POLYSPLINE)			(	GX_HANDLE *Handle, long Count, GX_POINT *Points );
typedef int		(*GX_PF_SETLINEWIDTH)		(	GX_HANDLE *Handle, long LineWidth );
typedef int		(*GX_PF_BEGINPATH)			(	GX_HANDLE *Handle );
typedef int		(*GX_PF_ENDPATH)			(	GX_HANDLE *Handle );
typedef int		(*GX_PF_STROKEPATH)			(	GX_HANDLE *Handle );
typedef int		(*GX_PF_BOX)				(	GX_HANDLE *Handle, long Flags, GX_POINT *Corner );
typedef int		(*GX_PF_QUERYTEXTBOX)		(	GX_HANDLE *Handle, char *String, GX_POINT *TextBox );
typedef int		(*GX_PF_QUERYFONTMETRICS)	(	GX_HANDLE *Handle, GX_FONT_METRICS *FontMetrics );
typedef int		(*GX_PF_SETCHARANGLE)		(	GX_HANDLE *Handle, double CharAngle );
typedef	int		(*GX_PF_SETMIXMODE)			(	GX_HANDLE *Handle, long MixMode );
typedef	int		(*GX_PF_ARCANGLE)			(	GX_HANDLE *Handle, GX_POINT *Center, double Angle, double Sweep, double XRadius, double YRadius, double Rotation );
typedef int		(*GX_PF_WRITECLIP)			(	GX_HANDLE *Handle );


struct GxHandleStructure
{
	void	*Handle;

	long	xSize,
			ySize,
			Color,
			ColorInvert,
			LineType,
			LineWidth,
			MarkerType,
			MixMode;

	double	CharAngle;

	int		DriverType;

	GX_PF_CREATE			pfCreate;
	GX_PF_DESTROY			pfDestroy;
	GX_PF_MOVE				pfMove;
	GX_PF_LINE				pfLine;
	GX_PF_RMOVE				pfRMove;
	GX_PF_RLINE				pfRLine;
	GX_PF_POLYLINE			pfPolyLine;
	GX_PF_SETCOLOR			pfSetColor;
	GX_PF_SETCOLORINVERT	pfSetColorInvert;
	GX_PF_SETLINETYPE		pfSetLineType;
	GX_PF_BEGINAREA			pfBeginArea;
	GX_PF_ENDAREA			pfEndArea;
	GX_PF_CREATEFONT		pfCreateFont;
	GX_PF_DESTROYFONT		pfDestroyFont;
	GX_PF_SETFONT			pfSetFont;
	GX_PF_DRAWTEXT			pfDrawText;
	GX_PF_ERASE				pfErase;
	GX_PF_BITMAPDRAW		pfBitmapDraw;
	GX_PF_IMAGELOAD			pfImageLoad;
	GX_PF_IMAGEUNLOAD		pfImageUnload;
	GX_PF_IMAGEDRAW			pfImageDraw;
	GX_PF_IMAGEINFO			pfImageInfo;
	GX_PF_SETMARKER			pfSetMarker;
	GX_PF_MARKER			pfMarker;
	GX_PF_POLYMARKER		pfPolyMarker;
	GX_PF_POLYSPLINE		pfPolySpline;
	GX_PF_SETLINEWIDTH		pfSetLineWidth;
	GX_PF_BEGINPATH			pfBeginPath;
	GX_PF_ENDPATH			pfEndPath;
	GX_PF_STROKEPATH		pfStrokePath;
	GX_PF_BOX				pfBox;
	GX_PF_QUERYTEXTBOX		pfQueryTextBox;
	GX_PF_QUERYFONTMETRICS	pfQueryFontMetrics;
	GX_PF_SETCHARANGLE		pfSetCharAngle;
	GX_PF_SETMIXMODE		pfSetMixMode;
	GX_PF_ARCANGLE			pfArcAngle;
	GX_PF_WRITECLIP			pfWriteClip;

}; // GX_HANDLE


/*
**	Helper macros
*/

#define	GX_BITMAP_CELL(b,x,y)			((b)->Buffer + (unsigned)(y) * (unsigned)(b)->xSize + (unsigned)(x))

#define	GX_MAKE_RGB(Red,Green,Blue)		((((Red) & 0xffl) << 16) | (((Green) & 0xffl) << 8) | ((Blue) & 0xffl))
#define	GX_MAKE_RED(Color)				(((Color) >> 16) & 0xffl)
#define	GX_MAKE_GREEN(Color)			(((Color) >> 8) & 0xffl)
#define	GX_MAKE_BLUE(Color)				((Color) & 0xffl)


/*
**	Graphic macros
*/

#define	GxMove(h,p)					((*(h)->pfMove)				((h),(p)))
#define	GxLine(h,p)					((*(h)->pfLine)				((h),(p)))
#define	GxRMove(h,p)	 			((*(h)->pfRMove)			((h),(p)))
#define	GxRLine(h,p)	 			((*(h)->pfRLine)			((h),(p)))
#define	GxPolyLine(h,c,p)			((*(h)->pfPolyLine)			((h),(c),(p)))
#define	GxSetColor(h,c)				((*(h)->pfSetColor)			((h),(c)))
#define	GxSetColorInvert(h,v)		((*(h)->pfSetColorInvert)	((h),(v)))
#define	GxSetLineType(h,t)			((*(h)->pfSetLineType)		((h),(t)))
#define	GxBeginArea(h)				((*(h)->pfBeginArea)		((h)))
#define	GxEndArea(h)				((*(h)->pfEndArea)			((h)))
#define	GxCreateFont(h,fh,fe,s,f)	((*(h)->pfCreateFont)		((h),(fh),(fe),(s),(f)))
#define	GxDestroyFont(h,fh)	   		((*(h)->pfDestroyFont)	  	((h),(fh)))
#define	GxSetFont(h,fh)				((*(h)->pfSetFont)			((h),(fh)))
#define	GxDrawText(h,t,f) 			((*(h)->pfDrawText)			((h),(t),(f)))
#define	GxErase(h)					((*(h)->pfErase)			((h)))
#define	GxBitmapDraw(h,b,x,y,m)		((*(h)->pfBitmapDraw)		((h),(b),(x),(y),(m)))
#define	GxSetMarker(h,m)			((*(h)->pfSetMarker)		((h),(m)))
#define	GxMarker(h,p)				((*(h)->pfMarker)			((h),(p)))
#define	GxPolyMarker(h,c,p)			((*(h)->pfPolyMarker)		((h),(c),(p)))
#define	GxPolySpline(h,c,p)			((*(h)->pfPolySpline)		((h),(c),(p)))
#define	GxSetLineWidth(h,w)			((*(h)->pfSetLineWidth)		((h),(w)))
#define	GxBeginPath(h)				((*(h)->pfBeginPath)		((h)))
#define	GxEndPath(h)				((*(h)->pfEndPath)			((h)))
#define	GxStrokePath(h)				((*(h)->pfStrokePath)		((h)))
#define	GxBox(h,f,p)				((*(h)->pfBox)				((h),(f),(p)))
#define	GxQueryTextBox(h,s,p)		((*(h)->pfQueryTextBox)		((h),(s),(p)))
#define	GxQueryFontMetrics(h,fm)	((*(h)->pfQueryFontMetrics)	((h),(fm)))
#define	GxSetCharAngle(h,a)			((*(h)->pfSetCharAngle)		((h),(a)))
#define	GxSetMixMode(h,mm)			((*(h)->pfSetMixMode)		((h),(mm)))
#define	GxArcAngle(h,c,a,s,xr,yr,r)	((*(h)->pfArcAngle)			((h),(c),(a),(s),(xr),(yr),(r)))
#define	GxWriteClip(h)				((*(h)->pfWriteClip)		((h)))

#define	GxQueryColor(h)				((h)->Color)
#define	GxQueryColorInvert(h)		((h)->ColorInvert)
#define	GxQueryLineType(h)			((h)->LineType)
#define	GxQueryLineWidth(h)			((h)->LineWidth)
#define	GxQueryMarkerType(h)		((h)->MarkerType)
#define	GxQueryMixMode(h)			((h)->MixMode)


#if defined( IN_KOOL_ADE_H )
DLLEXPORT CONFIGURATION *GxConfig;
#endif

/*
**	Prototype functions
*/

DLLEXPORT int		GxInitialize(		void );

DLLEXPORT GX_HANDLE
		*GxCreate(			int DriverType, void *Arg, long xSize, long ySize );

DLLEXPORT void
		GxDestroy(			GX_HANDLE *GxHandle );

DLLEXPORT GX_BITMAP
		*GxBitmapCreate(	int Type, long xSize, long ySize );

DLLEXPORT void
		GxBitmapDestroy(	GX_BITMAP *Bitmap );

DLLEXPORT GX_IMAGE
		*GxImageLoad(		char *FileName );

DLLEXPORT void
		GxImageUnload(		GX_IMAGE *GxImage );

DLLEXPORT int
		GxImageDraw(		GX_HANDLE *GxHandle, GX_IMAGE *GxImage, long xSize, long ySize ),
		GxImageQueryInfo( 	GX_HANDLE *GxHandle, GX_IMAGE *GxImage, GX_IMAGE_INFO *GxImageInfo );

DLLEXPORT GX_FONT_ENTRY
		*GxFontMatch(		char *Name, int Flags, int MatchBy );

DLLEXPORT int
		GxFontTableLoad(	char *FileName );

DLLEXPORT long
		GxHSV(				double Hue, double Saturation, double Value );

DLLEXPORT void
		GxGraphScale(		double Start, double End, double Lines, double *NewStart, double *NewEnd, double *NewStep ),
		GxGetMagnitude(		double Number, GX_MAGNITUDE *Magnitude );

#endif
