/****************************************************************
**
**	gscore/GsTensor.h	- GsTensor class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsTensor.h,v 1.30 2001/11/27 22:41:09 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSTENSOR_H )
#define IN_GSCORE_GSTENSOR_H

#include <gscore/base.h>
#include <gscore/types.h>
#include <gscore/GsUnsignedVector.h>
#include <gscore/GsDblBuffer.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

class GsDtTensor;



/****************************************************************
** Class	   : GsTensor 
** Description : A ref counted multi-dimensional array of doubles.
****************************************************************/

class EXPORT_CLASS_GSBASE GsTensor 
{
public:
	typedef GsUnsignedVector	index;

	typedef double*				iterator;
	typedef const double*		const_iterator;

	typedef double&				reference;

	typedef GsDtTensor GsDtType;

	GsTensor();

	GsTensor( const index& Dimension, double InitValue = 0.0 );
	GsTensor( const index& Dimension, GsUninitialized );

	GsTensor( double Value );
	GsTensor( const GsVector &Vector );
	GsTensor( const GsMatrix &Matrix );
	
	GsTensor( const GsTensor& Tensor );

    // pass in dimensions via variable arguments
    GsTensor( const double *Data, size_t NDim, ... );

	~GsTensor();
    GsTensor&	operator=( const GsTensor& Rhs );

	void			makeWritable()	{ m_Buffer.cow( size()); }

	const index&	dim()		const { return m_Dim; 		}
	size_t			size()		const;
	size_t			capacity()	const { return size();		}

	// Change the size
	void resize( const index& NewDim, double FillValue = 0.0 )
	{
		resize( NewDim, FillValue, true );
	}
	void resize( const index& NewDim, GsUninitialized Uninit )
	{
		resize( NewDim, 0.0, false );
	}


	iterator		begin()				{ makeWritable(); return Data(); }
	const_iterator	begin() const		{ return Data(); }
	iterator		end()				{ makeWritable(); return Data() + size(); }
	const_iterator	end() const			{ return Data() + size(); }

	// Always range check
	double&		at( const index& Index )		{ makeWritable(); return Data()[ elemOffsetRange( Index ) ]; }
	double		at( const index& Index ) const	{ return Data()[ elemOffsetRange( Index ) ]; }

	// Range check ifdef GS_RANGE_CHECK
	double&		operator()( const index& Index );
	double		operator()( const index& Index ) const;

	// Versions that construct an index and use it in the versions above.

	double &operator()( size_t Index0 ); 
	double  operator()( size_t Index0 ) const;
	double &operator()( size_t Index0, size_t Index1 );
	double  operator()( size_t Index0, size_t Index1 ) const;
	double &operator()( size_t Index0, size_t Index1, size_t Index2 );
	double  operator()( size_t Index0, size_t Index1, size_t Index2 ) const;

	// Arith with constant (applied elmentwise).
	GsTensor&	operator+=( double Rhs );
	GsTensor&	operator-=( double Rhs );
	GsTensor&	operator*=( double Rhs );
	GsTensor&	operator/=( double Rhs );

	// FIX - Get rid of all of these.  Having various arbitrary
	// combinations of this sort of functionality on tensor, 
	// matrix, and vector is hard to maintain, and the thing
	// you want is never there.  Then, of course, only some subset
	// of these show up at the addin layer.  Use iterators outside
	// the class instead, and elem function mapping APIs from slang.
	GsTensor tensorMax( double rhs ) const;
	GsTensor tensorMax( const GsTensor &rsh ) const;
	GsTensor tensorMin( double rhs          ) const;
	GsTensor tensorMin( const GsTensor &rsh ) const;
	GsTensor tensorAbs(  					) const;
	GsTensor tensorBinaryMax( double rhs    ) const;
	GsTensor tensorBinaryMin( double rhs    ) const;

	// Element-by-element multiplication, division, addition, and subtraction
	GsTensor&	operator*=( const GsTensor &Rhs );
	GsTensor&	operator/=( const GsTensor &Rhs );
	GsTensor&	operator+=( const GsTensor &Rhs );
	GsTensor&	operator-=( const GsTensor &Rhs );

	// Elementwise operations of various kinds.  Result always goes in *this.
	GsTensor&	LogAssign();
	GsTensor&	ExpAssign();
	GsTensor&	MinAssign( const GsTensor &rhs );
	GsTensor&	MaxAssign( const GsTensor &rhs );	

	// In range?
	bool inRange( const index &Index ) const;

	// Compute offset (...Range checks and throws exception)
	size_t	elemOffset( const index& Index ) const;
	size_t	elemOffsetRange( const index& Index ) const;

	// Reduce along a dimension.

