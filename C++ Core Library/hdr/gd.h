/* $Header: /home/cvs/src/gd/src/gd.h,v 1.7 1998/10/09 22:12:12 procmon Exp $ */
/****************************************************************
**
**	GD.H	
**
**	Created by the Quest Protein Database Center
**	Cold Spring Harbor Labs
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_GD_H )
#define IN_GD_H
#define GD_H 1

/* gd.h: declarations file for the gifdraw module.

	Written by Tom Boutell, 5/94.
	Copyright 1994, Cold Spring Harbor Labs.
	Permission granted to use this code in any fashion provided
	that this notice is retained and any alterations are
	labeled as such. It is requested, but not required, that
	you share extensions to this module with us so that we
	can incorporate them into new versions. */


/*
**	Define constants
*/

#define	gdMaxColors 	256		// Part of the GIF spec - don't change

// Special colors
#define gdStyled		(-2)
#define gdBrushed		(-3)
#define gdStyledBrushed (-4)
#define gdTiled			(-5)
#define gdTransparent	(-6)	// Used in line styles only

// Line types
#define gdLineStyle_INVISIBLE		0
#define gdLineStyle_SOLID			1
#define gdLineStyle_DOT				2
#define gdLineStyle_SHORTDASH		3
#define gdLineStyle_DASHDOT			4
#define gdLineStyle_DOUBLEDOT		5
#define gdLineStyle_LONGDASH		6
#define gdLineStyle_DASHDOUBLEDOT	7
#define gdLineStyle_ALTERNATE		8


/*
**	Define types
*/

typedef struct gdImageStruct		gdImage;
typedef gdImage						*gdImagePtr;
typedef struct gdPointStruct		gdPoint;
typedef gdPoint						*gdPointPtr;
typedef struct gdFontStruct			gdFont;
typedef gdFont						*gdFontPtr;


/*
**	Structure used to hold points
*/

struct gdPointStruct
{
	int		x,
			y;

};	// gdPoint, *gdPointPtr


/*
**	Structure used to hold fonts
*/

struct gdFontStruct
{
	int		nchars,		// Number of characters in font
			offset,		// First character is numbered... (usually 32 = space)
			w,			// Character width
			h;			// Character height

	char	*data;		// Font data - Array of chars one row after another

};	// gdFont, *gdFontPtr


/*
**	Image type. See functions below; you will not need to change
**	the elements directly. Use the provided macros to
**	access sx, sy, the color table, and colorsTotal for
**	read-only purposes.
*/

struct gdImageStruct
{
	unsigned char
			**pixels;

	int		sx,
			sy,
			colorsTotal,
			red[			gdMaxColors ],
			green[			gdMaxColors ],
			blue[			gdMaxColors ],
			open[			gdMaxColors ],
			transparent,
			*polyInts,
			polyAllocated;

	struct gdImageStruct
			*brush,
			*tile;	

	int		brushColorMap[	gdMaxColors ],
			tileColorMap[	gdMaxColors ],
			styleLength,
			stylePos,
			*style,
			interlace;

	int		InPath : 1,
			InArea : 1;

	int		CurrentColor,
			LineStyle,
			LineStyleBitPos;

	gdPoint	CurrentPoint;

	int		PathCount,
			PathLimit;

	gdPoint	*Path;

};	// gdImage, *gdImagePtr


/*
**	Prototype functions
*/

// Load, Save, Create & Destroy functions
DLLEXPORT gdImagePtr
		gdImageCreate(			int sx, int sy ),
		gdImageCreateFromGif(	FILE *fd ),
		gdImageCreateFromGd(	FILE *fd ),
		gdImageCreateFromXbm(	FILE *fd );

DLLEXPORT void
		gdImageDestroy(			gdImagePtr im ),
		gdImageGif(				gdImagePtr im, FILE *out ),
		gdImageGd(				gdImagePtr im, FILE *out );

// Drawing primitives
DLLEXPORT void
		gdImageMove(			gdImagePtr im, gdPointPtr p ),
		gdImageRMove(			gdImagePtr im, gdPointPtr p ),
		gdImageLine(			gdImagePtr im, gdPointPtr p ),
		gdImageRLine(			gdImagePtr im, gdPointPtr p ),
		gdImagePolyLine(	 	gdImagePtr im, gdPointPtr p, int pointsTotal ),
		gdImageRectangle(		gdImagePtr im, gdPointPtr p ),
		gdImageFilledRectangle(	gdImagePtr im, gdPointPtr p ),
		gdImageArc(				gdImagePtr im, int cx, int cy, int Angle, int Sweep, int XRadius, int YRadius, int Rotation ),
		gdImageFilledPolygon(	gdImagePtr im, gdPointPtr p, int pointsTotal );

