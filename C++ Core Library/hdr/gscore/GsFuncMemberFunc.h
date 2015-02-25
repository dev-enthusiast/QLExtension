/****************************************************************
**
**	GSFUNCMEMBERFUNC.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncMemberFunc.h,v 1.2 2000/12/18 21:33:04 irwinc Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCMEMBERFUNC_H )
#define IN_GSFUNCMEMBERFUNC_H

#include <gscore/base.h>
#include <gscore/GsException.h>

CC_BEGIN_NAMESPACE( Gs )

template< class T, class Domain, class Range > 
class GsFuncMemberFunc : public GsFunc< Domain, Range >
{
public:

	typedef Range (T::*MemFuncType)(Domain) const;

	GsFuncMemberFunc(const T* obj, MemFuncType f)
	: m_obj(obj), m_delete(true), m_f(f)
	{
	}


	GsFuncMemberFunc(const T& obj, MemFuncType f)
	: m_obj(&obj), m_delete(false), m_f(f)
	{
	}

	~GsFuncMemberFunc()
	{
		if (m_delete)
		{
			delete m_obj;
		}
	}

	Range operator () ( Domain Arg ) const
	{
		return (m_obj->*m_f)(Arg);
	}
	
private:

	const T* m_obj;
	bool m_delete;
	MemFuncType m_f;
};

CC_END_NAMESPACE

#endif 
