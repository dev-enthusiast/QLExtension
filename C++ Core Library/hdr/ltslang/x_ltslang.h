/****************************************************************
**
**	X_LTSLANG.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltslang/src/ltslang/x_ltslang.h,v 1.2 2001/03/07 18:12:27 vengrd Exp $
**
****************************************************************/

#if !defined( IN_LTSLANG_X_LTSLANG_H )
#define IN_LTSLANG_X_LTSLANG_H

#include <ltslang/base.h>
#include <secexpr.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_C_LTSLANG SLANG_RET_CODE SlangXLtSlangAddinRegister( SLANG_ARGS );
EXPORT_C_LTSLANG SLANG_RET_CODE SlangXLtSlangPreRegister(   SLANG_ARGS );

CC_END_NAMESPACE

#endif 
