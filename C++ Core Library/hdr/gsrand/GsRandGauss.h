/****************************************************************
**
**	gsrand/GsRandGauss.h	- class GsRandGauss
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/GsRandGauss.h,v 1.5 2011/08/02 21:14:20 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_GSRANDGAUSS_H )
#define IN_GSRAND_GSRANDGAUSS_H

#include <gsrand/base.h>	
#include <gsrand/GsRandUniform.h>
#ifdef GSRAND_GSL
#include <gsl/gsl_rng.h>
#endif

CC_BEGIN_NAMESPACE( Gs )

/*
** class GsRandGauss - produce Gaussian random numbers
*/

class EXPORT_CLASS_GSRAND GsRandGauss : public GsRand 
{
public:
	GsRandGauss(INT32 Seed = 0); 
	GsRandGauss(const GsRandGauss& Rand);
	GsRandGauss& operator=(const GsRandGauss& Rand);
	
	~GsRandGauss();
	
protected:

	virtual double drawOne();

private:

#ifdef GSRAND_GSL
    gsl_rng 
            *m_RandState;
#else
#   ifndef GSRAND_NO_IMSL
	int		
            m_SeedIMSL;
#   else
	GsRandUniform 
			m_Rand;
#   endif
#endif
};

CC_END_NAMESPACE


#endif 
