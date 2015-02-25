/****************************************************************
**
**	gsrand/GsRand.h	- GsRand class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/GsRand.h,v 1.3 1999/07/23 16:36:04 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_GSRAND_H )
#define IN_GSRAND_GSRAND_H

#include <port_int.h>
#include <gsrand/base.h>
#include <gscore/types.h>
#include <gscore/GsException.h>
#include <gscore/GsVector.h>

CC_BEGIN_NAMESPACE( Gs )

/*
** Class: GsRand
**
** GsRand is an abstract base class to describe the interface for random
** number generation.
*/

class EXPORT_CLASS_GSRAND GsRand 
{
public:
	virtual ~GsRand() = 0;
	
	inline double operator()() { return drawOne(); }

	GsVector operator()( size_t Number );

	void draw( GsVector& Vec);

protected:
	GsRand();
	GsRand( const GsRand& Rand );
	GsRand& operator=( const GsRand& Rand );

	virtual double drawOne() = 0;
};

CC_END_NAMESPACE

#endif 


