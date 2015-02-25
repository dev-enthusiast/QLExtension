/****************************************************************
**
**	LTSLANG.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltslang/src/ltslang/LtSlang.h,v 1.8 2001/03/21 18:49:16 vengrd Exp $
**
****************************************************************/

#if !defined( IN_LTSLANG_LTSLANG_H )
#define IN_LTSLANG_LTSLANG_H

#include <ltslang/base.h>
#include <gscore/GsString.h>
#include <secexpr.h>
#include <ccstl.h>
#include <vector>
#include <gsdt/GsType.h>


CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_LTSLANG bool 
		LtSlangAddinRegisterEnable(),
		LtSlangAddinRegisterEnable( bool );
	
EXPORT_CPP_LTSLANG GsStatus LtSlangAddinRegisterDefaultLib();

EXPORT_CPP_LTSLANG bool LtSlangDiagnostics();

EXPORT_CPP_LTSLANG GsStatus LtSlangLinkScript( 
	const GsString &LibName,
	bool RegisterEnable
);

EXPORT_CPP_LTSLANG GsStatus LtSlangGetFunctionArgTypes(
	const DT_SLANG *DtSlang,
	CC_STL_VECTOR( const GsType *) &ArgTypes,
	CC_STL_VECTOR( bool ) &ArgTypeIsDtValues,
	CC_STL_VECTOR( bool ) &ArgIsRequired,
	CC_STL_VECTOR( GsString ) &ArgNames,
	bool UseArgNamesGiven
);

CC_END_NAMESPACE

#endif 
