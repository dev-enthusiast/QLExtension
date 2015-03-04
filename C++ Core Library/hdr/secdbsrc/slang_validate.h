/****************************************************************
**
**	slang_validate.h         - Slang Code Safety API
**
**	Copyright 2001-2004 - Constellation Power Source, Inc. - Baltimore    
**	Copyright 1992-2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/slang_validate.h,v 1.2 2004/09/29 13:47:48 khodod Exp $
**
****************************************************************/

#ifndef SLANG_VALIDATE_H
#define SLANG_VALIDATE_H

#include <portable.h>
#include <kool_ade.h>
#include <secdrv.h>
#include <secexpr.h>
#include <crypt.h>
#include <secdbcc.h>


EXPORT_C_SECDB 
    int SlangCheckWriteAccess( SDB_DB *pDb );

EXPORT_C_SECDB
	bool SlangInUserScript( );

EXPORT_C_SECDB
    bool SlangModuleVerify( const char *ModuleName, const char *Code );

EXPORT_C_SECDB 
    bool SlangModuleIsSafe( const char *ModuleName );

EXPORT_C_SECDB
    void SlangModuleRefreshSafetyInfo( const char *ModuleName );

EXPORT_C_SECDB
    void SlangModuleMarkUnsafe( SDB_OBJECT *SecPtr );

EXPORT_C_SECDB
    bool SlangModuleAdd( SLANG_ERROR_INFO *ModuleInfo, bool Trusted );

EXPORT_C_SECDB
    bool SlangModuleAttachChild( SLANG_ERROR_INFO *ParentInfo, SLANG_ERROR_INFO *ChildInfo );


#endif
