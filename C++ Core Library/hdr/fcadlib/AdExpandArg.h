/****************************************************************
**
**	ADEXPANDARG.H	- AdExpandArg class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/AdExpandArg.h,v 1.28 2011/05/17 18:14:29 khodod Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADEXPANDARG_H )
#define IN_FCADLIB_ADEXPANDARG_H

#include	<fcadlib/base.h>
#include 	<vector>
#include 	<ccstream.h>
#include	<ccstl.h>
#include	<fcadlib/AdExpandArgBase.h>
#include	<fcadlib/autoarg.h>
#include 	<gscore/GsTsHandle.h>
#include	<gscore/AdVector.h>
#include    <gsdt/GsDtArray.h>

CC_BEGIN_NAMESPACE(Gs)



template <class ArgType> 
class AdExpandArg : public AdExpandArgBase
{
public:
	typedef ArgType value_type;
	typedef typename AdExpandVectorType<ArgType>::vector_type vector_type;
	typedef GsTimeSeries<ArgType> timeseries_type;


	AdExpandArg() : m_ExpandMode(AD_EXPAND_SCALAR), m_Val(0) {}
	~AdExpandArg() 
	{	
		clear();
	}

	AdExpandArg& operator=(const value_type& Value)
	{	
		clear();
		m_ExpandMode = AD_EXPAND_SCALAR;
		m_Val = new value_type( Value );
		return *this;
	}
	
	AdExpandArg& operator=(const vector_type& Vec)
	{
		clear();
		m_ExpandMode = AD_EXPAND_VECTOR;
		m_Vector = new vector_type(Vec);

		return *this;
	}

	AdExpandArg& operator=(const timeseries_type& TS)
	{
		clear();
		m_ExpandMode = AD_EXPAND_TIMESERIES;
		m_TS = new timeseries_type(TS);

		return *this;
	}

	AdExpandArg& operator=(vector_type* Vec)
	{
		clear();
		m_ExpandMode = AD_EXPAND_VECTOR;
		m_Vector = Vec;

		return *this;
	}

	AdExpandArg& operator=(timeseries_type* TS)
	{
		clear();
		m_ExpandMode = AD_EXPAND_TIMESERIES;
		m_TS = TS;

		return *this;
	}

	void clear()
	{
		switch (m_ExpandMode)
		{
			case AD_EXPAND_SCALAR:
				delete m_Val;
				break;
			case AD_EXPAND_VECTOR:
				delete m_Vector;
				break;
			case AD_EXPAND_TIMESERIES:
				delete m_TS;
				break;
		}
	}

	value_type& operator[](size_t Index)
	{	
		switch (m_ExpandMode)
		{
			case AD_EXPAND_VECTOR:
				return (*m_Vector)[Index];
				break;
			case AD_EXPAND_TIMESERIES:
				GSX_THROW(GsXUnsupported,"AdExpandArg::operator[] has no meaning in TIMESERIES mode");
				break;
			case AD_EXPAND_SCALAR:
				return *m_Val;
				break;
		}			
		
		GSX_THROW(GsXUnsupported,"AdExpandArg::operator[] unsupported mode (THIS SHOULD NEVER HAPPEN)");
	}


	value_type& at(const GsDate& Date)
	{
		switch (m_ExpandMode)
		{
			case AD_EXPAND_VECTOR:
				GSX_THROW(GsXUnsupported,"AdExpandArg::at has no meaning in VECTOR mode");
				break;
			case AD_EXPAND_TIMESERIES:
				return m_TS->at(Date).value();
				break;
			case AD_EXPAND_SCALAR:
				return *m_Val;
				break;
		}			

		GSX_THROW(GsXUnsupported,"AdExpandArg::at unsupported mode (THIS SHOULD NEVER HAPPEN)");
	}

	virtual AD_EXPAND_MODE mode() const { return m_ExpandMode; }

	inline operator const value_type&() const { return *m_Val; }

	size_t size() const
	{
		size_t Size = 0;
		switch (m_ExpandMode)
		{
			case AD_EXPAND_SCALAR:
				Size = 0;
				break;
			case AD_EXPAND_VECTOR:
				Size = m_Vector->size();
				break;
			case AD_EXPAND_TIMESERIES:
				Size = m_TS->size();
				break;
		}

		return Size;
	}
	
private:

	AdExpandArg( const AdExpandArg& );
	AdExpandArg& operator=( const AdExpandArg& );

	AD_EXPAND_MODE 
			m_ExpandMode;

	union
	{
		value_type*			m_Val;
		vector_type*		m_Vector;
		timeseries_type*	m_TS;
	};
};

template <class ArgType> 
class  AdExpandAutoArgRef : public AdExpandArgBase
{
public:
	typedef AutoArgRef < ArgType > 					value_type;
	typedef AdExpandVector< GsTsHandle <ArgType > >	vector_type;
	typedef GsTimeSeries< GsTsHandle < ArgType > > 	timeseries_type;

	AdExpandAutoArgRef() : m_ExpandMode(AD_EXPAND_SCALAR), m_Val(0) {}
	~AdExpandAutoArgRef() 
	{	
		clear();
	}

	AdExpandAutoArgRef& operator=(const value_type& Value)
	{	
		clear();
		m_ExpandMode = AD_EXPAND_SCALAR;
		m_Val = new value_type( Value );
		return *this;
	}
	
	AdExpandAutoArgRef& operator=(const vector_type& Vec)
	{
		clear();
		m_ExpandMode = AD_EXPAND_VECTOR;
		m_Vector = new vector_type(Vec);

		return *this;
	}

	AdExpandAutoArgRef& operator=(const timeseries_type& TS)
	{
		clear();
		m_ExpandMode = AD_EXPAND_TIMESERIES;
		m_TS = new timeseries_type(TS);

		return *this;
	}

	AdExpandAutoArgRef& operator=(vector_type* Vec)
	{
		clear();
		m_ExpandMode = AD_EXPAND_VECTOR;
		m_Vector = Vec;

		return *this;
	}

	AdExpandAutoArgRef& operator=(timeseries_type* TS)
	{
		clear();
		m_ExpandMode = AD_EXPAND_TIMESERIES;
		m_TS = TS;

		return *this;
	}

	void clear()
	{
		switch (m_ExpandMode)
		{
			case AD_EXPAND_SCALAR:
				delete m_Val;
				m_Val = 0;
				break;
			case AD_EXPAND_VECTOR:
				delete m_Vector;
				m_Vector = 0;
				break;
			case AD_EXPAND_TIMESERIES:
				delete m_TS;
				m_TS = 0;
				break;
		}

	}

	const ArgType* operator[](size_t Index)
	{	
		switch (m_ExpandMode)
		{
			case AD_EXPAND_VECTOR:
				return (*m_Vector)[Index];
				break;
			case AD_EXPAND_TIMESERIES:
				GSX_THROW(GsXUnsupported,"AdExpandArg::operator[] has no meaning in TIMESERIES mode");
				break;
			case AD_EXPAND_SCALAR:
				return m_Val->cPtr();
				break;
		}			
		
		GSX_THROW(GsXUnsupported,"AdExpandArg::operator[] unsupported mode (THIS SHOULD NEVER HAPPEN)");
	}


	const ArgType* at(const GsDate& Date)
	{
		switch (m_ExpandMode)
		{
			case AD_EXPAND_VECTOR:
				GSX_THROW(GsXUnsupported,"AdExpandArg::at has no meaning in VECTOR mode");
				break;
			case AD_EXPAND_TIMESERIES:
				return m_TS->at(Date).value();
				break;
			case AD_EXPAND_SCALAR:
				return m_Val->cPtr();
				break;
		}			

		GSX_THROW(GsXUnsupported,"AdExpandArg::at unsupported mode (THIS SHOULD NEVER HAPPEN)");
	}

	virtual AD_EXPAND_MODE mode() const { return m_ExpandMode; }

//	inline operator const value_type&()  { return *m_Val; }
	inline const ArgType& operator*() const { return m_Val->operator*(); }

	size_t size() const
	{
		size_t Size = 0;
		switch (m_ExpandMode)
		{
			case AD_EXPAND_SCALAR:
				Size = 0;
				break;
			case AD_EXPAND_VECTOR:
				Size = m_Vector->size();
				break;
			case AD_EXPAND_TIMESERIES:
				Size = m_TS->size();
				break;
		}

		return Size;
	}
	
private:

	AdExpandAutoArgRef( const AdExpandAutoArgRef& );
	AdExpandAutoArgRef& operator=( const AdExpandAutoArgRef& );

	AD_EXPAND_MODE 
			m_ExpandMode;

	union
	{
		value_type*			m_Val;
		vector_type*		m_Vector;
		timeseries_type*	m_TS;
	};
};

/****************************************************************
**	Routine: template <A,B> fromGsDtArrayToVector
**	Returns: GsStatus
**	Action : Copies contents of array to vector.
****************************************************************/


