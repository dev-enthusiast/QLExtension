/****************************************************************
**
**	gsdt/GsDtDouble.h	- The GsDtDouble class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtDouble
**      Base		- Double
**      BaseParent	- 
**      HdrName		- gsdt/GsDtDouble.h
**      FnClass		- GsDtDouble.cpp
**      Streamable	- 1
**      Iterable	- 0
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtDouble.h,v 1.4 2001/08/30 18:42:54 walkerpa Exp $
**
****************************************************************/

#ifndef IN_GSDT_GSDTDOUBLE_H
#define IN_GSDT_GSDTDOUBLE_H

#include	<gsdt/GsDt.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtDouble;


class EXPORT_CLASS_GSDTLITE GsDtDouble : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtDouble )
public:
	GsDtDouble();
	GsDtDouble( const GsDtDouble &Obj );
	GsDtDouble &operator=( const GsDtDouble &Obj );

	virtual ~GsDtDouble();

	virtual GsDt*		highLimit() const;
	virtual GsDt*		lowLimit() const;
	virtual GsDt*		errorSentinel() const;

	virtual GsDt*		CopyDeep() const;
	virtual GsDt*		CopyShallow() const;

	virtual size_t		GetSize() const;
	virtual size_t		GetCapacity() const;
	virtual size_t		GetMemUsage() const;
	virtual void		GetInfo( FcDataTypeInstanceInfo &Info ) const;

	virtual void		StreamStore( GsStreamOut &Stream ) const;
	virtual void		StreamRead(  GsStreamIn  &Stream );

	virtual FcHashCode	HashComplete() const;
	virtual FcHashCode  HashQuick() const;

	virtual GsStatus	BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );
	virtual int			CompareSame( const GsDt &Other ) const;

	virtual bool		Truth() const;
	virtual bool		IsValid() const;

	virtual GsStatus	Increment();
	virtual GsStatus	Decrement();
	virtual GsDt		*Negate() const;

	virtual GsString	toString() const;
	virtual bool        toXmlString( CC_NS( std, ostream ) & ) const;

	virtual GsDt		*MathOperateOnAll( GsFuncHandle<double, double> Func ) const;

	virtual GsDt		*MathBinOpOnAll(   GsFunc2Handle<double, double, double> Func, const GsDt &Rhs ) const;

private:
	double	m_data;
public:
	GsDtDouble( double Dbl );
	double	Dbl() const				{ return m_data; }
	void	DblSet( double Dbl )	{ m_data = Dbl; }
	      double &data()       { return m_data; }
	const double &data() const { return m_data; }

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

