/****************************************************************
**
**	SECEDIT.H	- SecDb Editting and Dialog functions
**
**	$Header: /home/cvs/src/secedit/src/secedit.h,v 1.24 2001/07/17 23:25:17 turokm Exp $
**
****************************************************************/

#if !defined( IN_SECEDIT_H )
#define IN_SECEDIT_H

#include <secdb.h>
#include <secexpr.h>
#include <editinfo.h>
#include <window.h>
#include <wfkeys.h>


/*
**	Structure used for Dialogs
*/

struct SECEDIT_DIALOG
{
	DT_VALUE
			EditArray,
			Values;

	const char
			*Title,			// Just set these values
			*Subtitle;

	EDIT_INFO_POSITION
			*Position;

	void	*UserData;		// Accessible from callbacks

};


struct SECEDIT_DEFAULTS
{
	const char
			*Title,			// use as window title/subtitle
			*Subtitle;

	DT_VALUE
			*EditInfoArray;	// use this array of EditInfos 

	const char
			*EditValueName;	// vs. "Edit Info Generic"

	SLANG_SCOPE
			*SlangScope;	// Required for slang call backs

	//FKEYS	*DialogFKeys;	// Overrides Config/DialogFKeys values

	unsigned int
			NoReload		: 1,	// Don't Reload( SecPtr ) before editing
			NoClearDiddles	: 1,	// Don't ClearDiddles( ) after editing
			NoUpdate		: 1,	// Don't UpdateSecurity() after F9
			NoValidate		: 1;	// Don't ValidateSecurity() after F9

	struct HashStructure
			*DiddleHash;	// Used by NoClearDiddles.  Initialize to NULL

	void	*UserData;		// Accessible from callbacks

	EDIT_INFO_POSITION
			*Position;		// Optional overriding position value

};


/*
**	Prototype functions
*/

struct EDIT_INFO;

DLLEXPORT int	SecEditEditSecurity(	   	SDB_OBJECT *SecPtr, SECEDIT_DEFAULTS *Defaults ),
                SecEditValidate(	        SDB_OBJECT *SecPtr ),
			   	SecEditEditTable(			SECEDIT_DEFAULTS *Defaults, SDB_OBJECT *SecPtr, DT_VALUE *StructValue, int *Changed ),
			   	SecEditEditStruct(			DT_VALUE *ReturnStruct, DT_VALUE *StructValue, SECEDIT_DEFAULTS *Defaults ),
			   	SecEditEditSheet(			DT_VALUE *Sheet, SECEDIT_DEFAULTS *Defaults );

DLLEXPORT void	SecEditClearDiddles(	   	SDB_OBJECT *SecPtr, SECEDIT_DEFAULTS *Defaults );

#ifdef IN_FORM_H
DLLEXPORT int	SecPickFormInpFunc(			FORM *pForm, FIELD *pField, int Code );
DLLEXPORT void	form_add_secclass(			FIELD *Field, const char *SecClass );
         
#endif

// In DIALOG.C
DLLEXPORT void 	SecEditDialogInit(			SECEDIT_DIALOG *Dialog, EDIT_INFO_INIT *DialogEditInfoInit ),
				SecEditDialogFree(			SECEDIT_DIALOG *Dialog ),
				SecEditDialogSetPointer(	SECEDIT_DIALOG *Dialog, const char *Component, DT_DATA_TYPE DataType, const void  *Value ),
				SecEditDialogSetNumber(		SECEDIT_DIALOG *Dialog, const char *Component, DT_DATA_TYPE DataType, double Value );

DLLEXPORT const void
				*SecEditDialogGetPointer(	SECEDIT_DIALOG *Dialog, const char *Component, DT_DATA_TYPE DataType );

DLLEXPORT double
				SecEditDialogGetNumber(		SECEDIT_DIALOG *Dialog, const char *Component, DT_DATA_TYPE DataType );

DLLEXPORT int	SecEditDialog(				SECEDIT_DIALOG *Dialog );

// In X_DIALOG.C
DLLEXPORT int	ErrorLogOpen(				const char *LogName, const char *LogText );

#endif
