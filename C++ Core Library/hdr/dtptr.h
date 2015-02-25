/****************************************************************
**
**	DTPTR.H	- Slang Pointer datatype
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/dtptr.h,v 1.14 2001/11/27 23:23:33 procmon Exp $
**
****************************************************************/

#if !defined( IN_DTPTR_H )
#define IN_DTPTR_H

#include <sdb_base.h>

/*
**	Define structure for pointer data
*/

enum DT_POINTER_TYPE
{
	PtrNull = 0,
	PtrVariable,
	PtrComponent,
	PtrSubscript

};

struct DT_POINTER
{
	DT_POINTER_TYPE
			Type;

	SLANG_VARIABLE
			*Variable;

	DT_VALUE
			Element;

};


/*
**	DtPointer functions
*/

EXPORT_C_SECDB int
		DtPointerAssign( 			DT_VALUE *Ptr, DT_VALUE *Value ),
		DtPointerDestroy(			DT_VALUE *Ptr ),
		DtPointerChangeElement(		DT_VALUE *Ptr, DT_VALUE *Element ),
		DtPointerVariable(			DT_VALUE *Ptr, SLANG_VARIABLE *Variable ),
		DtPointerSubscript(			DT_VALUE *Ptr, DT_VALUE *ParentPtr, DT_VALUE *Subscript ),
		DtPointerComponent(			DT_VALUE *Ptr, DT_VALUE *ParentPtr, DT_VALUE *Component ),
		DtPointerProtect(			DT_VALUE *Ptr ),
		DtPointerUnprotect(			DT_VALUE *Ptr ),
		DtPointerUnreference(		DT_VALUE *Ptr );
		
EXPORT_C_SECDB void
		DtPointerCollectGarbage(	SLANG_VARIABLE *Variable );

#endif

