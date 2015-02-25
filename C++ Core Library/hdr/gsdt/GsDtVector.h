/****************************************************************
**
**	gsdt/GsDtVector.h	- The GsDtVector class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtVector
**      Base		- Vector
**      BaseParent	- 
**      HdrName		- gsdt/GsDtVector.h
**      FnClass		- GsDtVector.cpp
**      Streamable	- 1
**      Iterable	- 1
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtVector.h,v 1.7 2001/11/27 22:45:03 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTVECTOR_H )
#define IN_GSDT_GSDTVECTOR_H

#include	<gsdt/GsDt.h>
#include	<gscore/GsVector.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtVector;


class EXPORT_CLASS_GSDTLITE GsDtVector : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtVector )
public:
	GsDtVector();
	GsDtVector( const GsDtVector &Obj );
	GsDtVector &operator=( const GsDtVector &Obj );

	virtual ~GsDtVector();

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

	virtual GsBool		Truth() const;
	virtual GsBool		IsValid() const;

	virtual GsStatus	Increment();
	virtual GsStatus	Decrement();
	virtual GsDt		*Negate() const;

	virtual GsString	toString() const;

	virtual GsDt		*MathOperateOnAll( GsFuncHandle<double, double> Func ) const;

	virtual GsDt		*MathBinOpOnAll(   GsFunc2Handle<double, double, double> Func, const GsDt &Rhs ) const;

	virtual GsDt		*IfOpOnAll( const GsDt &TrueVal, const GsDt &FalseVal ) const;

protected:
	virtual GsString instanceInfo( GsDt::FormatInfo &Info );

private:
	GsVector*	m_data;
public:
	GsDtVector( GsVector *Vec, GsCopyAction Action );
	GsDtVector( const GsVector &Vec );
	GsDtVector( size_t Size, const double *InitVal = NULL );

	// FIX-This is a pretty sleazy way of exporting m_data
	const GsVector& data() const	{ return *m_data; }
	      GsVector& data()			{ return *m_data; }
	// Release ownership of data() and forget about it 
	GsVector* stealData()
	{
		GsVector* tmp = m_data;
		m_data = NULL;
		return tmp;
	}
	class Iterator;
	friend class Iterator;

	virtual GsDt::Iterator*	Subscript( const GsDt &Index );
	virtual GsDt::Iterator*	SubscriptFirst( IterationFlag Flag );
	virtual GsStatus		SubscriptReplace( const GsDt &Index, GsDt *NewValue, GsCopyAction Action );
	virtual GsStatus		SubscriptDelete( const GsDt &Index );
	virtual GsStatus		SubscriptAppend( GsDt *NewValue, GsCopyAction Action );

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

