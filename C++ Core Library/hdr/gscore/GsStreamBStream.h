/****************************************************************
**
**	GSSTREAMBSTREAM.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsStreamBStream.h,v 1.8 2012/07/02 19:51:23 e19351 Exp $
**
****************************************************************/

#if !defined( IN_GSSTREAMBSTREAM_H )
#define IN_GSSTREAMBSTREAM_H

#include <gscore/GsStream.h>
#include <bstream.h>

CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsStreamBStreamIn 
** Description : Stream class built on top of kool_ade BStream
**				 API.
****************************************************************/

class EXPORT_CLASS_GSBASE GsStreamInBStream : public GsStreamIn
{
public:
	GsStreamInBStream( BSTREAM *BStream ); 

	virtual ~GsStreamInBStream();

	// Inherited from GsStreamIn:

	virtual void ReadFloat( 	float 	&Value	);
	virtual void ReadDouble( 	double 	&Value 	);
													 
	virtual void ReadByte( 		UINT8 	&Value	);	

	virtual void ReadInt16( 	INT16 	&Value	);
	virtual void ReadInt32( 	INT32 	&Value	);
													 
	virtual void ReadUInt16( 	UINT16	&Value	);
	virtual void ReadUInt32( 	UINT32 	&Value	);
													 
	virtual void ReadString( 	char *String, size_t StringSize	);

	virtual void ReadString( 	GsString &String	);

private:	
	// We don't really want anyone to use these.
	GsStreamInBStream( const GsStreamInBStream &rhs ); 	
	GsStreamInBStream operator=( const GsStreamInBStream &rhs ); 

	BSTREAM	*m_BStream;

	GsBool m_AllocatedBStream;
};



/****************************************************************
** Class	   : GsStreamOutBStream
** Description : Stream class built on top of kool_ade BStream
**				 API.
****************************************************************/

class EXPORT_CLASS_GSBASE GsStreamOutBStream : public GsStreamOut
{
public:
	// Might want to add a default constructor, but we don't actually
	// use these except for processing DT_MSG_TO_STREAM messages in
	// DtFuncGsDt(), so we never have to construct the underying
	// BStream ouself.  The same is true for GsStreamInBStream.
	GsStreamOutBStream( BSTREAM *BStream );

	virtual ~GsStreamOutBStream();

	// Inherited from GsStreamOut:

	virtual void WriteFloat( 	float 	Value	);
	virtual void WriteDouble( 	double 	Value 	);
												 
	virtual void WriteByte( 	UINT8	Value	);
	
	virtual void WriteInt16( 	INT16 	Value	);
	virtual void WriteInt32( 	INT32 	Value	);
												 
	virtual void WriteUInt16( 	UINT16	Value	);
	virtual void WriteUInt32( 	UINT32 	Value	);

	virtual void WriteString( 	const char *String, size_t StringSize );

	virtual void WriteString( 	const GsString &String );

private:	
	// We don't really want anyone to use these.
	inline GsStreamOutBStream( const GsStreamOutBStream &rhs ); 	
	inline GsStreamOutBStream operator=( const GsStreamOutBStream &rhs ); 


	BSTREAM	*m_BStream;

	GsBool m_AllocatedBStream;
};


CC_END_NAMESPACE


#endif 
