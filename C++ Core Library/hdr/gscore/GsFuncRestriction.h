/****************************************************************
**
**	GSFUNCRESTRICTION.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncRestriction.h,v 1.5 2001/11/27 22:45:48 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCRESTRICTION_H )
#define IN_GSFUNC_GSFUNCRESTRICTION_H

#include <gscore/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

#include <vector>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Vec, class Sc >
class GsFuncRestriction : public GsFunc< Sc, Sc >
{
public:
	typedef GsFuncHandle< const Vec&, Sc > node_type;
	typedef CC_STL_VECTOR( node_type ) vec_type;
	GsFuncRestriction( const vec_type& v,
					   const Vec& val,
					   size_t i,
					   size_t j )
	: m_v( v ), m_val( val ), m_i( i ), m_j( j ) {}


	// See GsFuncRestriction.hi
	virtual ~GsFuncRestriction();
	virtual Sc operator()( Sc x ) const;

private:
	vec_type	m_v;
	mutable Vec	m_val;
	size_t		m_i;
	size_t		m_j;
};

CC_END_NAMESPACE

#endif 

