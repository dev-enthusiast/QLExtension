/****************************************************************
**
**	gscore/GsFunc.h	- 
**
**	Copyright 1998-2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsFunc.h,v 1.3 2001/11/27 22:41:04 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSFUNC_H )
#define IN_GSCORE_GSFUNC_H

#include <gscore/base.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsException.h>
#include <gscore/GsStreamMap.h>

CC_BEGIN_NAMESPACE( Gs )

////////////
// GsFunc0
////////////

template< class Range > 
class GsFunc0 : public GsRefCount
{
public:
	GsFunc0() {}
	virtual ~GsFunc0() {}

	virtual Range operator () () const = 0;

	operator Range () const
	{
		return (*this)();
	}
};

///////////
// GsFunc
///////////

template <class T1> class GsHandleReadOnly;
template <class T1, class T2> class GsFuncHandle;

template< class Domain, class Range > 
class GsFunc : public GsRefCount
{
public:

	typedef Domain domain_type;
	typedef Range range_type;
	class Sample;
	class ConstIter;
	typedef Sample value_type;				// discrete functions
	typedef ConstIter const_iterator;		// discrete functions

	friend class GsHandleReadOnly< GsFunc<Domain, Range> >;
	friend class GsFuncHandle< Domain, Range>;

	GsFunc() {}
	virtual ~GsFunc() {}
	virtual Range operator () ( Domain Arg ) const = 0;
	
	virtual GsStreamMap* buildStreamMapper() const { return NULL; }

	//This returns newly allocated memory which should immediatly wrapped by a GsFuncHandle
	virtual GsFunc< Domain, Range >* Integrate( double initialValue ) const {
		GSX_THROW(GsXBadIndex, "Attempt to Integrate over non-integratable GsFunc");
		return 0;
	}
	
	///////////////////////
	// Domain Descriptions
	///////////////////////

	typedef Domain (GsFunc<Domain,Range>::*DomainEnumerator)(int) const;
	typedef Range (GsFunc<Domain,Range>::*RangeEnumerator)(int) const;

    class Sample
	{
	public:
		Sample(Domain x, Range y) : first(x), second(y) {}

//		bool operator == (const Sample& s) const
//		{
//			return (s.first == first);
//		}
		
		Domain first;
		Range second;
	};

	class ConstIter : public GsFunc0<Range>
	{
		
	public:
		ConstIter(const GsFunc<Domain, Range>& f, 
				   int p, 
				   DomainEnumerator de, 
				   RangeEnumerator re) 
		: m_pos(p), m_func(&f), m_de(de), m_re(re)
		{
		}
		ConstIter& operator=(const ConstIter& x)
		{
			m_pos = x.m_pos;
			m_func = x.m_func;
			m_de = x.m_de;
			m_re = x.m_re;
			return *this;
		}
		ConstIter& operator++()
		{
			m_pos++; 
			return *this;
		}
		ConstIter& operator++(int)
		{
			m_pos++; 
			return *this;
		}
		ConstIter& operator--()
		{
			m_pos--; 
			return *this;
		}
		ConstIter& operator--(int)
		{
			m_pos--; 
			return *this;
		}

		int operator - (const ConstIter& other)
		{
			return m_pos - other.m_pos;
		}
		int operator < (const ConstIter& other)
		{
			return m_pos < other.m_pos;
		}
		int operator <= (const ConstIter& other)
		{
			return m_pos <= other.m_pos;
		}
		int operator > (const ConstIter& other)
		{
			return m_pos > other.m_pos;
		}
		int operator >= (const ConstIter& other)
		{
			return m_pos >= other.m_pos;
		}
		int operator == (const ConstIter& other)
		{
			return m_pos == other.m_pos;
		}
		int operator != (const ConstIter& other)
		{
			return m_pos != other.m_pos;
		}
		const value_type operator * () const
		{
			return Sample( (m_func->*m_de)(m_pos), (m_func->*m_re)(m_pos) );
		}
		Range operator () () const
		{
			return (m_func->*m_re)(m_pos);
		}
	private:
		int m_pos;
		const GsFunc<Domain, Range>* m_func;
		DomainEnumerator m_de;
		RangeEnumerator m_re;
	};
	
	// nonzero -- enumerates domain values for which result is nonzero
	const_iterator nonZeroBegin() const	
	{ 
		// FIX-SparcWorks barfs without intermediate vars
		DomainEnumerator de = &GsFunc<Domain,Range>::nonZeroDomain;
		RangeEnumerator re = &GsFunc<Domain,Range>::nonZeroRange; 
		return ConstIter(*this, 0, de, re ); 
	}
	const_iterator nonZeroEnd() const 
	{ 
		// FIX-SparcWorks bars without intermediate vars
		DomainEnumerator de = &GsFunc<Domain,Range>::nonZeroDomain;
		RangeEnumerator re = &GsFunc<Domain,Range>::nonZeroRange; 
		return ConstIter(*this, nonZeroSize(), de, re );
	}

	value_type nonZeroBack() const 
	{ 
		ConstIter iter = nonZeroEnd();
		iter--;
		return *iter;
	}

	virtual int nonZeroSize() const 
	{ 
		GSX_THROW(GsXBadIndex, "Attempt to iterate over non-iterable GsFunc");
		return -1; 
	}
	virtual Domain nonZeroDomain(int) const 
	{ 
		GSX_THROW(GsXBadIndex, "Attempt to iterate over non-iterable GsFunc");
		return nonZeroDomain(0);
	}
	virtual Range nonZeroRange(int i) const 
	{ 
		GSX_THROW(GsXBadIndex, "Attempt to iterate over non-iterable GsFunc");
		return nonZeroRange(0); 
	}

};

///////////
// GsFunc2
///////////

template <class T1, class T2, class T3> class GsFunc2Handle;

template< class Domain1, class Domain2, class Range > 
class GsFunc2 : public GsRefCount
{
public:

	typedef Domain1 domain1_type;
	typedef Domain2 domain2_type;
	typedef Range range_type;

	friend class GsHandleReadOnly< GsFunc2<Domain1, Domain2, Range> >;
	friend class GsFunc2Handle<Domain1, Domain2, Range>;

	GsFunc2() {}
	virtual ~GsFunc2() {}

	virtual Range operator () ( Domain1 Arg1, Domain2 Arg2 ) const = 0;

};

CC_END_NAMESPACE

#endif

