/****************************************************************
**
**	GSFUNCCOMBO.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncCombo.h,v 1.8 2011/05/17 18:36:11 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCCOMBO_H )
#define IN_GSFUNCCOMBO_H

#include <vector>
#include <ccstl.h>
#include <gscore/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template<class Domain, class Range1, class Range2, class Combined>
class GsFuncCombo : public GsFunc<Domain, Combined>
{
public:
	GsFuncCombo(GsFuncHandle<Domain, Range1> f1,
				GsFuncHandle<Domain, Range2> f2,
				GsFunc2Handle<Range1, Range2, Combined> c)
	: m_f1(f1), m_f2(f2), m_combiner(c), m_combineDone(false)
	{
	}

	Combined operator () ( Domain Arg ) const
	{
		return m_combiner(m_f1(Arg), m_f2(Arg));
	}

	virtual int nonZeroSize() const 
	{ 
		nonZeroCombine();
		return m_dcombined.size(); 
	}
	virtual Domain nonZeroDomain(int i) const 
	{ 
		nonZeroCombine();
		return m_dcombined[i]; 
	}
	virtual Combined nonZeroRange(int i) const 
	{ 
		nonZeroCombine();
		return m_rcombined[i]; 
	}
	
	void nonZeroCombine() const
	{
		if (m_combineDone)
			return;

		GsFuncCombo<Domain, Range1, Range2, Combined>* This = 
				(GsFuncCombo<Domain, Range1, Range2, Combined>*)this;

		typename GsFuncHandle<Domain, Range1>::const_iterator iter1 = m_f1.nonZeroBegin();
		typename GsFuncHandle<Domain, Range1>::const_iterator end1 = m_f1.nonZeroEnd();
		typename GsFuncHandle<Domain, Range2>::const_iterator iter2 = m_f2.nonZeroBegin();
		typename GsFuncHandle<Domain, Range2>::const_iterator end2 = m_f2.nonZeroEnd();
		while (iter1 < end1 || iter2 < end2)
		{
			if (iter2 == end2 || (*iter1).first < (*iter2).first) 
			{
				This->m_dcombined.push_back((*iter1).first);
				This->m_rcombined.push_back((*iter1).second);
				iter1++;
			}
			else if (iter1 == end1 || (*iter2).first < (*iter1).first) 
			{
				This->m_dcombined.push_back((*iter2).first);
				This->m_rcombined.push_back((*iter2).second);
				iter2++;
			}
			else {
				This->m_dcombined.push_back((*iter2).first);
				This->m_rcombined.push_back(m_combiner((*iter1).second, (*iter2).second));
				iter1++;
				iter2++;
			}
		}
		This->m_combineDone = true;
	}

private:

	GsFuncHandle<Domain, Range1> m_f1;
	GsFuncHandle<Domain, Range2> m_f2;
	GsFunc2Handle<Range1, Range2, Combined> m_combiner;
	
	// combined
	GsBool m_combineDone;
	CC_STL_VECTOR( Domain )		m_dcombined;
	CC_STL_VECTOR( Combined )	m_rcombined;

};

template< class Domain, class Range1, class Range2, class Combined>
GsFuncHandle< Domain, Combined > 
GsFuncComboMake(
	GsFuncHandle<Domain, Range1> f1,
	GsFuncHandle<Domain, Range2> f2,
	GsFunc2Handle<Range1, Range2, Combined> combiner
)
{
	return new GsFuncCombo< Domain, Range1, Range2, Combined >( f1, f2, combiner );
}

CC_END_NAMESPACE

#endif 
