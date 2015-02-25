/****************************************************************
**
**	GSFUNCHANDLE.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsFuncHandle.h,v 1.4 2011/05/16 20:06:08 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCHANDLE_H )
#define IN_GSFUNCHANDLE_H

#include <gscore/GsHandle.h>
#include <gscore/GsFunc.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Range >
class GsFunc0Handle : public GsHandleReadOnly< GsFunc0<Range> >
{
public:

	typedef GsFunc0<Range> RepType;

	GsFunc0Handle(const GsFunc0Handle<Range>& obj)
	: GsHandleReadOnly< RepType >(obj)
	{
	}

	// FIX -- following two constructors exist to allow gsfunc
	//			reps to be created without handles

	GsFunc0Handle(const RepType* obj = 0)
	: GsHandleReadOnly< RepType >( obj )
	{
	}

	explicit GsFunc0Handle(const RepType& obj)
	: GsHandleReadOnly< RepType >( &obj )
	{
		if (this->m_rep)
		{
			this->m_rep->incCount();
		}
	}

	virtual ~GsFunc0Handle()
	{
	}

	void operator = (const GsFunc0Handle<Range>& f)
	{
		GsHandleReadOnly< RepType >::operator=(f);
	}

	void operator = (const RepType& f)
	{
		GsHandleReadOnly< RepType >::operator=( GsFunc0Handle<Range>(f) );
	}

	void operator = (const GsFunc0<Range>* f)
	{
		GsHandleReadOnly< RepType >::operator=( GsFunc0Handle<Range>(f) );
	}

	Range operator () () const
	{
		return (*this->m_rep)();
	}

};


template< class Domain, class Range >
class GsFuncHandle : public GsHandleReadOnly< GsFunc<Domain, Range> >
{
public:

	typedef GsDtFuncRepT< Domain, Range > GsDtType;

	typedef GsFunc<Domain, Range> RepType;

	GsFuncHandle(const GsFuncHandle<Domain, Range>& obj)
	: GsHandleReadOnly< RepType >(obj)
	{
	}

	// FIX -- following two constructors exist to allow gsfunc
	//			reps to be created without handles

	GsFuncHandle(const RepType* obj = 0)
	: GsHandleReadOnly< RepType >( obj )
	{
	}

	explicit GsFuncHandle(const RepType& obj)
	: GsHandleReadOnly< RepType >( &obj )
	{
		if( this->m_rep )
			const_cast<RepType *>( this->m_rep )->incCount();
	}

	virtual ~GsFuncHandle()
	{
	}

	void operator = (const GsFuncHandle<Domain, Range>& f)
	{
		GsHandleReadOnly< RepType >::operator=(f);
	}

	void operator = (const GsFunc<Domain, Range>* f)
	{
		GsHandleReadOnly< RepType >::operator=( GsFuncHandle<Domain, Range>(f) );
	}

	Range operator () ( Domain arg ) const
	{
		return (*this->m_rep)(arg);
	}

	virtual GsFuncHandle< Domain, Range > Integrate( double initialValue ) const {
		return GsFuncHandle< Domain, Range>( this->m_rep->Integrate( initialValue ) );
	}	
	
	///////////////////////
	// Domain Descriptions
	///////////////////////

	typedef typename RepType::const_iterator const_iterator;
	typedef typename RepType::value_type value_type;

	const_iterator nonZeroBegin() const
	{
		return this->m_rep->nonZeroBegin();
	}
	const_iterator nonZeroEnd() const
	{
		return this->m_rep->nonZeroEnd();
	}
	value_type nonZeroBack() const
	{
		return this->m_rep->nonZeroBack();
	}   

	const RepType* getFuncRep() const { return this->m_rep; }
};



template< class Domain1, class Domain2, class Range >
class GsFunc2Handle : public GsHandleReadOnly< GsFunc2<Domain1, Domain2, Range> >
{
public:

	typedef GsDtFunc2RepT< Domain1, Domain2, Range > GsDtType;

	typedef GsFunc2<Domain1, Domain2, Range> RepType;

	GsFunc2Handle(const GsFunc2Handle<Domain1, Domain2, Range>& obj)
	: GsHandleReadOnly< RepType >(obj)
	{
	}

	// FIX -- following two constructors exist to allow gsfunc
	//			reps to be created without handles

	GsFunc2Handle(const RepType* obj = 0)
	: GsHandleReadOnly< RepType >( obj )
	{
	}

	GsFunc2Handle(const RepType& obj)
	: GsHandleReadOnly< RepType >( &obj )
	{
		// FIX
		if (this->m_rep)
		{
			this->m_rep->incCount();
		}
	}

	~GsFunc2Handle()
	{
	}

	void operator = (const RepType& f)
	{
		GsHandleReadOnly< RepType >::operator=( GsFunc2Handle<Domain1, Domain2, Range>(f) );
	}

	void operator = (const GsFunc2Handle<Domain1, Domain2, Range>& f)
	{
		GsHandleReadOnly< RepType >::operator=(f);
	}

	void operator = (const GsFunc2<Domain1, Domain2, Range>* f)
	{
		GsHandleReadOnly< RepType >::operator=( GsFunc2Handle<Domain1, Domain2, Range>(f) );
	}

	Range operator () ( Domain1 arg1, Domain2 arg2 ) const
	{
		return (*this->m_rep)(arg1, arg2);
	}

};


CC_END_NAMESPACE

#endif 
