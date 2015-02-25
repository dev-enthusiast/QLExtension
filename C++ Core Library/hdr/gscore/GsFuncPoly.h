/* $Header: /home/cvs/src/gsfunc/src/gscore/GsFuncPoly.h,v 1.3 2000/06/09 16:47:06 vengrd Exp $ */
/****************************************************************
**
**	GSFUNCPOLY.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncPoly.h,v 1.3 2000/06/09 16:47:06 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCPOLY_H )
#define IN_GSFUNCPOLY_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsVector.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncPoly : public GsFunc<double, double>
{
public:
	GsFuncPoly( const GsVector& coefficients );

	int degree() { return m_coefficients.size()-1; }
	
	virtual ~GsFuncPoly();

	double operator() (double) const;

	GsFunc<double, double>* Square() const;

private:

	GsVector m_coefficients;

};

CC_END_NAMESPACE

#endif 
