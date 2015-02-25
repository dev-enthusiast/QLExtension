/****************************************************************
**
**	GSFUNCCTOR.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncCtor.h,v 1.2 1998/11/10 17:33:24 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCCTOR_H )
#define IN_GSFUNCCTOR_H

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Domain, class Range > 
class GsFuncCtor : public GsFunc< Domain, Range >
{
public:

	GsFuncCtor() : m_ret(0) {}
	virtual ~GsFuncCtor() 
	{
		if (m_ret) 
		{
			delete m_ret;
		}
	}
	Range operator () ( Domain Arg ) const
	{ 
		if (m_ret == 0)
		{
			GsFuncCtor<Domain, Range>* This = (GsFuncCtor<Domain, Range>*)this;
			This->m_ret = new Range(Arg);
		}
		return *m_ret;
	}

private:

	Range* m_ret;

};

template< class Domain1, class Domain2, class Range > 
class GsFunc2Ctor : public GsFunc2< Domain1, Domain2, Range >
{
public:

	GsFunc2Ctor() : m_ret(0) {}
	virtual ~GsFunc2Ctor() 
	{
		if (m_ret) 
		{
			delete m_ret;
		}
	}
	Range operator () ( Domain1 Arg1, Domain2 Arg2 ) const
	{ 
		if (m_ret == 0)
		{
			GsFunc2Ctor<Domain1, Domain2, Range>* This = 
					(GsFunc2Ctor<Domain1, Domain2, Range>*)this;
			This->m_ret = new Range(Arg1, Arg2);
		}
		return *m_ret;
	}

private:

	Range* m_ret;

};

CC_END_NAMESPACE

#endif 
