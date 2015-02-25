/****************************************************************
**
**	KnockoutData.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_KNOCKOUTDATA_H )
#define IN_KNOCKOUTDATA_H

#include <qgrid/base.h>
#include <qgrid/enums.h>
#include <gscore/GsVector.h>
#include <qenums.h>
#include <math.h>
#include <vector>
#include <ccstl.h>
#include <qgrid/KnockoutSegment.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CPP_QGRID KnockoutData
{
	public:
		typedef KnockoutSegment Segment;

		KnockoutData()
		:
			m_nSegments( 1 ),
			m_StartTimes( 1, 0. ),
			m_Segments( 1, Segment() )
		{}
		
		void AddLowerKnockout(
			double KOLevel,
			double StartTime,
			double EndTime,
			double Rebate,
			double RebateTime
		);

		void AddUpperKnockout(
			double KOLevel,
			double StartTime,
			double EndTime,
			double Rebate,
			double RebateTime
		);

		int nSegments() const { return m_nSegments; }
		double StartTime( int n ) const { return m_StartTimes[ n ]; }
		int AddSegment( double t ) const;

		int GetContainingSegment( double t ) const;
		int GetLHSContainingSegment( double t ) const;
		const Segment& segment( int i ) const { return m_Segments[ i ]; }

		void SetRebateTime( double t );

	private:
		int					     m_nSegments;
		CC_STL_VECTOR( double )  m_StartTimes;
		CC_STL_VECTOR( Segment ) m_Segments;

		void AddSegment( double Start, double End, int& iStart, int& iEnd );
		void PruneSegments();
};

CC_END_NAMESPACE

#endif

