/****************************************************************
**
**	GSFUNCVECTORRANGE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncVectorRange.h,v 1.3 2001/11/27 22:46:45 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCVECTORRANGE_H )
#define IN_GSQUANT_GSFUNCVECTORRANGE_H

#include <gsquant/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsVector.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSQUANT GsFuncVectorEndRange :	
	public GsFunc< GsVector, GsVector >
{
public:
	enum EndRangeType { FromBegin, ToEnd };
	// index is included, i.e.,
	// the range returned is v[ 0 ], ..., v[ m_size - 1 ] or
	// v[ v.size() - 1 - m_size   ], ...., v[ v.size() - 1 ]
	GsFuncVectorEndRange( size_t size, EndRangeType type );
	virtual ~GsFuncVectorEndRange() {}

	virtual GsVector operator()( GsVector v ) const
	{
		if( m_type == FromBegin )

			return GsVector( m_size, v.data() );

		else if( m_type == ToEnd )

			return GsVector( m_size, 
							 v.data() + v.size() - 1 - m_size );

		else
		{
			// FIX: Throw exception 
			return GsVector();
		}
	};

private:
	size_t m_size;
	EndRangeType m_type;
};

class EXPORT_CLASS_GSQUANT GsFuncVectorMidRange :	
	public GsFunc< GsVector, GsVector >
{
public:
	// the range returned is v[ from ], ..., v[ to ] 
	GsFuncVectorMidRange( size_t from, size_t to );
	virtual ~GsFuncVectorMidRange() {}

	virtual GsVector operator()( GsVector v ) const
	{
		// FIX: exception?
		return GsVector( m_to - m_from + 1, v.data() + m_from );
	};

private:
	size_t m_from;
	size_t m_to;
};

CC_END_NAMESPACE

#endif 
