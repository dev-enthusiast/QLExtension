/****************************************************************
**
**	GsFuncPiecewise.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncPiecewise.h,v 1.8 2001/11/27 22:45:48 procmon Exp $
**
****************************************************************/

#if !defined( IN_GsFuncPiecewise_H )
#define IN_GSFUNCPIECEWISE_H

#include <vector>
#include <ccstl.h>
#include <gscore/GsVector.h>
#include <gscore/GsFunc.h>
#include <gscore/base.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsFuncWindow.h>
#include <gscore/GsDoubleVector.h>
#include <gscore/GsVector.h>
#include <gscore/GsDateVector.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSFUNC
GsFuncHandle<double, double> GsFuncPiecewiseConstant(
	const GsDoubleVector& x,
    const GsDoubleVector& y
);

EXPORT_CPP_GSFUNC
GsFuncHandle<GsDate, double> GsFuncPiecewiseConstant(
	const GsDateVector& x,
    const GsDoubleVector& y
);




class EXPORT_CLASS_GSFUNC GsFuncPiecewise : public GsFunc<double, double>
{
public:
	typedef CC_STL_VECTOR( GsFuncWindow* ) vec_type;

	GsFuncPiecewise(
		const  GsVector& x,
		double extreme,
		const  GsVector& y,
		bool   isRightClosed,
		double other = 0
	);

	GsFuncPiecewise(
		const vec_type& windows
	);


	virtual ~GsFuncPiecewise();

	double operator() (double) const;

	virtual GsFunc< double, double >* Integrate( double initialValue  ) const;
private:

	GsFuncPiecewise( const GsFuncPiecewise& );
	GsFuncPiecewise& operator=( const GsFuncPiecewise& );
	vec_type m_windows;
	GsVector m_x;
	GsVector m_y;
	double m_extreme;
	double m_isRightClosed;
	double m_other;
};

EXPORT_CPP_GSFUNC
GsFuncPiecewise* GsFuncPiecewiseRight(
    const GsVector& x,
	double extreme,
	const GsVector& y
);

EXPORT_CPP_GSFUNC
GsFuncPiecewise* GsFuncPiecewiseLeft(
    const GsVector& x,
	double extreme,
	const GsVector& y
);


CC_END_NAMESPACE

#endif 





