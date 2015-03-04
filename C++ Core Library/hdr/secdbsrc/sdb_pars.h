/****************************************************************
**
**	SDB_PARS.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdb_pars.h,v 1.3 2000/12/19 17:20:47 simpsk Exp $
**
****************************************************************/

#if !defined( IN_SDB_PARS_H )
#define IN_SDB_PARS_H


struct SDB_NAMED_QUALIFIER
{
	char const*
			Name;

	SDB_QUALIFIER
			Qualifier;

};


// Nothing else uses this, why make it known?
//extern SDB_NAMED_QUALIFIER NamedQualifiers[];

#endif 
