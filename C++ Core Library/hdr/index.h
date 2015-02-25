/* $Header: /home/cvs/src/index/src/index.h,v 1.8 2000/07/25 10:09:25 goodfj Exp $ */
/****************************************************************
**
**	INDEX.H
**
**	FIX: This file should not be exported, as these functions are not 
**	used anywhere else.
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_INDEX_H )
#define IN_INDEX_H

DLLEXPORT int	NdxValueFuncMarketQuotes(	GOB_VALUE_FUNC_ARGS		);
DLLEXPORT int	NdxValueFuncTsdbQuotes(		GOB_VALUE_FUNC_ARGS		);

#endif

