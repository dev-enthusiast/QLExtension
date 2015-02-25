/****************************************************************
**
**	GSRDATEDATEWITHDATEGEN.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateDateWithDateGen.h,v 1.4 2000/04/11 10:52:37 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEDATEWITHDATEGEN_H )
#define IN_GSDATE_GSRDATEDATEWITHDATEGEN_H

#include <gscore/base.h>
#include <gscore/GsRDateDateCached.h>
#include <gscore/GsRDateDateGen.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )


class EXPORT_CLASS_GSDATE GsRDateDateWithDateGen : public GsRDateDateCached
{
	GsDate	m_Date;
	GsRDateDateGen* m_DateGen;

	GsRDateDateWithDateGen();

public:
	
	GsRDateDateWithDateGen( const GsDate& Date, GsRDateDateGen*  );
	GsRDateDateWithDateGen( const GsRDateDateWithDateGen& Other );
	~GsRDateDateWithDateGen();

	GsRDateDateWithDateGen& operator=( const GsRDateDateWithDateGen& Rhs );

	virtual GsRDateImp* copy() const { return new GsRDateDateWithDateGen( *this ); }

	virtual GsDate addToDate( const GsDate& Date ) const;
	virtual GsDate getDate() const;

	virtual GsString toString() const;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS
};

CC_END_NAMESPACE

#endif 