// Path & area functions
DLLEXPORT int
		gdImageBeginArea(		gdImagePtr im ),
		gdImageBeginPath(		gdImagePtr im );

DLLEXPORT void
		gdImageEndArea(	 		gdImagePtr im ),
		gdImageEndPath(	 		gdImagePtr im );

// Color functions
DLLEXPORT int
		gdImageColorAllocate(	gdImagePtr im, int r, int g, int b ),
		gdImageColorClosest(	gdImagePtr im, int r, int g, int b ),
		gdImageColorExact(		gdImagePtr im, int r, int g, int b );

DLLEXPORT void
		gdImageColorDeallocate(	gdImagePtr im, int color );

// Copy functions
DLLEXPORT void
		gdImageCopy(			gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h ),
		gdImageCopyResized(		gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int destw, int desth, int srcw, int srch );

// Brushes, styles & types
DLLEXPORT void
		gdImageSetBrush(		gdImagePtr im, gdImagePtr brush ),
		gdImageSetTile(			gdImagePtr im, gdImagePtr tile ),
		gdImageSetStyle(		gdImagePtr im, int *style,	int noOfPixels ),
		gdImageSetLineStyle(	gdImagePtr im, int LineStyle );

// Fill functions
DLLEXPORT void
		gdImageFill(			gdImagePtr im, int x, int y ),
		gdImageFillToBorder(	gdImagePtr im, int x, int y, int border );

// Font functions
DLLEXPORT void
		gdImageChar(			gdImagePtr im, gdFontPtr font, int c ),
		gdImageCharUp(			gdImagePtr im, gdFontPtr font, int c ),
		gdImageString(			gdImagePtr im, gdFontPtr font, char *s ),
		gdImageStringUp(		gdImagePtr im, gdFontPtr font, char *s );

// Misc functions
DLLEXPORT void
		gdImageSpecialPixel( 	gdImagePtr im, int x, int y ),
		gdImageLineStylePixel( 	gdImagePtr im, int x, int y, int color );


/*
**	Macros to get and set pixels
*/

#define	gdImageSetPixelColor(im, x, y, c)	\
		{	\
			if( gdImageBoundsSafe( (im), (x), (y) ))	\
			{	\
				if( (c) >= 0 )	\
					(im)->pixels[ y ][ x ] = (c);	\
				else	\
					gdImageSpecialPixel( (im), (x), (y));	\
			}	\
		}

#define	gdImageSetLinePixel(im, x, y )	\
		{	\
			if( gdImageBoundsSafe( (im), (x), (y) ))	\
			{	\
				if( (im)->CurrentColor >= 0 )	\
				{	\
					if( (im)->LineStyle != gdLineStyle_SOLID )	\
						gdImageLineStylePixel( (im), (x), (y), (im)->CurrentColor );	\
					else	\
						(im)->pixels[ y ][ x ] = (im)->CurrentColor;	\
				}	\
				else	\
					gdImageSpecialPixel( (im), (x), (y));	\
			}	\
		}

#define	gdImageSetPixel(im, x, y)	gdImageSetPixelColor( (im), (x), (y), (im)->CurrentColor )

#define	gdImageGetPixel(im, x, y)	((gdImageBoundsSafe((im),(x),(y))) ? (im)->pixels[(y)][(x)] : 0 )

#define	gdImageBoundX(im, x)		((x) < 0 ? 0 : ((x) >= (im)->sx ? (im)->sx - 1 : (x)))
#define	gdImageBoundY(im, y)		((y) < 0 ? 0 : ((y) >= (im)->sy ? (im)->sy - 1 : (y)))


/*
**	Macros to get information from images. READ ONLY.
**	Changing these values will NOT have the desired result.
*/

#define gdImageSX(im)				((im)->sx)
#define gdImageSY(im)				((im)->sy)
#define gdImageColorsTotal(im)		((im)->colorsTotal)
#define gdImageRed(im, c)			((im)->red[(c)])
#define gdImageGreen(im, c)			((im)->green[(c)])
#define gdImageBlue(im, c)			((im)->blue[(c)])
#define gdImageGetTransparent(im)	((im)->transparent)
#define gdImageGetInterlaced(im) 	((im)->interlace)


/*
**	Macros to set information in images
*/

#define	gdImageColorUse(im, c)			(((im)->CurrentColor) = (c))
#define	gdImageInterlace(im, s)			(((im)->interlace) = (s))
#define	gdImageColorTransparent(im, c)	(((im)->transparent) = (c))


/*
**	Macros to speed up things
*/

#define	gdImageBoundsSafe(im, x, y)		((x) >= 0 && (y) >= 0 && (x) < (im)->sx && (y) < (im)->sy)

#endif

