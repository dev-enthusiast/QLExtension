/****************************************************************
**
**	gsdt/GsDtMatrix.h	- The GsDtMatrix class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtMatrix
**      Base		- Matrix
**      BaseParent	- 
**      HdrName		- gsdt/GsDtMatrix.h
**      FnClass		- GsDtMatrix.cpp
**      Streamable	- 1
**      Iterable	- 1
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtMatrix.h,v 1.6 2001/11/27 22:45:01 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTMATRIX_H )
#define IN_GSDT_GSDTMATRIX_H

#include	<gsdt/GsDt.h>
#include	<gscore/GsMatrix.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtMatrix;


class EXPORT_CLASS_GSDTLITE GsDtMatrix : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtMatrix )
public:
	GsDtMatrix();
	GsDtMatrix( const GsDtMatrix &Obj );
	GsDtMatrix &operator=( const GsDtMatrix &Obj );

	virtual ~GsDtMatrix();

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

protected:
	virtual GsString instanceInfo( GsDt::FormatInfo &Info );

private:
	GsMatrix*	m_data;
public:
	GsDtMatrix( GsMatrix *Mat, GsCopyAction Action );
	GsDtMatrix( const GsMatrix &Mat );
	// FIX-This is a pretty sleazy way of exporting m_data
	const GsMatrix& data() const	{ return *m_data; }
		  GsMatrix& data()			{ return *m_data; }
	// Release ownership of data() and forget about it 
	GsMatrix* stealData()
	{
		GsMatrix* tmp = m_data;
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

