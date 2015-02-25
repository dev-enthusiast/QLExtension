/****************************************************************
**
**	GSFUNCCONSTRAINT.H	- Generic constraint class used by various fitters
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncConstraint.h,v 1.4 2012/08/28 20:47:44 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCCONSTRAINT_H )
#define IN_GSQUANT_GSFUNCCONSTRAINT_H

#include <gsquant/base.h>
#include <gscore/GsFuncHandle.h>
#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )


template< class Domain, class Range >
class GsFuncConstraint
{
public:
	GsFuncConstraint( Domain bound, Range target, 
					  GsFuncHandle< GsFuncHandle< Domain, Range >, Range > func);

	GsFuncConstraint( const GsFuncConstraint& other );
	~GsFuncConstraint() {}
	GsFuncConstraint& operator=( const GsFuncConstraint& other );

	Domain bound()  const { return m_bound ;}
	Range  target() const { return m_target;}
	
	
	Range operator()( GsFuncHandle< Domain, Range > f ) const { return m_func( f ) - m_target; }
	
	GsFuncHandle< GsFuncHandle< Domain, Range >, Range > function() const { return m_func; }
	
	//STL Vector Support for sorting
	int operator < (const GsFuncConstraint& other) const 
	{ 
		return m_bound < other.m_bound; 
	}
	// GsDt Support.
	typedef GsDtGeneric<GsFuncConstraint< Domain, Range> > GsDtType;
	GsString toString() const;
	inline static const char * typeName() { return "GsFuncConstraint"; }
	// GsSt Support
  	typedef GsStreamMapT< GsFuncConstraint > GsStType;
  	virtual GsStreamMap* buildStreamMapper() const;

private:
	GsFuncConstraint() {}
	Domain m_bound;
	Range  m_target;

	GsFuncHandle< GsFuncHandle< Domain, Range >, Range > m_func;
};


CC_END_NAMESPACE

#endif 







