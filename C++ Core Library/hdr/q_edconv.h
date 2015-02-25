/****************************************************************
**
**	q_edconv.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_Q_EDCONV_H )
#define IN_Q_EDCONV_H
#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif


typedef enum {
	Q_EDCONV_NONE = 0,
	Q_EDCONV_HO_LEE,
	Q_EDCONV_HULL_WHITE
} Q_EDCONVEXITY_TYPE ;

#define Q_EDCONVEXITY_TYPE_MAX = 2; // Increase this when you add more to the above

EXPORT_C_QBASIC double Q_EDConvexityHoLee(
	double stddev, 
	double t,      
	double tt
);
									   
EXPORT_C_QBASIC double Q_EDConvexityHullWhite(
	double stddev, 
	double meanrev,
	double t,      
	double tt     
);
									   
#endif

