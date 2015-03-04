#define GSCVSID "$Header: /home/cvs/src/secdb/src/conv.c,v 1.4 2001/11/27 23:23:32 procmon Exp $"
/****************************************************************
**
**	conv.c	- Conversion functions 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: conv.c,v $
**	Revision 1.4  2001/11/27 23:23:32  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.2  2001/05/11 14:48:33  johnsoc
**	Correct registration for GsDtSecurity
**	
**	Revision 1.1  2001/03/26 11:38:14  johnsoc
**	LtSecDb convertors to be registered on GsDt initialisation
**	
**
****************************************************************/

#define BUILDING_SECGSDT

#include <portable.h>
#include <secdb/base.h>
#include <assert.h>
#include <gsdt/op.h>
#include <gsdt/GsDtString.h>
#include <gsdt/GsDtGeneric.h>
#include <gsdt/GsDtToDtValue.h>
#include <gsdt/GsDtFromDtValue.h>
#include <gsdt/x_gsdt.h>
#include <secdb/LtSecDbObj.h>

CC_BEGIN_NAMESPACE( Gs )

DLLEXPORT void GsDtInitialize( void );

/****************************************************************
**	Routine: ConvStrToSecurity
**	Returns: GsDt *
**	Action : Convert a string to a security
****************************************************************/

static GsDt *ConvStrToSecurity(
	const GsType	&NewType,		// Must be LtSecDbObj
	const GsDt		&Other			// Must be String
)
{
	assert( NewType		 == LtSecDbObj::GsDtType::staticType() );
	assert( Other.type() == GsDtString::staticType() );

	const GsDtString
			*Str = GsDtString::Cast( &Other );

	return new LtSecDbObj::GsDtType( LtSecDbObj::getByName( Str->data() ) );
}

/****************************************************************
**	Routine: ConvertLtSecDbObj
**	Returns: 
**	Action : 
****************************************************************/

static GsStatus ConvertLtSecDbObj(
	const GsDt &GsDtVal,
 	DT_VALUE   *DtVal,
 	GSDT_TO_DTVALUE_FLAG ConversionFlag
)
{
 	const LtSecDbObj::GsDtType
 			*GsDtSec = LtSecDbObj::GsDtType::Cast( &GsDtVal );
	
 	if( GsDtSec )
 	{
 		DT_POINTER_TO_VALUE( DtVal, SecDbIncrementReference( GsDtSec->data().ptr() ), DtSecurity );
		
 		return GS_OK;
 	}
 	else
 		return GsErr( ERR_INVALID_ARGUMENTS, "Expected to be converting a LtSecDbObj::GsDtType, not a %s", GsDtVal.typeName() );
}


/****************************************************************
**	Routine: ConvertDtSecurity
**	Returns: 
**	Action : 
****************************************************************/

static GsDt *ConvertDtSecurity(
	DT_VALUE *Val
)
{
	return new LtSecDbObj::GsDtType( LtSecDbObj( SecDbIncrementReference( (SDB_OBJECT *) Val->Data.Pointer ) ) );
}


/****************************************************************
**	Routine: GsDtInitialize
**	Returns: nothing
**	Action : Initialize for GsDt
****************************************************************/

void GsDtInitialize()
{
	static bool done = false;
	if( !done )
	{
		GsType::Register( "GsDtSecurity",      0, GSTF_POINTER,	-100, new LtSecDbObj::GsDtType() );

		new GsDtuConvFunc( LtSecDbObj::GsDtType::staticType(),  GsDtString::staticType(), ConvStrToSecurity  );

		GsDtToDtValueConverterRegister( LtSecDbObj::GsDtType::staticType(), ConvertLtSecDbObj );
		GsDtFromDtValueConverterRegister( DtSecurity, ConvertDtSecurity );
	}
}

CC_END_NAMESPACE
