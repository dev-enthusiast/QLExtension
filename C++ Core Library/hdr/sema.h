/****************************************************************
**
**	SEMA.H	- Portable semaphore interface definition
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/kool_ade/src/sema.h,v 1.9 2001/11/27 22:49:08 procmon Exp $
**
****************************************************************/

#if !defined( IN_SEMA_H )
#define IN_SEMA_H


/*
**	Arguments to SemaphoreWait
**
**	Note:	SEM_INDEFINITE_WAIT and SEM_IMMEDIATE_RETURN are the same
**			values, but not as portably defined.
*/

#define GS_SEM_IMMEDIATE_RETURN	(0)
#define GS_SEM_INDEFINITE_WAIT	(-1)


/*
**	PublicSemaphoreStructure exists only as a type name, each
**	implementation has a PRIVATE_SEMAPHORE structure.
*/

typedef struct PublicSemaphoreStructure *SEMAPHORE;


DLLEXPORT void
		SemaphoreInitialize(	void );

// Use a count of
//	0 to get a Semaphore
//	1 to get a Mutex
// Don't use any other counts
DLLEXPORT SEMAPHORE
		SemaphoreNew( 			int Count );

// Known in textbooks as the 'P' semaphore operation.
DLLEXPORT int
		SemaphoreWait( 			SEMAPHORE Sema, long Period );

// Known in textbooks as the 'V' semaphore operation.
DLLEXPORT void
		SemaphorePost( 			SEMAPHORE Sema );

DLLEXPORT void
		SemaphoreFree(			SEMAPHORE Sema );


#endif

