/* $Header: /home/cvs/src/gsdate/src/gscore/GsNoRounding.h,v 1.5 1999/11/04 22:13:27 francm Exp $ */
/****************************************************************
**
**	GsNoRounding.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_GSNOROUNDING_H )
#define IN_GSNOROUNDING_H

#include	<gscore/base.h>
#include	<gscore/GsRoundingConvention.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsNoRounding : public GsRoundingConvention
{
public:
	virtual const GsString& getName() const ;
	virtual const GsString& getDescription() const ;  
	
	virtual GsRoundingConvention* clone() const ;

	virtual GsDate apply(const GsDate& date, const GsHolidaySchedule&) const ;
};

CC_END_NAMESPACE

#endif




