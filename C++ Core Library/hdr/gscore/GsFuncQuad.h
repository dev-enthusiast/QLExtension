/* $Header: /home/cvs/src/gsfunc/src/gscore/GsFuncQuad.h,v 1.4 1998/11/10 17:33:25 hammet Exp $ */
/****************************************************************
**
**	GSFUNCQUAD.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncQuad.h,v 1.4 1998/11/10 17:33:25 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCQUAD_H )
#define IN_GSFUNCQUAD_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncQuad : public GsFunc<double, double>
{
public:
	GsFuncQuad(double c = 0.0, double b = 0.0, double a = 0.0);
	virtual ~GsFuncQuad();

	double operator() (double) const;

private:

	double m_a;
	double m_b;
	double m_c;	

};

CC_END_NAMESPACE

#endif 
