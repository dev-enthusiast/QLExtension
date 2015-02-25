/****************************************************************
**
**	GSFUNCDIRSUMRANGE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncDirSumRange.h,v 1.6 2001/11/27 22:45:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCDIRSUMRANGE_H )
#define IN_GSFUNC_GSFUNCDIRSUMRANGE_H

#include <gscore/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

#include <vector>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Vec, class Sc >
class GsFuncDirSumRange : public GsFunc< const Vec&, Vec >
{
public:
	typedef GsFuncHandle< const Vec& , Sc > node_type;
	typedef CC_STL_VECTOR( node_type )		vec_type;

	GsFuncDirSumRange( const vec_type& v ) : m_funcVec( v ) {}

	// See .hi file for implementation
	virtual ~GsFuncDirSumRange();
	virtual Vec operator()( const Vec& x ) const;

protected:
	vec_type	m_funcVec;
};

CC_END_NAMESPACE

#endif 

