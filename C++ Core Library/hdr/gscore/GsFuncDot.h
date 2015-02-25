/****************************************************************
**
**	GSFUNCDOT.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncDot.h,v 1.8 2001/11/27 22:45:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCDOT_H )
#define IN_GSFUNCDOT_H

#include <vector>
#include <ccstl.h>
#include <gscore/GsVector.h>
#include <gscore/GsFunc.h>
#include <gscore/base.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDoubleVector.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncDot : public GsFunc<double, double>
{
public:
	typedef GsFuncHandle<double, double>	node_type;
	typedef CC_STL_VECTOR( node_type )		vec_type;

	GsFuncDot(const vec_type&,
			  const GsDoubleVector& weights);
	virtual ~GsFuncDot() {}

	double operator() (double) const;

private:

	vec_type		m_basis;
	GsDoubleVector	m_weight;

};

EXPORT_CPP_GSFUNC
GsFuncHandle<double, double> 
GsFuncDotMake(const GsFuncDot::vec_type& bf,
			  const GsDoubleVector& weights);

CC_END_NAMESPACE

#endif 
