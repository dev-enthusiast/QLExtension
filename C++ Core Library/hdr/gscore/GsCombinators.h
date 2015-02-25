/****************************************************************
**
**	GsCombinators.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSCOMBINATORS_H )
#define IN_GSFUNC_GSCOMBINATORS_H

#include <gscore/base.h>
#include <gscore/GsCFunc.h>
#include <gscore/GsCurriedFunc.h>
#include <ccstl.h>
#include <math.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Func, class Arg, class Result >
class GsExpCombinator
	: public GsCFunc< Func, GsCFuncHandle< Arg, Result > >
{
public:
    class Function
    : public GsCFunc< Arg, Result >
    {
    public:
        Function( const Func &func )
        : m_func( func ) {}
        Result operator()( const Arg &arg ) const
        {
            return exp( m_func( arg ) );
        }
        private:
           Func m_func;
    };

    GsCFuncHandle< Arg, Result > operator() ( const Func & func ) const
    {
        return new GsExpCombinator< Func, Arg, Result >::Function( func );
    }

    static GsExpCombinator< Func, Arg, Result > *getInstance()
    {
        if( s_instance == NULL )
        {
            s_instance = new GsExpCombinator< Func, Arg, Result >;
            s_instance->incCount(); // don't delete it.
        }
        return s_instance;
    }

    typedef GsCurriedFunc1< Func, GsCFuncHandle< Arg, Result > > combinator_t;
    static combinator_t curry() { return combinator_t( getInstance() ); }
private:
    static GsExpCombinator< Func, Arg, Result > *s_instance;
};

template< class Func, class Arg, class Result >
GsExpCombinator< Func, Arg, Result > *
GsExpCombinator< Func, Arg, Result >::s_instance = NULL;

template< class Func1, class Func2, class Arg, class Result >
class GsComposeCombinator
    : public GsCFunc2< Func1, Func2, GsCFuncHandle< Arg, Result > >
{
public:
    class Function 
    : public GsCFunc< Arg, Result >
    {
    public:
        Function( const Func1 &func1, const Func2 &func2 ) 
        : m_func1( func1 ),
          m_func2( func2 ) {}
        Result operator()( const Arg& arg ) const
	    {
			return( m_func1( m_func2 ( arg ) ) );
        }
    private:
        Func1 m_func1;
        Func2 m_func2;

    };

    GsCFuncHandle< Arg, Result > operator()( const Func1 &func1, const Func2 &func2 ) const
	{
         return new GsComposeCombinator< Func1, Func2, Arg, Result >::Function( func1, func2 );
	}
    
    static GsComposeCombinator< Func1, Func2, Arg, Result > *getInstance()
    {
        if( s_instance == NULL )
        {
            s_instance = new GsComposeCombinator< Func1, Func2, Arg, Result >;
            s_instance->incCount(); // don't delete it.
        }
        return s_instance;
    }

    typedef GsCurriedFunc2< Func1, Func2, GsCFuncHandle< Arg, Result > > combinator_t;
    static combinator_t curry() { return combinator_t( getInstance() ); }
private:
    static GsComposeCombinator< Func1, Func2, Arg, Result > *s_instance;
};

template< class Func1, class Func2, class Arg, class Result >
GsComposeCombinator< Func1, Func2, Arg, Result > *
GsComposeCombinator< Func1, Func2, Arg, Result >::s_instance=NULL;

template< class Func, class Arg1, class Arg2, class Result >
class GsSwitchCombinator
    : public GsCFunc< Func, GsCFuncHandle2< Arg1, Arg2, Result > >
{
public:
    class Function
    : GsCFunc2< Arg1, Arg2, Result >
    {
    public:
        Function( const Func &func )
        : m_func( func ) {}
        Result operator()( const Arg1 &arg1, const Arg2 &arg2 ) const
	    {
            return m_func( arg2, arg1 );
	    }
    private:
        Func m_func;
   
    };
 
    GsCFuncHandle2< Arg1, Arg2, Result > operator() ( const Func& func ) const
    {
        return new GsSwitchCombinator< Func, Arg1, Arg2, Result >::Function( func );
    }

    static GsSwitchCombinator< Func, Arg1, Arg2, Result > *getInstance()
    {
        if( s_instance == NULL )
        {
            s_instance = new GsSwitchCombinator< Func, Arg1, Arg2, Result >;
            s_instance->incCount(); // don't delete it.
        }
        return s_instance;
    }

    typedef GsCurriedFunc2< Func, Arg1, GsCFuncHandle< Arg2, Result > > combinator_t;
    static combinator_t curry() { return combinator_t( getInstance() ); }
private:
    static GsSwitchCombinator< Func, Arg1, Arg2, Result > *s_instance;
};

template< class Func, class Arg1, class Arg2, class Result >
GsSwitchCombinator< Func, Arg1, Arg2, Result > *
GsSwitchCombinator< Func, Arg1, Arg2, Result >::s_instance = NULL;

template< class Func, class FuncIntegrator >
class GsIntegralCombinator
    : public GsCFunc< Func, FuncIntegrator >
{
public:
    FuncIntegrator operator() ( const Func &func ) const
    {
		// need to make this general by taking out 0.0 later
        return func.Integrate( 0.0 );
    }

    static GsIntegralCombinator< Func, FuncIntegrator > *getInstance()
    {
        if( s_instance == NULL )
        {
            s_instance = new GsIntegralCombinator< Func, FuncIntegrator >;
            s_instance->incCount(); // don't delete it.
        }
        return s_instance;
    }

    typedef GsCurriedFunc1< Func, FuncIntegrator > combinator_t;
    static combinator_t curry() { return combinator_t( getInstance() ); }
private:
    static GsIntegralCombinator< Func, FuncIntegrator > *s_instance;
};

template< class Func, class FuncIntegrator >
GsIntegralCombinator< Func, FuncIntegrator > *
GsIntegralCombinator< Func, FuncIntegrator >::s_instance = NULL;

template< class Arg, class Result >
class GsCFuncAddCombinator
    : public GsCFunc2< GsCFuncHandle< Arg, Result >, GsCFuncHandle< Arg, Result >, GsCFuncHandle< Arg, Result > >
{
public:
    class Function
    : public GsCFunc< Arg, Result >
    {
    public:
        Function( const GsCFuncHandle< Arg, Result >& func1, const GsCFuncHandle< Arg, Result >& func2 )
        : m_func1( func1 ),
          m_func2( func2 )
        {
        }
        Result operator()( const Arg &arg ) const
        {
            return m_func1(arg) + m_func2(arg);
        }
        GsCFunc< Arg, Result > *Integrate( double initValue ) const
        {
            return new Function( m_func1.Integrate( initValue ), m_func2.Integrate( initValue ) );
        }
    private:
        GsCFuncHandle< Arg, Result > m_func1, m_func2;

    };

    GsCFuncHandle< Arg, Result > operator() ( 
        const GsCFuncHandle< Arg, Result > &func1,
        const GsCFuncHandle< Arg, Result > &func2
    ) const
    {
        return new GsCFuncAddCombinator< Arg, Result >::Function( func1, func2 );
    }

    static GsCFuncAddCombinator< Arg, Result > *getInstance()
    {
        if( s_instance == NULL )
        {
            s_instance = new GsCFuncAddCombinator< Arg, Result >;
            s_instance->incCount(); // don't delete it.
        }
        return s_instance;
    }

    typedef GsCurriedFunc2< GsCFuncHandle< Arg, Result >, GsCFuncHandle< Arg, Result >, GsCFuncHandle< Arg, Result > > combinator_t;
    static combinator_t curry() { return combinator_t( getInstance() ); }
private:
    static GsCFuncAddCombinator< Arg, Result > *s_instance;
};

template< class Arg, class Result >
GsCFuncAddCombinator< Arg, Result > *
GsCFuncAddCombinator< Arg, Result >::s_instance = NULL;

template< class Arg, class Result >
class GsCFuncMultiplyCombinator
    : public GsCFunc2< GsCFuncHandle< Arg, Result >, GsCFuncHandle< Arg, Result >, GsCFuncHandle< Arg, Result > >
{
public:
    class Function
    : public GsCFunc< Arg, Result >
    {
    public:
        Function( const GsCFuncHandle< Arg, Result >& func1, const GsCFuncHandle< Arg, Result >& func2 )
        : m_func1( func1 ),
          m_func2( func2 )
        {
        }
        Result operator()( const Arg &arg ) const
        {
            return m_func1(arg)*m_func2(arg);
        }
    private:
        GsCFuncHandle< Arg, Result > m_func1, m_func2;
    };

    GsCFuncHandle< Arg, Result > operator() ( 
        const GsCFuncHandle< Arg, Result > &func1,
        const GsCFuncHandle< Arg, Result > &func2
    ) const
    {
        return new GsCFuncMultiplyCombinator< Arg, Result >::Function( func1, func2 );
    }

    static GsCFuncMultiplyCombinator< Arg, Result > *getInstance()
    {
        if( s_instance == NULL )
        {
            s_instance = new GsCFuncMultiplyCombinator< Arg, Result >;
            s_instance->incCount(); // don't delete it.
        }
        return s_instance;
    }

    typedef GsCurriedFunc2< GsCFuncHandle< Arg, Result >, GsCFuncHandle< Arg, Result >, GsCFuncHandle< Arg, Result > > combinator_t;
 
    static combinator_t curry() { return combinator_t( getInstance() ); }
private:
    static GsCFuncMultiplyCombinator< Arg, Result > *s_instance;
};

template< class Arg, class Result >
GsCFuncMultiplyCombinator< Arg, Result > *
GsCFuncMultiplyCombinator< Arg, Result >::s_instance = NULL;
 
template< class Func, class Arg, class Result >
class GsApplyCombinator
    : public GsCFunc< Func, GsCFuncHandle< Arg, Result > >
{
public:
    class Function
    : public GsCFunc<  Arg, Result >
    {
    public:
        Function( const Func &func )
        : m_func( func ) 
		{
        }
        Result operator()( const Arg &arg ) const
        {
           return m_func( arg );
        }
    private:
        Func m_func;
    };

    GsCFuncHandle< Arg, Result > operator()( const Func& f ) const
    {
        return new GsApplyCombinator< Func, Arg, Result >::Function( f );
    }

    static GsApplyCombinator< Func, Arg, Result > *getInstance()
    {
        if( s_instance == NULL )
        {
            s_instance = new GsApplyCombinator< Func, Arg, Result >;
            s_instance->incCount(); // don't delete it.
        }
        return s_instance;
    }

    typedef GsCurriedFunc1< Func, GsCFuncHandle< Arg, Result > > combinator_t;
    static combinator_t curry() { return combinator_t( getInstance() ); }
private:
    static GsApplyCombinator< Func, Arg, Result > *s_instance;
};

template< class Func, class Arg, class Result >
GsApplyCombinator< Func, Arg, Result > *
GsApplyCombinator< Func, Arg, Result >::s_instance = NULL;

template< class FuncVec, class Arg, class ResultVec >
class GsFuncMapCombinator
    : public GsCFunc< FuncVec, GsCFuncHandle< Arg, ResultVec > >
{
public:
    class Function
    : public GsCFunc< Arg, ResultVec >
    {
    public:
        Function( const FuncVec &fVec )
        : m_funcVec( fVec )
        {
        }
	    ResultVec operator()( const Arg& arg ) const
        {
            ResultVec resultVec( m_funcVec.size() );
            for( unsigned i=0; i<m_funcVec.size(); i++ )
            {
                resultVec[i] = m_funcVec[i]( arg );
            }
            return resultVec;
	    }
    private:
        FuncVec m_funcVec;
    };

    GsCFuncHandle< Arg, ResultVec > operator() ( const FuncVec &fVec ) const 
    {
        return new GsFuncMapCombinator< FuncVec, Arg, ResultVec >::Function( fVec );
    }

    static GsFuncMapCombinator< FuncVec, Arg, ResultVec > *getInstance()
    {
        if( s_instance == NULL )
        {
            s_instance = new GsFuncMapCombinator< FuncVec, Arg, ResultVec >;
            s_instance->incCount(); // don't delete it.
        }
        return s_instance;
    }

    typedef GsCurriedFunc1< FuncVec, GsCFuncHandle< Arg, ResultVec > > combinator_t;
    static combinator_t curry() { return combinator_t( getInstance() ); }
private:
    static GsFuncMapCombinator< FuncVec, Arg, ResultVec > *s_instance;
};

template< class FuncVec, class Arg, class ResultVec >
GsFuncMapCombinator< FuncVec, Arg, ResultVec > *
GsFuncMapCombinator< FuncVec, Arg, ResultVec >::s_instance = NULL;

CC_END_NAMESPACE

#endif 


