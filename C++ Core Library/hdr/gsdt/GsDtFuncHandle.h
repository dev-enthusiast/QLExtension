/****************************************************************
**
**	GSDTFUNCHANDLE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtFuncHandle.h,v 1.12 2001/11/27 22:43:58 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTFUNCHANDLE_H )
#define IN_GSDT_GSDTFUNCHANDLE_H

#include <gsdt/base.h>
#include <gscore/GsFuncHandle.h>
#include <gsdt/GsType.h>
#include <gsdt/GsDtFunc.h>
#include <gsdt/GsDtTypeMap.h>

CC_BEGIN_NAMESPACE( Gs )



/****************************************************************
** Class	   : GsDtFuncRepT 
** Description : A function at the GsDt level that is just 
**				 a wrapper for a function at the GsFuncHandle< D, R >
**				 level.
****************************************************************/

template< class Domain, class Range >
class GsDtFuncRepT : public GsDtFunc 
{
public:
	typedef typename GsDtTypeMap<Domain>::GsDtType DomainGsDtType;
	typedef typename GsDtTypeMap<Range>::GsDtType  RangeGsDtType;

	typedef GsDtFuncRepT< Domain, Range > OwnType;

	GSDT_DECLARE_CASTS( OwnType );

private:

	static const char *GsTypeName;

public:

	static GsType &staticType();

	GsDtFuncRepT();
	GsDtFuncRepT( GsFuncHandle< Domain, Range > FuncHandle );
	GsDtFuncRepT( const GsDtFuncRepT< Domain, Range > & );
	GsDtFuncRepT( GsFuncHandle< Domain, Range > *FuncHandle, GsCopyAction Action );

	~GsDtFuncRepT();

	GsDtFuncRepT &operator=( const GsDtFuncRepT< Domain, Range > &rhs );

	virtual GsDt* operator()( const GsDt * ) const;

	GsFuncHandle< Domain, Range > FuncHandle() const { return data(); }

	const GsFuncHandle< Domain, Range > &data() const { return *m_FuncHandle; }

	GsFuncHandle< Domain, Range > *stealData() { GsFuncHandle< Domain, Range > *FuncHandle = m_FuncHandle; m_FuncHandle = NULL; return FuncHandle; }

	// Virtual functions from the GsDt world.

	virtual GsDt		*CopyDeep() 	const;
	virtual GsDt		*CopyShallow() 	const;

	virtual const GsType& type() const { return staticType(); }

	virtual bool isKindOf( const GsType& Type ) const;

	// DT_MSG_TO/FROM_STREAM
  	virtual void		StreamStore( GsStreamOut &Stream ) const;
  	virtual void		StreamRead(  GsStreamIn  &Stream );

private:
		
	GsFuncHandle< Domain, Range >
			*m_FuncHandle;
};



/****************************************************************
** Class	   : GsDtFunc2RepT 
** Description : A function at the GsDt level that is just 
**				 a wrapper for a function at the GsFuncHandle< D, R >
**				 level.
****************************************************************/

template< class Domain1, class Domain2, class Range >
class GsDtFunc2RepT : public GsDtFunc2 
{
public:
	typedef typename GsDtTypeMap<Domain1>::GsDtType Domain1GsDtType;
	typedef typename GsDtTypeMap<Domain2>::GsDtType Domain2GsDtType;
	typedef typename GsDtTypeMap<Range>::GsDtType   RangeGsDtType;

	typedef GsDtFunc2RepT< Domain1, Domain2, Range > OwnType;

	GSDT_DECLARE_CASTS( OwnType );

private:

	static const char *GsTypeName;

public:

	static GsType &staticType();

	GsDtFunc2RepT();
	GsDtFunc2RepT( GsFunc2Handle< Domain1, Domain2, Range > FuncHandle );
	GsDtFunc2RepT( const GsDtFunc2RepT< Domain1, Domain2, Range > & );
	GsDtFunc2RepT( GsFunc2Handle< Domain1, Domain2, Range > *FuncHandle, GsCopyAction Action );

	~GsDtFunc2RepT();

	GsDtFunc2RepT &operator=( const GsDtFunc2RepT< Domain1, Domain2, Range > &rhs );

	GsDt* operator()( const GsDt *, const GsDt * ) const;

	GsFunc2Handle< Domain1, Domain2, Range > FuncHandle() const { return data(); }

	const GsFunc2Handle< Domain1, Domain2, Range > &data() const { return *m_Func2Handle; }

	GsFunc2Handle< Domain1, Domain2, Range > *stealData() 
	{ 
		GsFunc2Handle< Domain1, Domain2, Range > *FuncHandle = m_Func2Handle; 
		m_Func2Handle = NULL; 
		return FuncHandle; 
	}

	// Virtual functions from the GsDt world.

	virtual GsDt *CopyDeep() const;

	virtual const GsType& type() const { return staticType(); }

	virtual bool isKindOf( const GsType& Type ) const;

private:

	GsFunc2Handle< Domain1, Domain2, Range >
			*m_Func2Handle;
};


CC_END_NAMESPACE

#endif 
