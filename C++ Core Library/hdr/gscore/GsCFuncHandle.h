/****************************************************************
**
**	GsCFuncHandle.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSCFUNCHANDLE_H )
#define IN_GSFUNC_GSCFUNCHANDLE_H

#include <gscore/GsHandle.h>
#include <gscore/GsCFunc.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsFuncHandle.h>

#include <ostream>

CC_BEGIN_NAMESPACE( Gs )

template< class Arg, class Result >
class GsCFuncHandle;

template<class Arg, class Result >
class GsCFuncAdaptor
   : public GsCFunc< Arg, Result >
{
public:
   GsCFuncAdaptor( const GsFuncHandle< Arg, Result > &func ) 
   : m_func( func ) {}
   GsCFuncAdaptor( const GsCFuncAdaptor& x ) 
   : GsCFunc<Arg, Result>( x ), m_func( x.m_func ) {}
   virtual ~GsCFuncAdaptor() { }

   GsCFunc< Arg, Result >* Integrate( double initValue ) const
   { 
       return new GsCFuncAdaptor< Arg, Result >(m_func.Integrate( initValue ));
   }
   Result operator() ( const Arg &arg ) const
   {
      return m_func( arg );
   }
private:
   GsFuncHandle< Arg, Result > m_func;
};

template<class Arg, class Result >
class GsFuncAdaptor
   : public GsFunc< Arg, Result >
{
public:
    GsFuncAdaptor( const GsCFuncHandle< Arg, Result > &func ) 
    : m_func( func ) {}
    GsFuncAdaptor( const GsFuncAdaptor& x ) 
    : GsFunc<Arg,Result>( x ), m_func( x.m_func ) {}
    virtual ~GsFuncAdaptor() { }
    Result operator() ( Arg arg ) const
    {
        return m_func( arg );
    }
    GsFunc< Arg, Result > *Integrate( double initValue ) const
	{
        return new GsFuncAdaptor< Arg, Result >( m_func.Integrate( initValue ) );
    }
        
private:
    GsCFuncHandle< Arg, Result > m_func;
};


template< class Arg, class Result > 
class GsFuncAdaptor_c
   : public GsFunc< const Arg&, Result >
{
public:
    GsFuncAdaptor_c( const GsCFuncHandle< Arg, Result > &func )
    : m_func( func ) {}
    GsFuncAdaptor_c( const GsFuncAdaptor_c &x )
    : GsFunc< const Arg&, Result >( x ), m_func( x.m_func ) {}
    virtual ~GsFuncAdaptor_c() { }
    Result operator() ( const Arg &arg ) const
    {
       return m_func( arg );
    }
    GsFunc< const Arg&, Result > *Integrate( double initValue ) const
	{
        return new GsFuncAdaptor_c< Arg, Result >( m_func.Integrate( initValue ) );
 
    }
 
private:
    GsCFuncHandle< Arg, Result > m_func;
};
 
template< class Arg, class Result >
class GsCFuncHandle 
    :   public GsHandleReadOnly< GsCFunc< Arg, Result > >
{
public:
    typedef GsCFunc< Arg, Result > rep_t;

	typedef Arg arg_t;
	typedef Result result_t;
	typedef GsCFuncHandle< Arg, Result > self_t;

	rep_t* operator->() {
		 return this->m_rep;
	}

	GsCFuncHandle( rep_t *rep ) : GsHandleReadOnly< rep_t >( rep ) {
	}

	explicit GsCFuncHandle(const rep_t& rep)
	: GsHandleReadOnly< rep_t >( &rep )
	{
		if( this->m_rep )
			const_cast<rep_t *>( this->m_rep )->incCount();
	}

	GsCFuncHandle(const rep_t* obj = 0)
	: GsHandleReadOnly< rep_t >( obj )
	{
	}

	GsCFuncHandle(const self_t& obj)
	: GsHandleReadOnly< rep_t >(obj)
	{
	}

    GsCFuncHandle( GsFunc< Arg, Result >* rep )
	: GsHandleReadOnly< rep_t >( new GsCFuncAdaptor< Arg, Result >( rep ) )
    {
    }

    GsCFuncHandle( const GsFuncHandle< Arg, Result > &obj )
    : GsHandleReadOnly< rep_t >( new GsCFuncAdaptor< Arg, Result >( obj ) )
    {
    }

	virtual ~GsCFuncHandle()
	{
	} 

    operator GsFuncHandle< Arg, Result >() const
    {
        return GsFuncHandle< Arg, Result >( new GsFuncAdaptor< Arg, Result >( *this ) );
    }

    GsFuncHandle< const Arg&, Result > constArgGsFuncHandle() const
    {
        return GsFuncHandle< const Arg&, Result > ( new GsFuncAdaptor_c< Arg, Result > ( *this )) ;
    }

    virtual self_t Integrate( double initValue ) const { return self_t( this->m_rep->Integrate( initValue ) ); }

	void operator = (const self_t& f)
	{
		GsHandleReadOnly< rep_t >::operator=(f);
	}

	void operator = (const rep_t *f)
	{
		GsHandleReadOnly< rep_t >::operator=( self_t(f) );
	}

	Result operator () ( Arg arg ) const
	{
        return (*this->m_rep)(arg);
    }

	const rep_t* getRep() const { return this->m_rep; }

};



template< class Arg1, class Arg2, class Result >
class GsCFuncHandle2 : public GsHandleReadOnly< GsCFunc2< Arg1, Arg2, Result > >
{
public:
    typedef GsCFunc2< Arg1, Arg2, Result > rep_t;
	typedef Arg1 arg1_t;
	typedef Arg2 arg2_t;
	typedef Result result_t;
	typedef GsCFuncHandle2< Arg1, Arg2, Result > self_t;

	rep_t* operator->() {
		 return this->m_rep;
	}

	GsCFuncHandle2( rep_t *rep ) : GsHandleReadOnly< rep_t >( rep ) {
	}

	explicit GsCFuncHandle2(const rep_t& rep)
	: GsHandleReadOnly< rep_t >( &rep )
	{
		if( this->m_rep )
			const_cast<rep_t *>( this->m_rep )->incCount();
	}

	GsCFuncHandle2(const rep_t* obj = 0)
	: GsHandleReadOnly< rep_t >( obj )
	{
	}

	GsCFuncHandle2(const self_t& obj)
	: GsHandleReadOnly< rep_t >(obj)
	{
	}

	virtual ~GsCFuncHandle2()
	{
	}

    virtual self_t Integrate( double initValue ) const { return self_t( this->m_rep->Integrate( initValue ) ); }

	void operator = (const self_t& f)
	{
		GsHandleReadOnly< rep_t >::operator=(f);
	}

	void operator = (const rep_t *f)
	{
		GsHandleReadOnly< rep_t >::operator=( self_t(f) );
	}

	Result operator () ( const Arg1 &arg1, const Arg2 &arg2 ) const
	{
        return (*this->m_rep)(arg1, arg2);
   	}

	const rep_t* getRep() const { return this->m_rep; }

};

template< class Arg1, class Arg2, class Arg3, class Result >
class GsCFuncHandle3 : public GsHandleReadOnly< GsCFunc3< Arg1, Arg2, Arg3, Result > >
{
public:
    typedef GsCFunc3< Arg1, Arg2, Arg3, Result > rep_t;

	typedef Arg1 arg1_t;
	typedef Arg2 arg2_t;
	typedef Arg3 arg3_t;
	typedef Result result_t;
	typedef GsCFuncHandle3< Arg1, Arg2, Arg3, Result > self_t;

	rep_t* operator->() {
		 return this->m_rep;
	}

	GsCFuncHandle3( rep_t *rep ) : GsHandleReadOnly< rep_t >( rep ) {
	}

	explicit GsCFuncHandle3(const rep_t& rep)
	: GsHandleReadOnly< rep_t >( &rep )
	{
		if( this->m_rep )
			const_cast<rep_t *>( this->m_rep )->incCount();
	}

	GsCFuncHandle3(const rep_t* obj = 0)
	: GsHandleReadOnly< rep_t >( obj )
	{
	}

	GsCFuncHandle3(const self_t& obj)
	: GsHandleReadOnly< rep_t >(obj)
	{
	}

	virtual ~GsCFuncHandle3()
	{
	}

    virtual self_t Integrate( double initValue ) const { 
        return self_t( this->m_rep->Integrate( initValue ) ); 
    }

	void operator = (const self_t& f)
	{
		GsHandleReadOnly< rep_t >::operator=(f);
	}

	void operator = (const rep_t *f)
	{
		//cout << "Assignment " << std::endl;
		GsHandleReadOnly< rep_t >::operator=( self_t(f) );
	}

	Result operator () ( const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3 ) const
	{
        return (*this->m_rep)(arg1, arg2, arg3);
   	}

	const rep_t* getRep() const { return this->m_rep; }

};



CC_END_NAMESPACE

#endif 

