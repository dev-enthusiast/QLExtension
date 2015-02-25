/* $Header: /home/cvs/src/gob/src/gobview.h,v 1.5 2000/12/19 23:02:37 simpsk Exp $ */
/****************************************************************
**
**	GOBVIEW.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gob/src/gobview.h,v 1.5 2000/12/19 23:02:37 simpsk Exp $
**
****************************************************************/

#if !defined( IN_GOBVIEW_H )
#define IN_GOBVIEW_H

#include <gob.h>



/****************************************************************
** 	Type: ValueTypeTransform
**
**  Returns: TRUE / Err( ... )
**
** 	Description: A function that transforms a value type
**  			 in some way.  Typically, this is used
**				 in conjunction with GobValueFuncAlternateView
**				 to transform a value type.
**				
**				 The args are the value to be transformed, the
**				 value into which it should be transformed, and
**				 extra data that may be needed by a particular
**				 function of this type.  On entry the ouput
**				 containes the current value of the output VT,
**				 which the function may wish to consult.
**
****************************************************************/
 
typedef int ( * ValueTypeTransform )( DT_VALUE *Input, DT_VALUE *Output, void *Extra	);



/****************************************************************
** Mapping structure used by GobValueFuncAlternateView().
****************************************************************/

struct GOB_ALTERNATE_VIEW_MAP
{
	char	*UnderlierName;		// Name of underlying VT.

	ValueTypeTransform	
			ViewFromUnderlier,	// Xform Underlier -> View
			ViewToUnderlier;	// Xform View -> Underlier

	void	*Extra;				// Whatever extra information
								// this view map needs.	

	const char	
			*Description;		// Description for SDB_MSG_DESCRIPTION.

};



/****************************************************************
** A structure mapping masks to/from bit names.  This is used
** by XFormBitStructure[To | From]Flag().
****************************************************************/

struct GOB_XFORM_BIT_LABEL
{
	const char
			*Name;

	INT32	Mask;
};



/****************************************************************
** Macro: GOB_ALTERNATE_VIEW_CASES
**
** Description: This macro is for use inside case statements
**				in Gob value functions to indicate which cases
**				should be passed into GobValueFuncAlternateView().
****************************************************************/

#define GOB_ALTERNATE_VIEW_CASES				\
		     SDB_MSG_GET_CHILD_LIST:			\
		case SDB_MSG_GET_VALUE:					\
		case SDB_MSG_SET_VALUE:					\
		case SDB_MSG_DIDDLE_SET:				\
		case SDB_MSG_DIDDLE_CHANGED:			\
		case SDB_MSG_DIDDLE_REMOVED



EXPORT_C_GOB int 
		GobValueFuncAlternateView( 	GOB_VALUE_FUNC_ARGS );

EXPORT_C_GOB int
		GobXFormBitStructureFromFlag(	DT_VALUE *FlagsValue, 			DT_VALUE *BitStructureValue, 	void *Extra ),
		GobXFormBitStructureToFlag( 	DT_VALUE *BitStructureValue, 	DT_VALUE *FlagsValue, 			void *Extra );
		

#endif 








