/****************************************************************
**
**	GsCFunc.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSCFUNC_H )
#define IN_GSFUNC_GSCFUNC_H

#include <gscore/base.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsException.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Arg, class Result > 
class GsCFunc : public GsRefCount
{

public:
	typedef Arg arg_t;
	typedef Result result_t;
	typedef GsCFunc< Arg, Result > cfself_t;

	GsCFunc() {}
	virtual ~GsCFunc() {}
	virtual Result operator () ( const Arg &arg ) const = 0;

	virtual cfself_t* Integrate( double initialValue ) const {
		GSX_THROW(GsXBadIndex, "Attempt to integral over non-integratable GsCFunc");
		return 0;
	}
};


template< class Arg1, class Arg2, class Result > 
class GsCFunc2 : public GsRefCount
{
public:
	typedef Arg1 arg1_t;
	typedef Arg2 arg2_t;
	typedef Result result_t;
	typedef GsCFunc2< Arg1, Arg2, Result > cfself_t;

	GsCFunc2() {}
	virtual ~GsCFunc2() {}
	virtual Result operator () ( const Arg1 &arg1, const Arg2 &arg2 ) const = 0;
	virtual cfself_t* Integrate( double initialValue ) const {
		GSX_THROW(GsXBadIndex, "Attempt to integral over non-integratable GsCFunc");
		return 0;
	}
};

template< class Arg1, class Arg2, class Arg3, class Result > 
class GsCFunc3 : public GsRefCount
{
public:
	typedef Arg1 arg1_t;
	typedef Arg2 arg2_t;
	typedef Arg3 arg3_t;
	typedef Result result_t;
	typedef GsCFunc3< Arg1, Arg2, Arg3, Result > cfself_t;

	GsCFunc3() {}
	virtual ~GsCFunc3() {}
	virtual Result operator () ( const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3 ) const = 0;
	virtual cfself_t* Integrate( double initialValue ) const {
		GSX_THROW(GsXBadIndex, "Attempt to integral over non-integratable GsCFunc");
		return 0;
	}
};



CC_END_NAMESPACE

#endif

