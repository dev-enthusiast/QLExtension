/* $Header: /home/cvs/src/gsdate/src/gscore/GsPreviousBusinessDay.h,v 1.1 2000/03/03 02:04:34 loflij Exp $ */
/****************************************************************
**
**	GsPreviousBusinessDay.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_GSPREVIOUSBUSINESSDAY_H )
#define IN_GSPREVIOUSBUSINESSDAY_H

#include	<gscore/base.h>
#include	<gscore/GsRoundingConvention.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsPreviousBusinessDay : public GsRoundingConvention
{
public:
    virtual const GsString& getName() const ;
    virtual const GsString& getDescription() const ;

    virtual GsRoundingConvention* clone() const ;

    virtual GsDate apply( const GsDate& date, const GsHolidaySchedule& hs ) const ;
};

CC_END_NAMESPACE

#endif




