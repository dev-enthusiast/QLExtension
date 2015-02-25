/****************************************************************
**
**	GSFUNCCONVERTTYPE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncConvertType.h,v 1.1 1999/11/09 16:49:03 tsoucp Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCCONVERTTYPE_H )
#define IN_GSFUNC_GSFUNCCONVERTTYPE_H

#include <gscore/base.h>

#include <gscore/GsFunc.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Type > class GsFuncConvertType : 
	public GsFunc< const Type&, Type >
{
public:
	GsFuncConvertType() {}
	~GsFuncConvertType() {}

	virtual Type operator()( const Type& x ) const
	{
		return x;
	}
};

CC_END_NAMESPACE

#endif 
