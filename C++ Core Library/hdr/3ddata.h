/* $Header: /home/cvs/src/3d/src/3ddata.h,v 1.24 1998/10/09 21:56:21 procmon Exp $ */
/****************************************************************
**
**	3DDATA.H	
**
**	$Revision: 1.24 $
**
****************************************************************/

#if !defined( IN_3DDATA_H )
#define IN_3DDATA_H

/*
**	Define constants
*/

#define	THREE_D_MAX_Y_LINES				384
#define	THREE_D_MAX_X_LINES				384

#define	THREE_D_STYLE_BLOCK				1
#define	THREE_D_STYLE_WIRE				2
#define	THREE_D_STYLE_SHADE		  		3
#define	THREE_D_STYLE_STICK		  		4
#define	THREE_D_STYLE_SPHERE	  		5
#define	THREE_D_STYLE_BAR		  		6
#define	THREE_D_STYLE_HEAT				7
#define	THREE_D_STYLE_TOROID			8
#define	THREE_D_STYLE_CONTOUR			9
#define	THREE_D_STYLE_LEVEL				10

#define	THREE_D_FLAG_BORDERS			0x0001
#define	THREE_D_FLAG_BLACKNESS			0x0002
#define	THREE_D_FLAG_WALLS				0x0004
#define	THREE_D_FLAG_BASE				0x0008
#define	THREE_D_FLAG_OUTLINE			0x0010
#define	THREE_D_FLAG_PERSPECTIVE		0x0020
#define	THREE_D_FLAG_DOUBLE_BUFFER		0x0040
#define	THREE_D_FLAG_AXIS_LABELS		0x0080
#define	THREE_D_FLAG_HIDDEN_LINE		0x0100
#define	THREE_D_FLAG_CROSS_HATCH		0x0200
#define	THREE_D_FLAG_INVERT_PALETTE		0x0400
#define	THREE_D_FLAG_CONTOURS			0x0800

#define	THREE_D_PAL_CUSTOM				0
#define	THREE_D_PAL_RED1  				1
#define	THREE_D_PAL_GREEN1				2
#define	THREE_D_PAL_BLUE1				3
#define	THREE_D_PAL_RED2	  			4
#define	THREE_D_PAL_GREEN2				5
#define	THREE_D_PAL_BLUE2				6
#define	THREE_D_PAL_RED_SOLID			7
#define	THREE_D_PAL_GREEN_SOLID			8
#define	THREE_D_PAL_BLUE_SOLID			9
#define	THREE_D_PAL_GRAY				10
#define	THREE_D_PAL_EARTH				11
#define	THREE_D_PAL_RAINBOW				12

#define	THREE_D_LIGHT_ZCOLOR			0x0001
#define	THREE_D_LIGHT_SHADING			0x0002
#define	THREE_D_LIGHT_BOTH				(THREE_D_LIGHT_ZCOLOR | THREE_D_LIGHT_SHADING)


#define	THREE_D_DEF_FLAT_ANGLE			30
#define	THREE_D_DEF_TILT_ANGLE			30
#define	THREE_D_DEF_ZOOM_FACTOR			1.0
#define	THREE_D_DEF_FLAGS				(THREE_D_FLAG_BLACKNESS | THREE_D_FLAG_CROSS_HATCH)
#define	THREE_D_DEF_STYLE				THREE_D_STYLE_BLOCK
#define	THREE_D_DEF_PALETTE				THREE_D_PAL_RED1
#define	THREE_D_DEF_LIGHTX				-1.0
#define	THREE_D_DEF_LIGHTY				-1.0
#define	THREE_D_DEF_LIGHTZ				1.0
#define	THREE_D_DEF_LIGHT				THREE_D_LIGHT_ZCOLOR
#define	THREE_D_DEF_FONT_SIZE			12
#define	THREE_D_DEF_X_PERCENT			0.80
#define	THREE_D_DEF_Y_PERCENT			0.80


/*
**	Define types
*/

typedef struct Chart3DStructure			CHART3D;
typedef struct Chart3dParamsStructure 	CHART3D_PARAMS;

