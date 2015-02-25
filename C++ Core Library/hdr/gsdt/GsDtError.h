/****************************************************************
**
**	gsdt/GsDtError.h	- The GsDtError class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtError
**      Base		- Error
**      BaseParent	- 
**      HdrName		- gsdt/GsDtError.h
**      FnClass		- GsDtError.cpp
**      Streamable	- 1
**      Iterable	- 0
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtError.h,v 1.4 2001/11/27 22:45:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTERROR_H )
#define IN_GSDT_GSDTERROR_H

#include	<gsdt/GsDt.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtError;


class EXPORT_CLASS_GSDTLITE GsDtError : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtError )
public:
	GsDtError();
	GsDtError( const GsDtError &Obj );
	GsDtError &operator=( const GsDtError &Obj );

	virtual ~GsDtError();

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


#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

