/****************************************************************
**
**	N_LAYERS.H
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_N_LAYERS_H )
#define IN_N_LAYERS_H

#if !defined( IN_DATE_H )
#include <date.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#include <mempool.h>

typedef struct
{
	double		MinGrid;
	double		MaxGrid;
	int			nSteps;
	double		DeltaX;
	double*		Values;
} SimpleGrid;

typedef struct
{
	int		nValues;			// the grid of values
	double*	Values;
	int		Level;
} HierarchyLayer;

typedef struct
{
	double*			Values;
	int				nValues;
	HierarchyLayer*	Layers;
	int				nLayerSteps;		// = ( nFullLayers - 1 ) * 2 ^ Hierarchy + nFullLayers
	int				Hierarchy;
	int				nGridSteps;
	double			LogMinGrid;
	double			LogMaxGrid;
	double			MinLayers;
	double			MaxLayers;
	MEM_POOL*		MemPool;
} HierarchicalLayerSpace;

EXPORT_C_NUM int N_CreateLayerSpace(
	HierarchicalLayerSpace*		Grid,			// pointer to the Layered grid to be created
	int							nGridSteps,		// number of grid steps (a power of 2 is best)
	int							nLayerSteps,	// number of steps in layer space (should be a multiple of 2^Hierarchy)
	double						LogMinGrid,		// minimum of grid range
	double						LogMaxGrid,		// maximum of grid range
	double						MinLayers,		// minimum of layer range
	double						MaxLayers,		// maximum of layer range
	int							Hierarchy,		// hierarchy
	MEM_POOL*					MemPool			// set to NULL if not used
);

EXPORT_C_NUM int N_SkewLayers(
	HierarchicalLayerSpace*			OutGrid,				// pointer to the outputted layers 
	HierarchicalLayerSpace*			InGrid,					// pointer to the inputted layers
	double*							Temp1,					// temp storage vector, of size at least Grid.nLayers
	double*							Temp2,					// temp storage vector, of size at least nNewLayers
	double*							Temp3,					// temp storage vector, of size at least nNewLayers
	double*							Temp4,					// temp storage vector, of size at least Grid.nLayers
	double*							Temp5					// temp storage vector, of size at least Grid.nLayers
);

EXPORT_C_NUM int N_NodalToHierarchicalLayers(
	HierarchicalLayerSpace*		Grid	// the Hierarchical Layer Space
);

EXPORT_C_NUM int N_HierarchicalToNodalLayers(
	HierarchicalLayerSpace*		Grid	// the Hierarchical Layer Space
);

#endif

