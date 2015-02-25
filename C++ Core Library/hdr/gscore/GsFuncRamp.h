/* $Header: /home/cvs/src/gsfunc/src/gscore/GsFuncRamp.h,v 1.4 1998/11/10 17:33:25 hammet Exp $ */
/****************************************************************
**
**	GSFUNCRAMP.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncRamp.h,v 1.4 1998/11/10 17:33:25 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCRAMP_H )
#define IN_GSFUNCRAMP_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncRamp : public GsFunc<double, double>
{
public:
	GsFuncRamp(double x1, double y1, double x2, double y2);
	virtual ~GsFuncRamp();

	double operator() (double) const;

private:

	double m_x1;
	double m_y1;
	double m_x2;
	double m_y2;

};

CC_END_NAMESPACE

#endif 
