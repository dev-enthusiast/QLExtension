/***********************************************************************
**
**	dist.h - interface for distribution object
**
**  Copyright 1998 - Goldman Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
***********************************************************************/

#if !defined( IN_DIST_H )
#define IN_DIST_H

#include <fiblack/base.h>

typedef enum dist_type_tag {
	DIST_TYPE_ERROR = -1,
	DIST_LOGNORMAL,
	DIST_NORMAL,
	DIST_SHIFTED_LOGNORMAL,
	DIST_NORMAL_ABSORBING
} DIST_TYPE;

typedef enum dist_field_tag {
	DIST_FIELD_ERROR = -1,
	DIST_BOUNDARY,
	DIST_SHIFT
} DIST_FIELD;

typedef struct dist_tag * DIST;

EXPORT_C_FIBLACK DIST	DistLognormalNew	( void );
EXPORT_C_FIBLACK DIST	DistNormalNew		( void );
EXPORT_C_FIBLACK DIST	DistShiftedLognormalNew
											( double boundary,
											  double shift );
EXPORT_C_FIBLACK DIST	DistNormalAbsorbingNew
											( double boundary );

EXPORT_C_FIBLACK void	DistFree			( DIST *d );
EXPORT_C_FIBLACK DIST_TYPE
						DistType			( DIST d );
EXPORT_C_FIBLACK double	DistGetParam		( DIST d, DIST_FIELD key );

#endif


