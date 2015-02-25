/****************************************************************
**
**	GSD.H	- generic SecDb screen driver
**
**	$Header: /home/cvs/src/secedit/src/gsd.h,v 1.25 1999/09/17 12:27:19 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSD_H )
#define IN_GSD_H

#include <editinfo.h>


/*
**	Dialog datatype
*/

DLLEXPORT DT_DATA_TYPE
		DtDialog;

// Call this before using DtDialog value
#define DTDIALOG_INIT()	if( !DtDialog ) DtFromName( "Dialog" )


/*
**	Prototype functions
*/

DLLEXPORT GSD_DIALOG
		*DialogFromValue(	DT_VALUE *Value );

DLLEXPORT int
		GsdDialogEdited(	GSD_DIALOG *Dialog ),
		GsdDialogRecalc(	GSD_DIALOG *Dialog );

#endif
