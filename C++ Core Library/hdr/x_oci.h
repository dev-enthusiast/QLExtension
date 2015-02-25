/* $Header: /home/cvs/src/slang/src/x_oci.h,v 1.4 2014/08/29 14:54:36 c01713 Exp $ */
/****************************************************************
**
**	X_OCI.H
**
**	Copyright 2014 - Exelon Corporation
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_X_OCI_H )
#define IN_X_OCI_H

DLLEXPORT int SlangXOCIConnect    ( SLANG_ARGS );
DLLEXPORT int SlangXOCIExec       ( SLANG_ARGS );
DLLEXPORT int SlangXOCICommit     ( SLANG_ARGS );
DLLEXPORT int SlangXOCIRPC        ( SLANG_ARGS );
DLLEXPORT int SlangXOCIRollback   ( SLANG_ARGS );

DLLEXPORT int SlangXOCILobWrite ( SLANG_ARGS );
DLLEXPORT int SlangXOCILobRead ( SLANG_ARGS );

#endif
