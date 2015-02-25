/****************************************************************
**
**	BSPricerFourierExp.h	- BSPricerFourier functions expoted to slang 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_BSPRICERFOURIEREXP_H )
#define IN_BSPRICERFOURIEREXP_H

#include <qgrid/base.h>
#include <qgrid/BSPricerFourier.h>
#include <gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID BSPricerFourier QGridBSPricerFourier(
	double			Spot, 					// spot                   
	const GsVector&	Discounts,          	// discount factors       
	const GsVector& DiscountTimes,      	// discount times         
	const GsVector& ForwardGrowth,      	// forward growth factors 
	const GsVector& ForwardTimes,       	// forward growth times   
	const GsVector& Volatilities,       	// implied volatilities   
	const GsVector& VolatilityTimes,     	// volatility times
	const GsVector&	GridAnchoringVols,		// grid anchoring vols
	const GsVector&	GridAnchoringVolTimes	// times of the grid anchoring vols
);

EXPORT_CPP_QGRID BSPricerFourier QGridBSPricerFourier_FromBSPricer(
	const BSPricer&	model			// the BSPricerFourier
);

EXPORT_CPP_QGRID int QGridBSPricerFourier_GridSize(
	const BSPricerFourier&	model		// the BSPricerFourier
);

EXPORT_CPP_QGRID BSPricerFourier QGridBSPricerFourier_GridSizeSet(
	const BSPricerFourier&	model,		// the BSPricerFourier
	int						GridSize	// the grid size
);

EXPORT_CPP_QGRID int QGridBSPricerFourier_FourierSize(
	const BSPricerFourier&	model		// the BSPricerFourier
);

EXPORT_CPP_QGRID BSPricerFourier QGridBSPricerFourier_FourierSizeSet(
	const BSPricerFourier&	model,		// the BSPricerFourier
	int						FourierSize	// number of Fourier terms
);

EXPORT_CPP_QGRID int QGridBSPricerFourier_MultiplierSize(
	const BSPricerFourier&	model		// the BSPricerFourier
);

EXPORT_CPP_QGRID BSPricerFourier QGridBSPricerFourier_MultiplierSizeSet(
	const BSPricerFourier&	model,		// the BSPricerFourier
	int						Size		// number of multiplier Fourier terms
);

EXPORT_CPP_QGRID double QGridBSPricerFourier_Spot(
	const BSPricerFourier&	model	// the BSPricerFourier
);

EXPORT_CPP_QGRID BSPricerFourier QGridBSPricerFourier_TermSet(
	const BSPricerFourier&	model,	// the BSPricerFourier
	double					term	// the term
);

EXPORT_CPP_QGRID bool QGridBSPricerFourier_ResizeFlag(
	const BSPricerFourier&	model	// the BSPricerFourier
);

EXPORT_CPP_QGRID BSPricerFourier QGridBSPricerFourier_ResizeFlagSet(
	const BSPricerFourier&	model,	// the BSPricerFourier
	bool					flag	// true = do resizing
);

CC_END_NAMESPACE

#endif

