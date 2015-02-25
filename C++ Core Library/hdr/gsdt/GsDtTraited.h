/****************************************************************
**
**	gsdt/GsDtTraited.h	- Generic traited 'smart-pointer' base class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtTraited.h,v 1.6 2001/11/27 22:45:02 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTTRAITED_H )
#define IN_GSDT_GSDTTRAITED_H

#include <gsdt/base.h>
#include <gsdt/macros.h>
#include <gsdt/GsDt.h>
#include <gsdt/GsDtTraits.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Generic traited smart-pointer template class
*/

template< class T, class R, class S, class I >
class GsDtTraited : public GsDt
{
protected:
	T*	m_data;
public:
	typedef T value_type;
	typedef R rtti;
	typedef S subscript;
	typedef I iterator;

	// static member functions to specialize
	// FIX-They don't have to be static

	static const GsDtTraited< T, R, S, I >* Cast( const GsDt* Obj );
	static GsDtTraited< T, R, S, I >* Cast( GsDt *Obj );
	static const GsDtTraited< T, R, S, I >& Cast( const GsDt& Obj );
	static GsDtTraited< T, R, S, I >& Cast( GsDt& Obj );

	
	static T* makeCopy( const T& data );
	static int compareSame( const T& lhs, const T& rhs );
	static bool isTrue( const T& data );
	static bool IsValid( const T& data );

	static T* makeCopy( const T* data )
	{
		if( data )
			return makeCopy( *data );
		else
			return NULL;
	}

	const T& data() const	{ return *m_data; }
	      T& data()		{ return *m_data; }

	T* stealData()
	{
		T* tmp = m_data;
		m_data = NULL;
		return tmp;
	}

	GsDtTraited();
	GsDtTraited( T* NewData, GsCopyAction Action );
	GsDtTraited( const T& NewData );
	GsDtTraited( const GsDtTraited< T, R, S, I >& Other );
	GsDtTraited& operator=( const GsDtTraited< T, R, S, I >& Other );
	virtual ~GsDtTraited();

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
	virtual GsDt::Iterator	*Subscript( const GsDt &Index );
	// return first subscript modulo flag
	virtual GsDt::Iterator	*SubscriptFirst( IterationFlag Flag );
	// delete This[ Index ]; This[ Index ] = NewValue;
	virtual GsStatus	SubscriptReplace( const GsDt &Index, GsDt *NewValue, GsCopyAction Action );
	// delete This[ Index ];
	virtual GsStatus	SubscriptDelete( const GsDt &Index );
	// Append a value (optionally make a copy)
	virtual GsStatus	SubscriptAppend( GsDt *NewValue, GsCopyAction Action );

	virtual GsStatus callMemberFn( const char *memberName, GsDt *Args[], int nArgs, GsDt *&RetValue );


private:

	static GsString myTypeName();
	static GsType *initType() CC_NOTHROW;

	// A hook that can be specialized to register ops.  Called by initType() and nobody else.
	virtual void RegisterOps() const;
};

CC_END_NAMESPACE
#endif

