/* $Header: /home/cvs/src/gsbase/src/gscore/GsAssert.h,v 1.7 2001/11/27 22:41:02 procmon Exp $ */
/****************************************************************
**
**	GsAssert.h	- GS_ASSERT macro
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsAssert.h,v 1.7 2001/11/27 22:41:02 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSASSERT_H )
#define IN_GSCORE_GSASSERT_H

#include	<gscore/GsException.h>
#include	<gscore/GsThrow.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSBASE GsXAssertionFailure : public GsException
{
public:
	GsXAssertionFailure( const char *Desc, const char *File, int Line ) CC_NOTHROW;
	virtual ~GsXAssertionFailure() CC_NOTHROW;
	
	virtual const char *what() const CC_NOTHROW;
	virtual const char *name() const CC_NOTHROW;

private:
	GsString m_description;
};


inline void GsAssert(
	bool		Test,	// Must be true
	const char	*Desc,	// String to display
	const char	*File,	// __FILE__
	int			Line )	// __LINE__
{
	if( !Test )
		GsThrow( GsXAssertionFailure( Desc, File, Line ));
}

#ifndef GS_ASSERT_OFF
#define GS_ASSERT_ON
#endif

// FIX-Should also allow use of normal assert() which starts debugger in MS
#ifdef GS_ASSERT_ON
#  define GS_ASSERT(test) GsAssert(test,"Assert Failed: " #test,__FILE__,__LINE__)
#else
#  define GS_ASSERT(test)
#endif

CC_END_NAMESPACE

#endif