template< class ElemType, class VectorType >
GsStatus fromGsDtArrayToVector( 
	const GsDtArray&	Array, 
	VectorType& 		Vector,
	ElemType*           NotUsed	// = NULL. Note: SW4.2 does not allow default args
)
{
	int 	Index,
			NumElems = Array.size();

	GsStatus
			Status = GS_OK;

	Vector.resize( NumElems );

	for( Index = 0; Status == GS_OK && Index < NumElems; Index++ )
	{
		const GsDtGeneric< ElemType >
				*Elem = GsDtGeneric< ElemType >::Cast( Array.elem( Index ) );

		if ( !Elem )
			Status =  GsErr( ERR_INVALID_ARGUMENTS, "Element %d is of wrong type %s, expected ",
							 Index, Array.elem( Index )->typeName(), ElemType::typeName() );
		else
			Vector[ Index ] = GsDtGeneric< ElemType >::makeCopy( Elem->data() );
	}

	return Status;
}



template <class DT_, class VT_>
GsDtArray *VectorTypeToGsDtArray(
	const VT_ &Vector,
	const DT_ * // = NULL.  Note: SW4.2 does not allow default args
)
{
	GsDtArray	*Result = new GsDtArray( Vector.size() );

	size_t		Index;

	for( Index = 0; Index < Vector.size(); ++Index )
		Result->elemSet( Index, new DT_( Vector[ Index ] ), GS_NO_COPY );

	return Result;
}



template <class DT_, class VT_>
GsDtArray *VectorTypeOfTsHandleToGsDtArray(
	const VT_ &Vector,
	const DT_ * // = NULL.  Note: SW4.2 does not allow default args
)
{
	GsDtArray	*Result = new GsDtArray( Vector.size() );

	size_t		Index;

	for( Index = 0; Index < Vector.size(); ++Index )
		Result->elemSet( Index, new DT_( *( Vector[ Index ].getPtr() ) ), GS_NO_COPY );

	return Result;
}



CC_END_NAMESPACE

#endif 

