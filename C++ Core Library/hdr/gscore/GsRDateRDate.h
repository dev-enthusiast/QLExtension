/****************************************************************
**
**	GSRDATERDATE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateRDate.h,v 1.6 2001/11/27 22:43:12 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATERDATE_H )
#define IN_GSDATE_GSRDATERDATE_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsDate.h>
#include <gscore/GsRDateDateGen.h>

#include <date.h>

CC_BEGIN_NAMESPACE( Gs )

class  EXPORT_CLASS_GSTSDB GsRDateRDate : public GsRDateDateGen
{
	GsString	
			m_Ccy1,
			m_Ccy2;

	GsRDateRDate( const GsRDateRDate& Other ) {}
	GsRDateRDate& operator=( const GsRDateRDate& Rhs ) { return *this; }

	~GsRDateRDate();

public:
	
	GsRDateRDate( const GsString& Ccy1 = GsString(), 
				  const GsString& Ccy2 = GsString() );

	virtual GsDate addToDate( const GsDate& Date, RDATE ) const;
	virtual GsDate rounding( const GsDate& ) const;

	virtual GsRDateImp* createPeriodImp( int Offset, char Method );

	virtual GsString description() const ;
};


CC_END_NAMESPACE

#endif 
