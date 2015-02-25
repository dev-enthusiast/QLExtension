/****************************************************************
**
**	qgrid/enums.h	- enums
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: enums.h,v $
**	Revision 1.6  2001/07/27 15:30:07  lowthg
**	BSPricer now has function return type as well as grid & price
**
**	Revision 1.5  2000/08/24 19:27:53  lowthg
**	added CN_SOLVER_TYPE
**	
**	Revision 1.4  2000/08/03 17:38:38  lowthg
**	More work on stochastic vol pricing class SVPricer and related classes
**	
**	Revision 1.3  2000/05/19 20:29:06  lowthg
**	enum added for discount, growth and vol curves
**	
**	Revision 1.2  2000/04/19 15:49:26  lowthg
**	corrected bug with forward rates in CreateCNCoefficientsFromTermVars
**	
**	Revision 1.1  2000/01/19 18:14:06  lowthg
**	Improvements to DVPricer class
**	
**
****************************************************************/

#if !defined( IN_QGRID_ENUMS_H )
#define IN_QGRID_ENUMS_H

CC_BEGIN_NAMESPACE( Gs )

enum QGRID_FDMETHOD
{
	QGRID_IMPLICIT,
	QGRID_EXPLICIT,
	QGRID_CRANK_NICHOLSON
};

enum DVPRICER_IN_DATA_TYPE
{
	DVPRICER_CALLS,
	DVPRICER_TERMVARS,
	DVPRICER_TERMVOLS
};

enum QGRID_CURVE_TYPE
{
	QGRID_DISCOUNT_CURVE,
	QGRID_GROWTH_CURVE,
	QGRID_VOLATILITY_CURVE
};

enum SV_MODEL_TYPE
{
	HESTON
};

enum CN_SOLVER_TYPE
{
	CN_SOLVER_CG,
	CN_SOLVER_CGS
};

enum PRICER_RETURN_TYPE
{
	PRICER_PRICE,
	PRICER_GRID,
	PRICER_FUNCTION
};

CC_END_NAMESPACE

#endif

