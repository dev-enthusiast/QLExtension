/****************************************************************
**
**	GSSTREAM.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsStream.h,v 1.22 2012/07/02 19:51:22 e19351 Exp $
**
****************************************************************/

#if !defined( IN_GSSTREAM_H )
#define IN_GSSTREAM_H

#include <port_int.h>
#include <gscore/err.h>
#include <gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsStreamOut 
** Description : Base class for output streams.  These streams
**				 provide the very low-level functionality
**				 for writing simple data types like doubles
**				 and strings.  Dt streaming is done with 
**				 member functions on the Dt that build on top
**				 of this low-level functionality.
****************************************************************/

class EXPORT_CLASS_GSBASE GsStreamOut
{
public:
	GsStreamOut();
	virtual ~GsStreamOut();

	virtual void WriteFloat( 	float 	Value	) = 0;	
	virtual void WriteDouble( 	double 	Value 	) = 0;	

	virtual void WriteByte( 	UINT8 	Value	) = 0;	

	virtual void WriteInt16( 	INT16 	Value	) = 0;	
	virtual void WriteInt32( 	INT32 	Value	) = 0;	 

	virtual void WriteUInt16( 	UINT16	Value	) = 0;	 
	virtual void WriteUInt32( 	UINT32 	Value	) = 0;	

	virtual void WriteString( 	const char *String, size_t StringSize	) = 0;	 

	virtual void WriteString( 	const GsString &String ) = 0;

private:
	// We don't really want anyone to use these.
	GsStreamOut( const GsStreamOut &rhs ); 	
	GsStreamOut &operator=( const GsStreamOut &rhs ); 
};



/****************************************************************
** Class	   : GsStreamIn 
** Description : Base class for input stream.  These streams
**				 provide the very low-level functionality
**				 for reading simple data types like doubles
**				 and strings.  Dt streaming is done with 
**				 member functions on the Dt that build on top
**				 of this low-level functionality.
****************************************************************/

class EXPORT_CLASS_GSBASE GsStreamIn
{
public:
	GsStreamIn();
	virtual ~GsStreamIn();

	virtual void ReadFloat( 	float 	&Value	) = 0;
	virtual void ReadDouble( 	double 	&Value 	) = 0;

	virtual void ReadByte( 		UINT8  &Value	) = 0;	

	virtual void ReadInt16( 	INT16 	&Value	) = 0;
	virtual void ReadInt32( 	INT32 	&Value	) = 0;

	virtual void ReadUInt16( 	UINT16	&Value	) = 0;
	virtual void ReadUInt32( 	UINT32 	&Value	) = 0;

	virtual void ReadString( 	char *String, size_t StringSize	) = 0;

	virtual void ReadString( 	GsString &String	) = 0;

private:
	// We don't really want anyone to use these.
	GsStreamIn( const GsStreamIn &rhs ); 	
	GsStreamIn &operator=( const GsStreamIn &rhs ); 
};


// Reading and writing of more complex types.  Might want to move
// matrix and vector to specific headers for those classes.

void EXPORT_CPP_GSBASE GsStreamWriteCompactInt(	GsStreamOut &Stream, UINT32	Value			);
void EXPORT_CPP_GSBASE GsStreamWriteVector(		GsStreamOut &Stream, const GsVector &Vector );
void EXPORT_CPP_GSBASE GsStreamWriteMatrix(		GsStreamOut &Stream, const GsMatrix &Matrix );

void EXPORT_CPP_GSBASE GsStreamReadCompactInt( 	GsStreamIn &Stream,	UINT32	 &Value	 );
void EXPORT_CPP_GSBASE GsStreamReadVector( 		GsStreamIn &Stream, GsVector &Vector );
void EXPORT_CPP_GSBASE GsStreamReadMatrix( 		GsStreamIn &Stream, GsMatrix &Matrix );


// Inline functions to make life easier for streamgen.

inline void GsStreamStoreFloat( GsStreamOut &Stream, float  Value ) { Stream.WriteFloat ( Value ); }
inline void GsStreamStoreDouble( GsStreamOut &Stream, double Value ) { Stream.WriteDouble( Value ); }
inline void GsStreamStoreChar( GsStreamOut &Stream, UINT8   Value ) { Stream.WriteByte  ( Value ); }
inline void GsStreamStoreInt16( GsStreamOut &Stream, INT16  Value ) { Stream.WriteInt16 ( Value ); }
inline void GsStreamStoreInt32( GsStreamOut &Stream, INT32  Value ) { Stream.WriteInt32 ( Value ); }
inline void GsStreamStoreUInt16( GsStreamOut &Stream, UINT16 Value ) { Stream.WriteUInt16( Value ); }
inline void GsStreamStoreUInt32( GsStreamOut &Stream, UINT32 Value ) { Stream.WriteUInt32( Value ); }

