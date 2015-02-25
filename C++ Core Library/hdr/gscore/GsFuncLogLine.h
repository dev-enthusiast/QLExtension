/****************************************************************
**
**	GSFUNCLOGLINE.C	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Log: GsFuncLogLine.h,v $
**	Revision 1.1  2000/07/31 23:13:15  tomasje
**	Initial revision.
**
****************************************************************/



#if !defined( IN_GSFUNCLOGLINE_H )
#define IN_GSFUNCLOGLINE_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncLogLine : public GsFunc<double, double>
{
public:
	GsFuncLogLine(double c = 0.0, double b = 0.0, double a = 0.0);
	virtual ~GsFuncLogLine();

	double operator() (double) const;

private:

	double m_a;
	double m_b;
	double m_c;

};

CC_END_NAMESPACE

#endif 
