/****************************************************************
**
**	GSFUNCPAIR.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncPair.h,v 1.9 2001/11/27 22:45:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCPAIR_H )
#define IN_GSFUNCPAIR_H

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Domain, class Range1, class Range2 > 
class GsFuncPair : public GsFunc< Domain, CC_STL_PAIR(Range1, Range2) >
{
public:
	GsFuncPair( const GsFunc< Domain, Range1 >& f1, 
				const GsFunc< Domain, Range2 >& f2 ) 
	: m_f1( f1 ), m_f2( f2 ) 
	{
	}
	GsFuncPair( const GsFunc< Domain, Range1 >* f1, 
				const GsFunc< Domain, Range2 >* f2 ) 
	: m_f1( f1 ), m_f2( f2 ) 
	{
	}
	GsFuncPair( GsFuncHandle< Domain, Range1 > f1, 
				GsFuncHandle< Domain, Range2 > f2 ) 
	: m_f1( f1 ), m_f2( f2 ) 
	{
	}
	virtual ~GsFuncPair() {}
	CC_STL_PAIR(Range1, Range2) operator () ( Domain Arg ) const
	{ 
		return CC_NS(std,make_pair)( m_f1(Arg), m_f2(Arg) );
	}

private:

	GsFuncHandle< Domain, Range1 > m_f1;
	GsFuncHandle< Domain, Range2 > m_f2;

};

template< class Domain, class Range1, class Range2>
GsFuncPair< Domain, Range1, Range2 > GsFuncPairMake(
	const GsFunc< Domain, Range1 >&	f1,
	const GsFunc< Domain, Range2 >&	f2
)
{
	return GsFuncPair< Domain, Range1, Range2 >( f1, f2 );
}

CC_END_NAMESPACE

#endif 
