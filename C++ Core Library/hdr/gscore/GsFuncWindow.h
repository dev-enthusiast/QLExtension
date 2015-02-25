/* $Header: /home/cvs/src/gsfunc/src/gscore/GsFuncWindow.h,v 1.6 1999/03/18 04:54:57 moffia Exp $ */
/****************************************************************
**
**	GSFUNCWINDOW.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncWindow.h,v 1.6 1999/03/18 04:54:57 moffia Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCWINDOW_H )
#define IN_GSFUNCWINDOW_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncWindow : public GsFunc<double, double>
{
public:
	GsFuncWindow(double l, double r, GsFuncHandle<double, double>);
	GsFuncWindow(double l, double r, GsFuncHandle<double, double>,
				 double lvalue, bool lopen,
				 double rvalue, bool ropen);
	virtual ~GsFuncWindow();

	double operator() (double) const;

	bool isBracketed(double x);

private:

	double m_l;
	double m_r;
	double m_lv;
	double m_rv;
	GsFuncHandle<double, double> m_f;	
	bool m_lopen;
	bool m_ropen;

};

CC_END_NAMESPACE

#endif 
