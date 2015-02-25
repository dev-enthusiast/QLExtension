/****************************************************************
**
**	GSDTRDATEITER.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtRDateIter.h,v 1.4 2001/11/27 22:44:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTRDATEITER_H )
#define IN_GSDT_GSDTRDATEITER_H

#include <gsdt/base.h>

#include	<gsdt/base.h>
#include	<gscore/gsdt_fwd.h>
#include	<gsdt/GsDt.h>
#include	<gsdt/GsDtDouble.h>

CC_BEGIN_NAMESPACE( Gs )

class GsDtRDateIter : public GsDt::Iterator
{
public:
	GsDtRDateIter( GsDtRDate* RDate, int StartIndex = 0 )
	: m_rdate( RDate ),
	  m_size( RDate->GetSize() ),
	  m_key( NULL ),
	  m_index( StartIndex ),
	  m_modValue( NULL )
	{
	}
		
	GsDtRDateIter( const GsDtRDateIter& Other )
	: m_rdate( Other.m_rdate ),
	  m_size( Other.m_size ),
	  m_key( NULL ),
	  m_index( Other.m_index ),
	  m_modValue( NULL )
	{
	}

	virtual ~GsDtRDateIter()
	{
		if( m_modValue )
		{
			// FIX-Generate error message?  Should have called modifyEnd() first
			delete m_modValue;
		}

		delete m_key;
	}

	virtual GsDt::Iterator*	Copy() { return new GsDtRDateIter( *this ); }
	virtual GsDt*			BaseValue() { return  m_rdate; }
	virtual GsDt*			CurrentKey() 
	{
		if( !m_key )
			m_key = new GsDtDouble( m_index );
		else
			m_key->DblSet( m_index );

		return m_key;
	}	

	virtual const GsDt*		CurrentValue()
	{
		if( !inRange() )
		{
			GsErr( ERR_INVALID_INDEX, "GsDtRDateIter::CurrentValue, @" );
			return NULL;
		}

		return new GsDtRDate( m_rdate->data().elem( m_index ) );
	}

	virtual GsDt*			ModifyValueBegin()
	{
		GSX_THROW( GsXUnsupported, "GsDtRDateIter::ModifyValueBegin" );
		return m_modValue;
	}
	
	virtual GsStatus		ModifyValueEnd()
	{
		return GsErr( ERR_UNSUPPORTED_OPERATION, "GsDtRDateIter::ModifyValueEnd, @" );;
	}
	
	virtual GsStatus		Replace( GsDt* Val, GsCopyAction Action )
	{
		if( !inRange() )
			return GsErr( ERR_INVALID_INDEX, "GsDtRDateIter::Replace, @" );


		GsDtRDate* RDate = GsDtRDate::Cast ( Val );
		if( !RDate )
			return GsErr( ERR_INVALID_ARGUMENTS, "GsDtRDateIter::Replace, @" );

		m_rdate->data().elemSet( m_index, RDate->data() );

		return GS_OK;
	}	

	virtual GsBool			More() { return inRange(); }

	virtual void			Next()
	{
		if( m_modValue )
			GSX_THROW( GsXInvalidOperation, "GsDtRDateIter::Next, missing ModifyEnd call" ); 
		m_index++;
	}


private:
	GsDtRDateIter &operator=( const GsDtRDateIter& Rhs );

	GsDtRDate*	m_rdate;	// vector over which we are iterating
	size_t	 	m_size;		// m_vec->size();
	GsDtDouble*	m_key;		// Current key
	size_t		m_index;	// Actual current index (converted to m_key as needed)
	GsDtRDate*	m_modValue;	// Non-NULL if ModifyBegin() called, but not End() yet

	GsBool		inRange() { return m_index >= 0 && m_index < m_size; }
};


CC_END_NAMESPACE

#endif 
