/****************************************************************
**
**	GSFUNCSLANG.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/slang_addins/src/slang_addins/GsFuncSlang.h,v 1.5 2012/07/26 17:46:16 e19351 Exp $
**
****************************************************************/

#ifndef IN_SLANG_ADDINS_GSFUNCSLANG_H
#define IN_SLANG_ADDINS_GSFUNCSLANG_H

#include <slang_addins/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsException.h>
#include <datatype.h>
#include <dtcore.h>						// for DT_VECTOR
#include <secexpr.h>					// for DT_SLANG
#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_SLANG_ADDINS GsSlangFuncAndContext
{
public:
	typedef GsDtGeneric<GsSlangFuncAndContext> GsDtType;

	GsSlangFuncAndContext();
	GsSlangFuncAndContext( GsSlangFuncAndContext const& src )
	{
		DTM_ASSIGN( &m_slang_func, const_cast<DT_VALUE*>( &src.m_slang_func ) ); // grrrr
		DTM_ASSIGN( &m_context, const_cast<DT_VALUE*>( &src.m_context ) ); // grrrr
	}
	virtual ~GsSlangFuncAndContext()
	{
		DTM_FREE( &m_slang_func );
		DTM_FREE( &m_context );
	}

	void operator=( GsSlangFuncAndContext const& src )
	{
		DTM_FREE( &m_slang_func );
		DTM_FREE( &m_context );
		DTM_ASSIGN( &m_slang_func, const_cast<DT_VALUE*>( &src.m_slang_func ) ); // grrr
		DTM_ASSIGN( &m_context, const_cast<DT_VALUE*>( &src.m_context ) ); // grrr
	}

	static CC_NS(std,string) typeName() { return "GsSlangFuncAndContext"; }
	CC_NS(std,string) toString();

/****************************************************************
**	Adlib Name: GsSlangFuncAndContext::setFunc
**	Returns: 
**	Action : 
****************************************************************/

	void setFunc(
		const dt_slang_addin_arg& func			// slang function
	)
	{
		DT_SLANG* slang = (DT_SLANG*) func.m_value->Data.Pointer;
		if( slang->Expression->Argc - 1 > 2 )
			GSX_THROW( GsXInvalidArgument, "GsSlangFuncAndContext::setFunc(): function has more than two args." );

		DTM_FREE( &m_slang_func );
		if( !DTM_ASSIGN( &m_slang_func, const_cast<DT_VALUE*>( func.m_value ) ) ) // grrrrr
			DTM_INIT( &m_slang_func );
	}

/****************************************************************
**	Adlib Name: GsSlangFuncAndContext::getFunc
**	Returns: 
**	Action : 
****************************************************************/

	dt_slang_addin_arg* getFunc(
	)
	{
		return new dt_slang_addin_arg( &m_slang_func );
	}

/****************************************************************
**	Adlib Name: GsSlangFuncAndContext::setContext
**	Returns: 
**	Action : 
****************************************************************/

	void setContext(
		const dt_value_addin_arg& val				// context value
	)
	{
		DTM_FREE( &m_context );
		if( !DTM_ASSIGN( &m_context, const_cast<DT_VALUE*>( val.m_value ) ) )	// grrr
			DTM_INIT( &m_context );
	}

/****************************************************************
**	Adlib Name: GsSlangFuncAndContext::getContext
**	Returns: 
**	Action : 
****************************************************************/

	dt_value_addin_arg* getContext(
	)
	{
		return new dt_value_addin_arg( &m_context );
	}

	void call_slang_func( DT_VALUE* arg, DT_VALUE* res ) const;

protected:
	DT_VALUE m_slang_func;
	DT_VALUE m_context;

};

inline

/****************************************************************
**	Adlib Name: GsSlangFuncAndContext::GsSlangFuncAndContext
**	Returns: 
**	Action : 
****************************************************************/

GsSlangFuncAndContext::GsSlangFuncAndContext()
{
	DTM_INIT( &m_slang_func );
	DTM_INIT( &m_context );
}

template< class Domain, class Range >
class GsFuncSlangBase : public GsFunc< Domain, Range >
{
public:
	GsFuncSlangBase() {}
	GsFuncSlangBase( GsSlangFuncAndContext const& rep ) : m_rep( rep )
	{}
	virtual ~GsFuncSlangBase() {}

	virtual Range operator()( Domain Arg ) const
	{
		GSX_THROW( GsXNotImplemented, "You must use one of the defined specializations, like GsFuncSlangVectorDouble" );
		return Range();
	}

	void setRep( GsSlangFuncAndContext const& rep ) { m_rep = rep; }
	GsSlangFuncAndContext& getRep() { return m_rep; }

protected:
	GsSlangFuncAndContext m_rep;
};

template< class Domain, class Range >
class GsFuncSlang : public GsFuncSlangBase< Domain, Range >
{
public:
	GsFuncSlang() {}
	virtual ~GsFuncSlang() {}
};

CC_END_NAMESPACE

#endif 
