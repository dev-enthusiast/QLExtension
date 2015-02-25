/****************************************************************
**
**	gscore/GsRefCount.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsRefCount.h,v 1.10 2001/11/27 22:41:07 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSREFCOUNT_H )
#define IN_GSCORE_GSREFCOUNT_H

#include	<gscore/base.h>

CC_BEGIN_NAMESPACE( Gs )

class GsRefCount 
{
public:
	bool isUniqueRep()   const { return m_refCount == 1; }
	bool hasReferences() const { return m_refCount != 0; }
	unsigned refCount() const { return m_refCount; }

protected:
	GsRefCount():m_refCount( 0 ) {}
	virtual ~GsRefCount() {}

// .... data members ....
private:
	unsigned m_refCount; 
public: // .... to be called only by GsHandle; declared public to avoid friends
	void incCount() { ++m_refCount; }
	void decCount() { if( --m_refCount == 0 ) delete this; }
};

class GsRefCountCopyOnWrite : public GsRefCount
{
public:
	bool isShareable() const { return m_shareable; }
protected:
	GsRefCountCopyOnWrite():GsRefCount(),m_shareable( true ) {}
	virtual ~GsRefCountCopyOnWrite() {}

// .... data members ....
private:
	bool m_shareable;
public: // .... to be called only by GsHandle; declared public to avoid friends
	void markUnshareable() { m_shareable = false; }
};

CC_END_NAMESPACE
#endif

