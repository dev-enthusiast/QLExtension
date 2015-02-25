/* $Header: /home/cvs/src/kool_ade/src/kool_yap.h,v 1.10 2001/11/27 22:49:06 procmon Exp $ */
/****************************************************************
**
**	KOOL_YAP.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_KOOL_YAP_H )
#define IN_KOOL_YAP_H

#include	<datatype.h>


/*
**	Types of fields
*/

typedef enum YapFieldTypeEnum
{								// FieldData		Description
								// --------------	---------------------------------
	YAP_FIELD_TYPE_DT,			// DT_DATA_TYPE	*	Converts string in field to given 
	YAP_FIELD_TYPE_STRHASH,		// HASH *			Looks up string in hash (returns DtNull with Data.Pointer = Value in hash)
	YAP_FIELD_TYPE_STRTAB,		// char **			Validates string against STRTAB (returns DtString)
	YAP_FIELD_TYPE_BITTAB,		// YAP_BITTAB *		Bit-wise or (returns DtDouble of mask)
	YAP_FIELD_TYPE_RECURSIVE	// YAP_PARSER *		Field is a yap-defined table itself

} YAP_FIELD_TYPE;


/*
**	Messages sent to callback
*/

typedef enum YapMsgEnum
{
	YAP_MSG_CREATE,
	YAP_MSG_DESTROY,
	YAP_MSG_ADD_FIELD,
	YAP_MSG_ITEM_START,
	YAP_MSG_ITEM_PARSED,
	YAP_MSG_PARSE_START,
	YAP_MSG_PARSE_STOP

} YAP_MSG;


/*
**	Typedefs
*/

typedef struct YapParserStructure 
		YAP_PARSER;
		
typedef struct YapBitTabStructure
		YAP_BITTAB;
		
typedef struct YapFieldStructure
		YAP_FIELD;
		
typedef int 
		(*YAP_CALLBACK)(	 YAP_PARSER *Yap, YAP_MSG Msg, DT_VALUE *FieldValues, int FieldCount, YAP_PARSER *Parent );


/*
**	Define other data structures
*/

struct YapBitTabStructure
{
	char	*BitName;	// Name to be parsed or Null for terminating table of bits
	
	unsigned long
			Bit;

};	// YAP_BITTAB


struct YapFieldStructure
{
	const char
			*Name;
	
	YAP_FIELD_TYPE
			FieldType;
			
	void	*FieldData;
	
	struct YapFieldStructure
			*Next;

};	// YAP_FIELD


struct YapParserStructure
{
	const char
			*Name;
	
	YAP_CALLBACK
			Callback;

	YAP_FIELD
			*FieldList;
			
	int		FieldCount;
	
	void	*Extra;
			
};	// YAP_PARSER
	


/*
**	Define function prototypes
*/

DLLEXPORT YAP_PARSER
		*YapCreate(		const char *Name, YAP_CALLBACK Callback );

DLLEXPORT void
		YapDestroy(		YAP_PARSER *Yap );

DLLEXPORT int
		YapAddField(	YAP_PARSER *Yap, const char *Name, YAP_FIELD_TYPE FieldType, void *FieldData ),
		YapParse(		YAP_PARSER *Yap, const char *FileName, const char *OrString, const char *Path );

#endif

