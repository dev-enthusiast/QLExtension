/****************************************************************
**
**	GSVECTORMASK.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsVectorMask.h,v 1.2 2000/05/16 00:24:04 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSVECTORMASK_H )
#define IN_GSQUANT_GSVECTORMASK_H

#include <gsquant/base.h>
#include <gscore/GsUnsignedVector.h>
#include <gscore/GsDateVector.h>

CC_BEGIN_NAMESPACE( Gs )

template< class VT > VT GsVectorMaskCompressTemplate( const VT &Input, const GsUnsignedVector &Mask );
template< class VT > VT GsVectorMaskExpandTemplate(   const VT &Input, const GsUnsignedVector &Mask, const VT &Defaults );


EXPORT_CPP_GSQUANT GsVector GsVectorMaskCompress( const GsVector &Input, const GsUnsignedVector &Mask );
EXPORT_CPP_GSQUANT GsVector GsVectorMaskExpand(   const GsVector &Input, const GsUnsignedVector &Mask, const GsVector &Defaults );

EXPORT_CPP_GSQUANT GsStringVector GsStringVectorMaskCompress( const GsStringVector &Input, const GsUnsignedVector &Mask );
EXPORT_CPP_GSQUANT GsStringVector GsStringVectorMaskExpand(   const GsStringVector &Input, const GsUnsignedVector &Mask, const GsStringVector &Defaults );

EXPORT_CPP_GSQUANT GsDateVector GsDateVectorMaskCompress( const GsDateVector &Input, const GsUnsignedVector &Mask );
EXPORT_CPP_GSQUANT GsDateVector GsDateVectorMaskExpand(   const GsDateVector &Input, const GsUnsignedVector &Mask, const GsDateVector &Defaults );

CC_END_NAMESPACE

#endif 
