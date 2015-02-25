/****************************************************************
**
**	FQBLACKCALCS.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqBlackCalcs.h,v 1.25 2011/08/11 03:06:01 khodod Exp $
**
****************************************************************/

#if !defined( IN_FQBLACKCALCS_H )
#define IN_FQBLACKCALCS_H

#include	<portable.h>
#include	<gscore/GsTermCalc.h>
#include	<gscore/GsIndexCurve.h>
#include	<gscore/GsDiscountCurve.h>
#include	<gscore/GsVolatilityCurve.h>
#include    <gsquant/GsFuncDateToContinuous.h>
#include	<fiinstr/FiDateStrip.h>

#include	<fiblack/FqBlack.h>

CC_BEGIN_NAMESPACE( Gs )

/////////////////////////////////////////////////
// basic conventions-free, date free calculators
/////////////////////////////////////////////////

EXPORT_CPP_FIBLACK
double FqBsCalcValue(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&
	);

EXPORT_CPP_FIBLACK
double FqBsCalcWingDelta(
	Q_OPT_TYPE opttype, 
	double delta, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&
	);

EXPORT_CPP_FIBLACK
double FqBsCalcDelta(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcGamma(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
);

EXPORT_CPP_FIBLACK
double FqBsCalcTheta(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcVanna(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcVega(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcVolga(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcImpVol(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double pv,
	const FqBlack&
	);
	
EXPORT_CPP_FIBLACK
double FqBsCalcImpStrike(
	Q_OPT_TYPE opttype, 
	double term, 
	double fwd, 
	double sigma,
	double pv,
	const FqBlack&
	);
	
EXPORT_CPP_FIBLACK
double FqBsCalcBinaryValue(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&
	);

EXPORT_CPP_FIBLACK
double FqBsCalcBinaryWingDelta(
	Q_OPT_TYPE opttype, 
	double delta, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&
	);

EXPORT_CPP_FIBLACK
double FqBsCalcBinaryDelta(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcBinaryGamma(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcBinaryTheta(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);
	
EXPORT_CPP_FIBLACK
double FqBsCalcBinaryVanna(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);
	
EXPORT_CPP_FIBLACK
double FqBsCalcBinaryVega(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcBinaryVolga(
	Q_OPT_TYPE opttype, 
	double strike, 
	double term, 
	double fwd, 
	double sigma,
	const FqBlack&,
	double bump
	);

EXPORT_CPP_FIBLACK
double FqBsCalcFwdVar(
	double term,
	double fwd,
	double sigma,
	const FqBlack&
	);

EXPORT_CPP_FIBLACK
double FqBsCalcFwdCov(
	double term,
	double fwd1,
	double fwd2,
	double sigma1,
	double sigma2,
	double rho,
	const FqBlack&,
	const FqBlack&
	);

//////////////////
// capped indices
//////////////////

EXPORT_CPP_FIBLACK
GsFuncHandle<double, double> FqBsIndexCappedCreate(
	Q_OPT_TYPE opttype,
	double strike,
	GsFuncHandle<double, double> index,
	GsFuncHandle<double, double> vol,
	const FqBlack& dist
	);


EXPORT_CPP_FIBLACK
GsIndexCurve FqBsIndexCappedCreate(
	Q_OPT_TYPE opttype,
	double strike,
	const GsIndexCurve&,
	const GsTermCalc& voltermcalc,
	const GsVolatilityCurve&,
	const FqBlack& dist
	);

////////
// caps
////////

EXPORT_CPP_FIBLACK
double FqBsCalcCapPriceFromImpVol(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double strike,
	double vol,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCapImpVolFromPrice(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double strike,
	double price,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCapImpStrikeFromPrice(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	const GsVolatilityCurve& volcurve,
	double price,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCapPrice(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double strike,
	const GsVolatilityCurve& volcurve,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCustomCapImpVolFromPrice(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	const GsVector& notionals,
	const GsVector& strikes,
	double price,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCustomCapImpStrikeFromPrice(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	const GsVector& notionals,
	const GsVolatilityCurve& volcurve,
	double price,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCustomCapPrice(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	const GsVector& notionals,
	const GsVector& strikes,
	const GsVolatilityCurve& volcurve,
	const FqBlack& dist,			  
	const GsDiscountCurve& zerocurve  
	);


EXPORT_CPP_FIBLACK
double FqBsCalcCapDelta(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double strike,
	const GsVolatilityCurve& volcurve,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCapDeltaFromImpVol(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double strike,
	double vol,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCapTheta(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double strike,
	const GsVolatilityCurve& volcurve,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCustomCapTheta(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	const GsVector& notionals,
	const GsVector& strikes,
	const GsVolatilityCurve& volcurve,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCapThetaFromImpVol(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double strike,
	double vol,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCapWingDelta(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double delta,
	const GsVolatilityCurve& volcurve,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcCapWingDeltaFromImpVol(
	const FiDateStrip&,
	Q_OPT_TYPE opttype,
	const GsIndexCurve& index,
	const GsTermCalc& voltermcalc,
	double delta,
	double vol,
	const FqBlack& dist,
	const GsDiscountCurve& zerocurve
	);

EXPORT_CPP_FIBLACK
double FqBsCalcConvexityCorrection(
	const GsDate& lookupDate,			
	const FiDateStrip& indexAnnuity,	
	const FiDateStrip& paymentAnnuity,	
	double volIndex,					
	double volPayment,  				
	double correlation,					
	const FqBlack& indexDist,			
	const FqBlack& paymentDist,			
	const GsDiscountCurve& curve		
);

EXPORT_CPP_FIBLACK
GsIndexCurve FqBsIndexConvexityCorrectedCreate(
	const GsDateGen& indexCal,
	const GsTermCalc& indexTc,
	const GsPeriod& indexTenor,
	const GsPeriod& indexPeriod,
	const GsDateGen& paymentCal,
	const GsTermCalc& paymentTc,
	const GsPeriod& paymentTenor,
	const GsPeriod& paymentPeriod,
	const GsPeriod& resetDelay,
	const GsDateGen& resetCal,
	double correlation,
	const GsVolatilityCurve& indexVolCurve,
	const FqBlack& indexDistribution,
    const GsVolatilityCurve& paymentVolCurve,
	const FqBlack& paymentDistribution,
	const GsDiscountCurve& discountCurve
);

EXPORT_CPP_FIBLACK
double FqBsCalcEuroOptionPrice(
	const GsDate& expDate,
	const GsDate& delivery,			
	const GsPeriod& tenor,			
	const GsDateGen& dg,			
	const GsTermCalc& tc,			
	Q_OPT_TYPE opttype,			
	const GsTermCalc& voltermcalc,	
	double strike,					
	double vol,
	const FqBlack& dist,			
	const GsDiscountCurve& discountCurve
);

EXPORT_CPP_FIBLACK
double FqBsCalcEuroOptionImpVol(
	const GsDate& expDate,			
	const GsDate& delivery,			
	const GsPeriod& tenor,			
	const GsDateGen& dg,			
	const GsTermCalc& tc,			
	Q_OPT_TYPE opttype,				
	const GsTermCalc& voltermcalc,	
	double strike,					
	double price,  					
	const FqBlack& dist,			
	const GsDiscountCurve& discountCurve
);

EXPORT_CPP_FIBLACK
FqBlack* FqBsCalcBestFitDistribution(
	Q_OPT_TYPE opttype, 			
	const GsVector&	 strikes, 		
	double expiration, 				
	double fwd, 					
	const GsVector&	optionValues,	
	const GsStringVector& paramNames,
	const GsVector& lowerBounds,	
	const GsVector& upperBounds, 	
	double tolerance,				
	const FqBlack& dist				
);


EXPORT_CPP_FIBLACK
double FqBsCalcGammaGain(
   	 
   Q_OPT_TYPE opttype, 	// option type   
   double strike, 			// strike        
   double expiration, 		// expiration
   double horizon,			// horizon
   double fwd, 		        // forward       
   double mktVol,			// market vol
   double myVol,			// expected vol
   const FqBlack& dist		// distribution  

   );




EXPORT_CPP_FIBLACK
double FqBsCalcGammaGainBounds(
   	 
   Q_OPT_TYPE opttype, 	// option type   
   double strike, 			// strike        
   double expiration, 		// expiration      
   double horizon, 			// horizon     
   double mktVol,			// market vol
   double myVol,			// expected vol
   int    whichBound,		// upper bound (=1) or lower bound (=0)
   const FqBlack& dist		// distribution  

   );

EXPORT_CPP_FIBLACK
double FqBsCalcEqLognormVol(
	Q_OPT_TYPE opttype, 
	double strike, 
	double expiration, 
	double fwd, 
	double sigma,
	const FqBlack&
	);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcValueVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist  
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcDeltaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble& ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcGammaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcThetaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcVannaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,   
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcVegaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcVolgaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcBinaryValueVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble& ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist  
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcBinaryDeltaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcBinaryGammaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcBinaryThetaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcBinaryVannaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcBinaryVegaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

EXPORT_CPP_FIBLACK
GsVector FqBsCalcBinaryVolgaVector(
	Q_OPT_TYPE OptType,  
	const GsVector &Strikes, 	  
	const GsDateVector &Expirations,
	const GsFuncDateToDouble &ctime,
	const GsVector &Fwds, 		  
	const GsVector &Vols,		  
	const FqBlack &Dist,
	double bump
);

CC_END_NAMESPACE



#endif 
