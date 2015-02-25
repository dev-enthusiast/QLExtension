/****************************************************************
**
**	GSFUNCSTREAM.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsFcStream.h,v 1.5 2012/07/23 13:05:03 e19351 Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSFUNCSTREAM_H )
#define IN_GSDT_GSFUNCSTREAM_H

#include <gsdt/base.h>
#include <ccstream.h>
#include <gscore/GsException.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsStreamMap.h>

CC_BEGIN_NAMESPACE( Gs )

template < class Domain, class Range >
void StreamWriteGsFuncObjects( GsStreamOut& Stream, GsFuncHandle< Domain, Range >& Func )
{
	typedef typename GsFuncHandle< Domain, Range >::RepType rep_t;
	const rep_t* Rep = Func.getFuncRep();

	if( !Rep )
	{
		CC_OSTRSTREAM Error;
		Error << "StreamWriteGsFuncObjects - Handle does not contain a function object";
		GSX_THROW( GsXStreamWrite, CC_OSTRSTREAM_STR( Error ) );
	}
	
	GsStreamMap* StreamMap = Rep->buildStreamMapper();

	if( !StreamMap )
	{
		CC_OSTRSTREAM Error;
		Error << "StreamWriteGsFuncObjects - Object does not have an associated stream map object";
		GSX_THROW( GsXStreamWrite, CC_OSTRSTREAM_STR( Error ) );		
	}

	StreamMap->StreamStore( Stream  );

	delete StreamMap;
}

template < class Domain, class Range >
void StreamReadGsFuncObjects( GsStreamIn& Stream, GsFuncHandle< Domain, Range >*& Ret )
{
	typedef typename GsFuncHandle< Domain, Range >::RepType rep_t;

	Ret = NULL;

	GsStreamMap* StreamMap = GsStreamMap::find( Stream );

	if( !StreamMap )
	{
		CC_OSTRSTREAM Error;
		Error << "StreamReadGsFuncObjects - Object does not have an associated stream map object";
		GSX_THROW( GsXStreamRead, CC_OSTRSTREAM_STR( Error ) );		
	}

	StreamMap->StreamRead( Stream );
	
	rep_t* Rep = reinterpret_cast< rep_t* >( StreamMap->getInObject() );

	if( !Rep )
	{
		CC_OSTRSTREAM Error;
		Error << "StreamWriteGsFuncObjects - Stream does not contain a function object";
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Error ) );
	}

	Ret = new GsFuncHandle< Domain, Range >( Rep );

	delete StreamMap;
}

CC_END_NAMESPACE

#endif 