	void reduceSum( 			size_t ReduceDim, GsTensor &Result ) const;
	void reduceProduct( 		size_t ReduceDim, GsTensor &Result ) const;
	void reduceMin( 			size_t ReduceDim, GsTensor &Result ) const;
	void reduceMax( 			size_t ReduceDim, GsTensor &Result ) const;
	void reduceSumOfSquares( 	size_t ReduceDim, GsTensor &Result ) const;

	// Get the vector given by two indices Position1 <= Position2 that differ in (up to) one dimension
	GsVector getVector( const index &Position1,	const index &Position2) const;

	// Equality
	bool operator==( const GsTensor &rhs ) const;

	// Strided references 

	typedef GsDblBuffer::StridedRef StridedRef;
	typedef GsDblBuffer::StridedRef::iterator strided_iterator;

	typedef GsDblBuffer::ConstStridedRef ConstStridedRef;
	typedef GsDblBuffer::ConstStridedRef::const_iterator const_strided_iterator;

	// Reference to an inner range.

	     StridedRef InnerRangeRef( const index &SubIndex );
	ConstStridedRef InnerRangeRef( const index &SubIndex ) const;

	// Reference to an outer range.

	     StridedRef OuterRangeRef( const index &SubIndex );
	ConstStridedRef OuterRangeRef( const index &SubIndex ) const;

	// Makes it easier for them to grab and share our buffer.
	friend class GsVector;
	friend class GsMatrix;

private:		

	index	m_Dim;

	GsDblBuffer	m_Buffer;

	double *Data() const { return m_Buffer.data(); }

	// Generate exceptions
	void errIndex( const index& Index ) const;
	void errSize( const char *OpName, const index& Index ) const;

	// Never range check
	double	&elemNoRange( const index& Index )		 { makeWritable(); return Data()[ elemOffset( Index ) ]; }
	double	 elemNoRange( const index& Index ) const { return Data()[ elemOffset( Index ) ]; }

    // common allocation function for several constructors
    void alloc(); 

	void resize( const index &NewDim, double FillValue, bool DoFill );

	// Helper function for resizing
	void ResizeCopyDataHelper( const index &OldDim, const index &NewDim, 
							   size_t OldNextLevelSize, size_t NewNextLevelSize, 
							   size_t CopyDim, 
							   const double *OldData, double *NewData, 
							   double FillValue, bool DoFill );


};


// Arith with constant lhs (applied elmentwise).
EXPORT_CPP_GSBASE GsTensor operator+( double Lhs, const GsTensor &Rhs );
EXPORT_CPP_GSBASE GsTensor operator-( double Lhs, const GsTensor &Rhs );
EXPORT_CPP_GSBASE GsTensor operator*( double Lhs, const GsTensor &Rhs );
EXPORT_CPP_GSBASE GsTensor operator/( double Lhs, const GsTensor &Rhs );

#ifdef GS_RANGE_CHECK
inline double& GsTensor::operator()( const index& Index )
{
	return elemNoRange( Index );
}

inline double GsTensor::operator()( const index& Index ) const
{
	return elemNoRange( Index );
}
#else
inline double& GsTensor::operator()( const index& Index )
{
	return at( Index );
}

inline double GsTensor::operator()( const index& Index ) const
{
	return at( Index );
}
#endif

// Versions that give more convenient access for smaller cases.
inline double & GsTensor::operator()( size_t Index0 ) 
{
	index	Index( 1, Index0 );
	
	return	(*this)(Index);
}

inline double GsTensor::operator()( size_t Index0 ) const
{
	index	Index( 1, Index0 );

	return	(*this)(Index);
}

inline double & GsTensor::operator()( size_t Index0, size_t Index1 ) 
{
	index	Index( 2, GsUninitialized() );

	Index[ 0 ] = Index0;
	Index[ 1 ] = Index1;
	
	return	(*this)(Index);
}

inline double GsTensor::operator()( size_t Index0, size_t Index1 ) const
{
	index	Index( 2, GsUninitialized() );

	Index[ 0 ] = Index0;
	Index[ 1 ] = Index1;
	
	return	(*this)(Index);
}

inline double & GsTensor::operator()( size_t Index0, size_t Index1, size_t Index2 )
{
	index	Index( 3, GsUninitialized() );

	Index[ 0 ] = Index0;
	Index[ 1 ] = Index1;
	Index[ 2 ] = Index2;
	
	return	(*this)(Index);
}

inline double GsTensor::operator()( size_t Index0, size_t Index1, size_t Index2 ) const
{
	index	Index( 3, GsUninitialized() );

	Index[ 0 ] = Index0;
	Index[ 1 ] = Index1;
	Index[ 2 ] = Index2;
	
	return	(*this)(Index);
}



CC_END_NAMESPACE

#endif

