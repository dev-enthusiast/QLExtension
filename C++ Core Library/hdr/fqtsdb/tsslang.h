/***********************************************************************
**
**	tsslang.h - Slang functions for tsdb
**
**  Copyright 2000 - Goldman Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
***********************************************************************/
#if !defined( IN_TSSLANG_H )
#define IN_TSSLANG_H

#include <fqtsdb/base.h>

#define SLANG_WINDOW_SCOPE     "NTPLOT"
#define SLANG_WINDOW_VARIABLE  "Window"

EXPORT_C_FQTSDB HASH *SlangAddinFunctionHash;
EXPORT_C_FQTSDB void TsdbRegisterSlangFuncs( void );
EXPORT_C_FQTSDB void TsdbSlangFuncLookupHook( char *Name );
EXPORT_C_FQTSDB SYMBOL *fSlang(TSDBARG *ts);
EXPORT_C_FQTSDB SYMBOL *fSlangF(TSDBARG *ts);
EXPORT_C_FQTSDB SYMBOL *fSlangS(TSDBARG *ts);
EXPORT_C_FQTSDB SYMBOL *fSlangV(TSDBARG *ts);
EXPORT_C_FQTSDB SYMBOL *fSlangLog( TSDBARG *ts );
EXPORT_C_FQTSDB SYMBOL *fSlangDb( TSDBARG *ts );
EXPORT_C_FQTSDB SYMBOL *fSlangServ( TSDBARG *ts );
EXPORT_C_FQTSDB void SlangSetWindowInfo( char *Component, double Value );

#endif
