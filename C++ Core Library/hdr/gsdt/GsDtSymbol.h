/****************************************************************
**
**	gsdt/GsDtSymbol.h	- The GsDtSymbol class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**		Class		- GsDtSymbol
**      Base		- Symbol
**      BaseParent	- 
**      HdrName		- gsdt/GsDtSymbol.h
**      FnClass		- GsDtSymbol.cpp
**      Streamable	- 1
**      Iterable	- 0
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtSymbol.h,v 1.11 2001/11/27 22:44:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTSYMBOL_H )
#define IN_GSDT_GSDTSYMBOL_H

#include	<gsdt/GsDt.h>
#include 	<gscore/GsSymbol.h>



CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/

class GsDtSymbol;


class EXPORT_CLASS_GSDT GsDtSymbol : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtSymbol )
public:
	GsDtSymbol();
	GsDtSymbol( const GsDtSymbol &Obj );
	GsDtSymbol &operator=( const GsDtSymbol &Obj );

	GsSymbol  data() const { return m_data; }
	GsSymbol &data()  	   { return m_data; }

	virtual ~GsDtSymbol();

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
	GsSymbol
			m_data;
public:			
		GsDtSymbol( const GsSymbol &Symbol ) : m_data( Symbol ) {}	

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
};

CC_END_NAMESPACE

#endif

