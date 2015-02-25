/****************************************************************
**
**	DATEUTIL.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/DateUtil.h,v 1.2 2000/03/10 16:02:40 lischf Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_DATEUTIL_H )
#define IN_GSQUANT_DATEUTIL_H

#include <gsquant/base.h>
#include <gscore/GsDateVector.h>

CC_BEGIN_NAMESPACE( Gs )

// This non-addin function is exported for use by other DLLs.

EXPORT_CPP_GSQUANT unsigned PolyMergeDatesNoDuplicates( GsDateVector::const_iterator *Current, GsDateVector::const_iterator *End, unsigned NumInputs, GsDateVector::iterator Result );

EXPORT_CPP_GSQUANT unsigned SortDatesNoDuplicates( GsDateVector::const_iterator *Start, GsDateVector::const_iterator *End );


EXPORT_CPP_GSQUANT GsDateVector *GsDateVectorMergeWithNoDuplicates( const GsDateVector &V0, const GsDateVector &V1, const GsDateVector &V2, const GsDateVector &V3 );

EXPORT_CPP_GSQUANT GsDateVector *GsDateVectorSortWithNoDuplicates( const GsDateVector &Dates );

CC_END_NAMESPACE

#endif 
