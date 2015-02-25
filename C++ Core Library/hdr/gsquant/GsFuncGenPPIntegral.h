/****************************************************************
**
**	GSFUNCGENPPINTEGRAL.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncGenPPIntegral.h,v 1.2 2000/03/31 23:56:36 tsoucp Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCGENPPINTEGRAL_H )
#define IN_GSQUANT_GSFUNCGENPPINTEGRAL_H

#include <gsquant/base.h>

#include <gsquant/GsFuncPiecewisePolyIntegral.h>

#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

// .... FIX: replace with integrator gsfunc ...
class EXPORT_CLASS_GSQUANT GsFuncGenPPIntegral : 
	public GsFunc< GsVector, GsFuncHandle< double, double > >
{
public:
	GsFuncGenPPIntegral( const GsVector& knots ) : m_knots( knots ) {}
	virtual ~GsFuncGenPPIntegral() {}

	virtual GsFuncHandle< double, double > 
			operator() ( GsVector p ) const;

protected:
	const GsVector m_knots;
};

class EXPORT_CLASS_GSQUANT GsFuncGenPP : public  
		GsFunc< GsVector, GsFuncHandle< double, double > >
{
public:
	GsFuncGenPP( const GsVector& knots ) : m_knots( knots ) {}
	virtual ~GsFuncGenPP() {}

	virtual GsFuncHandle< double, double > operator() ( GsVector p ) const;

protected:
	const GsVector m_knots;
};

CC_END_NAMESPACE

#endif 
