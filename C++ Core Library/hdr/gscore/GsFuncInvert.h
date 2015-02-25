/****************************************************************
**
**	GSFUNCINVERT.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncInvert.h,v 1.6 2001/11/27 22:45:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCINVERT_H )
#define IN_GSFUNC_GSFUNCINVERT_H

#include <gscore/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Domain, class Range >
class GsFuncInvert : public GsFunc< const Range&, Domain >
{
public:
	virtual ~GsFuncInvert() {}
	
	virtual Domain operator()( const Range& x ) const = 0;

protected:
	GsFuncInvert( const Domain& v ): m_initialValue( v ) {}

	Domain m_initialValue;
};

CC_END_NAMESPACE

#endif 





