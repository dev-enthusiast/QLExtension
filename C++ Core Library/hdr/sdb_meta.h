/****************************************************************
**
**	sdb_meta.h	- Metadatabase functions
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_SDB_META_H )
#define IN_SDB_META_H

#include	<sdb_base.h>

#ifndef IN_DATATYPE_H
#include	<datatype.h>
#endif
#ifndef IN_SECDRV_H
#include	<secdrv.h>
#endif

EXPORT_C_SECDB SDB_DB 
		*SecDbAttachMetaDatabase( int ReadOnly );

EXPORT_C_SECDB int
		SecDbDetachMetaDatabase( SDB_DB *Db );

int SecDbLoadValueTypeRegistryTable( const char *Path );

int	SecDbValueTypeCheck( const char *ValueName, DT_DATA_TYPE DataType );

#endif

