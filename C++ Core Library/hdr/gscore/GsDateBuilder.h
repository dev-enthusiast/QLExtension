/****************************************************************
**
**	gscore/GsDateBuilder.h - Database access layer
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsDateBuilder.h,v 1.1 1999/03/22 16:53:49 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSDATEBUILDER_H )
#define IN_GSCORE_GSDATEBUILDER_H

#include	<gscore/base.h>
#include	<gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	GsDateDbAccessor - Abstract base class for GsDate database access
**	SeeAlso: GsDateDbAccessor
*/

class EXPORT_CLASS_GSDATE GsDateBuilder
{
public:
	static void setBuilder( GsDateBuilder *builder );
	static GsDateBuilder *getBuilder();

	virtual ~GsDateBuilder() = 0;

 	virtual GsString findScheduleName( const GsString& description ) const = 0;
	virtual GsHolidaySchedule* buildSchedule( const GsString& name ) = 0;

 	virtual GsString findCalendarName( const GsString& description ) const = 0;
	virtual GsCalendar* buildCalendar( const GsString& name ) = 0;

 	virtual GsString findTermCalcName( const GsString& description ) const = 0;
	virtual GsTermCalc* buildTermCalc( const GsString& name ) = 0;
};

CC_END_NAMESPACE
#endif

