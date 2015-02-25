/****************************************************************
**
**	gsdt/GsDtDate.h	- The GsDtDate class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtDate
**      Base		- Date
**      BaseParent	- 
**      HdrName		- gsdt/GsDtDate.h
**      FnClass		- GsDtDate.cpp
**      Streamable	- 1
**      Iterable	- 0
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtDate.h,v 1.8 2001/11/27 22:43:57 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTDATE_H )
#define IN_GSDT_GSDTDATE_H

#include	<gsdt/GsDt.h>
#include	<gscore/GsDate.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtDate;


class EXPORT_CLASS_GSDT GsDtDate : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtDate )
public:
	GsDtDate();
	GsDtDate( const GsDtDate &Obj );
	GsDtDate &operator=( const GsDtDate &Obj );

	virtual ~GsDtDate();

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
	GsDate	m_data;
public:
	GsDtDate( GsDate *date, GsCopyAction action );
	GsDtDate( const GsDate& date );
	const GsDate& data() const { return m_data; }
	      GsDate& data()       { return m_data; }
	void	dataSet( const GsDate& date ) { m_data = date; }

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

