/****************************************************************
**
**	gscore/GsDoubleMatrix.h	- Header for GsDouble
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSDOUBLEMATRIX_H)
#define IN_GSCORE_GSDOUBLEMATRIX_H

#include <gscore/GsDouble.h>
#include <gscore/GsDoubleVector.h>
#include <gscore/GsMatrix.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Classes declared in this header
*/

class GsDoubleMatrix;


/*
**  class GsDoubleMatrix
*/

class EXPORT_CLASS_GSBASE GsDoubleMatrix
{
public:
	typedef GsDouble index_type;
	typedef size_t size_type;

	// Iterator and reference come right out of the matrix inside.
	typedef GsMatrix::iterator iterator;
	typedef GsMatrix::const_iterator const_iterator;
	typedef GsMatrix::reference reference;		

	// Constructors
	GsDoubleMatrix() : m_IndexVector(), m_DataMatrix() {}

	GsDoubleMatrix( const GsDoubleVector &rhs, size_t Cols = 0 ) : m_IndexVector( rhs ), m_DataMatrix( rhs.size(), Cols ) {}

	GsDoubleMatrix( const GsDoubleMatrix &rhs ) : m_IndexVector( rhs.m_IndexVector ), m_DataMatrix( rhs.m_DataMatrix ) {}

	// Destructor
	~GsDoubleMatrix() {};

	// Assignment
	GsDoubleMatrix &operator=(const GsDoubleMatrix&);

	// Element Access
	reference operator() ( index_type Row, size_t Col );
	double operator() ( index_type Row, size_t Col ) const;

	// Iterators
	iterator		begin()				{ return m_DataMatrix.begin(); }
	const_iterator	begin() const		{ return m_DataMatrix.begin(); }
	iterator		end()				{ return m_DataMatrix.end();   }
	const_iterator	end() const			{ return m_DataMatrix.end();   }

	// Get the vector of row indices.
	const GsDoubleVector &RowIndices() const { return m_IndexVector; }

private:

	GsDoubleVector
		m_IndexVector;

	GsMatrix
		m_DataMatrix;

	// Convert an index to a row number in m_DataMatrix;
	size_t IndexToRowNum( index_type Row ) const;
	
};	



/****************************************************************
**	Class  : GsDoubleMatrix 
**	Routine: operator() 
**	Returns: Reference to element.
**	Action : Index into the array.
****************************************************************/

inline GsDoubleMatrix::reference GsDoubleMatrix::operator()(
	index_type Row,
	size_t Col
)
{
	size_t	RowNum = IndexToRowNum( Row );

	return m_DataMatrix( RowNum, Col ); 
}



/****************************************************************
**	Class  : GsDoubleMatrix 
**	Routine: operator() 
**	Returns: value of an element.
**	Action : Index into the array.
****************************************************************/

inline double GsDoubleMatrix::operator()(
	index_type Row,
	size_t Col
) const
{
	size_t	RowNum = IndexToRowNum( Row );

	return m_DataMatrix( RowNum, Col ); 
}



CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSDOUBLEMATRIX_H) */

