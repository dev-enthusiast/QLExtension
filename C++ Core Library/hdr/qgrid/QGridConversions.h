/****************************************************************
**
**	QGridConversions.h	- conversions between types
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_QGRIDCONVERSIONS_H )
#define IN_QGRIDCONVERSIONS_H

#include <gsdt/GsDtDictionary.h>
#include <qgrid/base.h>
#include <qenums.h>
#include <gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )

GsDtDictionary* Grid1DToGsDt(
	const GsVector& Spots,
	const GsVector& Values
);

CC_END_NAMESPACE

#endif

