/****************************************************************
**
**	Gs1DLayersExp.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_GS1DLAYERSEXP_H )
#define IN_GS1DLAYERSEXP_H

#include <gscore/types.h>
#include <gsquant/Gs1DLayers.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT Gs1DLayers Gs1DLayersCreate(
	const GsVector& CoordValues	// the points on the grid
);

EXPORT_CPP_GSQUANT int Gs1DLayers_Size(
	const Gs1DLayers& layers	// the grid
);

EXPORT_CPP_GSQUANT int Gs1DLayers_LevelSize(
	const Gs1DLayers& layers,	// the grid
	int 			  level		// the required level
);

EXPORT_CPP_GSQUANT GsVector Gs1DLayers_CoordValues(
	const Gs1DLayers& layers	// the grid
);

EXPORT_CPP_GSQUANT GsVector Gs1DLayers_LevelCoordValues(
	const Gs1DLayers& layers,	// the grid
	int				  level		// the required level
);

EXPORT_CPP_GSQUANT int Gs1DLayers_MaxLevel(
	const Gs1DLayers& layers	// the grid
);

EXPORT_CPP_GSQUANT Gs1DLayers Gs1DLayers_AddBisections(
	const Gs1DLayers& layers,	// the grid
	int				  nLayers	// number of layers to add
);

#if 0
EXPORT_CPP_GSQUANT GsVector Gs1DLayers_IndexVector(
	const Gs1DLayers& layers	// the grid
);

EXPORT_CPP_GSQUANT GsVector Gs1DLayers_LevelVector(
	const Gs1DLayers& layers	// the grid
);
#endif

CC_END_NAMESPACE

#endif

