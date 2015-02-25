/****************************************************************
**
**	GSCURRIEDFUNC.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsCurriedFunc.h,v 1.10 2011/05/23 22:46:01 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSCURRIEDFUNC_H )
#define IN_GSFUNC_GSCURRIEDFUNC_H

#include <gscore/base.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsCFunc.h>
#include <gscore/GsCFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

// Exploring the usability of GsHandleReadOnly for curried Functions
// all curried Function classes should inherit from GsHandleReadOnly
// all Functions passed to these therefore should inherit from
// GsRefCount class. This would not have been necessary if GsHandleReadOnly 
// design was slightly different
 

template< class Arg, class Result >
class GsCurriedFunc1 
   : public GsCFuncHandle< Arg, Result >
{
public:

    typedef Arg arg1_t;
    typedef Result result_t;
    typedef GsCFunc< Arg, Result > fun_t;
    typedef GsCurriedFunc1< Arg, Result > self_t;
    
    GsCurriedFunc1( fun_t *func ) 
    : GsCFuncHandle< Arg, Result >( func ), m_func1( *self_t::m_rep ) 
    {
    }

    GsCurriedFunc1( const GsCurriedFunc1 &cfunc )
	: GsCFuncHandle< Arg, Result >( cfunc ), m_func1( *self_t::m_rep ){}
    
    virtual ~GsCurriedFunc1() {}

    self_t *clone()
    {
        self_t *cloneptr = new self_t( this->m_rep );
        return cloneptr;
    }

    inline Result operator() ( const Arg &arg1 ) const { 
        return m_func1( arg1 );
    }

private:
    
    const fun_t &m_func1;

};

template< class Arg1, class Arg2, class Result >
class GsCurriedFunc2 : 
    public GsCFuncHandle2< Arg1, Arg2, Result >
{
public:

    typedef Arg1 arg1_t;
    typedef Arg2 arg2_t;
    typedef Result result_t;
    typedef GsCFunc2< Arg1, Arg2, Result > fun_t;
    typedef GsCurriedFunc2< Arg1, Arg2, Result > self_t;
    typedef GsCurriedFunc1< Arg2, Result > paf_t;
    
    GsCurriedFunc2( fun_t *func ) 
    : GsCFuncHandle2< Arg1, Arg2, Result > ( func ), m_func2( *self_t::m_rep ) 
    { 
    }
    
    GsCurriedFunc2( const GsCurriedFunc2 &cfunc )
	: GsCFuncHandle2< Arg1, Arg2, Result >( cfunc ), m_func2( *self_t::m_rep )
    {
    }
  
    virtual ~GsCurriedFunc2() { }
   
    class  PAF : public GsCFunc< Arg2, Result >
    {
        self_t *m_curryfuncptr;
        const self_t &m_curryfunc2;
        
        const Arg1 m_arg1;
    public:    
        PAF( const self_t &func, const Arg1 &arg1 ) 
        : m_curryfuncptr( func.clone() ), m_curryfunc2( *m_curryfuncptr ), m_arg1( arg1 )
        {
        }
        
        virtual ~PAF() { delete m_curryfuncptr; }
        
        inline Result operator() ( const Arg2 &arg2 ) const
        {
           
           return m_curryfunc2( m_arg1, arg2 );
        }
    };

    self_t *clone() const
    {
        self_t *cloneptr = new self_t( const_cast< fun_t *>( this->m_rep ) );
        return cloneptr;
    }

    Result operator() ( const Arg1 &arg1, const Arg2 &arg2 ) const {
        return m_func2( arg1, arg2 );
    }
    
    paf_t operator() ( const Arg1 &arg1 ) const
    {
        return paf_t( new PAF( *this, arg1 ) );
    }

private:
    
    const fun_t &m_func2;

};

template< class Arg1, class Arg2, class Arg3, class Result >
class GsCurriedFunc3 
	: public GsCFuncHandle3< Arg1, Arg2, Arg3, Result >
{
public:
    
    typedef Arg1 arg1_t;
    typedef Arg2 arg2_t;
    typedef Result result_t;
    typedef GsCFunc3< Arg1, Arg2, Arg3, Result > fun_t;
    typedef GsCurriedFunc3< Arg1, Arg2, Arg3, Result > self_t;
    typedef GsCurriedFunc2< Arg2, Arg3, Result > paf_t;
    
    GsCurriedFunc3( fun_t *func ) : 
        GsCFuncHandle3< Arg1, Arg2, Arg3, Result >( func ), m_func3( *self_t::m_rep ) 
	{
    }
	GsCurriedFunc3( const GsCurriedFunc3 &cfunc )
	: GsCFuncHandle3< Arg1, Arg2, Arg3, Result >( cfunc ), m_func3( *self_t::m_rep ){}
	    
    virtual ~GsCurriedFunc3() {}

    // partial application class with application to 1st argument

    class PAF : public GsCFunc2< Arg2, Arg3, Result >
    {
        self_t *m_curryfuncptr;
        const self_t &m_curryfunc3;
        const Arg1 m_arg1;

    public:
        // need to call func.clone() since we need to maintain a valid handle of func in PAF
        //
        PAF( const self_t &func, const Arg1 &arg1 )
	    : m_curryfuncptr( func.clone() ), m_curryfunc3( *m_curryfuncptr ), m_arg1( arg1 )
        {
        }
   
        virtual ~PAF() 
        {
            delete m_curryfuncptr;
        }

        Result operator() ( const Arg2 &arg2, const Arg3 &arg3 ) const
        {
            return m_curryfunc3( m_arg1, arg2, arg3 );
        }
    }; 

    self_t *clone() const
    {
        self_t *cloneptr = new self_t( const_cast< fun_t *>( this->m_rep ) );
        return cloneptr;
    }

    Result operator() ( const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3 )  const
    {
        return m_func3( arg1, arg2, arg3 );
    }
   
    paf_t operator() ( const Arg1 &arg1 ) const
    {
        return paf_t( new PAF( *this, arg1 ) );
    }

private:
    
    const fun_t &m_func3;
};

CC_END_NAMESPACE


#endif 





