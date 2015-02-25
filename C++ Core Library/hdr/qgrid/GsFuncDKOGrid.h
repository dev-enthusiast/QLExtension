/****************************************************************
**
**	GsFuncDKOGrid.h	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_GSFUNCDKOGRID_H )
#define IN_GSFUNCDKOGRID_H

#include <qgrid/base.h>
#include <gsquant/GsFuncSpecial.h>
#include <gsdt/GsDtDictionary.h>
#include <gsdt/GsDt.h>
#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_QGRID GsFuncDKOGrid :public GsFuncScFunc
{
public:
	typedef GsDtGeneric<GsFuncDKOGrid> GsDtType;
	inline static const char* typeName() { return "GsFuncDKOGrid"; }
	GsString toString() const;

	GsFuncDKOGrid( const GsVector& Values, double LogMinSpot,
				   double LogMaxSpot, double LowerKO,
                   double UpperKO, double LowerRebate,
                   double UpperRebate, double LowRange,
				   double HighRange );

	virtual ~GsFuncDKOGrid() {}

	GsDtDictionary* ToDictionary() const;

	double operator()( double arg ) const;
	const GsFuncDKOGrid& operator*=( double rhs )
	{ m_Values *= rhs; return *this; }
	const GsFuncDKOGrid& operator/=( double rhs )
	{ m_Values /= rhs; return *this; }
	const GsFuncDKOGrid& operator+=( double rhs )
	{ m_Values += rhs; return *this; }
	const GsFuncDKOGrid& operator-=( double rhs )
	{ m_Values -= rhs; return *this; }


private:
	GsVector 	m_Values;
	GsVector	m_Spots;
	double 		m_LowerKO;
	double 		m_UpperKO;
	double 		m_LowerRebate;
	double 		m_UpperRebate;
	double		m_LowRange;
	double		m_HighRange;
};

/****************************************************************
**	Adlib Name:	GsFuncDKOGridCreate
**	Summary:		
**	References:		
**	Description: 
**	Example:		
****************************************************************/

EXPORT_CPP_QGRID GsFuncDKOGrid* GsFuncDKOGridCreate(
	const GsVector&	Values,			// values on the grid
	double			MinSpot,		// min spot on the grid
	double			MaxSpot,		// max spot on the grid
	double			LowerKO,		// lower knockout ( <= 0 for none)
	double			UpperKO,		// upper knockout ( <= 0 for none)
	double			LowerRebate,	// rebate at lower KO
	double			UpperRebate		// rebate at upper KO
);


CC_END_NAMESPACE

#endif

