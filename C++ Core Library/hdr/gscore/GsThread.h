/****************************************************************
**
**	gscore/GsThread.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsThread.h,v 1.7 2001/11/27 22:41:10 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSTHREAD_H )
#define IN_GSCORE_GSTHREAD_H

#include	<gscore/base.h>

// .... FIX: shouldn't need that when sema.h becomes includable ....
typedef struct PublicSemaphoreStructure* SEMAPHORE;

CC_BEGIN_NAMESPACE( Gs )

class GsThreadComputation;

class EXPORT_CLASS_GSBASE GsThread
{
public:
	GsThread( long int stackSize = 128000 );
	~GsThread();

	void computeBegin( GsThreadComputation* comp );
	void computeWait();

private:
	// .... functions ....
	static void loop( void* thread );
	void exit(); // NOTE: TO BE CALLED ONLY FROM DESTRUCTOR ONCE

	// .... data members ....
	SEMAPHORE
			m_beginSema,
			m_endSema;

	GsThreadComputation*
			m_computation;

	long int
			m_stackSize;

	bool	m_exit;

private:
	// Unclear semantics. Not to be called 
	GsThread( const GsThread& ) {}
	void operator=( const GsThread& ) { }
};

CC_END_NAMESPACE
#endif

