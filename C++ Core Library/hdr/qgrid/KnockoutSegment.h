/****************************************************************
**
**	KnockoutSegment.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_KNOCKOUTSEGMENT_H )
#define IN_KNOCKOUTSEGMENT_H

#include <qgrid/base.h>
#include <qgrid/enums.h>
#include <qenums.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CPP_QGRID KnockoutSegment
{
	public:
		KnockoutSegment();

		bool operator==( const KnockoutSegment& rhs ) const;

		double LowerKO() const { return m_LowerKO; }
		double UpperKO() const { return m_UpperKO; }
		double LowerRebate() const { return m_LowerRebate; }
		double UpperRebate() const { return m_UpperRebate; }
		double LowerRebateTime() const { return m_LowerRebateTime; }
		double UpperRebateTime() const { return m_UpperRebateTime; }

		void SetLowerKO( double x ) { m_LowerKO = x; }
		void SetUpperKO( double x ) { m_UpperKO = x; }
		void SetLowerRebate( double x ) { m_LowerRebate = x; }
		void SetUpperRebate( double x ) { m_UpperRebate = x; }
		void SetLowerRebateTime( double x ) { m_LowerRebateTime = x; }
		void SetUpperRebateTime( double x ) { m_UpperRebateTime = x; }
	private:
		double m_LowerKO;
		double m_UpperKO;
		double m_LowerRebate;
		double m_UpperRebate;
		double m_LowerRebateTime;
		double m_UpperRebateTime;
};

CC_END_NAMESPACE


#endif

