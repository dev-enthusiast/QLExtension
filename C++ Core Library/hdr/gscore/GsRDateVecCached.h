/****************************************************************
**
**	GSRDATEVECCACHED.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateVecCached.h,v 1.1 2000/03/23 18:35:26 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEVECCACHED_H )
#define IN_GSDATE_GSRDATEVECCACHED_H

#include <gscore/base.h>
#include <cccomp.h>
#include <gscore/types.h>
#include <gscore/GsRDateImp.h>


CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsRDateVecCached 
** Description : GsRDateVecCached class
****************************************************************/

class EXPORT_CLASS_GSDATE GsRDateVecCached : public GsRDateImp
{

protected:

	mutable GsDateVector*
			m_Vector;

	virtual GsDateVector getVec() const = 0;
	// Any class that derives from this class should rename it's toVec method to getVec
	inline void clearCache()
	{
		delete m_Vector;
		m_Vector = NULL;
	}

	~GsRDateVecCached();

public:
	GsRDateVecCached();
	GsRDateVecCached( const GsRDateVecCached & );

	GsRDateVecCached &operator=( const GsRDateVecCached &rhs );

	virtual GsDateVector toVec() const;
};


CC_END_NAMESPACE

#endif 
