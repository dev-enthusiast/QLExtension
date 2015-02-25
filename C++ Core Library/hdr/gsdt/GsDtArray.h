/****************************************************************
**
**	gsdt/GsDtArray.h	- The GsDtArray class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtArray
**      Base		- Array
**      BaseParent	- 
**      HdrName		- gsdt/GsDtArray.h
**      FnClass		- GsDtArray.cpp
**      Streamable	- 1
**      Iterable	- 1
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtArray.h,v 1.7 2001/11/27 22:44:59 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTARRAY_H )
#define IN_GSDT_GSDTARRAY_H

#include	<gsdt/GsDt.h>
#include    <gsdt/FormatInfo.h>


CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtArray;


class EXPORT_CLASS_GSDTLITE GsDtArray : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtArray )
public:
	GsDtArray();
	GsDtArray( const GsDtArray &Obj );
	GsDtArray &operator=( const GsDtArray &Obj );

	virtual ~GsDtArray();

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

private:
	size_t		m_Size,
				m_Capacity;

	GsDt**		m_Data;

public:
	size_t		size() const		{ return m_Size; }
	size_t		capacity() const	{ return m_Capacity; }

	const GsDt*	elem( size_t Index ) const { return m_Data[ Index ]; }
	GsDt*		elem( size_t Index )		{ return m_Data[ Index ]; }
	GsStatus	elemSet( size_t Index, const GsDt *elem )
	{
		return elemSet( Index, const_cast<GsDt*>( elem ), GS_COPY );
	}
	GsStatus	elemSet( size_t Index, GsDt *elem, GsCopyAction Action );

	GsDt**		asCArray() { return m_Data; }

protected:
	size_t		capacityFromSize( size_t NewSize ) const;
public:
	GsDtArray( size_t Size );
	GsStatus	resize( size_t NewSize, GsDt *FillValue = NULL );
	GsStatus	reserve( size_t NewCap );
	class Iterator;
	friend class Iterator;

	virtual GsDt::Iterator*	Subscript( const GsDt &Index );
	virtual GsDt::Iterator*	SubscriptFirst( IterationFlag Flag );
	virtual GsStatus		SubscriptReplace( const GsDt &Index, GsDt *NewValue, GsCopyAction Action );
	virtual GsStatus		SubscriptDelete( const GsDt &Index );
	virtual GsStatus		SubscriptAppend( GsDt *NewValue, GsCopyAction Action );

protected:
	virtual GsStatus doFormat( GsDt::FormatInfo &Info );
	virtual GsString instanceInfo( GsDt::FormatInfo &Info );

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	Print(		GsDt &PrintMe );
	virtual GsStatus	PrintStart(	GsDt &PrintMe );
	virtual GsStatus	PrintEnd(	GsDt &PrintMe );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

