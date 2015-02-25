/****************************************************************
**
**	GSFUNCDOTDISJOINT.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncDotDisjoint.h,v 1.7 2001/11/27 22:45:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCDOTDISJOINT_H )
#define IN_GSFUNCDOTDISJOINT_H

#include <vector>
#include <ccstl.h>
#include <gscore/GsVector.h>
#include <gscore/GsFunc.h>
#include <gscore/base.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDoubleVector.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncDotDisjoint : public GsFunc<double, double>
{
public:
	typedef GsFuncHandle<double, double>	node_type;
	typedef CC_STL_VECTOR( node_type )		vec_type;

	GsFuncDotDisjoint(const vec_type&,
					  const GsDoubleVector& weights,
					  const GsDoubleVector& bounds
					 );
	virtual ~GsFuncDotDisjoint() {}

	double operator() (double) const;

private:

	const vec_type			m_basis;
	const GsDoubleVector	m_weight;
	const GsDoubleVector	m_bounds;

	// cached info
	GsVector	m_cumulative;
	int			m_lastIndx;

};

EXPORT_CPP_GSFUNC
GsFuncHandle<double, double> 
GsFuncDotDisjointMake(const GsFuncDotDisjoint::vec_type& bf,
					  const GsDoubleVector& weights,
					  const GsDoubleVector& bounds);

CC_END_NAMESPACE

#endif 
