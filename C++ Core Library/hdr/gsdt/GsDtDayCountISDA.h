/****************************************************************
**
**	gsdt/GsDtDayCountISDA.h	- The GsDtDayCountISDA class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtDayCountISDA
**      Base		- DayCountISDA
**      BaseParent	- 
**      HdrName		- gsdt/GsDtDayCountISDA.h
**      FnClass		- GsDtDayCountISDA.cpp
**      Streamable	- 1
**      Iterable	- 0
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtDayCountISDA.h,v 1.4 2001/11/27 22:43:57 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTDAYCOUNTISDA_H )
#define IN_GSDT_GSDTDAYCOUNTISDA_H

#include	<gsdt/GsDt.h>
#include	<gscore/GsDayCountISDA.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtDayCountISDA;


class EXPORT_CLASS_GSDT GsDtDayCountISDA : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtDayCountISDA )
public:
	GsDtDayCountISDA();
	GsDtDayCountISDA( const GsDtDayCountISDA &Obj );
	GsDtDayCountISDA &operator=( const GsDtDayCountISDA &Obj );

	virtual ~GsDtDayCountISDA();

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
	GsDayCountISDA	m_data;
public:
	GsDtDayCountISDA( GsDayCountISDA *date, GsCopyAction action );
	GsDtDayCountISDA( const GsDayCountISDA& date );
	const GsDayCountISDA& data() const { return m_data; }
	      GsDayCountISDA& data()       { return m_data; }
	void	dataSet( const GsDayCountISDA& date ) { m_data = date; }

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

