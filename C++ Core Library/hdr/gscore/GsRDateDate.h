/****************************************************************
**
**	GSRDATEDATE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateDate.h,v 1.6 2000/04/11 13:48:29 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEDATE_H )
#define IN_GSDATE_GSRDATEDATE_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsRDateDate : public GsRDateImp
{
	GsDate	m_Date;

public:
	
	GsRDateDate();
	GsRDateDate( const GsDate& Date );
	GsRDateDate( const GsRDateDate& Other );
	~GsRDateDate();

	GsRDateDate& operator=( const GsRDateDate& Rhs );

	virtual GsRDateImp* copy() const;

	virtual GsRDate::quotient_type findQuotient( const GsDate& Date ) const;

	virtual GsDate addToDate( const GsDate& Date ) const;
	virtual GsDate toDate() const;

	virtual GsString toString() const;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS
			
};



CC_END_NAMESPACE

#endif 
