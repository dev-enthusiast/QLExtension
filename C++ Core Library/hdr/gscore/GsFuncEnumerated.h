/****************************************************************
**
**	GSFUNCENUMERATED.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncEnumerated.h,v 1.5 1999/12/09 03:24:29 deeka Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCENUMERATED_H )
#define IN_GSFUNCENUMERATED_H

#include <vector>
#include <gscore/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template<class DomainVec, class RangeVec>
class GsFuncEnumerated 
: public GsFunc< typename DomainVec::value_type,
				 typename RangeVec ::value_type >
{
public:

	GsFuncEnumerated(const DomainVec& in,
					 const RangeVec& out,
					 typename RangeVec::value_type elsewhere
					 )
	: m_in(in), m_out(out), m_elsewhere(elsewhere)
	{
		// FIX -- should check that all elements in domain vector are
		//     distinct
	}
	
	typename RangeVec::value_type operator () (
		typename DomainVec::value_type Arg
	) const
	{
		// FIX -- should avoid linear search
		typename DomainVec::const_iterator diter = m_in.begin();
		typename DomainVec::const_iterator end = m_in.end();
		typename RangeVec::const_iterator riter = m_out.begin();
		while( diter < end )
		{
			if( *diter == Arg )
				return *riter;
			diter++;
			riter++;
		}
		return m_elsewhere;
	}

	virtual int nonZeroSize() const 
	{ 
		return m_in.size(); 
	}

	virtual typename DomainVec::value_type nonZeroDomain(int i) const 
	{ 
		return m_in[i]; 
	}
	virtual typename RangeVec::value_type nonZeroRange(int i) const 
	{ 
		return m_out[i]; 
	}

private:

	DomainVec m_in;
	RangeVec m_out;
	typename RangeVec::value_type m_elsewhere;
};


/*
SW4.2 compiler does not recognize the types, so comment out the function for now
template< class DomainVec, class RangeVec>
GsFuncHandle<
		typename DomainVec::value_type,
		typename RangeVec::value_type > 
GsFuncEnumeratedMake(
	const DomainVec& ins,
	const RangeVec& outs,
	typename RangeVec::value_type elsewhere
)
{
	return new GsFuncEnumerated< DomainVec, RangeVec >( ins, outs, elsewhere );
}

*/
CC_END_NAMESPACE

#endif 
