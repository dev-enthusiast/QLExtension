/****************************************************************
**
**	GSRDATERDATEPERIODWITHDATEGEN.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateRDatePeriodWithDateGen.h,v 1.1 2000/03/22 11:51:15 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATERDATEPERIODWITHDATEGEN_H )
#define IN_GSDATE_GSRDATERDATEPERIODWITHDATEGEN_H


#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsRDateDateGen.h>

#include <date.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSTSDB GsRDateRDatePeriodWithDateGen : public GsRDateImp
{
	RDATE
			m_RDate;
	
	GsRDateDateGen
			*m_DateGen;

	GsRDateRDatePeriodWithDateGen() {}

public:

	GsRDateRDatePeriodWithDateGen( RDATE, GsRDateDateGen* );

	GsRDateRDatePeriodWithDateGen( const GsRDateRDatePeriodWithDateGen& Other );
	~GsRDateRDatePeriodWithDateGen();

	GsRDateRDatePeriodWithDateGen& operator=( const GsRDateRDatePeriodWithDateGen& Rhs );

	virtual GsRDateImp* copy() const { return new GsRDateRDatePeriodWithDateGen( *this ); }

	virtual GsDate addToDate( const GsDate& Date ) const;

	virtual GsRDateImp* multiply( int M ); 

	virtual GsString toString() const;
};

CC_END_NAMESPACE

#endif 
