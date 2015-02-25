/****************************************************************
**
**	gsdt/GsDtGeneric.h	- Generic 'smart-pointer' base class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtGeneric.h,v 1.17 2001/11/27 22:45:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTGENERIC_H )
#define IN_GSDT_GSDTGENERIC_H

#include	<gsdt/base.h>
#include	<gsdt/macros.h>
#include	<gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Generic smart-pointer template class
**	FIX-More of this should be declared out-of-line like initType()
*/

template<class T>
class GsDtGeneric : public GsDt
{
	GSDT_DECLARE_CASTS( GsDtGeneric<T> )
protected:
	T*	m_data;
public:
	// static member functions to specialize
	// FIX-They don't have to be static
	static T* makeCopy( const T& data );
	static int compareSame( const T& lhs, const T& rhs );
	static bool isTrue( const T& data );

	static bool isValid( const T& data ); // Deprecated, do not call directly, please call via IsValid( const T& data )
	static bool IsValid( const T& data );

	static T* makeCopy( const T* data )
	{
		if( data )
			return makeCopy( *data );
		else
			return NULL;
	}

	const T& data() const	{ return *m_data; }
		  T& data()			{ return *m_data; }
	T* stealData()
	{
		T* tmp = m_data;
		m_data = NULL;
		return tmp;
	}

	GsDtGeneric();
	GsDtGeneric( T* NewData, GsCopyAction Action );
	GsDtGeneric( const T& NewData );
	GsDtGeneric( const GsDtGeneric<T>& Other );
	GsDtGeneric& operator=( const GsDtGeneric<T>& Other );
	virtual ~GsDtGeneric();

	virtual void releaseData();

	virtual GsDt* CopyDeep() const;
	virtual GsDt* CopyShallow() const;

	// DT_MSG_TO/FROM_STREAM
  	virtual void		StreamStore( GsStreamOut &Stream ) const;
  	virtual void		StreamRead(  GsStreamIn  &Stream );

	virtual int CompareSame( const GsDt& Other ) const;

    virtual FcHashCode  HashQuick() const;	// Quick hashing (like HashStrHash--for structures)

	virtual GsStatus BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );

	virtual GsBool Truth() const;


	virtual GsBool isValid() const; // Deprecated, this was never part of the GsDt interface
	virtual GsBool IsValid() const;

	virtual GsString toString() const;

	virtual GsDt *MathOperateOnAll( GsFuncHandle<double, double> Func ) const;

	virtual GsDt *Evaluate( const GsDtArray &Args ) const;

	// Things from GSDT_DECLARE_COMMON
	static GsType& staticType();
	virtual const GsType& type() const;
	virtual bool isKindOf( const GsType& Type ) const;

	virtual size_t      GetSize() const;
	virtual size_t		GetMemUsage() const;

	// return modifiable iterator starting at This[ Index ]
	virtual Iterator	*Subscript( const GsDt &Index );
	// return first subscript modulo flag
	virtual Iterator	*SubscriptFirst( IterationFlag Flag );

	virtual GsStatus callMemberFn( const char *memberName, GsDt *Args[], int nArgs, GsDt *&RetValue );

	virtual CC_NS(std,string) memberFunctionWrapperPrefix() const;

private:

	static GsString RawTypeName();

	static GsString myTypeName();
	static GsType *initType() CC_NOTHROW;

	// A hook that can be specialized to register ops.  Called by initType() and nobody else.
	virtual void RegisterOps() const;
};

// Specialize makeCopy() if the copy constructor is not the best way
// to make a copy of a T--for example, when T is an abstract base class.
template<class T>
inline T *GsDtGeneric<T>::makeCopy( const T& data )
{
	return new T( data );
}



// Specialize compareSame() if you wish to be able to sort and/or do
// something more than just compare pointers when testing equivalence.
//
// Potential specialization:
//	template<class T>
//	int compareSame( const T& lhs, const T& rhs )
//	{
//		return lhs.compare( rhs );
//	}

template<class T>
inline int GsDtGeneric<T>::compareSame( const T& lhs, const T& rhs )
{
	if( &lhs > &rhs )
		return 1;
	else if( &lhs == &rhs )
		return 0;
	else
		return -1;
}


// Specialize BinaryOpAssignSame() if your type can handle T OP T
//
// Potential specialization:
//	bool GsDtGeneric<MyType>::BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other ) 
//	{
//		GsDtGeneric<MyType>* Ptr = GsDtGeneric<MyType>::Cast( &Other );
//		
//		// Really needs a switch statement to do all OPs
//		return *m_data OP= *Ptr->m_data;
//	}

template<class T>
GsStatus GsDtGeneric<T>::BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other )
{
	// This default implementation fails with an appropriate error message
	return GsDt::BinaryOpAssignSame( Op, Other );
}


// Specialize isTrue() if your type should respond to if( T )
//
// Potential specialization:
//	template<class T>
//	bool isTrue( const T& data )
//	{
//		return data != 0;
//	}

template<class T>
inline bool GsDtGeneric<T>::isTrue( const T& data )
{
	return true;
}



// Specialize isValid() if your type has an error sentinel
//
// Potential specialization:
//	template<class T>
//	bool isValid( const T& data )
//	{
//		return data.ptr() != NULL;
//	}

template<class T>
inline bool GsDtGeneric<T>::isValid( const T& data )
{
	return IsValid( data );
}

template<class T>
inline bool GsDtGeneric<T>::IsValid( const T& data )
{
	return true;
}

// Specialize MathOperateOnAll() if your type contains doubles.

template<class T>
inline GsDt *GsDtGeneric<T>::MathOperateOnAll( GsFuncHandle<double, double> Func ) const
{
	GsErr( ERR_UNSUPPORTED_OPERATION, "%s: MathOperateOnAll(), @", this->typeName() );
	return NULL;	
}



// Specialize Evaluate() if your type is a functional form

template<class T>
inline GsDt *GsDtGeneric<T>::Evaluate( const GsDtArray &Args ) const
{
	GsErr( ERR_UNSUPPORTED_OPERATION, "%s: Evaluate(), @", this->typeName() );
	return NULL;	
}

CC_END_NAMESPACE
#endif

