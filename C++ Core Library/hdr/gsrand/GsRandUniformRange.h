/****************************************************************
**
**	gsrand/GsRandUniformRange.h	- class GsRandUniformRange
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/GsRandUniformRange.h,v 1.2 1999/05/17 19:50:44 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_GSRANDUNIFORMRANGE_H )
#define IN_GSRAND_GSRANDUNIFORMRANGE_H

#include <gsrand/base.h>	
#include <gscore/types.h>
#include <gsrand/GsRandUniform.h>

CC_BEGIN_NAMESPACE( Gs )

/*
** class GsRandUniformRange - produce UniformRange random numbers in the range Lo -> Hi
*/

class EXPORT_CLASS_GSRAND GsRandUniformRange : public GsRand 
{
public:
	GsRandUniformRange( double Lo, double Hi, INT32 Seed = 0 );
	GsRandUniformRange( const GsRandUniformRange& Rand );
	GsRandUniformRange& operator=( const GsRandUniformRange& Rand );

	virtual ~GsRandUniformRange();
	
	void setRange( double Lo, double Hi );

protected:

	virtual double drawOne();

private:
	GsRandUniformRange(); 

	GsRandUniform m_Rand;

	double  m_Lo,
			m_Delta;
};

CC_END_NAMESPACE

#endif

