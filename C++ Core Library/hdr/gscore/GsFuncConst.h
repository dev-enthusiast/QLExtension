/****************************************************************
**
**	GSFUNCCONST.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncConst.h,v 1.1 1999/09/24 21:47:33 tsoucp Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCCONST_H )
#define IN_GSFUNC_GSFUNCCONST_H

#include <gscore/base.h>

#include <gscore/GsFunc.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Domain, class Range >
class GsFuncConst : public GsFunc< Domain, Range >
{
public:
	GsFuncConst(): m_constVal() {}
	GsFuncConst( const Range constVal ) : m_constVal( constVal ) {}
	virtual ~GsFuncConst() {}

	virtual Range operator()( Domain d ) const { return m_constVal; }

protected:
	Range m_constVal;
};

CC_END_NAMESPACE

#endif 
