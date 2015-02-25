/* $Header: /home/cvs/src/datatype/src/dtcolumn.h,v 1.5 2001/11/27 21:32:07 procmon Exp $ */
/****************************************************************
**
**	DTCOLUMN.H	- Extraction from arrays of structures
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_DTCOLUMN_H )
#define IN_DTCOLUMN_H


#if !defined( IN_DATE_H )
#	include		<date.h>
#endif
#if !defined( IN_DATATYPE_H )
#	include		<datatype.h>
#endif


DLLEXPORT DATE	*DtColumnToDateArray(	DT_ARRAY *ArrayOfStructures, char *TagName );

DLLEXPORT double	*DtColumnToDoubleArray(	DT_ARRAY *ArrayOfStructures, char *TagName );

DLLEXPORT int		*DtColumnToIntArray(	DT_ARRAY *ArrayOfStructures, char *TagName );


#endif
