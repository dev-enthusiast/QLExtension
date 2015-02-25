/****************************************************************
**
**	GSRDATEDATECACHED.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateDateCached.h,v 1.4 2000/04/11 13:49:58 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEDATECACHED_H )
#define IN_GSDATE_GSRDATEDATECACHED_H

#include <gscore/base.h>
#include <cccomp.h>
#include <gscore/types.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsRDateDateCached 
** Description : GsRDateDateCached class
****************************************************************/

class EXPORT_CLASS_GSDATE GsRDateDateCached : public GsRDateImp
{
protected:

	mutable GsDate*
			m_Date;

	virtual GsDate getDate() const = 0;
	// Any class that derives from this class should rename it's toDate method to getDate
	
	inline void clearCache()
	{
		delete m_Date;
		m_Date = NULL;
	}


	~GsRDateDateCached();
	
public:
	GsRDateDateCached();
	GsRDateDateCached( const GsRDateDateCached & );

	GsRDateDateCached &operator=( const GsRDateDateCached &rhs );

	virtual GsRDate::quotient_type findQuotient( const GsDate& Date ) const;

	virtual GsDate toDate() const;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS

};


CC_END_NAMESPACE

#endif 
