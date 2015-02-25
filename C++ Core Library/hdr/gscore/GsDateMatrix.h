/****************************************************************
**
**	gscore/GsDateMatrix.h	- Header for GsDate
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSDATEMATRIX_H)
#define IN_GSCORE_GSDATEMATRIX_H

#include <gscore/GsDate.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsMatrix.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Classes declared in this header
*/

class GsDateMatrix;


/*
**  class GsDateMatrix
*/

class EXPORT_CLASS_GSDATE GsDateMatrix
{
public:
	typedef GsDate index_type;
	typedef size_t size_type;

	// Iterator and reference come right out of the matrix inside.
	typedef GsMatrix::iterator iterator;
	typedef GsMatrix::const_iterator const_iterator;
	typedef GsMatrix::reference reference;		

	// Constructors
	GsDateMatrix() : m_IndexVector(), m_DataMatrix() {}

	GsDateMatrix( const GsDateVector &rhs, size_t Cols = 0 ) : m_IndexVector( rhs ), m_DataMatrix( rhs.size(), Cols ) {}

	GsDateMatrix( const GsDateMatrix &rhs ) : m_IndexVector( rhs.m_IndexVector ), m_DataMatrix( rhs.m_DataMatrix ) {}

	// Destructor
	~GsDateMatrix() {};

	// Assignment
	GsDateMatrix &operator=(const GsDateMatrix&);

	// Element Access
	reference operator() ( index_type Row, size_t Col );
	double operator() ( index_type Row, size_t Col ) const;

	// Iterators
	iterator		begin()				{ return m_DataMatrix.begin(); }
	const_iterator	begin() const		{ return m_DataMatrix.begin(); }
	iterator		end()				{ return m_DataMatrix.end();   }
	const_iterator	end() const			{ return m_DataMatrix.end();   }

	// Get the vector of row indices.
	const GsDateVector &RowIndices() const { return m_IndexVector; }

private:

	GsDateVector
		m_IndexVector;

	GsMatrix
		m_DataMatrix;

	// Convert an index to a row number in m_DataMatrix;
	size_t IndexToRowNum( index_type Row ) const;
	
};	



/****************************************************************
**	Class  : GsDateMatrix 
**	Routine: operator() 
**	Returns: Reference to element.
**	Action : Index into the array.
****************************************************************/

inline GsDateMatrix::reference GsDateMatrix::operator()(
	index_type Row,
	size_t Col
)
{
	size_t	RowNum = IndexToRowNum( Row );

	return m_DataMatrix( RowNum, Col ); 
}



/****************************************************************
**	Class  : GsDateMatrix 
**	Routine: operator() 
**	Returns: value of an element.
**	Action : Index into the array.
****************************************************************/

inline double GsDateMatrix::operator()(
	index_type Row,
	size_t Col
) const
{
	size_t	RowNum = IndexToRowNum( Row );

	return m_DataMatrix( RowNum, Col ); 
}



CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSDATEMATRIX_H) */

