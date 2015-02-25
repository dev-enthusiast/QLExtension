/****************************************************************
**
**	GSRDATEPERIODFUNC.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDatePeriod.h,v 1.10 2001/11/27 22:43:11 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEPERIOD_H )
#define IN_GSDATE_GSRDATEPERIOD_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsRDateDefaultDateGen.h>
#include <gscore/GsPeriod.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )


class GsRDate;
class EXPORT_CLASS_GSDATE GsRDatePeriod : public GsRDateImp
{
	GsPeriod	
			m_Period;

	char
			m_Method;
public:

	GsRDatePeriod( const GsString& );
	GsRDatePeriod( const GsPeriod& Period );

	GsRDatePeriod( const GsRDatePeriod& Other );
	~GsRDatePeriod();

	GsRDatePeriod& operator=( const GsRDatePeriod& Rhs );

	virtual GsRDateImp* copy() const;

	virtual GsDate addToDate( const GsDate& Date ) const;

	virtual GsRDateImp* multiply( int M ); 

	virtual GsRDateImp* setDefaultDateGen( GsRDateDateGen* );

	virtual GsString toString() const;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS
};




CC_END_NAMESPACE

#endif 
