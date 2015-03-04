#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_vd.c,v 1.11 2000/06/27 23:50:19 nauntm Exp $"
/****************************************************************
**
**	SDB_VD.C	- Security database validate functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_vd.c,v $
**	Revision 1.11  2000/06/27 23:50:19  nauntm
**	fix buglet in allocation for validation
**
**	Revision 1.10  1999/09/01 15:29:33  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.9  1998/01/02 21:33:14  rubenb
**	added GSCVSID
**	
**	Revision 1.8  1996/12/23 03:06:28  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
**	
**	Revision 1.7  1996/09/18 02:20:53  lundek
**	Clean up SecDb trace stuff
**
**	Revision 1.6  1996/01/03 00:05:04  ERICH
**	NT Compatibility
**
**	   Rev 1.5   02 Aug 1994 17:06:40   LUNDEK
**	const char *ErrGetString
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<sectrace.h>



/****************************************************************
**	Routine: SecDbValidateAdd
**	Returns: None
**	Summary: Add a validation warning or error
**	Action : This function is called from a value function during
**			 SDB_MSG_VALIDATE processing.
****************************************************************/

void SecDbValidateAdd(
	SDB_M_DATA		*MsgData,	// MsgData passed to value function
	SDB_VALUE_TYPE	ValueType,	// Value type message is for
	int				Flags,		// SDB_VALIDATE_ flags
	const char		*Text		// Optional message text
)
{
	SDB_VALIDATE
			*NewListElement;


	NewListElement = (SDB_VALIDATE *) malloc( sizeof( SDB_VALIDATE ));
	NewListElement->ValueType	= ValueType;
	NewListElement->Flags		= Flags;
	NewListElement->Text		= (Text ? strdup( Text ) : NULL);
	NewListElement->Next		= NULL;

	MsgData->Validate.List->Next= NewListElement;
	MsgData->Validate.List		= NewListElement;
	MsgData->Validate.Count++;
}



/****************************************************************
**	Routine: SecDbValidateEnumFirst
**	Returns: Pointer to the first validate structure
**			 NULL if security is valid
**	Summary: Validate security and return pointer to first item
****************************************************************/

SDB_VALIDATE *SecDbValidateEnumFirst(
	SDB_OBJECT		*SecPtr,		// Security to validate
	SDB_ENUM_PTR	*EnumPtr		// Pointer to enumeration pointer
)
{
	SDB_VALIDATE
			*ListElement;

	SDB_M_DATA
			MsgData;


/*
**	Allocate the enumeration structures
*/

	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));
	(*EnumPtr)->Validate		= ListElement = (SDB_VALIDATE *) calloc( 1, sizeof( SDB_VALIDATE ));
	MsgData.Validate.Base		=
	MsgData.Validate.List		= ListElement;
	MsgData.Validate.Count		= 0;


/*
**	Send the validate message to the object
*/

	SDB_MESSAGE( SecPtr, SDB_MSG_VALIDATE, &MsgData );


/*
**	Get the first validate structure from the hash table
*/

	(*EnumPtr)->Count			= MsgData.Validate.Count;
	(*EnumPtr)->Validate	= ListElement->Next;
	if( ListElement->Text )
		free( ListElement->Text );
	free( ListElement );
	(*EnumPtr)->Extra			= (*EnumPtr)->Validate;

	return (*EnumPtr)->Validate;
}



/****************************************************************
**	Routine: SecDbValidateEnumNext
**	Returns: Pointer to the next validate structure
**			 NULL if there are no more validate structures
**	Summary: Get the next validation information
****************************************************************/

SDB_VALIDATE *SecDbValidateEnumNext(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr->Validate )
		EnumPtr->Validate = EnumPtr->Validate->Next;
	return EnumPtr->Validate;
}



/****************************************************************
**	Routine: SecDbValidateEnumClose
**	Returns: Nothing
**	Summary: End enumeration of validation information
****************************************************************/

void SecDbValidateEnumClose(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	SDB_VALIDATE
			*ListElement;


	if( EnumPtr )
	{
		EnumPtr->Validate = (SDB_VALIDATE *) EnumPtr->Extra;

		while( EnumPtr->Validate )
		{
			ListElement = EnumPtr->Validate->Next;
			if( EnumPtr->Validate->Text )
				free( EnumPtr->Validate->Text );
			free( EnumPtr->Validate );
			EnumPtr->Validate = ListElement;
		}
		free( EnumPtr );
	}
}



/****************************************************************
**	Routine: SecDbValidateEnumRewind
**	Returns: Pointer to the first validate structure
**			 NULL if there are no structures (security was valid)
**	Summary: Restart validation enumeration at first structure
****************************************************************/

SDB_VALIDATE *SecDbValidateEnumRewind(
	SDB_ENUM_PTR	EnumPtr		// Enumeration pointer
)
{
	return EnumPtr->Validate = (SDB_VALIDATE *) EnumPtr->Extra;
}
