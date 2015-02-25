/****************************************************************
**
**	GSACCRUALRATEANNUITY.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsAccrualRateAnnuity.h,v 1.2 2000/03/13 14:55:17 thompcl Exp $
**
****************************************************************/

#if !defined( IN_FIINSTR_GSACCRUALRATEANNUITY_H )
#define IN_FIINSTR_GSACCRUALRATEANNUITY_H

#include <fiinstr/base.h>
#include <fiinstr/GsRateAnnuity.h>
#include <gscore/GsPTimeCalc.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsAccrualRateAnnuity 
** Description : GsAccrualRateAnnuity class
****************************************************************/

class EXPORT_CLASS_FIINSTR GsAccrualRateAnnuity : public GsRateAnnuity
{
	GsRDate
			m_AccrualDates;

	GsPTimeCalc
			m_AccrualTermCalc;

	void checkSizes();

public:
	GsAccrualRateAnnuity();
	GsAccrualRateAnnuity( const GsRDate& AnnuityDates, double Notional, const GsAnnuityRate& AnnuityRate,
						  const GsRDate& AccrualDates, const GsPTimeCalc& AccrualTermCalc, bool CalcVales = true );

	GsAccrualRateAnnuity( const GsAccrualRateAnnuity & );

	~GsAccrualRateAnnuity();

	GsAccrualRateAnnuity &operator=( const GsAccrualRateAnnuity &rhs );

	virtual double term ( size_t Index ) const;
	virtual double rate ( size_t Index ) const;

	typedef GsDtGeneric< GsAccrualRateAnnuity > GsDtType;

	static const char* typeName() { return "GsAccrualRateAnnuity"; }
	GsString toString() const { return "GsAccrualRateAnnuity"; }
		
};

EXPORT_CPP_FIINSTR GsAccrualRateAnnuity
		*GsAccrualRateAnnuity1( const GsRDate& AnnuityDates, double Notional, const GsAnnuityRate& Rate,
								const GsRDate& AccuaralDates, const GsPTimeCalc& PTime );

EXPORT_CPP_FIINSTR double
		GsAccrualRateAnnuityPayment( const GsAccrualRateAnnuity& Annuity, int Index );

CC_END_NAMESPACE

#endif 
