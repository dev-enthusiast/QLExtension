/* $Header: /home/cvs/src/gsbase/src/gscore/GsMatrix.h,v 1.32 2004/12/01 17:04:22 khodod Exp $ */
/****************************************************************
**
**	gscore/GsMatrix.h	- GsMatrix class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsMatrix.h,v 1.32 2004/12/01 17:04:22 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_MATRIX_H )
#define IN_GSCORE_MATRIX_H

#include <gscore/base.h>
#include <gscore/types.h>
#include <gscore/GsDblBuffer.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	GsMatrix is a matrix of doubles
*/

class GsVector;
class GsTensor;

// Must forward declare to get correct linkage on friend statement
//
//EXPORT_CPP_GSBASE void swap( GsMatrix& MatA, GsMatrix& MatB );
//EXPORT_CPP_GSBASE GsMatrix product( const GsMatrix& MatA, 
//									const GsMatrix& MatB );
//EXPORT_CPP_GSBASE GsVector product( const GsMatrix& Mat,  
//									const GsVector& Vec );
//EXPORT_CPP_GSBASE GsVector product( const GsVector& Vec,  
//									const GsMatrix& Mat );


class EXPORT_CLASS_GSBASE GsMatrix
{
public:
	typedef class GsDtMatrix	GsDtType;
	typedef double*				iterator;
	typedef const double*		const_iterator;

	typedef double&				reference;

	enum INIT_STYLE
	{
		ROW_MAJOR,	// C-style, column index moves fastest
		COL_MAJOR	// Fortran-style, row index moves fastest
	};

	enum VEC_KIND
	{
		AS_ROW,		// Treat as an 1 x N matrix
		AS_COL		// Treat as an N x 1 matrix
	};

	GsMatrix();
	GsMatrix( size_t Rows, size_t Cols, double InitValue = 0.0 );
	GsMatrix( size_t Rows, size_t Cols, GsUninitialized );
	GsMatrix( size_t Rows, size_t Cols, const double *InitValues, INIT_STYLE InitStyle=ROW_MAJOR );
	GsMatrix( const GsVector& Vec, VEC_KIND Kind );
    GsMatrix( const GsMatrix& Mat, size_t Row, size_t Col, size_t N_rows, size_t N_cols );

	// Works only if the tensor is two-dimensional.  Shares the same underlying GsDblBuffer.
	explicit GsMatrix( const GsTensor &Tensor );

	// Copy constructor, destructor and default assignment operator are not needed to be defined

	//GsMatrix( const GsMatrix& Mat );

	//~GsMatrix();
    //GsMatrix&	operator=( const GsMatrix& Rhs );

	size_t			rows() const	{ return m_Rows; }
	size_t			cols() const	{ return m_Cols; }
	size_t			size() const	{ return m_Cols * m_Rows; }
	size_t			capacity() const{ return size(); }

	const double*	data() const	{ return Data(); }

	void			makeWritable()	{ m_Buffer.cow( size()); }

	void resize( size_t NewRows, size_t NewCols, double FillValue = 0.0 )
	{
		resize( NewRows, NewCols, FillValue, true );
	}
	void resize( size_t NewRows, size_t NewCols, GsUninitialized )
	{
		resize( NewRows, NewCols, 0.0, false );
	}


	iterator		begin()				{ makeWritable(); return Data(); }
	const_iterator	begin() const		{ return Data(); }
	iterator		end()				{ makeWritable(); return Data() + size(); }
	const_iterator	end() const			{ return Data() + size(); }


	// Quick range check
	bool		inRange( size_t Row, size_t Col ) const { return Row < m_Rows && Col < m_Cols; }

	// Always range check
	double&		at( size_t Row, size_t Col );
	double		at( size_t Row, size_t Col ) const;

	// Range check ifdef GS_RANGE_CHECK
	double&		operator()( size_t Row, size_t Col );
	double		operator()( size_t Row, size_t Col ) const;
	void		elemSet( size_t Row, size_t Col, double Value ) { (*this)( Row, Col ) = Value; }
	
	GsMatrix&	operator+=( double Rhs );
	GsMatrix&	operator-=( double Rhs );
	GsMatrix&	operator*=( double Rhs );
	GsMatrix&	operator/=( double Rhs );

	// Element-by-element multiplication, division, addition, and subtraction
	GsMatrix&	operator*=( const GsMatrix &Rhs );
	GsMatrix&	operator/=( const GsMatrix &Rhs );
	GsMatrix&	operator+=( const GsMatrix &Rhs );
	GsMatrix&	operator-=( const GsMatrix &Rhs );

	// Generate related exceptions
	void		errIndex( size_t Row, size_t Col ) const;
	void		errSize( const char *OpName, size_t Rows, size_t Cols ) const;

