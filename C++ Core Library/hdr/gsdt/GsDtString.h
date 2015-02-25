/****************************************************************
**
**	gsdt/GsDtString.h	- The GsDtString class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtString
**      Base		- String
**      BaseParent	- 
**      HdrName		- gsdt/GsDtString.h
**      FnClass		- GsDtString.cpp
**      Streamable	- 1
**      Iterable	- 0
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtString.h,v 1.5 2001/11/27 22:45:02 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTSTRING_H )
#define IN_GSDT_GSDTSTRING_H

#include	<gsdt/GsDt.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtString;


class EXPORT_CLASS_GSDTLITE GsDtString : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtString )
public:
	GsDtString();
	GsDtString( const GsDtString &Obj );
	GsDtString &operator=( const GsDtString &Obj );

	virtual ~GsDtString();

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
	// char	*m_Str;		// Always malloc'd
	GsString	m_data;
public:
	// All of these constructors always copy the underlying string.
	// Some of the constructors are just here to keep templates happy
	GsDtString( const char *Str );	// Same as Str, GS_COPY
	GsDtString( const GsString& Str );	// Same as Str, GS_COPY
//	GsDtString( char *Str, GsCopyAction Action );
//	GsDtString( const GsString& Str, GsCopyAction Action );

	const GsString& data() const { return m_data; }
	const char *c_str() const { return m_data.c_str(); }
	void	dataSet( const GsString& str ) { m_data = str; }

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

