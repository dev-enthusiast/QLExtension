/****************************************************************
**
**	gsdt/GsDtStream.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtStream.h,v 1.11 2011/05/17 15:50:01 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTSTREAM_H )
#define IN_GSDT_GSDTSTREAM_H

#include <gsdt/base.h>
#include <gscore/GsStream.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Exported functions
*/

EXPORT_CPP_GSDT void GsDtStreamWriteTypeAndObject( GsStreamOut &Stream, const GsDt &Object );
EXPORT_CPP_GSDT GsDt *GsDtStreamReadTypeAndObject( GsStreamIn &Stream );



template< class T >
class GsDtGenericStreamable : public GsDt
{
public:
	inline GsDtGenericStreamable( T *NewData )
	:	m_data( NewData )
	{
	}
	inline virtual ~GsDtGenericStreamable() 
	{
	}
	inline T *Data() 
	{ 
		return m_data; 
	}
	inline const T *Data() const 
	{ 
		return m_data; 
	}

	virtual void StreamStore( GsStreamOut &Stream ) const = 0;
	virtual void StreamRead(  GsStreamIn &Stream ) = 0;

private:
	T *m_data;
};



// A macro for declaring a generic streamable wrapper for a class.

#define GSDT_DECLARE_GENERIC_STREAMABLE( ClassName, T )		\
															\
class ClassName : public GsDtGenericStreamable< T >			\
{															\
public:														\
	ClassName( T *NewData )									\
		 : GsDtGenericStreamable< T >( NewData ) {};		\
	virtual ~ClassName() {};								\
															\
	virtual void StreamStore( GsStreamOut &Stream ) const;	\
	virtual void StreamRead(  GsStreamIn &Stream );			\
};



/****************************************************************
**	Routine: GsStreamStoreGsDtWrapper
**	Returns: void
**	Action : Create a wrapper around an object and then
**			 stream it.
****************************************************************/

template< class T >
inline void GsStreamStoreGsDtWrapper( 
	GsStreamOut &Stream, 
	T *Value 
) 
{ 
	const GsDt	
			*GsDtWrapper = Value->StreamableSelf();

	GsDtStreamWriteTypeAndObject( Stream, *GsDtWrapper );

	delete GsDtWrapper;
}



/****************************************************************
**	Routine: GsStreamReadGsDtWrapper
**	Returns: void
**	Action : Unstream a GsDt wrapping a T and then pull the 
**			 T out of it.
****************************************************************/

template< class T >
inline void GsStreamReadGsDtWrapper( 
	GsStreamIn &Stream, 
	T *&Value 
)
{ 
	UINT32	TypeId;

	GsDt	*GsDtWrapper = Value->StreamableSelf();


	GsStreamReadCompactInt( Stream, TypeId );

	if( ( TypeId != GsDtWrapper->typeId() ) )
	{
		delete GsDtWrapper;

		GSX_THROW( GsXStreamRead, "Type id mismatch in stream." );
	}
	else
	{
		GsDtWrapper->StreamRead( Stream );

		// The type matched, and we know we are dealing with a wrapped
		// object, so the following cast is not nearly as miserable as
		// it looks.

		GsDtGenericStreamable< T > *GsDtWrapperGeneric = reinterpret_cast< GsDtGenericStreamable< T > * >( GsDtWrapper );

		Value = GsDtWrapperGeneric->Data();

		delete GsDtWrapper;
	}
}


CC_END_NAMESPACE

#endif 

