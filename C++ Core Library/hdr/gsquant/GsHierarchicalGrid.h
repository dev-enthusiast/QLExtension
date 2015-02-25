/****************************************************************
**
**	GsHierarchicalGrid.h	- Multidimensional hierarchical grid class
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.20 $
**
****************************************************************/

#if !defined( IN_GSHIERARCHICALGRID_H )
#define IN_GSHIERARCHICALGRID_H

#include <gsquant/base.h>
#include <gscore/types.h>
#include <gsquant/GsHierarchy.h>
#include <gsquant/Gs1DLayers.h>
#include <gsdt/GsDtArray.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
**
**	Class: GsHierarchicalGrid
**	
**	Represents a general Hierarchical grid (in particular -- sparse
**	grids), and allows you to perform operations of functions on
**	such a grid. The functions are just taken to be pointers to
**	doubles.
**
**	The class is constructed by providing the necessary hierarchy
**	of levels (using the GsHierarchy class) and the necessary
**	layers of one dimensional grids (using the Gs1DLayers class
**	-- one set of layers for each dimension.
**	
****************************************************************/

class EXPORT_CLASS_GSQUANT GsHierarchicalGrid
{
public:
	typedef CC_STL_VECTOR( int ) RowVector;

	// GsDt support
	typedef	GsDtGeneric<GsHierarchicalGrid> GsDtType;
	GsString		toString() const;
	inline static const char * typeName() { return "GsHierarchicalGrid"; }
	
	GsHierarchicalGrid( const GsHierarchy& levels, 
						const Gs1DLayers& layers );
	GsHierarchicalGrid( const GsHierarchy& levels, 
						const CC_STL_VECTOR( Gs1DLayers )& layers );
	virtual ~GsHierarchicalGrid();

	int size() const { return m_Size; }
	int size( int dim ) const { return m_Layers[ dim ].SubGridSize( m_Hierarchy.MaxLevels()[ dim ]); }
	int dimension() const { return m_dimension; }
	const GsHierarchy& Hierarchy() const { return m_Hierarchy; }
	const Gs1DLayers& GridLayers( int dim ) const { return m_Layers[ dim ]; }

	inline int GetLevelGridIndex( const CC_STL_VECTOR( int )& level ) const; 
	inline int GetLevelGridIndex( const GsHierarchy::iterator& iter ) const; 
	inline int GetLevelGridSize( const CC_STL_VECTOR( int )& level ) const; 
	inline int GetLevelGridSize( const GsHierarchy::iterator& iter ) const; 

	void GetLevelRowIndex(
		const CC_STL_VECTOR( int )& level,		// level of row
		int					  dim,				// direction of row
		const CC_STL_VECTOR( int )& indices,	// indices of row within level subgrid
		int&				  rowindex, 		// returned index
		int&				  stepsize  		// returned step size
	) const;

	void AddLevelGrid( const CC_STL_VECTOR( int )& level,  
					   const double* values, 
					   double* gridfn ) const;
	void AddLevelGrid( const GsHierarchy::iterator& iter,
					   const double* values, 
					   double* gridfn ) const;

	GsTensor ToFullGrid( const double* Values ) const;
	void FromFullGrid( const GsTensor& FullGridVals, double* Values ) const;

	void SetValues( const GsFuncHandle< const GsVector&, double >& f, const GsVector& MinRange, const GsVector& MaxRange, double* Values ) const;
	void SetValues( const double* f, int dim, double* Values ) const;

	GsDtArray GetRows() const;

	// virtual member functions
	virtual void HierarchicalTransform( int dim, double* Values ) const;
	virtual void InvHierarchicalTransform( int dim, double* Values ) const;
	virtual void FirstDiff( int dim, double Range, double* Values ) const;
	virtual void SecondDiff( int dim, double Range, double* Values ) const;

private:
	GsHierarchy	m_Hierarchy;
	CC_STL_VECTOR( Gs1DLayers ) m_Layers;

	int m_dimension;
	int m_Size;					// total number of grid points

	CC_STL_VECTOR( int ) m_GridSizes;  	// size of each subgrid
	CC_STL_VECTOR( int ) m_GridIndices;	// index of each subgrid

	CC_STL_VECTOR( CC_STL_VECTOR( RowVector )) m_RowVectors; // vectors of 1 dimensional rows

	void construct();
	void SetUpRows();
	int NextLevel( CC_STL_VECTOR( int )& level ) const;
	int CountRows( int dim ) const;
};

int GsHierarchicalGrid::GetLevelGridIndex( const CC_STL_VECTOR( int )& level ) const
{
	return m_GridIndices[ m_Hierarchy.GetIndex( &(*level.begin()) ) ];
}

int GsHierarchicalGrid::GetLevelGridIndex( const GsHierarchy::iterator& iter ) const 
{
	return m_GridIndices[ iter.ValueIndex() ];
}

int GsHierarchicalGrid::GetLevelGridSize( const CC_STL_VECTOR( int )& level ) const 
{
	return m_GridSizes[ m_Hierarchy.GetIndex( &(*level.begin()) ) ];
}

int GsHierarchicalGrid::GetLevelGridSize( const GsHierarchy::iterator& iter ) const 
{
	return m_GridSizes[ iter.ValueIndex() ];
}

CC_END_NAMESPACE

#endif


