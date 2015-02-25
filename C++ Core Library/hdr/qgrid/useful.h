/****************************************************************
**
**	useful.h	- useful functions
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_USEFUL_H )
#define IN_USEFUL_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<qenums.h>

CC_BEGIN_NAMESPACE( Gs )

int BinarySearchL(
	const double*	Times,		// increasing vector of doubles
	int				iStart,		// lowest index of array
	int				iEnd,		// largest index of array
	double			t			// value to find in array
);

int BinarySearchLE(
	const double*	Times,		// increasing vector of doubles
	int				iStart,		// lowest index of array
	int				iEnd,		// largest index of array
	double			t			// value to find in array
);


#if 0
template< class T > class c_array
{
	public:
		c_array() { m_ptr = 0; }
		c_array( int n ) : m_ptr( new T[ n ] ) {}
		~c_array() { delete[] m_ptr; }
        void resize( int n ) { delete[] m_ptr; alloc( n ); }
		operator T*() { return m_ptr; }
		operator const T*() const{ return m_ptr; }
	private:
		const c_array< T >& operator=( const c_array& rhs ) {}
		T*	m_ptr;
};

//typedef c_array< double > DoubleArray;

#endif

CC_END_NAMESPACE
#endif

