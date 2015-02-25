/****************************************************************
**
**	GSFUNCAPPLY.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncApply.h,v 1.2 1998/11/10 17:33:24 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCAPPLY_H )
#define IN_GSFUNCAPPLY_H

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Domain, class Range > 
class GsFuncApply : public Delay< Range >
{
public:
	GsFuncApply(const GsFunc<Domain, Range>& f, 
				Domain d)
	: m_f(f), m_d(d)
	{
	}
	GsFuncApply(const GsFunc<Domain, Range>* f, 
				Domain d)
	: m_f(f), m_d(d)
	{
	}
	virtual ~GsFuncApply() {}
	Range operator () () const
	{
		return m_f(m_d);
	}

private:

	GsFuncHandle<Domain, Range> m_f;
	Domain m_d;
	
};


template< class Domain1, class Domain2, class Range > 
class GsFunc2Apply : public GsFunc< Domain2, Range >
{
public:

	GsFunc2Apply(const GsFunc2<Domain1, Domain2, Range>& f, Domain1 d)
	: m_f(f), m_d(d)
	{
	}
	GsFunc2Apply(const GsFunc2<Domain1, Domain2, Range>* f, 
				Domain1 d)
	: m_f(f), m_d(d)
	{
	}
	virtual ~GsFunc2Apply() {}
	Range operator () (Domain2 Arg2) const
	{
		return m_f(m_d, Arg2);
	}

private:

	GsFunc2Handle<Domain1, Domain2, Range> m_f;
	Domain1 m_d;
	
};

template< class Domain1, class Domain2, class Range>
GsFuncHandle< Domain2, Range > GsFuncApplyMake(
	const GsFunc2<Domain1, Domain2, Range>* f,
	Domain1 x
)
{
	return new GsFunc2Apply< Domain1, Domain2, Range >( f, x );
}

CC_END_NAMESPACE

#endif 
