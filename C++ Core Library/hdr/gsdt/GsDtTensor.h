/****************************************************************
**
**	gsdt/GsDtTensor.h	- The GsDtTensor class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtTensor
**      Base		- Tensor
**      BaseParent	- 
**      HdrName		- gsdt/GsDtTensor.h
**      FnClass		- GsDtTensor.cpp
**      Streamable	- 1
**      Iterable	- 1
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtTensor.h,v 1.7 2001/11/27 22:45:02 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTTensor_H )
#define IN_GSDT_GSDTTensor_H

#include	<gsdt/GsDt.h>
#include	<gscore/GsTensor.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtTensor;


class EXPORT_CLASS_GSDTLITE GsDtTensor : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtTensor )
public:
	GsDtTensor();
	GsDtTensor( const GsDtTensor &Obj );
	GsDtTensor &operator=( const GsDtTensor &Obj );

	virtual ~GsDtTensor();

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
	GsTensor*	m_data;

	GsString 	formatItems( GsString prefix, int level, GsTensor::index &index ) const;

public:
	GsDtTensor( GsTensor *Mat, GsCopyAction Action );
	GsDtTensor( const GsTensor &Mat );
	// FIX-This is a pretty sleazy way of exporting m_data, but not quite as bad as GsDtMatrix.
	const GsTensor& data() const	{ if( !m_data ) GSX_THROW( GsXInvalidOperation, "GsDtTensor is empty." ); return *m_data; }
		  GsTensor& data()			{ if( !m_data ) GSX_THROW( GsXInvalidOperation, "GsDtTensor is empty." ); return *m_data; }
	// Release ownership of data() and forget about it 
	GsTensor* stealData()
	{
		GsTensor* tmp = m_data;
		m_data = NULL;
		return tmp;
	}
	
	const GsTensor::index &dim() const { return data().dim(); }

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

