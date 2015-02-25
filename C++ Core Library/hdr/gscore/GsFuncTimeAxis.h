/* $Header: /home/cvs/src/gsfunc/src/gscore/GsFuncTimeAxis.h,v 1.4 1998/11/10 17:33:25 hammet Exp $ */
/****************************************************************
**
**	GSFUNCTIMEAXIS.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncTimeAxis.h,v 1.4 1998/11/10 17:33:25 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCTIMEAXIS_H )
#define IN_GSFUNCTIMEAXIS_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsCTime.h>
#include <gscore/GsDate.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncTimeAxis 
	: public GsFunc<GsDate, double>
{
public:

	GsFuncTimeAxis(GsFuncHandle<double, double> f,
				   GsDate ref,
				   double scale = 1.0
				   )
	: m_f(f), m_ref(ref), m_scale(scale)
	{
	}

	virtual ~GsFuncTimeAxis();

	double operator()(GsDate) const;

private:

	GsFuncHandle<double, double> m_f;
	GsDate m_ref;
	double m_scale;

};

CC_END_NAMESPACE

#endif 