	typedef GsDblBuffer::StridedRef StridedRef;
	typedef GsDblBuffer::StridedRef::iterator strided_iterator;

	typedef GsDblBuffer::ConstStridedRef ConstStridedRef;
	typedef GsDblBuffer::ConstStridedRef::const_iterator const_strided_iterator;

	StridedRef RowRef(  size_t RowNum  );
	StridedRef ColRef(  size_t ColNum  );
	StridedRef DiagRef( int    DiagNum );

	ConstStridedRef RowRef(  size_t RowNum  ) const;
	ConstStridedRef ColRef(  size_t ColNum  ) const;
	ConstStridedRef DiagRef( int 	DiagNum ) const;

	// Equality
	bool operator==( const GsMatrix &rhs ) const;

	// Friends
	friend EXPORT_CPP_GSBASE void		swap( GsMatrix& MatA, GsMatrix& MatB );
	friend EXPORT_CPP_GSBASE GsMatrix	product( const GsMatrix& MatA, const GsMatrix& MatB );
	friend EXPORT_CPP_GSBASE GsVector	product( const GsMatrix& Mat,  const GsVector& Vec );
	friend EXPORT_CPP_GSBASE GsVector	product( const GsVector& Vec,  const GsMatrix& Mat );

private:

	size_t		m_Rows,
				m_Cols;

	GsDblBuffer	m_Buffer;

	double	*	Data()	const							{ return m_Buffer.data(); }

	// Never range check
	double	&elemNoRange( size_t Row, size_t Col )		{ makeWritable(); return Data()[ Row * m_Cols + Col ]; }
	double	elemNoRange( size_t Row, size_t Col ) const	{ return Data()[ Row * m_Cols + Col ]; }

	void	resize( size_t NewRows, size_t NewCols, double FillValue, bool DoFill );

	friend class GsTensor;
};

// operator +
inline GsMatrix operator+( const GsMatrix& Lhs, double Rhs )
{
	GsMatrix m = Lhs;
	m.makeWritable();
	return m += Rhs;
}
inline GsMatrix operator+( double Lhs, const GsMatrix& Rhs )
{
	GsMatrix m = Rhs;
	m.makeWritable();
	return m += Lhs;
}
inline GsMatrix operator+( const GsMatrix& Lhs, const GsMatrix& Rhs )
{
	GsMatrix m = Lhs;
	m.makeWritable();
	return m += Rhs;
}

// operator *
inline GsMatrix operator*( const GsMatrix& Lhs, double Rhs )
{
	GsMatrix m = Lhs;
	m.makeWritable();
	return m *= Rhs;
}
inline GsMatrix operator*( double Lhs, const GsMatrix& Rhs )
{
	GsMatrix m = Rhs;
	m.makeWritable();
	return m *= Lhs;
}
inline GsMatrix operator*( const GsMatrix& Lhs, const GsMatrix& Rhs )
{
	GsMatrix m = Lhs;
	m.makeWritable();
	return m *= Rhs;
}

// operator -
inline GsMatrix operator-( const GsMatrix& Lhs, double Rhs )
{
	GsMatrix m = Lhs;
	m.makeWritable();
	return m -= Rhs;
}
inline GsMatrix operator-( double Lhs, const GsMatrix& Rhs )
{
	GsMatrix m = Rhs*(-1);
	return m += Lhs;
}
inline GsMatrix operator-( const GsMatrix& Lhs, const GsMatrix& Rhs )
{
	GsMatrix m = Lhs;
	m.makeWritable();
	return m -= Rhs;
}

// operator /
inline GsMatrix operator/( const GsMatrix& Lhs, double Rhs )
{
	GsMatrix m = Lhs;
	m.makeWritable();
	return m /= Rhs;
}
inline GsMatrix operator/( const GsMatrix& Lhs, const GsMatrix& Rhs )
{
	GsMatrix m = Lhs;
	m.makeWritable();
	return m /= Rhs;
}
inline GsMatrix operator/( double Lhs, const GsMatrix& Rhs )
{
	GsMatrix m( Rhs.rows(), Rhs.cols(), Lhs );
	return m /= Rhs;
}


#ifndef GS_RANGE_CHECK
inline double& GsMatrix::operator()( size_t Row, size_t Col )
{
	return elemNoRange( Row, Col );
}

inline double GsMatrix::operator()( size_t Row, size_t Col ) const
{
	return elemNoRange( Row, Col );
}
#else
inline double& GsMatrix::operator()( size_t Row, size_t Col )
{
	return at( Row, Col );
}

inline double GsMatrix::operator()( size_t Row, size_t Col ) const
{
	return at( Row, Col );
}
#endif

CC_END_NAMESPACE

#endif

