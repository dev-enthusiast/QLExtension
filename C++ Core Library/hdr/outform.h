/* $Header: /home/cvs/src/kool_ade/src/outform.h,v 1.11 2001/03/29 16:27:29 simpsk Exp $ */
/****************************************************************
**
**	OUTFORM.H		- Header for output format functions
**
**	$Revision: 1.11 $
**
****************************************************************/

#ifndef IN_OUTFORM_H
#define	IN_OUTFORM_H

/*
**	Define output control flags
*/

#define	OUTFLAG_NONE			0x0000
#define	OUTFLAG_PARENS			0x0001
#define	OUTFLAG_COMMAS			0x0002
#define	OUTFLAG_PERCENT			0x0004
#define	OUTFLAG_CONCISE			0x0008
#define OUTFLAG_CIPHER			0x0010
#define OUTFLAG_BLANK_ZERO		0x0020
#define	OUTFLAG_LEFT			0x0040
#define	OUTFLAG_RIGHT			0x0080
#define	OUTFLAG_CENTER			0x0100
#define	OUTFLAG_SCALE_K			0x0200
#define	OUTFLAG_SCALE_M			0x0400
#define	OUTFLAG_SCALE_B			0x0800
#define	OUTFLAG_SCALE_SPACE		0x1000
#define	OUTFLAG_PLUS			0x2000
#define	OUTFLAG_PAD_ZEROS		0x4000
#define	OUTFLAG_TRIM_LEADING	0x8000
#define OUTFLAG_MUSH_COMPATIBLE	0x00010000	// Portable backward-compatible hash mushing string
#define OUTFLAG_FULL_PRECISION	0x00020000	// Print out all significant digits regardless of Decimal (space permitting)

#define	OUTFLAG_SCALE_ALL	(OUTFLAG_SCALE_K | OUTFLAG_SCALE_M | OUTFLAG_SCALE_B | OUTFLAG_SCALE_SPACE)


/*
**	Define structure used to output doubles
*/

struct OUTPUT_FORMAT
{
	short	Width,
			Decimal;
			
	int		Flags;

	double	ScaleAmt;

};


/*
**	Define a macro for easy initialization of an output format structure
*/

#define	SET_FORMAT(o,w,d,f,s)	(o)->Width=(w);(o)->Decimal=(d);(o)->Flags=(f);(o)->ScaleAmt=(s);


/*
**	Prototype functions
*/

DLLEXPORT char
		*OutputFormat(	char *Buffer, OUTPUT_FORMAT *Format, double Number );

DLLEXPORT double
		InFormat( const char *Input );

#endif
