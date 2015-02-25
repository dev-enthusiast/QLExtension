/****************************************************************
**
**	GsHierarchyExp.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_GSHIERARCHYEXP_H )
#define IN_GSHIERARCHYEXP_H

#include <gscore/types.h>
#include <gsquant/GsHierarchy.h>
				   
CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsHierarchy GsHierarchyCreate(
	int dimension	// dimension of hierarchy
);

EXPORT_CPP_GSQUANT int GsHierarchy_Dimension(
	const GsHierarchy& hierarchy	// the GsHierarchy
);

EXPORT_CPP_GSQUANT int GsHierarchy_nLevels(
	const GsHierarchy& hierarchy	// the GsHierarchy
);

EXPORT_CPP_GSQUANT GsHierarchy GsHierarchy_AddLevel(
	const GsHierarchy& hierarchy,	// the GsHierarchy
	const GsVector&	   levels  		// the levels to be added
);

EXPORT_CPP_GSQUANT GsHierarchy GsHierarchy_AddSparseLevels(
	const GsHierarchy& hierarchy,	// the GsHierarchy
	int				   level		// level of sparse grid
);

EXPORT_CPP_GSQUANT int GsHierarchy_GetIndex(
	const GsHierarchy& hierarchy,	// the GsHierarchy
	const GsVector&	   levels  		// the levels to be looked at
);

EXPORT_CPP_GSQUANT GsHierarchy_iterator GsHierarchy_Iterator(
	const GsHierarchy& hierarchy,	// the GsHierarchy
	const GsVector&	   levels  		// the level of the iterator
);

EXPORT_CPP_GSQUANT GsHierarchy_iterator GsHierarchyIterator_Next(
	const GsHierarchy_iterator& iter,	// the GsHierarchy
	int                			dim		// the index to increment
);

EXPORT_CPP_GSQUANT GsHierarchy_iterator GsHierarchyIterator_Prev(
	const GsHierarchy_iterator& iter,	// the GsHierarchy
	int                			dim		// the index to increment
);

CC_END_NAMESPACE

#endif

