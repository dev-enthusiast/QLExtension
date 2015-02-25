/****************************************************************
**
**	gsdt/GsDtSymDate.h	- The GsDtSymDate class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtSymDate
**      Base		- SymDate
**      BaseParent	- 
**      HdrName		- gsdt/GsDtSymDate.h
**      FnClass		- GsDtSymDate.cpp
**      Streamable	- 1
**      Iterable	- 0
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtSymDate.h,v 1.4 2001/11/27 22:44:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTSYMDATE_H )
#define IN_GSDT_GSDTSYMDATE_H

#include	<gsdt/GsDt.h>
#include	<gscore/GsSymDate.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtSymDate;


class EXPORT_CLASS_GSDT GsDtSymDate : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtSymDate )
public:
	GsDtSymDate();
	GsDtSymDate( const GsDtSymDate &Obj );
	GsDtSymDate &operator=( const GsDtSymDate &Obj );

	virtual ~GsDtSymDate();

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
	GsSymDate	m_data;
public:
	GsDtSymDate( GsSymDate *date, GsCopyAction action );
	GsDtSymDate( const GsSymDate& date );
	const GsSymDate& data() const { return m_data; }
	      GsSymDate& data()       { return m_data; }
	void	dataSet( const GsSymDate& date ) { m_data = date; }

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

