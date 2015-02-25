/****************************************************************
**
**	GsFunctor.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_GSFUNCTOR_H )
#define IN_GSFUNCTOR_H

#include <gscore/base.h>
#include <gscore/GsCFunc.h>
#include <gscore/GsCurriedFunc.h>

CC_BEGIN_NAMESPACE( Gs )

// .... FIX: Get rid of GsFuncNoParams name use only GsFuncGlobal below ....
template< class Arg, class Result > 
class GsFunctor : public GsCFunc< Arg, Result >
{
public:
    GsFunctor(Result (*f)(const Arg&)) : m_f(f) {}
	
    virtual ~GsFunctor() {}

    Result operator () ( const Arg &arg ) const { return (*m_f)(arg); }

    typedef GsFunctor< Arg, Result > self_t;
    typedef GsCurriedFunc1< Arg, Result > curry_t;

    curry_t curry() { return curry_t( this ); }
private:
    Result (*m_f)(const Arg&);
};

template< class Arg1, class Arg2, class Result > 
class GsFunctor2 : public GsCFunc2< Arg1, Arg2, Result >
{
public:
    GsFunctor2(Result (*f)(const Arg1&, const Arg2&)) : m_f(f) {}
    virtual ~GsFunctor2() {}
	
    Result operator () ( const Arg1 &arg1, const Arg2 &arg2 ) const { return (*m_f)(arg1, arg2); }

    typedef GsFunctor2< Arg1, Arg2, Result > self_t;
    typedef GsCurriedFunc2< Arg1, Arg2, Result > curry_t;

    curry_t curry() { return curry_t( this ); }
private:
    Result (*m_f)(const Arg1&, const Arg2&);
};

template< class Arg1, class Arg2, class Arg3, class Result > 
class GsFunctor3 : public GsCFunc3< Arg1, Arg2, Arg3, Result >
{
public:
    GsFunctor3(Result (*f)(const Arg1&, const Arg2&, const Arg3&)) : m_f(f) {}
    virtual ~GsFunctor3() {}

    Result operator () ( const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3 ) const { return (*m_f)(arg1, arg2, arg3); }

    typedef GsFunctor3< Arg1, Arg2, Arg3, Result > self_t;
    typedef GsCurriedFunc3< Arg1, Arg2, Arg3, Result > curry_t;

    curry_t curry() { return curry_t( this ); }

private:
    Result (*m_f)(const Arg1&, const Arg2&, const Arg3&);
};

CC_END_NAMESPACE

#endif 

