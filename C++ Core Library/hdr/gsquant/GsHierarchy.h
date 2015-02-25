/****************************************************************
**
**	GsHierarchy.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.16 $
**
****************************************************************/

#if !defined( IN_GSHIERARCHY_H )
#define IN_GSHIERARCHY_H

#include <gsquant/base.h>
#include <qenums.h>
#include <gscore/types.h>
#include <gscore/GsHash.h>
#include <gscore/GsHashForward.h>
#include <gsdt/GsDtGeneric.h>
#include <vector>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
**
**	Class: GsHierarchy
**	
**		Stores a set of levels to be used by a hierarchical grid.
**	This is just a finite subset of Z^d_+ satisfying
**
**	1) level2 <= level and level is in the hierarchy
**		=> level2 is in the hierarchy
**
**	For every level it returns an index to index into an array,
**	(returns index of -1 if level is not in the hierarchy).
**
**	Uses a hash table to convert level to index
**
**	To be added: need a GsHierarchy::iterator so that you can step
**	efficiently from one level to the next.
****************************************************************/

class EXPORT_CLASS_GSQUANT GsHierarchy
{
public:
	class iterator;

	// GsDt support
	typedef GsDtGeneric<GsHierarchy> GsDtType;
	GsString toString() const { return "GsHierarchy"; }
	inline static const char *typeName() { return "GsHierarchy"; }

	GsHierarchy( int dimension );
	virtual ~GsHierarchy() {}

	void AddLevel( const int* Levels );
	int  GetIndex( const int* Levels ) const;
	int	 dimension() const { return m_dimension; }
	int	 nLevels() const { return m_nLevels; }
	void AddSparseLevels( int level );
	const CC_STL_VECTOR( int )& MaxLevels() const { return m_MaxLevels; }

	iterator begin() const
	{ iterator retval( *this ); return retval;	}

	iterator Iterator( const CC_STL_VECTOR( int )& level ) const
	{ iterator retval( *this, level ); return retval; }

private:
	int  	  m_dimension;
	int  	  m_nLevels;

	CC_STL_VECTOR( int ) m_MaxLevels;

	GsHashMapStringToInt m_HashMap;

	void AddSingleLevel( const int* Levels );
	void UpdateMaxLevels( const int* Levels );

public:
	class EXPORT_CLASS_GSQUANT iterator
	{
	public:
		typedef GsDtGeneric<iterator> GsDtType;
		GsString toString() const;
		inline static const char *typeName() 
		{ return "GsHierarchy::iterator"; }
		
		iterator( const GsHierarchy& Hierarchy,
				  const CC_STL_VECTOR( int)&  level	):
		m_level( level ),
		m_Hierarchy( Hierarchy ){}

		iterator( const GsHierarchy& Hierarchy ):
		m_level( Hierarchy.dimension(), 0 ),
		m_Hierarchy( Hierarchy ){}

		void next( int dim ) { ++m_level[ dim ]; }
		void prev( int dim ) { --m_level[ dim ]; }
		int ValueIndex() const 
		{ return m_Hierarchy.GetIndex( &(*m_level.begin()) ); }

		int Level( int dim ) const { return m_level[ dim ]; }
		const CC_STL_VECTOR( int )& Level() const { return m_level; }
		bool next();
		bool IsValid() const;

	private:
		CC_STL_VECTOR( int )   m_level;
		int					   m_ValueIndex;
		const GsHierarchy&	   m_Hierarchy;
	};
};

typedef GsHierarchy::iterator GsHierarchy_iterator;

CC_END_NAMESPACE

#endif

