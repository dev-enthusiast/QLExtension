/* $Header: /home/cvs/src/gsbase/src/gscore/GsVector.h,v 1.38 2001/11/27 22:41:10 procmon Exp $ */
/****************************************************************
**
**	gscore/GsVector.h	- GsVector class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsVector.h,v 1.38 2001/11/27 22:41:10 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_VECTOR_H )
#define IN_GSCORE_VECTOR_H

#include <gscore/base.h>
#include <gscore/types.h>
#include <stddef.h>
#include <string>
#include <algorithm>

#include <gscore/GsDblBuffer.h>

CC_BEGIN_NAMESPACE( Gs )

class GsTensor;

/*
**	GsVector is a vector of doubles
*/

class EXPORT_CLASS_GSBASE GsVector
{
public:
	typedef class GsDtVector	GsDtType;
	typedef double* 			iterator;
	typedef const double*		const_iterator;

	static const char *typeName() { return "GsVector"; }

	explicit GsVector( size_t InitialSize = 0, const double InitVal = 0.0 );
	GsVector( size_t InitialSize, const double* InitVals );
	GsVector( size_t InitialSize, GsUninitialized );

	// Works only if the tensor is one-dimensional.  Shares the same underlying GsDblBuffer.
	explicit GsVector( const GsTensor &Tensor );

	//GsVector( const GsVector& Vec );
	//~GsVector();
	//GsVector&		operator=( const GsVector& Vec );
	const GsVector& operator=( double a );

	GsVector( const GsUnsignedVector &UnsignedVec );
	GsVector( const GsIntegerVector  &IntegerVec  );

	size_t			size() const		{ return m_Size; }
	size_t			capacity() const	{ return m_Size; }

	const double*	data() const		{ return Data(); }
	void			makeWritable()		{ m_Buffer.cow( size()); }

	void resize( size_t NewSize, double FillValue = 0 )
	{
		resize( NewSize, FillValue, true );
	}
	void resize( size_t NewSize, GsUninitialized )
	{
		resize( NewSize, 0.0, false );
	}

	iterator		begin()				{ makeWritable(); return Data(); }
	const_iterator	begin() const		{ return Data(); }
	iterator		end()				{ makeWritable(); return Data() + m_Size; }
	const_iterator	end() const			{ return Data() + m_Size; }

	// Range check ifdef GS_RANGE_CHECK
	double&			operator()( size_t Index );
	double const&	operator()( size_t Index ) const;
	double&			operator[]( size_t Index )			{ return (*this)( Index ); }
	double const&	operator[]( size_t Index ) const	{ return (*this)( Index ); }
	void			elemSet( size_t Index, double Val ) { makeWritable(); (*this)( Index ) = Val; }

	// Always range check
	double const&	at( size_t Index ) const;
	double&			at( size_t Index );

	GsVector&		operator+=( double Val );
	GsVector&		operator-=( double Val );
	GsVector&		operator*=( double Val );
	GsVector&		operator/=( double Val );

	// Element-by-element multiplication, division, addition, and subtraction
	GsVector&		operator*=( const GsVector& Vec );
	GsVector&		operator/=( const GsVector& Vec );
	GsVector&		operator+=( const GsVector& Vec );
	GsVector&		operator-=( const GsVector& Vec );

	// Equality, less than
	bool operator==( const GsVector &rhs ) const;
	bool operator< ( const GsVector& rsh ) const;

	inline friend void swap( GsVector& lhs, GsVector& rhs );

private:
	size_t		m_Size;

	GsDblBuffer	m_Buffer;

	double * Data() const						{ return m_Buffer.data(); }

	// Never range check
	double const&	elemNoRange( size_t Index ) const	{ return Data()[ Index ]; }
	double&	elemNoRange( size_t Index )			{ makeWritable(); return Data()[ Index ]; }

	void	resize( size_t NewSize, double FillValue, bool DoFill );


	// Throw exceptions
	void	errIndex( size_t Index ) const;
	void	errSize( const char* Op, size_t SizeOther ) const;

	friend class GsTensor;
};

#ifndef GS_RANGE_CHECK
inline double const& GsVector::operator()( size_t Index ) const
{
	return elemNoRange( Index );
}

inline double &GsVector::operator()( size_t Index )
{
	return elemNoRange( Index );
}
#else
inline double const& GsVector::operator()( size_t Index ) const
{
	return at( Index );
}

inline double &GsVector::operator()( size_t Index )
{
	return at( Index );
}

#endif

void swap( GsVector& lhs, GsVector& rhs )
{
	CC_USING( std::swap );
	swap( lhs.m_Buffer, rhs.m_Buffer );
	swap( lhs.m_Size, rhs.m_Size );
}

EXPORT_CPP_GSBASE double InnerProduct( const GsVector& Vec1, const GsVector& Vec2 );

/*
**	All operators are element-wise
*/

EXPORT_CPP_GSBASE GsVector
	operator*( const GsVector& Vec1, const GsVector& Vec2 ),
	operator*( double sc, const GsVector& Vec ),
	operator*( const GsVector& Vec, double sc );

EXPORT_CPP_GSBASE GsVector
	operator/( const GsVector& Vec1, const GsVector& Vec2 ),
	operator/( double sc, const GsVector& Vec ),
	operator/( const GsVector& Vec, double sc );

EXPORT_CPP_GSBASE GsVector
	operator+( const GsVector& Vec1, const GsVector& Vec2 ),
	operator+( double sc, const GsVector& Vec ),
	operator+( const GsVector& Vec, double sc );

EXPORT_CPP_GSBASE GsVector
	operator-( const GsVector& Vec1, const GsVector& Vec2 ),
	operator-( double sc, const GsVector& Vec ),
	operator-( const GsVector& Vec, double sc );

EXPORT_CPP_GSBASE CC_OSTREAM& operator << ( CC_OSTREAM& o, const GsVector& v );	

CC_END_NAMESPACE

#endif

