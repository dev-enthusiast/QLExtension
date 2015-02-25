/* $Header: /home/cvs/src/gsfunc/src/gscore/GsFuncNoParams.h,v 1.7 2001/05/03 23:06:50 vengrd Exp $ */
/****************************************************************
**
**	GSFUNCNOPARAMS.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncNoParams.h,v 1.7 2001/05/03 23:06:50 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCNOPARAMS_H )
#define IN_GSFUNCNOPARAMS_H

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

// .... FIX: Get rid of GsFuncNoParams name use only GsFuncGlobal below ....
template< class Domain, class Range > 
class GsFuncNoParams : public GsFunc< Domain, Range>
{
public:
	GsFuncNoParams(Range (*f)(Domain)) : m_f(f) {}
	virtual ~GsFuncNoParams() {}

	Range operator () ( Domain Arg ) const { return (*m_f)(Arg); }

private:
	Range (*m_f)(Domain);
};

template< class Domain, class Range > 
class GsFuncGlobal : public GsFuncNoParams< Domain, Range >
{
public:
	GsFuncGlobal(Range (*f)(Domain)) : 
	GsFuncNoParams< Domain, Range >( f ) {}
	virtual ~GsFuncGlobal() {}
};

template< class Domain1, class Domain2, class Range > 
class GsFunc2NoParams : public GsFunc2< Domain1, Domain2, Range>
{
public:
	GsFunc2NoParams(Range (*f)(Domain1, Domain2)) : m_f(f) {}
	virtual ~GsFunc2NoParams() {}

	Range operator () ( Domain1 Arg1, Domain2 Arg2 ) const
	{ return (*m_f)(Arg1, Arg2); }

private:
	Range (*m_f)(Domain1, Domain2);
};


CC_END_NAMESPACE

#endif 

