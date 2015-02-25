/* $Header: /home/cvs/src/gsfunc/src/gscore/GsFuncLine.h,v 1.3 1998/11/10 17:33:25 hammet Exp $ */
/****************************************************************
**
**	GSFUNCLINE.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncLine.h,v 1.3 1998/11/10 17:33:25 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCLINE_H )
#define IN_GSFUNCLINE_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncLine : public GsFunc<double, double>
{
public:
	GsFuncLine(double b = 0.0, double a = 0.0);
	virtual ~GsFuncLine();

	double operator() (double) const;

private:

	double m_a;
	double m_b;

};

CC_END_NAMESPACE

#endif 
