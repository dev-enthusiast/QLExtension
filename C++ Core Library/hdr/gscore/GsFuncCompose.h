/****************************************************************
**
**	GSFUNCCOMPOSE.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncCompose.h,v 1.5 2000/09/08 11:05:39 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCCOMPOSE_H )
#define IN_GSFUNCCOMPOSE_H

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsStreamMap.h>

CC_BEGIN_NAMESPACE( Gs )

template< class Domain, class Common, class Range > 
class GsFuncCompose : public GsFunc< Domain, Range >
{
public:
	GsFuncCompose()
	: m_Inner(), m_Outer() {}
	GsFuncCompose( GsFuncHandle< Common, Range > Outer, 
				   GsFuncHandle< Domain, Common > Inner ) 
	: m_Inner( Inner ), m_Outer( Outer ) {}
	virtual ~GsFuncCompose() {}

	virtual Range operator () ( Domain Arg ) const
	{ 
		Common Tmp = m_Inner( Arg );
		return m_Outer( Tmp );
	}

	typedef GsStreamMapT< GsFuncCompose< Domain, Common, Range> > GsStType;

	virtual GsStreamMap* buildStreamMapper() const
	{
		static GsStreamMap* MapCloner = GsStreamMap::find( GsStType::typeName() );

		if( !MapCloner )
			GSX_THROW( GsXInvalidOperation, "GsFuncCompose::buildStreamMapper - No GsStreamMap" );

		return MapCloner->clone( this );
	}


	const GsFuncHandle< Domain, Common >&  getInner() const { return m_Inner; }
	const GsFuncHandle< Common, Range >& getOuter() const { return m_Outer; }
	
private:
	GsFuncHandle< Domain, Common > m_Inner;
	GsFuncHandle< Common, Range > m_Outer;
};

template< class Domain, class Common, class Range > 
class GsFuncConstCompose : public GsFunc< const Domain&, Range >
{
public:
	GsFuncConstCompose( GsFuncHandle< const Common&, Range > Outer, 
						GsFuncHandle< const Domain&, Common > Inner ) 
	: m_Inner( Inner ), m_Outer( Outer ) {}
	virtual ~GsFuncConstCompose() {}

	virtual Range operator()( const Domain& Arg ) const
	{ 
		Common Tmp = m_Inner( Arg );
		return m_Outer( Tmp );
	}

private:
	GsFuncHandle< const Domain&, Common > m_Inner;
	GsFuncHandle< const Common&, Range > m_Outer;
};

template< class Domain, class Common, class Range > 
class GsFuncGenCompose : public 
	GsFunc< GsFuncHandle< Domain, Common >, GsFuncHandle< Domain, Range > >
{
public:
	GsFuncGenCompose( const GsFuncHandle< Common, Range >& Outer )
	: m_Outer( Outer ) {}
	virtual ~GsFuncGenCompose() {}

	virtual GsFuncHandle< Domain, Range > operator() ( 
		GsFuncHandle< Domain, Common> Inner ) const
	{ 
		return new GsFuncCompose< Domain, Common, Range >( m_Outer, Inner );
	}

private:
	GsFuncHandle< Common, Range > m_Outer;
};

template< class Domain, class Common, class Range>
GsFuncHandle< Domain, Range > GsFuncComposeMake(
	const GsFunc< Common, Range >& Outer,
	const GsFunc< Domain, Common >& Inner
)
{
	return new GsFuncCompose< Domain, Common, Range >( Outer, Inner );
}

template< class Domain, class Common, class Range>
GsFuncHandle< Domain, Range > GsFuncComposeMake(
	GsFuncHandle< Common, Range > Outer,
	GsFuncHandle< Domain, Common > Inner
)
{
	return new GsFuncCompose< Domain, Common, Range >( Outer, Inner );
}

CC_END_NAMESPACE

#endif 

