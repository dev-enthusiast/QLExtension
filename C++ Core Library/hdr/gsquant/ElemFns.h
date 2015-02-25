/****************************************************************
**
**	ELEMFNS.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/ElemFns.h,v 1.3 2001/01/11 19:32:23 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_ELEMFNS_H )
#define IN_GSQUANT_ELEMFNS_H

#include <gsquant/base.h>
#include <gsdt/GsDt.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

// FIX - if autofunc could parse templates args this would not be needed.
typedef GsFuncHandle<  double, double > GsFuncDoubleDouble;
typedef GsFunc2Handle< double, double, double > GsFuncDoubleDoubleDouble;

EXPORT_CPP_GSQUANT GsDt *GsElemApply(  const GsDt &Container, const GsFuncDoubleDouble &Func );

EXPORT_CPP_GSQUANT GsDt *GsElemApply2( const GsDt &Arg0, const GsDt &Arg1, const GsFuncDoubleDoubleDouble &Func );

EXPORT_CPP_GSQUANT GsDt *GsElemMax( const GsDt &Arg0, const GsDt &Arg1 );
EXPORT_CPP_GSQUANT GsDt *GsElemMin( const GsDt &Arg0, const GsDt &Arg1 );

EXPORT_CPP_GSQUANT GsDt *GsElemBinaryMax( const GsDt &Arg0, const GsDt &Arg1 );
EXPORT_CPP_GSQUANT GsDt *GsElemBinaryMin( const GsDt &Arg0, const GsDt &Arg1 );

EXPORT_CPP_GSQUANT GsDt * GsElemIf( const GsDt &Condition, const GsDt &TrueVal, const GsDt &FalseVal );

CC_END_NAMESPACE

#endif 
