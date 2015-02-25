/****************************************************************
**
**	GSFUNCIMPLICIT.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncImplicit.h,v 1.2 1998/11/10 17:33:25 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCIMPLICIT_H )
#define IN_GSFUNCIMPLICIT_H

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Domain, class Range > 
class GsFuncImplicit : public GsFunc<Domain, Range>
{
public:
	GsFuncImplicit(Domain& port, 
				   const GsFunc0<Range>& f)
	: m_f(f), m_port(port)
	{
	}
	GsFuncImplicit(Domain& port, 
				   const GsFunc0<Range>* f)
	: m_f(f), m_port(port)
	{
	}
	GsFuncImplicit(Domain& port, 
				   GsFunc0Handle<Range> f)
	: m_f(f), m_port(port)
	{
	}

	virtual ~GsFuncImplicit() {}
	Range operator () (Domain d) const
	{
		m_port = d;
		return m_f();
	}

private:

	GsFunc0Handle< Range > m_f;
	Domain& m_port;
	
};

CC_END_NAMESPACE

#endif 
