/****************************************************************
**
**	GSRDATERDATEPERIOD.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateRDatePeriod.h,v 1.1 2000/03/22 11:51:14 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATERDATEPERIOD_H )
#define IN_GSDATE_GSRDATERDATEPERIOD_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsRDateDefaultDateGen.h>

#include <date.h>


CC_BEGIN_NAMESPACE( Gs )


class EXPORT_CLASS_GSTSDB GsRDateRDatePeriod : public GsRDateImp
{
	RDATE
			m_RDate;

	GsRDateRDatePeriod() {}

public:

	GsRDateRDatePeriod( const GsString& );
	GsRDateRDatePeriod( RDATE );

	GsRDateRDatePeriod( const GsRDateRDatePeriod& Other );
	~GsRDateRDatePeriod();

	GsRDateRDatePeriod& operator=( const GsRDateRDatePeriod& Rhs );

	virtual GsRDateImp* copy() const { return new GsRDateRDatePeriod( *this ); }

	virtual GsDate addToDate( const GsDate& Date ) const;

	virtual GsRDateImp* multiply( int M ); 

	virtual GsRDateImp* setDefaultDateGen( GsRDateDateGen* );

	virtual GsString toString() const;
};


CC_END_NAMESPACE

#endif 