inline void GsStreamStoreFloat( GsStreamOut &Stream, float  *Value ) { Stream.WriteFloat ( *Value ); }
inline void GsStreamStoreDouble( GsStreamOut &Stream, double *Value ) { Stream.WriteDouble( *Value ); }
inline void GsStreamStoreChar( GsStreamOut &Stream, UINT8  *Value ) { Stream.WriteByte  ( *Value ); }
inline void GsStreamStoreInt16( GsStreamOut &Stream, INT16  *Value ) { Stream.WriteInt16 ( *Value ); }
inline void GsStreamStoreInt32( GsStreamOut &Stream, INT32  *Value ) { Stream.WriteInt32 ( *Value ); }
inline void GsStreamStoreUInt16( GsStreamOut &Stream, UINT16 *Value ) { Stream.WriteUInt16( *Value ); }
inline void GsStreamStoreUInt32( GsStreamOut &Stream, UINT32 *Value ) { Stream.WriteUInt32( *Value ); }

inline void GsStreamReadFloat( GsStreamIn &Stream, float  &Value ) { Stream.ReadFloat ( Value ); }
inline void GsStreamReadDouble( GsStreamIn &Stream, double &Value ) { Stream.ReadDouble( Value ); }
inline void GsStreamReadChar( GsStreamIn &Stream, UINT8 &Value ) { Stream.ReadByte  ( Value ); }
inline void GsStreamReadInt16( GsStreamIn &Stream, INT16  &Value ) { Stream.ReadInt16 ( Value ); }
inline void GsStreamReadInt32( GsStreamIn &Stream, INT32  &Value ) { Stream.ReadInt32 ( Value ); }
inline void GsStreamReadUInt16( GsStreamIn &Stream, UINT16 &Value ) { Stream.ReadUInt16( Value ); }
inline void GsStreamReadUInt32( GsStreamIn &Stream, UINT32 &Value ) { Stream.ReadUInt32( Value ); }

inline void GsStreamReadFloat( GsStreamIn &Stream, float  *Value ) { Stream.ReadFloat ( *Value ); }
inline void GsStreamReadDouble( GsStreamIn &Stream, double *Value ) { Stream.ReadDouble( *Value ); }
inline void GsStreamReadChar( GsStreamIn &Stream, UINT8 *Value ) { Stream.ReadByte  ( *Value ); }
inline void GsStreamReadInt16( GsStreamIn &Stream, INT16  *Value ) { Stream.ReadInt16 ( *Value ); }
inline void GsStreamReadInt32( GsStreamIn &Stream, INT32  *Value ) { Stream.ReadInt32 ( *Value ); }
inline void GsStreamReadUInt16( GsStreamIn &Stream, UINT16 *Value ) { Stream.ReadUInt16( *Value ); }
inline void GsStreamReadUInt32( GsStreamIn &Stream, UINT32 *Value ) { Stream.ReadUInt32( *Value ); }

inline void GsStreamStoreString( GsStreamOut &Stream, const GsString &Value ) { Stream.WriteString( Value ); }
inline void GsStreamReadString(  GsStreamIn  &Stream, GsString &Value ) { Stream.ReadString( Value ); }

inline void GsStreamStoreBool( GsStreamOut &Stream, bool Value  ) { Stream.WriteByte  ( Value ); }
inline void GsStreamReadBool(  GsStreamIn  &Stream, bool &Value ) { UINT8 Byte; Stream.ReadByte( Byte ); Value = !!Byte; }


// Support for localized typing of handle/rep members of streamable
// classes.



typedef UINT32 GsLocalTypeID;

template< class T >
void GsStreamWriteLocalTypeAndObject( GsStreamOut &Stream, const T *Object )
{
	GsStreamWriteCompactInt( Stream, Object->LocalTypeID() );
	Object->StreamStore( Stream );
}



template< class T >
T *GsStreamReadLocalTypeAndObject( GsStreamIn &Stream, T * )
{
	GsLocalTypeID	
			LocalTypeID;

	T		*NewObject;


	GsStreamReadCompactInt( Stream, LocalTypeID );

	NewObject = T::LocalEmptyInstance( LocalTypeID );

	NewObject->StreamRead( Stream );

	return NewObject;
}




CC_END_NAMESPACE

#endif 