typedef	void	(*THREE_D_PF_DRAW_IMAGE)(			CHART3D *Chart, struct GxHandleStructure *Gx, long ImageSizeX, long ImageSizeY );
typedef	void	(*THREE_D_PF_DEVICE_DESTROY)(		CHART3D *Chart );
typedef	void	(*THREE_D_PF_NEW_CHART)(			CHART3D *Chart );
typedef	void	(*THREE_D_PF_COORD_FROM_POINTER)(	CHART3D *Chart, long ImageSizeX, long ImageSizeY, int PointerX, int PointerY, int *CoordX, int *CoordY );
typedef void	(*THREE_D_PF_DRAW_DATA_CURSOR)(		CHART3D *Chart, struct GxHandleStructure *Gx, long ImageSizeX, long ImageSizeY, int CoordX, int CoordY );
typedef int		(*THREE_D_PF_ABORT_CHECK)(			CHART3D *Chart );


/*
**	Structure used to hold a 3d vector
*/

typedef struct ThreeDVector3Structure
{
	float	x,
			y,
			z;

} THREE_D_VECTOR3;


/*
**	Structure used to hold a plane
*/

typedef struct ThreeDPlaneStructure
{
	float	a,
			b,
			c,
			d;

} THREE_D_PLANE;


/*
**	Structure used to contain tick values
*/

typedef struct ThreeDTickInfoStructure
{
	double	Position;

	char	*Text;

} THREE_D_TICK_INFO;


/*
**	Structure used to contain parameters
*/

struct Chart3dParamsStructure
{
	int		ChangingAngles,
			BufferValid,
			Xstart,
			Xend,
			Xstep,
			Ystart,
			Yend,
			Ystep,
			Youtside,
			CullingDirection,
			ContentsCentered;

	double	a1,
			b1,
			a2,
			b2,
			ZAxisMin,
			ZAxisMax,
			ZAxisRange,
			ZAxisStep,
			MinValue,
			MaxValue,
			Range,
			MinColor,
			MaxColor,
			ColorRange,
			SquishFactor,
			*Xvalues,
			*Yvalues;

	long	ImageSizeX,
			ImageSizeY;

	THREE_D_VECTOR3
			Light;

	THREE_D_TICK_INFO
			*Xticks,
			*Yticks;

	int		XtickCount,
			YtickCount;

	long	ColorTable[ 256 ];

	void	*Extra;

};	// CHART3D_PARAMS


/*
**	Structure used to hold chart information
*/

struct Chart3DStructure
{
	int		ReferenceCount,
			ParametersChanged,
			Xsize,
			Ysize;

	float	**Zvalues,
			**Colors;

	DT_VALUE
			Title,
			Xlabel,
			Ylabel,
			Zlabel,
			Xticks,
			Yticks,
			Zticks,
			Light,
			Xlight,
			Ylight,
			Zlight,
			Xvalues,
			Yvalues,
			Flags,
			Style,
			FlatAngle,
			TiltAngle,
			ZoomFactor,
			Zlow,
			Zhigh,
			Palette,
			CustomPalette,
			FontName,
			FontSize,
			XPercentOfSquare,
			YPercentOfSquare;

	CHART3D_PARAMS
			PP;

	THREE_D_PF_DRAW_IMAGE
			pfDrawImage;

	void	*DeviceHandle,
			*AbortHandle;

	THREE_D_PF_DEVICE_DESTROY
			pfDeviceDestroy;

	THREE_D_PF_COORD_FROM_POINTER
			pfCoordFromPointer;

	THREE_D_PF_DRAW_DATA_CURSOR
			pfDrawDataCursor;

	THREE_D_PF_ABORT_CHECK
			pfAbortCheck;

};	// CHART3D


/*
**	Macros
*/

#define	MAKE_RGB(Red,Green,Blue)	((((Red) & 0xffl) << 16) | (((Green) & 0xffl) << 8) | ((Blue) & 0xffl))
#define	MAKE_RED(Color)				(((Color) >> 16) & 0xffl)
#define	MAKE_GREEN(Color)			(((Color) >> 8) & 0xffl)
#define	MAKE_BLUE(Color)			((Color) & 0xffl)

#define VZERO(v)    (v.x = v.y = v.z = (float) 0.0)
#define VNORM(v)    (sqrt(v.x * v.x + v.y * v.y + v.z * v.z))
#define VDOT(u, v)  (u.x * v.x + u.y * v.y + u.z * v.z)
#define VINCR(u, v) (u.x += v.x, u.y += v.y, u.z += v.z)

#endif

