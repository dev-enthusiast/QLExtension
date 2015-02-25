/****************************************************************
**
**	TYPECONSTRUCTORS.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltsecdb/src/ltsecdb/TypeConstructors.h,v 1.11 2001/11/27 23:01:11 procmon Exp $
**
****************************************************************/

#if !defined( IN_LTSECDB_TYPECONSTRUCTORS_H )
#define IN_LTSECDB_TYPECONSTRUCTORS_H

#include <portable.h>
#include <ltsecdb/base.h>
#include <gscore/GsString.h>
#include <gscore/GsDouble.h>
#include <gscore/GsDate.h>
#include <gscore/GsVector.h>
#include <gscore/GsMatrix.h>
#include <gscore/GsTensor.h>
#include <gscore/GsRDate.h>
#include <gscore/GsSymbol.h>
#include <gscore/GsStringVector.h>
#include <gscore/GsIntegerVector.h>
#include <gscore/GsUnsignedVector.h>
#include <gscore/GsDoubleVector.h>
#include <gscore/GsSymbolVector.h>
#include <gscore/GsVectorVector.h>
#include <gscore/GsRDateVector.h>
#include <gscore/GsDayCountVector.h>
#include <gscore/GsDayCountISDAVector.h>
#include <gscore/GsDayCount.h>
#include <gscore/GsDayCountISDA.h>
#include <gscore/GsDiscountCurve.h>
#include <gscore/GsVolatilityCurve.h>
#include <secdb/LtSecDbObj.h>
#include <gsdt/GsDtStlTraits.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_LTSECDB GsString		GsStringCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsDouble		GsDoubleCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsDate		GsDateCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsVector		GsVectorCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsMatrix		GsMatrixCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsDtArray	*GsArrayCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsTensor		GsTensorCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsDtDictionary	*GsDictionaryCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsRDate		GsRDateCtor( const GsDt &  );

EXPORT_CPP_LTSECDB GsStringVector GsStringVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsIntegerVector GsIntegerVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsUnsignedVector GsUnsignedVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsDoubleVector GsDoubleVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsVectorVector GsVectorVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsSymbolVector GsSymbolVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsRDateVector GsRDateVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsDayCountVector GsDayCountVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsDayCountISDAVector GsDayCountISDAVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsDateVector GsDateVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsSymbol GsSymbolCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsDayCount GsDayCountCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsDayCountISDA GsDayCountISDACtor( const GsDt& );

EXPORT_CPP_LTSECDB LtSecDbObj LtSecDbObjCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsDateGen GsDateGenCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsDiscountCurve GsDiscountCurveCtor( const GsDt& );

EXPORT_CPP_LTSECDB CC_STL_VECTOR( double ) GsStdVectorCtor( const GsDt& );

EXPORT_CPP_LTSECDB GsVolatilityCurve GsVolatilityCurveCtor( const GsDt& );

CC_END_NAMESPACE

#endif 


