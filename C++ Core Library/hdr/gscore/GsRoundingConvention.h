/* $Header: /home/cvs/src/gsdate/src/gscore/GsRoundingConvention.h,v 1.7 1999/11/04 22:13:30 francm Exp $ */
/****************************************************************
**
**	GsRoundingConvention.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_GSROUNDINGCONVENTION_H )
#define IN_GSROUNDINGCONVENTION_H

#include	<gscore/base.h>
#include	<gscore/GsDate.h>
#include	<gscore/GsStream.h>
#include	<new>

//class std::bad_alloc;

CC_BEGIN_NAMESPACE( Gs )

class GsHolidaySchedule;

class EXPORT_CLASS_GSDATE GsRoundingConvention
{
public:	
	virtual ~GsRoundingConvention()  {}

	virtual const GsString& getName() const  = 0;
	virtual const GsString& getDescription() const  = 0;

	virtual GsRoundingConvention* clone() const  = 0;

	virtual GsDate apply(const GsDate& date, const GsHolidaySchedule& hs) const  = 0;

	// Streaming support.
	virtual void StreamStore( GsStreamOut &Stream ) const;
	virtual void StreamRead( GsStreamIn &Stream );
};

CC_END_NAMESPACE

#endif




