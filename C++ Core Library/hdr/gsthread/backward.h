/****************************************************************
**
**	BACKWARD.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/backward.h,v 1.4 2001/11/27 22:48:07 procmon Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_BACKWARD_H
#define IN_GSTHREAD_BACKWARD_H

#ifndef GSTHREAD_KLUDGE
#error "DO NOT USE THIS!!!! ONLY FOR MIGRATING OLD BROKEN CODE!!!!"
#endif

#include <gsthread/gsthread_base.h> // for EXPORT macros
#include <extension/GsNoCopy.h> // for GsNoCopy<Y>

CC_BEGIN_NAMESPACE( Gs )

class GsRecursiveMutexRep;

// recursive mutex class for migrating old code ONLY!!!!!
class EXPORT_CLASS_GSTHREAD GsRecursiveMutex
	: private CC_NS(extension,GsNoCopy)<GsRecursiveMutex>
{
public:
	GsRecursiveMutex() throw();
	~GsRecursiveMutex() throw();

	// no copy constructor due to GsNoCopy
	// no assignment operator due to GsNoCopy

private:
	void m_lock()    throw();
	void m_unlock()  throw();

	GsRecursiveMutexRep* m_rep;

	friend class GsRecursiveMutexLock;
};

class GsRecursiveMutexLock
	: private CC_NS(extension,GsNoCopy)<GsRecursiveMutexLock>
{
public:
	GsRecursiveMutexLock( GsRecursiveMutex& mut ) throw() : m_mut(mut) { m_mut.m_lock();}
	~GsRecursiveMutexLock() throw() { m_mut.m_unlock();}

private:
	GsRecursiveMutex& m_mut;
};

CC_END_NAMESPACE

#endif 
