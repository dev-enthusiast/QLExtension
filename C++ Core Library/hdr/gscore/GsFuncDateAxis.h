/****************************************************************
**
**	GSFUNCDATEAXIS.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncDateAxis.h,v 1.1 1998/11/18 15:21:26 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCDATEAXIS_H )
#define IN_GSFUNCDATEAXIS_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsCTime.h>
#include <gscore/GsDate.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncDateAxis : public GsFunc<GsDate, double>
{
public:

	GsFuncDateAxis(GsFuncHandle<double, double> f,
				   GsDate ref,
				   double scale = 1.0
				   )
	: m_f(f), m_ref(ref), m_scale(scale)
	{
	}

	virtual ~GsFuncDateAxis();

	double operator()(GsDate) const;

private:

	GsFuncHandle<double, double> m_f;
	GsDate m_ref;
	double m_scale;

};

CC_END_NAMESPACE

#endif 
