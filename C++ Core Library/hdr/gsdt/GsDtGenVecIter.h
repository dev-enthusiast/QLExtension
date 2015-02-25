/****************************************************************
**
**	gsdt/GsDtGenVecIter.h	- Iterator classes for GsDtGenericVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtGenVecIter.h,v 1.6 2001/11/27 22:43:59 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTGENVECITER_H )
#define IN_GSDT_GSDTGENVECITER_H

#include	<gsdt/base.h>
#include	<gscore/gsdt_fwd.h>
#include	<gsdt/GsDt.h>
#include	<gsdt/GsDtDouble.h>

CC_BEGIN_NAMESPACE( Gs )

template<class VEC_TYPE>
class GsDtGenVecIter : public GsDt::Iterator
{
public:
	GsDtGenVecIter( GsDtGenVector<VEC_TYPE>* Vec, int StartIndex = 0 );
	GsDtGenVecIter( const GsDtGenVecIter<VEC_TYPE> &Other );
	virtual ~GsDtGenVecIter();

	virtual GsDt::Iterator*	Copy();
	virtual GsDt*			BaseValue();
	virtual GsDt*			CurrentKey();
	virtual const GsDt*		CurrentValue();
	virtual GsDt*			ModifyValueBegin();
	virtual GsStatus		ModifyValueEnd();
	virtual GsStatus		Replace( GsDt *Val, GsCopyAction Action );

	virtual GsBool			More();
	virtual void			Next();

private:
	GsDtGenVecIter<VEC_TYPE> &operator=( const GsDtGenVecIter<VEC_TYPE> &Rhs );

	GsDtGenVector<VEC_TYPE>	*m_vec;	// vector over which we are iterating
	size_t			m_vecSize;		// m_vec->size();
	GsDtDouble		*m_key;			// Current key
	size_t			m_index;		// Actual current index (converted to m_key as needed)
	typename VEC_TYPE::GsDtTypeElem
					*m_modValue;	// Non-NULL if ModifyBegin() called, but not End() yet

	GsBool			inRange() { return m_index >= 0 && m_index < m_vecSize; }
};



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::Iterator
**	Returns: 
**	Action : Constructor
****************************************************************/

