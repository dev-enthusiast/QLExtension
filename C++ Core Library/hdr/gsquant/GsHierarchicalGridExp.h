/****************************************************************
**
**	GsHierarchicalGridExp.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_GSHIERARCHICALGRIDEXP_H )
#define IN_GSHIERARCHICALGRIDEXP_H

#include <gsquant/GsHierarchicalGrid.h>
#include <gsdt/GsDtArray.h>
#include <gsquant/GsFuncSpecial.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsHierarchicalGrid GsHierarchicalGridCreate(
	const GsHierarchy& levels,		// the hierarchy
	const GsDtArray&   layers		// the Gs1DLayers array
);

EXPORT_CPP_GSQUANT int GsHierarchicalGrid_Size(
	const GsHierarchicalGrid&	grid	// the grid
);

EXPORT_CPP_GSQUANT int GsHierarchicalGrid_DimensionSize(
	const GsHierarchicalGrid&	grid,		// the grid
	int							dimension	// the dimension
);

EXPORT_CPP_GSQUANT int GsHierarchicalGrid_Dimension(
	const GsHierarchicalGrid&	grid	// the grid
);

EXPORT_CPP_GSQUANT GsHierarchy GsHierarchicalGrid_Hierarchy(
	const GsHierarchicalGrid&	grid	// the grid
);

EXPORT_CPP_GSQUANT Gs1DLayers GsHierarchicalGrid_1DLayers(
	const GsHierarchicalGrid&	grid,	// the grid
	int							dim		// the dimension
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_GetLevelCoords(
	const GsHierarchicalGrid&	grid,	// the grid
	int							level,	// the level
	int							dim		// the dimension
);

EXPORT_CPP_GSQUANT GsTensor GsHierarchicalGrid_GetLevelGrid(
	const GsHierarchicalGrid&	grid,	// the grid
	const GsVector&				level,	// the grid level
	const GsVector&				GridFn	// the hierarchical grid function
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_AddLevelGrid(
	const GsHierarchicalGrid&	grid,		// the grid
	const GsVector&				level,		// the grid level
	const GsTensor&				LevelGrid,	// to be added to grid function
	const GsVector&				GridFn		// the hierarchical grid function
);

EXPORT_CPP_GSQUANT GsDtArray GsHierarchicalGrid_GetRows(
	const GsHierarchicalGrid& grid	// the hierarchical grid
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_HierarchicalTransform(
	const GsHierarchicalGrid& grid,		 // the hierarchical grid
	int						  dimension, // direction in which to transform
	const GsVector&           Values	 // vector to transform
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_InvHierarchicalTransform(
	const GsHierarchicalGrid& grid,		 // the hierarchical grid
	int						  dimension, // direction in which to transform
	const GsVector&           Values	 // vector to transform
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_FirstDiff(
	const GsHierarchicalGrid& grid,		 // the hierarchical grid
	int						  dimension, // direction in which to differentiate
	double					  range, 	 // range of grid
	const GsVector&           Values	 // vector to transform
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_SecondDiff(
	const GsHierarchicalGrid& grid,		 // the hierarchical grid
	int						  dimension, // direction in which to differentiate
	double					  range, 	 // range of grid
	const GsVector&           Values	 // vector to transform
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_Set1DValues(
	const GsHierarchicalGrid& 		   grid,		// the hierarchical grid
	const GsVector& 				   f,		 	// the function of one dimension
	int						  		   dimension 	// direction in which the values vary
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_SetValues(
	const GsFuncVecToScFunc&					   f,			// the function
	const GsHierarchicalGrid& 		   			   grid,		// the hierarchical grid
	const GsVector&								   MinRange, 	// the lower grid boundary
	const GsVector&				   				   MaxRange 	// the upper grid boundary
);

EXPORT_CPP_GSQUANT GsTensor GsHierarchicalGrid_ToFullGrid(
	const GsHierarchicalGrid&	grid,		// the hierarchical grid
	const GsVector&				Values		// the grid values
);

EXPORT_CPP_GSQUANT GsVector GsHierarchicalGrid_FromFullGrid(
	const GsHierarchicalGrid&	grid,			// the hierarchical grid
	const GsTensor&				FullGridVals	// the values on the full grid
);

CC_END_NAMESPACE

#endif

