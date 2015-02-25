/****************************************************************
**
**	GSFUNCVECTORCURRY.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncVectorCurry.h,v 1.1 2000/06/30 14:25:14 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCVECTORCURRY_H )
#define IN_GSQUANT_GSFUNCVECTORCURRY_H

#include <gsquant/base.h>
#include <gscore/GsVector.h>
#include <gscore/GsUnsignedVector.h>
#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsFuncVectorCurry 
** Description : Wrapper for a function from vector to vector
**				 with some args defaulted.
****************************************************************/

class EXPORT_CLASS_GSQUANT GsFuncVectorCurry : public GsFunc< GsVector, GsVector >
{
public:

	GsFuncVectorCurry( GsFuncHandle< GsVector, GsVector > Func, const GsUnsignedVector &Mask, const GsVector &Defaults );
	GsFuncVectorCurry( const GsFuncVectorCurry & );

	virtual ~GsFuncVectorCurry();

	GsFuncVectorCurry &operator=( const GsFuncVectorCurry &rhs );

	virtual GsVector operator()( GsVector Arg ) const;

private:

	GsFuncHandle< GsVector, GsVector > 
			m_Func;

	GsUnsignedVector
			m_Mask;

	GsVector
			m_Defaults;
};


typedef GsFuncHandle< GsVector, GsVector > GsFuncVectorVector;

EXPORT_CPP_GSQUANT GsFuncVectorVector GsFuncVectorCurryCreate( const GsFuncVectorVector &Func, const GsUnsignedVector &Mask, const GsVector &Defaults );


CC_END_NAMESPACE

#endif 
