/****************************************************************
**
**	gsdt/GsDtDayCount.h	- The GsDtDayCount class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtDayCount
**      Base		- DayCount
**      BaseParent	- 
**      HdrName		- gsdt/GsDtDayCount.h
**      FnClass		- GsDtDayCount.cpp
**      Streamable	- 1
**      Iterable	- 0
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtDayCount.h,v 1.4 2001/11/27 22:43:57 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTDayCount_H )
#define IN_GSDT_GSDTDayCount_H

#include	<gsdt/GsDt.h>
#include	<gscore/GsDayCount.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtDayCount;


class EXPORT_CLASS_GSDT GsDtDayCount : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtDayCount )
public:
	GsDtDayCount();
	GsDtDayCount( const GsDtDayCount &Obj );
	GsDtDayCount &operator=( const GsDtDayCount &Obj );

	virtual ~GsDtDayCount();

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
	GsDayCount	m_data;
public:
	GsDtDayCount( GsDayCount *date, GsCopyAction action );
	GsDtDayCount( const GsDayCount& date );
	const GsDayCount& data() const { return m_data; }
	      GsDayCount& data()       { return m_data; }
	void	dataSet( const GsDayCount& date ) { m_data = date; }

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

