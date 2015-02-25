/****************************************************************
**
**	gsdt/GsDtGenVector.h - Generic wrapper for GsXXXXVector classes
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtGenVector.h,v 1.12 2012/06/28 15:44:11 e19351 Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTGENVECTOR_H )
#define IN_GSDT_GSDTGENVECTOR_H

#include    <ccstream.h>
#include	<gsdt/base.h>
#include	<gsdt/macros.h>
#include	<gsdt/GsDt.h>
#include	<datatype.h>

CC_BEGIN_NAMESPACE( Gs )


//class GsDtcGenVector;

template<class VT> class GsDtGenVecIter;

template<class VEC_TYPE>
class GsDtGenVector : public GsDt
{
	GSDT_DECLARE_CASTS( GsDtGenVector<VEC_TYPE> )

//	friend GsDtcGenVector;
	VEC_TYPE	*m_data;

public:
	int compareSame( const VEC_TYPE& lhs, const VEC_TYPE& rhs ) const;

	VEC_TYPE* makeCopy( const VEC_TYPE& data )
	{
		return new VEC_TYPE( data );
	}
	VEC_TYPE* makeCopy( const VEC_TYPE* data )
	{
		if( data )
			return makeCopy( *data );
		else
			return NULL;
	}

	GsDtGenVector()
	:	m_data( NULL )
	{
	}

	GsDtGenVector( VEC_TYPE *NewData, GsCopyAction Action )
	:	m_data( NewData )
	{
		if( NewData && Action != GS_NO_COPY )
			m_data = makeCopy( NewData );
	}
	GsDtGenVector( const VEC_TYPE &NewData )
	:	m_data( makeCopy( NewData ))
	{
	}
	GsDtGenVector( const GsDtGenVector<VEC_TYPE>& Other )
	:	m_data( makeCopy( Other.m_data ))
	{
	}
	GsDtGenVector( size_t Size )
	:	m_data( new VEC_TYPE( Size ))
	{
	}
	GsDtGenVector<VEC_TYPE> &operator=( const GsDtGenVector<VEC_TYPE> &Other )
	{
		if( m_data )
			delete m_data;
		m_data = makeCopy( Other.m_data );
		return *this;
	}

	virtual ~GsDtGenVector()
	{
		delete m_data;
	}

	virtual void releaseData()
	{
		m_data = NULL;
	}
	virtual GsDt* CopyDeep() const
	{
		return new GsDtGenVector<VEC_TYPE>( m_data, GS_COPY );
	}

	virtual GsDt* CopyShallow() const
	{
		return CopyDeep();
	}

	virtual int CompareSame( const GsDt& Other ) const
	{
		const GsDtGenVector<VEC_TYPE>* OPtr = GsDtGenVector<VEC_TYPE>::Cast( &Other );
		if( m_data == OPtr->m_data )
			return 0;
		if( m_data && !OPtr->m_data )
			return 1;
		if( !m_data && OPtr->m_data )
			return -1;
		return compareSame( *m_data, *OPtr->m_data );
	}

	virtual GsStatus BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );

	virtual size_t		GetSize() const
	{
		if( m_data )
			return m_data->size();
		else
			return 0;
	}
	virtual size_t		GetCapacity() const
	{
		if( m_data )
			return m_data->capacity();
		else
			return 0;
	}
	virtual size_t		GetMemUsage() const
	{
		if( m_data )
//			return m_data->memUsage();
			return 0;
		else
			return 0;
	}
//	virtual GsStatus	GetInfo( FcDataTypeInstanceInfo &Info ) const;

	virtual void		StreamStore( GsStreamOut &Stream ) const;
	virtual void		StreamRead(  GsStreamIn  &Stream );

//	virtual FcHashCode	HashComplete() const;	// Complete hashing (like _HASH_PORTABLE)

	virtual FcHashCode  HashQuick() const	// Quick hashing (like HashStrHash--for structures)
	{		
		FcHashCode
				AddrHash  	 = ( FcHashCode )this, 
				TmpHash[ 2 ] = { m_data->size(), 0xccbb003f };

		DT_HASH_MIX( AddrHash, TmpHash[ 0 ], TmpHash[ 1 ] );

		return AddrHash;	
	}

//	virtual GsStatus	BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );
//	virtual int			CompareSame( const GsDt &Other ) const;

	virtual GsBool		Truth() const
	{
		return m_data != 0;
	}

	virtual GsBool		IsValid() const
	{
		return m_data != 0;
	}

//	virtual GsStatus	Increment();
//	virtual GsStatus	Decrement();
//	virtual GsDt		*Negate() const;

	virtual GsString	toString() const
	{
		if( m_data )
			return m_data->toString();
		return staticType().name() + ": NULL";
	}

	const VEC_TYPE& data() const	{ return *m_data; }
	      VEC_TYPE& data()			{ return *m_data; }
	// Release ownership of data() and forget about it 
	VEC_TYPE *stealData()
	{
		VEC_TYPE *tmp = m_data;
		m_data = NULL;
		return tmp;
	}
	GsBool resize( size_t newSize, GsDt *val )
	{
		// FIX-does not use val
		if( !m_data )
			return false;	// FIX-should this allocate m_data?
		m_data->resize( newSize );
		return true;
	}
	size_t size()
	{
		if( m_data )
			return m_data->size();
		else
			return 0;
	}


	friend class GsDtGenVecIter<VEC_TYPE>;
	virtual GsDt::Iterator	*Subscript( const GsDt &Index );
	virtual GsDt::Iterator	*SubscriptFirst( GsDt::IterationFlag Flag );
	virtual GsStatus		SubscriptReplace( const GsDt &Index, GsDt *NewValue, GsCopyAction Action );
	virtual GsStatus		SubscriptDelete( const GsDt &Index );
	virtual GsStatus		SubscriptAppend( GsDt *NewValue, GsCopyAction Action );

	// Things from GSDT_DECLARE_COMMON
	static GsType& staticType() CC_NOTHROW
	{
		return *initType();
	}
	virtual const GsType& type() const CC_NOTHROW
	{
		return *initType();
	}
	virtual bool isKindOf( const GsType& Type ) const CC_NOTHROW
	{
		// Check my type, then check my parent's type
		return *initType() == Type || GsDt::isKindOf( Type );
	}
private:
	static GsString myTypeName();
	static GsType *initType() CC_NOTHROW;
};


CC_END_NAMESPACE

#endif