template<class VEC_TYPE>
GsDtGenVecIter<VEC_TYPE>::GsDtGenVecIter(
	GsDtGenVector<VEC_TYPE>	*vec,
	int						StartIndex
)
:	m_vec( vec ),
	m_vecSize( vec->size() ),
	m_key( NULL ),
	m_index( StartIndex ),
	m_modValue( NULL )
{
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::Iterator
**	Returns: 
**	Action : Copy constructor
**			 m_key is not copied because it will get recreated
**			 when necessary
****************************************************************/

template<class VEC_TYPE>
GsDtGenVecIter<VEC_TYPE>::GsDtGenVecIter(
	const GsDtGenVecIter<VEC_TYPE>& Other
)
:	m_vec( Other.m_vec ),
	m_vecSize( Other.m_vecSize ),
	m_key( NULL ),
	m_index( Other.m_index ),
	m_modValue( NULL )	// The other one might be, but this one isn't
{
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::~Iterator
**	Returns: 
**	Action : Destructor
****************************************************************/

template<class VEC_TYPE>
GsDtGenVecIter<VEC_TYPE>::~GsDtGenVecIter()
{
//	if( m_modActive )
//		m_vec->Unlock();
	if( m_modValue )
	{
		// FIX-Generate error message?  Should have called modifyEnd() first
		delete m_modValue;
	}
	delete m_key;
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::Copy
**	Returns: GsDt::Iterator * or NULL/GsErr()
**	Action : Return a copy of this iterator
****************************************************************/

template<class VEC_TYPE>
GsDt::Iterator* GsDtGenVecIter<VEC_TYPE>::Copy()
{
	return new GsDtGenVecIter<VEC_TYPE>( *this );
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::BaseValue
**	Returns: GsDt * or NULL/GsErr()
**	Action : Return the value for which this iterator is iterating
****************************************************************/

template<class VEC_TYPE>
GsDt *GsDtGenVecIter<VEC_TYPE>::BaseValue()
{
	return m_vec;
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::CurrentKey
**	Returns: GsDt * or NULL/GsErr()
**	Action : Return the current key value
****************************************************************/

template<class VEC_TYPE>
GsDt *GsDtGenVecIter<VEC_TYPE>::CurrentKey()
{
	if( !m_key )
		m_key = new GsDtDouble( m_index );
	else
		m_key->DblSet( m_index );
	return m_key;
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::CurrentValue
**	Returns: GsDt * or NULL/GsErr()
**	Action : Return the current value
****************************************************************/

template<class VEC_TYPE>
const GsDt *GsDtGenVecIter<VEC_TYPE>::CurrentValue()
{
	if( !inRange() )
	{
		GsErr( ERR_INVALID_INDEX, "GsDtGenVecIter<VEC_TYPE>::CurrentValue, @" );
		return NULL;
	}

	typename VEC_TYPE::value_type Val = m_vec->data()[ m_index ];
	return new typename VEC_TYPE::GsDtTypeElem( Val );
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::Replace
**	Returns: GsDt * or NULL/GsErr()
**	Action : Replace the current value
****************************************************************/

template<class VEC_TYPE>
GsStatus GsDtGenVecIter<VEC_TYPE>::Replace(
	GsDt			*Value,
	GsCopyAction	Action
)
{
	if( !inRange() )
		return GsErr( ERR_INVALID_INDEX, "GsDtGenVecIter<VEC_TYPE>::Replace, @" );
	
	return GsErr( ERR_UNSUPPORTED_OPERATION, "GsDtGenVecIter<VEC_TYPE>::Replace, @" );

	// FIX-Handle Action
	// FIX-Actually do an assign operation
	//->ElemSet( m_index, Value, Action );
	//(*m_vec)[ m_index ] = Value;
	//	return Gs_OK;
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::ModifyValueBegin
**	Returns: GsDt * or NULL/GsErr()
**	Action : Return the current value in a modifiable form
****************************************************************/

template<class VEC_TYPE>
GsDt *GsDtGenVecIter<VEC_TYPE>::ModifyValueBegin()
{
	if( m_modValue )
		GSX_THROW( GsXInvalidOperation, "GsDtGenVecIter<VEC_TYPE>::ModifyValueBegin, missing ModifyEnd call" );

	if( !inRange() )
	{
		GsErr( ERR_INVALID_INDEX, "GsDtGenVecIter<VEC_TYPE>::ModifyValueBegin, @" );
		return NULL;
	}

	m_modValue = new typename VEC_TYPE::GsDtTypeElem( m_vec->data()[ m_index ] );
	return m_modValue;
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::ModifyValueEnd
**	Returns: Ok/GsErr
**	Action : Push the changes made to current value since
**			 ModifyValueBegin
****************************************************************/

template<class VEC_TYPE>
GsStatus GsDtGenVecIter<VEC_TYPE>::ModifyValueEnd()
{
	if( !m_modValue )
		GSX_THROW( GsXInvalidOperation, "GsDtGenVecIter<VEC_TYPE>::ModifyValueEnd, missing ModifyBegin call" );

	m_vec->data()[ m_index ] = m_modValue->data();
	delete m_modValue;
	m_modValue = NULL;
    return GS_OK;
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::Next
**	Returns: TRUE/FALSE
**	Action : Move to next value, return TRUE if more values
****************************************************************/

template<class VEC_TYPE>
void GsDtGenVecIter<VEC_TYPE>::Next()
{
	if( m_modValue )
		GSX_THROW( GsXInvalidOperation, "GsDtGenVecIter<VEC_TYPE>::Next, missing ModifyEnd call" ); 
	m_index++;
}



/****************************************************************
**	Routine: GsDtGenVecIter<VEC_TYPE>::More
**	Returns: TRUE/FALSE
**	Action : Return TRUE if more values
****************************************************************/

template<class VEC_TYPE>
GsBool GsDtGenVecIter<VEC_TYPE>::More()
{
	return inRange();
}


CC_END_NAMESPACE
#endif

