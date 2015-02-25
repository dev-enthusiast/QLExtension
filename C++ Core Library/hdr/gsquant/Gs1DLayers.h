/****************************************************************
**
**	Gs1DLayers.h	- layers of one dimensional grids
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_GS1DLAYERS_H )
#define IN_GS1DLAYERS_H

#include <vector>
#include <gsquant/base.h>
#include <gsdt/GsDtGeneric.h>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSQUANT Gs1DLayers
{
	public:
		// GsDt support
		typedef GsDtGeneric<Gs1DLayers> GsDtType;
		GsString toString() const;
		inline static const char *typeName() { return "Gs1DLayers"; }

		Gs1DLayers( int size, const double* CoordValues );
		inline Gs1DLayers( const Gs1DLayers& rhs );
		virtual ~Gs1DLayers() {}

		void AddBisections( int nlayers );

		int size() const { return m_Size; }
		int MaxLevel() const { return m_nLevels - 1; }
		int nLevels() const { return m_nLevels; }

		int size( int level ) const { return m_LevelSizes[ level ]; }
		const CC_STL_VECTOR( double )& CoordValues() const { return m_TotalCoordValues; }
		const CC_STL_VECTOR( double )& CoordValues( int level ) const { return m_CoordValues[ level ]; }
		void GetLevelIndices( CC_STL_VECTOR( int )& indices, int sublevel, int gridlevel ) const;
		int GetValueIndex( int level, int level_index, int maxlevel ) const;
		int SubGridSize( int level ) const { return (( m_LevelSizes[ 0 ] - 1 ) << level ) + 1; }

	private:
		int			m_nLevels;									   // number of levels
		int			m_Size;										   // total size of grid
		CC_STL_VECTOR( int )  m_LevelSizes;						   // size of each level
		CC_STL_VECTOR( CC_STL_VECTOR( double ) )  m_CoordValues;	   // coordinates of each grid level
		CC_STL_VECTOR( double ) 				  m_TotalCoordValues;  // coordinates of union of all the layers
};

Gs1DLayers::Gs1DLayers( const Gs1DLayers& rhs )
:
	m_nLevels( rhs.m_nLevels ),
	m_Size( rhs.m_Size ),
	m_LevelSizes( rhs.m_LevelSizes ),
	m_CoordValues( rhs.m_CoordValues ),
	m_TotalCoordValues( rhs.m_TotalCoordValues )
{}

CC_END_NAMESPACE

#endif

