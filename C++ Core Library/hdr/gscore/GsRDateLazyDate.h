/****************************************************************
**
**	GSRDATELAZYDATE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateLazyDate.h,v 1.3 2000/04/12 13:31:55 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATELAZYDATE_H )
#define IN_GSDATE_GSRDATELAZYDATE_H

#include <gscore/base.h>
#include <gscore/GsRDateDateCached.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsRDateLazyDate: public GsRDateDateCached
{
	GsString m_String;

public:

	GsRDateLazyDate();
	GsRDateLazyDate( const GsString& Str );
	GsRDateLazyDate( const GsRDateLazyDate& Other);
	~GsRDateLazyDate();

	GsRDateLazyDate&	operator=( const GsRDateLazyDate& Rhs );

	virtual	GsDate		addToDate( const GsDate& Date )	const	{ return toDate(); }
	virtual	GsDate		getDate()						const;

	virtual	GsString	toString()						const	{ return m_String; }
	virtual	GsRDateImp*	copy()							const	{ return new GsRDateLazyDate( *this ); }

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS
};

CC_END_NAMESPACE

#endif 






