/****************************************************************
**
**	LTSWAPMONEYMARKETINFO.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltaddin/src/ltaddin/LtSwapMoneyMarketInfo.h,v 1.9 2001/11/27 22:50:50 procmon Exp $
**
****************************************************************/

#if !defined( IN_LTADDIN_LTSWAPMONEYMARKETINFO_H )
#define IN_LTADDIN_LTSWAPMONEYMARKETINFO_H

#include <ltaddin/base.h>
#include <gscore/GsVector.h>
#include <gsdt/GsDtArray.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsDiscountCurve.h>
#include <gsquant/Enums.h>
#include <gscore/GsVectorVector.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_LTADDIN LtSwapMoneyMarketInfo
{
public:
	typedef GsDtGeneric<LtSwapMoneyMarketInfo> GsDtType;

	LtSwapMoneyMarketInfo();
	LtSwapMoneyMarketInfo( const GsDateVector& fixingDates, 
						   const GsDateVector& endDates, 
						   const GsVector& dfcs,
						   double numCashflows);

	LtSwapMoneyMarketInfo( const LtSwapMoneyMarketInfo& other );
	LtSwapMoneyMarketInfo& operator=( const LtSwapMoneyMarketInfo& other );
	~LtSwapMoneyMarketInfo();

	
	inline static const char* typeName() { return "LtSwapMoneyMarketInfo"; }
	GsString toString() const { return "LtSwapMoneyMarketInfo"; }

	virtual LtSwapMoneyMarketInfo* copy() const { return new LtSwapMoneyMarketInfo( *this ); }

	const GsDateVector& 	fixingDates() const { return m_fixingDates; }
	const GsDateVector& 	endDates()    const { return m_endDates; 	}
	const GsVector&     	dcfs()        const { return m_dcfs; 		}
	const GsVector&     	forwards()    const { return m_forwards; 	}
	      GsVector&     	forwards()     		{ return m_forwards; 	}


	
	void setFixingDates( const GsDateVector& 	dvec ) { m_fixingDates = dvec; }
	void setEndDates   ( const GsDateVector& 	dvec ) { m_endDates = dvec; 	}
	void setDcfs       ( const GsVector&     	dvec ) { m_dcfs = dvec; 		}
	void setForwards   ( const GsVector&     	dvec ) { m_forwards = dvec; 	}


	void compoundPeriodApplicable(
		const GsUnsignedVector 	& mask,				
		const GsVector  		& amortization,		
		const GsVectorVector 	& dcfs,				
		const GsVector  		& spreads, 	   		
		const GsVector  		& floating,			
		LT_IRM            		interestMethod,		
		double			  		multSpread ) const;


	double floatingPrice(
		const GsUnsignedVector 	  	& 	paymentMask,	
		const GsUnsignedVector 		& 	fixingMask,		
		const GsVector  			& 	amortization,	
		const GsVectorVector 		& 	dcfs,			
		const GsVector  			& 	spreads, 	   	
		LT_IRM            				interestMethod,	
		const GsDiscountCurve		& 	df,				
		double			  				multSpread,
		const GsVector				&   exchangeAmounts,
		const GsDateVector			&   allPaymentDates
	) const;

private:
	GsDateVector m_fixingDates;
	GsDateVector m_endDates;
	GsVector     m_dcfs;
	
	//precomputed memory for forward rates
	mutable GsVector m_forwards;
	mutable GsVector m_cashflows;
};


EXPORT_CPP_LTADDIN GsDateVector LtSwapMoneyMarketInfoFixingDates( const LtSwapMoneyMarketInfo& );
EXPORT_CPP_LTADDIN GsDateVector LtSwapMoneyMarketInfoEndDates   ( const LtSwapMoneyMarketInfo& );
EXPORT_CPP_LTADDIN GsVector     LtSwapMoneyMarketInfoDcfs       ( const LtSwapMoneyMarketInfo& );
EXPORT_CPP_LTADDIN GsVector     LtSwapMoneyMarketInfoForwards   ( const LtSwapMoneyMarketInfo& );
EXPORT_CPP_LTADDIN LtSwapMoneyMarketInfo* LtSwapMoneyMarketInfoCreate( const GsDateVector&,
																	   const GsDateVector&,
																	   const GsVector&,
																	   double);
EXPORT_CPP_LTADDIN GsVectorVector LtGsDtArrayToGsVectorVector(	const GsDtArray& array );



EXPORT_CPP_LTADDIN double
		LtSwapFloatingPriceFit(
			const GsUnsignedVector 	  	& 	paymentMask,	
			const GsUnsignedVector 		& 	fixingMask,		
			const GsVector  			& 	amortization,	
			const GsVectorVector 		& 	dcfs,			
			const GsVector  			& 	spreads, 	   	
			LT_IRM            				interestMethod,	
			const LtSwapMoneyMarketInfo	&	mminfo,  		
			const GsDiscountCurve		& 	df,				
			double			  				multSpread,
			const GsVector				&	exchangeamounts,
			const GsDateVector			&	allPaymentDates

);

EXPORT_CPP_LTADDIN double LtSwapFixedPriceFit(
	const GsDiscountCurve	&df,				// the discount curve	
	const GsDateVector		&allPaymentDates,	// the payment dates
	const GsVector			&interestPayments,	// the interest payments
	const GsVector			&exchangePayments	// the exchange payments
);


CC_END_NAMESPACE

#endif 
