/****************************************************************
**
**	GsHierarchyIterator.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_GSHIERARCHYITERATOR_H )
#define IN_GSHIERARCHYITERATOR_H

#include <gsquant/base.h>
#include <qenums.h>
#include <gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )


class GsHierarchyIterator
{
	public:
		GsHierarchyIterator(
//			const GsHierarchy&	  		Hierarchy,
			const CC_STL_VECTOR( int)&  level
//			int					  		valueindex,
//			const CC_STL_VECTOR( int )* prevptrs,
//			const CC_STL_VECTOR( int )* nextptrs
		)
		:
//			m_Hierarchy( Hierarchy ),
			m_level( level )
//			m_ValueIndex( valueindex ),
//			m_prevptrs( prevptrs ),
//			m_nextptrs( nextptrs )
		{}

		void next( int dim )
		{
			++m_level[ dim ];
//			m_ValueIndex = m_nextptrs[ dim ][ m_ValueIndex ];
		}

		void prev( int dim )
		{
			--m_level[ dim ];
//			m_ValueIndex = m_prevptrs[ dim ][ m_ValueIndex ];
		}

		int ValueIndex() const
		{
			return m_ValueIndex;
//			return m_Hierarchy.GetIndex( m_level );
		}

		int Level( int dim ) const
		{
			return m_level[ dim ];
		}

		const CC_STL_VECTOR( int )& Level() const
		{
			return m_level;
		}

	private:
		CC_STL_VECTOR( int )   m_level;
		int					   m_ValueIndex;

//		const GsHierarchy&	   m_Hierarchy;

//		const CC_STL_VECTOR( int )* m_prevptrs;
//		const CC_STL_VECTOR( int )* m_nextptrs;
};

CC_END_NAMESPACE

#endif

