/****************************************************************
**
**	GSFUNCSAMPLE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncSample.h,v 1.1 1999/11/09 16:49:03 tsoucp Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCSAMPLE_H )
#define IN_GSFUNC_GSFUNCSAMPLE_H

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Domain, class Range > 	
class GsFuncSample : 
	public GsFunc< GsFuncHandle< Domain, Range >, 
				   Range >
{
public:	
	GsFuncSample( const Domain& val ) : m_val( val ) {}
	~GsFuncSample() {}
	
	virtual Range operator()( GsFuncHandle< Domain, Range > f ) const
	{
		return f( m_val );
	}

private:
	Domain m_val;
};

CC_END_NAMESPACE

#endif 
