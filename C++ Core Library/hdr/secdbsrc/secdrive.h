/****************************************************************
**
**	SECDRIVE.H	- Database server/driver linked functions
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdrive.h,v 1.14 2000/12/19 17:20:47 simpsk Exp $
**
****************************************************************/

#if !defined( IN_SECDRIVE_H )
#define IN_SECDRIVE_H

#include	<secindex.h>

/*
**  Forward declarations
*/

struct HashStructure;
struct MemPoolStructure;


#define	SDB_INDEX_DESCRIPTOR_ERROR	( (SDB_INDEX_DESCRIPTOR_NUMBER) -1 )


/*
**	Return codes from SecDbDriverResolveInsert
*/

enum SDB_RESOLVE_ACTION
{
	SDB_RESOLVE_FAIL,
	SDB_RESOLVE_IGNORE,
	SDB_RESOLVE_UPDATE

};


/*
**	Define handle structure for incremental api
*/

struct SDB_DRIVER_INCREMENTAL
{
	SDB_STORED_OBJECT
			StoredObject;	// object after increment
			
	HashStructure
			*Values;		// Values of object after increment
			
	MemPoolStructure
			*MemPool;

	unsigned
			Insert:1;		// TRUE:  Object did not exist before increment; 
							// FALSE: Object did exist before increment
	
};


/*
**	Incremental function supplied by class:
**		Allocates and fills in	Incremental->Mem
**		Fills in 				Incremental->MemSize
**		Adds values to			Incremental->Values		with SecDbDriverAddValue
*/

typedef int (SDB_INCREMENTAL_FUNC)( SDB_DRIVER_INCREMENTAL *Incremental, void *CurMem, unsigned int CurMemSize, void *MsgMem, unsigned int MsgMemSize, int Direction );


/*
**	Prototypes
*/

EXPORT_C_SECDB int
		SecDbDriverStartup(				const char *Path, const char *FileName ),
		SecDbDriverIncremental(			SDB_DRIVER_INCREMENTAL *Incremental, SDB_DISK *CurSdbDisk, void *CurMem, unsigned int CurMemSize, SDB_DISK *MsgSdbDisk, void *MsgMem, unsigned int MsgMemSize, int Direction ),
		SecDbDriverIndexIncremental(	SDB_DRIVER_INCREMENTAL *Incremental, SDB_INDEX_DESCRIPTOR *IndexDesc, DT_VALUE *Values );

EXPORT_C_SECDB void
		SecDbDriverIncrementalFree(		SDB_DRIVER_INCREMENTAL *Incremental ),
		SecDbDriverShutdown(			void );

EXPORT_C_SECDB int
		SecDbDriverAddValue( 			SDB_DRIVER_INCREMENTAL *Incremental, char *ValueTypeName, DT_VALUE_RESULTS *Value );

EXPORT_C_SECDB SDB_RESOLVE_ACTION
		SecDbDriverResolveInsert(		SDB_STORED_OBJECT *Proposed, SDB_STORED_OBJECT *Current );
		
EXPORT_C_SECDB void
		SecDbDriverErrStartup(			void );

#endif

