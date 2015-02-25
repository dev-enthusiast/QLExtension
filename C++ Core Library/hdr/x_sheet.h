/* $Header: /home/cvs/src/slang/src/x_sheet.h,v 1.13 2000/03/28 10:23:27 goodfj Exp $ */
/****************************************************************
**
**	X_SHEET.H	- Spreadsheet functions
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.13 $
**
****************************************************************/

#if !defined( IN_X_SHEET_H )
#define IN_X_SHEET_H

#if !defined( IN_OUTFORM_H )
#	include	<outform.h>
#endif
#if !defined( IN_DATATYPE_H )
#	include	<datatype.h>
#endif

#include	<stdio.h>


/*
**	Define constants
*/

#define	SHEET_MAX_COLUMNS			256
#define	SHEET_DEFAULT_COLUMN_WIDTH	10.0
#define	SHEET_DEFAULT_FONT_FACE		CELL_FACE_HELVETICA
#define	SHEET_DEFAULT_FONT_SIZE		10
#define	SHEET_DEFAULT_FONT_WEIGHT	CELL_WEIGHT_NORMAL
#define	SHEET_AUTO_ROW_HEIGHT		(-1.0)

#define	SHEET_MODE_PORTRAIT			0x0001
#define	SHEET_MODE_EPS				0x0002

#define	SHEET_FILE_TABBED			0x0001
#define	SHEET_FILE_COMMAED			0x0002


/*
**	Define cell attributes
*/

#define	CELL_AT_BORDER_TOP		0x00000001
#define	CELL_AT_BORDER_BOTTOM	0x00000002
#define	CELL_AT_BORDER_LEFT		0x00000004
#define	CELL_AT_BORDER_RIGHT	0x00000008
#define	CELL_AT_BORDER_BOX		0x0000000F		// TOP | BOTTOM | LEFT | RIGHT

#define	CELL_AT_LEFT			0x00000020
#define	CELL_AT_RIGHT			0x00000040
#define	CELL_AT_CENTER			0x00000080

#define	CELL_AT_SHADE			0x00000100
#define	CELL_AT_BLACK			0x00000200

#define	CELL_AT_LOGO			0x00000400
#define	CELL_AT_HORIZONTAL_BAR	0x00000800
#define	CELL_AT_EPS_FILE		0x00001000
#define	CELL_AT_CHART3D			0x00002000
#define	CELL_AT_CHART2D			0x00004000
#define CELL_AT_SPECIAL_MASK	( CELL_AT_LOGO | CELL_AT_HORIZONTAL_BAR | CELL_AT_EPS_FILE | CELL_AT_CHART3D | CELL_AT_CHART2D )

#define	CELL_AT_PAGE_NUMBER		0x00008000
#define	CELL_AT_NEW_PAGE		0x00010000


#define CELL_AT_HEADING_0		0x00100000
#define CELL_AT_HEADING_1		0x00200000
#define CELL_AT_HEADING_2		0x00400000
#define CELL_AT_HEADING_3		0x00800000
#define CELL_AT_HEADING_END		0x01000000
#define CELL_AT_END_HEADING_0	( CELL_AT_HEADING_0 | CELL_AT_HEADING_END )
#define CELL_AT_END_HEADING_1	( CELL_AT_HEADING_1 | CELL_AT_HEADING_END )
#define CELL_AT_END_HEADING_2	( CELL_AT_HEADING_2 | CELL_AT_HEADING_END )
#define CELL_AT_END_HEADING_3	( CELL_AT_HEADING_3 | CELL_AT_HEADING_END )
#define CELL_AT_HEADING_MASK	( CELL_AT_HEADING_0 | CELL_AT_HEADING_1 | CELL_AT_HEADING_2 | CELL_AT_HEADING_3 | CELL_AT_HEADING_END )

#define CELL_AT_BOX_BY_2		0x02000000
#define CELL_AT_BOX_BY_3		0x04000000
#define CELL_AT_BOX_BY_5		0x08000000
#define CELL_AT_BOX_MASK		( CELL_AT_BOX_BY_2 | CELL_AT_BOX_BY_3 | CELL_AT_BOX_BY_5 )

#define CELL_AT_SHADE_BY_3		0x10000000
#define CELL_AT_SHADE_BY_5		0x20000000
#define CELL_AT_SHADE_MASK		( CELL_AT_SHADE_BY_3 | CELL_AT_SHADE_BY_5 )

#define CELL_AT_GROUPING_MASK	( CELL_AT_BOX_MASK | CELL_AT_SHADE_MASK )

/*
**	Define cell font information
*/

#define	CELL_WEIGHT_NORMAL			0x0000
#define	CELL_WEIGHT_BOLD			0x0001
#define	CELL_WEIGHT_ITALIC			0x0002

#define	CELL_FACE_AVANT_GARDE		0
#define	CELL_FACE_BOOKMAN			1
#define	CELL_FACE_COURIER			2
#define	CELL_FACE_HELVETICA			3
#define	CELL_FACE_HELVETICA_NARROW	4
#define	CELL_FACE_HELVETICA_BLACK	5
#define	CELL_FACE_NEW_CENTURY		6
#define	CELL_FACE_PALATINO			7
#define	CELL_FACE_SYMBOL			8
#define	CELL_FACE_TIMES				9
#define	CELL_FACE_ZAPF_CHANCERY		10 
#define	CELL_FACE_ZAPF_DINGBATS		11


/*
**	Structures used to hold a spread sheet
*/

typedef struct SheetCellStructure
{
	long	Attributes;

	int		FontFace,
			FontSize,
			FontWeight;
			
	DT_VALUE
			ColumnWidth,
			Value;

	OUTPUT_FORMAT
			*OutForm;

} SHEET_CELL;

typedef struct SheetRowStructure
{
	int		ColumnCount,
			ColumnsAllocated;
			
	float	RowHeight;

	SHEET_CELL
			**Cells;

} SHEET_ROW;

typedef struct SheetStructure
{
	int		ReferenceCount;

	DT_VALUE
			FontFace,
			FontSize,
			FontWeight;

	int		RowCount,
			RowsAllocated,
			ColumnCount;

	DT_VALUE
			ColumnWidths[ SHEET_MAX_COLUMNS ];

	SHEET_ROW
			**Rows;

} SHEET;


/*
**	Prototype functions
*/

DLLEXPORT int		SheetSetCellData(		SHEET *Sheet, int Column, int Row, DT_VALUE *Value ),
		SheetSetCellAttributes(	SHEET *Sheet, int Column, int Row, long Attributes );

DLLEXPORT SHEET_CELL
		*SheetGetCell(			SHEET *Sheet, int Column, int Row ),
		*SheetLookupCell(		SHEET *Sheet, int Column, int Row );

char	*SheetCellFormat(		SHEET_CELL *Cell, int Width );
				
// SHT_PS.C

int		SheetToPostscript(		SHEET *Sheet, FILE *fpOutput, int Mode );

#endif

