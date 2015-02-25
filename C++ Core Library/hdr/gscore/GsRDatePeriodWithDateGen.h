/****************************************************************
**
**	GSRDATEPERIODWITHDATEGEN.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDatePeriodWithDateGen.h,v 1.1 2000/03/22 11:51:13 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEPERIODWITHDATEGEN_H )
#define IN_GSDATE_GSRDATEPERIODWITHDATEGEN_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsRDateDefaultDateGen.h>
#include <gscore/GsPeriod.h>

CC_BEGIN_NAMESPACE( Gs )

class GsRDate;
class EXPORT_CLASS_GSDATE GsRDatePeriodWithDateGen : public GsRDateImp
{
	GsPeriod	
			m_Period;

	GsRDateDateGen* 
			m_DateGen;

public:
	GsRDatePeriodWithDateGen( const GsPeriod& Period, GsRDateDateGen* DateGen );

	GsRDatePeriodWithDateGen( const GsRDatePeriodWithDateGen& Other );
	~GsRDatePeriodWithDateGen();

	GsRDatePeriodWithDateGen& operator=( const GsRDatePeriodWithDateGen& Rhs );

	virtual GsRDateImp* copy() const;

	virtual GsDate addToDate( const GsDate& Date ) const;

	virtual GsRDateImp* multiply( int M ); 

	virtual GsString toString() const;
};



CC_END_NAMESPACE

#endif 
