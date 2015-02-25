/****************************************************************
**
**	gsrand/GsRandUniform.h	- class GsRandUniform
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/GsRandUniform.h,v 1.2 1999/05/17 19:50:44 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_GSRANDUNIFORM_H )
#define IN_GSRAND_GSRANDUNIFORM_H

#include <gsrand/base.h>
#include <gscore/types.h>
#include <gsrand/GsRand.h>

CC_BEGIN_NAMESPACE( Gs )

/*
** class GsRandUniform - produce uniform random numbers in the range 0 -> 1
*/

class EXPORT_CLASS_GSRAND GsRandUniform : public GsRand 
{
public:
	GsRandUniform( INT32 Seed = 0);
	GsRandUniform( const GsRandUniform& Rand );
	GsRandUniform& operator=( const GsRandUniform& Rand );

	virtual ~GsRandUniform();

protected:
	
	virtual double drawOne();

private:
	enum PrivateConstants { NTAB = 32 };

	INT32	m_Seed,
			m_Seed2,  
			m_IY,
			m_Table[ NTAB ];
};

CC_END_NAMESPACE

#endif

