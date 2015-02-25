/****************************************************************
**
**	gsdt/GsDtRDate.h	- The GsDtRDate class 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtRDate.h,v 1.5 2001/11/27 22:43:59 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTRDATE_H )
#define IN_GSDT_GSDTRDATE_H

#include	<gsdt/GsDt.h>
#include	<gscore/GsRDate.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtRDate;


class EXPORT_CLASS_GSDT GsDtRDate : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtRDate )
public:
	GsDtRDate();
	GsDtRDate( const GsDtRDate &Obj );
	GsDtRDate &operator=( const GsDtRDate &Obj );

	virtual ~GsDtRDate();

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


	virtual GsDt::Iterator	*Subscript( const GsDt &Index );

private:
	GsRDate	m_data;
public:
	GsDtRDate( GsRDate *date, GsCopyAction action );
	GsDtRDate( const GsRDate& date );
	const GsRDate& data() const { return m_data; }
	      GsRDate& data()       { return m_data; }
	void	dataSet( const GsRDate& date ) { m_data = date; }

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif



