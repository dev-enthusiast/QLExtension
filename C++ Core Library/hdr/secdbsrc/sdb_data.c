#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_data.c,v 1.16 1999/09/01 15:29:27 singhki Exp $"
/****************************************************************
**
**	SDB_DATA.C	- Security database data types
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_data.c,v $
**	Revision 1.16  1999/09/01 15:29:27  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**
**	Revision 1.15  1998/01/02 21:32:50  rubenb
**	added GSCVSID
**	
**	Revision 1.14  1996/12/23 03:06:11  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
**	
**	Revision 1.13  1996/01/02 23:33:38  ERICH
**	NT Compatibility
**
**	   Rev 1.12   07 Apr 1994 19:11:12   LUNDEK
**	Split off datatype project
**
**	   Rev 1.11   02 Feb 1994 21:33:52   LUNDEK
**	Fix small compiler warnings
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<hash.h>
#include	<secdb.h>



/****************************************************************
**	Routine: SecDbDataTypeEnumFirst
**	Returns: First data type
**			 NULL if no data types
**	Summary: Start the enumeration of all data types
****************************************************************/

DT_DATA_TYPE SecDbDataTypeEnumFirst(
	SDB_ENUM_PTR *EnumPtr		// Enumeration pointer
)
{
	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));

	(*EnumPtr)->Count = DtNamesHash->KeyCount;
	(*EnumPtr)->Ptr = HashFirst( DtNamesHash );
	if( HashEnd( (*EnumPtr)->Ptr ))
		return 0;
	return (DT_DATA_TYPE) HashValue( (*EnumPtr)->Ptr );
}



/****************************************************************
**	Routine: SecDbDataTypeEnumNext
**	Returns: Next data type
**			 NULL if no more data types
**	Action : Return the next data type
****************************************************************/

DT_DATA_TYPE SecDbDataTypeEnumNext(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashNext( DtNamesHash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return 0;
	return (DT_DATA_TYPE) HashValue( EnumPtr->Ptr );
}



/****************************************************************
**	Routine: SecDbDataTypeEnumClose
**	Returns: Nothing
**	Action : End the enumeration of data types
****************************************************************/

void SecDbDataTypeEnumClose(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
		free( EnumPtr );
}
