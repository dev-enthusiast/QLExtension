/****************************************************************
**
**	GSRDATERANGESTUB.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateRangeStub.h,v 1.4 2000/04/17 15:04:40 loflij Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATERANGESTUB_H )
#define IN_GSDATE_GSRDATERANGESTUB_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsRDateSeries.h>
#include <gscore/GsException.h>
#include	<gscore/GsDateEnums.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsRDateRangeStub : public GsRDateSeries
{
public:
	GsRDateRangeStub();

	GsRDateRangeStub(
		const GsRDate& Increment, 
		const GsRDate& WindowStart, 
		const GsRDate& WindowEnd,
		enum LT_INCLUDE_END IncludeFirst,		// Always, Never	
		enum LT_INCLUDE_END IncludeLast,		// Always, Never	
		enum LT_STUB_TYPE StartStub,			// Short, Long, Full
		enum LT_STUB_TYPE EndStub,				// Short, Long
		int CompoundingPeriodsPerPayment = 1	// Only for Full First with compounding
	);

	GsRDateRangeStub( const GsRDateRangeStub& Other );

	~GsRDateRangeStub();

	GsRDateRangeStub& operator=( const GsRDateRangeStub& Rhs );
};



CC_END_NAMESPACE

#endif 
