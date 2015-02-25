/****************************************************************
**
**	SECDT.H	- Typedefs for internal SecDb datatypes
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdt.h,v 1.16 2001/05/30 21:07:43 simpsk Exp $
**
****************************************************************/


#ifndef IN_SECDT_H
#define IN_SECDT_H

#include <secdb.h>

/*
**	Typedefs for SecDb datatypes
**	Note:	DT_SLANG	is in secexpr.h
**			DT_POINTER	is in dtptr.h
*/

typedef DT_ARRAY		DT_VALUE_ARRAY;
typedef SDB_OBJECT		DT_SECURITY;
typedef SDB_VALUE_TYPE	DT_VALUE_TYPE;
typedef DT_ARRAY		DT_EACH;

struct SLANG_SCOPE_DIDDLES;
struct SLANG_SCOPE;


/*
**	Structure used for DtSecurity datatype
*/

struct DT_SECURITY_LIST
{
	int		Size;
	
	SDB_OBJECT
			**SecPtrs;

};


/*
**	Structure used to contain Database data type instances
*/

struct DT_DATABASE
{
	int		UseCount;

	SDB_DB	*Db;

};


/*
** structure used to contain a diddle scope + it's diddles
*/

struct DT_DIDDLE_SCOPE
{
	int		RefCount;

	SDB_DB	*Db;

    SLANG_SCOPE_DIDDLES
			*Diddles;
};


/*
**	Structure used for DtDiddleScopeList datatype
*/

struct DT_DIDDLE_SCOPE_LIST
{
	int		Size;
	
    DT_VALUE*
	        DiddleScopeLists; // each of these may be a DT_DIDDLE_SCOPE_LIST or a DT_DIDDLE_SCOPE
};

EXPORT_C_SECDB int
		DiddleScopeFromArray(	DT_ARRAY *DiddleState, SLANG_SCOPE *Scope ),
		DiddleScopeNestNew(		SDB_DB *BaseDb, DT_VALUE *DiddleScopeValue ),
		DiddleScopeFromDb(		DT_VALUE *DiddleScopeValue, SDB_DB *Db );

EXPORT_C_SECDB SDB_DB*
		DiddleScopeTemporaryGet();

EXPORT_C_SECDB int
		SecDbStreamByNameDefault;

#endif

